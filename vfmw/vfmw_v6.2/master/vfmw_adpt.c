/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "vfmw_intf.h"
#include "vfmw_adpt.h"
#include "dbg.h"
#include "vfmw_pdt.h"
#ifdef VFMW_PROC_SUPPORT
#include "vfmw_adpt_proc.h"
#endif
#ifdef VFMW_VMM_SUPPORT
#include "vmm_ext.h"
#endif
#ifdef VFMW_CRC_SUPPORT
#include "crc_ext.h"
#endif
#ifdef IPT_SUPPORT
#include "ip_test.h"
#endif
#include "usrdata.h"
#include "vfmw_img.h"
#include "pts_ext.h"
#ifdef VFMW_LOCAL_SUPPORT
#include "local.h"
#endif
#ifdef VFMW_MDC_SUPPORT
#include "mdc_client.h"
#endif
#ifdef VFMW_TEE_SUPPORT
#include "tee_client.h"
#endif
#ifdef VFMW_USER_SUPPORT
#include "userdec.h"
#endif
#ifdef VFMW_VPP_SUPPORT
#include "vpp.h"
#endif
#ifdef VFMW_STREAM_SUPPORT
#include "stream.h"
#endif
#ifdef VFMW_FENCE_SUPPORT
#include "fence.h"
#endif
#ifdef VFMW_USD_POOL_SUPPORT
#include "usdpool.h"
#endif


#define VFMW_CHECK 0x1234abcd

static vfmw_entry g_s_vfmw_entry = {
    .reserve1 = VFMW_CHECK,
    .state = VFMW_STATE_NULL,
    .reserve2 = VFMW_CHECK,
    .dflt.save_begin = 0,
    .dflt.save_end = 0xffffffff,
};

static hi_bool g_s_vfmw_load = HI_FALSE;

vfmw_entry *vadpt_entry(hi_void)
{
    return &g_s_vfmw_entry;
}

vfmw_dflt *vadpt_get_dflt(hi_void)
{
    return &g_s_vfmw_entry.dflt;
}

vfmw_chan *vadpt_chan(hi_s32 chan_id)
{
    if (chan_id < 0 || chan_id >= VFMW_CHAN_NUM) {
        return HI_NULL;
    }

    return g_s_vfmw_entry.chan + chan_id;
}

static hi_void vadpt_sema_down(hi_void)
{
    OS_SEMA_DOWN(g_s_vfmw_entry.sema);
}

static hi_void vadpt_sema_up(hi_void)
{
    OS_SEMA_UP(g_s_vfmw_entry.sema);
}

static hi_void vadpt_set_load(hi_bool val)
{
    g_s_vfmw_load = val;
}

static hi_bool vadpt_get_load(hi_void)
{
    return g_s_vfmw_load;
}

#ifdef VFMW_VMM_SUPPORT
static hi_s32 vadpt_vmm_cb_alloc(hi_void *param_out, hi_s32 ret_val)
{
    hi_s32 ret = HI_FAILURE;
    hi_s32 chan_id;
    vmm_buffer *vmm_buf = HI_NULL;
    vfmw_mem_report mem_report;

    VFMW_ASSERT_RET(param_out != HI_NULL, VMM_FAILURE);

    vmm_buf = (vmm_buffer *)param_out;

    if (ret_val != VMM_SUCCESS) {
        dprint(PRN_ERROR, "%s name %s failed with ret_val %x\n", __func__, vmm_buf->buf_name, ret_val);
        return VMM_FAILURE;
    }

    VFMW_CHECK_SEC_FUNC(memset_s(&mem_report, sizeof(vfmw_mem_report), 0, sizeof(vfmw_mem_report)));

    if (snprintf_s(mem_report.buf_name, VFMW_REPORT_MAX_NAME_LEN,
                   VFMW_REPORT_MAX_NAME_LEN, "%s", vmm_buf->buf_name) < 0) {
        dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
    }
    mem_report.buf_name[sizeof(mem_report.buf_name) - 1] = '\0';
    mem_report.dma_buf = vmm_buf->dma_buf;
    mem_report.is_map = vmm_buf->map;
    mem_report.is_cache = vmm_buf->cache;
    mem_report.size = vmm_buf->size;
    mem_report.unique_id = vmm_buf->priv_id;
    mem_report.phy_addr = vmm_buf->start_phy_addr;
    mem_report.vir_addr = vmm_buf->start_vir_addr;
    mem_report.sec_info = vmm_buf->sec_info;

    chan_id = (hi_s32)GET_CHAN_ID_BY_UID(mem_report.unique_id);

    ret = vfmw_control(chan_id, VFMW_CID_BIND_MEM, &mem_report, sizeof(vfmw_mem_report));

    return (ret == HI_SUCCESS) ? VMM_SUCCESS : VMM_FAILURE;
}

static hi_s32 vadpt_vmm_cb_release(hi_void *param_out, hi_s32 ret_val)
{
    vmm_buffer *vmm_buf = HI_NULL;

    VFMW_ASSERT_RET(param_out != HI_NULL, VMM_FAILURE);

    vmm_buf = (vmm_buffer *)param_out;

    if (ret_val != VMM_SUCCESS) {
        dprint(PRN_ERROR, "%s name %s failed with ret_val %x\n", __func__, vmm_buf->buf_name, ret_val);
        return VMM_FAILURE;
    }

    return VMM_SUCCESS;
}

static hi_s32 vadpt_vmm_cb_release_burst(hi_void *param_out, hi_s32 ret_val)
{
    hi_s64 *ppriv_id = HI_NULL;

    VFMW_ASSERT_RET(param_out != HI_NULL, VMM_FAILURE);

    ppriv_id = (hi_s64 *)param_out;

    if (ret_val != VMM_SUCCESS) {
        dprint(PRN_ERROR, "%s priv_id 0x%lld failed with ret_val %x\n", __func__, ppriv_id, ret_val);
        return VMM_FAILURE;
    }

    return VMM_SUCCESS;
}

hi_s32 vadpt_vmm_call_back(vmm_cmd_type cmd_id, hi_void *param_out, hi_s32 ret_val)
{
    hi_s32 ret = VMM_FAILURE;

    switch (cmd_id) {
        case VMM_CMD_ALLOC:
            ret = vadpt_vmm_cb_alloc(param_out, ret_val);
            break;

        case VMM_CMD_RELEASE:
            ret = vadpt_vmm_cb_release(param_out, ret_val);
            break;

        case VMM_CMD_RELEASE_BURST:
            ret = vadpt_vmm_cb_release_burst(param_out, ret_val);
            break;

        default:
            dprint(PRN_ERROR, "%s unsupport cmd_id %d\n", __func__, cmd_id);
            break;
    }

    return ret;
}

