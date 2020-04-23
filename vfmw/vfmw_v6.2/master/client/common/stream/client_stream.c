/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "client_stream.h"
#include "vfmw_osal.h"
#include "dbg.h"

static vfmw_stream_intf g_clt_stm_intf[VFMW_CHAN_NUM];
static clt_stm_chn g_clt_stm_ctx[VFMW_CHAN_NUM];

/* ----------------------------INTERNAL FUNCTION------------------------------- */
static hi_s32 clt_stm_get_free_slot(hi_s32 chan_id, vfmw_stream_data **slot)
{
    clt_stm_chn *chan = clt_stm_get_chan_entry(chan_id);
    hi_s32 head_c = chan->stm_list->head_c;
    hi_s32 tail_c = chan->stm_list->tail_c;

    if (tail_c == SHR_STM_INC(head_c) % SHR_STM_QUEUE_SIZE) {
        return HI_FAILURE;
    }

    (*slot) = &(chan->stm_list->stm_data[head_c]);

    return HI_SUCCESS;
}

static hi_s32 clt_stm_get_releasable_slot(hi_s32 chan_id, vfmw_stream_data **slot)
{
    clt_stm_chn *chan = clt_stm_get_chan_entry(chan_id);
    hi_s32 tail_s = chan->stm_list->tail_s;
    hi_s32 tail_c = chan->stm_list->tail_c;

    if (tail_c == tail_s) {
        return HI_FAILURE;
    }

    (*slot) = &chan->stm_list->stm_data[tail_c];

    return HI_SUCCESS;
}

static hi_void clt_stm_clear_chan(hi_s32 chan_id)
{
    clt_stm_chn *chan = clt_stm_get_chan_entry(chan_id);

    chan->stm_list->head_c = 0;
    chan->stm_list->head_s = 0;
    chan->stm_list->tail_c = 0;
    chan->stm_list->tail_s = 0;
}

/* ----------------------------EXTERNAL FUNCTION---------------------------- */
hi_s32 clt_stm_init(hi_void)
{
    VFMW_CHECK_SEC_FUNC(memset_s(g_clt_stm_ctx,
                                 sizeof(clt_stm_chn) * VFMW_CHAN_NUM, 0, sizeof(clt_stm_chn) * VFMW_CHAN_NUM));
    VFMW_CHECK_SEC_FUNC(memset_s(g_clt_stm_intf,
        sizeof(vfmw_stream_intf) * VFMW_CHAN_NUM, 0, sizeof(vfmw_stream_intf) * VFMW_CHAN_NUM));

    return HI_SUCCESS;
}

hi_s32 clt_stm_read(hi_s32 chan_id)
{
    vfmw_stream_data *stm_data = HI_NULL;
    clt_stm_chn *chan = clt_stm_get_chan_entry(chan_id);

    VFMW_ASSERT_RET((g_clt_stm_intf[chan_id].read_stream != HI_NULL), HI_FAILURE);
    VFMW_ASSERT_RET((chan->stm_list != HI_NULL), HI_FAILURE);

    while (clt_stm_get_free_slot(chan_id, &stm_data) == HI_SUCCESS) {
        if (0 == g_clt_stm_intf[chan_id].read_stream(g_clt_stm_intf[chan_id].stream_provider_inst_id, stm_data)) {
            chan->stm_list->head_c = SHR_STM_INC(chan->stm_list->head_c);
        } else {
            break;
        }
    }

    return HI_SUCCESS;
}

hi_s32 clt_stm_release(hi_s32 chan_id)
{
    vfmw_stream_data *stm_data = HI_NULL;
    clt_stm_chn *chan = clt_stm_get_chan_entry(chan_id);

    VFMW_ASSERT_RET((g_clt_stm_intf[chan_id].read_stream != HI_NULL), HI_FAILURE);
    VFMW_ASSERT_RET((chan->stm_list != HI_NULL), HI_FAILURE);

    while (clt_stm_get_releasable_slot(chan_id, &stm_data) == HI_SUCCESS) {
        if (0 == g_clt_stm_intf[chan_id].release_stream(g_clt_stm_intf[chan_id].stream_provider_inst_id, stm_data)) {
            chan->stm_list->tail_c = SHR_STM_INC(chan->stm_list->tail_c);
        } else {
            break;
        }
    }

    return HI_SUCCESS;
}

hi_s32 clt_stm_reset_queue(hi_s32 chan_id)
{
    hi_s32 head_c;
    hi_s32 tail_c;
    vfmw_stream_data *stm_data = HI_NULL;
    clt_stm_chn *chan = clt_stm_get_chan_entry(chan_id);

    VFMW_ASSERT_RET((g_clt_stm_intf[chan_id].read_stream != HI_NULL), HI_FAILURE);
    VFMW_ASSERT_RET((chan->stm_list != HI_NULL), HI_FAILURE);

    head_c = chan->stm_list->head_c;
    tail_c = chan->stm_list->tail_c;
    while (head_c != tail_c) {
        stm_data = &chan->stm_list->stm_data[tail_c];
        if (0 == g_clt_stm_intf[chan_id].release_stream(g_clt_stm_intf[chan_id].stream_provider_inst_id, stm_data)) {
            tail_c = SHR_STM_INC(tail_c);
        } else {
            break;
        }
    }
    clt_stm_clear_chan(chan_id);

    return HI_SUCCESS;
}

hi_u32 clt_stm_get_queue_size(hi_void)
{
    return sizeof(shr_stm_list);
}

hi_s32 clt_stm_bind_queue(hi_s32 chan_id, hi_void *list_addr)
{
    clt_stm_chn *chan = clt_stm_get_chan_entry(chan_id);

    chan->stm_list = (shr_stm_list *)list_addr;

    return HI_SUCCESS;
}

hi_void clt_stm_unbind_queue(hi_s32 chan_id)
{
    clt_stm_chn *chan = clt_stm_get_chan_entry(chan_id);

    chan->stm_list = HI_NULL;

    return;
}

hi_s32 clt_stm_set_intf(hi_s32 chan_id, vfmw_stream_intf *stm_intf)
{
    VFMW_CHECK_SEC_FUNC(memcpy_s(&g_clt_stm_intf[chan_id], sizeof(vfmw_stream_intf),
                                 stm_intf, sizeof(vfmw_stream_intf)));

    return HI_SUCCESS;
}

clt_stm_chn *clt_stm_get_chan_entry(hi_s32 chan_id)
{
    return &g_clt_stm_ctx[chan_id];
}

hi_void clt_stm_print(hi_s32 chan_id)
{
    clt_stm_chn *chan = clt_stm_get_chan_entry(chan_id);

    dprint(PRN_ALWS, "STREAM Queue:head_c:%d tail_c:%d head_s:%d tail_s:%d\n",
           chan->stm_list->head_c, chan->stm_list->tail_c, chan->stm_list->head_s, chan->stm_list->tail_s);
}


