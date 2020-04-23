/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: decoder
* Author: sdk
* Create: 2019-04-22
 */

#include "dbg.h"
#include "fence.h"
#include "vfmw_adpt.h"
#include "hi_osal.h"
#include "hi_drv_sys.h"
#include "hi_drv_video.h"

static fence_entry g_fence_entry = {0};

fence_entry *fence_get_entry(hi_void)
{
    return &g_fence_entry;
}

fence_chan *fence_get_chan(hi_u32 chan_id)
{
    fence_entry *entry = HI_NULL;
    fence_chan *chan = HI_NULL;

    entry = fence_get_entry();
    chan = entry->chan[chan_id];

    return chan;
}

hi_s32 fence_init(fence_init_param *param)
{
    fence_entry *entry = HI_NULL;

    entry = fence_get_entry();
    entry->rls_image = param->rls_image;
    entry->chk_image = param->chk_image;
    entry->fence_frm_en = 1;
    entry->src_wait_time = 0;
    entry->sink_wait_time = 0;

    entry->inited = HI_TRUE;

    return HI_SUCCESS;
}

hi_s32 fence_exit(hi_void)
{
    fence_entry *entry = HI_NULL;

    entry = fence_get_entry();

    entry->inited = HI_FALSE;

    return HI_SUCCESS;
}

hi_s32 fence_create_chan(hi_u32 chan_id, hi_void *args)
{
    hi_s32 idx;
    fence_chan *chan = HI_NULL;
    fence_entry *entry = fence_get_entry();

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(entry->inited == HI_TRUE, HI_FAILURE);

    chan = fence_get_chan(chan_id);
    if (chan != HI_NULL) {
        dprint(PRN_ERROR, "%s chan %d exist\n", __func__, chan_id);
        return HI_FAILURE;
    }

    chan = OS_ALLOC_VIR("fence_inst", sizeof(fence_chan));
    if (chan == HI_NULL) {
        return HI_FAILURE;
    }

    VFMW_CHECK_SEC_FUNC(memset_s(chan, sizeof(fence_chan), 0, sizeof(fence_chan)));
    chan->created = HI_TRUE;
    chan->chan_id = chan_id;
    entry->chan[chan_id] = chan;

    for (idx = 0; idx < FENCE_FRM_NUM; idx++) {
        chan->fence_frm[idx].src_fence = FENCE_INVALID;
        chan->fence_frm[idx].sink_fence = FENCE_INVALID;
    }

    return HI_SUCCESS;
}

static hi_void fence_free_frame(fence_chan *chan)
{
    hi_s32 i;
    fence_frame *frm = HI_NULL;

    for (i = 0; i < FENCE_FRM_NUM; i++) {
        frm = &(chan->fence_frm[i]);
        if (frm->is_valid && frm->sink_fence != FENCE_INVALID) {
            osal_fence_destroy(HI_ID_VFMW, frm->sink_fence);
            frm->sink_fence = -1;
        }
    }
}

hi_s32 fence_destroy_chan(hi_u32 chan_id)
{
    fence_chan *chan = HI_NULL;
    fence_entry *entry = fence_get_entry();

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(entry->inited == HI_TRUE, HI_FAILURE);

    chan = fence_get_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "%s chan %d is null\n", __func__, chan_id);
        return HI_FAILURE;
    }

    fence_free_frame(chan);
    entry->chan[chan_id] = HI_NULL;
    OS_FREE_VIR(chan);

    return HI_SUCCESS;
}

hi_s32 fence_set_frm_flag(hi_u32 chan_id, vfmw_image *img)
{
    fence_entry *entry = HI_NULL;

    entry = fence_get_entry();
    if (entry->fence_frm_en == 1) {
        img->user_tag = FENCE_ENABLE;
    } else {
        img->user_tag = FENCE_DISABLE;
    }

    return HI_SUCCESS;
}

