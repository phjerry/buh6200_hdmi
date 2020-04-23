/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "event.h"

#define EVENT_DEFAULT_MAP  (EVNT_ALLOC_MEM |  \
            EVNT_RELEASE_MEM | EVNT_RELEASE_BURST_MEM | EVNT_CANCEL_ALLOC | EVNT_SET_ADDRESS_IN)

typedef struct {
    hi_u32 event_map;
} event_chan;

typedef struct {
    event_chan chan[VFMW_CHAN_NUM];
    fun_vfmw_event_report report;
} event_entry;

static event_entry g_event_enty = {{{0x0}}, HI_NULL};

static event_entry *event_get_entry(hi_void)
{
    return &g_event_enty;
}

static event_chan *event_get_chan(hi_s32 chan_id)
{
    if (chan_id < 0 || chan_id >= VFMW_CHAN_NUM) {
        return HI_NULL;
    }
    return &g_event_enty.chan[chan_id];
}

hi_s32 event_set_map(hi_s32 chan_id, hi_u32 event_map)
{
    event_chan *chan = HI_NULL;

    chan = event_get_chan(chan_id);
    if (chan == HI_NULL) {
        return HI_FAILURE;
    }

    chan->event_map = event_map;

    return HI_SUCCESS;
}

hi_void event_set_callback(fun_vfmw_event_report report)
{
    event_entry *entry = event_get_entry();

    entry->report = report;

    return;
}

hi_void event_report(hi_s32 inst_id, hi_s32 type, hi_void *args, hi_s32 len)
{
    event_chan *chan = HI_NULL;
    event_entry *entry = event_get_entry();
    hi_u32 event_map = EVENT_DEFAULT_MAP;

    chan = event_get_chan(inst_id);
    if (chan == HI_NULL) {
        return;
    }

    event_map |= chan->event_map;
    if ((event_map & ((hi_u32)type)) != 0 && entry->report != HI_NULL) {
        (hi_void)(entry->report)(inst_id, type, args, len);
    }

    return;
}


