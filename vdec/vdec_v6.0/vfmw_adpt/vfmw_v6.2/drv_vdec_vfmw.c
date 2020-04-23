#include "drv_vdec_vfmw.h"
#include "vdec_priv_type.h"
#include "drv_vdec_sdk.h"
#include "drv_vdec_proc.h"
#include "drv_vdec_convert.h"

typedef struct {
    hi_s32 vdec_chan_id;
    vfmw_chan_option option;
    hi_vdec_out_order out_order;
    hi_bool is_sec;
    hi_bool is_used;
    hi_bool is_eos;
    fn_vdec_vfmw_callback call_back;
} vdec_vfmw_chan;

typedef struct {
    vfmw_export_fun *vfmw_func;
    vdec_vfmw_chan chan[VDEC_MAX_CHAN_NUM];
    vdec_osal_atomic init_cnt;
    vfmw_cap cap;
} vdec_vfmw_entry;

typedef hi_s32 (*fun_set_param)(hi_u32 chan_id, const hi_u8* param, hi_u32 param_size);
typedef hi_s32 (*fun_get_param)(hi_u32 chan_id, hi_u8 *param, hi_u32 param_size);
typedef hi_s32 (*fun_command)(hi_u32 chan_id, hi_u8 *param, hi_u32 param_size);

typedef struct {
    hi_vdec_param_id id;
    fun_set_param set_param;
    fun_get_param get_param;
} vdec_vfmw_param_item;

typedef struct {
    hi_vdec_cmd_id id;
    fun_command cmd_fun;
} vdec_vfmw_cmd_item;

vdec_vfmw_entry g_vfmw_entry = {
    .vfmw_func = HI_NULL,
};

static vdec_vfmw_entry* vdec_vfmw_get_entry(hi_void)
{
    return &g_vfmw_entry;
}

static vdec_vfmw_chan* vdec_vfmw_get_chan(hi_u32 vdec_vfmw_id)
{
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    return &entry->chan[vdec_vfmw_id];
}

static hi_void vdec_vfmw_init_chan_param(vfmw_chan_option *option, const vdec_vfmw_create_param *param)
{
    option->adpt_type = ADPT_TYPE_VDEC;
    option->is_user_dec = param->attr->is_user_dec;
    option->vdec_handle = param->vdec_handle;
    option->ssm_handle = param->ssm_handle;
    option->is_sec_mode = param->attr->is_tvp;
    option->vid_std = vdec_cnvt_std_2_vfmw(param->attr->standard);
}

