/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "client_event_log.h"
#include "client_event.h"
#include "vfmw_osal.h"

hi_s32 clt_evt_log_read(hi_void *buffer, hi_s32 chan_id)
{
    hi_s32 used_size = 0;
    clt_event_chan *chan = clt_evt_get_chan_entry(chan_id);

    OS_DUMP_PROC(buffer, 0, &used_size, 0, "  ----------------------SHR EVT--------------------------\n");

    if (chan->evt_list != HI_NULL) {
        OS_DUMP_PROC(buffer, 0, &used_size, 0, "  head/tail                   :(%d/%d)\n",
                     chan->evt_list->head, chan->evt_list->tail);
    }
    OS_DUMP_PROC(buffer, 0, &used_size, 0, "\n");

    return HI_SUCCESS;
}