static hi_s32 vadpt_vmm_set_image_out(vfmw_image *image)
{
    hi_s32 ret;
    vmm_attr tmp_location = { 0 };

    tmp_location.location = VMM_LOCATION_OUTSIDE;
    tmp_location.phy_addr = image->disp_info.luma_phy_addr;
    ret = vmm_send_command_block(VMM_CMD_BLK_SET_LOCATION, &tmp_location);
    if (ret != VMM_SUCCESS) {
        dprint(PRN_ERROR, "Set image phy 0x%x location %d failed.\n", tmp_location.phy_addr, tmp_location.location);
        goto error_exit_0;
    }

    if (image->hdr_input.hdr_metadata_phy_addr != 0) {
        tmp_location.location = VMM_LOCATION_OUTSIDE;
        tmp_location.phy_addr = image->hdr_input.hdr_metadata_phy_addr;
        ret = vmm_send_command_block(VMM_CMD_BLK_SET_LOCATION, &tmp_location);
        if (ret != VMM_SUCCESS) {
            dprint(PRN_ERROR, "Set image metadata phy 0x%x location %d failed.\n", tmp_location.phy_addr,
                   tmp_location.location);
            goto error_exit_1;
        }
    }

    if (image->image_id_1 != -1 && image->disp_info.luma_phy_addr_1 != 0) {
        tmp_location.location = VMM_LOCATION_OUTSIDE;
        tmp_location.phy_addr = image->disp_info.luma_phy_addr_1;
        ret = vmm_send_command_block(VMM_CMD_BLK_SET_LOCATION, &tmp_location);
        if (ret != VMM_SUCCESS) {
            dprint(PRN_ERROR, "Set image view 1 phy 0x%x location %d failed.\n",
                tmp_location.phy_addr, tmp_location.location);
            goto error_exit_2;
        }
    }

    return HI_SUCCESS;

error_exit_2:
    if (image->hdr_input.hdr_metadata_phy_addr != 0) {
        tmp_location.location = VMM_LOCATION_INSIDE;
        tmp_location.phy_addr = image->hdr_input.hdr_metadata_phy_addr;
        (hi_void)vmm_send_command_block(VMM_CMD_BLK_SET_LOCATION, &tmp_location);
    }

error_exit_1:
    tmp_location.location = VMM_LOCATION_INSIDE;
    tmp_location.phy_addr = image->disp_info.luma_phy_addr;
    (hi_void)vmm_send_command_block(VMM_CMD_BLK_SET_LOCATION, &tmp_location);

error_exit_0:

    return HI_FAILURE;
}

static hi_s32 vadpt_vmm_set_image_in(const vfmw_image *image)
{
    hi_s32 ret = HI_FAILURE;
    vmm_attr tmp_location = { 0 };

    if (image->disp_info.luma_phy_addr != 0) {
        tmp_location.location = VMM_LOCATION_INSIDE;
        tmp_location.phy_addr = image->disp_info.luma_phy_addr;
        ret = vmm_send_command_block(VMM_CMD_BLK_SET_LOCATION, &tmp_location);
        if (ret != VMM_SUCCESS) {
            dprint(PRN_ERROR, "Set image phy 0x%x location %d failed.\n", tmp_location.phy_addr, tmp_location.location);
            return HI_FAILURE;
        }
    }

    if (image->hdr_input.hdr_metadata_phy_addr != 0) {
        tmp_location.location = VMM_LOCATION_INSIDE;
        tmp_location.phy_addr = image->hdr_input.hdr_metadata_phy_addr;
        ret = vmm_send_command_block(VMM_CMD_BLK_SET_LOCATION, &tmp_location);
        if (ret != VMM_SUCCESS) {
            dprint(PRN_ERROR, "Set image metadata phy 0x%x location %d failed.\n", tmp_location.phy_addr,
                   tmp_location.location);
            return HI_FAILURE;
        }
    }

    if (image->image_id_1 != -1 && image->disp_info.luma_phy_addr_1 != 0) {
        tmp_location.location = VMM_LOCATION_INSIDE;
        tmp_location.phy_addr = image->disp_info.luma_phy_addr_1;
        ret = vmm_send_command_block(VMM_CMD_BLK_SET_LOCATION, &tmp_location);
        if (ret != VMM_SUCCESS) {
            dprint(PRN_ERROR, "Set image view 1 phy 0x%x location %d failed.\n",
                tmp_location.phy_addr, tmp_location.location);
            return HI_FAILURE;
        }
    }

    return (ret == VMM_SUCCESS) ? HI_SUCCESS : HI_FAILURE;
}
#endif

