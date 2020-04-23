#include "drv_vdec.h"
#include "drv_vdec_sdk.h"
#include "drv_vdec_proc.h"
#include "drv_vdec_vfmw.h"
#include "vdec_priv_type.h"

typedef enum {
    VDEC_CHAN_STATE_NULL = 0,
    VDEC_CHAN_STATE_CREATE,
    VDEC_CHAN_STATE_START,
    VDEC_CHAN_STATE_STOP,
} drv_vdec_chan_state;

typedef enum {
    VDEC_WAIT_PARAM_PVR_INDEX = 0,
    VDEC_WAIT_PARAM_FRM_RATE_INDEX,
    VDEC_WAIT_PARAM_SPEED_INDEX,
    VDEC_WAIT_PARAM_START_PTS_INDEX,
    VDEC_WAIT_PARAM_MAX_INDEX
} drv_vdec_wait_param_index;

typedef struct {
    hi_u32 create_time;
    hi_u32 config_time;
    hi_u32 start_time;
    hi_u32 stop_time;
    hi_u32 reset_time;
    hi_u32 flush_time;
    hi_u32 destroy_time;
    hi_u32 reg_event_time;
} drv_vdec_kpi;

typedef struct {
    hi_u32 try_que_stm_num;
    hi_u32 que_stm_num;
    hi_u32 try_deq_stm_num;
    hi_u32 deq_stm_num;
    hi_u32 try_acq_frm_num;
    hi_u32 acq_frm_num;
    hi_u32 try_rel_frm_num;
    hi_u32 rel_frm_num;
} drv_vdec_statis;

typedef struct {
    hi_s32 vdec_id;
    hi_s32 vfmw_id;
    hi_handle ssm_handle;
    hi_handle vdec_handle;
    hi_handle user_handle;
    hi_u64 event_map;
    hi_vdec_attr attr;
    vdec_vfmw_param ext_attr;
    vdec_osal_sema sema;
    drv_vdec_chan_state status;
    drv_vdec_statis statis;
    fn_drv_vdec_callback callback;
    hi_void *filp;
    vdec_osal_event wait_event;
    hi_u32 pvr_buf_num;
    hi_u32 new_frm_evt;
} drv_vdec_chan;

typedef struct {
    hi_u32 chan_num;
    vdec_osal_sema sema;
    drv_vdec_chan chan[VDEC_MAX_CHAN_NUM];
    drv_vdec_kpi kpi_info[VDEC_MAX_CHAN_NUM];
    hi_u32 open_cnt;
} drv_vdec_entry;

typedef hi_s32 (*fun_set_param)(drv_vdec_chan *chan, const hi_u8 *param, hi_u32 param_size);
typedef hi_s32 (*fun_get_param)(drv_vdec_chan *chan, hi_u8 *param, hi_u32 param_size);

typedef struct {
    hi_vdec_param_id id;
    fun_set_param set_param;
    fun_get_param get_param;
} drv_vdec_param_item;

static drv_vdec_entry g_drv_vdec_entry = {0};

static inline drv_vdec_entry* drv_vdec_get_entry(hi_void)
{
    return &g_drv_vdec_entry;
}

static inline drv_vdec_chan* drv_vdec_get_chan(hi_u32 chan_id)
{
    drv_vdec_entry *entry = drv_vdec_get_entry();
    return &(entry->chan[chan_id]);
}

static hi_void drv_vdec_lock_entry(hi_void)
{
    drv_vdec_entry *entry = drv_vdec_get_entry();
    vdec_osal_sema_down(&entry->sema);
}

static hi_void drv_vdec_unlock_entry(hi_void)
{
    drv_vdec_entry *entry = drv_vdec_get_entry();
    vdec_osal_sema_up(&entry->sema);
}

static hi_void drv_vdec_lock_chan(hi_u32 chan_id)
{
    drv_vdec_chan *chan = drv_vdec_get_chan(chan_id);
    vdec_osal_sema_down(&chan->sema);
}

static hi_void drv_vdec_unlock_chan(hi_u32 chan_id)
{
    drv_vdec_chan *chan = drv_vdec_get_chan(chan_id);
    vdec_osal_sema_up(&chan->sema);
}

static hi_void drv_vdec_clear_chan(drv_vdec_chan *chan)
{
    chan->vfmw_id = HI_INVALID_HANDLE;
    chan->event_map = HI_VDEC_EVT_START_RESERVE;
    chan->callback = HI_NULL;
    (hi_void)memset_s(&chan->statis, sizeof(drv_vdec_statis), 0, sizeof(drv_vdec_statis));
    (hi_void)memset_s(&chan->ext_attr, sizeof(vdec_vfmw_param), 0, sizeof(vdec_vfmw_param));
    chan->ext_attr.frame_rate.type = HI_VDEC_FR_TYPE_INVALID;
    chan->ext_attr.is_pvr_info_valid = HI_FALSE;
    chan->ext_attr.start_pts.pts = VDEC_INVALID_PTS;
    chan->ext_attr.speed_info.direct = HI_VDEC_SPEED_INALID;
    chan->ext_attr.frm_3d_type = -1;
    chan->pvr_buf_num = 0;
}

static hi_s32 drv_vdec_event_call_back(hi_u32 chan_id, hi_u32 evt_id, hi_void *param, hi_u32 size)
{
    hi_s32 ret = HI_FAILURE;
    drv_vdec_chan *chan = NULL;

    VDEC_ASSERT_RET(chan_id < VDEC_MAX_CHAN_NUM);

    chan = drv_vdec_get_chan(chan_id);
    if (chan->callback != HI_NULL) {
        if ((evt_id & chan->event_map) != 0) {
            ret = chan->callback(chan->user_handle, evt_id, param, size);
        } else {
            VDEC_PRINT_DBG("chan %d event %d is not in set in map %x!\n", chan_id, evt_id, chan->event_map);
        }
    } else {
        VDEC_PRINT_DBG("chan %d event callback func is null!\n", chan_id);
    }

    if (evt_id == HI_VDEC_EVT_NEW_FRAME) {
        chan->new_frm_evt++;
        vdec_osal_event_give(&chan->wait_event);
    }

    return ret;
}

static hi_s32 drv_vdec_set_uvmos(drv_vdec_chan *chan, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret = HI_SUCCESS;

    chan->ext_attr.enable_uvmos = *((hi_bool *)param);

    if (chan->vfmw_id != HI_INVALID_HANDLE) {
        ret = vdec_vfmw_set_param(chan->vfmw_id, HI_VDEC_PARAM_UVMOS, param, param_size);
    }

    return ret;
}

static hi_s32 drv_vdec_set_low_delay(drv_vdec_chan *chan, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret = HI_SUCCESS;

    chan->ext_attr.enable_low_delay = *((hi_bool*)param);

    if (chan->vfmw_id != HI_INVALID_HANDLE) {
        ret = vdec_vfmw_set_param(chan->vfmw_id, HI_VDEC_PARAM_LOW_DELAY, param, param_size);
    }

    return ret;
}

static hi_s32 drv_vdec_set_dpb_mode(drv_vdec_chan *chan, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret = HI_SUCCESS;

    chan->ext_attr.dpb_mode = *((hi_vdec_dpb_mode*)param);

    if (chan->vfmw_id != HI_INVALID_HANDLE) {
        ret = vdec_vfmw_set_param(chan->vfmw_id, HI_VDEC_PARAM_DPB_MODE, param, param_size);
    }

    return ret;
}

