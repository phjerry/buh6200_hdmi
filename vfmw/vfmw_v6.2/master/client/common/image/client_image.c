/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "client_image.h"
#include "vfmw_osal.h"
#include "dbg.h"
#ifdef VFMW_FENCE_SUPPORT
#include "fence.h"
#endif
#ifdef VFMW_MVC_SUPPORT
#include "mvc_img.h"
#endif

static clt_img_chan g_clt_img_ctx[VFMW_CHAN_NUM];

/* ----------------------------INTERNAL FUNCTION------------------------------- */
static hi_s32 clt_img_state_check(shr_img *shr_img, hi_u32 img_state)
{
    hi_u32 check_rlt;
    hi_u32 top_check_rlt;
    hi_u32 bot_check_rlt;
    hi_u32 field_valid_flag = shr_img->img.format.field_valid_flag;

    if (field_valid_flag == 0) {
        return HI_FALSE;
    }

    top_check_rlt = shr_img->img.dec_over.bits.top_over == img_state;
    bot_check_rlt = shr_img->img.dec_over.bits.bot_over == img_state;

    check_rlt = top_check_rlt | (bot_check_rlt << 1); /* bit 1 is bot field */

    check_rlt &= field_valid_flag;

    switch (img_state) {
        case SHR_IMG_READY_STATE:
            /* valid field all ready */
            return (check_rlt == field_valid_flag);
        case SHR_IMG_ERROR_STATE:
            /* valid field any error */
            return (check_rlt != 0);
        default:
            dprint(PRN_ALWS, "clt_img_state_check img_state = 0x%x hasn't been supported yet!!!\n", img_state);
    }

    return HI_FALSE;
}

static hi_void clt_img_count(hi_s32 chan_id)
{
    hi_u32 cur_time_ms;
    hi_u32 time_period;

    clt_img_chan *chan = clt_img_get_chan_entry(chan_id);
    chan->frm_cnt++;

    cur_time_ms = OS_GET_TIME_MS();
    if (chan->last_time_ms == 0) {
        chan->last_time_ms = cur_time_ms;
    } else {
        time_period = cur_time_ms - chan->last_time_ms;
        if (time_period > 1000) { /* 1000 :a number */
            chan->frm_rate = chan->frm_cnt * 1000 / time_period; /* 1000 :a number */
            chan->frm_cnt = 0;
            chan->last_time_ms = cur_time_ms;
        }
    }

    return;
}

static hi_s32 clt_img_release_by_img_id(clt_img_chan *chan, hi_u32 img_id)
{
    hi_s32 pos;

    for (pos = 0; pos < SHR_IMG_QUEUE_SIZE; pos++) {
        if (chan->list->img_rels[pos].release == 0) {
            chan->list->img_rels[pos].image_id = img_id;
            chan->list->img_rels[pos].release = 1;
            return HI_SUCCESS;
        }
    }

    dprint(PRN_ERROR, "Can not find the image!\n");

    return HI_FAILURE;
}

