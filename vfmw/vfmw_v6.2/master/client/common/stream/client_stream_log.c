/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "client_stream_log.h"
#include "client_stream.h"
#include "vfmw_osal.h"

hi_s32 clt_stm_log_read(hi_void *buffer, hi_s32 chan_id)
{
    hi_s32 used_size = 0;
    clt_stm_chn *chan = clt_stm_get_chan_entry(chan_id);

    OS_DUMP_PROC(buffer, 0, &used_size, 0, "  ----------------------SHR STM--------------------------\n");

    if (chan->stm_list != HI_NULL) {
        OS_DUMP_PROC(buffer, 0, &used_size, 0, "  head_c/tail_c/head_s/tail_s     :(%d/%d/%d/%d)\n",
                     chan->stm_list->head_c, chan->stm_list->tail_c, chan->stm_list->head_s, chan->stm_list->tail_s);
    }
    OS_DUMP_PROC(buffer, 0, &used_size, 0, "\n");

    return HI_SUCCESS;
}


