/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __CLT_IMAGE_H__
#define __CLT_IMAGE_H__

#include "vfmw.h"
#include "comm_share.h"

typedef struct {
    hi_u32 id;
    shr_img_list *list;
    hi_u32 frm_cnt;
    hi_u32 last_time_ms;
    hi_u32 frm_rate;
} clt_img_chan;

hi_s32 clt_img_init(hi_void);
hi_s32 clt_img_get(hi_s32 chan_id, vfmw_image *img);
hi_s32 clt_img_release(hi_s32 chan_id, hi_s32 image_id);
hi_s32 clt_img_check(hi_s32 chan_id, vfmw_image *img);
hi_u32 clt_img_get_queue_size(hi_void);
hi_s32 clt_img_bind_queue(hi_s32 chan_id, hi_void *list_addr);
hi_void clt_img_unbind_queue(hi_s32 chan_id);
hi_s32 clt_img_reset_queue(hi_s32 chan_id);
clt_img_chan *clt_img_get_chan_entry(hi_s32 chan_id);
hi_void clt_img_print(hi_s32 chan_id);

#endif