static hi_s32 drv_vdec_set_hdr_attr(drv_vdec_chan *chan, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;

    ret = memcpy_s(&chan->ext_attr.hdr_attr, sizeof(hi_vdec_hdr_attr), param, sizeof(hi_vdec_hdr_attr));
    ret = (ret == EOK) ? HI_SUCCESS : HI_FAILURE;

    if (ret == HI_SUCCESS && chan->vfmw_id != HI_INVALID_HANDLE) {
        ret = vdec_vfmw_set_param(chan->vfmw_id, HI_VDEC_PARAM_HDR_ATTR, param, param_size);
    }

    return ret;
}

static hi_s32 drv_vdec_set_stm_mode(drv_vdec_chan *chan, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret = HI_SUCCESS;

    chan->ext_attr.stm_mode = *((hi_vdec_stm_buf_mode*)param);

    if (chan->vfmw_id != HI_INVALID_HANDLE) {
        ret = vdec_vfmw_set_param(chan->vfmw_id, HI_VDEC_PARAM_STM_BUF_MODE, param, param_size);
    }

    return ret;
}

static hi_s32 drv_vdec_set_frm_mode(drv_vdec_chan *chan, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret = HI_SUCCESS;

    chan->ext_attr.frm_mode = *((hi_vdec_frm_buf_mode*)param);

    if (chan->vfmw_id != HI_INVALID_HANDLE) {
        ret = vdec_vfmw_set_param(chan->vfmw_id, HI_VDEC_PARAM_FRM_BUF_MODE, param, param_size);
    }

    return ret;
}

static hi_s32 drv_vdec_set_out_fmt(drv_vdec_chan *chan, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;

    ret = memcpy_s(&chan->ext_attr.out_format, sizeof(hi_vdec_out_format), param, sizeof(hi_vdec_out_format));
    ret = (ret == EOK) ? HI_SUCCESS : HI_FAILURE;

    if (ret == HI_SUCCESS && chan->vfmw_id != HI_INVALID_HANDLE) {
        ret = vdec_vfmw_set_param(chan->vfmw_id, HI_VDEC_PARAM_OUTPUT_FORMAT, param, param_size);
    }

    return ret;
}

static hi_s32 drv_vdec_set_src_tag(drv_vdec_chan *chan, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;

    ret = memcpy_s(&chan->ext_attr.source_tag, sizeof(hi_vdec_src_tag), param, sizeof(hi_vdec_src_tag));
    ret = (ret == EOK) ? HI_SUCCESS : HI_FAILURE;

    if (ret == HI_SUCCESS && chan->vfmw_id != HI_INVALID_HANDLE) {
        ret = vdec_vfmw_set_param(chan->vfmw_id, HI_VDEC_PARAM_SOURCE_TAG, param, param_size);
    }

    return ret;
}

static hi_s32 drv_vdec_set_poc_thrshld(drv_vdec_chan *chan, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret = HI_SUCCESS;

    chan->ext_attr.poc_thrshold = *((hi_u32*)param);

    if (chan->vfmw_id != HI_INVALID_HANDLE) {
        ret = vdec_vfmw_set_param(chan->vfmw_id, HI_VDEC_PARAM_POC_THRSHOLD, param, param_size);
    }

    return ret;
}

static hi_s32 drv_vdec_set_pvr_info(drv_vdec_chan *chan, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;

    ret = memcpy_s(&chan->ext_attr.pvr_info, sizeof(hi_vdec_pvr_info), param, param_size);
    ret = (ret == EOK) ? HI_SUCCESS : HI_FAILURE;

    if (ret == HI_SUCCESS && chan->vfmw_id != HI_INVALID_HANDLE) {
        ret = vdec_vfmw_set_param(chan->vfmw_id, HI_VDEC_PARAM_PVR_INFO, param, param_size);
    }

    return ret;
}

static hi_s32 drv_vdec_set_speed_info(drv_vdec_chan *chan, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;

    ret = memcpy_s(&chan->ext_attr.speed_info, sizeof(hi_vdec_speed), param, param_size);
    ret = (ret == EOK) ? HI_SUCCESS : HI_FAILURE;

    if (ret == HI_SUCCESS && chan->vfmw_id != HI_INVALID_HANDLE) {
        if (chan->ext_attr.speed_info.direct == HI_VDEC_SPEED_BACKWARD) {
            chan->attr.ext_fs_num += chan->pvr_buf_num;
            ret = vdec_vfmw_set_attr(chan->vfmw_id, &chan->attr, &chan->ext_attr);
        }

        ret = vdec_vfmw_set_param(chan->vfmw_id, HI_VDEC_PARAM_FF_FB_SPEED, param, param_size);
    }

    return ret;
}

static hi_s32 drv_vdec_set_frm_rate(drv_vdec_chan *chan, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;

    ret = memcpy_s(&chan->ext_attr.frame_rate, sizeof(hi_vdec_frm_rate), param, param_size);
    ret = (ret == EOK) ? HI_SUCCESS : HI_FAILURE;

    if (ret == HI_SUCCESS && chan->vfmw_id != HI_INVALID_HANDLE) {
        ret = vdec_vfmw_set_param(chan->vfmw_id, HI_VDEC_PARAM_FRAME_RATE, param, param_size);
    }

    return ret;
}

static hi_s32 drv_vdec_set_start_pts(drv_vdec_chan *chan, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret = HI_SUCCESS;
    hi_vdec_start_pts *start_pts = (hi_vdec_start_pts *)param;

    chan->ext_attr.start_pts.pts = start_pts->pts;

    if (chan->vfmw_id != HI_INVALID_HANDLE) {
        ret = vdec_vfmw_set_param(chan->vfmw_id, HI_VDEC_PARAM_START_PTS, param, param_size);
    }

    return ret;
}

static hi_s32 drv_vdec_set_dec_mode(drv_vdec_chan *chan, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret = HI_SUCCESS;

    chan->ext_attr.dec_mode = *((hi_vdec_dec_mode*)param);

    if (chan->vfmw_id != HI_INVALID_HANDLE) {
        ret = vdec_vfmw_set_param(chan->vfmw_id, HI_VDEC_PARAM_DEC_MODE, param, param_size);
    }

    return ret;
}

static hi_s32 drv_vdec_set_frm_3d_type(drv_vdec_chan *chan, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret = HI_SUCCESS;

    chan->ext_attr.frm_3d_type = *((hi_u32*)param);

    if (chan->vfmw_id != HI_INVALID_HANDLE) {
        ret = vdec_vfmw_set_param(chan->vfmw_id, HI_VDEC_PARAM_FRM_3D_TYPE, param, param_size);
    }

    return ret;
}

static hi_s32 drv_vdec_get_frm_3d_type(drv_vdec_chan *chan, hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret = HI_SUCCESS;

    if (chan->vfmw_id != HI_INVALID_HANDLE) {
        ret = vdec_vfmw_get_param(chan->vfmw_id, HI_VDEC_PARAM_FRM_3D_TYPE, param, param_size);
    } else {
        *(hi_u32*)param = chan->ext_attr.frm_3d_type;
    }

    return ret;
}

static hi_s32 drv_vdec_get_out_fmt(drv_vdec_chan *chan, hi_u8 *param, hi_u32 param_size)
{
    *((hi_vdec_out_format*)param) = chan->ext_attr.out_format;

    return HI_SUCCESS;
}

static hi_s32 drv_vdec_get_frm_rate(drv_vdec_chan *chan, hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;
    if (chan->vfmw_id != HI_INVALID_HANDLE) {
        ret = vdec_vfmw_get_param(chan->vfmw_id, HI_VDEC_PARAM_FRAME_RATE, param, param_size);
    } else {
        ret = memcpy_s(param, param_size, &chan->ext_attr.frame_rate, sizeof(hi_vdec_frm_rate));
        ret = (ret == EOK) ? HI_SUCCESS : HI_FAILURE;
    }

    return ret;
}