static hi_s32 vdec_vfmw_event_call_back(hi_s32 chan_id, hi_s32 type, hi_void *args, hi_s32 len)
{
    hi_s32 ret;
    hi_vdec_event out = {0};
    vdec_vfmw_chan *chan = HI_NULL;

    ret = vdec_cnvt_event_2_vdec(chan_id, type, args, len, &out);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("chan %d convert event %d error!\n", chan_id, type);
        return HI_FAILURE;
    }

    chan = vdec_vfmw_get_chan(chan_id);
    if (chan->call_back != HI_NULL) {
        ret = chan->call_back(chan->vdec_chan_id, out.event_id, &out.event_param,
            sizeof(out.event_param));
        if (ret != HI_SUCCESS) {
            VDEC_PRINT_DBG("chan %d event %d call event callback func fail!\n", chan_id, out.event_id);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_is_std_support(hi_u32 chan_id, hi_u8 *param, hi_u32 size)
{
    hi_u32 i = 0;
    vfmw_vid_std standard;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    hi_vdec_opt_is_std_support *support = (hi_vdec_opt_is_std_support*)param;

    VDEC_ASSERT_RET(param != HI_NULL);
    VDEC_ASSERT_RET(size == sizeof(hi_vdec_opt_is_std_support));

    support->support = HI_FALSE;
    standard = vdec_cnvt_std_2_vfmw(support->standard);
    while (entry->cap.supported_std[i] != VFMW_STD_MAX) {
        if (standard == entry->cap.supported_std[i]) {
            support->support = HI_TRUE;
        }
        i++;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_is_random_stm_support(hi_u32 chan_id, hi_u8 *param, hi_u32 size)
{
    hi_vdec_check_random *random = (hi_vdec_check_random*)param;

    VDEC_ASSERT_RET(param != HI_NULL);
    VDEC_ASSERT_RET(size == sizeof(hi_vdec_check_random));

    if (random->standard == HI_VDEC_STD_H265 ||
        random->standard == HI_VDEC_STD_H264 ||
        random->standard == HI_VDEC_STD_MPEG2 ||
        random->standard == HI_VDEC_STD_MPEG4 ||
        random->standard == HI_VDEC_STD_AVS ||
        random->standard == HI_VDEC_STD_H264_MVC ||
        (random->standard == HI_VDEC_STD_VC1 && random->standard_ext.vc1.is_adv_profile == HI_TRUE)) {
        random->support = HI_TRUE;
    } else {
        random->support = HI_FALSE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_set_tvp(hi_u32 chan_id, hi_u8 *param, hi_u32 size)
{
    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_bind_ext_es_buf(hi_u32 chan_id, hi_u8 *param, hi_u32 size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_stream_buf buf = {0};
    hi_void *dmabuf = HI_NULL;
    vdec_vfmw_chan *chan = vdec_vfmw_get_chan(chan_id);
    hi_vdec_opt_bind_input *input = (hi_vdec_opt_bind_input*)param;

    VDEC_ASSERT_RET(param != HI_NULL);
    VDEC_ASSERT_RET(size == sizeof(hi_vdec_opt_bind_input));

    dmabuf = vdec_drv_get_dma_buf(input->handle);
    if (dmabuf == HI_NULL) {
        VDEC_PRINT_ERR("esbuf fd %llx get dma buf fail!\n", input->handle);
        return HI_FAILURE;
    }

    buf.dma_buf = (hi_u64)(hi_ulong)(dmabuf);
    buf.phy_addr = vdec_drv_get_phy(dmabuf, chan->is_sec);
    buf.length = input->length;

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_BIND_ES_BUF, &buf, sizeof(buf));

    vdec_drv_put_phy(dmabuf, buf.phy_addr, chan->is_sec);
    vdec_drv_put_dma_buf(dmabuf);

    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vfmw chan:%d bind ext es buffer fail!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_drop_stream(hi_u32 chan_id, hi_u8 *param, hi_u32 size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    hi_u64 data[3]; /* 3: array size */
    hi_vdec_drop_stm *stm = (hi_vdec_drop_stm*)param;

    VDEC_ASSERT_RET(param != HI_NULL);
    VDEC_ASSERT_RET(size == sizeof(hi_vdec_drop_stm));

    data[0] = stm->expect_pts;
    data[1] = stm->gap;

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_SET_PTS_TO_SEEK, data, sizeof(data));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vfmw chan:%d set pts to seek fail!\n", chan_id);
        return HI_FAILURE;
    }

    stm->actual_pts = data[2]; /* 2 is return value index in array */

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_check_usd(hi_u32 chan_id, hi_u8 *param, hi_u32 size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    VDEC_ASSERT_RET(param != HI_NULL);
    VDEC_ASSERT_RET(size == sizeof(hi_vdec_user_data));

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_CHK_USD, param, sizeof(hi_vdec_user_data));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_DBG("check userdata fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_bind_usd_buf(hi_u32 chan_id, hi_u8 *param, hi_u32 size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_mem_report mem;
    hi_vdec_bind_usd_param *usd_buf_param = (hi_vdec_bind_usd_param*)param;

    VDEC_ASSERT_RET(param != HI_NULL);
    VDEC_ASSERT_RET(size == sizeof(hi_vdec_bind_usd_param));

    ret = memset_s(&mem, sizeof(vfmw_mem_report), 0, sizeof(vfmw_mem_report));
    if (ret != EOK) {
        VDEC_PRINT_ERR("call memset_s fail!\n");
        return HI_FAILURE;
    }

    mem.mem_fd = usd_buf_param->handle;
    mem.size = usd_buf_param->size;

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_BIND_USD_BUF, &mem, sizeof(vfmw_mem_report));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("bind usd buf fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_get_usd(hi_u32 chan_id, hi_u8 *param, hi_u32 size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    VDEC_ASSERT_RET(param != HI_NULL);
    VDEC_ASSERT_RET(size == sizeof(hi_vdec_user_data));

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_ACQ_USD, param, sizeof(hi_vdec_user_data));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_DBG("get userdata fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_rls_usd(hi_u32 chan_id, hi_u8 *param, hi_u32 size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    VDEC_ASSERT_RET(param != HI_NULL);
    VDEC_ASSERT_RET(size == sizeof(hi_vdec_user_data));

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_RLS_USD, param, sizeof(hi_vdec_user_data));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_DBG("rls userdata fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_get_usrdec_stream(hi_u32 chan_id, hi_u8 *param, hi_u32 size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_stream_buf stream_data = {0};
    hi_vdec_stream *stream = (hi_vdec_stream*)param;

    VDEC_ASSERT_RET(param != HI_NULL);
    VDEC_ASSERT_RET(size == sizeof(hi_vdec_stream));

    if (entry->vfmw_func == HI_NULL) {
        VDEC_PRINT_ERR("vfmw not open!\n");
        return HI_FAILURE;
    }

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_GET_USRDEC_STREAM, &stream_data, sizeof(stream_data));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    (hi_void)memset_s(stream, sizeof(hi_vdec_stream), 0, sizeof(hi_vdec_stream));
    stream->buf_phy = stream_data.phy_addr;
    stream->pts = stream_data.pts;
    stream->buf_size = stream_data.length;
    stream->buf_vir.integer = stream_data.usr_vir_addr;
    stream->buf_vir.pointer = (hi_u8 *)(hi_ulong)(stream_data.usr_vir_addr);
    stream->is_end_of_frm = stream_data.is_end_of_frm;
    stream->handle = stream_data.handle;
    stream->offset = stream_data.offset;

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_put_usrdec_stream(hi_u32 chan_id, hi_u8 *param, hi_u32 size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_stream_buf stream_data = {0};
    hi_vdec_stream *stream = (hi_vdec_stream*)param;

    VDEC_ASSERT_RET(param != HI_NULL);
    VDEC_ASSERT_RET(size == sizeof(hi_vdec_stream));

    if (entry->vfmw_func == HI_NULL) {
        VDEC_PRINT_ERR("vfmw not open!\n");
        return HI_FAILURE;
    }

    stream_data.phy_addr = stream->buf_phy;
    stream_data.pts = stream->pts;
    stream_data.length = stream->buf_size;
    stream_data.is_end_of_frm = stream->is_end_of_frm;

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_PUT_USRDEC_STREAM, &stream_data, sizeof(stream_data));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_DBG("vfmw chan %d rel stream fail!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_get_usrdec_frame(hi_u32 chan_id, hi_u8 *param, hi_u32 size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_usrdec_frame_desc usr_frame = {0};
    vdec_usrdec_frame *frame = (vdec_usrdec_frame*)param;

    VDEC_ASSERT_RET(param != HI_NULL);
    VDEC_ASSERT_RET(size == sizeof(vdec_usrdec_frame));

    if (entry->vfmw_func == HI_NULL) {
        VDEC_PRINT_ERR("vfmw not open!\n");
        return HI_FAILURE;
    }

    usr_frame.width = frame->width;
    usr_frame.height = frame->height;
    usr_frame.mem_size = frame->mem_size;
    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_GET_USRDEC_FRAME, &usr_frame, sizeof(usr_frame));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    frame->phy_addr = usr_frame.phy_addr;
    frame->line_num_offset = usr_frame.line_num_offset;
    frame->line_num_size = usr_frame.line_num_size;
    frame->mem_handle = usr_frame.mem_handle;
    frame->mem_size = usr_frame.mem_size;

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_put_usrdec_frame(hi_u32 chan_id, hi_u8 *param, hi_u32 size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_usrdec_frame_desc usr_frame = {0};
    vdec_usrdec_frame *frame = (vdec_usrdec_frame*)param;

    VDEC_ASSERT_RET(param != HI_NULL);
    VDEC_ASSERT_RET(size == sizeof(vdec_usrdec_frame));

    if (entry->vfmw_func == HI_NULL) {
        VDEC_PRINT_ERR("vfmw not open!\n");
        return HI_FAILURE;
    }

    usr_frame.is_frame_valid = frame->is_frame_valid;
    usr_frame.fmt = vdec_cnvt_yuv_fmt_2_vfmw(frame->fmt);
    usr_frame.pts = frame->pts;
    usr_frame.bit_depth = frame->bit_depth;
    usr_frame.width = frame->width;
    usr_frame.height = frame->height;
    usr_frame.luma_stride = frame->luma_stride;
    usr_frame.chrom_stride = frame->chrom_stride;
    usr_frame.phy_addr = frame->phy_addr;
    usr_frame.chrom_offset = frame->chrom_offset;
    usr_frame.line_num_offset = frame->line_num_offset;
    usr_frame.mem_handle = frame->mem_handle;
    usr_frame.mem_size = frame->mem_size;

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_PUT_USRDEC_FRAME, &usr_frame, sizeof(usr_frame));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_send_eos(hi_u32 chan_id, hi_u8 *param, hi_u32 size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vdec_vfmw_chan *chan = vdec_vfmw_get_chan(chan_id);
    hi_bool *eos = (hi_bool*)param;

    VDEC_ASSERT_RET(param != HI_NULL);
    VDEC_ASSERT_RET(size == sizeof(hi_bool));

    chan->is_eos = *eos;
    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_SEND_EOS, eos, sizeof(hi_bool));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_aft_init_input(hi_u32 chan_id, hi_u8 *param, hi_u32 size)
{
    vfmw_stream_buf buf = {0};
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vdec_vfmw_chan *chan = vdec_vfmw_get_chan(chan_id);

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_BIND_ES_BUF, &buf, sizeof(buf));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vfmw chan %d bind es buffer fail!\n", chan_id);
        return HI_FAILURE;
    }

    if (chan->is_sec == HI_FALSE) {
        *((hi_handle*)param) = vdec_drv_get_fd(UINT_2_PTR(*((hi_u64*)param)));
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_aft_command(hi_u32 chan_id, hi_vdec_cmd_id cmd, hi_u8 *param, hi_u32 size)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case HI_VDEC_CMD_INIT_INPUT_BUF:
            ret = vdec_vfmw_aft_init_input(chan_id, param, size);
            break;
        default:
            ret = HI_SUCCESS;
            break;
    }

    return ret;
}

static hi_s32 vdec_vfmw_set_pvr_info(hi_u32 chan_id, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    hi_vdec_pvr_info *pvr_info = (hi_vdec_pvr_info*)param;
    vfmw_control_info ctrl_info;

    ctrl_info.idr_flag = pvr_info->is_idr_flag;
    ctrl_info.disp_pptimize_flag = pvr_info->is_display_optimize;
    ctrl_info.continuous_flag = pvr_info->is_continuous;
    ctrl_info.b_frm_ref_flag = pvr_info->is_b_frm_ref;
    ctrl_info.backward_optimize_flag = pvr_info->is_backward_optimize;
    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_SET_CTRL_INFO, &ctrl_info, sizeof(ctrl_info));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vfmw chan:%d set ctrl info fail!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_set_ff_fb_speed(hi_u32 chan_id, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    hi_vdec_speed *speed_info = (hi_vdec_speed*)param;
    hi_s32 speed;

    /* 1000 :a number 1024 :a number 1000 :a number */
    speed = (speed_info->speed_integer * 1000 + speed_info->speed_decimal) * 1024 / 1000;
    if (speed_info->direct == HI_VDEC_SPEED_BACKWARD) {
        speed = -speed;
    }

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_SET_TRICK_MODE, &speed, sizeof(speed));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vfmw chan:%d set trick mode fail!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_set_frame_rate(hi_u32 chan_id, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    hi_vdec_frm_rate *set_frm_rate = (hi_vdec_frm_rate*)param;
    vfmw_pts_frmrate frm_rate;

    frm_rate.en_frm_rate_type = vdec_cnvt_frmrate_type_2_vfmw(set_frm_rate->type);
    frm_rate.st_set_frm_rate.fps_integer = set_frm_rate->frame_rate / 1000; /* 1000 :a number */
    frm_rate.st_set_frm_rate.fps_decimal = set_frm_rate->frame_rate % 1000; /* 1000 :a number */

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_SET_FRAME_RATE_TYPE, &frm_rate, sizeof(frm_rate));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vfmw chan:%d set frame rate type fail!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_set_start_pts(hi_u32 chan_id, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    hi_vdec_start_pts *start_pts = (hi_vdec_start_pts *)param;

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_SET_IDR_PTS, &start_pts->pts, sizeof(hi_s64));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vfmw chan:%d set IDR PTS fail!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_get_frame_rate(hi_u32 chan_id, hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    hi_vdec_frm_rate *get_frm_rate = (hi_vdec_frm_rate*)param;
    vfmw_pts_frmrate frm_rate;

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_GET_FRAME_RATE_TYPE, &frm_rate, sizeof(frm_rate));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("get vfmw frame rate type error!\n");
        return HI_FAILURE;
    }

    get_frm_rate->type = vdec_cnvt_frmrate_type_2_vdec(frm_rate.en_frm_rate_type);
    /* 1000 :a number */
    get_frm_rate->frame_rate = frm_rate.st_set_frm_rate.fps_integer * 1000 + frm_rate.st_set_frm_rate.fps_decimal;

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_set_uvmos(hi_u32 chan_id, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    hi_bool uvmos_en = *((hi_bool*)param);
    vfmw_chan_cfg cfg = {0};

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_GET_CHAN_CFG, &cfg, sizeof(vfmw_chan_cfg));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("get chan %d vfmw cfg fail!\n", chan_id);
        return HI_FAILURE;
    }

    cfg.uvmos_enable = uvmos_en;

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_CFG_CHAN, &cfg, sizeof(vfmw_chan_cfg));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("set chan %d vfmw cfg fail!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_set_low_delay(hi_u32 chan_id, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    hi_bool low_delay_en = *((hi_bool*)param);
    vfmw_chan_cfg cfg = {0};

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_GET_CHAN_CFG, &cfg, sizeof(vfmw_chan_cfg));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("get chan %d vfmw cfg fail!\n", chan_id);
        return HI_FAILURE;
    }

    cfg.module_lowly_enable = low_delay_en;

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_CFG_CHAN, &cfg, sizeof(vfmw_chan_cfg));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("set chan %d vfmw cfg fail!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_set_dpb_mode(hi_u32 chan_id, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vdec_vfmw_chan *chan = vdec_vfmw_get_chan(chan_id);
    hi_vdec_dpb_mode dpb_mode = *((hi_vdec_dpb_mode*)param);
    vfmw_chan_cfg cfg = {0};

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_GET_CHAN_CFG, &cfg, sizeof(vfmw_chan_cfg));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("get chan %d vfmw cfg fail!\n", chan_id);
        return HI_FAILURE;
    }

    switch (dpb_mode) {
        case HI_VDEC_DPB_MODE_SKYPLAY:
            cfg.dec_order_output = VFMW_DEC_ORDER_SKYPLAY;
            break;
        case HI_VDEC_DPB_MODE_SIMPLE:
            cfg.dec_order_output = VFMW_DEC_ORDER_SIMPLE_DPB;
            break;
        default:
            cfg.dec_order_output = (chan->out_order == HI_VDEC_OUT_ORDER_DISPLAY) ? 0 : 1;
            break;
    }

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_CFG_CHAN, &cfg, sizeof(vfmw_chan_cfg));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("set chan %d vfmw cfg fail!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_set_dec_mode(hi_u32 chan_id, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    hi_vdec_dec_mode dec_mode = *((hi_vdec_dec_mode*)param);
    vfmw_chan_cfg cfg = {0};

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_GET_CHAN_CFG, &cfg, sizeof(vfmw_chan_cfg));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("get chan %d vfmw cfg fail!\n", chan_id);
        return HI_FAILURE;
    }

    cfg.dec_mode = vdec_cnvt_decmode_2_vfmw(dec_mode);

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_CFG_CHAN, &cfg, sizeof(vfmw_chan_cfg));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("set chan %d vfmw cfg fail!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_set_3d_type(hi_u32 chan_id, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    hi_u32 frm_3d_type = *((hi_u32*)param);

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_SET_3D_TYPE, &frm_3d_type, sizeof(hi_u32));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("chan %d set 3d type error!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_get_3d_type(hi_u32 chan_id, hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_GET_3D_TYPE, param, param_size);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("chan %d set 3d type error!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

#ifdef HI_HDR_DOLBYVISION_SUPPORT
static hi_void vdec_vfmw_set_hdr_attr(const hi_vdec_hdr_attr *hdr_attr, vfmw_chan_cfg *cfg)
{
    if (hdr_attr->chan_type != HI_VDEC_HDR_CHAN_NONE) {
        cfg->be_hdr_chan = 1;

        if (hdr_attr->chan_type == HI_VDEC_HDR_CHAN_BL) {
            cfg->dv_capacity = DV_BL_DECODING_SUPPORT;
        } else if (hdr_attr->chan_type == HI_VDEC_HDR_CHAN_EL) {
            cfg->dv_capacity = DV_EL_DECODING_SUPPORT;
        } else {
            VDEC_PRINT_ERR("hdr info error!");
        }

        if (hdr_attr->stm_type == HI_VDEC_HDR_SS_DL_VES || hdr_attr->stm_type == HI_VDEC_HDR_SS_SL_VES) {
            cfg->ves_imp = DV_SINGLE_VES_IMP;
        } else if (hdr_attr->stm_type == HI_VDEC_HDR_DS_DL_VES) {
            cfg->ves_imp = DV_DUAL_VES_IMP;
        } else {
            cfg->ves_imp = DV_VES_IMP_MAX;
        }

        if (hdr_attr->stm_type == HI_VDEC_HDR_SS_SL_VES) {
            cfg->layer_imp = DV_SINGLE_LAYER_IMP;
        } else if (hdr_attr->stm_type == HI_VDEC_HDR_SS_DL_VES
                 || hdr_attr->stm_type == HI_VDEC_HDR_DS_DL_VES) {
            cfg->layer_imp = DV_DUAL_LAYER_IMP;
        } else {
            cfg->layer_imp = DV_LAYER_IMP_MAX;
        }

        cfg->dv_compatible = hdr_attr->is_dv_frm_compatible;
    } else {
        cfg->be_hdr_chan = 0;
    }

    return;
}

static hi_s32 vdec_vfmw_set_hdr_param(hi_u32 chan_id, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    hi_vdec_hdr_attr *hdr_attr = (hi_vdec_hdr_attr*)param;
    vfmw_chan_cfg cfg = {0};

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_GET_CHAN_CFG, &cfg, sizeof(vfmw_chan_cfg));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("get chan %d vfmw cfg fail!\n", chan_id);
        return HI_FAILURE;
    }

    vdec_vfmw_set_hdr_attr(hdr_attr, &cfg);

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_CFG_CHAN, &cfg, sizeof(vfmw_chan_cfg));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("set chan %d vfmw cfg fail!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
#endif

static hi_s32 vdec_vfmw_set_stm_mode(hi_u32 chan_id, const hi_u8 *param, hi_u32 param_size)
{
    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_set_frm_mode(hi_u32 chan_id, const hi_u8 *param, hi_u32 param_size)
{
    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_set_out_fmt(hi_u32 chan_id, const hi_u8 *param, hi_u32 param_size)
{
    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_set_poc_threshold(hi_u32 chan_id, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    hi_u32 poc_threshold = *((hi_u32*)param);
    vfmw_chan_cfg cfg = {0};

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_GET_CHAN_CFG, &cfg, sizeof(vfmw_chan_cfg));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("get chan %d vfmw cfg fail!\n", chan_id);
        return HI_FAILURE;
    }

    cfg.poc_threshold = poc_threshold;

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_CFG_CHAN, &cfg, sizeof(vfmw_chan_cfg));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("set chan %d vfmw cfg fail!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_set_source_tag(hi_u32 chan_id, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    hi_s32 src_tag = ((hi_vdec_src_tag*)param)->tag_value;
    vfmw_chan_cfg cfg = {0};

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_GET_CHAN_CFG, &cfg, sizeof(vfmw_chan_cfg));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("get chan %d vfmw cfg fail!\n", chan_id);
        return HI_FAILURE;
    }

    cfg.source = src_tag;

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_CFG_CHAN, &cfg, sizeof(vfmw_chan_cfg));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("set chan %d vfmw cfg fail!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_get_feature(hi_u32 chan_id, hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;
    hi_vdec_feature *feature = (hi_vdec_feature*)param;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_chan_info vfmw_status = {0};

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_GET_CHAN_STATE, &vfmw_status, sizeof(vfmw_chan_info));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("get vfmw chan status fail!\n");
        return HI_FAILURE;
    }

    feature->standard = vdec_cnvt_std_2_vdec(vfmw_status.std);
    feature->sub_std = HI_VDEC_SUB_STD_UNKNOWN;
    feature->sub_version = 0;
    feature->profile = vfmw_status.profile;
    feature->level = vfmw_status.level;
    feature->norm_type = vdec_cnvt_disp_norm_2_vdec(vfmw_status.video_format);
    feature->sampling_type = vdec_cnvt_sampling_type_2_vdec(vfmw_status.scan_type);
    feature->aspect_height = vfmw_status.aspect_height;
    feature->aspect_width = vfmw_status.aspect_width;
    feature->bit_rate = vfmw_status.bit_rate;
    feature->frame_rate = vfmw_status.frame_rate;
    feature->dec_width = vfmw_status.image_width;
    feature->dec_height = vfmw_status.image_height;
    feature->disp_width = vfmw_status.disp_width;
    feature->disp_height = vfmw_status.disp_height;
    feature->disp_center_x = vfmw_status.disp_center_x;
    feature->disp_center_y = vfmw_status.disp_center_y;
    feature->bit_depth = vdec_cnvt_bit_depth_2_vdec(vfmw_status.bit_depth);
    feature->color_space = vdec_cnvt_color_space_2_vdec(vfmw_status.std, &vfmw_status.color_info,
        vfmw_status.image_width, vfmw_status.image_height);

    VDEC_PRINT_DBG("std:%x profile:%x level:%x norm:%d sample_type:%d aspect_height:%d aspect_width:%d bit_rate:%d "
        "frame_rate:%d dec_width:%d dec_height:%d disp_height:%d disp_width:%d center_x:%d center_y:%d "
        "bit_depth:%d color_space:%d\n", feature->standard, feature->profile, feature->level, feature->norm_type,
        feature->sampling_type, feature->aspect_height, feature->aspect_width, feature->bit_rate, feature->frame_rate,
        feature->dec_width, feature->dec_height, feature->disp_height, feature->disp_width,
        feature->disp_center_x, feature->disp_center_y, feature->bit_depth, feature->color_space);

    return HI_SUCCESS;
}