static hi_s32 vadpt_is_init_sec_vfmw(hi_void *args)
{
    if (args != HI_NULL && ((vfmw_init_param *)args)->is_secure) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

static vfmw_type vadpt_get_vfmw_type(hi_s32 chan_id, vfmw_cid cmd_id, hi_void *args)
{
    vfmw_type type;
    vfmw_chan *chan = HI_NULL;
    hi_s32 sec_flag = 0;
    hi_s32 user_flag = 0;

#ifdef VFMW_MDC_SUPPORT
    type = VFMW_TYPE_MCU;
#else
    type = VFMW_TYPE_LOCAL;
#endif

    switch (cmd_id) {
        case VFMW_CID_CREATE_CHAN:
        case VFMW_CID_GET_CHAN_MEMSIZE:
            if (args != HI_NULL) {
                vfmw_chan_option *opt;
                opt = (vfmw_chan_option *)args;

                sec_flag = opt->is_sec_mode;
                user_flag = (opt->is_user_dec == HI_TRUE);
            }

#ifndef VFMW_MDC_SUPPORT
            if (sec_flag) {
                type = VFMW_TYPE_TEE;
            }
#endif
            if (user_flag) {
                type = VFMW_TYPE_USER;
            }

            if (cmd_id == VFMW_CID_CREATE_CHAN) {
                chan = vadpt_chan(chan_id);
                if (chan != HI_NULL) {
                    chan->type = type;
                    chan->is_sec = (sec_flag == 0) ? HI_FALSE : HI_TRUE;
                }
            }
            break;

        case VFMW_CID_ALLOC_SEC_BUF:  /* use default type */
        case VFMW_CID_FREE_SEC_BUF:
        case VFMW_CID_GET_CAPABILITY:
            break;

        default:
            chan = vadpt_chan(chan_id);
            if (chan != HI_NULL) {
                type = chan->type;
            }
            break;
    }

    return type;
}

static hi_bool vadpt_is_chan_valid(hi_s32 chan_id, vfmw_cid cmd_id)
{
    if (cmd_id == VFMW_CID_GET_CAPABILITY ||
        cmd_id == VFMW_CID_GET_CHAN_MEMSIZE ||
        cmd_id == VFMW_CID_CREATE_CHAN ||
        cmd_id == VFMW_CID_ALLOC_SEC_BUF ||
        cmd_id == VFMW_CID_FREE_SEC_BUF) {
        return HI_TRUE; /* not care chan id in these cmds */
    }

    if (chan_id >= 0 && chan_id < VFMW_CHAN_NUM && vadpt_chan(chan_id) != HI_NULL) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

static hi_s32 vadpt_new_chan_id(hi_void)
{
    hi_s32 id;
    vfmw_chan *chan = HI_NULL;

    for (id = 0; id < VFMW_CHAN_NUM; id++) {
        chan = vadpt_chan(id);
        if (chan != HI_NULL && chan->state == VFMW_CHAN_STATE_NULL) {
            break;
        }
    }

    return (id >= VFMW_CHAN_NUM) ? -1 : id;
}

static hi_s32 vadpt_get_chan_id(vfmw_cid cmd, hi_s32 *chan_id)
{
    vfmw_chan *chan = HI_NULL;

    switch (cmd) {
        case VFMW_CID_GET_CAPABILITY:
        case VFMW_CID_GET_CHAN_MEMSIZE:
        case VFMW_CID_ALLOC_SEC_BUF:
        case VFMW_CID_FREE_SEC_BUF:
            *chan_id = 0;
            break;

        case VFMW_CID_CREATE_CHAN:
            /* for bvt specify chanid */
            chan = vadpt_chan(*chan_id);
            if (chan != HI_NULL && chan->state == VFMW_CHAN_STATE_NULL) {
                chan->state = VFMW_CHAN_STATE_WAIT;
                return HI_SUCCESS;
            }

            *chan_id = vadpt_new_chan_id();
            VFMW_ASSERT_RET(*chan_id != -1, HI_FAILURE);
            chan = vadpt_chan(*chan_id);
            chan->state = VFMW_CHAN_STATE_WAIT;
            break;
        default:
            break;
    }

    return HI_SUCCESS;
}

static hi_s32 vadpt_reset_hardware(hi_void)
{
    hi_s32 id;
    vfmw_entry *entry = HI_NULL;
    vfmw_chan *chan = HI_NULL;

    entry = vadpt_entry();
    if (entry->ref_cnt != 0) {
        return HI_SUCCESS;
    }

    for (id = 0; id < VFMW_CHAN_NUM; id++) {
        chan = vadpt_chan(id);
        if (chan != HI_NULL && chan->state != VFMW_CHAN_STATE_NULL) {
            return HI_SUCCESS;
        }
    }

    return pdt_set_attr(ATTR_RESET_VDH, HI_NULL);
}

static hi_s32 vadpt_queue_stream(hi_s32 chan_id, hi_void *args)
{
    hi_s32 ret;
    hi_u32 time;
    vfmw_chan *chan = HI_NULL;

    time = OS_GET_TIME_US();
    chan = vadpt_chan(chan_id);

    ret = stream_queue_buffer(chan_id, (vfmw_stream_buf *)args);
    if (chan != HI_NULL &&
        ret == HI_SUCCESS &&
        chan->kpi.questm.begin == 0) {
        chan->kpi.questm.begin = time;
        chan->kpi.questm.end = OS_GET_TIME_US();
        chan->kpi.questm.cost = chan->kpi.questm.end - chan->kpi.start.end;
    }

    return ret;
}

static hi_s32 vadpt_local_control(hi_s32 chan_id, vfmw_cid cmd_id, hi_void *args, hi_bool *is_local_case)
{
    hi_s32 ret = HI_FAILURE;
    pdt_license license;
    vfmw_chan_cfg *cfg = (vfmw_chan_cfg *)args;

    *is_local_case = HI_TRUE;

    switch (cmd_id) {
        case VFMW_CID_CFG_CHAN: {
            if (pdt_get_attr(ATTR_GET_LICENSE, &license) == HI_SUCCESS &&
                ((cfg->vid_std == VFMW_AV1 && license.av1_en == 0) ||
                 (cfg->vid_std == VFMW_AVS3 && license.avs3_en == 0))) {
                dprint(PRN_ERROR, "vfmw std %d not support\n", cfg->vid_std);
                ret = HI_FAILURE;
            } else {
                *is_local_case = HI_FALSE;
            }
            break;
        }
        case VFMW_CID_SET_FRAME_RATE_TYPE: {
            vfmw_pts_frmrate *pts_frm_rate = (vfmw_pts_frmrate *)args;
            ret = pts_info_set_frm_rate(chan_id, pts_frm_rate);
            break;
        }
        case VFMW_CID_GET_FRAME_RATE_TYPE: {
            vfmw_pts_frmrate *pts_frm_rate = (vfmw_pts_frmrate *)args;
            ret = pts_info_get_frm_rate(chan_id, pts_frm_rate);
            break;
        }
#ifdef VFMW_STREAM_SUPPORT
        case VFMW_CID_QUEUE_STREAM:
            ret = vadpt_queue_stream(chan_id, args);
            break;
        case VFMW_CID_DEQUEUE_STREAM:
            ret = stream_dequeue_buffer(chan_id, (vfmw_stream_buf *)args);
            break;
        case VFMW_CID_INIT_ES_BUF:
            ret = stream_alloc_buffer(chan_id, args);
            break;
        case VFMW_CID_DEINIT_ES_BUF:
            ret = stream_free_buffer(chan_id);
            break;
        case VFMW_CID_BIND_ES_USR_ADDR:
            ret = stream_bind_usr_addr(chan_id, *((hi_u64 *)args));
            break;
        case VFMW_CID_SEND_EOS:
            ret = stream_send_eos(chan_id);
            break;
        case VFMW_CID_SET_IDR_PTS:
            ret = stream_set_idr_pts(chan_id, *((hi_s64 *)args));
            break;
#endif
#ifdef VFMW_USD_POOL_SUPPORT
        case VFMW_CID_BIND_USD_BUF:
            ret = usrdat_bind_chan_mem(chan_id, args);
            break;
        case VFMW_CID_ACQ_USD:
            ret = usrdat_acquire(chan_id, args);
            break;
        case VFMW_CID_RLS_USD:
            ret = usrdat_release(chan_id, args);
            break;
        case VFMW_CID_CHK_USD:
            ret = usrdat_check(chan_id, args);
            break;
#endif
#ifdef VFMW_VPP_SUPPORT
        case VFMW_CID_SET_3D_TYPE:
            ret = vpp_set_frm_3d_type(chan_id, args);
            break;
        case VFMW_CID_GET_3D_TYPE:
            ret = vpp_get_frm_3d_type(chan_id, args);
            break;
#endif
        default: {
            *is_local_case = HI_FALSE;
            return HI_SUCCESS;
        }
    }

    return ret;
}


#ifdef VFMW_STREAM_SUPPORT
STATIC hi_s32 vadpt_read_stream(hi_s32 chan_id, vfmw_stream_data *packet)
{
    hi_s32 ret;
    vfmw_stream_buf buffer = {0};

    ret = stream_read_buffer(chan_id, &buffer);
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    }

    VFMW_CHECK_SEC_FUNC(memset_s(packet, sizeof(vfmw_stream_data), 0, sizeof(vfmw_stream_data)));

    if (ret == STREAM_EOS) {
        packet->is_stream_end_flag = 1;
        packet->is_not_last_packet_flag = 0;
        packet->pts = VFMW_INVALID_PTS;
    } else {
        packet->is_not_last_packet_flag = !buffer.is_end_of_frm;
    }

    packet->vir_addr = buffer.kern_vir_addr;
    packet->phy_addr = buffer.phy_addr;
    packet->pts = buffer.pts;
    packet->dv_dual_layer = buffer.is_dual_layer;
    packet->length = buffer.length;
    packet->disp_enable_flag = buffer.disp_enable_flag;
    packet->disp_frame_distance = buffer.disp_frm_distance;
    packet->disp_time = buffer.disp_time;
    packet->distance_before_first_frame = buffer.distance_before_first_frame;
    packet->gop_num = buffer.gop_num;
    packet->index = buffer.index;

    return HI_SUCCESS;
}

STATIC hi_s32 vadpt_release_stream(hi_s32 chan_id, vfmw_stream_data *packet)
{
    vfmw_stream_buf buffer = {0};

    if (packet->is_stream_end_flag == 1) {
        return HI_SUCCESS;
    }

    buffer.phy_addr = packet->phy_addr;
    buffer.kern_vir_addr = packet->vir_addr;
    buffer.length = packet->length;

    return stream_release_buffer(chan_id, &buffer);
}
#endif

static hi_void vadpt_bef_create_chan(hi_s32 chan_id, hi_void *args)
{
    vfmw_chan *chan;

    chan = vadpt_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
    }

    VFMW_CHECK_SEC_FUNC(memset_s(&chan->kpi, sizeof(vfmw_kpi), 0, sizeof(vfmw_kpi)));
    chan->kpi.create.begin = OS_GET_TIME_US();

    if (args != HI_NULL) {
        vfmw_chan_option *opt;
        opt = (vfmw_chan_option *)args;
        if (opt->seg_buf_size <= 0) {
            opt->seg_buf_size = pdt_get_attr(ATTR_GET_SEG_BUF_SIZE, &(opt->vid_std));
        }
    }

    return;
}