static hi_s32 drv_vdec_get_feature(drv_vdec_chan *chan, hi_u8 *param, hi_u32 param_size)
{
    VDEC_ASSERT_RET(chan->vfmw_id != HI_INVALID_HANDLE);

    return vdec_vfmw_get_param(chan->vfmw_id, HI_VDEC_PARAM_FEATURE, param, param_size);
}

static hi_s32 drv_vdec_get_status(drv_vdec_chan *chan, hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;

    if (chan->vfmw_id == HI_INVALID_HANDLE) {
        memset_s(param, param_size, 0, param_size);
        return HI_SUCCESS;
    }

    ret = vdec_vfmw_get_param(chan->vfmw_id, HI_VDEC_PARAM_STATUS, param, param_size);
    if (ret == HI_SUCCESS) {
        chan->pvr_buf_num = ((hi_vdec_status*)param)->frm_state.pvr_buf_num;
    }

    return ret;
}

static drv_vdec_param_item g_param_item[] = {
    { HI_VDEC_PARAM_UVMOS,         drv_vdec_set_uvmos,       HI_NULL },
    { HI_VDEC_PARAM_LOW_DELAY,     drv_vdec_set_low_delay,   HI_NULL },
    { HI_VDEC_PARAM_DPB_MODE,      drv_vdec_set_dpb_mode,    HI_NULL },
    { HI_VDEC_PARAM_HDR_ATTR,      drv_vdec_set_hdr_attr,    HI_NULL },
    { HI_VDEC_PARAM_STM_BUF_MODE,  drv_vdec_set_stm_mode,    HI_NULL },
    { HI_VDEC_PARAM_FRM_BUF_MODE,  drv_vdec_set_frm_mode,    HI_NULL },
    { HI_VDEC_PARAM_OUTPUT_FORMAT, drv_vdec_set_out_fmt,     drv_vdec_get_out_fmt },
    { HI_VDEC_PARAM_SOURCE_TAG,    drv_vdec_set_src_tag,     HI_NULL},
    { HI_VDEC_PARAM_POC_THRSHOLD,  drv_vdec_set_poc_thrshld, HI_NULL },
    { HI_VDEC_PARAM_PVR_INFO,      drv_vdec_set_pvr_info,    HI_NULL },
    { HI_VDEC_PARAM_FF_FB_SPEED,   drv_vdec_set_speed_info,  HI_NULL },
    { HI_VDEC_PARAM_FRAME_RATE,    drv_vdec_set_frm_rate,    drv_vdec_get_frm_rate },
    { HI_VDEC_PARAM_START_PTS,     drv_vdec_set_start_pts,   HI_NULL },
    { HI_VDEC_PARAM_DEC_MODE,      drv_vdec_set_dec_mode,    HI_NULL },
    { HI_VDEC_PARAM_FEATURE,       HI_NULL,                  drv_vdec_get_feature },
    { HI_VDEC_PARAM_STATUS,        HI_NULL,                  drv_vdec_get_status },
    { HI_VDEC_PARAM_FRM_3D_TYPE,   drv_vdec_set_frm_3d_type, drv_vdec_get_frm_3d_type },
};

static hi_void drv_vdec_set_wait_param(drv_vdec_chan *chan)
{
    hi_s32 ret;

    if (chan->ext_attr.is_pvr_info_valid) {
        ret = vdec_vfmw_set_param(chan->vfmw_id, HI_VDEC_PARAM_PVR_INFO, (hi_u8*)&chan->ext_attr.pvr_info,
            sizeof(hi_vdec_pvr_info));
        if (ret != HI_SUCCESS) {
            VDEC_PRINT_ERR("set pvr info fail!\n");
        }
    }

    if (chan->ext_attr.frame_rate.type != HI_VDEC_FR_TYPE_INVALID) {
        ret = vdec_vfmw_set_param(chan->vfmw_id, HI_VDEC_PARAM_FRAME_RATE, (hi_u8*)&chan->ext_attr.frame_rate,
            sizeof(hi_vdec_frm_rate));
        if (ret != HI_SUCCESS) {
            VDEC_PRINT_ERR("set frame rate info fail!\n");
        }
    }

    if (chan->ext_attr.speed_info.direct != HI_VDEC_SPEED_INALID) {
        ret = vdec_vfmw_set_param(chan->vfmw_id, HI_VDEC_PARAM_FF_FB_SPEED, (hi_u8*)&chan->ext_attr.speed_info,
            sizeof(hi_vdec_speed));
        if (ret != HI_SUCCESS) {
            VDEC_PRINT_ERR("set speed info fail!\n");
        }
    }

    if (chan->ext_attr.start_pts.pts != VDEC_INVALID_PTS) {
        ret = vdec_vfmw_set_param(chan->vfmw_id, HI_VDEC_PARAM_START_PTS, (hi_u8*)&chan->ext_attr.start_pts,
            sizeof(hi_vdec_start_pts));
        if (ret != HI_SUCCESS) {
            VDEC_PRINT_ERR("set start pts fail!\n");
        }
    }

    if (chan->ext_attr.frm_3d_type != -1) {
        ret = vdec_vfmw_set_param(chan->vfmw_id, HI_VDEC_PARAM_FRM_3D_TYPE, (hi_u8*)&chan->ext_attr.frm_3d_type,
            sizeof(hi_u32));
        if (ret != HI_SUCCESS) {
            VDEC_PRINT_ERR("set 3d type fail!\n");
        }
    }
}