static hi_s32 vdec_vfmw_get_status(hi_u32 chan_id, hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;
    hi_vdec_status *status = (hi_vdec_status*)param;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_chan_info vfmw_status = {0};

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_GET_CHAN_STATE, &vfmw_status, sizeof(vfmw_chan_info));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("get vfmw chan status fail!\n");
        return HI_FAILURE;
    }

    status->stm_state.bit_rate_in_bps = vfmw_status.bit_rate;
    status->stm_state.free_stm_buf_size = vfmw_status.free_stm_buf_size;
    status->stm_state.is_end_of_stm = vfmw_status.is_end_of_stm;
    status->stm_state.total_stm_buf_size = vfmw_status.total_stm_buf_size;
    status->stm_state.undecode_stm_num = vfmw_status.undecode_stm_num;
    status->stm_state.undecode_stm_size = vfmw_status.undecode_stm_size;
    status->stm_state.used_stm_buf_size = vfmw_status.used_stm_buf_size;

    status->frm_state.error_frm_num = vfmw_status.error_frame_num;
    status->frm_state.total_output_frm_num = vfmw_status.total_frame_num;
    status->frm_state.wait_output_frm_num = vfmw_status.wait_disp_frame_num;
    status->frm_state.is_field = vfmw_status.is_field_flg;
    status->frm_state.pvr_buf_num = vfmw_status.pvr_buf_num;
    status->frm_state.out_frm_rate = vfmw_status.frame_rate;
    status->frm_state.dec_frm_rate = vfmw_status.dec_frm_rate;

    VDEC_PRINT_DBG("bitrate:%d free_stm_buf %x eos:%d total_stm_buf:%x undecoded_num:%d undecoded size:%x "
        "used_stm_size:%x err_frm_num:%d total_out:%d wait_out:%d fps:%d field_flag:%d pvr_buf_num: %d "
        "dec_fps:%d \n",
        vfmw_status.bit_rate, vfmw_status.free_stm_buf_size, vfmw_status.is_end_of_stm, vfmw_status.total_stm_buf_size,
        vfmw_status.undecode_stm_num, vfmw_status.undecode_stm_size, vfmw_status.used_stm_buf_size,
        vfmw_status.error_frame_num, vfmw_status.total_frame_num, vfmw_status.wait_disp_frame_num,
        vfmw_status.frame_rate, vfmw_status.is_field_flg, vfmw_status.pvr_buf_num, vfmw_status.dec_frm_rate);

    return HI_SUCCESS;
}