static hi_void vadpt_bef_config_chan(hi_s32 chan_id, hi_void *args)
{
    vfmw_chan *chan = HI_NULL;
    pdt_license license;
#ifdef VFMW_STREAM_SUPPORT
    vfmw_stream_intf *intf = HI_NULL;
#endif

    vfmw_chan_cfg *cfg = (vfmw_chan_cfg *)args;

    VFMW_ASSERT(cfg != HI_NULL);

    chan = vadpt_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
    }

    chan->kpi.config.begin = OS_GET_TIME_US();

    if (cfg->sample_frd_enable == HI_TRUE) {
        pts_info_enable_sample_frd(chan_id);
    } else {
        pts_info_disable_sample_frd(chan_id);
    }

    chan->alloc_type = cfg->alloc_type;

#ifdef VFMW_CRC_SUPPORT
    crc_set_config(chan_id, cfg);
#endif

#ifdef VFMW_STREAM_SUPPORT
    intf = OS_ALLOC_VIR("vfmw_stream_intf", sizeof(vfmw_stream_intf));
    if (intf == HI_NULL) {
        dprint(PRN_ERROR, "Chan %d alloc virmem for stream intf fail!\n", chan_id);
        return ;
    }
    intf->stream_provider_inst_id = chan_id;
    intf->read_stream = vadpt_read_stream;
    intf->release_stream = vadpt_release_stream;
    cfg->private = PTR_UINT64(intf);
#endif

    if (pdt_get_attr(ATTR_GET_LICENSE, &license) == HI_SUCCESS) {
        cfg->max_core_num = license.core_num;
    }

    return;
}

static hi_void vadpt_bef_start_chan(hi_s32 chan_id, hi_void *args)
{
    vfmw_chan *chan;

    chan = vadpt_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
    }

    chan->kpi.start.begin = OS_GET_TIME_US();

    return;
}

static hi_void vadpt_bef_stop_chan(hi_s32 chan_id, hi_void *args)
{
    vfmw_chan *chan;

    chan = vadpt_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
    }

    chan->kpi.stop.begin = OS_GET_TIME_US();

    return;
}

static hi_void vadpt_bef_reset_chan(hi_s32 chan_id, hi_void *args)
{
    vfmw_chan *chan;

    chan = vadpt_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
    }

#ifdef VFMW_VPP_SUPPORT
    vpp_reset(chan_id);
#endif

    chan->kpi.reset.begin = OS_GET_TIME_US();

    return;
}

static hi_void vadpt_bef_destroy_chan(hi_s32 chan_id, hi_void *args)
{
    vfmw_chan *chan;

    chan = vadpt_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
    }

    chan->kpi.destroy.begin = OS_GET_TIME_US();

    pts_info_free(chan_id);

#ifdef VFMW_CRC_SUPPORT
    if (crc_destroy_instance(chan_id) != HI_SUCCESS) {
        dprint(PRN_ERROR, "destroy CRC inst %d failed!\n", chan_id);
    }
#endif

#ifdef VFMW_VPP_SUPPORT
    vpp_destroy(chan_id);
#endif

#ifdef VFMW_FENCE_SUPPORT
    fence_destroy_chan(chan_id);
#endif

#ifdef VFMW_USD_POOL_SUPPORT
    usrdat_destroy_chan(chan_id);
#endif

    return;
}

static hi_void vadpt_bef_bind_es_buffer(hi_s32 chan_id, hi_void *args)
{
    vfmw_chan *chan;

    chan = vadpt_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        return;
    }

#ifdef VFMW_STREAM_SUPPORT
    if (((vfmw_stream_buf *)args)->phy_addr == 0) {
        if (stream_get_es_info(chan_id, (vfmw_stream_buf *)args) != HI_SUCCESS) {
            dprint(PRN_ERROR, "Get chan %d es info fail!\n", chan_id);
            return;
        }
    } else if (chan->is_sec) {
        if (stream_attach_es_buf(chan_id, (vfmw_stream_buf *)args) != HI_SUCCESS) {
            dprint(PRN_ERROR, "attach chan %d es buf fail!\n", chan_id);
            return;
        }
    }
#endif

    return;
}

#ifdef VFMW_VPP_SUPPORT
static hi_void vadpt_vpp_create(hi_s32 chan_id, hi_void *args)
{
    hi_s32 ret;
    vfmw_entry *entry = vadpt_entry();
    vpp_create_param param = {0};
    vfmw_chan_option *chan_option = HI_NULL;
    fun_vfmw_event_report event_report = HI_NULL;
    vfmw_chan *chan = HI_NULL;

    chan = vadpt_chan(chan_id);
    VFMW_ASSERT(chan != HI_NULL);

    chan_option = (vfmw_chan_option *)args;
    if (chan_option != HI_NULL) {
        param.work_mode = (chan_option->adpt_type == ADPT_TYPE_VDEC) ? QUERY_MODE : REPORT_MODE;
        if (chan_option->adpt_type == ADPT_TYPE_VDEC) {
            event_report = entry->event_report_vdec;
        } else {
            event_report = entry->event_report_omxvdec;
        }
        param.event_report = event_report;
    }

    chan->work_mode = param.work_mode;

    ret = vpp_create(chan_id, &param);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "vpp_create failed!\n", chan_id);
    }
}
#endif

#ifdef VFMW_STREAM_SUPPORT
static hi_void vadpt_stream_create(hi_s32 chan_id, hi_void *args)
{
    vfmw_chan_option *chan_option = (vfmw_chan_option *)args;

    (hi_void)stream_create_chan(chan_id, chan_option);
}
#endif

static hi_void vadpt_aft_create_chan(hi_s32 chan_id, hi_void *args, hi_s32 result)
{
    vfmw_chan *chan = HI_NULL;
    vfmw_entry *entry = vadpt_entry();

    if (result != HI_SUCCESS) {
        if ((chan = vadpt_chan(chan_id)) != NULL) {
            chan->state = VFMW_CHAN_STATE_NULL;
        }
        return;
    }

    pts_info_alloc(chan_id);

#ifdef VFMW_CRC_SUPPORT
    if (crc_create_instance(chan_id) != HI_SUCCESS) {
        dprint(PRN_ERROR, "create CRC inst %d failed!\n", chan_id);
    }
#endif

    chan = vadpt_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
    }

    chan->state = VFMW_CHAN_STATE_CREATE;
    chan->frm_cnt = 0;
    entry->chan_cnt++;
    chan->work_mode = QUERY_MODE;

#ifdef VFMW_FENCE_SUPPORT
    fence_create_chan(chan_id, args);
#endif

#ifdef VFMW_VPP_SUPPORT
    vadpt_vpp_create(chan_id, args);
#endif

#ifdef VFMW_STREAM_SUPPORT
    vadpt_stream_create(chan_id, args);
#endif

#ifdef VFMW_USD_POOL_SUPPORT
    usrdat_create_chan(chan_id);
#endif

    chan->kpi.create.end = OS_GET_TIME_US();
    chan->kpi.create.cost = chan->kpi.create.end - chan->kpi.create.begin;

    return;
}

static hi_void vadpt_aft_config_chan(hi_s32 chan_id, hi_void *args, hi_s32 result)
{
    vfmw_chan *chan = HI_NULL;
    vfmw_chan_cfg *cfg = (vfmw_chan_cfg *)args;

    if (result != HI_SUCCESS) {
        return;
    }

    chan = vadpt_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
    }

#ifdef VFMW_VPP_SUPPORT
    vpp_config(chan_id, cfg);
#endif