static hi_s32 drv_vdec_create_vfmw_chan(drv_vdec_chan *chan)
{
    hi_s32 ret;
    vdec_vfmw_create_param param = {0};

    ret = vdec_vfmw_init();
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("init vfmw error!\n");
        return HI_FAILURE;
    }

    param.vdec_handle = chan->vdec_handle;
    param.vdec_id = chan->vdec_id;
    param.ssm_handle =  chan->ssm_handle;
    param.attr = &(chan->attr);
    param.callback = drv_vdec_event_call_back;
    ret = vdec_vfmw_create(&chan->vfmw_id, &param);
    if (ret != HI_SUCCESS) {
        (hi_void)vdec_vfmw_exit();
        VDEC_PRINT_ERR("vdec %d create vfmw failed!\n", chan->vdec_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_vdec_check_command(drv_vdec_chan *chan, hi_vdec_cmd_id cmd)
{
    if (chan->status == VDEC_CHAN_STATE_NULL || chan->vfmw_id == HI_INVALID_HANDLE) {
        VDEC_PRINT_ERR("vdec %d cmd 0x%x status(%d) or vfmw_id(%d) invalid!\n",
            chan->vdec_id, cmd, chan->status, chan->vfmw_id);
        return HI_FAILURE;
    }

    if (cmd == HI_VDEC_CMD_INIT_INPUT_BUF && chan->ext_attr.stm_mode == HI_VDEC_STM_EXTERNAL) {
        VDEC_PRINT_ERR("chan %d stm_mode %d cmd %d is invalid\n", chan->vdec_id, chan->ext_attr.stm_mode, cmd);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void drv_vdec_init(hi_void)
{
    hi_u32 i;
    drv_vdec_chan *chan = HI_NULL;
    drv_vdec_entry *entry = drv_vdec_get_entry();

    vdec_osal_sema_init(&entry->sema);
    for (i = 0; i < VDEC_MAX_CHAN_NUM; i++) {
        chan = drv_vdec_get_chan(i);
        (hi_void)memset_s(chan, sizeof(drv_vdec_chan), 0, sizeof(drv_vdec_chan));
        chan->vdec_id = i;
        chan->vfmw_id = HI_INVALID_HANDLE;
        vdec_osal_sema_init(&chan->sema);
    }
}

hi_void drv_vdec_deinit(hi_void)
{
    hi_u32 i;
    drv_vdec_chan *chan = HI_NULL;
    drv_vdec_entry *entry = drv_vdec_get_entry();

    vdec_osal_sema_exit(&entry->sema);

    for (i = 0; i < VDEC_MAX_CHAN_NUM; i++) {
        chan = drv_vdec_get_chan(i);
        vdec_osal_sema_exit(&chan->sema);
        (hi_void)memset_s(chan, sizeof(drv_vdec_chan), 0, sizeof(drv_vdec_chan));
    }
}

hi_s32 drv_vdec_open(hi_void)
{
    hi_s32 ret;
    drv_vdec_entry *entry = drv_vdec_get_entry();

    drv_vdec_lock_entry();

    if (entry->open_cnt == 0) {
        ret = vdec_vfmw_open();
        if (ret != HI_SUCCESS) {
            drv_vdec_unlock_entry();
            VDEC_PRINT_ERR("open vfmw fail!\n");
            return HI_FAILURE;
        }
    }

    entry->open_cnt++;

    drv_vdec_unlock_entry();

    return HI_SUCCESS;
}

hi_s32 drv_vdec_close(hi_void)
{
    hi_s32 ret;
    drv_vdec_entry *entry = drv_vdec_get_entry();

    drv_vdec_lock_entry();

    if (entry->open_cnt == 1) {
        if (entry->chan_num > 0) {
            drv_vdec_unlock_entry();
            VDEC_PRINT_ERR("%d chans do not destroy!\n", entry->chan_num);
            return HI_FAILURE;
        }

        ret = vdec_vfmw_close();
        if (ret != HI_SUCCESS) {
            drv_vdec_unlock_entry();
            VDEC_PRINT_ERR("close vfmw fail!\n");
            return HI_FAILURE;
        }
    }

    if (entry->open_cnt > 0) {
        entry->open_cnt--;
    }

    drv_vdec_unlock_entry();

    return HI_SUCCESS;
}

hi_s32 drv_vdec_create(hi_u32 *chan_id, const hi_vdec_opt_create *option, hi_void *filp, hi_u32 size)
{
    hi_s32 i;
    drv_vdec_chan *chan = NULL;
    drv_vdec_entry *entry = drv_vdec_get_entry();
    hi_u32 begin;
    hi_u32 end;

    VDEC_ASSERT_RET(chan_id != HI_NULL);
    VDEC_ASSERT_RET(option != HI_NULL);

    begin = vdec_osal_get_time();

    drv_vdec_lock_entry();

    for (i = 0; i < VDEC_MAX_CHAN_NUM; i++) {
        drv_vdec_lock_chan(i);
        chan = drv_vdec_get_chan(i);
        if (chan->status == VDEC_CHAN_STATE_NULL) {
            break;
        }
        drv_vdec_unlock_chan(i);
    }

    if (i >= VDEC_MAX_CHAN_NUM) {
        drv_vdec_unlock_entry();
        VDEC_PRINT_ERR("no free vdec chan!\n");
        return HI_FAILURE;
    }

    drv_vdec_clear_chan(chan);

    chan->status = VDEC_CHAN_STATE_CREATE;

    chan->filp = filp;
    chan->ssm_handle = option->ssm_handle;
    chan->vdec_id = i;
    vdec_osal_event_init(&chan->wait_event);
    entry->chan_num++;
    *chan_id = i;

    drv_vdec_unlock_chan(i);
    drv_vdec_unlock_entry();

    end = vdec_osal_get_time();
    entry->kpi_info[i].create_time = end - begin;

    return HI_SUCCESS;
}

hi_s32 drv_vdec_destroy(hi_u32 chan_id, const hi_vdec_opt_destroy *option)
{
    hi_s32 ret;
    drv_vdec_chan *chan = NULL;
    drv_vdec_entry *entry = drv_vdec_get_entry();
    hi_u32 begin;
    hi_u32 end;

    VDEC_ASSERT_RET(chan_id < VDEC_MAX_CHAN_NUM);
    VDEC_ASSERT_RET(option != HI_NULL);

    begin = vdec_osal_get_time();

    drv_vdec_lock_entry();
    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    if (chan->status == VDEC_CHAN_STATE_NULL || chan->status == VDEC_CHAN_STATE_START) {
        drv_vdec_unlock_chan(chan_id);
        drv_vdec_unlock_entry();
        VDEC_PRINT_ERR("vdec %d state %d(state_null) error!\n", chan_id, chan->status);
        return HI_FAILURE;
    }

    vdec_osal_event_exit(&chan->wait_event);

    if (chan->vfmw_id != HI_INVALID_HANDLE) {
        if (chan->ext_attr.stm_mode == HI_VDEC_STM_INTERNAL) {
            ret = vdec_vfmw_command(chan->vfmw_id, HI_VDEC_CMD_DEINIT_INPUT_BUF, HI_NULL, 0);
            if (ret != HI_SUCCESS) {
                VDEC_PRINT_ERR("chan %d deinit input buf fail!\n", chan_id);
                return HI_FAILURE;
            }
        }

        ret = vdec_vfmw_destroy(chan->vfmw_id, HI_NULL);
        if (ret != HI_SUCCESS) {
            drv_vdec_unlock_chan(chan_id);
            drv_vdec_lock_entry();
            VDEC_PRINT_ERR("vdec %d destroy vfmw %d failed!\n", chan_id, chan->vfmw_id);
            return HI_FAILURE;
        }

        ret = vdec_vfmw_exit();
        if (ret != HI_SUCCESS) {
            drv_vdec_unlock_chan(chan_id);
            drv_vdec_lock_entry();
            VDEC_PRINT_ERR("vdec exit vfmw fail!\n");
            return HI_FAILURE;
        }
    }

    drv_vdec_clear_chan(chan);

    chan->status = VDEC_CHAN_STATE_NULL;
    chan->filp = HI_NULL;

    entry->chan_num--;

    drv_vdec_unlock_chan(chan_id);
    drv_vdec_unlock_entry();

    end = vdec_osal_get_time();
    entry->kpi_info[chan_id].destroy_time = end - begin;

    return HI_SUCCESS;
}

hi_s32 drv_vdec_start(hi_u32 chan_id, const hi_vdec_opt_start *option)
{
    hi_s32 ret;
    drv_vdec_chan *chan = NULL;
    drv_vdec_entry *entry = drv_vdec_get_entry();
    hi_u32 begin;
    hi_u32 end;

    VDEC_ASSERT_RET(chan_id < VDEC_MAX_CHAN_NUM);
    VDEC_ASSERT_RET(option != HI_NULL);

    begin = vdec_osal_get_time();

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    if (chan->status == VDEC_CHAN_STATE_NULL || chan->vfmw_id == HI_INVALID_HANDLE) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_ERR("vdec %d status(%d) or vfmw_id(%d) invalid!\n", chan_id, chan->status, chan->vfmw_id);
        return HI_FAILURE;
    }

    if (chan->status == VDEC_CHAN_STATE_START) {
        drv_vdec_unlock_chan(chan_id);
        return HI_SUCCESS;
    }

    ret = vdec_vfmw_start(chan->vfmw_id, option);
    if (ret != HI_SUCCESS) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_ERR("vdec %d start vfmw %d failed!\n", chan_id, chan->vfmw_id);
        return HI_FAILURE;
    }

    chan->status = VDEC_CHAN_STATE_START;

    drv_vdec_unlock_chan(chan_id);

    end = vdec_osal_get_time();
    entry->kpi_info[chan_id].start_time = end - begin;

    return HI_SUCCESS;
}

hi_s32 drv_vdec_stop(hi_u32 chan_id, const hi_vdec_opt_stop *option)
{
    hi_s32 ret;
    drv_vdec_chan *chan = NULL;
    drv_vdec_entry *entry = drv_vdec_get_entry();
    hi_u32 begin;
    hi_u32 end;

    VDEC_ASSERT_RET(chan_id < VDEC_MAX_CHAN_NUM);
    VDEC_ASSERT_RET(option != HI_NULL);

    begin = vdec_osal_get_time();

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    if (chan->status == VDEC_CHAN_STATE_NULL || chan->vfmw_id == HI_INVALID_HANDLE) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_ERR("vdec %d status(%d) or vfmw_id(%d) invalid!\n", chan_id, chan->status, chan->vfmw_id);
        return HI_FAILURE;
    }

    if (chan->status == VDEC_CHAN_STATE_STOP) {
        drv_vdec_unlock_chan(chan_id);
        return HI_SUCCESS;
    }

    ret = vdec_vfmw_stop(chan->vfmw_id, option);
    if (ret != HI_SUCCESS) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_ERR("vdec %d stop vfmw %d failed!\n", chan_id, chan->vfmw_id);
        return HI_FAILURE;
    }

    chan->status = VDEC_CHAN_STATE_STOP;

    drv_vdec_unlock_chan(chan_id);
    end = vdec_osal_get_time();
    entry->kpi_info[chan_id].stop_time = end - begin;

    return HI_SUCCESS;
}

