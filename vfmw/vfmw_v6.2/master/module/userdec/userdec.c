/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "userdec.h"
#include "vfmw_osal.h"
#include "dbg.h"
#include "vmm_ext.h"
#include "vfmw_define.h"
#include "stream.h"

#define USER_MAX_FS_NUM         6
#define USER_MAX_FS_SIZE        (20*1024*1024)  /* 20M */
#define USER_INDEX_OVERFLOW_NUM 1000000

#define USER_LOWDELAY_LINE_SIZE 32

typedef hi_s32 (*user_handler)(hi_s32, hi_void *, hi_u32);

typedef struct {
    vfmw_cid cmd_id;
    user_handler handler;
    hi_u32 length;
} user_case;

typedef enum {
    FRM_NULL,
    FRM_IDLE,
    FRM_GET,
    FRM_PUT,
    FRM_ACQ,
    FRM_BUTT
} user_frm_state;

typedef struct {
    hi_s32 order;
    user_frm_state state;
    vfmw_mem_desc st_mem;
    vfmw_image image;
} user_frame;

typedef struct {
    hi_s32 chan_id;
    hi_s32 unid;
    hi_s32 index;
    hi_s32 sec_flag;
    hi_u32 img_slot_len;
    hi_s32 frm_alloc_done_cnt;
    hi_u32 last_image_id;
    hi_bool report_eos;
    vfmw_adpt_type adpt_type;
    vfmw_chan_cfg cfg;
    user_frame frm[USER_MAX_FS_NUM];
} user_ctx;

typedef struct {
    OS_SEMA sema;
    hi_u32 chan_num;
    fun_vfmw_event_report fp_vdec_report;
    fun_vfmw_event_report fp_omx_report;
} user_global_ctx;

user_ctx *g_user_chan_ctx[VFMW_CHAN_NUM];
user_global_ctx g_user_ctx;

#ifdef VFMW_USER_SUPPORT
static hi_void user_read_chan(hi_void *buf, hi_s32 len, hi_s32 *used, user_ctx *chan)
{
    hi_s32 i;
    user_frame *frm = HI_NULL;
    hi_u8 *state_str[FRM_BUTT] = { "null", "idle", "get", "put", "acq"};

    OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_D, "chan_id", chan->chan_id, "unid", chan->unid);
    OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_D, "adpt_type", chan->adpt_type, "frm_num",
                 chan->frm_alloc_done_cnt);
    OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_D, "sec_flag", chan->sec_flag, "frm_size", chan->img_slot_len);
    OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_D, "last_image_id", chan->last_image_id, "report_eos",
                 chan->report_eos);
    OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_S, "standard", dbg_show_std(chan->cfg.vid_std, &chan->cfg.std_ext),
                 "lowdelay", chan->cfg.module_lowly_enable);

    OS_DUMP_PROC(buf, len, used, 0, "  frame list:\n");
    for (i = 0; i < USER_MAX_FS_NUM; i++) {
        frm = &(chan->frm[i]);
        if (frm->state == FRM_NULL) {
            continue;
        }
        OS_DUMP_PROC(buf, len, used, 0, "  %d: state(%s), size(0x%x), phy(0x%llx)\n",
                     i, state_str[frm->state], frm->st_mem.length, frm->st_mem.phy_addr);
    }
    OS_DUMP_PROC(buf, len, used, 0, "\n");
}

static hi_s32 user_read_proc(hi_void *buf, hi_s32 chan_id)
{
    hi_s32 used = 0;
    hi_s32 len = 0;

    VFMW_ASSERT_RET(buf != HI_NULL, HI_FAILURE);
    OS_DUMP_PROC(buf, len, &used, 0, LOG_FMT_HEAD, "user dec");

    if (chan_id < 0 || chan_id >= VFMW_CHAN_NUM) {
        OS_DUMP_PROC(buf, len, &used, 0, "  chan id %d invalid.\n\n", chan_id);
        return HI_FAILURE;
    }

    if (g_user_ctx.chan_num == 0) {
        OS_DUMP_PROC(buf, len, &used, 0, "  user no chan exist.\n\n");
        return HI_FAILURE;
    }

    if (g_user_chan_ctx[chan_id] == HI_NULL) {
        OS_DUMP_PROC(buf, len, &used, 0, "  user chan %d not exist.\n\n", chan_id);
        return HI_FAILURE;
    }

    user_read_chan(buf, len, &used, g_user_chan_ctx[chan_id]);

    return HI_SUCCESS;
}