#ifdef VFMW_STREAM_SUPPORT
    stream_config(chan_id, cfg);

    if (cfg->private != 0) {
        OS_FREE_VIR(UINT64_PTR(cfg->private));
        cfg->private = 0;
    }
#endif

#ifdef VFMW_USD_POOL_SUPPORT
    usrdat_config_chan(chan_id, cfg);
#endif

    chan->kpi.config.end = OS_GET_TIME_US();
    chan->kpi.config.cost = chan->kpi.config.end - chan->kpi.config.begin;

    return;
}

static hi_void vadpt_aft_start_chan(hi_s32 chan_id, hi_void *args, hi_s32 result)
{
    vfmw_chan *chan = HI_NULL;

    if (result != HI_SUCCESS) {
        return;
    }

    chan = vadpt_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
    }

#ifdef VFMW_VPP_SUPPORT
    vpp_start(chan_id);
#endif

    chan->kpi.start.end = OS_GET_TIME_US();
    chan->kpi.start.cost = chan->kpi.start.end - chan->kpi.start.begin;

    return;
}

static hi_void vadpt_aft_stop_chan(hi_s32 chan_id, hi_void *args, hi_s32 result)
{
    vfmw_chan *chan = HI_NULL;

    if (result != HI_SUCCESS) {
        return;
    }

    chan = vadpt_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
    }

#ifdef VFMW_VPP_SUPPORT
    vpp_stop(chan_id);
#endif

    chan->kpi.stop.end = OS_GET_TIME_US();
    chan->kpi.stop.cost = chan->kpi.stop.end - chan->kpi.stop.begin;

    return;
}

static hi_void vadpt_aft_reset_chan(hi_s32 chan_id, hi_void *args, hi_s32 result)
{
    vfmw_chan *chan = HI_NULL;

    pts_info_reset(chan_id);

    if (result != HI_SUCCESS) {
        return;
    }

    chan = vadpt_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
    }

#ifdef VFMW_STREAM_SUPPORT
    stream_reset(chan_id);
#endif

#ifdef VFMW_USD_POOL_SUPPORT
    (hi_void)usrdat_reset_chan(chan_id);
#endif

    chan->kpi.reset.end = OS_GET_TIME_US();
    chan->kpi.reset.cost = chan->kpi.reset.end - chan->kpi.reset.begin;

    return;
}

static hi_void vadpt_aft_flush_chan(hi_s32 chan_id, hi_void *args, hi_s32 result)
{
    vfmw_chan *chan = HI_NULL;
    vfmw_flush_type flush_type = *((vfmw_flush_type *)args);

    pts_info_reset(chan_id);

    if (result != HI_SUCCESS) {
        return;
    }

    chan = vadpt_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
    }

#ifdef VFMW_VPP_SUPPORT
    if (flush_type == VFMW_FLUSH_OUT || flush_type == VFMW_FLUSH_ALL) {
        vpp_reset(chan_id);
    }
#endif

#ifdef VFMW_STREAM_SUPPORT
    if (flush_type == VFMW_FLUSH_IN || flush_type == VFMW_FLUSH_ALL) {
        stream_reset(chan_id);
    }
#endif

#ifdef VFMW_USD_POOL_SUPPORT
    if (flush_type == VFMW_FLUSH_IN || flush_type == VFMW_FLUSH_ALL) {
        (hi_void)usrdat_reset_chan(chan_id);
    }
#endif

    return;
}

static hi_void vadpt_aft_destroy_chan(hi_s32 chan_id, hi_void *args, hi_s32 result)
{
    vfmw_chan *chan = HI_NULL;
    vfmw_entry *entry = vadpt_entry();

    if (result != HI_SUCCESS) {
        return;
    }

    chan = vadpt_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        return;
    }

#ifdef VFMW_STREAM_SUPPORT
    (hi_void)stream_destroy_chan(chan_id);
#endif

    entry->chan_cnt--;
    chan->state = VFMW_CHAN_STATE_NULL;

    chan->kpi.destroy.end = OS_GET_TIME_US();
    chan->kpi.destroy.cost = chan->kpi.destroy.end - chan->kpi.destroy.begin;

    return;
}

static hi_void vadpt_aft_get_chan_state(hi_s32 chan_id, hi_void *args, hi_s32 result)
{
    vfmw_chan *chan = HI_NULL;

    if (result != HI_SUCCESS) {
        return;
    }

    chan = vadpt_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        return;
    }

#ifdef VFMW_STREAM_SUPPORT
    (hi_void)stream_get_chan_status(chan_id, (vfmw_chan_info *)args);
#endif

#ifdef VFMW_VPP_SUPPORT
    (hi_void)vpp_get_chan_status(chan_id, (vfmw_chan_info *)args);
#endif

    return;
}

static hi_void vadpt_aft_set_trick_mode(hi_s32 chan_id, hi_void *args, hi_s32 result)
{
    vfmw_chan *chan = HI_NULL;

    if (result != HI_SUCCESS) {
        return;
    }

    chan = vadpt_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        return;
    }

#ifdef VFMW_VPP_SUPPORT
    {
        hi_s32 ret;
        ret = vpp_set_pvr_speed(chan_id, *((hi_s32 *)args));
        if (ret != HI_SUCCESS) {
            dprint(PRN_ERROR, "vpp_set_pvr_speed fail!\n");
        }
    }
#endif

    return;
}

static hi_void vadpt_aft_set_evt_map(hi_s32 chan_id, hi_void *args, hi_s32 result)
{
    vfmw_chan *chan = HI_NULL;

    if (result != HI_SUCCESS) {
        return;
    }

    chan = vadpt_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        return;
    }

#ifdef VFMW_VPP_SUPPORT
    {
        hi_s32 ret;
        ret = vpp_set_evt_map(chan_id, *((hi_u32 *)args));
        if (ret != HI_SUCCESS) {
            dprint(PRN_ERROR, "vpp_set_evt_map fail!\n");
        }
    }
#endif

#ifdef VFMW_STREAM_SUPPORT
    {
        hi_s32 ret;
        ret = stream_set_evt_map(chan_id, *((hi_u32 *)args));
        if (ret != HI_SUCCESS) {
            dprint(PRN_ERROR, "stream_set_evt_map fail!\n");
        }
    }
#endif
}

static hi_void vadpt_aft_set_ctrl_info(hi_s32 chan_id, hi_void *args, hi_s32 result)
{
    vfmw_chan *chan = HI_NULL;

    if (result != HI_SUCCESS) {
        return;
    }

    chan = vadpt_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        return;
    }

#ifdef VFMW_VPP_SUPPORT
    {
        hi_s32 ret;
        ret = vpp_set_pvr_ctrl_info(chan_id, (vfmw_control_info *)args);
        if (ret != HI_SUCCESS) {
            dprint(PRN_ERROR, "vpp_set_pvr_ctrl_info fail!\n");
        }
    }
#endif

    return;
}

static hi_void vadpt_before_control(hi_s32 chan_id, vfmw_cid cmd_id, hi_void *args)
{
    switch (cmd_id) {
        case VFMW_CID_CREATE_CHAN:
            vadpt_bef_create_chan(chan_id, args);
            break;
        case VFMW_CID_CFG_CHAN:
            vadpt_bef_config_chan(chan_id, args);
            break;
        case VFMW_CID_START_CHAN:
            vadpt_bef_start_chan(chan_id, args);
            break;
        case VFMW_CID_STOP_CHAN:
            vadpt_bef_stop_chan(chan_id, args);
            break;
        case VFMW_CID_RESET_CHAN:
        case VFMW_CID_RESET_CHAN_WITH_OPTION:
            vadpt_bef_reset_chan(chan_id, args);
            break;
        case VFMW_CID_DESTROY_CHAN:
            vadpt_bef_destroy_chan(chan_id, args);
            break;
        case VFMW_CID_BIND_ES_BUF:
            vadpt_bef_bind_es_buffer(chan_id, args);
            break;
        default:
            break;
    }

    return;
}