hi_s32 drv_vdec_reset(hi_u32 chan_id, const hi_vdec_opt_reset *option)
{
    hi_s32 ret = HI_FAILURE;
    drv_vdec_chan *chan = NULL;
    drv_vdec_entry *entry = drv_vdec_get_entry();
    hi_u32 begin;
    hi_u32 end;

    VDEC_ASSERT_RET(chan_id < VDEC_MAX_CHAN_NUM);
    VDEC_ASSERT_RET(option != HI_NULL);

    begin = vdec_osal_get_time();

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    if (chan->status != VDEC_CHAN_STATE_STOP || chan->vfmw_id == HI_INVALID_HANDLE) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_ERR("vdec %d status(%d) or vfmw_id(%d) invalid!\n", chan_id, chan->status, chan->vfmw_id);
        return HI_FAILURE;
    }

    if (chan->ext_attr.stm_mode == HI_VDEC_STM_INTERNAL) {
        ret = vdec_vfmw_command(chan->vfmw_id, HI_VDEC_CMD_DEINIT_INPUT_BUF, HI_NULL, 0);
        if (ret != HI_SUCCESS) {
            VDEC_PRINT_ERR("chan %d deinit input buf fail!\n", chan_id);
            return HI_FAILURE;
        }
    }

    ret = vdec_vfmw_destroy(chan->vfmw_id, HI_NULL);
    if (ret != HI_SUCCESS) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_ERR("vdec %d destroy vfmw %d failed!\n", chan_id, chan->vfmw_id);
        return HI_FAILURE;
    }

    ret = vdec_vfmw_exit();
    if (ret != HI_SUCCESS) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_ERR("vdec exit vfmw fail!\n");
        return HI_FAILURE;
    }

    drv_vdec_clear_chan(chan);

    drv_vdec_unlock_chan(chan_id);

    end = vdec_osal_get_time();
    entry->kpi_info[chan_id].reset_time = end - begin;

    return HI_SUCCESS;
}

hi_s32 drv_vdec_flush(hi_u32 chan_id, const hi_vdec_opt_flush *option)
{
    hi_s32 ret;
    drv_vdec_chan *chan = NULL;
    drv_vdec_entry *entry = drv_vdec_get_entry();
    hi_u32 begin;
    hi_u32 end;

    VDEC_ASSERT_RET(chan_id < VDEC_MAX_CHAN_NUM);
    VDEC_ASSERT_RET(option != HI_NULL);

    begin = vdec_osal_get_time();

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    if (chan->status != VDEC_CHAN_STATE_STOP || chan->vfmw_id == HI_INVALID_HANDLE) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_ERR("vdec %d status(%d) or vfmw_id(%d) invalid!\n", chan_id, chan->status, chan->vfmw_id);
        return HI_FAILURE;
    }

    ret = vdec_vfmw_flush(chan->vfmw_id, option);
    if (ret != HI_SUCCESS) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_ERR("vdec %d flush vfmw %d failed!\n", chan_id, chan->vfmw_id);
        return HI_FAILURE;
    }

    drv_vdec_unlock_chan(chan_id);

    end = vdec_osal_get_time();
    entry->kpi_info[chan_id].flush_time = end - begin;

    return HI_SUCCESS;
}

hi_s32 drv_vdec_decode(hi_u32 chan_id, drv_vdec_decode_param *param)
{
    hi_s32 ret;
    hi_vdec_opt_init_input input_param = {0};
    hi_vdec_stream stream_info = {0};
    hi_vdec_opt_deq_stm deq_opt = {0};
    hi_u8 *stream_buf = HI_NULL;
    drv_vdec_chan *chan = HI_NULL;
    hi_bool eos = HI_TRUE;

    VDEC_ASSERT_RET(chan_id < VDEC_MAX_CHAN_NUM);

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    if (chan->vfmw_id == HI_INVALID_HANDLE) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_ERR("vfmw not create!\n");
        return HI_FAILURE;
    }

    input_param.size = param->stream->buf_size;
    ret = vdec_vfmw_command(chan->vfmw_id, HI_VDEC_CMD_INIT_INPUT_BUF, (hi_u8*)(&input_param), sizeof(input_param));
    if (ret != HI_SUCCESS) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_ERR("vfmw init input buf fail!\n");
        return HI_FAILURE;
    }

    ret = vdec_vfmw_start(chan->vfmw_id, HI_NULL);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vfmw start fail!\n");
        goto exit;
    }

    deq_opt.expect_size = param->stream->buf_size;
    deq_opt.is_kern = HI_TRUE;
    ret = vdec_vfmw_dequeue_stream(chan->vfmw_id, &deq_opt, &stream_info);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vfmw dequeue stream fail!\n");
        goto exit;
    }

    /* get kern_vir addr stream_buf through fd */
    stream_buf = VDEC_UINT64_PTR(stream_info.buf_vir.integer);
    if (param->is_user_space == HI_TRUE) {
        ret = COPY_FROM_USER(stream_buf, VDEC_UINT64_PTR(param->stream->buf_vir.integer), param->stream->buf_size);
        if (ret != 0) {
            VDEC_PRINT_ERR("vfmw copy_from_user fail!\n");
            goto exit;
        }
    } else {
        ret = memcpy_s(stream_buf, param->stream->buf_size, VDEC_UINT64_PTR(param->stream->buf_vir.integer),
                       param->stream->buf_size);
        if (ret != EOK) {
            VDEC_PRINT_ERR("vfmw memcpy_s fail!\n");
            goto exit;
        }
    }

    ret = vdec_vfmw_queue_stream(chan->vfmw_id, HI_NULL, &stream_info);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vfmw queue stream fail!\n");
        goto exit;
    }

    ret = vdec_vfmw_command(chan->vfmw_id, HI_VDEC_CMD_SEND_EOS, (hi_u8*)&eos, sizeof(hi_bool));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vfmw send eos fail!\n");
        goto exit;
    }

    ret = vdec_vfmw_acquire_frame(chan->vfmw_id, HI_NULL, param->frame, param->frame_size, param->ext_frm_info);
    if (ret != HI_SUCCESS) {
        vdec_osal_event_wait(&chan->wait_event, param->time_out);
        ret = vdec_vfmw_acquire_frame(chan->vfmw_id, HI_NULL, param->frame, param->frame_size, param->ext_frm_info);
    }

exit:
    (hi_void)vdec_vfmw_stop(chan->vfmw_id, HI_NULL);
    (hi_void)vdec_vfmw_command(chan->vfmw_id, HI_VDEC_CMD_DEINIT_INPUT_BUF, HI_NULL, 0);
    drv_vdec_unlock_chan(chan_id);

    return ret;
}

