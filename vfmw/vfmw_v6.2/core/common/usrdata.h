/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __USRDAT_H__
#define __USRDAT_H__

#include "vfmw.h"
#include "dbg.h"

#define USD_OK  0
#define USD_ERR (-1)

#define MAX_USERDATA_NUM 4

hi_s32 usd_open(hi_void);
hi_s32 usd_close(hi_void);
hi_s32 usd_create(hi_s32 chan_id);
hi_s32 usd_destroy(hi_s32 chan_id);
hi_s32 usd_reset(hi_s32 chan_id);
vfmw_usrdat *usd_get(hi_s32 chan_id);
hi_void usd_release(hi_s32 chan_id, hi_void *usrdata);
hi_s32 usd_get_mem_need_size(vfmw_svr_mem_need *mem);

#endif