static hi_void vadpt_after_control(hi_s32 chan_id, vfmw_cid cmd_id, hi_void *args, hi_s32 result)
{
    switch (cmd_id) {
        case VFMW_CID_CREATE_CHAN:
            vadpt_aft_create_chan(chan_id, args, result);
            break;
        case VFMW_CID_CFG_CHAN:
            vadpt_aft_config_chan(chan_id, args, result);
            break;
        case VFMW_CID_START_CHAN:
            vadpt_aft_start_chan(chan_id, args, result);
            break;
        case VFMW_CID_STOP_CHAN:
            vadpt_aft_stop_chan(chan_id, args, result);
            break;
        case VFMW_CID_RESET_CHAN:
        case VFMW_CID_RESET_CHAN_WITH_OPTION:
            vadpt_aft_reset_chan(chan_id, args, result);
            break;
        case VFMW_CID_FLUSH_CHAN:
            vadpt_aft_flush_chan(chan_id, args, result);
            break;
        case VFMW_CID_DESTROY_CHAN:
            vadpt_aft_destroy_chan(chan_id, args, result);
            break;
        case VFMW_CID_GET_CHAN_STATE:
            vadpt_aft_get_chan_state(chan_id, args, result);
            break;
        case VFMW_CID_SET_CTRL_INFO:
            vadpt_aft_set_ctrl_info(chan_id, args, result);
            break;
        case VFMW_CID_SET_TRICK_MODE:
            vadpt_aft_set_trick_mode(chan_id, args, result);
            break;
        case VFMW_CID_SET_EVENT_MAP:
            vadpt_aft_set_evt_map(chan_id, args, result);
            break;
        default:
            break;
    }

    return;
}

static hi_void vadpt_save_image_yuv(vfmw_chan *chan, hi_void *image)
{
    hi_s32 ret;
    vfmw_dflt *dflt = vadpt_get_dflt();

    if (chan->is_sec == HI_TRUE) {
        dprint(PRN_ERROR, "%s not support saving secure image yuv.\n", __func__);
        return;
    }

    if (chan->frm_cnt < dflt->save_begin || chan->frm_cnt > dflt->save_end) {
        dprint(PRN_ALWS, "%s current frame count %d not in the range[%d, %d].\n", __func__,
               chan->frm_cnt, dflt->save_begin, dflt->save_end);
        return;
    }

    /* REPORT_MODE(OMX path) get image ,have vpp_sema locked that lead to dead lock when reset/stop/start cmd */
    if (chan->work_mode == QUERY_MODE) {
        ret = OS_SEMA_DOWN(chan->sema);
        if (ret != HI_SUCCESS) {
            dprint(PRN_ERROR, "%s down_interruptible error.\n", __func__);
            return;
        }
    }

    if (chan->yuv != HI_NULL) {
        dprint(PRN_ALWS, "chan %d frame %d yuv:\n", chan->id, chan->frm_cnt);
        dbg_save_yuv(chan->id, image, chan->yuv);
    }

    if (chan->mtdt != HI_NULL) {
        dprint(PRN_ALWS, "chan %d frame %d metadata:\n", chan->id, chan->frm_cnt);
        dbg_save_mtdt(chan->id, image, chan->mtdt);
    }

    if (chan->work_mode == QUERY_MODE) {
        OS_SEMA_UP(chan->sema);
    }

    return;
}

static hi_s32 vadpt_process_get_image_success(vfmw_chan *chan, hi_void *image)
{
    hi_s32 ret;

    img_info_convert(chan->id, image);

#ifdef VFMW_VMM_SUPPORT
    if (chan->alloc_type == VFMW_FRM_ALLOC_INNER) {
        ret = vadpt_vmm_set_image_out(image);
        if (ret != HI_SUCCESS) {
            dprint(PRN_ERROR, "%s vadpt_vmm_set_image_out failed.\n", __func__);
            return HI_FAILURE;
        }
    }
#endif

#ifdef VFMW_CRC_SUPPORT
    crc_put_image(chan->id, image);
#endif

#ifdef VFMW_USD_POOL_SUPPORT
    usrdat_process(chan->id, image);
#endif

    vadpt_save_image_yuv(chan, image);

    chan->frm_cnt++;

    return HI_SUCCESS;
}

static hi_s32 vadpt_process_release_image_success(vfmw_chan *chan, const hi_void *image)
{
#ifdef VFMW_VMM_SUPPORT
    if (chan->alloc_type == VFMW_FRM_ALLOC_INNER) {
        (hi_void)vadpt_vmm_set_image_in(image);
    }
#endif

    return HI_SUCCESS;
}

STATIC hi_void vadpt_init_entry(hi_void)
{
    hi_s32 id;
    vfmw_entry *entry = HI_NULL;

    entry = vadpt_entry();
    entry->ref_cnt = 0;
    OS_SEMA_INIT(&entry->sema);

    for (id = 0; id < VFMW_CHAN_NUM; id++) {
        OS_SEMA_INIT(&(entry->chan[id].sema));
    }
}

#ifdef VFMW_VPP_SUPPORT
static hi_void vadapt_set_callback(vfmw_entry *entry, vfmw_adpt_type type, fun_vfmw_event_report event_report)
{
    VFMW_ASSERT(event_report != HI_NULL);

    switch (type) {
        case ADPT_TYPE_VDEC:
            entry->event_report_vdec = event_report;
            break;

        case ADPT_TYPE_OMX:
            entry->event_report_omxvdec = event_report;
            break;

        default:
            dprint(PRN_FATAL, "%s unkown adapter type: %d\n", __func__, type);
            return;
    }

    return;
}
#endif

hi_s32 vadpt_release_image(hi_s32 chan_id, const hi_void *image)
{
    hi_s32 ret = HI_FAILURE;
    vfmw_chan *chan = HI_NULL;

    VFMW_ASSERT_RET(vadpt_get_load(), HI_FAILURE);

    chan = vadpt_chan(chan_id);
    VFMW_ASSERT_RET(chan != HI_NULL, HI_FAILURE);

    switch (chan->type) {
#ifdef VFMW_LOCAL_SUPPORT
        case VFMW_TYPE_LOCAL:
            ret = local_release_image(chan_id, image);
            break;
#endif
#ifdef VFMW_TEE_SUPPORT
        case VFMW_TYPE_TEE:
            ret = tee_client_release_image(chan_id, image);
            break;
#endif
#ifdef VFMW_MDC_SUPPORT
        case VFMW_TYPE_MCU:
            ret = mdc_client_rel_image(chan_id, image);
            break;
#endif
#ifdef VFMW_USER_SUPPORT
        case VFMW_TYPE_USER:
            ret = user_release_image(chan_id, image);
            break;
#endif
        default:
            dprint(PRN_ERROR, "vfmw adapter can not support type %d\n", chan->type);
            break;
    }

    if (ret == HI_SUCCESS) {
        (hi_void)vadpt_process_release_image_success(chan, image);
    }

    return ret;
}