static hi_s32 clt_img_get_view(clt_img_chan *chan, vfmw_image **param_img)
{
    hi_s32 head_c;
    hi_s32 head_s;
    shr_img *img_que = HI_NULL;
    vfmw_image *que_img = HI_NULL;
#ifdef VFMW_FENCE_SUPPORT
    vfmw_image *src_img = (*param_img);
#endif

    VFMW_ASSERT_RET((chan->list != HI_NULL), HI_FAILURE);

    head_c = chan->list->head_c;
    head_s = chan->list->head_s;
    img_que = chan->list->img_que;

    while (head_c != head_s && clt_img_state_check(&(img_que[head_c]), SHR_IMG_ERROR_STATE)) {
        chan->list->head_c = SHR_IMG_INC(chan->list->head_c);
        if (clt_img_release(chan->id, img_que[head_c].img.image_id) != HI_SUCCESS) {
            dprint(PRN_ERROR, "%s ERROR: image tailc up to headc after headc++.\n");
        }
        head_c = chan->list->head_c;
    }

    if (head_c == head_s) {
        dprint(PRN_DBG, "img queue no (not err) image!\n");
        return HI_FAILURE;
    }
    que_img = &(img_que[head_c].img);

#ifdef VFMW_FENCE_SUPPORT
    if (src_img != HI_NULL && src_img->user_tag == FENCE_ENABLE && que_img->is_3d == 0) {
        if (que_img->disp_info.is_valid != 0) {
            img_que[head_c].ready = 0;
            goto out;
        }
        return HI_FAILURE;
    }
#endif

    if (!(clt_img_state_check(&(img_que[head_c]), SHR_IMG_READY_STATE))) {
        dprint(PRN_DBG, "img is not ready!\n");
        return HI_FAILURE;
    }

#ifdef VFMW_FENCE_SUPPORT
out:
#endif
    chan->list->head_c = SHR_IMG_INC(chan->list->head_c);
    *param_img = que_img;

    return HI_SUCCESS;
}

#ifdef VFMW_MVC_SUPPORT
static hi_s32 clt_img_reque_last_image(clt_img_chan *chan, vfmw_image *img)
{
    hi_s32 head_c;
    shr_img *img_que = chan->list->img_que;

    head_c = SHR_IMG_DEC(chan->list->head_c);
    if (img_que[head_c].img.image_id != img->image_id) {
        dprint(PRN_ERROR, "last image unmatch, reque failed %d %d.\n", img_que[head_c].img.image_id, img->image_id);
        return HI_FAILURE;
    }
    chan->list->head_c = head_c;

    return HI_SUCCESS;
}