hi_s32 drv_vdec_get_attr(hi_u32 chan_id, hi_vdec_attr *attr)
{
    hi_s32 ret;
    drv_vdec_chan *chan = NULL;

    VDEC_ASSERT_RET(chan_id < VDEC_MAX_CHAN_NUM);
    VDEC_ASSERT_RET(attr != HI_NULL);

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    if (chan->status == VDEC_CHAN_STATE_NULL) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_ERR("vdec %d state %d(state_null) error!\n", chan_id, chan->status);
        return HI_FAILURE;
    }

    ret = memcpy_s(attr, sizeof(hi_vdec_attr), &chan->attr, sizeof(hi_vdec_attr));
    if (ret != EOK) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_ERR("get attr memcpy failed!\n");
        return HI_FAILURE;
    }

    drv_vdec_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 drv_vdec_set_attr(hi_u32 chan_id, hi_handle vdec_handle, const hi_vdec_attr *attr)
{
    hi_s32 ret;
    drv_vdec_chan *chan = NULL;
    drv_vdec_entry *entry = drv_vdec_get_entry();
    hi_u32 begin;
    hi_u32 end;

    VDEC_ASSERT_RET(chan_id < VDEC_MAX_CHAN_NUM);
    VDEC_ASSERT_RET(attr != HI_NULL);

    begin = vdec_osal_get_time();

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    if (chan->status == VDEC_CHAN_STATE_NULL || chan->status == VDEC_CHAN_STATE_START) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_ERR("chan %d status %d error!\n", chan_id, chan->status);
        return HI_FAILURE;
    }

    chan->vdec_handle = vdec_handle;
    ret = memcpy_s(&chan->attr, sizeof(hi_vdec_attr), attr, sizeof(hi_vdec_attr));
    if (ret != EOK) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_ERR("set attr memcpy failed!\n");
        return HI_FAILURE;
    }

    if (chan->vfmw_id == HI_INVALID_HANDLE) {
        ret = drv_vdec_create_vfmw_chan(chan);
        if (ret != HI_SUCCESS) {
            drv_vdec_unlock_chan(chan_id);
            VDEC_PRINT_ERR("create vfmw chan failed!\n");
            return HI_FAILURE;
        }
    }

    ret = vdec_vfmw_set_attr(chan->vfmw_id, &chan->attr, &chan->ext_attr);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("chan %d set attr fail!\n", chan->vfmw_id);
        drv_vdec_unlock_chan(chan_id);
        return HI_FAILURE;
    }

    drv_vdec_set_wait_param(chan);

    drv_vdec_unlock_chan(chan_id);

    end = vdec_osal_get_time();
    entry->kpi_info[chan_id].config_time = end - begin;

    return HI_SUCCESS;
}

hi_s32 drv_vdec_register_event(hi_u32 chan_id, hi_u32 event_map)
{
    hi_s32 ret;
    drv_vdec_chan *chan = NULL;
    drv_vdec_entry *entry = drv_vdec_get_entry();
    hi_u32 begin;
    hi_u32 end;

    VDEC_ASSERT_RET(chan_id < VDEC_MAX_CHAN_NUM);

    begin = vdec_osal_get_time();

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    chan->event_map = event_map;

    ret = vdec_vfmw_set_event_map(chan->vfmw_id, event_map);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("chan %d set event map %x fail!\n", chan_id, event_map);
        drv_vdec_unlock_chan(chan_id);
        return HI_FAILURE;
    }

    drv_vdec_unlock_chan(chan_id);

    end = vdec_osal_get_time();
    entry->kpi_info[chan_id].reg_event_time = end - begin;

    return HI_SUCCESS;
}

hi_s32 drv_vdec_listen_event(hi_u32 chan_id, const hi_vdec_opt_listen *option, hi_vdec_event *event)
{
    return HI_SUCCESS;
}

hi_s32 drv_vdec_queue_stream(hi_u32 chan_id, const hi_vdec_opt_que_stm *option, const hi_vdec_stream *stream)
{
    hi_s32 ret;
    drv_vdec_chan *chan = NULL;

    VDEC_ASSERT_RET(chan_id < VDEC_MAX_CHAN_NUM);
    VDEC_ASSERT_RET(option != HI_NULL);
    VDEC_ASSERT_RET(stream != HI_NULL);

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    chan->statis.try_que_stm_num++;

    if (chan->status == VDEC_CHAN_STATE_NULL || chan->vfmw_id == HI_INVALID_HANDLE) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_ERR("vdec %d status(%d) or vfmw_id(%d) invalid!\n", chan_id, chan->status, chan->vfmw_id);
        return HI_FAILURE;
    }

    ret = vdec_vfmw_queue_stream(chan->vfmw_id, option, stream);
    if (ret != HI_SUCCESS) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_DBG("vdec %d queue vfmw %d stream failed!\n", chan_id, chan->vfmw_id);
        return HI_FAILURE;
    }

    if (chan->statis.que_stm_num <= 1 && chan->statis.acq_frm_num <= 1) {
        ret = vdec_vfmw_wakeup(chan->vfmw_id, HI_NULL, 0);
        if (ret != HI_SUCCESS) {
            VDEC_PRINT_DBG("vdec %d wakeup vfmw %d failed!\n", chan_id, chan->vfmw_id);
        }
    }

    chan->statis.que_stm_num++;

    drv_vdec_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 drv_vdec_dequeue_stream(hi_u32 chan_id, const hi_vdec_opt_deq_stm *option, hi_vdec_stream *stream)
{
    hi_s32 ret;
    drv_vdec_chan *chan = NULL;

    VDEC_ASSERT_RET(chan_id < VDEC_MAX_CHAN_NUM);
    VDEC_ASSERT_RET(option != HI_NULL);
    VDEC_ASSERT_RET(stream != HI_NULL);

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    chan->statis.try_deq_stm_num++;

    if (chan->status == VDEC_CHAN_STATE_NULL || chan->vfmw_id == HI_INVALID_HANDLE) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_ERR("vdec %d status(%d) or vfmw_id(%d) invalid!\n", chan_id, chan->status, chan->vfmw_id);
        return HI_FAILURE;
    }

    ret = vdec_vfmw_dequeue_stream(chan->vfmw_id, option, stream);
    if (ret != HI_SUCCESS) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_DBG("vdec %d dequeue vfmw %d stream failed!\n", chan_id, chan->vfmw_id);
        return HI_FAILURE;
    }

    chan->statis.deq_stm_num++;

    drv_vdec_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 drv_vdec_acquire_frame(hi_u32 chan_id, const hi_vdec_opt_acq_frm *option, hi_void *frame, hi_u32 size,
    hi_vdec_ext_frm_info *ext_frm_info)
{
    hi_s32 ret;
    drv_vdec_chan *chan = NULL;

    VDEC_ASSERT_RET(option != HI_NULL);
    VDEC_ASSERT_RET(frame != HI_NULL);
    VDEC_ASSERT_RET(chan_id < VDEC_MAX_CHAN_NUM);

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    chan->statis.try_acq_frm_num++;

    if (chan->status == VDEC_CHAN_STATE_NULL || chan->vfmw_id == HI_INVALID_HANDLE) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_ERR("vdec %d status(%d) or vfmw_id(%d) invalid!\n", chan_id, chan->status, chan->vfmw_id);
        return HI_FAILURE;
    }

    ret = vdec_vfmw_acquire_frame(chan->vfmw_id, option, frame, size, ext_frm_info);
    if (ret != HI_SUCCESS) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_DBG("vdec %d acquire vfmw %d frame failed!\n", chan_id, chan->vfmw_id);
        return HI_FAILURE;
    }

    chan->statis.acq_frm_num++;

    drv_vdec_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 drv_vdec_release_frame(hi_u32 chan_id, const hi_vdec_opt_rel_frm *option, const hi_void *frame, hi_u32 size)
{
    hi_s32 ret;
    drv_vdec_chan *chan = NULL;

    VDEC_ASSERT_RET(option != HI_NULL);
    VDEC_ASSERT_RET(frame != HI_NULL);
    VDEC_ASSERT_RET(chan_id < VDEC_MAX_CHAN_NUM);

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    chan->statis.try_rel_frm_num++;

    if (chan->status == VDEC_CHAN_STATE_NULL || chan->vfmw_id == HI_INVALID_HANDLE) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_ERR("vdec %d status(%d) or vfmw_id(%d) invalid!\n", chan_id, chan->status, chan->vfmw_id);
        return HI_FAILURE;
    }

    ret = vdec_vfmw_release_frame(chan->vfmw_id, option, frame, size);
    if (ret != HI_SUCCESS) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_ERR("vdec %d release vfmw %d frame failed!\n", chan_id, chan->vfmw_id);
        return HI_FAILURE;
    }

    chan->statis.rel_frm_num++;

    drv_vdec_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 drv_vdec_get_param(hi_u32 chan_id, hi_vdec_param_id param_id, hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;
    drv_vdec_chan *chan = NULL;
    drv_vdec_param_item *item = HI_NULL;
    fun_get_param get_param = HI_NULL;
    hi_s32 i = 0;

    VDEC_ASSERT_RET(chan_id < VDEC_MAX_CHAN_NUM);
    VDEC_ASSERT_RET(param != HI_NULL);

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    if (chan->status == VDEC_CHAN_STATE_NULL) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_ERR("vdec %d status(%d) invalid!\n", chan_id, chan->status);
        return HI_FAILURE;
    }

    for (i = 0; i < (sizeof(g_param_item) / sizeof(drv_vdec_param_item)); i++) {
        item = &g_param_item[i];
        if (param_id == item->id) {
            get_param = g_param_item[i].get_param;
            break;
        }
    }

    if (get_param == HI_NULL) {
        VDEC_PRINT_ERR("get param %d not support!\n", param_id);
        drv_vdec_unlock_chan(chan_id);
        return HI_FAILURE;
    }

    ret = get_param(chan, param, param_size);

    drv_vdec_unlock_chan(chan_id);

    return ret;
}