hi_s32 vdec_vfmw_open(hi_void)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    ret = vdec_osal_atomic_init(&entry->init_cnt);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (entry->vfmw_func == HI_NULL) {
        ret = vdec_drv_get_vfmw_func((hi_void **)&entry->vfmw_func);
        if (ret != HI_SUCCESS) {
            vdec_osal_atomic_exit(&entry->init_cnt);
            VDEC_PRINT_ERR("get vfmw ext func fail!\n");
            return HI_FAILURE;
        }

        ret = entry->vfmw_func->fn_vfmw_control(HI_INVALID_HANDLE, VFMW_CID_GET_CAPABILITY,
            &entry->cap, sizeof(entry->cap));
        if (ret != HI_SUCCESS) {
            vdec_osal_atomic_exit(&entry->init_cnt);
            VDEC_PRINT_ERR("get vfmw cap fail!\n");
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

hi_s32 vdec_vfmw_close(hi_void)
{
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    entry->vfmw_func = HI_NULL;

    vdec_osal_atomic_exit(&entry->init_cnt);

    return HI_SUCCESS;
}

hi_s32 vdec_vfmw_init(hi_void)
{
    hi_s32 ret = HI_FAILURE;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_init_param operation = {0};

    if (entry->vfmw_func == HI_NULL) {
        VDEC_PRINT_ERR("vfmw not open!\n");
        return HI_FAILURE;
    }

    if (vdec_osal_atomic_inc_return(&entry->init_cnt) == 1) {
        operation.adpt_type = ADPT_TYPE_VDEC;
        operation.fn_callback = vdec_vfmw_event_call_back;
        ret = entry->vfmw_func->fn_vfmw_init(&operation);
        if (ret != HI_SUCCESS) {
            vdec_osal_atomic_dec(&entry->init_cnt);
            VDEC_PRINT_ERR("vfmw init error!\n");
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

hi_s32 vdec_vfmw_exit(hi_void)
{
    hi_s32 ret = HI_FAILURE;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    hi_bool is_sec = HI_FALSE;

    if (entry->vfmw_func == HI_NULL) {
        VDEC_PRINT_ERR("vfmw not open!\n");
        return HI_FAILURE;
    }

    if (vdec_osal_atomic_dec_return(&entry->init_cnt) == 0) {
        ret = entry->vfmw_func->fn_vfmw_exit(&is_sec);
        if (ret != HI_SUCCESS) {
            vdec_osal_atomic_inc(&entry->init_cnt);
            VDEC_PRINT_ERR("vfmw exit error!\n");
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

hi_s32 vdec_vfmw_suspend(hi_void)
{
    hi_s32 ret;
    vfmw_export_fun *vfmw_func = HI_NULL;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    vfmw_func = entry->vfmw_func;
    if (vfmw_func == HI_NULL) {
        ret = vdec_drv_get_vfmw_func((hi_void **)&vfmw_func);
        if (ret != HI_SUCCESS) {
            VDEC_PRINT_ERR("get vfmw ext func fail!\n");
            return HI_FAILURE;
        }
    }

    ret = vfmw_func->fn_vfmw_suspend(HI_NULL);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vfmw suspend fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vdec_vfmw_resume(hi_void)
{
    hi_s32 ret;
    vfmw_export_fun *vfmw_func = HI_NULL;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    vfmw_func = entry->vfmw_func;
    if (vfmw_func == HI_NULL) {
        ret = vdec_drv_get_vfmw_func((hi_void **)&vfmw_func);
        if (ret != HI_SUCCESS) {
            VDEC_PRINT_ERR("get vfmw ext func fail!\n");
            return HI_FAILURE;
        }
    }

    ret = vfmw_func->fn_vfmw_resume(HI_NULL);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vfmw resume fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vdec_vfmw_create(hi_u32 *chan_id, const vdec_vfmw_create_param *param)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vdec_vfmw_chan *chan = HI_NULL;
    vfmw_chan_option option = {0};

    vdec_vfmw_init_chan_param(&option, param);

    if (entry->vfmw_func == HI_NULL) {
        VDEC_PRINT_ERR("vfmw not open!\n");
        return HI_FAILURE;
    }

    ret = entry->vfmw_func->fn_vfmw_control(-1, VFMW_CID_CREATE_CHAN, &option, sizeof(option));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vfmw create chan fail!\n");
        return HI_FAILURE;
    }

    *chan_id = option.chan_id;
    chan = vdec_vfmw_get_chan(*chan_id);

    chan->is_used = HI_TRUE;
    chan->is_sec = param->attr->is_tvp;
    chan->call_back = param->callback;
    chan->vdec_chan_id = param->vdec_id;

    return HI_SUCCESS;
}

hi_s32 vdec_vfmw_destroy(hi_u32 chan_id,  const hi_vdec_opt_destroy *param)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vdec_vfmw_chan *chan = vdec_vfmw_get_chan(chan_id);

    if (entry->vfmw_func == HI_NULL) {
        VDEC_PRINT_ERR("vfmw not open!\n");
        return HI_FAILURE;
    }

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_DESTROY_CHAN, HI_NULL, 0);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vfmw destroy chan %d fail!\n", chan_id);
        return HI_FAILURE;
    }

    chan->is_used = HI_FALSE;

    return HI_SUCCESS;
}

hi_s32 vdec_vfmw_start(hi_u32 chan_id, const hi_vdec_opt_start *param)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    if (entry->vfmw_func == HI_NULL) {
        VDEC_PRINT_ERR("vfmw not open!\n");
        return HI_FAILURE;
    }

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_START_CHAN, HI_NULL, 0);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vfmw start chan %d fail!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vdec_vfmw_stop(hi_u32 chan_id, const hi_vdec_opt_stop *param)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    if (entry->vfmw_func == HI_NULL) {
        VDEC_PRINT_ERR("vfmw not open!\n");
        return HI_FAILURE;
    }

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_STOP_CHAN, HI_NULL, 0);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vfmw stop chan %d fail!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vdec_vfmw_reset(hi_u32 chan_id, const hi_vdec_opt_reset *param)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    if (entry->vfmw_func == HI_NULL) {
        VDEC_PRINT_ERR("vfmw not open!\n");
        return HI_FAILURE;
    }

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_RESET_CHAN, HI_NULL, 0);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vfmw reset chan %d fail!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vdec_vfmw_flush(hi_u32 chan_id, const hi_vdec_opt_flush *param)
{
    hi_s32 ret ;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_flush_type flush_type;

    if (entry->vfmw_func == HI_NULL) {
        VDEC_PRINT_ERR("vfmw not open!\n");
        return HI_FAILURE;
    }

    flush_type = vdec_cnvt_flush_type_2_vfmw(param->type);
    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_FLUSH_CHAN, &flush_type, sizeof(flush_type));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vfmw flush chan %d fail!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vdec_vfmw_set_attr(hi_u32 chan_id, const hi_vdec_attr *param, const vdec_vfmw_param *ext_attr)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vdec_vfmw_chan *chan = vdec_vfmw_get_chan(chan_id);
    vfmw_chan_cfg vfmw_cfg = {0};

    if (entry->vfmw_func == HI_NULL) {
        VDEC_PRINT_ERR("vfmw not open!\n");
        return HI_FAILURE;
    }

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_GET_CHAN_CFG, &vfmw_cfg, sizeof(vfmw_cfg));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vfmw get chan %d cfg fail!\n", chan_id);
        return HI_FAILURE;
    }

    vfmw_cfg.is_miracast_chan = (ext_attr->dpb_mode == HI_VDEC_DPB_MODE_SIMPLE) ? 1 : 0;
    vfmw_cfg.module_lowly_enable = ext_attr->enable_low_delay;
    vfmw_cfg.chan_lowly_enable = 0;
    vfmw_cfg.sed_only_enable = 1;
    vfmw_cfg.max_raw_packet_num = param->max_raw_num;
    if (ext_attr->dpb_mode == HI_VDEC_DPB_MODE_SKYPLAY) {
        vfmw_cfg.dec_order_output = VFMW_DEC_ORDER_SKYPLAY;
    } else if (ext_attr->dpb_mode == HI_VDEC_DPB_MODE_SIMPLE) {
        vfmw_cfg.dec_order_output = VFMW_DEC_ORDER_SIMPLE_DPB;
    } else {
        vfmw_cfg.dec_order_output = (param->out_order == HI_VDEC_OUT_ORDER_DISPLAY) ? 0 : 1;
    }
    chan->out_order = param->out_order;

    vfmw_cfg.vid_std = vdec_cnvt_std_2_vfmw(param->standard);

    vdec_cnvt_standard_ext(param->standard, &param->standard_ext, &vfmw_cfg.std_ext);

    vfmw_cfg.extra_frame_store_num = param->ext_fs_num;
    vfmw_cfg.chan_priority = param->priority;
    vfmw_cfg.chan_err_thr = param->error_cover;
    vfmw_cfg.dec_mode = vdec_cnvt_decmode_2_vfmw(ext_attr->dec_mode);

    if (ext_attr->enable_low_delay) {
        vfmw_cfg.dec_mode = VFMW_DEC_MODE_NORMAL;
    }

    vfmw_cfg.v_cmp_mode = vdec_cnvt_cmp_2_vfmw(param->cmp_mode);

    vfmw_cfg.is_tunneled_path = param->is_tunnel;
    vfmw_cfg.uvmos_enable = ext_attr->enable_uvmos;

    vfmw_cfg.vdec_handle = chan->vdec_chan_id;
    vfmw_cfg.source = ext_attr->source_tag.tag_value;
    vfmw_cfg.poc_threshold = ext_attr->poc_thrshold;
    vfmw_cfg.usd_pool_size = param->usd_pool_size;

