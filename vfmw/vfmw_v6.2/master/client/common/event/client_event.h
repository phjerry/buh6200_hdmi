/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __CLT_EVENT_H__
#define __CLT_EVENT_H__

#include "vfmw.h"
#include "comm_share.h"

typedef struct {
    hi_u8 sec_flag;
} clt_evt_chn_cfg;

typedef struct {
    clt_evt_chn_cfg cfg;
    shr_evt_list *evt_list;
} clt_event_chan;

hi_s32 clt_evt_init(hi_void);
hi_s32 clt_evt_process(hi_s32 chan_id);
hi_s32 clt_evt_reset_queue(hi_s32 chan_id);
hi_u32 clt_evt_get_queue_size(hi_void);
hi_s32 clt_evt_bind_queue(hi_s32 chan_id, hi_void *list_addr);
hi_void clt_evt_unbind_queue(hi_s32 chan_id);
hi_s32 clt_evt_set_call_back(vfmw_adpt_type type, fun_vfmw_event_report event_report);
clt_event_chan *clt_evt_get_chan_entry(hi_s32 chan_id);
hi_s32 clt_evt_config_chan(hi_s32 chan_id, clt_evt_chn_cfg *cfg);

#endif


