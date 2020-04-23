/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __CLT_STATE_H__
#define __CLT_STATE_H__

#include "vfmw.h"

hi_s32  clt_sta_init(hi_void);
hi_u32  clt_sta_get_mem_size(hi_void);
hi_s32  clt_sta_bind_queue(hi_s32 chan_id, hi_void *list_addr);
hi_void clt_sta_unbind_queue(hi_s32 chan_id);
hi_s32  clt_sta_get_chan_state(hi_s32 chan_id, vfmw_chan_info *state);

#endif