#ifdef HI_HDR_DOLBYVISION_SUPPORT
    vdec_vfmw_set_hdr_attr(&ext_attr->hdr_attr, &vfmw_cfg);
#endif

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_CFG_CHAN, &vfmw_cfg, sizeof(vfmw_cfg));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vfmw set chan %d cfg fail!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vdec_vfmw_listen_event(hi_u32 chan_id, const hi_vdec_opt_listen *param)
{
    return HI_SUCCESS;
}

hi_s32 vdec_vfmw_set_event_map(hi_u32 chan_id, hi_u32 event_map)
{
    hi_s32 ret;
    hi_u32 vfmw_event_map;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    if (entry->vfmw_func == HI_NULL) {
        VDEC_PRINT_ERR("vfmw not open!\n");
        return HI_FAILURE;
    }

    vfmw_event_map = vdec_cnvt_event_map_2_vfmw(event_map);

    VDEC_PRINT_DBG("vdec_event_map %x vfmw_event_map %x\n", event_map, vfmw_event_map);

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_SET_EVENT_MAP, &vfmw_event_map, sizeof(hi_u32));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vfmw set chan %d event map fail!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vdec_vfmw_queue_stream(hi_u32 chan_id, const hi_vdec_opt_que_stm *option, const hi_vdec_stream *stream)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_stream_buf stream_data = {0};

    if (entry->vfmw_func == HI_NULL) {
        VDEC_PRINT_ERR("vfmw not open!\n");
        return HI_FAILURE;
    }

    stream_data.handle = stream->handle;
    stream_data.offset = stream->offset;
    stream_data.index = stream->index;
    stream_data.pts = stream->pts;
    stream_data.length = stream->buf_size;
    stream_data.is_end_of_frm = stream->is_end_of_frm;
    stream_data.is_dual_layer = stream->is_dual_layer;
    stream_data.disp_enable_flag = stream->disp_enable_flag;
    stream_data.disp_frm_distance = stream->disp_frm_distance;
    stream_data.disp_time = stream->disp_time;
    stream_data.gop_num = stream->gop_num;
    stream_data.distance_before_first_frame = stream->distance_before_first_frame;

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_QUEUE_STREAM, &stream_data, sizeof(stream_data));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_DBG("vfmw chan %d queue stream fail!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vdec_vfmw_dequeue_stream(hi_u32 chan_id, const hi_vdec_opt_deq_stm *option, hi_vdec_stream *stream)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_stream_buf stream_data = {0};

    if (entry->vfmw_func == HI_NULL) {
        VDEC_PRINT_ERR("vfmw not open!\n");
        return HI_FAILURE;
    }
    stream_data.length = option->expect_size;
    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_DEQUEUE_STREAM, &stream_data, sizeof(stream_data));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_DBG("vfmw chan %d dequeue stream fail!\n", chan_id);
        return HI_FAILURE;
    }

    stream->buf_phy = stream_data.phy_addr;
    stream->buf_size = stream_data.length;
    if (option->is_kern) {
        stream->buf_vir.integer = stream_data.kern_vir_addr;
    } else {
        stream->buf_vir.integer = stream_data.usr_vir_addr;
    }
    stream->pts = stream_data.pts;
    stream->handle = stream_data.handle;
    stream->offset = stream_data.offset;

    return HI_SUCCESS;
}