hi_s32 vadpt_get_image(hi_s32 chan_id, hi_void *image)
{
    hi_s32 ret = HI_FAILURE;
    vfmw_chan *chan = HI_NULL;

    VFMW_ASSERT_RET(vadpt_get_load(), HI_FAILURE);

    chan = vadpt_chan(chan_id);
    VFMW_ASSERT_RET(chan != HI_NULL, HI_FAILURE);

    switch (chan->type) {
#ifdef VFMW_LOCAL_SUPPORT
        case VFMW_TYPE_LOCAL:
            ret = local_get_image(chan_id, image);
            break;
#endif
#ifdef VFMW_TEE_SUPPORT
        case VFMW_TYPE_TEE:
            ret = tee_client_get_image(chan_id, image);
            break;
#endif
#ifdef VFMW_MDC_SUPPORT
        case VFMW_TYPE_MCU:
            ret = mdc_client_get_image(chan_id, image);
            break;
#endif
#ifdef VFMW_USER_SUPPORT
        case VFMW_TYPE_USER:
            ret = user_get_image(chan_id, image);
            break;
#endif
        default:
            dprint(PRN_ERROR, "vfmw adapter can not support type %d\n", chan->type);
            break;
    }

    if (ret == HI_SUCCESS) {
        ret = vadpt_process_get_image_success(chan, image);
        if (ret != HI_SUCCESS) {
            (hi_void)vadpt_release_image(chan->id, image);
        }
    }

    return ret;
}

hi_s32 vadpt_check_image(hi_s32 chan_id, hi_void *image)
{
    hi_s32 ret = HI_FAILURE;
    vfmw_chan *chan = HI_NULL;

    VFMW_ASSERT_RET(vadpt_get_load(), HI_FAILURE);

    chan = vadpt_chan(chan_id);
    VFMW_ASSERT_RET(chan != HI_NULL, HI_FAILURE);

    switch (chan->type) {
#ifdef VFMW_LOCAL_SUPPORT
        case VFMW_TYPE_LOCAL:
            ret = local_check_image(chan_id, image);
            break;
#endif
#ifdef VFMW_MDC_SUPPORT
        case VFMW_TYPE_MCU:
            ret = mdc_client_check_image(chan_id, image);
            break;
#endif
        default:
            dprint(PRN_ERROR, "%s type %d error\n", __func__, chan->type);
            break;
    }

    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return ret;
}

/* ==== extern function define ==== */
hi_s32 vadpt_open(hi_void)
{
    hi_s32 ret = HI_SUCCESS;
#ifdef VFMW_VPP_SUPPORT
    vpp_init_param param = {0};
#endif
#ifdef VFMW_FENCE_SUPPORT
    fence_init_param fence = {0};
#endif

    os_intf_init();

    OS_PROC_INIT();

    vadpt_init_entry();

#ifdef VFMW_VMM_SUPPORT
    vmm_mod_init(vadpt_vmm_call_back);
#endif

#ifdef VFMW_MDC_SUPPORT
    (hi_void)mdc_client_open();
#endif

#ifdef VFMW_LOCAL_SUPPORT
    (hi_void)local_open();
#endif

#ifdef VFMW_USER_SUPPORT
    (hi_void)user_open();
#endif

#ifdef VFMW_CRC_SUPPORT
    crc_init();
#endif

#ifdef IPT_SUPPORT
    ipt_open();
#endif

#ifdef VFMW_TEE_SUPPORT
    tee_client_open();
#endif

#ifdef VFMW_PROC_SUPPORT
    (hi_void)vfmw_adpt_proc_create();
#endif

    img_info_init();

#ifdef VFMW_VPP_SUPPORT
    param.get_image = vadpt_get_image;
    param.rls_image = vadpt_release_image;
    vpp_init(&param);
#endif

#ifdef VFMW_FENCE_SUPPORT
    fence.rls_image = vadpt_release_image;
    fence.chk_image = vadpt_check_image;
    fence_init(&fence);
#endif

#ifdef VT_SUPPORT
    {
        hi_s32 VT_ModInit(hi_void);
        VT_ModInit();
    }
#endif

    vadpt_set_load(HI_TRUE);

    return ret;
}

hi_s32 vadpt_close(hi_void)
{
    hi_s32 id;
    vfmw_entry *entry = HI_NULL;

#ifdef VT_SUPPORT
    {
        hi_s32 VT_ModExit(hi_void);
        VT_ModExit();
    }
#endif

#ifdef VFMW_VPP_SUPPORT
    vpp_deinit();
#endif

#ifdef VFMW_FENCE_SUPPORT
    fence_exit();
#endif

#ifdef VFMW_CRC_SUPPORT
    crc_exit();
#endif

#ifdef IPT_SUPPORT
    ipt_close();
#endif

#ifdef VFMW_MDC_SUPPORT
    (hi_void)mdc_client_close();
#endif

#ifdef VFMW_LOCAL_SUPPORT
    (hi_void)local_close();
#endif

#ifdef VFMW_USER_SUPPORT
    (hi_void)user_close();
#endif

#ifdef VFMW_VMM_SUPPORT
    vmm_mod_exit();
#endif

#ifdef VFMW_PROC_SUPPORT
    (hi_void)vfmw_adpt_proc_destroy();
#endif

    entry = vadpt_entry();
    OS_SEMA_EXIT(entry->sema);

    for (id = 0; id < VFMW_CHAN_NUM; id++) {
        OS_SEMA_EXIT(entry->chan[id].sema);
    }

    OS_PROC_EXIT();

#ifdef VFMW_TEE_SUPPORT
    tee_client_close();
#endif

    img_info_exit();

    (hi_void)os_intf_exit();

    vadpt_set_load(HI_FALSE);

    return HI_SUCCESS;
}

hi_s32 vadpt_init(hi_void *args)
{
    hi_s32 id;
    hi_s32 ret;
    vfmw_entry *entry = HI_NULL;
    hi_s32 sec_init_flag = HI_FALSE;

    VFMW_ASSERT_RET(vadpt_get_load(), HI_FAILURE);

    vadpt_sema_down();

    entry = vadpt_entry();
    entry->init_kpi.begin = OS_GET_TIME_US();

    ret = vadpt_reset_hardware();
    if (ret != HI_SUCCESS) {
        vadpt_sema_up();
        dprint(PRN_ERROR, "vfmw reset hardware fail\n");
        return HI_FAILURE;
    }

    sec_init_flag = vadpt_is_init_sec_vfmw(args);
    if (sec_init_flag == HI_TRUE) {
#ifdef VFMW_TEE_SUPPORT
        ret = tee_client_init(args, sizeof(vfmw_init_param));
#endif
    } else {
#ifdef VFMW_MDC_SUPPORT
        ret = mdc_client_init(args, sizeof(vfmw_init_param));
#endif
#ifdef VFMW_LOCAL_SUPPORT
        ret = local_init(args, sizeof(vfmw_init_param));
#endif
#ifdef VFMW_USER_SUPPORT
        ret = user_init(args, sizeof(vfmw_init_param));
#endif
    }

#ifdef VFMW_CRC_SUPPORT
    crc_init_calc_env();
#endif

#ifdef VFMW_VPP_SUPPORT
    if (ret == HI_SUCCESS && (args != HI_NULL)) {
        vfmw_init_param *param = (vfmw_init_param *)args;
        vadapt_set_callback(entry, param->adpt_type, param->fn_callback);
    }
#endif

#ifdef VFMW_STREAM_SUPPORT
    stream_init((vfmw_init_param *)args);
#endif

#ifdef VFMW_USD_POOL_SUPPORT
    usrdat_init();
#endif

    if (entry->ref_cnt == 0) {
        entry->state = VFMW_STATE_OPEN;
        for (id = 0; id < VFMW_CHAN_NUM; id++) {
            entry->chan[id].state = VFMW_CHAN_STATE_NULL;
            entry->chan[id].id = id;
        }
    }

    entry->ref_cnt++;
    if (entry->ref_cnt >= 0x7fffffff) {
        dprint(PRN_ERROR, "ref_cnt:%d is invalid!\n", entry->ref_cnt);
        ret = HI_FAILURE;
    }

    entry->init_kpi.end = OS_GET_TIME_US();
    entry->init_kpi.cost = entry->init_kpi.end - entry->init_kpi.begin;

    vadpt_sema_up();

    return ret;
}