hi_s32 drv_vdec_set_param(hi_u32 chan_id, hi_vdec_param_id param_id, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;
    drv_vdec_chan *chan = HI_NULL;
    drv_vdec_param_item *item = HI_NULL;
    fun_set_param set_param = HI_NULL;
    hi_s32 i = 0;

    VDEC_ASSERT_RET(chan_id < VDEC_MAX_CHAN_NUM);
    VDEC_ASSERT_RET(param != HI_NULL);

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    if (chan->status == VDEC_CHAN_STATE_NULL) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_ERR("vdec %d status(%d)invalid!\n", chan_id, chan->status);
        return HI_FAILURE;
    }

    for (i = 0; i < (sizeof(g_param_item) / sizeof(drv_vdec_param_item)); i++) {
        item = &g_param_item[i];
        if (param_id == item->id) {
            set_param = g_param_item[i].set_param;
            break;
        }
    }

    if (set_param == HI_NULL) {
        VDEC_PRINT_ERR("set param %d not support!\n", param_id);
        drv_vdec_unlock_chan(chan_id);
        return HI_FAILURE;
    }

    ret = set_param(chan, param, param_size);

    drv_vdec_unlock_chan(chan_id);

    return ret;
}

hi_s32 drv_vdec_command(hi_u32 chan_id, hi_vdec_cmd_id cmd_id, hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;
    drv_vdec_chan *chan = NULL;
    hi_u32 vfmw_id = HI_INVALID_HANDLE;
    hi_bool is_chn_valid;

    VDEC_ASSERT_RET(param != HI_NULL);
    is_chn_valid = (chan_id < VDEC_MAX_CHAN_NUM) ? HI_TRUE : HI_FALSE;

    if (is_chn_valid) {
        drv_vdec_lock_chan(chan_id);

        chan = drv_vdec_get_chan(chan_id);
        if (drv_vdec_check_command(chan, cmd_id) != HI_SUCCESS) {
            drv_vdec_unlock_chan(chan_id);
            VDEC_PRINT_ERR("chan %d check comamnd %d fail!\n", chan_id, cmd_id);
            return HI_FAILURE;
        }

        vfmw_id = chan->vfmw_id;
    }

    ret = vdec_vfmw_command(vfmw_id, cmd_id, param, param_size);

    if (is_chn_valid) {
        drv_vdec_unlock_chan(chan_id);
    }

    return ret;
}