hi_s32 vdec_vfmw_acquire_frame(hi_u32 chan_id, const hi_vdec_opt_acq_frm *option, hi_void *frame, hi_u32 size,
    hi_vdec_ext_frm_info *ext_frm_info)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    if (entry->vfmw_func == HI_NULL) {
        VDEC_PRINT_ERR("vfmw not open!\n");
        return HI_FAILURE;
    }

    ret = entry->vfmw_func->fn_vfmw_get_frame(chan_id, frame, ext_frm_info);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vdec_vfmw_release_frame(hi_u32 chan_id, const hi_vdec_opt_rel_frm *option, const hi_void *frame, hi_u32 size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    if (entry->vfmw_func == HI_NULL) {
        VDEC_PRINT_ERR("vfmw not open!\n");
        return HI_FAILURE;
    }

    ret = entry->vfmw_func->fn_vfmw_rels_frame(chan_id, frame);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_DBG("vfmw chan %d release frame fail!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

vdec_vfmw_cmd_item g_vdec_vfmw_command[] = {
    { HI_VDEC_CMD_IS_STD_SUPPORT,                        vdec_vfmw_is_std_support },
    { HI_VDEC_CMD_IS_RANDOM_STM_SUPPORT,                 vdec_vfmw_is_random_stm_support },
    { HI_VDEC_CMD_SET_TVP,                               vdec_vfmw_set_tvp },
    { HI_VDEC_CMD_BIND_INPUT_BUF,                        vdec_vfmw_bind_ext_es_buf },
    { HI_VDEC_CMD_DROP_STREAM,                           vdec_vfmw_drop_stream },
    { HI_VDEC_CMD_BIND_USD_BUF,                          vdec_vfmw_bind_usd_buf },
    { HI_VDEC_CMD_ACQ_USER_DATA,                         vdec_vfmw_get_usd },
    { HI_VDEC_CMD_REL_USER_DATA,                         vdec_vfmw_rls_usd },
    { HI_VDEC_CMD_CHECK_USER_DATA,                       vdec_vfmw_check_usd },
    { (hi_vdec_cmd_id)VDEC_PRIVATE_CMD_GET_STREAM,       vdec_vfmw_get_usrdec_stream },
    { (hi_vdec_cmd_id)VDEC_PRIVATE_CMD_PUT_STREAM,       vdec_vfmw_put_usrdec_stream },
    { (hi_vdec_cmd_id)VDEC_PRIVATE_CMD_GET_FRAME,        vdec_vfmw_get_usrdec_frame },
    { (hi_vdec_cmd_id)VDEC_PRIVATE_CMD_PUT_FRAME,        vdec_vfmw_put_usrdec_frame },
    { HI_VDEC_CMD_SEND_EOS,                              vdec_vfmw_send_eos },
};

hi_s32 vdec_vfmw_command(hi_u32 chan_id, hi_vdec_cmd_id cmd, hi_u8 *param, hi_u32 size)
{
    hi_s32 ret = HI_FAILURE;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    hi_s32 i = 0;
    fun_command cmd_fun = HI_NULL;

    if (entry->vfmw_func == HI_NULL) {
        VDEC_PRINT_ERR("vfmw not open!\n");
        return HI_FAILURE;
    }

    for (i = 0; i < sizeof(g_vdec_vfmw_command) / sizeof(vdec_vfmw_cmd_item); i++) {
        if (g_vdec_vfmw_command[i].id == cmd) {
            cmd_fun = g_vdec_vfmw_command[i].cmd_fun;
            break;
        }
    }

    if (cmd_fun != HI_NULL) {
        ret = cmd_fun(chan_id, param, size);
    } else {
        ret = entry->vfmw_func->fn_vfmw_control(chan_id, vdec_cnvt_cmd_id_2_vfmw(cmd), param, size);
    }

    if (ret == HI_SUCCESS) {
        ret = vdec_vfmw_aft_command(chan_id, cmd, param, size);
    }

    return ret;
}

static vdec_vfmw_param_item g_vdec_vfmw_param[] = {
    { HI_VDEC_PARAM_UVMOS,         vdec_vfmw_set_uvmos,         HI_NULL },
    { HI_VDEC_PARAM_LOW_DELAY,     vdec_vfmw_set_low_delay,     HI_NULL },
    { HI_VDEC_PARAM_DPB_MODE,      vdec_vfmw_set_dpb_mode,      HI_NULL },
#ifdef HI_HDR_DOLBYVISION_SUPPORT
    { HI_VDEC_PARAM_HDR_ATTR,      vdec_vfmw_set_hdr_param,     HI_NULL },
#endif
    { HI_VDEC_PARAM_STM_BUF_MODE,  vdec_vfmw_set_stm_mode,      HI_NULL },
    { HI_VDEC_PARAM_FRM_BUF_MODE,  vdec_vfmw_set_frm_mode,      HI_NULL },
    { HI_VDEC_PARAM_OUTPUT_FORMAT, vdec_vfmw_set_out_fmt,       HI_NULL },
    { HI_VDEC_PARAM_POC_THRSHOLD,  vdec_vfmw_set_poc_threshold, HI_NULL },
    { HI_VDEC_PARAM_SOURCE_TAG,    vdec_vfmw_set_source_tag,    HI_NULL },
    { HI_VDEC_PARAM_FRAME_RATE,    vdec_vfmw_set_frame_rate,    vdec_vfmw_get_frame_rate },
    { HI_VDEC_PARAM_PVR_INFO,      vdec_vfmw_set_pvr_info,      HI_NULL },
    { HI_VDEC_PARAM_FF_FB_SPEED,   vdec_vfmw_set_ff_fb_speed,   HI_NULL },
    { HI_VDEC_PARAM_START_PTS,     vdec_vfmw_set_start_pts,     HI_NULL },
    { HI_VDEC_PARAM_DEC_MODE,      vdec_vfmw_set_dec_mode,      HI_NULL },
    { HI_VDEC_PARAM_FEATURE,       HI_NULL,                     vdec_vfmw_get_feature },
    { HI_VDEC_PARAM_STATUS,        HI_NULL,                     vdec_vfmw_get_status },
    { HI_VDEC_PARAM_FRM_3D_TYPE,   vdec_vfmw_set_3d_type,       vdec_vfmw_get_3d_type },
};

hi_s32 vdec_vfmw_get_param(hi_u32 chan_id, hi_vdec_param_id param_id, hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;
    hi_s32 i = 0;
    fun_get_param get_param = HI_NULL;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    if (entry->vfmw_func == HI_NULL) {
        VDEC_PRINT_ERR("vfmw not open!\n");
        return HI_FAILURE;
    }

    for (i = 0; i < (sizeof(g_vdec_vfmw_param) / sizeof(vdec_vfmw_param_item)); i++) {
        if (g_vdec_vfmw_param[i].id == param_id) {
            get_param = g_vdec_vfmw_param[i].get_param;
            break;
        }
    }

    if (get_param == HI_NULL) {
        VDEC_PRINT_ERR("chan %d get param %d not support!\n");
        return HI_FAILURE;
    }

    ret = get_param(chan_id, param, param_size);

    return ret;
}

hi_s32 vdec_vfmw_set_param(hi_u32 chan_id, hi_vdec_param_id param_id, const hi_u8 *param, hi_u32 param_size)
{
    hi_s32 ret;
    hi_s32 i = 0;
    fun_set_param set_param = HI_NULL;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    if (entry->vfmw_func == HI_NULL) {
        VDEC_PRINT_ERR("vfmw not open!\n");
        return HI_FAILURE;
    }

    for (i = 0; i < (sizeof(g_vdec_vfmw_param) / sizeof(vdec_vfmw_param_item)); i++) {
        if (g_vdec_vfmw_param[i].id == param_id) {
            set_param = g_vdec_vfmw_param[i].set_param;
            break;
        }
    }

    if (set_param == HI_NULL) {
        VDEC_PRINT_ERR("chan %d set param %d not support!\n");
        return HI_FAILURE;
    }

    ret = set_param(chan_id, param, param_size);

    return ret;
}

hi_s32 vdec_vfmw_wakeup(hi_u32 chan_id, hi_u8 *param, hi_u32 size)
{
    hi_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    if (entry->vfmw_func == HI_NULL) {
        VDEC_PRINT_ERR("vfmw not open!\n");
        return HI_FAILURE;
    }

    ret = entry->vfmw_func->fn_vfmw_control(chan_id, VFMW_CID_EXTRA_WAKEUP_THREAD, param, size);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_DBG("vfmw chan %d wakeup fail!\n", chan_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vdec_vfmw_read_proc(hi_void *pf, hi_u32 chan_id)
{
    vdec_vfmw_chan *chan = HI_NULL;

    hi_u8 *out_order[] = {"display", "dec"};

    VDEC_ASSERT_RET(chan_id < VDEC_MAX_CHAN_NUM);

    chan = vdec_vfmw_get_chan(chan_id);

    PROC_PRINT(pf, VP_HEAD, "VFMW");

    PROC_PRINT(pf, VP_D_D, "vdec_id", chan->vdec_chan_id, "vfmw_id", chan_id);
    PROC_PRINT(pf, VP_S_D, "out_order", out_order[chan->out_order], "is_sec", chan->is_sec);
    PROC_PRINT(pf, VP_D_D, "is_used", chan->is_used, "is_eos", chan->is_eos);

    PROC_PRINT(pf, "\n");

    return HI_SUCCESS;
}

