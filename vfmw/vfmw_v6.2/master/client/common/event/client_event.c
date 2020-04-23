/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "client_event.h"
#ifdef VFMW_VMM_SUPPORT
#include "vmm_ext.h"
#endif
#include "dbg.h"

typedef struct {
    fun_vfmw_event_report evt_report_vdec;
    fun_vfmw_event_report evt_report_omxvdec;
} clt_event_callback;

static clt_event_callback g_clt_callback;
static clt_event_chan g_clt_evt_ctx[VFMW_CHAN_NUM];

/* ----------------------------INTERNAL FUNCTION------------------------------- */
static hi_s32 clt_evt_is_inner(hi_s32 type)
{
    if (type == EVNT_ALLOC_MEM || type == EVNT_RELEASE_MEM
        || type == EVNT_RELEASE_BURST_MEM || type == EVNT_CANCEL_ALLOC) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

#ifdef VFMW_VMM_SUPPORT
static hi_s32 clt_evt_alloc_mem(hi_s32 chan_id, hi_void *args)
{
    hi_s32 ret;
    vmm_cmd_prio proir;
    vfmw_mem_report *report;
    vmm_buffer vmm_buf;

    report = (vfmw_mem_report *)args;

    VFMW_CHECK_SEC_FUNC(memset_s(&vmm_buf, sizeof(vmm_buffer), 0, sizeof(vmm_buffer)));

    if (snprintf_s(vmm_buf.buf_name, sizeof(vmm_buf.buf_name),
                   sizeof(vmm_buf.buf_name), "%s", report->buf_name) < 0) {
        dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
    }
    vmm_buf.buf_name[sizeof(vmm_buf.buf_name) - 1] = '\0';

    vmm_buf.map = 0;
    vmm_buf.cache = report->is_cache;
    vmm_buf.size = report->size;
    vmm_buf.priv_id = report->unique_id;
    vmm_buf.mem_type = VMM_MEM_TYPE_SYS_ADAPT;
    vmm_buf.sec_flag = report->sec_flag;

    /* use phyaddr to trans priority, fix me... */
    proir = (report->phy_addr == 1) ? VMM_CMD_PRIO_MAX : VMM_CMD_PRIO_MIN;

    ret = vmm_send_command(VMM_CMD_ALLOC, proir, &vmm_buf);
    if (ret != VMM_SUCCESS) {
        dprint(PRN_ERROR, "%s ERROR: VMM_CMD_ALLOC %s size %d failed!\n", __func__, vmm_buf.buf_name, vmm_buf.size);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 clt_evt_release_mem(hi_s32 chan_id, hi_void *args)
{
    hi_s32 ret;
    vfmw_mem_report *report;
    vmm_buffer vmm_buf;

    report = (vfmw_mem_report *)args;

    VFMW_CHECK_SEC_FUNC(memset_s(&vmm_buf, sizeof(vmm_buffer), 0, sizeof(vmm_buffer)));

    if (snprintf_s(vmm_buf.buf_name, sizeof(vmm_buf.buf_name),
                   sizeof(vmm_buf.buf_name), "%s", report->buf_name) < 0) {
        dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
    }
    vmm_buf.buf_name[sizeof(vmm_buf.buf_name) - 1] = '\0';

    vmm_buf.start_phy_addr = report->phy_addr;
    vmm_buf.start_vir_addr = report->vir_addr;
    vmm_buf.map = 0;
    vmm_buf.cache = report->is_cache;
    vmm_buf.size = report->size;
    vmm_buf.priv_id = report->unique_id;
    vmm_buf.mem_type = VMM_MEM_TYPE_SYS_ADAPT;
    vmm_buf.sec_flag = report->sec_flag;

    ret = vmm_send_command(VMM_CMD_RELEASE, VMM_CMD_PRIO_MIN, &vmm_buf);
    if (ret != VMM_SUCCESS) {
        dprint(PRN_ERROR, "%s ERROR: VMM_CMD_RELEASE %s phy 0x%x failed!\n", __func__, vmm_buf.buf_name,
               vmm_buf.start_phy_addr);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 clt_evt_release_mem_burst(hi_void *args)
{
    hi_s32 ret;
    hi_s64 *p64;
    vmm_rls_burst rls_burst = { 0 };

    p64 = (hi_s64 *)args;

    rls_burst.priv_id = p64[0];
    rls_burst.priv_mask = p64[1];

    ret = vmm_send_command(VMM_CMD_RELEASE_BURST, VMM_CMD_PRIO_MIN, &rls_burst);
    if (ret != VMM_SUCCESS) {
        dprint(PRN_ERROR, "%s ERROR: VMM_CMD_RELEASE_BURST failed!\n", __func__);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 clt_evt_cancel_alloc(hi_void *args)
{
    hi_s32 ret;
    hi_s64 *p64;
    vmm_cancel_cmd cancel_cmd = { 0 };

    p64 = (hi_s64 *)args;

    cancel_cmd.cmd_id = VMM_CMD_ALLOC;
    cancel_cmd.priv_id = p64[0];
    cancel_cmd.priv_mask = p64[1];

    ret = vmm_send_command_block(VMM_CMD_BLK_CANCLE_CMD, &cancel_cmd);
    if (ret != VMM_SUCCESS) {
        dprint(PRN_ERROR, "%s ERROR: VMM_CMD_BLK_CANCLE_CMD %d failed!\n", __func__, cancel_cmd.cmd_id);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 clt_evt_process_inner(hi_s32 chan_id, hi_s32 type, hi_void *args)
{
    hi_s32 ret = HI_FAILURE;

    switch (type) {
        case EVNT_ALLOC_MEM:
            ret = clt_evt_alloc_mem(chan_id, args);
            break;

        case EVNT_RELEASE_MEM:
            ret = clt_evt_release_mem(chan_id, args);
            break;

        case EVNT_RELEASE_BURST_MEM:
            ret = clt_evt_release_mem_burst(args);
            break;

        case EVNT_CANCEL_ALLOC:
            ret = clt_evt_cancel_alloc(args);
            break;

        default:
            break;
    }

    return ret;
}
#endif

static hi_s32 clt_evt_get_one(hi_s32 chan_id, shr_evt **event)
{
    clt_event_chan *chan = clt_evt_get_chan_entry(chan_id);
    hi_s32 tail = chan->evt_list->tail;
    hi_s32 head = chan->evt_list->head;

    while (!(chan->evt_list->event[tail].valid) && (tail != head)) {
        tail = SHR_EVT_INC(tail);
    }
    chan->evt_list->tail = tail;

    if (tail == head) {
        return HI_FAILURE;
    }

    (*event) = &(chan->evt_list->event[tail]);

    return HI_SUCCESS;
}

static hi_s32 clt_evt_process_outer(hi_s32 chan_id, shr_evt *event)
{
    hi_s32 ret = HI_FAILURE;
    hi_s32 type;
    hi_u8 *para;
    vfmw_adpt_type adpt_type;
    hi_u32 param_len;

    type = event->type;
    para = (hi_u8 *)(event->para);
    adpt_type = event->adpt_type;
    param_len = event->param_len;

    switch (adpt_type) {
        case ADPT_TYPE_VDEC:
            if (g_clt_callback.evt_report_vdec == HI_NULL) {
                dprint(PRN_DBG, "s_CallBack.evt_report_vdec = HI_NULL\n");
            } else {
                ret = (g_clt_callback.evt_report_vdec)(chan_id, type, para, param_len);
            }
            break;

        case ADPT_TYPE_OMX:
            if (g_clt_callback.evt_report_omxvdec == HI_NULL) {
                dprint(PRN_ERROR, "s_CallBack.evt_report_omxvdec = HI_NULL\n");
            } else {
                ret = (g_clt_callback.evt_report_omxvdec)(chan_id, type, para, param_len);
            }
            break;

        default:
            dprint(PRN_ERROR, "%s Unkown Adapter type: %d\n", __func__, adpt_type);
            break;
    }

    return ret;
}

/* ----------------------------EXTERNAL FUNCTION------------------------------- */
hi_s32 clt_evt_init(hi_void)
{
    VFMW_CHECK_SEC_FUNC(memset_s(g_clt_evt_ctx,
                                 sizeof(clt_event_chan) * VFMW_CHAN_NUM, 0, sizeof(clt_event_chan) * VFMW_CHAN_NUM));
    VFMW_CHECK_SEC_FUNC(memset_s(&g_clt_callback, sizeof(clt_event_callback), 0, sizeof(clt_event_callback)));

    return HI_SUCCESS;
}

hi_s32 clt_evt_process(hi_s32 chan_id)
{
    hi_s32 ret = HI_FAILURE;
    shr_evt *event = HI_NULL;
    clt_event_chan *chan = clt_evt_get_chan_entry(chan_id);

    VFMW_ASSERT_RET((chan->evt_list != HI_NULL), HI_FAILURE);

    while (clt_evt_get_one(chan_id, &event) == HI_SUCCESS) {
        if (clt_evt_is_inner(event->type)) {
#ifdef VFMW_VMM_SUPPORT
            ret = clt_evt_process_inner(chan_id, event->type, event->para);
#endif
        } else {
            ret = clt_evt_process_outer(chan_id, event);
        }
        if (ret != HI_SUCCESS) {
            dprint(PRN_DBG, "Process event Error!\n");
        }

        event->valid = 0;
        chan->evt_list->tail = SHR_EVT_INC(chan->evt_list->tail);
    }

    return HI_SUCCESS;
}

hi_s32 clt_evt_reset_queue(hi_s32 chan_id)
{
#ifdef VFMW_VMM_SUPPORT
    hi_s32 ret = HI_FAILURE;
#endif
    shr_evt *event = HI_NULL;
    clt_event_chan *chan = clt_evt_get_chan_entry(chan_id);

    VFMW_ASSERT_RET((chan->evt_list != HI_NULL), HI_FAILURE);

    while (clt_evt_get_one(chan_id, &event) == HI_SUCCESS) {
        if (clt_evt_is_inner(event->type)) {
#ifdef VFMW_VMM_SUPPORT
            ret = clt_evt_process_inner(chan_id, event->type, event->para);
            if (ret != HI_SUCCESS) {
                dprint(PRN_ERROR, "Process event Error!\n");
            }
#endif
        }

        event->valid = 0;
        chan->evt_list->tail = SHR_EVT_INC(chan->evt_list->tail);
    }

    return HI_SUCCESS;
}

hi_u32 clt_evt_get_queue_size(hi_void)
{
    return sizeof(shr_evt_list);
}

hi_s32 clt_evt_bind_queue(hi_s32 chan_id, hi_void *list_addr)
{
    clt_event_chan *chan = clt_evt_get_chan_entry(chan_id);

    chan->evt_list = (shr_evt_list *)list_addr;

    return HI_SUCCESS;
}

hi_void clt_evt_unbind_queue(hi_s32 chan_id)
{
    clt_event_chan *chan = clt_evt_get_chan_entry(chan_id);

    chan->evt_list = HI_NULL;

    return;
}

hi_s32 clt_evt_set_call_back(vfmw_adpt_type type, fun_vfmw_event_report event_report)
{
    switch (type) {
        case ADPT_TYPE_VDEC:
            g_clt_callback.evt_report_vdec = event_report;
            break;

        case ADPT_TYPE_OMX:
            g_clt_callback.evt_report_omxvdec = event_report;
            break;

        default:
            dprint(PRN_ERROR, "%s Unkown Adapter type: %d\n", __func__, type);

            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

clt_event_chan *clt_evt_get_chan_entry(hi_s32 chan_id)
{
    return &g_clt_evt_ctx[chan_id];
}

hi_s32 clt_evt_config_chan(hi_s32 chan_id, clt_evt_chn_cfg *cfg)
{
    clt_event_chan *chan = clt_evt_get_chan_entry(chan_id);

    VFMW_CHECK_SEC_FUNC(memcpy_s(&chan->cfg, sizeof(clt_evt_chn_cfg), cfg, sizeof(clt_evt_chn_cfg)));

    return HI_SUCCESS;
}