hi_s32 drv_vdec_register_callback(hi_u32 chan_id, hi_handle user_handle, fn_drv_vdec_callback func)
{
    drv_vdec_chan *chan = NULL;

    VDEC_ASSERT_RET(func != HI_NULL);
    VDEC_ASSERT_RET(chan_id < VDEC_MAX_CHAN_NUM);

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    if (chan->status == VDEC_CHAN_STATE_NULL) {
        drv_vdec_unlock_chan(chan_id);
        VDEC_PRINT_ERR("vdec %d status(%d) invalid!\n", chan_id, chan->status);
        return HI_FAILURE;
    }

    chan->callback = func;
    chan->user_handle = user_handle;

    drv_vdec_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 drv_vdec_suspend(hi_void)
{
    hi_s32 ret;

    drv_vdec_lock_entry();

    ret = vdec_vfmw_suspend();

    drv_vdec_unlock_entry();

    return ret;
}

hi_s32 drv_vdec_resume(hi_void)
{
    hi_s32 ret;

    drv_vdec_lock_entry();

    ret = vdec_vfmw_resume();

    drv_vdec_unlock_entry();

    return ret;
}

hi_void drv_vdec_release(hi_void *filp, hi_u32 size)
{
    hi_s32 i = 0;
    drv_vdec_chan *chan = NULL;
    drv_vdec_entry *entry = drv_vdec_get_entry();

    drv_vdec_lock_entry();

    for (i = 0; i < VDEC_MAX_CHAN_NUM; i++) {
        drv_vdec_lock_chan(i);
        chan = drv_vdec_get_chan(i);
        if (chan->filp != filp || chan->status == VDEC_CHAN_STATE_NULL) {
            drv_vdec_unlock_chan(i);
            continue;
        }

        if (chan->vfmw_id != HI_INVALID_HANDLE) {
            (hi_void)vdec_vfmw_stop(chan->vfmw_id, HI_NULL);
            (hi_void)vdec_vfmw_destroy(chan->vfmw_id, HI_NULL);
            (hi_void)vdec_vfmw_exit();
        }

        drv_vdec_clear_chan(chan);
        chan->status = VDEC_CHAN_STATE_NULL;
        chan->filp = HI_NULL;
        drv_vdec_unlock_chan(i);
        entry->chan_num--;
    }

    drv_vdec_unlock_entry();

    return;
}

hi_u8* drv_vdec_get_standard(hi_vdec_std standard)
{
    switch (standard) {
        case HI_VDEC_STD_MPEG1:
            return "mpeg1";
        case HI_VDEC_STD_MPEG2:
            return "mpeg2";
        case HI_VDEC_STD_MPEG4:
            return "mpeg4";
        case HI_VDEC_STD_H261:
            return "h261";
        case HI_VDEC_STD_H262:
            return "h262";
        case HI_VDEC_STD_H263:
            return "h263";
        case HI_VDEC_STD_SORENSON:
            return "sorenson";
        case HI_VDEC_STD_H264:
            return "h264";
        case HI_VDEC_STD_H264_MVC:
            return "mvc";
        case HI_VDEC_STD_H265:
            return "h265";
        case HI_VDEC_STD_VP3:
            return "vp3";
        case HI_VDEC_STD_VP5:
            return "vp5";
        case HI_VDEC_STD_VP6:
            return "vp6";
        case HI_VDEC_STD_VP6F:
            return "vp6f";
        case HI_VDEC_STD_VP6A:
            return "vp6a";
        case HI_VDEC_STD_VP8:
            return "vp8";
        case HI_VDEC_STD_VP9:
            return "vp9";
        case HI_VDEC_STD_AVS:
            return "avs";
        case HI_VDEC_STD_AVS2:
            return "avs2";
        case HI_VDEC_STD_AVS3:
            return "avs3";
        case HI_VDEC_STD_VC1:
            return "vc1";
        case HI_VDEC_STD_AV1:
            return "av1";
        case HI_VDEC_STD_MJPEG:
            return "mjpeg";
        case HI_VDEC_STD_MJPEGB:
            return "mjpegb";
        case HI_VDEC_STD_REAL8:
            return "real8";
        case HI_VDEC_STD_REAL9:
            return "real9";
        case HI_VDEC_STD_DIVX3:
            return "divx3";
        case HI_VDEC_STD_DIVX4:
            return "divx4";
        case HI_VDEC_STD_DIVX5:
            return "divx5";
        case HI_VDEC_STD_WMV1:
            return "wmv1";
        case HI_VDEC_STD_WMV2:
            return "wmv2";
        case HI_VDEC_STD_WMV3:
            return "wmv3";
        case HI_VDEC_STD_MSMPEG4V1:
            return "msmpeg4v1";
        case HI_VDEC_STD_MSMPEG4V2:
            return "msmpeg4v2";
        case HI_VDEC_STD_MSVIDEO1:
            return "msvideo1";
        case HI_VDEC_STD_INDEO2:
            return "indeo2";
        case HI_VDEC_STD_INDEO3:
            return "indeo3";
        case HI_VDEC_STD_INDEO4:
            return "indeo4";
        case HI_VDEC_STD_INDEO5:
            return "indeo5";
        case HI_VDEC_STD_CINEPAK:
            return "cinepak";
        case HI_VDEC_STD_SVQ1:
            return "svq1";
        case HI_VDEC_STD_SVQ3:
            return "svq3";
        case HI_VDEC_STD_DV:
            return "dv";
        case HI_VDEC_STD_RV10:
            return "rv10";
        case HI_VDEC_STD_RV20:
            return "rv20";
        case HI_VDEC_STD_RAW:
            return "raw";
        default:
            return "unknown";
    }
}

hi_s32 drv_vdec_read_proc(hi_void *pf, hi_u32 chan_id)
{
    drv_vdec_entry *entry = drv_vdec_get_entry();
    drv_vdec_chan *chan = HI_NULL;
    drv_vdec_kpi *kpi_info = HI_NULL;

    hi_u8 *chan_state[] = {"null", "create", "start", "stop"};
    hi_u8 *dec_mode[] = {"normal", "ip", "i", "drop_invld_b", "first_i"};
    hi_u8 *evt_mode[] = {"active", "passive"};
    hi_u8 *out_order[] = {"display", "dec"};
    hi_u8 *frm_type[] = {"drv", "unf"};
    hi_u8 *cmp_mode[] = {"adjust", "off", "on"};
    hi_u8 *out_fmt[] = {"420sp", "420sp_fix_8bit", "420sp_expand_16bit", "420sp_cmp", "420sp_afbc", "420sp_tile"};
    hi_u8 *dpb_mode[] = {"normal", "simple", "skyplay"};

    VDEC_ASSERT_RET(pf != HI_NULL);
    VDEC_ASSERT_RET(chan_id < VDEC_MAX_CHAN_NUM);

    PROC_PRINT(pf, VP_HEAD, "DRV");
    PROC_PRINT(pf, VP_S_D, "version", VDEC_VERSION, "chan_num", entry->chan_num);
    PROC_PRINT(pf, "\n");

    chan = drv_vdec_get_chan(chan_id);
    if (chan->status != VDEC_CHAN_STATE_NULL) {
        PROC_PRINT(pf, VP_HEAD, "VDEC");
        PROC_PRINT(pf, VP_D_D, "chan_id", chan_id, "vfmw_id", chan->vfmw_id);
        PROC_PRINT(pf, VP_X_X, "usr_handle", chan->user_handle, "event_map", (hi_u32)chan->event_map);
        PROC_PRINT(pf, VP_S_S, "status", chan_state[chan->status],
            "standard", drv_vdec_get_standard(chan->attr.standard));
        PROC_PRINT(pf, VP_D_D, "is_user_dec", chan->attr.is_user_dec, "priority", chan->attr.priority);
        PROC_PRINT(pf, VP_S_S, "dec_mode", dec_mode[chan->attr.dec_mode], "evt_mode", evt_mode[chan->attr.evt_mode]);
        PROC_PRINT(pf, VP_S_D, "out_order", out_order[chan->attr.out_order], "is_tvp", chan->attr.is_tvp);
        PROC_PRINT(pf, VP_D_D, "is_tunnel", chan->attr.is_tunnel, "keep_cfg", chan->attr.enable_keep_codec_config);
        PROC_PRINT(pf, VP_D_D, "err_cover", chan->attr.error_cover, "usd_pool_size", chan->attr.usd_pool_size);
        PROC_PRINT(pf, VP_D_S, "ext_fs", chan->attr.ext_fs_num, "frm_type", frm_type[chan->attr.frm_type]);
        PROC_PRINT(pf, VP_S_D, "cmp_mode", cmp_mode[chan->attr.cmp_mode], "new_frm_evt", chan->new_frm_evt);
        PROC_PRINT(pf, VP_S_D, "out_format", out_fmt[chan->ext_attr.out_format.yuv_fmt],
            "poc_thrshold", chan->ext_attr.poc_thrshold);
        PROC_PRINT(pf, VP_D_S, "u_first", chan->ext_attr.out_format.is_u_first,
            "dpb_mode", dpb_mode[chan->ext_attr.dpb_mode]);
        PROC_PRINT(pf, VP_D_D, "lowdelay", chan->ext_attr.enable_low_delay, "uvmos", chan->ext_attr.enable_uvmos);
        PROC_PRINT(pf, VP_D_D, "try_que_stm", chan->statis.try_que_stm_num, "que_stm_ok", chan->statis.que_stm_num);
        PROC_PRINT(pf, VP_D_D, "try_deq_stm", chan->statis.try_deq_stm_num, "deq_stm_ok", chan->statis.deq_stm_num);
        PROC_PRINT(pf, VP_D_D, "try_acq_frm", chan->statis.try_acq_frm_num, "acq_frm_ok", chan->statis.acq_frm_num);
        PROC_PRINT(pf, VP_D_D, "try_rls_frm", chan->statis.try_rel_frm_num, "rls_frm_ok", chan->statis.rel_frm_num);
        PROC_PRINT(pf, "\n");
    }

    if (chan->vfmw_id != HI_INVALID_HANDLE) {
        vdec_vfmw_read_proc(pf, chan->vfmw_id);
    }

    kpi_info = &entry->kpi_info[chan_id];
    PROC_PRINT(pf, VP_HEAD, "KPI");
    PROC_PRINT(pf, VP_D_D, "create", kpi_info->create_time, "config", kpi_info->config_time);
    PROC_PRINT(pf, VP_D_D, "start", kpi_info->start_time, "stop", kpi_info->stop_time);
    PROC_PRINT(pf, VP_D_D, "reset", kpi_info->reset_time, "flush", kpi_info->flush_time);
    PROC_PRINT(pf, VP_D_D, "reg_event", kpi_info->reg_event_time, "destroy", kpi_info->destroy_time);

    return HI_SUCCESS;
}
