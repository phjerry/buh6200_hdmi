/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "client_state.h"
#include "comm_share.h"
#include "vfmw_osal.h"
#include "dbg.h"

typedef struct {
    vfmw_chan_info *state;
} clt_sta_chan;

static clt_sta_chan g_clt_sta_ctx[VFMW_CHAN_NUM];

static INLINE clt_sta_chan *clt_sta_get_chan_entry(hi_s32 chan_id)
{
    return &g_clt_sta_ctx[chan_id];
}

hi_s32 clt_sta_init(hi_void)
{
    VFMW_CHECK_SEC_FUNC(memset_s(g_clt_sta_ctx,
                                 sizeof(clt_sta_chan) * VFMW_CHAN_NUM, 0, sizeof(clt_sta_chan) * VFMW_CHAN_NUM));

    return HI_SUCCESS;
}

hi_u32 clt_sta_get_mem_size(hi_void)
{
    return sizeof(vfmw_chan_info);
}

hi_s32 clt_sta_bind_queue(hi_s32 chan_id, hi_void *list_addr)
{
    clt_sta_chan *chan = clt_sta_get_chan_entry(chan_id);

    chan->state = (vfmw_chan_info *)list_addr;

    return HI_SUCCESS;
}

hi_void clt_sta_unbind_queue(hi_s32 chan_id)
{
    clt_sta_chan *chan = clt_sta_get_chan_entry(chan_id);

    chan->state = HI_NULL;

    return;
}

hi_s32 clt_sta_get_chan_state(hi_s32 chan_id, vfmw_chan_info *state)
{
    clt_sta_chan *chan = clt_sta_get_chan_entry(chan_id);

    VFMW_ASSERT_RET((chan->state != HI_NULL), HI_FAILURE);

    VFMW_CHECK_SEC_FUNC(memcpy_s(state, sizeof(vfmw_chan_info), chan->state, sizeof(vfmw_chan_info)));

    return HI_SUCCESS;
}