hi_s32 fence_proc_bypass(hi_u32 chan_id, hi_s32 *fence, vfmw_image *img)
{
    hi_s32 ret;
    hi_drv_fence_info *priv = HI_NULL;

    ret = osal_fence_create(HI_ID_VFMW, fence, sizeof(hi_drv_fence_info));
    if (ret != 0) {
        dprint(PRN_ERROR, "%s create fence fail\n", __func__);
        return HI_FAILURE;
    }

    ret = osal_fence_acquire_private(HI_ID_VFMW, *fence, (hi_void **)&priv);
    if (ret == 0 && priv != HI_NULL) {
        priv->need_display = HI_TRUE;
        osal_fence_signal(*fence);
        osal_fence_release_private(HI_ID_VFMW, *fence, (hi_void *)priv);
        return HI_SUCCESS;
    } else {
        dprint(PRN_ERROR, "%s get src priv fail", __func__);
        osal_fence_destroy(HI_ID_VFMW, *fence);
        *fence = FENCE_INVALID;
        return HI_FAILURE;
    }
}

hi_s32 fence_add_rcv_frm(hi_u32 chan_id, hi_s32 *fence, vfmw_image *img)
{
    hi_s32 ret;
    hi_s32 idx;
    hi_u32 size;
    fence_chan *chan = HI_NULL;
    fence_frame *frm = HI_NULL;
    fence_entry *entry = HI_NULL;

    *fence = FENCE_INVALID;

    entry = fence_get_entry();
    if (entry->fence_frm_en == 0) {
        return HI_SUCCESS;
    }

    if (img->is_3d == 1 || img->is_user_dec == 1) {
        return fence_proc_bypass(chan_id, fence, img);
    }

    chan = fence_get_chan(chan_id);
    for (idx = 0; idx < FENCE_FRM_NUM; idx++) {
        frm = &(chan->fence_frm[idx]);
        if (frm->is_valid == 0) {
            size = sizeof(hi_u32);
            ret = osal_fence_create(HI_ID_VFMW, fence, size);
            if (ret != 0) {
                dprint(PRN_ERROR, "%s create fence fail\n", __func__);
                break;
            }

            frm->src_fence = *fence;
            frm->sink_fence = FENCE_INVALID;
            frm->image_id = img->image_id;
            frm->image_id_1 = img->image_id_1;
            frm->share_idx = img->shr_index;
            frm->src_chk_cnt = 0;
            frm->src_chk_start = OS_GET_TIME_MS();
            frm->src_frm_cnt = chan->src_frm_cnt++;
            frm->is_valid = 1;
            chan->fence_create++;
            return HI_SUCCESS;
        }
    }

    dprint(PRN_ERROR, "%s fail\n", __func__);

    return HI_FAILURE;
}

hi_s32 fence_add_rls_frm(hi_u32 chan_id, hi_s32 src_fence, hi_s32 sink_fence, vfmw_image *img)
{
    hi_s32 idx;
    fence_chan *chan = HI_NULL;
    fence_frame *frm = HI_NULL;

    chan = fence_get_chan(chan_id);
    for (idx = 0; idx < FENCE_FRM_NUM; idx++) {
        frm = &(chan->fence_frm[idx]);
        if (frm->is_valid == 0) {
            frm->sink_fence = sink_fence;
            frm->src_fence = FENCE_INVALID;
            frm->rls_src_fence = src_fence;
            frm->image_id = img->image_id;
            frm->image_id_1 = img->image_id_1;
            frm->sink_chk_cnt = 0;
            frm->sink_chk_start = OS_GET_TIME_MS();
            frm->sink_frm_cnt = chan->sink_frm_cnt++;
            frm->is_valid = 1;
            if (sink_fence != FENCE_INVALID) {
                chan->fence_wait++;
            }
            return HI_SUCCESS;
        }
    }

    dprint(PRN_ERROR, "%s fail\n", __func__);

    return HI_FAILURE;
}