static hi_s32 clt_img_get_view1(clt_img_chan *chan, vfmw_image *img0)
{
    hi_s32 ret;
    vfmw_image *img1 = HI_NULL;
    mvc_img_state img1_state = MVC_IMG_NULL;

    if (img0->last_frame == 1) {
        mvc_img_set_view0_last_frame(img0);
        return HI_SUCCESS;
    }

    ret = clt_img_get_view(chan, &img1);
    if (ret != HI_SUCCESS) {
        (hi_void)clt_img_reque_last_image(chan, img0);  /* requeue image0 */
        return HI_FAILURE;
    }

    ret = mvc_img_patch_two_view(img0, img1, &img1_state);
    if (ret != HI_SUCCESS) {
        (hi_void)clt_img_release_by_img_id(chan, img0->image_id);      /* discard image0 */
        if (img1_state == MVC_IMG_REQUE) {
            (hi_void)clt_img_reque_last_image(chan, img1);             /* requeue image1 */
        } else if (img1_state == MVC_IMG_DISCARD) {
            (hi_void)clt_img_release_by_img_id(chan, img1->image_id);  /* discard image1 */
        }
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
#endif

/* ----------------------------EXTERNAL FUNCTION---------------------------- */
hi_s32 clt_img_init(hi_void)
{
    VFMW_CHECK_SEC_FUNC(memset_s(g_clt_img_ctx,
                                 sizeof(clt_img_chan) * VFMW_CHAN_NUM, 0, sizeof(clt_img_chan) * VFMW_CHAN_NUM));

    return HI_SUCCESS;
}

hi_s32 clt_img_get(hi_s32 chan_id, vfmw_image *orig_img)
{
    hi_s32 ret;
    hi_u64 out_frm_ptr;
    vfmw_image *out_img = HI_NULL;
    clt_img_chan *chan = HI_NULL;

    chan = clt_img_get_chan_entry(chan_id);
    VFMW_ASSERT_RET((chan->list != HI_NULL), HI_FAILURE);

    out_img = orig_img;
    out_frm_ptr = orig_img->frame_ptr;

    ret = clt_img_get_view(chan, &orig_img);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

#ifdef VFMW_MVC_SUPPORT
    if (orig_img->is_3d == 1) {
        ret = clt_img_get_view1(chan, orig_img);
        if (ret != HI_SUCCESS) {
            return HI_FAILURE;
        }
    }
#endif

    VFMW_CHECK_SEC_FUNC(memcpy_s(out_img, sizeof(vfmw_image), orig_img, sizeof(vfmw_image)));
    out_img->frame_ptr = out_frm_ptr;

    clt_img_count(chan_id);

    return HI_SUCCESS;
}

hi_s32 clt_img_release(hi_s32 chan_id, hi_s32 image_id)
{
    clt_img_chan *chan = clt_img_get_chan_entry(chan_id);

    VFMW_ASSERT_RET((chan->list != HI_NULL), HI_FAILURE);

    return clt_img_release_by_img_id(chan, image_id);
}

hi_s32 clt_img_check(hi_s32 chan_id, vfmw_image *img)
{
    hi_s32 share_idx;
    shr_img *shr_img = HI_NULL;
    clt_img_chan *chan = clt_img_get_chan_entry(chan_id);

    VFMW_ASSERT_RET((chan->list != HI_NULL), HI_FAILURE);

    share_idx = img->shr_index;
    shr_img = chan->list->img_que;

    if (share_idx < 0 ||
        share_idx >= SHR_IMG_QUEUE_SIZE ||
        img->image_id != shr_img[share_idx].img.image_id) {
        img->error_level = 1;
        dprint(PRN_ERROR, "%s idx %d img %d err\n", __func__, share_idx, img->image_id);
        return HI_SUCCESS;
    }

    if (clt_img_state_check(&(shr_img[share_idx]), SHR_IMG_READY_STATE)) {
        img->error_level = 0;
    } else if (clt_img_state_check(&(shr_img[share_idx]), SHR_IMG_ERROR_STATE)) {
        img->error_level = 1;
    } else {
        return HI_FAILURE;
    }

    shr_img[share_idx].ready = 1;

    return HI_SUCCESS;
}

hi_u32 clt_img_get_queue_size(hi_void)
{
    return sizeof(shr_img_list);
}

hi_s32 clt_img_bind_queue(hi_s32 chan_id, hi_void *list_addr)
{
    clt_img_chan *chan = clt_img_get_chan_entry(chan_id);

    chan->list = (shr_img_list *)list_addr;
    chan->id = chan_id;

    return HI_SUCCESS;
}

hi_void clt_img_unbind_queue(hi_s32 chan_id)
{
    clt_img_chan *chan = clt_img_get_chan_entry(chan_id);
    chan->list = HI_NULL;

    return;
}

hi_s32 clt_img_reset_queue(hi_s32 chan_id)
{
    hi_s32 head_c;
    hi_s32 head_s;
    hi_u32 image_id;
    clt_img_chan *chan = clt_img_get_chan_entry(chan_id);

    VFMW_ASSERT_RET((chan->list != HI_NULL), HI_FAILURE);

    head_c = chan->list->head_c;
    head_s = chan->list->head_s;
    while (head_c != head_s) {
        image_id = chan->list->img_que[head_c].img.image_id;
        head_c = SHR_IMG_INC(head_c);
        if (clt_img_release_by_img_id(chan, image_id) != HI_SUCCESS) {
            dprint(PRN_ERROR, "%s rls imgid %d fail\n", __func__, image_id);
        }
    }

    chan->list->head_c = head_s;
    chan->frm_rate = 0;
    chan->frm_cnt = 0;

    return HI_SUCCESS;
}

clt_img_chan *clt_img_get_chan_entry(hi_s32 chan_id)
{
    return &g_clt_img_ctx[chan_id];
}

hi_void clt_img_print(hi_s32 chan_id)
{
    clt_img_chan *chan = clt_img_get_chan_entry(chan_id);

    dprint(PRN_ALWS, "image Queue:head_c:%d head_s:%d\n",
           chan->list->head_c, chan->list->head_s);
}


