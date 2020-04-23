/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __CLT_STREAM_H__
#define __CLT_STREAM_H__

#include "vfmw.h"
#include "comm_share.h"

typedef struct {
    shr_stm_list *stm_list;
} clt_stm_chn;

hi_s32 clt_stm_init(hi_void);
hi_s32 clt_stm_read(hi_s32 chan_id);
hi_s32 clt_stm_release(hi_s32 chan_id);
hi_s32 clt_stm_reset_queue(hi_s32 chan_id);
hi_u32 clt_stm_get_queue_size(hi_void);
hi_s32 clt_stm_bind_queue(hi_s32 chan_id, hi_void *list_addr);
hi_void clt_stm_unbind_queue(hi_s32 chan_id);
hi_s32 clt_stm_set_intf(hi_s32 chan_id, vfmw_stream_intf *stm_intf);
clt_stm_chn *clt_stm_get_chan_entry(hi_s32 chan_id);
hi_void clt_stm_print(hi_s32 chan_id);

#endif