hi_s32 vadpt_exit(hi_void *args)
{
    hi_s32 ret = HI_FAILURE;
    vfmw_entry *entry = HI_NULL;
    hi_s32 *sec_flag = (hi_s32 *)args;

    VFMW_ASSERT_RET(vadpt_get_load(), HI_FAILURE);

    vadpt_sema_down();

    entry = vadpt_entry();
    entry->exit_kpi.begin = OS_GET_TIME_US();

    if (sec_flag != HI_NULL && (*sec_flag) != 0) {
#ifdef VFMW_TEE_SUPPORT
        ret = tee_client_exit(args);
#endif
    } else {
#ifdef VFMW_MDC_SUPPORT
        ret = mdc_client_exit(args, 0);
#endif

#ifdef VFMW_LOCAL_SUPPORT
        ret = local_exit(args);
#endif
    }

#ifdef VFMW_CRC_SUPPORT
    crc_exit_calc_env();
#endif

#ifdef VFMW_STREAM_SUPPORT
    stream_exit();
#endif

#ifdef VFMW_USD_POOL_SUPPORT
    usrdat_exit();
#endif

    (hi_void)pdt_suspend(HI_NULL);

    if (entry->ref_cnt <= 0) {
        vadpt_sema_up();
        return HI_FAILURE;
    } else if (entry->ref_cnt == 1) {
        entry->state = VFMW_STATE_NULL;
    }

    entry->ref_cnt--;

    entry->exit_kpi.end = OS_GET_TIME_US();
    entry->exit_kpi.cost = entry->exit_kpi.end - entry->exit_kpi.begin;

    vadpt_sema_up();

    return ret;
}

hi_s32 vadpt_suspend(hi_void *args)
{
    hi_s32 ret;
    vfmw_entry *entry = vadpt_entry();

    VFMW_ASSERT_RET(vadpt_get_load(), HI_FAILURE);

    vadpt_sema_down();

    ret = HI_SUCCESS;

#ifdef VFMW_MDC_SUPPORT
    if (entry->chan_cnt != 0) {
        vadpt_sema_up();
        return HI_FAILURE;
    }
    (hi_void)mdc_client_close();
#endif

#ifdef VFMW_LOCAL_SUPPORT
    ret = local_suspend(args);
#endif

#ifdef VFMW_TEE_SUPPORT
    tee_client_suspend(args);
#endif

    (hi_void)pdt_suspend(HI_NULL);

    vadpt_sema_up();

    return ret;
}

hi_s32 vadpt_resume(hi_void *args)
{
    hi_s32 ret;
    vfmw_entry *entry = vadpt_entry();

    VFMW_ASSERT_RET(vadpt_get_load(), HI_FAILURE);

    vadpt_sema_down();

    ret = HI_SUCCESS;

    (hi_void)pdt_resume(HI_NULL);

#ifdef VFMW_MDC_SUPPORT
    if (entry->chan_cnt == 0) {
        (hi_void)mdc_client_open();
    }
#endif

#ifdef VFMW_LOCAL_SUPPORT
    ret = local_resume(args);
#endif

#ifdef VFMW_TEE_SUPPORT
    tee_client_resume(args);
#endif

    vadpt_sema_up();

    return ret;
}

hi_s32 vadpt_get_frame(hi_s32 chan_id, hi_void *frame, hi_void *ext_frm_info)
{
    hi_s32 ret;
    hi_u32 time;
    vfmw_chan *chan = HI_NULL;
    VFMW_ASSERT_RET(vadpt_get_load(), HI_FAILURE);

    chan = vadpt_chan(chan_id);
    VFMW_ASSERT_RET(chan != HI_NULL, HI_FAILURE);

    time = OS_GET_TIME_US();

#ifndef VFMW_VPP_SUPPORT
    ret = vadpt_get_image(chan_id, frame);
#else
    ret = vpp_receive_frame(chan_id, frame, ext_frm_info);
#endif
    if (ret == HI_SUCCESS &&
        chan->kpi.getfrm.begin == 0) {
        chan->kpi.getfrm.begin = time;
        chan->kpi.getfrm.end = OS_GET_TIME_US();
        chan->kpi.getfrm.cost = chan->kpi.getfrm.end - chan->kpi.questm.end;
    }

    return ret;
}

hi_s32 vadpt_release_frame(hi_s32 chan_id, const hi_void *frame)
{
    vfmw_chan *chan = HI_NULL;
    VFMW_ASSERT_RET(vadpt_get_load(), HI_FAILURE);

    chan = vadpt_chan(chan_id);
    VFMW_ASSERT_RET(chan != HI_NULL, HI_FAILURE);

#ifndef VFMW_VPP_SUPPORT
    return vadpt_release_image(chan_id, frame);
#else
    return vpp_release_frame(chan_id, frame);
#endif
}

hi_s32 vadpt_control(hi_s32 chan_id, hi_s32 cmd, hi_void *args, hi_u32 Len)
{
    hi_s32 ret;
    vfmw_entry *entry = HI_NULL;
    hi_bool local_case = HI_FALSE;
    vfmw_type type;

    VFMW_ASSERT_RET(vadpt_get_load(), HI_FAILURE);
    VFMW_ASSERT_RET(vadpt_is_chan_valid(chan_id, cmd) == HI_TRUE, HI_FAILURE);

    ret = vadpt_local_control(chan_id, cmd, args, &local_case);
    if (local_case == HI_TRUE) {
        return ret;
    }

    vadpt_sema_down();

    ret = vadpt_get_chan_id(cmd, &chan_id);
    if (ret != HI_SUCCESS) {
        vadpt_sema_up();
        dprint(PRN_ALWS, "error: %s vadpt_get_chan_id error\n", __func__);
        return HI_FAILURE;
    }

    vadpt_sema_up();

    entry = vadpt_entry();

    OS_SEMA_DOWN(entry->chan[chan_id].sema);

    vadpt_before_control(chan_id, cmd, args);

    type = vadpt_get_vfmw_type(chan_id, cmd, args);

    switch (type) {
#ifdef VFMW_LOCAL_SUPPORT
        case VFMW_TYPE_LOCAL:
            ret = local_control(chan_id, cmd, args, Len);
            break;
#endif
#ifdef VFMW_TEE_SUPPORT
        case VFMW_TYPE_TEE:
            ret = tee_client_control(chan_id, cmd, args, Len);
            break;
#endif
#ifdef VFMW_MDC_SUPPORT
        case VFMW_TYPE_MCU:
            ret = mdc_client_control(chan_id, cmd, args, Len);
            break;
#endif
#ifdef VFMW_USER_SUPPORT
        case VFMW_TYPE_USER:
            ret = user_control(chan_id, cmd, args, Len);
            break;
#endif
        default:
            ret = HI_FAILURE;
            dprint(PRN_ERROR, "vfmw adapter can not support type %d\n", type);
            break;
    }

    vadpt_after_control(chan_id, cmd, args, ret);

    OS_SEMA_UP(entry->chan[chan_id].sema);

    return ret;
}