static hi_s32 user_write_proc(cmd_str_ptr buf, hi_u32 count)
{
    VFMW_ASSERT_RET(buf != HI_NULL, HI_FAILURE);

    return count;
}
#endif

static hi_s32 user_set_callback(vfmw_adpt_type type, fun_vfmw_event_report fp_event_report)
{
    VFMW_ASSERT_RET(fp_event_report != HI_NULL, HI_FAILURE);

    switch (type) {
        case ADPT_TYPE_VDEC:
            g_user_ctx.fp_vdec_report = fp_event_report;
            break;

        case ADPT_TYPE_OMX:
            g_user_ctx.fp_omx_report = fp_event_report;
            break;

        default:
            dprint(PRN_ERROR, "%s Unkown adpt type: %d\n", __func__, type);
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void user_event_report(hi_s32 chan_id, hi_s32 type, hi_void *args, hi_s32 len)
{
    user_ctx *chan = g_user_chan_ctx[chan_id];

    switch (chan->adpt_type) {
        case ADPT_TYPE_VDEC:
            VFMW_ASSERT_PRNT(g_user_ctx.fp_vdec_report != HI_NULL, "ERR: fp_vdec_report = HI_NULL\n");
            g_user_ctx.fp_vdec_report(chan_id, type, args, len);
            break;
        case ADPT_TYPE_OMX:
            VFMW_ASSERT_PRNT(g_user_ctx.fp_omx_report != HI_NULL, "ERR: fp_omx_report = HI_NULL\n");
            g_user_ctx.fp_omx_report(chan_id, type, args, len);
            break;
        default:
            dprint(PRN_ERROR, "Unsupport adpt_type %d.\n", chan->adpt_type);
            break;
    }

    return;
}

static hi_bool user_is_frame_list_empty(user_ctx *chan)
{
    hi_s32 i = 0;

    for (i = 0; i < USER_MAX_FS_NUM; i++) {
        if (chan->frm[i].state == FRM_PUT) {
            return HI_FALSE;
        }
    }

    return HI_TRUE;
}

static hi_s32 user_alloc_frame_buffer(user_ctx *chan, hi_u32 frame_size)
{
    hi_u64 unid;
    hi_s32 index;
    hi_s32 ret;
    hi_u32 img_slot_len;
    vmm_buffer user_vmm_buf;

    if (frame_size >= USER_MAX_FS_SIZE) {
        dprint(PRN_ERROR, "Invalid param: frame_size %u >= USER_MAX_FS_SIZE %u.\n", frame_size, USER_MAX_FS_SIZE);
        return HI_FAILURE;
    }

    img_slot_len = frame_size;
    if (chan->cfg.module_lowly_enable == 1) {
#ifdef VFMW_MODULE_LOWDLY_SUPPORT
        img_slot_len += USER_LOWDELAY_LINE_SIZE;
#endif
    }

    unid = SET_CHAN_UID((hi_u32)chan->unid, (hi_u32)chan->chan_id);

    VFMW_CHECK_SEC_FUNC(memset_s(&user_vmm_buf, sizeof(vmm_buffer), 0, sizeof(vmm_buffer)));

    if (snprintf_s(user_vmm_buf.buf_name, sizeof(user_vmm_buf.buf_name),
                   sizeof(user_vmm_buf.buf_name), "%s", "MjpegFrmBuf") < 0) {
        dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
    }
    user_vmm_buf.buf_name[sizeof(user_vmm_buf.buf_name) - 1] = '\0';
    user_vmm_buf.map = 0;
    user_vmm_buf.cache = 0;
    user_vmm_buf.size = img_slot_len;
    user_vmm_buf.priv_id = SET_MEM_UID(unid, VFMW_MEM_GROUP_USER, 0);
    user_vmm_buf.mem_type = VMM_MEM_TYPE_SYS_ADAPT;
    user_vmm_buf.sec_flag = chan->sec_flag;

    for (index = 0; index < USER_MAX_FS_NUM; index++) {
        ret = vmm_send_command(VMM_CMD_ALLOC, VMM_CMD_PRIO_MIN, &user_vmm_buf);
        if (ret != VMM_SUCCESS) {
            dprint(PRN_ERROR, "VMM_CMD_ALLOC %s size %d failed!\n", user_vmm_buf.buf_name, user_vmm_buf.size);
        }
    }

    chan->img_slot_len = img_slot_len;

    return HI_SUCCESS;
}

static hi_void user_free_frame_buffer(user_ctx *chan)
{
    hi_s32 ret;
    hi_u64 unid;
    hi_s32 index;
    vmm_buffer user_vmm_buf ;

    unid = SET_CHAN_UID((hi_u32)chan->unid, (hi_u32)chan->chan_id);

    VFMW_CHECK_SEC_FUNC(memset_s(&user_vmm_buf, sizeof(vmm_buffer), 0, sizeof(vmm_buffer)));

    if (snprintf_s(user_vmm_buf.buf_name, sizeof(user_vmm_buf.buf_name),
                   sizeof(user_vmm_buf.buf_name), "%s", "MjpegFrmBuf") < 0) {
        dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
    }
    user_vmm_buf.buf_name[sizeof(user_vmm_buf.buf_name) - 1] = '\0';
    user_vmm_buf.map = 0;
    user_vmm_buf.cache = 0;
    user_vmm_buf.priv_id = SET_MEM_UID(unid, VFMW_MEM_GROUP_USER, 0);
    user_vmm_buf.mem_type = VMM_MEM_TYPE_SYS_ADAPT;
    user_vmm_buf.sec_flag = chan->sec_flag;

    for (index = 0; index < USER_MAX_FS_NUM; index++) {
        if (chan->frm[index].st_mem.length == 0) {
            continue;
        }

        user_vmm_buf.size = chan->frm[index].st_mem.length;
        user_vmm_buf.start_vir_addr = chan->frm[index].st_mem.vir_addr;
        user_vmm_buf.start_phy_addr = chan->frm[index].st_mem.phy_addr;
        user_vmm_buf.dma_buf = chan->frm[index].st_mem.dma_buf;

        ret = vmm_send_command(VMM_CMD_RELEASE, VMM_CMD_PRIO_MIN, &user_vmm_buf);
        if (ret != VMM_SUCCESS) {
            dprint(PRN_ERROR, "VMM_CMD_RELEASE %s size %d failed!\n", user_vmm_buf.buf_name, user_vmm_buf.size);
        }

        if (chan->frm_alloc_done_cnt > 0) {
            chan->frm_alloc_done_cnt--;
        }
        chan->frm[index].state = FRM_NULL;
    }

    return;
}

static user_frame *user_get_idle_frame(user_ctx *chan)
{
    hi_s32 i;

    for (i = 0; i < USER_MAX_FS_NUM; i++) {
        if (chan->frm[i].state == FRM_IDLE &&
            chan->frm[i].st_mem.length != 0) {
            chan->frm[i].state = FRM_GET;
            break;
        }
    }

    if (i >= USER_MAX_FS_NUM) {
        return HI_NULL;
    }

    return &chan->frm[i];
}

static user_frame *user_find_match_frame(user_ctx *chan, vfmw_usrdec_frame_desc *user_frm)
{
    hi_s32 index = 0;
    user_frame *frame = HI_NULL;

    for (index = 0; index < USER_MAX_FS_NUM; index++) {
        frame = &chan->frm[index];
        if (frame->state == FRM_GET &&
            user_frm->phy_addr == frame->st_mem.phy_addr) {
            frame->state = FRM_PUT;
            frame->order = ++chan->index;
            break;
        }
    }

    if (index >= USER_MAX_FS_NUM) {
        dprint(PRN_ERROR, "no match user frame.\n");
        return HI_NULL;
    }

    if (user_frm->is_frame_valid == HI_FALSE) {
        frame->state = FRM_IDLE;
        frame->order = 0;
    } else {
        /* resolve overflow */
        if (chan->index > USER_INDEX_OVERFLOW_NUM) {
            frame->order %= USER_INDEX_OVERFLOW_NUM;
            chan->index %= USER_INDEX_OVERFLOW_NUM;
        }
    }

    return frame;
}

static hi_void user_try_report_eos(hi_s32 chan_id)
{
    hi_bool no_frame;
    user_ctx *chan = g_user_chan_ctx[chan_id];

    VFMW_ASSERT(chan != HI_NULL);

    no_frame = user_is_frame_list_empty(chan);
    if (no_frame == HI_TRUE && chan->report_eos == HI_FALSE) {
        hi_u32 last_id_plus2 = VFMW_LAST_IMG_ID(chan->last_image_id);
        user_event_report(chan_id, EVNT_LAST_FRAME, &last_id_plus2, sizeof(hi_u32));
        chan->report_eos = HI_TRUE;
    }
}

static hi_s32 user_get_dec_stream(hi_s32 chan_id, hi_void *args, hi_u32 len)
{
    hi_s32 ret;

    VFMW_ASSERT_RET(args != HI_NULL, HI_FAILURE);

    ret = stream_read_buffer(chan_id, (vfmw_stream_buf *)args);
    if (ret == STREAM_EOS) {
        user_try_report_eos(chan_id);
        ret = HI_FAILURE;
    }

    return ret;
}

static hi_s32 user_put_dec_stream(hi_s32 chan_id, hi_void *args, hi_u32 len)
{
    VFMW_ASSERT_RET(args != HI_NULL, HI_FAILURE);

    return stream_release_buffer(chan_id, (vfmw_stream_buf *)args);
}

static hi_s32 user_get_dec_frame(hi_s32 chan_id, hi_void *args, hi_u32 len)
{
    hi_s32 ret;
    hi_u32 offset = 0;
    user_frame *frm = HI_NULL;
    user_ctx *chan = g_user_chan_ctx[chan_id];
    vfmw_usrdec_frame_desc *user_frm = (vfmw_usrdec_frame_desc *)args;

    VFMW_ASSERT_RET(args != HI_NULL, HI_FAILURE);
    VFMW_ASSERT_RET(chan != HI_NULL, HI_FAILURE);

    if (chan->img_slot_len == 0) {
        ret = user_alloc_frame_buffer(chan, user_frm->mem_size);
        if (ret != HI_SUCCESS) {
            dprint(PRN_ERROR, "user alloc frame buffer size %d failed.\n", user_frm->mem_size);
            return HI_FAILURE;
        }
    } else if (chan->img_slot_len < user_frm->mem_size) {
        dprint(PRN_ERROR, "user not support resolutian change, slot len %d < frame size %d.\n",
               chan->img_slot_len, user_frm->mem_size);
        return HI_FAILURE;
    }

    frm = user_get_idle_frame(chan);
    if (frm == HI_NULL) {
        return HI_FAILURE;
    }

    user_frm->mem_handle = OS_GET_MEM_FD(UINT64_PTR(frm->st_mem.dma_buf));
    user_frm->phy_addr = frm->st_mem.phy_addr;
    offset += user_frm->mem_size;
#ifdef VFMW_MODULE_LOWDLY_SUPPORT
    user_frm->line_num_offset = offset;
    user_frm->line_num_size = USER_LOWDELAY_LINE_SIZE;
    offset += user_frm->line_num_size;
#endif
    user_frm->mem_size = frm->st_mem.length;

    if (offset > frm->st_mem.length) {
        dprint(PRN_ERROR, "user get frame failed: need size %d > slot size %d\n", offset, frm->st_mem.length);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 user_put_dec_frame(hi_s32 chan_id, hi_void *args, hi_u32 len)
{
    vfmw_image *img = HI_NULL;
    user_frame *frm = HI_NULL;
    user_ctx *chan = g_user_chan_ctx[chan_id];
    vfmw_usrdec_frame_desc *user_frm = (vfmw_usrdec_frame_desc *)args;

    VFMW_ASSERT_RET(args != HI_NULL, HI_FAILURE);
    VFMW_ASSERT_RET(chan != HI_NULL, HI_FAILURE);

    frm = user_find_match_frame(chan, user_frm);
    if (frm == HI_NULL) {
        return HI_FAILURE;
    }

    img = &(frm->image);

    VFMW_CHECK_SEC_FUNC(memset_s(img, sizeof(vfmw_image), 0, sizeof(vfmw_image)));

    OS_PUT_MEM_FD(user_frm->mem_handle);

    img->is_user_dec = HI_TRUE;
    img->vid_std = chan->cfg.vid_std;
    img->image_id = frm->order;
    img->image_id_1 = -1;
    img->src_pts = user_frm->pts;
    img->pts = user_frm->pts;
    img->user_tag = user_frm->pts;
    img->disp_time = user_frm->pts;
    img->disp_enable_flag = 1;
    img->image_width = user_frm->width;
    img->image_height = user_frm->height;
    img->bit_depth = user_frm->bit_depth;
    img->yuv_format = user_frm->fmt;

    img->disp_info.frm_dma_buf = frm->st_mem.dma_buf;
    img->disp_info.linear_en = 1;
    img->disp_info.disp_width = user_frm->width;
    img->disp_info.disp_height = user_frm->height;
    img->disp_info.y_stride = user_frm->luma_stride;
    img->disp_info.uv_stride = user_frm->chrom_stride;
    img->disp_info.disp_center_x = img->disp_info.disp_width / 2;   /* 2: disp center in middle */
    img->disp_info.disp_center_y = img->disp_info.disp_height / 2;  /* 2: disp center in middle */
    img->disp_info.luma_phy_addr = user_frm->phy_addr;
    img->disp_info.chrom_phy_addr = user_frm->phy_addr + user_frm->chrom_offset;
    img->disp_info.linear_en = 1;

    img->format.frame_type = 0;
    img->format.sample_type = 0;
    img->format.video_format = 1;
    img->format.source_format = 0;
    img->format.field_valid_flag = 3; /* 3: both fields valid */
    img->format.top_field_first = 1;
#ifdef VFMW_MODULE_LOWDLY_SUPPORT
    img->line_num_phy_addr = user_frm->phy_addr + user_frm->line_num_offset;
#endif

    user_event_report(chan_id, EVNT_NEW_IMAGE, (hi_void *)img, sizeof(vfmw_image));

    return HI_SUCCESS;
}

static hi_s32 user_acquire_frame(hi_s32 chan_id, vfmw_image *image_info)
{
    hi_s32 i = 0;
    hi_s32 index = USER_INDEX_OVERFLOW_NUM;
    hi_s32 order = USER_INDEX_OVERFLOW_NUM;
    hi_s32 order_than_half_flag = 0;
    hi_s32 half_index = (USER_INDEX_OVERFLOW_NUM + 1) / 2; /* 1: add 1 num; 2: use half num */
    hi_u64 out_frame_addr;
    user_ctx *chan = g_user_chan_ctx[chan_id];

    VFMW_ASSERT_RET(chan != HI_NULL, HI_FAILURE);
    VFMW_ASSERT_RET(image_info != HI_NULL, HI_FAILURE);

    /* Get min order one, it is the first output image */
    for (i = 0; i < USER_MAX_FS_NUM; i++) {
        if (chan->frm[i].state == FRM_PUT &&
            chan->frm[i].order >= half_index) {
            order_than_half_flag = 1;
            break;
        }
    }

    if (order_than_half_flag) {
        for (i = 0; i < USER_MAX_FS_NUM; i++) {
            if (chan->frm[i].state == FRM_PUT &&
                chan->frm[i].order <= order &&
                chan->frm[i].order >= half_index) {
                order = chan->frm[i].order;
                index = i;
            }
        }
    } else {
        for (i = 0; i < USER_MAX_FS_NUM; i++) {
            if (chan->frm[i].state == FRM_PUT &&
                chan->frm[i].order <= order) {
                order = chan->frm[i].order;
                index = i;
            }
        }
    }

    if (index >= USER_MAX_FS_NUM) {
        return HI_FAILURE;
    }

    chan->frm[index].state = FRM_ACQ;
    out_frame_addr = image_info->frame_ptr;
    VFMW_CHECK_SEC_FUNC(memcpy_s(image_info, sizeof(vfmw_image), &(chan->frm[index].image), sizeof(vfmw_image)));
    image_info->frame_ptr = out_frame_addr;
    chan->last_image_id = image_info->image_id;

    return HI_SUCCESS;
}

static hi_s32 user_release_frame(hi_s32 chan_id, const vfmw_image *image_info)
{
    hi_s32 i = 0;
    user_ctx *chan = g_user_chan_ctx[chan_id];

    VFMW_ASSERT_RET(chan != HI_NULL, HI_FAILURE);
    VFMW_ASSERT_RET(image_info != HI_NULL, HI_FAILURE);

    for (i = 0; i < USER_MAX_FS_NUM; i++) {
        if (chan->frm[i].order == image_info->image_id &&
            chan->frm[i].state == FRM_ACQ) {
            chan->frm[i].order = 0;
            chan->frm[i].state = FRM_IDLE;
            break;
        }
    }

    if (i >= USER_MAX_FS_NUM) {
        dprint(PRN_ERROR, "%s not found image %d\n", __func__, image_info->image_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 user_create_chan(hi_s32 chan_id, hi_void *args, hi_u32 len)
{
    user_ctx *chan = HI_NULL;
    vfmw_chan_option *user_opt = HI_NULL;

    VFMW_ASSERT_RET(args != HI_NULL, HI_FAILURE);

    user_opt = (vfmw_chan_option *)args;

    if (g_user_chan_ctx[chan_id] != HI_NULL) {
        DPRINT_WARN("%s chan is not null\n", __func__);
        return HI_FAILURE;
    }

    g_user_chan_ctx[chan_id] = OS_ALLOC_VIR("USER_Inst", sizeof(user_ctx));
    chan = g_user_chan_ctx[chan_id];
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "%s chan OS_ALLOC_VIR_MEM fail\n", __func__);
        return HI_FAILURE;
    }
    VFMW_CHECK_SEC_FUNC(memset_s(chan, sizeof(user_ctx), 0, sizeof(user_ctx)));

    chan->chan_id = chan_id;
    chan->adpt_type = user_opt->adpt_type;
    chan->unid = OS_GET_UNID();

    VFMW_ASSERT_RET(USER_INDEX_OVERFLOW_NUM > 2 * USER_MAX_FS_NUM, HI_FAILURE); /* 2: twice fs num */

    chan->sec_flag = user_opt->is_sec_mode;

    user_opt->chan_id = chan_id;

    return HI_SUCCESS;
}

static hi_s32 user_get_config(hi_s32 chan_id, hi_void *args, hi_u32 len)
{
    user_ctx *chan = g_user_chan_ctx[chan_id];
    vfmw_chan_cfg *cfg = (vfmw_chan_cfg *)args;

    VFMW_ASSERT_RET(chan != HI_NULL, HI_FAILURE);
    VFMW_ASSERT_RET(cfg != HI_NULL, HI_FAILURE);

    VFMW_CHECK_SEC_FUNC(memcpy_s(cfg, sizeof(vfmw_chan_cfg), &chan->cfg, sizeof(vfmw_chan_cfg)));

    return HI_SUCCESS;
}

static hi_s32 user_set_config(hi_s32 chan_id, hi_void *args, hi_u32 len)
{
    user_ctx *chan = g_user_chan_ctx[chan_id];
    vfmw_chan_cfg *cfg = (vfmw_chan_cfg *)args;

    VFMW_ASSERT_RET(chan != HI_NULL, HI_FAILURE);
    VFMW_ASSERT_RET(cfg != HI_NULL, HI_FAILURE);

    VFMW_CHECK_SEC_FUNC(memcpy_s(&chan->cfg, sizeof(vfmw_chan_cfg), cfg, sizeof(vfmw_chan_cfg)));

    return HI_SUCCESS;
}

static hi_s32 user_destroy_chan(hi_s32 chan_id, hi_void *args, hi_u32 len)
{
    user_ctx *chan = g_user_chan_ctx[chan_id];

    VFMW_ASSERT_RET(chan != HI_NULL, HI_FAILURE);

    user_free_frame_buffer(chan);

    OS_FREE_VIR(chan);
    g_user_chan_ctx[chan_id] = HI_NULL;

    return HI_SUCCESS;
}

static hi_s32 user_bind_req_mem(hi_s32 chan_id, hi_void *args, hi_u32 len)
{
    user_ctx *chan = g_user_chan_ctx[chan_id];
    hi_s32 index = 0;
    vfmw_mem_report *user_mem = HI_NULL;
    hi_u64 unique_id_tmp;
    hi_u64 unid;

    VFMW_ASSERT_RET(args != HI_NULL, HI_FAILURE);
    VFMW_ASSERT_RET(chan != HI_NULL, HI_FAILURE);

    user_mem = (vfmw_mem_report *)args;

    unique_id_tmp = (hi_u64)(user_mem->unique_id);

    unid = SET_CHAN_UID((hi_u32)chan->unid, (hi_u32)chan->chan_id);
    if (unid != GET_CHAN_UID(unique_id_tmp)) {
        dprint(PRN_ERROR, "%s Fs Chan ID %llx != Chan ID %llx\n", __func__, GET_CHAN_UID(unique_id_tmp), unid);
        return HI_FAILURE;
    }

    for (index = 0; index < USER_MAX_FS_NUM; index++) {
        if (chan->frm[index].st_mem.length == 0) {
            chan->frm[index].st_mem.length = user_mem->size;
            chan->frm[index].st_mem.phy_addr = user_mem->phy_addr;
            chan->frm[index].st_mem.vir_addr = user_mem->vir_addr;
            chan->frm[index].st_mem.dma_buf = user_mem->dma_buf;
            chan->frm[index].state = FRM_IDLE;
            chan->frm_alloc_done_cnt++;
            break;
        }
    }

    if (index == USER_MAX_FS_NUM) {
        dprint(PRN_ERROR, "user frm has something wrong!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 user_flush_chan(hi_s32 chan_id, hi_void *args, hi_u32 len)
{
    hi_s32 i = 0;
    vfmw_flush_type flush_type;
    user_ctx *chan = g_user_chan_ctx[chan_id];

    VFMW_ASSERT_RET(args != HI_NULL, HI_FAILURE);
    VFMW_ASSERT_RET(chan != HI_NULL, HI_FAILURE);

    flush_type = *((vfmw_flush_type *)args);

    if (flush_type == VFMW_FLUSH_OUT || flush_type == VFMW_FLUSH_ALL) {
        for (i = 0; i < USER_MAX_FS_NUM; i++) {
            if (chan->frm[i].state == FRM_GET || chan->frm[i].state == FRM_PUT) {
                chan->frm[i].state = FRM_IDLE;
                chan->frm[i].order = 0;
            }
        }
    }

    dprint(PRN_ERROR, "user flush chan %d\n", chan_id);

    return HI_SUCCESS;
}

static user_case g_user_case_table[] = {
    {VFMW_CID_CREATE_CHAN,       user_create_chan,    VFMW_CREATE_OPT_SIZE},
    {VFMW_CID_GET_CHAN_CFG,      user_get_config,     sizeof(vfmw_chan_cfg)},
    {VFMW_CID_CFG_CHAN,          user_set_config,     sizeof(vfmw_chan_cfg)},
    {VFMW_CID_DESTROY_CHAN,      user_destroy_chan,   0},
    {VFMW_CID_BIND_MEM,          user_bind_req_mem,   sizeof(vfmw_mem_report)},
    {VFMW_CID_GET_USRDEC_STREAM, user_get_dec_stream, sizeof(vfmw_stream_buf)},
    {VFMW_CID_PUT_USRDEC_STREAM, user_put_dec_stream, sizeof(vfmw_stream_buf)},
    {VFMW_CID_GET_USRDEC_FRAME,  user_get_dec_frame,  sizeof(vfmw_usrdec_frame_desc)},
    {VFMW_CID_PUT_USRDEC_FRAME,  user_put_dec_frame,  sizeof(vfmw_usrdec_frame_desc)},
    {VFMW_CID_FLUSH_CHAN,        user_flush_chan,     sizeof(vfmw_flush_type)},
    {VFMW_CID_MAX,               HI_NULL,             0},
};

hi_s32 user_open(hi_void)
{
    user_global_ctx *ctx = &g_user_ctx;

    VFMW_CHECK_SEC_FUNC(memset_s(ctx, sizeof(user_global_ctx), 0, sizeof(user_global_ctx)));
    OS_SEMA_INIT(&ctx->sema);

    return HI_SUCCESS;
}

hi_s32 user_close(hi_void)
{
    user_global_ctx *ctx = &g_user_ctx;

    OS_SEMA_EXIT(ctx->sema);

    return HI_SUCCESS;
}

hi_s32 user_init(hi_void *args, hi_u32 len)
{
    hi_s32 ret;
    vfmw_init_param *param = (vfmw_init_param *)args;

    OS_SEMA_DOWN(g_user_ctx.sema);

    ret = user_set_callback(param->adpt_type, param->fn_callback);

    OS_SEMA_UP(g_user_ctx.sema);

    return ret;
}

hi_s32 user_control(hi_s32 chan_id, hi_s32 cmd, hi_void *args, hi_u32 len)
{
    hi_s32 ret;
    hi_u32 index;
    hi_u32 case_num;
    user_case *ctr_case = HI_NULL;
    user_handler user_target_handler = HI_NULL;

    case_num = sizeof(g_user_case_table) / sizeof(user_case);
    for (index = 0; index < case_num; index++) {
        ctr_case = &g_user_case_table[index];
        if (cmd == ctr_case->cmd_id) {
            if (ctr_case->length > len) {
                dprint(PRN_ERROR, "%s cmd 0x%x param len invalid %d > %d\n", __func__, cmd, ctr_case->length, len);
                return HI_FAILURE;
            }
            user_target_handler = ctr_case->handler;
            break;
        }
    }

    if (user_target_handler == HI_NULL) {
        return HI_SUCCESS;
    }

    OS_SEMA_DOWN(g_user_ctx.sema);
    ret = user_target_handler(chan_id, args, len);
    OS_SEMA_UP(g_user_ctx.sema);

    return ret;
}

hi_s32 user_get_image(hi_s32 chan_id, hi_void *user_image)
{
    hi_s32 ret;

    OS_SEMA_DOWN(g_user_ctx.sema);

    ret = user_acquire_frame(chan_id, (vfmw_image *)user_image);

    OS_SEMA_UP(g_user_ctx.sema);

    return ret;
}

hi_s32 user_release_image(hi_s32 chan_id, const hi_void *user_image)
{
    hi_s32 ret;

    OS_SEMA_DOWN(g_user_ctx.sema);

    ret = user_release_frame(chan_id, (vfmw_image *)user_image);

    OS_SEMA_UP(g_user_ctx.sema);

    return ret;
}

hi_s32 user_info_read(hi_void *buf, hi_s32 chan_id)
{
#ifdef VFMW_PROC_SUPPORT
    hi_s32 ret;

    OS_SEMA_DOWN(g_user_ctx.sema);
    ret = user_read_proc(buf, chan_id);
    OS_SEMA_UP(g_user_ctx.sema);

    return ret;
#else

    return HI_SUCCESS;
#endif
}

hi_s32 user_info_write(cmd_str_ptr buffer, hi_u32 count)
{
#ifdef VFMW_PROC_SUPPORT
    OS_SEMA_DOWN(g_user_ctx.sema);
    user_write_proc(buffer, count);
    OS_SEMA_UP(g_user_ctx.sema);
#endif

    return HI_SUCCESS;
}