hi_s32 fence_proc_frm_src(hi_s32 chan_id, fence_frame *frm, vfmw_image *img)
{
    hi_s32 ret;
    fence_chan *chan = HI_NULL;
    fence_entry *entry = HI_NULL;
    hi_drv_fence_info *priv = HI_NULL;

    entry = fence_get_entry();
    chan = fence_get_chan(chan_id);

    frm->src_chk_cnt++;
    if (entry->chk_image(chan_id, img) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    ret = osal_fence_acquire_private(HI_ID_VFMW, frm->src_fence, (hi_void **)&priv);
    if (ret == 0 && priv != HI_NULL) {
        if (img->error_level == 0) {
            priv->need_display = HI_TRUE;
        } else {
            priv->need_display = HI_FALSE;
        }
        osal_fence_signal(frm->src_fence);
        osal_fence_release_private(HI_ID_VFMW, frm->src_fence, (hi_void *)priv);
    } else {
        dprint(PRN_ERROR, "%s get src priv fail", __func__);
    }
    frm->src_fence = -1;
    frm->is_valid = 0;
    frm->src_chk_end = OS_GET_TIME_MS();
    chan->fence_signal++;

    if (entry->src_wait_time > 0 &&
        frm->src_chk_end > frm->src_chk_start &&
        frm->src_chk_end - frm->src_chk_start > entry->src_wait_time) {
        dprint(PRN_ALWS, "vfmw fence src[%d] cnt %d time %d\n", frm->src_frm_cnt,
               frm->src_chk_cnt, frm->src_chk_end - frm->src_chk_start);
    }

    return HI_SUCCESS;
}

hi_void fence_proc_frm_sink(hi_s32 chan_id, fence_frame *frm, vfmw_image *img)
{
    hi_s32 ret;
    fence_chan *chan = HI_NULL;
    fence_entry *entry = HI_NULL;

    entry = fence_get_entry();
    chan = fence_get_chan(chan_id);

    if (frm->sink_fence == FENCE_INVALID) {
        if (osal_fence_trywait(frm->rls_src_fence) != 0) {
            return;
        }
        osal_fence_destroy(HI_ID_VFMW, frm->rls_src_fence);
        ret = entry->rls_image(chan_id, img);
        if (ret != HI_SUCCESS) {
            dprint(PRN_ERROR, "call rls_image failed\n");
        }
        frm->sink_fence = FENCE_INVALID;
        frm->rls_src_fence = FENCE_INVALID;
        frm->is_valid = 0;
        return;
    }

    frm->sink_chk_cnt++;
    if (osal_fence_trywait(frm->sink_fence) != 0) {
        return;
    }

    osal_fence_destroy(HI_ID_VFMW, frm->sink_fence);
    frm->sink_fence = FENCE_INVALID;
    frm->rls_src_fence = FENCE_INVALID;
    frm->is_valid = 0;
    frm->sink_chk_end = OS_GET_TIME_MS();
    chan->fence_free++;

    ret = entry->rls_image(chan_id, img);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "call rls_image failed\n");
    }

    if (entry->sink_wait_time > 0 &&
        frm->sink_chk_end > frm->sink_chk_start &&
        frm->sink_chk_end - frm->sink_chk_start > entry->sink_wait_time) {
        dprint(PRN_ALWS, "vfmw fence sink[%d] cnt %d time %d\n", frm->sink_frm_cnt,
               frm->sink_chk_cnt, frm->sink_chk_end - frm->sink_chk_start);
    }
}

hi_s32 fence_proc_frm_state(hi_u32 chan_id)
{
    hi_s32 ret;
    hi_s32 idx;
    vfmw_image tmp_image = {0};
    fence_chan *chan = HI_NULL;
    fence_frame *frm = HI_NULL;
    fence_entry *entry = HI_NULL;

    entry = fence_get_entry();
    chan = fence_get_chan(chan_id);
    if (entry->inited == 0 ||
        entry->rls_image == HI_NULL ||
        entry->chk_image == HI_NULL ||
        chan == HI_NULL ||
        chan->created != HI_TRUE) {
        return 0;
    }

    for (idx = 0; idx < FENCE_FRM_NUM; idx++) {
        frm = &(chan->fence_frm[idx]);
        if (frm->is_valid == 0) {
            continue;
        }

        tmp_image.image_id = frm->image_id;
        tmp_image.image_id_1 = frm->image_id_1;
        tmp_image.shr_index = frm->share_idx;
        tmp_image.is_3d = (tmp_image.image_id_1 != -1) ? 1 : 0;

        if (frm->src_fence != FENCE_INVALID) {
            ret = fence_proc_frm_src(chan_id, frm, &tmp_image);
            if (ret != HI_SUCCESS) {
                dprint(PRN_DBG, "src fence %d process fail!\n", frm->src_fence);
            }
        } else {
            fence_proc_frm_sink(chan_id, frm, &tmp_image);
        }
    }

    return chan->fence_create != chan->fence_signal ? 1 : 0;
}


