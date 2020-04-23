/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux basic function impl.
 * Author: sdk
 * Create: 2017-05-31
 */

#include <linux/kernel.h>
#include "linux/delay.h"
#include "linux/bitmap.h"
#include "linux/io.h"
#include "linux/interrupt.h"
#include "linux/printk.h"
#include "linux/ratelimit.h"
#include "linux/hisilicon/securec.h"

#include "hi_drv_sys.h"
#include "hi_drv_module.h"
#include "hi_drv_mem.h"
#include "hi_drv_proc.h"
#include "hi_drv_mem.h"
#include "hi_drv_file.h"
#include "hi_type.h"

#include "hal_demux.h"
#include "drv_demux_plyfct.h"
#include "drv_demux_func.h"
#include "drv_demux_define.h"
#include "drv_demux_utils.h"
#include "drv_demux_dscfct.h"

static hi_s32 _dmx_r_play_fct_open_impl(struct dmx_r_play_fct *rplay_fct);
static hi_s32 _dmx_r_play_fct_close_impl(struct dmx_r_play_fct *rplay_fct);
/*************************dmx_r_play_fct_ts_xxx_impl*************************************************/
static hi_s32 _dmx_r_play_fct_ts_attach_pid_ch_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_pid_ch *rpid_ch)
{
    hi_s32 ret;

    dmx_r_get_raw((struct dmx_r_base *)rpid_ch);
    ret = rpid_ch->ops->attach(rpid_ch, (struct dmx_r_base *)rplay_fct);
    if (ret == HI_SUCCESS) {
        /* assign the pidch object */
        rplay_fct->rpid_ch = rpid_ch;
        /* add the pid_ch obj reference */
        dmx_r_get_raw((struct dmx_r_base *)rpid_ch);
    }
    dmx_r_put((struct dmx_r_base *)rpid_ch);

    return ret;
}

static hi_s32 dmx_r_play_fct_ts_attach_pid_ch_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_pid_ch *rpid_ch)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    if (rplay_fct->is_opened != HI_FALSE) {
        HI_ERR_DEMUX("playfct ts not close, can not attach pid channel!\n");
        ret = HI_ERR_DMX_OPENING_CHAN;
        goto out;
    }

    if (unlikely(rplay_fct->rpid_ch != HI_NULL)) {
        HI_ERR_DEMUX("don't allow to attach pidch to one playfct repeatly!\n");
        ret = HI_ERR_DMX_BUSY;
        goto out;
    }

    ret = _dmx_r_play_fct_ts_attach_pid_ch_impl(rplay_fct, rpid_ch);
    if (ret == HI_SUCCESS) {
        rplay_fct->rband = rpid_ch->rband;
    }
out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_ts_detach_pid_ch_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret = HI_FAILURE;

    /* Attention: this function may run two time,
     * first time is the general detach,
     * second time is post detach in _play_fct_destroy_ts.
    */
    if (unlikely(rplay_fct->rpid_ch == HI_NULL)) {
        /* pid channel can be detach without attach */
        HI_DBG_DEMUX("please attach pid_ch first!\n");
        ret = HI_SUCCESS;
        goto out;
    }

    dmx_r_get_raw((struct dmx_r_base *)rplay_fct->rpid_ch);
    ret = rplay_fct->rpid_ch->ops->detach(rplay_fct->rpid_ch, (struct dmx_r_base *)rplay_fct);
    dmx_r_put((struct dmx_r_base *)rplay_fct->rpid_ch);
    if (ret == HI_SUCCESS) {
        /* if this is the last pidch of playfct */
        DMX_NULL_POINTER_RETURN(rplay_fct->rpid_ch->rband);
        dmx_hal_band_clear_chan(rplay_fct->base.mgmt, rplay_fct->rpid_ch->rband->base.id);

        /* release the pid_ch obj */
        dmx_r_put((struct dmx_r_base *)rplay_fct->rpid_ch);
        /* set the pidch object as null */
        rplay_fct->rpid_ch = HI_NULL;
    }

out:
    return ret;
}

static hi_s32 dmx_r_play_fct_ts_detach_pid_ch_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    if (rplay_fct->is_opened != HI_FALSE) {
        HI_ERR_DEMUX("playfct ts not close, can not detach pid channel!\n");
        ret = HI_ERR_DMX_OPENING_CHAN;
        goto out;
    }

    ret = _dmx_r_play_fct_ts_detach_pid_ch_impl(rplay_fct);

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_ts_acquire_buf_impl(struct dmx_r_play_fct *rplay_fct, hi_u32 acq_num, hi_u32 time_out,
    hi_u32 *acqed_num, dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    struct dmx_r_buf *rbuf = rplay_fct->rbuf;

    if (unlikely(rplay_fct->is_opened != HI_TRUE)) {
        HI_WARN_DEMUX("play[%u] not open yet!\n", rplay_fct->base.id);
        return HI_ERR_DMX_NOT_OPEN_CHAN;
    }

    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->acquire_buf(rbuf, acq_num, time_out, acqed_num, play_fct_buf);
    dmx_r_put((struct dmx_r_base *)rbuf);

    return ret;
}

static hi_s32 dmx_r_play_fct_ts_acquire_buf_impl(struct dmx_r_play_fct *rplay_fct, hi_u32 acq_num, hi_u32 time_out,
    hi_u32 *acqed_num, dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_ts_acquire_buf_impl(rplay_fct, acq_num, time_out, acqed_num, play_fct_buf);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_ts_release_buf_impl(struct dmx_r_play_fct *rplay_fct, hi_u32 rel_num,
                                                  dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    struct dmx_mgmt        *mgmt = rplay_fct->base.mgmt;
    struct dmx_r_buf       *rbuf = rplay_fct->rbuf;
    struct dmx_r_pid_ch    *rpid_ch = rplay_fct->rpid_ch;

    DMX_NULL_POINTER_RETURN(mgmt);
    DMX_NULL_POINTER_RETURN(rbuf);
    DMX_NULL_POINTER_RETURN(rpid_ch);

    if (unlikely(rplay_fct->is_opened != HI_TRUE)) {
        HI_WARN_DEMUX("play[%u] not open yet!\n", rplay_fct->base.id);
        return HI_ERR_DMX_NOT_OPEN_CHAN;
    }

    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->release_buf(rbuf, rel_num, play_fct_buf);
    if (ret == HI_SUCCESS) {
        /* check the pc buffer ap status */
        dmx_cls_pcbuf_ap_status(mgmt, rpid_ch);
    }
    dmx_r_put((struct dmx_r_base *)rbuf);

    return ret;
}

static hi_s32 dmx_r_play_fct_ts_release_buf_impl(struct dmx_r_play_fct *rplay_fct, hi_u32 rel_num,
                                                 dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_ts_release_buf_impl(rplay_fct, rel_num, play_fct_buf);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 dmx_r_play_fct_ts_suspend_impl(struct dmx_r_play_fct *rplay_fct)
{
    struct dmx_r_pid_ch *tmp_rpid_ch = HI_NULL;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    tmp_rpid_ch = rplay_fct->rpid_ch;

    dmx_dofunc_no_return(_dmx_r_play_fct_close_impl(rplay_fct));
    dmx_dofunc_no_return(_dmx_r_play_fct_ts_detach_pid_ch_impl(rplay_fct));

    rplay_fct->rpid_ch = tmp_rpid_ch;

    osal_mutex_unlock(&rplay_fct->lock);

    return HI_SUCCESS;
}

static hi_s32 dmx_r_play_fct_ts_resume_impl(struct dmx_r_play_fct *rplay_fct)
{
    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    dmx_dofunc_no_return(_dmx_r_play_fct_ts_attach_pid_ch_impl(rplay_fct, rplay_fct->rpid_ch));
    dmx_dofunc_no_return(_dmx_r_play_fct_open_impl(rplay_fct));

    osal_mutex_unlock(&rplay_fct->lock);

    return HI_SUCCESS;
}

struct dmx_r_playfct_ts_ops g_dmx_play_fct_ts_ops = {

    .attach_pid_ch      = dmx_r_play_fct_ts_attach_pid_ch_impl,
    .detach_pid_ch      = dmx_r_play_fct_ts_detach_pid_ch_impl,
    .acquire_buf        = dmx_r_play_fct_ts_acquire_buf_impl,
    .release_buf        = dmx_r_play_fct_ts_release_buf_impl,

    .suspend            = dmx_r_play_fct_ts_suspend_impl,
    .resume             = dmx_r_play_fct_ts_resume_impl,
};

/* declaration the function define below */
static hi_s32 play_fct_destroy_flt(hi_handle flt_handle);

/*************************dmx_r_play_fct_sec_xxx_impl*************************************************/
static hi_s32 dmx_r_play_fct_sec_set_attrs_impl(struct dmx_r_play_fct *rplay_fct, const dmx_play_attrs *attr)
{
    hi_s32 ret;
    struct dmx_buf_attrs buf_attrs;

    struct dmx_r_buf *rbuf = rplay_fct->rbuf;

    DMX_NULL_POINTER_RETURN(rbuf);

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    /* get buffer attrs */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->get_attrs(rbuf, &buf_attrs);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("get rbuf attrs failed! ret = %#x\n", ret);
        goto exit;
    }

    if ((attr->buf_size != buf_attrs.buf_size) ||
        (attr->secure_mode != buf_attrs.secure_mode) ||
        (attr->type != rplay_fct->play_fct_type) ||
        (attr->data_mode != rplay_fct->data_mode)) {
        HI_ERR_DEMUX("not support change attr of buf size/chan type/chan output/secure mode!");
        ret = HI_ERR_DMX_NOT_SUPPORT;
        goto exit;
    }

    switch (attr->crc_mode) {
        case DMX_FLT_CRC_MODE_FORBID:
        case DMX_FLT_CRC_MODE_FORCE_AND_DISCARD:
        case DMX_FLT_CRC_MODE_FORCE_AND_SEND:
        case DMX_FLT_CRC_MODE_BY_SYNTAX_AND_DISCARD:
        case DMX_FLT_CRC_MODE_BY_SYNTAX_AND_SEND: {
            rplay_fct->crc_mode = attr->crc_mode;
            dmx_hal_flt_set_ctl_crc(rplay_fct->base.mgmt, attr->crc_mode);
            break;
        }
        default : {
            HI_ERR_DEMUX("invalid crc mode %u\n", attr->crc_mode);
            ret = HI_ERR_DMX_INVALID_PARA;
            goto exit;
        }
    }
    ret = HI_SUCCESS;

exit:
    dmx_r_put((struct dmx_r_base *)rbuf);

    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_sec_attach_pid_ch_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_pid_ch *rpid_ch)
{
    hi_s32 ret;
    struct list_head *flt_node = HI_NULL;
    struct dmx_r_playfct_pes_sec *rplay_fct_sec = (struct dmx_r_playfct_pes_sec *)rplay_fct;
    flt_attach_attr attach_attr = {0};

    /* get the buf obj and put */
    dmx_r_get_raw((struct dmx_r_base *)rpid_ch);
    ret = rpid_ch->ops->attach(rpid_ch, (struct dmx_r_base *)rplay_fct);
    if (ret == HI_SUCCESS) {
        /* assign the pidch object */
        rplay_fct->rpid_ch = rpid_ch;
        /* add the pid_ch obj reference */
        dmx_r_get_raw((struct dmx_r_base *)rpid_ch);
    }
    dmx_r_put((struct dmx_r_base *)rpid_ch);

    if (ret != HI_SUCCESS) {
        goto out;
    }

    /* add flt first, then set pid will reset the flt, need add flt again */
    osal_mutex_lock(&rplay_fct_sec->play_pes_sec.play_flt_list_lock);
    list_for_each(flt_node, &rplay_fct_sec->play_pes_sec.play_flt_head) {
        struct dmx_r_flt *rflt = list_entry(flt_node, struct dmx_r_flt, node1);
        dmx_r_get_raw((struct dmx_r_base *)rflt);
        attach_attr.flt_index = rflt->flt_index;
        attach_attr.pes_sec_id = rplay_fct_sec->play_pes_sec.pes_sec_chan_id;
        attach_attr.buf_id = rplay_fct->rbuf->base.id;
        attach_attr.crc_mode = rplay_fct_sec->play_pes_sec.crc_mode;
        rflt->rplay_fct = HI_NULL;
        ret = rflt->ops->attach(rflt, rplay_fct, &attach_attr);
        if (ret != HI_SUCCESS) {
            dmx_r_put((struct dmx_r_base *)rflt);
            goto out1;
        }
        dmx_r_put((struct dmx_r_base *)rflt);
    }

out1:
    osal_mutex_unlock(&rplay_fct_sec->play_pes_sec.play_flt_list_lock);

out:
    return ret;
}

static hi_s32 dmx_r_play_fct_sec_attach_pid_ch_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_pid_ch *rpid_ch)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    if (rplay_fct->is_opened != HI_FALSE) {
        HI_ERR_DEMUX("playfct sec not close, can not attach pid channel!\n");
        ret = HI_ERR_DMX_OPENING_CHAN;
        goto out;
    }

    if (unlikely(rplay_fct->rpid_ch != HI_NULL)) {
        HI_ERR_DEMUX("don't allow to attach pidch to one playfct repeatly!\n");
        ret = HI_ERR_DMX_BUSY;
        goto out;
    }

    ret = _dmx_r_play_fct_sec_attach_pid_ch_impl(rplay_fct, rpid_ch);
    if (ret == HI_SUCCESS) {
        rplay_fct->rband = rpid_ch->rband;
    }
out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_sec_detach_pid_ch_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret = HI_FAILURE;

    struct dmx_r_playfct_pes_sec *rplay_fct_sec = (struct dmx_r_playfct_pes_sec *)rplay_fct;
    /* Attention: this function may run two time,
     * first time is the general detach,
     * second time is post detach in _play_fct_destroy_sec.
    */
    if (unlikely(rplay_fct->rpid_ch == HI_NULL)) {
        /* pid channel can be detach without attach */
        HI_DBG_DEMUX("please attach pid_ch first!\n");
        ret = HI_SUCCESS;
        goto out;
    }

    dmx_r_get_raw((struct dmx_r_base *)rplay_fct->rpid_ch);
    ret = rplay_fct->rpid_ch->ops->detach(rplay_fct->rpid_ch, (struct dmx_r_base *)rplay_fct);
    dmx_r_put((struct dmx_r_base *)rplay_fct->rpid_ch);
    if (ret == HI_SUCCESS) {
        /* if this is the last pidch of playfct */
        DMX_NULL_POINTER_RETURN(rplay_fct->rpid_ch->rband);
        dmx_hal_band_clear_chan(rplay_fct->base.mgmt, rplay_fct->rpid_ch->rband->base.id);

        /* release the pid_ch obj */
        dmx_r_put((struct dmx_r_base *)rplay_fct->rpid_ch);
        /* set the pidch object as null */
        rplay_fct->rpid_ch = HI_NULL;
    }

    /* if there is no pidch in the section channel, we will clear the section channle */
    if (likely(rplay_fct->rpid_ch == HI_NULL)) {
        /* clear the pes section channel */
        dmx_hal_flt_clear_pes_sec_chan(rplay_fct->base.mgmt, rplay_fct_sec->play_pes_sec.pes_sec_chan_id);
    }

out:
    return ret;
}

static hi_s32 dmx_r_play_fct_sec_detach_pid_ch_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    if (rplay_fct->is_opened != HI_FALSE) {
        HI_ERR_DEMUX("playfct sec not close, can not detach pid channel!\n");
        ret = HI_ERR_DMX_OPENING_CHAN;
        goto out;
    }

    ret = _dmx_r_play_fct_sec_detach_pid_ch_impl(rplay_fct);

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_sec_add_flt_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 flt_index;
    flt_attach_attr attach_attr = {0};
    /* do something about hardware */
    struct dmx_r_playfct_pes_sec *rplay_fct_sec = (struct dmx_r_playfct_pes_sec *)rplay_fct;

    osal_mutex_lock(&rplay_fct_sec->play_pes_sec.play_flt_list_lock);
    flt_index = find_first_zero_bit(rplay_fct_sec->play_pes_sec.play_flt_bitmap,
                                    DMX_FLT_CNT_PER_PLAYFCT);
    if (!(flt_index < DMX_FLT_CNT_PER_PLAYFCT)) {
        HI_ERR_DEMUX("flt is full(32) in pes chan[%d]!\n",
                     rplay_fct_sec->play_pes_sec.pes_sec_chan_id);
        ret = HI_ERR_DMX_NO_RESOURCE;
        goto out;
    }

    attach_attr.flt_index = flt_index;
    attach_attr.pes_sec_id = rplay_fct_sec->play_pes_sec.pes_sec_chan_id;
    attach_attr.buf_id = rplay_fct->rbuf->base.id;
    attach_attr.crc_mode = rplay_fct_sec->play_pes_sec.crc_mode;

    dmx_r_get_raw((struct dmx_r_base *)rflt);
    ret = rflt->ops->attach(rflt, rplay_fct, &attach_attr);
    if (ret != HI_SUCCESS) {
        dmx_r_put((struct dmx_r_base *)rflt);
        goto out;
    }
    dmx_r_put((struct dmx_r_base *)rflt);

    set_bit(flt_index, rplay_fct_sec->play_pes_sec.play_flt_bitmap);

    list_add_tail(&rflt->node1, &rplay_fct_sec->play_pes_sec.play_flt_head);

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct_sec->play_pes_sec.play_flt_list_lock);
    return ret;
}

static hi_s32 dmx_r_play_fct_sec_add_flt_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_sec_add_flt_impl(rplay_fct, rflt);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    /* get the flt obj, it will be put in del or del all flt */
    dmx_r_get_raw((struct dmx_r_base *)rflt);

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_sec_del_flt_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 flt_index;
    unsigned long mask;
    unsigned long *p = HI_NULL;

    /* do something about hardware */
    struct dmx_r_playfct_pes_sec *rplay_fct_sec = (struct dmx_r_playfct_pes_sec *)rplay_fct;

    osal_mutex_lock(&rplay_fct_sec->play_pes_sec.play_flt_list_lock);

    /* check if the paly facility and filter match */
    if (rflt->rplay_fct != rplay_fct) {
        osal_mutex_unlock(&rplay_fct_sec->play_pes_sec.play_flt_list_lock);
        HI_ERR_DEMUX("filter and play fct is not match!flt_handle = %#x, play_handle = %#x\n",
            rflt->flt_handle, rplay_fct->play_fct_handle);
        ret = HI_ERR_DMX_UNMATCH_FILTER;
        goto out;
    }

    flt_index = rflt->flt_index;
    mask = BIT_MASK(flt_index);
    p = ((unsigned long *)rplay_fct_sec->play_pes_sec.play_flt_bitmap) + BIT_WORD(flt_index);
    if (!(*p & mask)) {
        osal_mutex_unlock(&rplay_fct_sec->play_pes_sec.play_flt_list_lock);
        HI_ERR_DEMUX("flt_index(%d) is invalid.\n", flt_index);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    /* clear the flt_index bitmap */
    clear_bit(flt_index, rplay_fct_sec->play_pes_sec.play_flt_bitmap);

    /* delete the node from playfct list */
    list_del(&rflt->node1);
    osal_mutex_unlock(&rplay_fct_sec->play_pes_sec.play_flt_list_lock);

    dmx_r_get_raw((struct dmx_r_base *)rflt);
    ret = rflt->ops->destroy(rflt);
    if (ret != HI_SUCCESS) {
        dmx_r_put((struct dmx_r_base *)rflt);
        goto out;
    }

    dmx_r_put((struct dmx_r_base *)rflt);

out:
    return ret;
}

static hi_s32 dmx_r_play_fct_sec_del_flt_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_sec_del_flt_impl(rplay_fct, rflt);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    /* put the flt obj */
    dmx_r_put((struct dmx_r_base *)rflt);

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_sec_del_all_flt_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret;

    struct list_head *node = HI_NULL;
    struct list_head *tmp_node = HI_NULL;

    struct dmx_r_playfct_pes_sec *rplay_fct_sec = (struct dmx_r_playfct_pes_sec *)rplay_fct;

    osal_mutex_lock(&rplay_fct_sec->play_pes_sec.play_flt_list_lock);
    list_for_each_safe(node, tmp_node, &rplay_fct_sec->play_pes_sec.play_flt_head) {
        struct dmx_r_flt *rflt = list_entry(node, struct dmx_r_flt, node1);

        /* delete the entry from the list and destroy the flt handle */
        list_del(&rflt->node1);

        if (rflt->flt_handle != HI_INVALID_HANDLE) {
            ret = play_fct_destroy_flt(rflt->flt_handle);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("playfct destroy filter[0x%x] failed!\n", rflt->flt_handle);
            }
        }
        /* put the flt obj */
        dmx_r_put((struct dmx_r_base *)rflt);
    }
    /* clear all the bitmap */
    bitmap_zero(rplay_fct_sec->play_pes_sec.play_flt_bitmap, DMX_FLT_CNT_PER_PLAYFCT);

    osal_mutex_unlock(&rplay_fct_sec->play_pes_sec.play_flt_list_lock);

    return HI_SUCCESS;
}

static hi_s32 dmx_r_play_fct_sec_del_all_flt_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_sec_del_all_flt_impl(rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_sec_update_flt_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt,
    const dmx_filter_attrs *attrs)
{
    hi_s32 ret;

    dmx_r_get_raw((struct dmx_r_base *)rflt);
    ret = rflt->ops->set_attrs(rflt, attrs);
    if (ret == HI_SUCCESS) {
        goto out;
    }

out:
    dmx_r_put((struct dmx_r_base *)rflt);
    return ret;
}

static hi_s32 dmx_r_play_fct_sec_update_flt_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt,
    const dmx_filter_attrs *attrs)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_sec_update_flt_impl(rplay_fct, rflt, attrs);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_sec_get_flt_attrs_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt,
    dmx_filter_attrs *attrs)
{
    hi_s32 ret;

    /* get the flt obj */
    dmx_r_get_raw((struct dmx_r_base *)rflt);

    ret = rflt->ops->get_attrs(rflt, attrs);
    if (ret == HI_SUCCESS) {
        goto out;
    }

out:
    dmx_r_put((struct dmx_r_base *)rflt);
    return ret;
}

static hi_s32 dmx_r_play_fct_sec_get_flt_attrs_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt,
    dmx_filter_attrs *attrs)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_sec_get_flt_attrs_impl(rplay_fct, rflt, attrs);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_sec_acquire_buf_impl(struct dmx_r_play_fct *rplay_fct, hi_u32 acq_num, hi_u32 time_out,
    hi_u32 *acqed_num, dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    struct dmx_r_buf *rbuf = rplay_fct->rbuf;
    DMX_NULL_POINTER_RETURN(rbuf);

    if (unlikely(rplay_fct->is_opened != HI_TRUE)) {
        HI_WARN_DEMUX("play[%u] not open yet!\n", rplay_fct->base.id);
        return HI_ERR_DMX_NOT_OPEN_CHAN;
    }

    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->acquire_buf(rbuf, acq_num, time_out, acqed_num, play_fct_buf);
    dmx_r_put((struct dmx_r_base *)rbuf);

    return ret;
}

static hi_s32 dmx_r_play_fct_sec_acquire_buf_impl(struct dmx_r_play_fct *rplay_fct, hi_u32 acq_num, hi_u32 time_out,
                                                  hi_u32 *acqed_num, dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_sec_acquire_buf_impl(rplay_fct, acq_num, time_out, acqed_num, play_fct_buf);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_sec_release_buf_impl(struct dmx_r_play_fct *rplay_fct, hi_u32 rel_num,
                                                   dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    struct dmx_mgmt        *mgmt = rplay_fct->base.mgmt;
    struct dmx_r_buf       *rbuf = rplay_fct->rbuf;
    struct dmx_r_pid_ch     *rpid_ch = rplay_fct->rpid_ch;

    DMX_NULL_POINTER_RETURN(mgmt);
    DMX_NULL_POINTER_RETURN(rbuf);
    DMX_NULL_POINTER_RETURN(rpid_ch);

    if (unlikely(rplay_fct->is_opened != HI_TRUE)) {
        HI_WARN_DEMUX("play[%u] not open yet!\n", rplay_fct->base.id);
        return HI_ERR_DMX_NOT_OPEN_CHAN;
    }

    /* release the buffer */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->release_buf(rbuf, rel_num, play_fct_buf);
    if (ret == HI_SUCCESS) {
        /* check the pc buffer ap status */
        dmx_cls_pcbuf_ap_status(mgmt, rpid_ch);
    }
    dmx_r_put((struct dmx_r_base *)rbuf);

    return ret;
}

static hi_s32 dmx_r_play_fct_sec_release_buf_impl(struct dmx_r_play_fct *rplay_fct, hi_u32 rel_num,
                                                  dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_sec_release_buf_impl(rplay_fct, rel_num, play_fct_buf);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 dmx_r_play_fct_sec_suspend_impl(struct dmx_r_play_fct *rplay_fct)
{
    struct dmx_r_pid_ch *tmp_rpid_ch = HI_NULL;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    /* store the rpid_ch temporarilly */
    tmp_rpid_ch = rplay_fct->rpid_ch;

    dmx_dofunc_no_return(_dmx_r_play_fct_close_impl(rplay_fct));
    dmx_dofunc_no_return(_dmx_r_play_fct_sec_attach_pid_ch_impl(rplay_fct, rplay_fct->rpid_ch));

    rplay_fct->rpid_ch = tmp_rpid_ch;

    osal_mutex_unlock(&rplay_fct->lock);

    return HI_SUCCESS;
}

static hi_s32 dmx_r_play_fct_sec_resume_impl(struct dmx_r_play_fct *rplay_fct)
{
    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    dmx_dofunc_no_return(_dmx_r_play_fct_sec_attach_pid_ch_impl(rplay_fct, rplay_fct->rpid_ch));
    dmx_dofunc_no_return(_dmx_r_play_fct_open_impl(rplay_fct));

    osal_mutex_unlock(&rplay_fct->lock);

    return HI_SUCCESS;
}

struct dmx_r_playfct_pes_sec_ops g_dmx_play_fct_sec_ops = {

    .set_attrs      = dmx_r_play_fct_sec_set_attrs_impl,
    .attach_pid_ch  = dmx_r_play_fct_sec_attach_pid_ch_impl,
    .detach_pid_ch  = dmx_r_play_fct_sec_detach_pid_ch_impl,
    .add_flt        = dmx_r_play_fct_sec_add_flt_impl,
    .del_flt        = dmx_r_play_fct_sec_del_flt_impl,
    .del_all_flt    = dmx_r_play_fct_sec_del_all_flt_impl,
    .update_flt     = dmx_r_play_fct_sec_update_flt_impl,
    .get_flt_attrs  = dmx_r_play_fct_sec_get_flt_attrs_impl,
    .acquire_buf    = dmx_r_play_fct_sec_acquire_buf_impl,
    .release_buf    = dmx_r_play_fct_sec_release_buf_impl,

    .suspend        = dmx_r_play_fct_sec_suspend_impl,
    .resume         = dmx_r_play_fct_sec_resume_impl,
};

/*************************dmx_r_play_fct_pes_xxx_impl*************************************************/
static hi_s32 _dmx_r_play_fct_pes_attach_pid_ch_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_pid_ch *rpid_ch)
{
    hi_s32 ret;
    struct list_head *flt_node = HI_NULL;
    struct dmx_r_playfct_pes_sec *rplay_fct_sec = (struct dmx_r_playfct_pes_sec *)rplay_fct;
    flt_attach_attr attach_attr = {0};

    dmx_r_get_raw((struct dmx_r_base *)rpid_ch);
    ret = rpid_ch->ops->attach(rpid_ch, (struct dmx_r_base *)rplay_fct);
    if (ret == HI_SUCCESS) {
        /* assign the pidch object */
        rplay_fct->rpid_ch = rpid_ch;
        /* add the pid_ch obj reference */
        dmx_r_get_raw((struct dmx_r_base *)rpid_ch);
    }
    dmx_r_put((struct dmx_r_base *)rpid_ch);

    if (ret != HI_SUCCESS) {
        goto out;
    }

    /* add flt first, then set pid will reset the flt, need add flt again */
    osal_mutex_lock(&rplay_fct_sec->play_pes_sec.play_flt_list_lock);
    list_for_each(flt_node, &rplay_fct_sec->play_pes_sec.play_flt_head) {
        struct dmx_r_flt *rflt = list_entry(flt_node, struct dmx_r_flt, node1);
        dmx_r_get_raw((struct dmx_r_base *)rflt);
        attach_attr.flt_index = rflt->flt_index;
        attach_attr.pes_sec_id = rplay_fct_sec->play_pes_sec.pes_sec_chan_id;
        attach_attr.buf_id = rplay_fct->rbuf->base.id;
        attach_attr.crc_mode = rplay_fct_sec->play_pes_sec.crc_mode;
        rflt->rplay_fct = HI_NULL;
        ret = rflt->ops->attach(rflt, rplay_fct, &attach_attr);
        if (ret != HI_SUCCESS) {
            dmx_r_put((struct dmx_r_base *)rflt);
            goto out1;
        }
        dmx_r_put((struct dmx_r_base *)rflt);
    }

out1:
    osal_mutex_unlock(&rplay_fct_sec->play_pes_sec.play_flt_list_lock);

out:
    return ret;
}

static hi_s32 dmx_r_play_fct_pes_attach_pid_ch_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_pid_ch *rpid_ch)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    if (rplay_fct->is_opened != HI_FALSE) {
        HI_ERR_DEMUX("playfct pes not close, can not attach pid channel!\n");
        ret = HI_ERR_DMX_OPENING_CHAN;
        goto out;
    }

    if (unlikely(rplay_fct->rpid_ch != HI_NULL)) {
        HI_ERR_DEMUX("don't allow to attach pidch to one playfct repeatly!\n");
        ret = HI_ERR_DMX_BUSY;
        goto out;
    }

    ret = _dmx_r_play_fct_pes_attach_pid_ch_impl(rplay_fct, rpid_ch);
    if (ret == HI_SUCCESS) {
        rplay_fct->rband = rpid_ch->rband;
    }

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_pes_detach_pid_ch_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_r_playfct_pes_sec *rplay_fct_pes = (struct dmx_r_playfct_pes_sec *)rplay_fct;

    /* Attention: this function may run two time,
     * first time is the general detach,
     * second other time is post detach in _play_fct_destroy_pes.
    */
    if (unlikely(rplay_fct->rpid_ch == HI_NULL)) {
        /* pid channel can be detach without attach */
        HI_DBG_DEMUX("please attach pid_ch first!\n");
        ret = HI_SUCCESS;
        goto out;
    }

    dmx_r_get_raw((struct dmx_r_base *)rplay_fct->rpid_ch);
    ret = rplay_fct->rpid_ch->ops->detach(rplay_fct->rpid_ch, (struct dmx_r_base *)rplay_fct);
    dmx_r_put((struct dmx_r_base *)rplay_fct->rpid_ch);

    if (ret == HI_SUCCESS) {
        /* if this is the last pidch of playfct */
        DMX_NULL_POINTER_RETURN(rplay_fct->rpid_ch->rband);
        dmx_hal_band_clear_chan(rplay_fct->base.mgmt, rplay_fct->rpid_ch->rband->base.id);

        /* release the pid_ch obj */
        dmx_r_put((struct dmx_r_base *)rplay_fct->rpid_ch);
        /* set the pidch object as null */
        rplay_fct->rpid_ch = HI_NULL;
    }

    /* if there is no pidch in the pes channel, we will clear the pes channle */
    if (likely(rplay_fct->rpid_ch == HI_NULL)) {
        /* clear the pes section channel */
        dmx_hal_flt_clear_pes_sec_chan(rplay_fct->base.mgmt, rplay_fct_pes->play_pes_sec.pes_sec_chan_id);
    }

out:
    return ret;
}

static hi_s32 dmx_r_play_fct_pes_detach_pid_ch_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    if (rplay_fct->is_opened != HI_FALSE) {
        HI_ERR_DEMUX("playfct pes not close, can not detach pid channel!\n");
        ret = HI_ERR_DMX_OPENING_CHAN;
        goto out;
    }

    ret = _dmx_r_play_fct_pes_detach_pid_ch_impl(rplay_fct);

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_pes_add_flt_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 flt_index;
    flt_attach_attr attach_attr = {0};
    /* do something about hardware */
    struct dmx_r_playfct_pes_sec *rplay_fct_pes = (struct dmx_r_playfct_pes_sec *)rplay_fct;

    osal_mutex_lock(&rplay_fct_pes->play_pes_sec.play_flt_list_lock);
    flt_index = find_first_zero_bit(rplay_fct_pes->play_pes_sec.play_flt_bitmap, DMX_FLT_CNT_PER_PLAYFCT);
    if (!(flt_index < DMX_FLT_CNT_PER_PLAYFCT)) {
        HI_ERR_DEMUX("flt is full in pes chan[%d]!\n", rplay_fct_pes->play_pes_sec.pes_sec_chan_id);
        ret = HI_ERR_DMX_NO_RESOURCE;
        goto out;
    }

    attach_attr.flt_index = flt_index;
    attach_attr.pes_sec_id = rplay_fct_pes->play_pes_sec.pes_sec_chan_id;
    attach_attr.buf_id = rplay_fct->rbuf->base.id;
    attach_attr.crc_mode = rplay_fct_pes->play_pes_sec.crc_mode;

    dmx_r_get_raw((struct dmx_r_base *)rflt);
    ret = rflt->ops->attach(rflt, rplay_fct, &attach_attr);
    if (ret != HI_SUCCESS) {
        dmx_r_put((struct dmx_r_base *)rflt);
        goto out;
    }
    dmx_r_put((struct dmx_r_base *)rflt);

    set_bit(flt_index, rplay_fct_pes->play_pes_sec.play_flt_bitmap);

    list_add_tail(&rflt->node1, &rplay_fct_pes->play_pes_sec.play_flt_head);

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct_pes->play_pes_sec.play_flt_list_lock);
    return ret;
}

static hi_s32 dmx_r_play_fct_pes_add_flt_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_pes_add_flt_impl(rplay_fct, rflt);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    /* get the flt obj, it will put in del or del all flt */
    dmx_r_get_raw((struct dmx_r_base *)rflt);

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_pes_del_flt_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 flt_index;
    unsigned long mask;
    unsigned long *p = HI_NULL;

    /* do something about hardware */
    struct dmx_r_playfct_pes_sec *rplay_fct_pes = (struct dmx_r_playfct_pes_sec *)rplay_fct;

    osal_mutex_lock(&rplay_fct_pes->play_pes_sec.play_flt_list_lock);

    /* check if the paly facility and filter match */
    if (rflt->rplay_fct != rplay_fct) {
        osal_mutex_unlock(&rplay_fct_pes->play_pes_sec.play_flt_list_lock);
        HI_ERR_DEMUX("filter and play fct is not match!flt_handle = %#x, play_handle = %#x\n",
            rflt->flt_handle, rplay_fct->play_fct_handle);
        ret = HI_ERR_DMX_UNMATCH_FILTER;
        goto out;
    }

    flt_index = rflt->flt_index;
    mask = BIT_MASK(flt_index);
    p = ((unsigned long *)rplay_fct_pes->play_pes_sec.play_flt_bitmap) + BIT_WORD(flt_index);
    if (!(*p & mask)) {
        osal_mutex_unlock(&rplay_fct_pes->play_pes_sec.play_flt_list_lock);
        HI_ERR_DEMUX("flt_index(%d) is invalid.\n", flt_index);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    /* clear the flt_index bitmap */
    clear_bit(flt_index, rplay_fct_pes->play_pes_sec.play_flt_bitmap);

    /* delete the node from playfct list */
    list_del(&rflt->node1);
    osal_mutex_unlock(&rplay_fct_pes->play_pes_sec.play_flt_list_lock);

    dmx_r_get_raw((struct dmx_r_base *)rflt);
    ret = rflt->ops->destroy(rflt);
    if (ret != HI_SUCCESS) {
        dmx_r_put((struct dmx_r_base *)rflt);
        goto out;
    }

    dmx_r_put((struct dmx_r_base *)rflt);

out:

    return ret;
}

static hi_s32 dmx_r_play_fct_pes_del_flt_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_pes_del_flt_impl(rplay_fct, rflt);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    /* put the flt obj */
    dmx_r_put((struct dmx_r_base *)rflt);

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_pes_del_all_flt_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret;

    struct list_head *node = HI_NULL;
    struct list_head *tmp_node = HI_NULL;

    struct dmx_r_playfct_pes_sec *rplay_fct_pes = (struct dmx_r_playfct_pes_sec *)rplay_fct;

    osal_mutex_lock(&rplay_fct_pes->play_pes_sec.play_flt_list_lock);
    list_for_each_safe(node, tmp_node, &rplay_fct_pes->play_pes_sec.play_flt_head) {
        struct dmx_r_flt *rflt = list_entry(node, struct dmx_r_flt, node1);

        /* delete the entry from the list and destroy the flt handle */
        list_del(&rflt->node1);
        if (rflt->flt_handle != HI_INVALID_HANDLE) {
            ret = play_fct_destroy_flt(rflt->flt_handle);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("playfct destroy filter[0x%x] failed!\n", rflt->flt_handle);
            }
        }
        /* put the flt obj */
        dmx_r_put((struct dmx_r_base *)rflt);
    }
    /* clear all the bitmap */
    bitmap_zero(rplay_fct_pes->play_pes_sec.play_flt_bitmap, DMX_FLT_CNT_PER_PLAYFCT);

    osal_mutex_unlock(&rplay_fct_pes->play_pes_sec.play_flt_list_lock);

    return HI_SUCCESS;
}

static hi_s32 dmx_r_play_fct_pes_del_all_flt_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_pes_del_all_flt_impl(rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_pes_update_flt_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt,
    const dmx_filter_attrs *attrs)
{
    hi_s32 ret;

    dmx_r_get_raw((struct dmx_r_base *)rflt);
    ret = rflt->ops->set_attrs(rflt, attrs);
    if (ret == HI_SUCCESS) {
        goto out;
    }

out:
    dmx_r_put((struct dmx_r_base *)rflt);
    return ret;
}

static hi_s32 dmx_r_play_fct_pes_update_flt_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt,
    const dmx_filter_attrs *attrs)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_pes_update_flt_impl(rplay_fct, rflt, attrs);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_pes_get_flt_attrs_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt,
    dmx_filter_attrs *attrs)
{
    hi_s32 ret;

    dmx_r_get_raw((struct dmx_r_base *)rflt);
    ret = rflt->ops->get_attrs(rflt, attrs);
    if (ret == HI_SUCCESS) {
        goto out;
    }

out:
    dmx_r_put((struct dmx_r_base *)rflt);
    return ret;
}

static hi_s32 dmx_r_play_fct_pes_get_flt_attrs_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt,
    dmx_filter_attrs *attrs)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_pes_get_flt_attrs_impl(rplay_fct, rflt, attrs);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_pes_acquire_buf_impl(struct dmx_r_play_fct *rplay_fct, hi_u32 acq_num, hi_u32 time_out,
    hi_u32 *acqed_num, dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    struct dmx_r_buf *rbuf = rplay_fct->rbuf;
    DMX_NULL_POINTER_RETURN(rbuf);

    if (unlikely(rplay_fct->is_opened != HI_TRUE)) {
        HI_WARN_DEMUX("play[%u] not open yet!\n", rplay_fct->base.id);
        return HI_ERR_DMX_NOT_OPEN_CHAN;
    }

    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->acquire_buf(rbuf, acq_num, time_out, acqed_num, play_fct_buf);
    dmx_r_put((struct dmx_r_base *)rbuf);
    return ret;
}

static hi_s32 dmx_r_play_fct_pes_acquire_buf_impl(struct dmx_r_play_fct *rplay_fct, hi_u32 acq_num, hi_u32 time_out,
                                                  hi_u32 *acqed_num, dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_pes_acquire_buf_impl(rplay_fct, acq_num, time_out, acqed_num, play_fct_buf);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_pes_release_buf_impl(struct dmx_r_play_fct *rplay_fct, hi_u32 rel_num,
                                                   dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    struct dmx_mgmt     *mgmt = rplay_fct->base.mgmt;
    struct dmx_r_buf    *rbuf = rplay_fct->rbuf;
    struct dmx_r_pid_ch *rpid_ch = rplay_fct->rpid_ch;

    DMX_NULL_POINTER_RETURN(mgmt);
    DMX_NULL_POINTER_RETURN(rbuf);
    DMX_NULL_POINTER_RETURN(rpid_ch);

    if (unlikely(rplay_fct->is_opened != HI_TRUE)) {
        HI_WARN_DEMUX("play[%u] not open yet!\n", rplay_fct->base.id);
        return HI_ERR_DMX_NOT_OPEN_CHAN;
    }

    /* release the buffer */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->release_buf(rbuf, rel_num, play_fct_buf);
    if (ret == HI_SUCCESS) {
        /* check the pc buffer ap status */
        dmx_cls_pcbuf_ap_status(mgmt, rpid_ch);
    }
    dmx_r_put((struct dmx_r_base *)rbuf);

    return ret;
}

static hi_s32 dmx_r_play_fct_pes_release_buf_impl(struct dmx_r_play_fct *rplay_fct, hi_u32 rel_num,
                                                  dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_pes_release_buf_impl(rplay_fct, rel_num, play_fct_buf);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 dmx_r_play_fct_pes_suspend_impl(struct dmx_r_play_fct *rplay_fct)
{
    struct dmx_r_pid_ch *tmp_rpid_ch = HI_NULL;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);
    tmp_rpid_ch = rplay_fct->rpid_ch;

    dmx_dofunc_no_return(_dmx_r_play_fct_close_impl(rplay_fct));
    dmx_dofunc_no_return(_dmx_r_play_fct_pes_detach_pid_ch_impl(rplay_fct));

    rplay_fct->rpid_ch = tmp_rpid_ch;

    osal_mutex_unlock(&rplay_fct->lock);

    return HI_SUCCESS;
}

static hi_s32 dmx_r_play_fct_pes_resume_impl(struct dmx_r_play_fct *rplay_fct)
{
    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    dmx_dofunc_no_return(_dmx_r_play_fct_pes_attach_pid_ch_impl(rplay_fct, rplay_fct->rpid_ch));
    dmx_dofunc_no_return(_dmx_r_play_fct_open_impl(rplay_fct));

    osal_mutex_unlock(&rplay_fct->lock);

    return HI_SUCCESS;
}

struct dmx_r_playfct_pes_sec_ops g_dmx_play_fct_pes_ops = {

    .attach_pid_ch      = dmx_r_play_fct_pes_attach_pid_ch_impl,
    .detach_pid_ch      = dmx_r_play_fct_pes_detach_pid_ch_impl,
    .add_flt            = dmx_r_play_fct_pes_add_flt_impl,
    .del_flt            = dmx_r_play_fct_pes_del_flt_impl,
    .del_all_flt        = dmx_r_play_fct_pes_del_all_flt_impl,
    .update_flt         = dmx_r_play_fct_pes_update_flt_impl,
    .get_flt_attrs      = dmx_r_play_fct_pes_get_flt_attrs_impl,
    .acquire_buf        = dmx_r_play_fct_pes_acquire_buf_impl,
    .release_buf        = dmx_r_play_fct_pes_release_buf_impl,

    .suspend            = dmx_r_play_fct_pes_suspend_impl,
    .resume             = dmx_r_play_fct_pes_resume_impl,
};

/*************************dmx_r_play_fct_av_pes_xxx_impl*************************************************/
static hi_s32 _dmx_r_play_fct_av_pes_open_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret;

    struct dmx_r_playfct_avpes *rplay_avpes = (struct dmx_r_playfct_avpes *)rplay_fct;
    struct dmx_r_buf *rbuf = rplay_fct->rbuf;

    DMX_NULL_POINTER_RETURN(rbuf);

    if (rplay_fct->rband != HI_NULL) {
        /* get the buf obj and put, attach port to buffer to support anti-pressure */
        dmx_r_get_raw((struct dmx_r_base *)rbuf);
        ret = rbuf->ops->attach(rbuf, rplay_fct->rband->port);
        dmx_r_put((struct dmx_r_base *)rbuf);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("buf attach port failed!\n");
            goto out;
        }
    }

    /* config the secure buffer */
    if (rplay_fct->secure_mode == DMX_SECURE_TEE) {
        ret = dmx_tee_config_buf(rplay_avpes->play_avpes.av_pes_chan_id, rplay_fct->play_fct_type);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("configure secure buf failed!\n");
            goto out;
        }
    }

    /* open the buffer */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->open(rbuf);
    dmx_r_put((struct dmx_r_base *)rbuf);

    /* set play status */
    rplay_fct->is_opened = HI_TRUE;

out:
    return ret;
}

static hi_s32 dmx_r_play_fct_av_pes_open_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_av_pes_open_impl(rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_av_pes_get_status_impl(struct dmx_r_play_fct *rplay_fct, dmx_play_status *status)
{
    hi_s32 ret;
    struct dmx_buf_status buf_status;

    struct dmx_r_buf *rbuf = rplay_fct->rbuf;
    DMX_NULL_POINTER_RETURN(rbuf);

    /* get the buf obj */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->get_status(rbuf, &buf_status);
    if (ret == HI_SUCCESS) {
        status->is_opened = rplay_fct->is_opened;
        status->buf_size = buf_status.buf_size;
        status->buf_used_size = buf_status.buf_used_size;
        status->read_ptr = buf_status.hw_read;
        status->write_ptr = buf_status.hw_write;
        status->buf_ker_vir_addr = (hi_u64)buf_status.buf_ker_addr;
        status->buf_handle = buf_status.buf_handle;
        status->offset = buf_status.offset;
    }
    dmx_r_put((struct dmx_r_base *)rbuf);

    return ret;
}

static hi_s32 dmx_r_play_fct_av_pes_get_status_impl(struct dmx_r_play_fct *rplay_fct, dmx_play_status *status)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_av_pes_get_status_impl(rplay_fct, status);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_av_pes_attach_pid_ch_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_pid_ch *rpid_ch)
{
    hi_s32 ret;

    dmx_r_get_raw((struct dmx_r_base *)rpid_ch);
    ret = rpid_ch->ops->attach(rpid_ch, (struct dmx_r_base *)rplay_fct);
    if (ret == HI_SUCCESS) {
        /* assign the pidch object */
        rplay_fct->rpid_ch = rpid_ch;
        /* add the pid_ch obj reference */
        dmx_r_get_raw((struct dmx_r_base *)rpid_ch);
    }

    dmx_r_put((struct dmx_r_base *)rpid_ch);

    return ret;
}

static hi_s32 dmx_r_play_fct_av_pes_attach_pid_ch_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_pid_ch *rpid_ch)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    if (rplay_fct->is_opened != HI_FALSE) {
        HI_ERR_DEMUX("playfct avpes not close, can not attach pid channel!\n");
        ret = HI_ERR_DMX_OPENING_CHAN;
        goto out;
    }

    if (unlikely(rplay_fct->rpid_ch != HI_NULL)) {
        HI_ERR_DEMUX("don't allow to attach pidch to one playfct repeatly!\n");
        ret = HI_ERR_DMX_BUSY;
        goto out;
    }

    ret = _dmx_r_play_fct_av_pes_attach_pid_ch_impl(rplay_fct, rpid_ch);
    if (ret == HI_SUCCESS) {
        rplay_fct->rband = rpid_ch->rband;
    }
out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_av_pes_detach_pid_ch_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_r_playfct_avpes *rplay_fct_avpes = (struct dmx_r_playfct_avpes *)rplay_fct;

    /* Attention: this function may run two time,
     * first time is the general detach,
     * second other time is post detach in _play_fct_destroy_av_pes.
     */
    if (unlikely(rplay_fct->rpid_ch == HI_NULL)) {
        /* pid channel can be detach without attach */
        HI_DBG_DEMUX("please attach pid_ch first!\n");
        ret = HI_SUCCESS;
        goto out;
    }

    dmx_r_get_raw((struct dmx_r_base *)rplay_fct->rpid_ch);
    ret = rplay_fct->rpid_ch->ops->detach(rplay_fct->rpid_ch, (struct dmx_r_base *)rplay_fct);
    dmx_r_put((struct dmx_r_base *)rplay_fct->rpid_ch);

    if (ret == HI_SUCCESS) {
        /* if this is the last pidch of playfct */
        DMX_NULL_POINTER_RETURN(rplay_fct->rpid_ch->rband);
        dmx_hal_band_clear_chan(rplay_fct->base.mgmt, rplay_fct->rpid_ch->rband->base.id);

        /* release the pid_ch obj */
        dmx_r_put((struct dmx_r_base *)rplay_fct->rpid_ch);
        /* set the pidch object as null */
        rplay_fct->rpid_ch = HI_NULL;
    }

    /* if there is no pidch in the avpes channel, we will clear the avpes channle */
    if (likely(rplay_fct->rpid_ch == HI_NULL)) {
        /* clear the avpes channel */
        dmx_hal_flt_clear_av_pes_chan(rplay_fct->base.mgmt, rplay_fct_avpes->play_avpes.av_pes_chan_id);

        /* clear av/rec channel */
        dmx_hal_scd_clear_chan(rplay_fct->base.mgmt, rplay_fct_avpes->play_avpes.av_pes_chan_id, HI_FALSE);
    }

out:
    return ret;
}

static hi_s32 dmx_r_play_fct_av_pes_detach_pid_ch_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    if (rplay_fct->is_opened != HI_FALSE) {
        HI_ERR_DEMUX("playfct avpes not close, can not detach pid channel!\n");
        ret = HI_ERR_DMX_OPENING_CHAN;
        goto out;
    }

    ret = _dmx_r_play_fct_av_pes_detach_pid_ch_impl(rplay_fct);

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_av_pes_acquire_buf_impl(struct dmx_r_play_fct *rplay_fct, hi_u32 acq_num, hi_u32 time_out,
                                                      hi_u32 *acqed_num, dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    struct dmx_r_buf *rbuf = rplay_fct->rbuf;
    DMX_NULL_POINTER_RETURN(rbuf);

    if (unlikely(rplay_fct->is_opened != HI_TRUE)) {
        HI_WARN_DEMUX("play[%u] not open yet!\n", rplay_fct->base.id);
        return HI_ERR_DMX_NOT_OPEN_CHAN;
    }

    /* get the buf obj */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->acquire_buf(rbuf, acq_num, time_out, acqed_num, play_fct_buf);
#ifdef HI_DEMUX_PROC_SUPPORT
    if (ret == HI_SUCCESS) {
        dmx_play_fct_save_es(rplay_fct, play_fct_buf);
    }
#endif
    dmx_r_put((struct dmx_r_base *)rbuf);

    return ret;
}

static hi_s32 dmx_r_play_fct_av_pes_acquire_buf_impl(struct dmx_r_play_fct *rplay_fct, hi_u32 acq_num, hi_u32 time_out,
                                                     hi_u32 *acqed_num, dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    DMX_FATAL_CON_GOTO(rplay_fct->staled == HI_TRUE, HI_ERR_DMX_STALED, out);

    ret = _dmx_r_play_fct_av_pes_acquire_buf_impl(rplay_fct, acq_num, time_out, acqed_num, play_fct_buf);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_av_pes_release_buf_impl(struct dmx_r_play_fct *rplay_fct, hi_u32 rel_num,
                                                      dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    struct dmx_mgmt     *mgmt = rplay_fct->base.mgmt;
    struct dmx_r_buf    *rbuf = rplay_fct->rbuf;
    struct dmx_r_pid_ch *rpid_ch = rplay_fct->rpid_ch;

    DMX_NULL_POINTER_RETURN(mgmt);
    DMX_NULL_POINTER_RETURN(rbuf);
    DMX_NULL_POINTER_RETURN(rpid_ch);

    if (unlikely(rplay_fct->is_opened != HI_TRUE)) {
        HI_WARN_DEMUX("play[%u] not open yet!\n", rplay_fct->base.id);
        return HI_ERR_DMX_NOT_OPEN_CHAN;
    }

    /* release the buffer */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->release_buf(rbuf, rel_num, play_fct_buf);
    if (ret == HI_SUCCESS) {
        /* check the pc buffer ap status */
        dmx_cls_pcbuf_ap_status(mgmt, rpid_ch);
    }
    dmx_r_put((struct dmx_r_base *)rbuf);

    return ret;
}

static hi_s32 dmx_r_play_fct_av_pes_release_buf_impl(struct dmx_r_play_fct *rplay_fct, hi_u32 rel_num,
                                                     dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    DMX_FATAL_CON_GOTO(rplay_fct->staled == HI_TRUE, HI_ERR_DMX_STALED, out);

    ret = _dmx_r_play_fct_av_pes_release_buf_impl(rplay_fct, rel_num, play_fct_buf);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 __dmx_r_playfct_start_scd(struct dmx_mgmt *mgmt, struct dmx_r_buf *scd_rbuf, hi_u32 scd_id,
    dmx_vcodec_type vcodec_type)
{
    hi_s32 ret = HI_FAILURE;

    HI_ERR_DEMUX("#####vcodec_type=%d\n", vcodec_type);

    switch (vcodec_type) {
        case DMX_VCODEC_TYPE_MPEG2: {
            dmx_hal_set_scd_range_filter(mgmt, 0, 0x00, 0x00, 0x00);
            dmx_hal_set_scd_range_filter(mgmt, 1, 0xb3, 0xb3, 0x00);
            break;
        }
        case DMX_VCODEC_TYPE_AVS: {
            dmx_hal_set_scd_range_filter(mgmt, 1, 0xb3, 0xb3, 0x00);
            dmx_hal_set_scd_range_filter(mgmt, 2, 0xb6, 0xb6, 0x00); /* filter id 2 */
            dmx_hal_set_scd_range_filter(mgmt, 3, 0xb1, 0xb0, 0x00); /* filter id 3 */
            break;
        }
        case DMX_VCODEC_TYPE_MPEG4: {
            dmx_hal_set_scd_range_filter(mgmt, 4, 0xb0, 0xb0, 0x00); /* filter id 4 */
            dmx_hal_set_scd_range_filter(mgmt, 5, 0xb6, 0xb5, 0x00); /* filter id 5 */
            dmx_hal_set_scd_range_filter(mgmt, 6, 0x2f, 0x00, 0x00); /* filter id 6 */
            break;
        }
        case DMX_VCODEC_TYPE_H264: {
            dmx_hal_set_scd_range_filter(mgmt, 7, 0x01, 0x01, 0x00); /* filter id 7 */
            dmx_hal_set_scd_range_filter(mgmt, 8, 0x05, 0x05, 0x00); /* filter id 8 */
            dmx_hal_set_scd_range_filter(mgmt, 9, 0x08, 0x07, 0x00); /* filter id 9 */
            dmx_hal_set_scd_range_filter(mgmt, 10, 0x11, 0x11, 0x00); /* filter id 10 */
            dmx_hal_set_scd_range_filter(mgmt, 11, 0x15, 0x15, 0x00); /* filter id 11 */
            dmx_hal_set_scd_range_filter(mgmt, 12, 0x18, 0x17, 0x00); /* filter id 12 */
            dmx_hal_set_scd_range_filter(mgmt, 13, 0x21, 0x21, 0x00); /* filter id 13 */
            dmx_hal_set_scd_range_filter(mgmt, 14, 0x25, 0x25, 0x00); /* filter id 14 */
            dmx_hal_set_scd_range_filter(mgmt, 15, 0x28, 0x27, 0x00); /* filter id 15 */
            dmx_hal_set_scd_range_filter(mgmt, 16, 0x78, 0x31, 0x00); /* filter id 16 */
            break;
        }
        case DMX_VCODEC_TYPE_HEVC: {
            dmx_hal_set_scd_range_filter(mgmt, 17, 0x12, 0x0, 0x1);  /* [0x0 ~ 0x9] << 1, id 17 */
            dmx_hal_set_scd_range_filter(mgmt, 18, 0x2a, 0x20, 0x1); /* [0x10 ~ 0x15] << 1, id 18 */
            dmx_hal_set_scd_range_filter(mgmt, 19, 0x50, 0x40, 0x1); /* [0x20 ~ 0x28] << 1, id 19 */
            break;
        }
        default: {
            WARN(1, "Invalid vcodec type[%#x]!\n", vcodec_type);
            ret = HI_ERR_DMX_INVALID_PARA;
            goto out;
        }
    }

    /* enable play the index */
    dmx_hal_scd_set_play_tab(mgmt, scd_id, HI_FALSE, HI_TRUE);
    dmx_hal_scd_set_buf_id(mgmt, scd_id, scd_rbuf->base.id);
    /* enable the filter */
    dmx_hal_scd_set_flt_en(mgmt, scd_id, HI_TRUE);
    /* enable play the index */
    dmx_hal_scd_en(mgmt, scd_id, HI_TRUE);

    /* open the buffer */
    dmx_r_get_raw((struct dmx_r_base *)scd_rbuf);
    ret = scd_rbuf->ops->open(scd_rbuf);
    dmx_r_put((struct dmx_r_base *)scd_rbuf);

out:

    return ret;
}

static hi_s32 _dmx_r_play_fct_av_pes_start_idx_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_r_raw_pid_ch *rraw_pidch = HI_NULL;
    struct dmx_r_pid_ch *rpid_ch = HI_NULL;
    struct dmx_r_buf *scd_rbuf = HI_NULL;
    struct dmx_r_playfct_avpes *rplayfct_avpes = (struct dmx_r_playfct_avpes *)rplay_fct;

    rpid_ch = rplay_fct->rpid_ch;
    DMX_NULL_POINTER_RETURN(rpid_ch);

    scd_rbuf = rplayfct_avpes->play_avpes.scd_rbuf;
    DMX_NULL_POINTER_RETURN(scd_rbuf);

    rraw_pidch = _get_raw_pid_obj_for_play(rplay_fct->rpid_ch, rplay_fct);
    if (rraw_pidch != HI_NULL) {
        /* enable the pes scd */
        dmx_hal_pid_tab_ctl_en_set(rplay_fct->base.mgmt, rraw_pidch->raw_pid_chan_id, DMX_PID_CHN_PES_SCD_FLAG);
        dmx_hal_pid_set_scd_tab(rplay_fct->base.mgmt, rraw_pidch->raw_pid_chan_id,
                                rplayfct_avpes->play_avpes.av_pes_scd_id, 0);

        /* attach scd buffer to port */
        dmx_r_get_raw((struct dmx_r_base *)scd_rbuf);
        scd_rbuf->ops->attach(scd_rbuf, rpid_ch->rband->port);
        dmx_r_put((struct dmx_r_base *)scd_rbuf);

        ret = __dmx_r_playfct_start_scd(rplay_fct->base.mgmt, scd_rbuf, rplayfct_avpes->play_avpes.av_pes_scd_id,
                                        rplay_fct->vcodec_type);
    }

    return ret;
}

static hi_s32 dmx_r_play_fct_av_pes_start_idx_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    DMX_FATAL_CON_GOTO(rplay_fct->vcodec_type == DMX_VCODEC_TYPE_MAX, HI_ERR_DMX_INVALID_PARA, out);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_av_pes_start_idx_impl(rplay_fct);

out:
    osal_mutex_unlock(&rplay_fct->lock);
    return ret;
}

static hi_void dmx_scdidx_to_dmx_idx(const dmx_index_scd *scd_index_data, dmx_index_parse *index_parse)
{
    dmx_index_parse index_parse_tmp;

    index_parse_tmp.over_flow = !((scd_index_data->scd_type_ovflag_goback_num >> 28) & 0x1); /* 28 bits */
    if (index_parse_tmp.over_flow) {
        HI_INFO_DEMUX("index_data.over_flow == 1\n");
    }

    index_parse_tmp.index_type = (scd_index_data->scd_type_ovflag_goback_num >> 29) & 0xf; /* 29 bits */
    index_parse_tmp.start_code = (scd_index_data->sc_type_byte12after_sc_offset_in_ts >> 8) & 0xffU; /* 8 bits */

    if (index_parse_tmp.index_type == DMX_INDEX_SC_TYPE_PTS_FOR_TS ||
            index_parse_tmp.index_type == DMX_INDEX_SC_TYPE_PTS_FOR_PES) {
        index_parse_tmp.byte_after_sc[0]
            = (scd_index_data->sc_type_byte12after_sc_offset_in_ts >> 24) & 0xffU; /* 24 bits */
        index_parse_tmp.byte_after_sc[1]
            = (scd_index_data->sc_type_byte12after_sc_offset_in_ts >> 16) & 0xffU; /* 16 bits */
        index_parse_tmp.byte_after_sc[0x2]
            = (scd_index_data->ts_cnt_hi8_byte345after_sc >> 16) & 0xffU; /* 16 bits */
        index_parse_tmp.byte_after_sc[0x3]
            = (scd_index_data->ts_cnt_hi8_byte345after_sc >> 8) & 0xffU; /* 8 bits */
        index_parse_tmp.byte_after_sc[0x4]
            = (scd_index_data->ts_cnt_hi8_byte345after_sc) & 0xffU;
        index_parse_tmp.byte_after_sc[0x5]
            = (scd_index_data->sc_code_byte678after_sc >> 16) & 0xffU; /* 16 bits */
        index_parse_tmp.byte_after_sc[0x6]
            = (scd_index_data->sc_code_byte678after_sc >> 8) & 0xffU; /* 8 bits */
        index_parse_tmp.byte_after_sc[0x7]
            = (scd_index_data->sc_code_byte678after_sc) & 0xffU;
    } else {
        index_parse_tmp.byte_after_sc[0]
            = (scd_index_data->sc_type_byte12after_sc_offset_in_ts >> 16) & 0xffU; /* 16 bits */
        index_parse_tmp.byte_after_sc[1]
            = (scd_index_data->sc_type_byte12after_sc_offset_in_ts >> 24) & 0xffU; /* 24 bits */
        index_parse_tmp.byte_after_sc[0x2]
            = (scd_index_data->ts_cnt_hi8_byte345after_sc) & 0xffU;
        index_parse_tmp.byte_after_sc[0x3]
            = (scd_index_data->ts_cnt_hi8_byte345after_sc >> 8) & 0xffU; /* 8 bits */
        index_parse_tmp.byte_after_sc[0x4]
            = (scd_index_data->ts_cnt_hi8_byte345after_sc >> 16) & 0xffU; /* 16 bits */
        index_parse_tmp.byte_after_sc[0x5]
            = (scd_index_data->sc_code_byte678after_sc) & 0xffU;
        index_parse_tmp.byte_after_sc[0x6]
            = (scd_index_data->sc_code_byte678after_sc >> 8) & 0xffU; /* 8 bits */
        index_parse_tmp.byte_after_sc[0x7]
            = (scd_index_data->sc_code_byte678after_sc >> 16) & 0xffU; /* 16 bits */
    }

    memcpy(index_parse, &index_parse_tmp, sizeof(dmx_index_parse));
    return;
}

static hi_s32 _dmx_r_play_fct_av_pes_recv_idx_impl(struct dmx_r_play_fct *rplay_fct, hi_u32 req_num, hi_u32 time_out,
    hi_u32 *reqed_num, dmx_index_data *index)
{
    hi_s32 ret;
    hi_u32 real_num;
    dmx_index_scd *scd_data = HI_NULL;
    struct dmx_r_buf *scd_rbuf = HI_NULL;
    struct dmx_r_playfct_avpes *rplayfct_avpes = (struct dmx_r_playfct_avpes *)rplay_fct;
    hi_u32 i;
    dmx_index_parse index_parse;

    scd_rbuf = rplayfct_avpes->play_avpes.scd_rbuf;
    DMX_NULL_POINTER_RETURN(scd_rbuf);

    ret = scd_rbuf->ops->acquire_scd(scd_rbuf, DMX_MAX_SCD_CNT, time_out, &real_num, &scd_data);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("scd buf acquire failed!\n");
        goto out;
    }

    for (i = 0; i < real_num; i++) {
        dmx_scdidx_to_dmx_idx(&scd_data[i], &index_parse);
        *((hi_u8 *)(&index[i])) = index_parse.start_code;
        memcpy((hi_u8 *)(&index[i]) + 1, index_parse.byte_after_sc, 8); /* buf size 8 */
        HI_ERR_DEMUX("real_num[%u] index type[%02x] startcode[%02x] %02x %02x %02x %02x %02x %02x %02x %02x.\n",
            real_num, index_parse.index_type,
            index_parse.start_code, index_parse.byte_after_sc[0], index_parse.byte_after_sc[1],
            index_parse.byte_after_sc[0x2], index_parse.byte_after_sc[0x3], index_parse.byte_after_sc[0x4],
            index_parse.byte_after_sc[0x5], index_parse.byte_after_sc[0x6], index_parse.byte_after_sc[0x7]);
    }
    *reqed_num = real_num;

    ret = scd_rbuf->ops->release_scd(scd_rbuf, real_num, scd_data);
    /* check the pc buffer ap status */
    dmx_cls_pcbuf_ap_status(rplay_fct->base.mgmt, rplay_fct->rpid_ch);

out:
    return ret;
}

static hi_s32 dmx_r_play_fct_av_pes_recv_idx_impl(struct dmx_r_play_fct *rplay_fct, hi_u32 req_num, hi_u32 time_out,
    hi_u32 *reqed_num, dmx_index_data *index)
{
    hi_s32 ret;
    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_av_pes_recv_idx_impl(rplay_fct, req_num, time_out, reqed_num, index);

    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 __dmx_r_playfct_stop_scd(struct dmx_mgmt *mgmt, struct dmx_r_buf *scd_rbuf, hi_u32 scd_id)
{
    /* disable the index */
    dmx_hal_scd_en(mgmt, scd_id, HI_FALSE);
    dmx_hal_scd_set_play_tab(mgmt, scd_id, HI_FALSE, HI_FALSE);
    dmx_hal_scd_set_buf_id(mgmt, scd_id, 0);
    /* disable the filter */
    dmx_hal_scd_set_flt_en(mgmt, scd_id, HI_FALSE);

    /* close the buffer */
    dmx_r_get_raw((struct dmx_r_base *)scd_rbuf);
    scd_rbuf->ops->close(scd_rbuf);
    dmx_r_put((struct dmx_r_base *)scd_rbuf);

    return HI_SUCCESS;
}

static hi_s32 _dmx_r_play_fct_av_pes_stop_idx_impl(struct dmx_r_play_fct *rplay_fct)
{
    struct dmx_r_raw_pid_ch *rraw_pidch = HI_NULL;
    struct dmx_r_buf *scd_rbuf = HI_NULL;
    struct dmx_r_playfct_avpes *rplayfct_avpes = (struct dmx_r_playfct_avpes *)rplay_fct;

    scd_rbuf = rplayfct_avpes->play_avpes.scd_rbuf;
    DMX_NULL_POINTER_RETURN(scd_rbuf);

    rraw_pidch = _get_raw_pid_obj_for_play(rplay_fct->rpid_ch, rplay_fct);
    if (rraw_pidch != HI_NULL) {
        /* disable the pes scd */
        dmx_hal_pid_tab_ctl_dis_set(rplay_fct->base.mgmt, rraw_pidch->raw_pid_chan_id,
                                    DMX_PID_CHN_PES_SCD_FLAG);
        __dmx_r_playfct_stop_scd(rplay_fct->base.mgmt, scd_rbuf, rplayfct_avpes->play_avpes.av_pes_scd_id);
    }

    return HI_SUCCESS;
}

static hi_s32 dmx_r_play_fct_av_pes_stop_idx_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    DMX_FATAL_CON_GOTO(rplay_fct->vcodec_type == DMX_VCODEC_TYPE_MAX, HI_ERR_DMX_INVALID_PARA, out);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_av_pes_stop_idx_impl(rplay_fct);

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_av_pes_close_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret;

    struct dmx_r_playfct_avpes *rplay_avpes = (struct dmx_r_playfct_avpes *)rplay_fct;
    struct dmx_r_buf *rbuf = rplay_fct->rbuf;

    DMX_NULL_POINTER_RETURN(rbuf);

        /* deconfig the secure buffer */
    if (rplay_fct->secure_mode == DMX_SECURE_TEE) {
        dmx_tee_deconfig_buf(rplay_avpes->play_avpes.av_pes_chan_id, rplay_fct->play_fct_type);
    }

    /* close the buffer */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->close(rbuf);
    dmx_r_put((struct dmx_r_base *)rbuf);

    /* set the play status */
    rplay_fct->is_opened = HI_FALSE;

    return ret;
}

static hi_s32 dmx_r_play_fct_av_pes_close_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_av_pes_close_impl(rplay_fct);

    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 dmx_r_play_fct_av_pes_suspend_impl(struct dmx_r_play_fct *rplay_fct)
{
    struct dmx_r_pid_ch *tmp_rpid_ch = HI_NULL;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    /* store the rpid_ch temporarilly */
    tmp_rpid_ch = rplay_fct->rpid_ch;

    dmx_dofunc_no_return(_dmx_r_play_fct_close_impl(rplay_fct));
    dmx_dofunc_no_return(_dmx_r_play_fct_av_pes_detach_pid_ch_impl(rplay_fct));

    rplay_fct->rpid_ch = tmp_rpid_ch;

    osal_mutex_unlock(&rplay_fct->lock);

    return HI_SUCCESS;
}

static hi_s32 dmx_r_play_fct_av_pes_resume_impl(struct dmx_r_play_fct *rplay_fct)
{
    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    dmx_dofunc_no_return(_dmx_r_play_fct_av_pes_attach_pid_ch_impl(rplay_fct, rplay_fct->rpid_ch));
    dmx_dofunc_no_return(_dmx_r_play_fct_open_impl(rplay_fct));

    osal_mutex_unlock(&rplay_fct->lock);

    return HI_SUCCESS;
}

struct dmx_r_playfct_avpes_ops g_dmx_play_fct_av_pes_ops = {

    .open               = dmx_r_play_fct_av_pes_open_impl,
    .get_status         = dmx_r_play_fct_av_pes_get_status_impl,
    .attach_pid_ch      = dmx_r_play_fct_av_pes_attach_pid_ch_impl,
    .detach_pid_ch      = dmx_r_play_fct_av_pes_detach_pid_ch_impl,
    .acquire_buf        = dmx_r_play_fct_av_pes_acquire_buf_impl,
    .release_buf        = dmx_r_play_fct_av_pes_release_buf_impl,
    .start_idx          = dmx_r_play_fct_av_pes_start_idx_impl,
    .recv_idx           = dmx_r_play_fct_av_pes_recv_idx_impl,
    .stop_idx           = dmx_r_play_fct_av_pes_stop_idx_impl,
    .close              = dmx_r_play_fct_av_pes_close_impl,

    .suspend            = dmx_r_play_fct_av_pes_suspend_impl,
    .resume             = dmx_r_play_fct_av_pes_resume_impl,
};


/*************************dmx_r_flt_xxx_impl*************************************************/
#define DMX_R_FLT_GET(handle, rflt) ({ \
    hi_s32 ret = HI_FAILURE; \
    ret = dmx_r_get(handle, (struct dmx_r_base **)&rflt); \
    if (ret == HI_SUCCESS) { \
        if (&g_dmx_flt_ops != rflt->ops) { \
            dmx_r_put((struct dmx_r_base*)rflt);\
            HI_ERR_DEMUX("handle is in active, but not a valid flt handle(0x%x).\n", handle); \
            ret = HI_ERR_DMX_INVALID_PARA; \
        } \
    } \
    ret;\
})

hi_s32 dmx_mgmt_create_flt(const dmx_filter_attrs *attrs, struct dmx_r_flt **rflt)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    if (!attrs) {
        HI_ERR_DEMUX("invalid attrs(%#x).\n", attrs);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = mgmt->ops->create_flt(mgmt, attrs, rflt);
out:
    return ret;
}

hi_s32 dmx_mgmt_destroy_flt(struct dmx_r_base *obj)
{
    hi_s32 ret;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    ret = mgmt->ops->destroy_flt(mgmt, (struct dmx_r_flt *)obj);

    return ret;
}

static hi_s32 _dmx_r_flt_attach_impl(struct dmx_r_flt *rflt, struct dmx_r_play_fct *rplay_fct,
    const flt_attach_attr *attach_attr)
{
    rflt->rplay_fct  = rplay_fct;
    rflt->crc_mode   = attach_attr->crc_mode;
    rflt->pes_sec_id = attach_attr->pes_sec_id;
    rflt->flt_index  = attach_attr->flt_index;
    rflt->buf_id     = attach_attr->buf_id;

    dmx_hal_flt_attach_config(rflt->base.mgmt, rflt->flt_index, rflt->base.id,
                              rflt->pes_sec_id, rflt->buf_id, rflt->crc_mode);

    if (rflt->attr_status == DMX_FLT_ATTR_INIT) {
        dmx_hal_flt_attrs_config(rflt->base.mgmt, rflt->pes_sec_id, rflt->depth);
    }

    return HI_SUCCESS;
}

static hi_s32 dmx_r_flt_attach_impl(struct dmx_r_flt *rflt, struct dmx_r_play_fct *rplay_fct,
    const flt_attach_attr *attach_attr)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&rflt->lock);

    WARN_ON(rflt->staled == HI_TRUE);

    if (rflt->rplay_fct != HI_NULL) {
        HI_ERR_DEMUX("can't allow attach flt[%u] repeatedly!\n", rflt->base.id);
        ret = HI_ERR_DMX_ATTACHED_FILTER;
        goto out;
    }

    ret = _dmx_r_flt_attach_impl(rflt, rplay_fct, attach_attr);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rflt->lock);

    return ret;
}

static hi_s32 _dmx_r_flt_detach_impl(struct dmx_r_flt *rflt)
{
    if (rflt->pes_sec_id < DMX_PES_SEC_CHAN_CNT) {
        dmx_hal_flt_detach_config(rflt->base.mgmt, rflt->flt_index, rflt->base.id, rflt->pes_sec_id);
    }

    rflt->rplay_fct  = HI_NULL;
    rflt->crc_mode   = DMX_FLT_CRC_MODE_MAX;
    rflt->pes_sec_id = DMX_PES_SEC_CHAN_CNT;
    rflt->flt_index  = 0x0;

    return HI_SUCCESS;
}

static hi_s32 dmx_r_flt_detach_impl(struct dmx_r_flt *rflt)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&rflt->lock);

    WARN_ON(rflt->staled == HI_TRUE);

    if (rflt->rplay_fct == HI_NULL) {
        HI_ERR_DEMUX("please attach flt[%u] first!\n", rflt->base.id);
        ret = HI_ERR_DMX_NOATTACH_FILTER;
        goto out;
    }

    ret = _dmx_r_flt_detach_impl(rflt);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rflt->lock);

    return ret;
}

static hi_u32 __dmx_flt_get_valid_depth(const dmx_filter_attrs *attrs)
{
    hi_u32 depth = attrs->depth;

    while (depth) {
        if (attrs->mask[depth - 1] == 0xFF) { /* if mask is 0xFF, it means not valid filter byte */
            --depth;
        } else {
            break;
        }
    }

    return depth;
}

static hi_s32 _dmx_r_flt_set_attrs_impl(struct dmx_r_flt *rflt, const dmx_filter_attrs *attrs)
{
    hi_s32 ret;
    hi_s32 i;

    if (rflt->base.id == DMX_INVALID_FILTER_ID) {
        HI_ERR_DEMUX("invalid rflt handle[0x%x]\n", rflt->flt_handle);

        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    if (attrs->depth > DMX_FILTER_MAX_DEPTH) {
        HI_ERR_DEMUX("filter 0x%x set depth is 0x%x too long\n", rflt->base.id, attrs->depth);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    for (i = 0; i < attrs->depth; i++) {
        if (attrs->negate[i] > 1) {
            HI_ERR_DEMUX("filter %u set negate is invalid\n", rflt->base.id);

            ret = HI_ERR_DMX_INVALID_PARA;
            goto out;
        }
    }

    rflt->depth = __dmx_flt_get_valid_depth(attrs);
    for (i = 0; i < DMX_FILTER_MAX_DEPTH; i++) {
        if (i < attrs->depth) {
            hi_u32 negate = attrs->negate[i];

            if (attrs->mask[i] == 0xFF) { /* if mask is 0xFF, it means receive all bytes */
                negate = 0;
            }
            rflt->match[i]   = attrs->match[i];
            rflt->mask[i]    = attrs->mask[i];
            rflt->negate[i]  = negate;

            dmx_hal_flt_set_flt_byte(rflt->base.mgmt, rflt->base.id, i, negate, rflt->match[i], rflt->mask[i]);
        } else {
            rflt->match[i]   = 0;
            rflt->mask[i]    = 0xff;
            rflt->negate[i]  = 0;

            dmx_hal_flt_set_flt_byte(rflt->base.mgmt, rflt->base.id, i, 0, 0, 0xff);
        }
    }

    rflt->attr_status = DMX_FLT_ATTR_INIT;

    if (rflt->pes_sec_id < DMX_PES_SEC_CHAN_CNT) {
        /* configure the filter register */
        dmx_hal_flt_attrs_config(rflt->base.mgmt, rflt->pes_sec_id, rflt->depth);
        rflt->attr_status = DMX_FLT_ATTR_SET;
    }

    ret = HI_SUCCESS;
out:
    return ret;
}

hi_s32 dmx_r_flt_set_attrs_impl(struct dmx_r_flt *rflt, const dmx_filter_attrs *attrs)
{
    hi_s32 ret;

    osal_mutex_lock(&rflt->lock);

    WARN_ON(rflt->staled == HI_TRUE);

    ret = _dmx_r_flt_set_attrs_impl(rflt, attrs);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rflt->lock);

    return ret;
}

static hi_s32 _dmx_r_flt_get_attrs_impl(struct dmx_r_flt *rflt, dmx_filter_attrs *attrs)
{
    attrs->depth = rflt->depth;

    memcpy(attrs->mask, rflt->mask, sizeof(attrs->mask));
    memcpy(attrs->match, rflt->match, sizeof(attrs->match));
    memcpy(attrs->negate, rflt->negate, sizeof(attrs->negate));

    return HI_SUCCESS;
}

hi_s32 dmx_r_flt_get_attrs_impl(struct dmx_r_flt *rflt, dmx_filter_attrs *attrs)
{
    hi_s32 ret;

    osal_mutex_lock(&rflt->lock);

    WARN_ON(rflt->staled == HI_TRUE);

    ret = _dmx_r_flt_get_attrs_impl(rflt, attrs);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rflt->lock);

    return ret;
}

static hi_s32 _dmx_r_flt_clear_attrs_impl(struct dmx_r_flt *rflt)
{
    hi_s32 ret;

    /* configure the filter register */
    if (rflt->pes_sec_id < DMX_PES_SEC_CHAN_CNT) {
        dmx_hal_flt_attrs_config(rflt->base.mgmt, rflt->pes_sec_id, 0);
    }
    rflt->attr_status = DMX_FLT_ATTR_MAX;

    ret = HI_SUCCESS;

    return ret;
}

static hi_s32 dmx_r_flt_create_impl(struct dmx_r_flt *rflt, const dmx_filter_attrs *attrs)
{
    hi_s32 ret;

    osal_mutex_lock(&rflt->lock);

    WARN_ON(rflt->staled == HI_TRUE);

    ret = _dmx_r_flt_set_attrs_impl(rflt, attrs);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rflt->lock);

    return ret;
}

static hi_s32 dmx_r_flt_destroy_impl(struct dmx_r_flt *rflt)
{
    hi_s32 ret;

    osal_mutex_lock(&rflt->lock);

    WARN_ON(rflt->staled == HI_TRUE);

    ret = _dmx_r_flt_clear_attrs_impl(rflt);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = _dmx_r_flt_detach_impl(rflt);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rflt->lock);

    return ret;
}

static hi_s32 dmx_r_flt_suspend_impl(struct dmx_r_flt *rflt)
{
    hi_s32 ret;

    osal_mutex_lock(&rflt->lock);
    ret = _dmx_r_flt_clear_attrs_impl(rflt);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("clear flt attr failed in suspend!\n");
    }
    osal_mutex_unlock(&rflt->lock);

    return HI_SUCCESS;
}

static hi_s32 dmx_r_flt_resume_impl(struct dmx_r_flt *rflt)
{
    hi_s32 ret;

    dmx_filter_attrs attrs = {0};
    flt_attach_attr  attach_attrs = {0};

    osal_mutex_lock(&rflt->lock);
    memset(&attrs, 0x0, sizeof(dmx_filter_attrs));

    attrs.depth = rflt->depth;
    memcpy(attrs.mask, rflt->mask, sizeof(attrs.mask));
    memcpy(attrs.match, rflt->match, sizeof(attrs.match));
    memcpy(attrs.negate, rflt->negate, sizeof(attrs.negate));

    ret = _dmx_r_flt_set_attrs_impl(rflt, &attrs);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("set flt attr failed in resume!\n");
    }

    attach_attrs.buf_id = rflt->buf_id;
    attach_attrs.crc_mode = rflt->crc_mode;
    attach_attrs.flt_index = rflt->flt_index;
    attach_attrs.pes_sec_id = rflt->pes_sec_id;

    ret = _dmx_r_flt_attach_impl(rflt, rflt->rplay_fct, &attach_attrs);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("attah flt attr failed in resume!\n");
    }

    osal_mutex_unlock(&rflt->lock);

    return HI_SUCCESS;
}

struct dmx_r_flt_ops g_dmx_flt_ops = {
    .create          = dmx_r_flt_create_impl,
    .attach          = dmx_r_flt_attach_impl,
    .detach          = dmx_r_flt_detach_impl,
    .set_attrs       = dmx_r_flt_set_attrs_impl,
    .get_attrs       = dmx_r_flt_get_attrs_impl,
    .destroy         = dmx_r_flt_destroy_impl,

    .suspend         = dmx_r_flt_suspend_impl,
    .resume          = dmx_r_flt_resume_impl,
};

/*
 * obj is staled after Close, it should discard all possible call request after that.
 */
#define DMX_R_PLAY_FCT_GET(handle, rplay_fct) ({ \
    hi_s32 ret = HI_FAILURE; \
    ret = dmx_r_get(handle, (struct dmx_r_base **)&rplay_fct); \
    if (ret == HI_SUCCESS) { \
        if (&rplay_fct->inner_ops != rplay_fct->ops) { \
            dmx_r_put((struct dmx_r_base*)rplay_fct);\
            HI_ERR_DEMUX("handle is in active, but not a valid play_fct handle(0x%x).\n", handle); \
            ret = HI_ERR_DMX_INVALID_PARA; \
        } \
    } \
    ret;\
})

static hi_s32 play_fct_create_flt(const dmx_filter_attrs *attrs, hi_handle *flt_handle, struct dmx_session *session)
{
    hi_s32 ret;
    struct dmx_slot  *slot = HI_NULL;
    struct dmx_r_flt *rflt = HI_NULL;

    ret = dmx_mgmt_create_flt(attrs, &rflt);
    if (ret != HI_SUCCESS) {
        goto out0;
    }
    /* get the flt obj */
    dmx_r_get_raw((struct dmx_r_base *)rflt);

    /* set the filter attribute */
    ret = rflt->ops->create(rflt, attrs);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    /* associate a slot to the object  */
    ret = dmx_slot_create((struct dmx_r_base *)rflt, &slot);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    /* associate an slot to the session. */
    slot->release = play_fct_destroy_flt;
    ret = dmx_session_add_slot(session, slot);
    if (ret != HI_SUCCESS) {
        goto out3;
    }

    *flt_handle = slot->handle;

    rflt->flt_handle = slot->handle;

    /* release the flt obj */
    dmx_r_put((struct dmx_r_base *)rflt);

    return HI_SUCCESS;

out3:
    dmx_slot_destroy(slot);
out2:
    rflt->ops->destroy(rflt);
out1:
    dmx_r_put((struct dmx_r_base *)rflt);

    dmx_r_put((struct dmx_r_base *)rflt);
out0:
    return ret;
}

static hi_s32 play_fct_destroy_flt(hi_handle flt_handle)
{
    hi_s32 ret;
    struct dmx_slot *slot = HI_NULL;
    struct dmx_r_flt *rflt = HI_NULL;

    ret = DMX_R_FLT_GET(flt_handle, rflt);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = dmx_slot_find(flt_handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    dmx_session_del_slot(slot->session, slot);

    dmx_slot_destroy(slot);

    /* set the flthandle invalid after destroy it */
    rflt->flt_handle = HI_INVALID_HANDLE;

    ret = rflt->ops->destroy(rflt);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("destroy filter failed!\n");
        goto out1;
    }

    dmx_r_put((struct dmx_r_base *)rflt);
out1:
    dmx_r_put((struct dmx_r_base *)rflt);
out0:
    return ret;
}


/*************************dmx_mgmt_play_fct*************************************************/
static hi_s32 trim_play_fct_attrs(const dmx_play_attrs *in_attrs, dmx_play_attrs *out_attrs)
{
    hi_s32 ret = HI_ERR_DMX_INVALID_PARA;

    if (in_attrs->buf_size > PLAY_MAX_BUFFER_SIZE || in_attrs->buf_size < PLAY_MIN_BUFFER_SIZE) {
        HI_ERR_DEMUX("invalid play buffer size(0x%x)\n", in_attrs->buf_size);
        goto out;
    }

    if (in_attrs->type >= DMX_PLAY_TYPE_MAX) {
        HI_ERR_DEMUX("invalid play type(0x%x)\n", in_attrs->type);
        goto out;
    }

    if (in_attrs->crc_mode >= DMX_FLT_CRC_MODE_MAX) {
        HI_ERR_DEMUX("invalid play CRC mode(0x%x)\n", in_attrs->crc_mode);
        goto out;
    }

    if (in_attrs->data_mode >= DMX_DATA_MODE_MAX) {
        HI_ERR_DEMUX("invalid play data_mode(0x%x)\n", in_attrs->data_mode);
        goto out;
    }

    if (in_attrs->secure_mode >= DMX_SECURE_MAX) {
        HI_ERR_DEMUX("invalid play secure_mode(0x%x)\n", in_attrs->secure_mode);
        goto out;
    }

    memcpy(out_attrs, in_attrs, sizeof(dmx_play_attrs));

    return HI_SUCCESS;

out:
    return ret;
}

hi_s32 dmx_mgmt_create_play_fct(const dmx_play_attrs *attrs, struct dmx_r_play_fct **rplay_fct)
{
    hi_s32 ret = HI_FAILURE;
    dmx_play_attrs valid_attrs = {0};
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    if (attrs == HI_NULL) {
        HI_ERR_DEMUX("invalid parameter:attrs(%#x).\n", attrs);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = trim_play_fct_attrs(attrs, &valid_attrs);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid play attrs.\n");
        goto out;
    }

    ret = mgmt->ops->create_play_fct(mgmt, &valid_attrs, rplay_fct);

out:
    return ret;
}

hi_s32 dmx_mgmt_destroy_play_fct(struct dmx_r_base *obj)
{
    hi_s32 ret;

    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    ret = mgmt->ops->destroy_play_fct(mgmt, (struct dmx_r_play_fct *)obj);

    return ret;
}

/*************************dmx_mgmt_play_fct*************************************************/
hi_s32 dmx_play_fct_create(const dmx_play_attrs *attrs, hi_handle *handle, struct dmx_session *session)
{
    hi_s32 ret;
    struct dmx_slot  *slot = HI_NULL;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = dmx_mgmt_create_play_fct(attrs, &rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out0;
    }
    /* get the play_fct obj */
    dmx_r_get_raw((struct dmx_r_base *)rplay_fct);

    /* create the subplayfct */
    ret = rplay_fct->ops->create(rplay_fct, attrs);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    /* associate a slot to the object  */
    ret = dmx_slot_create((struct dmx_r_base *)rplay_fct, &slot);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    /* associate an slot to the session. */
    slot->release = dmx_play_fct_destroy;
    ret = dmx_session_add_slot(session, slot);
    if (ret != HI_SUCCESS) {
        goto out3;
    }

    *handle = slot->handle;

    rplay_fct->play_fct_handle = slot->handle;

    /* release the play_fct obj */
    dmx_r_put((struct dmx_r_base *)rplay_fct);

    return HI_SUCCESS;

out3:
    dmx_slot_destroy(slot);
out2:
    rplay_fct->ops->destroy(rplay_fct);
out1:
    dmx_r_put((struct dmx_r_base *)rplay_fct);

    dmx_r_put((struct dmx_r_base *)rplay_fct);
out0:
    return ret;
}

hi_s32 dmx_play_fct_open(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rplay_fct->ops->open(rplay_fct);

    dmx_r_put((struct dmx_r_base *)rplay_fct);
out:
    return ret;
}

hi_s32 dmx_play_fct_get_attrs(hi_handle handle, dmx_play_attrs *attrs)
{
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rplay_fct->ops->get_attrs(rplay_fct, attrs);

    dmx_r_put((struct dmx_r_base *)rplay_fct);
out:
    return ret;
}

hi_s32 dmx_play_fct_set_attrs(hi_handle handle, const dmx_play_attrs *attrs)
{
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rplay_fct->ops->set_attrs(rplay_fct, attrs);

    dmx_r_put((struct dmx_r_base *)rplay_fct);
out:
    return ret;
}

hi_s32 dmx_play_fct_get_status(hi_handle handle, dmx_play_status *status)
{
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rplay_fct->ops->get_status(rplay_fct, status);

    dmx_r_put((struct dmx_r_base *)rplay_fct);
out:
    return ret;
}

hi_s32 dmx_play_get_scrambled_flag(hi_handle handle, hi_dmx_scrambled_flag *scramble_flag)
{
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rplay_fct->ops->get_scrambled_flag(rplay_fct, scramble_flag);

    dmx_r_put((struct dmx_r_base *)rplay_fct);
out:
    return ret;
}

hi_s32 dmx_play_get_packet_num(hi_handle handle, dmx_chan_packet_num *chn_stat)
{
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    osal_mutex_lock(&rplay_fct->lock);

    if (rplay_fct->rpid_ch == HI_NULL) {
        chn_stat->cc_disc_cnt = 0;
        chn_stat->ts_cnt = 0;
        HI_WARN_DEMUX("play fct do not attach pid channel!\n");
        ret = HI_SUCCESS;
        goto unlock_exit;
    }

    osal_mutex_lock(&rplay_fct->rpid_ch->lock);

    chn_stat->cc_disc_cnt = rplay_fct->rpid_ch->cc_disc_err;
    chn_stat->ts_cnt = dmx_hal_pid_ch_pkt_cnt(rplay_fct->rpid_ch->base.mgmt, rplay_fct->rpid_ch->base.id);

    osal_mutex_unlock(&rplay_fct->rpid_ch->lock);
    ret = HI_SUCCESS;

unlock_exit:
    osal_mutex_unlock(&rplay_fct->lock);
    dmx_r_put((struct dmx_r_base *)rplay_fct);
out:
    return ret;
}

hi_s32 dmx_play_fct_get_pid_ch(hi_handle handle, hi_handle *pid_handle)
{
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rplay_fct->ops->get_pid_ch(rplay_fct, pid_handle);

    dmx_r_put((struct dmx_r_base *)rplay_fct);

out:
    return ret;
}

hi_s32 dmx_play_fct_attach_pid_ch(hi_handle handle, hi_handle pid_handle)
{
    hi_s32 ret;
    struct dmx_r_pid_ch *rpid_ch = HI_NULL;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_PID_CH_GET(pid_handle, rpid_ch);
    if (ret != HI_SUCCESS) {
        goto exit;
    }

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto put_pid_ch;
    }

    if (rplay_fct->is_opened != HI_FALSE) {
        HI_ERR_DEMUX("play facility is not closed,could not attach pid channel!\n");
        ret = HI_ERR_DMX_OPENING_CHAN;
        goto put_play;
    }

    ret = rplay_fct->ops->attach_pid_ch(rplay_fct, rpid_ch);

put_play:
    dmx_r_put((struct dmx_r_base *)rplay_fct);
put_pid_ch:
    dmx_r_put((struct dmx_r_base *)rpid_ch);
exit:
    return ret;
}

hi_s32 dmx_play_fct_detach_pid_ch(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto exit;
    }

    if (rplay_fct->is_opened != HI_FALSE) {
        HI_ERR_DEMUX("play facility is not closed,could not detach pid channel!\n");
        ret = HI_ERR_DMX_OPENING_CHAN;
        goto put_play;
    }

    ret = rplay_fct->ops->detach_pid_ch(rplay_fct);

put_play:
    dmx_r_put((struct dmx_r_base *)rplay_fct);
exit:
    return ret;
}

hi_s32 dmx_play_fct_create_filter(const dmx_filter_attrs *attrs, hi_handle *flt_handle,
                                     struct dmx_session *session)
{
    hi_s32 ret;
    hi_handle  tmp_flt_hanle = HI_INVALID_HANDLE;

    ret = play_fct_create_flt(attrs, &tmp_flt_hanle, session);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    *flt_handle = tmp_flt_hanle;

    return HI_SUCCESS;

out0:
    return ret;
}

hi_s32 dmx_play_fct_add_filter(hi_handle handle, hi_handle flt_handle)
{
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;
    struct dmx_r_flt *rflt = HI_NULL;

    ret = DMX_R_FLT_GET(flt_handle, rflt);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    if (rplay_fct == HI_NULL || rflt == HI_NULL || rflt->rplay_fct != HI_NULL) {
        ret = HI_ERR_DMX_ATTACHED_FILTER;
        HI_ERR_DEMUX("can't allow attach flt[%u] repeatedly!\n", rflt->base.id);
        goto out2;
    }

    ret = rplay_fct->ops->add_flt(rplay_fct, rflt);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

out2:
    dmx_r_put((struct dmx_r_base *)rplay_fct);
out1:
    dmx_r_put((struct dmx_r_base *)rflt);
out0:
    return ret;
}


hi_s32 dmx_play_fct_del_filter(hi_handle handle, hi_handle flt_handle)
{
    hi_s32 ret;
    struct dmx_r_flt *rflt = HI_NULL;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_FLT_GET(flt_handle, rflt);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    if (rplay_fct == HI_NULL || rflt == HI_NULL || rflt->rplay_fct == HI_NULL) {
        ret = HI_ERR_DMX_NOATTACH_FILTER;
        HI_ERR_DEMUX("please attach filter first!\n");
        goto out2;
    }

    ret = rplay_fct->ops->del_flt(rplay_fct, rflt);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("delete filter failed!\n");
        goto out2;
    }

out2:
    dmx_r_put((struct dmx_r_base *)rplay_fct);
out1:
    dmx_r_put((struct dmx_r_base *)rflt);
out0:
    return ret;
}

hi_s32 dmx_play_fct_del_all_filter(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = rplay_fct->ops->del_all_flt(rplay_fct);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("delete all filter failed!\n");
        goto out1;
    }

out1:
    dmx_r_put((struct dmx_r_base *)rplay_fct);
out0:
    return ret;
}

hi_s32 dmx_play_fct_update_filter(hi_handle flt_handle, const dmx_filter_attrs *attrs)
{
    hi_s32 ret;
    struct dmx_r_flt *rflt = HI_NULL;

    ret = DMX_R_FLT_GET(flt_handle, rflt);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    if (rflt == HI_NULL) {
        ret = HI_ERR_DMX_DETACH_FAILED;
        HI_ERR_DEMUX("please attach filter first!\n");
        goto out1;
    }

    ret = rflt->ops->set_attrs(rflt, attrs);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("set filter attrs failed!\n");
        goto out1;
    }

out1:
    dmx_r_put((struct dmx_r_base *)rflt);
out0:
    return ret;
}

hi_s32 dmx_play_fct_get_filter_attrs(hi_handle flt_handle, dmx_filter_attrs *attrs)
{
    hi_s32 ret;
    struct dmx_r_flt *rflt = HI_NULL;

    ret = DMX_R_FLT_GET(flt_handle, rflt);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    if (rflt == HI_NULL) {
        ret = HI_ERR_DMX_DETACH_FAILED;
        HI_ERR_DEMUX("please add filter first!\n");
        goto out1;
    }

    ret = rflt->ops->get_attrs(rflt, attrs);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("get filter failed!\n");
        goto out1;
    }

out1:
    dmx_r_put((struct dmx_r_base *)rflt);
out0:
    return ret;
}

hi_s32 dmx_play_fct_destroy_filter(hi_handle flt_handle)
{
    hi_s32 ret;
    struct dmx_r_flt *rflt = HI_NULL;

    ret = DMX_R_FLT_GET(flt_handle, rflt);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = play_fct_destroy_flt(flt_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("destroy filter failed!\n");
        goto out1;
    }

out1:
    dmx_r_put((struct dmx_r_base *)rflt);
out0:
    return ret;
}

hi_s32 dmx_play_fct_get_handle_by_filter(hi_handle flt_handle, hi_handle *play_handle)
{
    hi_s32 ret;
    struct dmx_r_flt *rflt = HI_NULL;

    ret = DMX_R_FLT_GET(flt_handle, rflt);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    if (rflt == HI_NULL || rflt->rplay_fct == HI_NULL) {
        ret = HI_ERR_DMX_NOATTACH_FILTER;
        HI_ERR_DEMUX("please add filter first!\n");
        goto out1;
    }

    ret = rflt->rplay_fct->ops->get_handle(rflt->rplay_fct, rflt, play_handle);

out1:
    dmx_r_put((struct dmx_r_base *)rflt);
out0:
    return ret;
}

hi_s32 dmx_play_fct_acquire_buf(hi_handle handle, hi_u32 acq_num, hi_u32 time_out, hi_u32 *acqed_num,
                                dmx_buffer *play_fct_buf)
{
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rplay_fct->ops->acquire_buf(rplay_fct, acq_num, time_out, acqed_num, play_fct_buf);

    dmx_r_put((struct dmx_r_base *)rplay_fct);
out:
    return ret;
}

hi_s32 dmx_play_fct_release_buf(hi_handle handle, hi_u32 rel_num, dmx_buffer *play_fct_buf)
{
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rplay_fct->ops->release_buf(rplay_fct, rel_num, play_fct_buf);

    dmx_r_put((struct dmx_r_base *)rplay_fct);
out:
    return ret;
}

hi_s32 dmx_play_fct_reset_buf(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rplay_fct->ops->reset_buf(rplay_fct);

    dmx_r_put((struct dmx_r_base *)rplay_fct);
out:
    return ret;
}

hi_s32 dmx_play_fct_start_idx(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rplay_fct->ops->start_idx(rplay_fct);

    dmx_r_put((struct dmx_r_base *)rplay_fct);
out:
    return ret;
}

hi_s32 dmx_play_fct_recv_idx(hi_handle handle, hi_u32 req_num, hi_u32 time_out, hi_u32 *reqed_num,
    dmx_index_data *index)
{
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("DMX_R_REC_FCT_GET failed!\n");
        goto out;
    }

    ret = rplay_fct->ops->recv_idx(rplay_fct, req_num, time_out, reqed_num, index);

    dmx_r_put((struct dmx_r_base *)rplay_fct);
out:
    return ret;
}

hi_s32 dmx_play_fct_stop_idx(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rplay_fct->ops->stop_idx(rplay_fct);

    dmx_r_put((struct dmx_r_base *)rplay_fct);
out:
    return ret;
}

hi_s32 dmx_play_fct_close(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rplay_fct->ops->close(rplay_fct);

    dmx_r_put((struct dmx_r_base *)rplay_fct);
out:
    return ret;
}

hi_s32 dmx_play_fct_destroy(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_slot *slot = HI_NULL;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = dmx_slot_find(handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    dmx_session_del_slot(slot->session, slot);

    dmx_slot_destroy(slot);

    ret = rplay_fct->ops->destroy(rplay_fct);

    dmx_r_put((struct dmx_r_base *)rplay_fct);

out1:
    dmx_r_put((struct dmx_r_base *)rplay_fct);
out0:
    return ret;
}

hi_s32 dmx_play_fct_pre_mmap(hi_handle handle, hi_s64 *buf_handle, hi_u32 *buf_size, hi_void **buf_usr_addr)
{
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rplay_fct->ops->pre_mmap(rplay_fct, buf_handle, buf_size, buf_usr_addr);

    dmx_r_put((struct dmx_r_base *)rplay_fct);
out:
    return ret;
}

hi_s32 dmx_play_fct_pst_mmap(hi_handle handle, hi_void *buf_usr_addr)
{
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rplay_fct->ops->pst_mmap(rplay_fct, buf_usr_addr);

    dmx_r_put((struct dmx_r_base *)rplay_fct);
out:
    return ret;
}

static hi_s32 _dmx_play_select_data_handle(hi_handle *watch_handle, hi_u32 watch_num,
    hi_handle *valid_array, hi_u32 *valid_num)
{
    hi_s32 ret;
    hi_s32 i;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;
    dmx_play_status status = {0};

    if (watch_num == 0) {
        HI_ERR_DEMUX("invalid watch num 0.\n");
        ret = HI_FAILURE;
        goto out;
    }

    *valid_num = 0;

    for (i = 0; i < watch_num; i++) {
        ret = DMX_R_PLAY_FCT_GET(watch_handle[i], rplay_fct);
        if (ret != HI_SUCCESS) {
            goto out;
        }

        ret = dmx_play_fct_get_status(watch_handle[i], &status);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("play get status failed! ret = %#x\n", ret);
            dmx_r_put((struct dmx_r_base *)rplay_fct);
            continue;
        }

        if (status.buf_used_size != 0) {
            valid_array[*valid_num] = watch_handle[i];
            (*valid_num)++;
        }

        dmx_r_put((struct dmx_r_base *)rplay_fct);
    }

    ret = *valid_num > 0 ? HI_SUCCESS : HI_ERR_DMX_NOAVAILABLE_DATA;
out:
    return ret;
}

static hi_s32 dmx_play_set_buf_wait_queue_flag(struct dmx_data_sel *data_sel, hi_handle *watch_handle,
    hi_u32 watch_num, hi_bool flag)
{
    hi_s32 i;
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    for (i = 0; i < watch_num; i++) {
        ret = DMX_R_PLAY_FCT_GET(watch_handle[i], rplay_fct);
        if (ret != HI_SUCCESS) {
            goto out;
        }
        osal_mutex_lock(&rplay_fct->lock);

        if (rplay_fct->rbuf == HI_NULL) {
            osal_mutex_unlock(&rplay_fct->lock);
            dmx_r_put((struct dmx_r_base *)rplay_fct);
            HI_DBG_DEMUX("no buf obj, handle(%u).\n", watch_handle[i]);
            continue;
        }

        osal_mutex_lock(&(rplay_fct->rbuf->interrupt_lock));
        data_sel->condition = 0;
        rplay_fct->rbuf->select_wait_queue_flag = flag;
        osal_mutex_unlock(&(rplay_fct->rbuf->interrupt_lock));

        osal_mutex_unlock(&rplay_fct->lock);

        dmx_r_put((struct dmx_r_base *)rplay_fct);
    }

    ret = HI_SUCCESS;

 out:
    return ret;
}

static hi_s32 _dmx_play_select_data_handle_timeout(hi_handle *watch_handle, hi_u32 watch_num, hi_u32 time_out)
{
    hi_s32 ret, ret_tmp;
    struct dmx_data_sel *wait_mgr = HI_NULL;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    if (watch_num == 0) {
        HI_ERR_DEMUX("invalid watch num 0.\n");
        ret = HI_FAILURE;
        goto out0;
    }

    if (mgmt == HI_NULL) {
        HI_ERR_DEMUX("mgmt is null.\n");
        ret = HI_FAILURE;
        goto out0;
    }

    wait_mgr = &(mgmt->select_wait_queue);

    osal_mutex_lock(&wait_mgr->data_sel_lock);

    ret = dmx_play_set_buf_wait_queue_flag(wait_mgr, watch_handle, watch_num, HI_TRUE);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    ret = wait_event_interruptible_timeout(wait_mgr->wait_queue, wait_mgr->condition, osal_msecs_to_jiffies(time_out));
    if (ret == 0) {
        ret = HI_ERR_DMX_TIMEOUT;
        goto out1;
    } else if (ret < 0) {
        ret = HI_ERR_DMX_INTR;
        goto out1;
    }

    /* if wait_event_interruptible_timeout return value larger than 0, we should return it to the caller. */
    ret_tmp = dmx_play_set_buf_wait_queue_flag(wait_mgr, watch_handle, watch_num, HI_FALSE);
    if (ret_tmp != HI_SUCCESS) {
        ret = ret_tmp;
        goto out1;
    }

out1:
    osal_mutex_unlock(&wait_mgr->data_sel_lock);
out0:
    return ret;
}

hi_s32 dmx_play_fct_get_data_handle(hi_handle *valid_array, hi_u32 *valid_num, hi_u32 watch_num, hi_u32 time_out_ms)
{
    hi_s32 ret;
    hi_handle watch_handle[watch_num];
    struct list_head *cur = HI_NULL;
    struct dmx_r_play_fct *cur_play = HI_NULL;
    hi_u32 cnt = 0;
    hi_u32 real_watch_cnt;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    if (memset_s(watch_handle, sizeof(watch_handle), 0, sizeof(watch_handle)) != HI_SUCCESS) {
        HI_ERR_DEMUX("memset failed.\n");
        ret = HI_FAILURE;
        goto out;
    }

    osal_mutex_lock(&mgmt->play_fct_list_lock);
    osal_mutex_lock(&mgmt->lock);

    list_for_each(cur, &mgmt->play_fct_head) {
        cur_play = list_entry(cur, struct dmx_r_play_fct, node);
        if ((cur_play->play_fct_type != DMX_PLAY_TYPE_AUD) &&
            (cur_play->play_fct_type != DMX_PLAY_TYPE_VID)) {
            watch_handle[cnt++] = cur_play->play_fct_handle;
        }
    }

    real_watch_cnt = cnt > watch_num ? watch_num : cnt;

    osal_mutex_unlock(&mgmt->lock);
    osal_mutex_unlock(&mgmt->play_fct_list_lock);

    /* if play fct is not created, the above list_for_each will return cnt == 0 */
    if (real_watch_cnt == 0) {
        ret = HI_ERR_DMX_NOAVAILABLE_DATA;
        goto out;
    }

    ret = _dmx_play_select_data_handle(watch_handle, real_watch_cnt, valid_array, valid_num);
    if (ret == HI_SUCCESS) {
        goto out;
    }

    while (time_out_ms != 0) {
        ret = _dmx_play_select_data_handle_timeout(watch_handle, real_watch_cnt, time_out_ms);
        if (ret < 0) {
            break;
        }
        time_out_ms = jiffies_to_msecs(ret);
        ret = _dmx_play_select_data_handle(watch_handle, real_watch_cnt, valid_array, valid_num);
        if (ret == HI_SUCCESS) {
            break;
        } else if (ret == HI_ERR_DMX_NOAVAILABLE_DATA) {
            continue;
        } else {
            break;
        }
    }
out:
    return ret;
}

hi_s32 dmx_play_fct_select_data_handle(hi_u32 *watch_array, hi_u32 watch_num, hi_handle *valid_array,
    hi_u32 *valid_num, hi_u32 time_out_ms)
{
    hi_s32 ret;
    hi_handle watch_handle[watch_num];
    struct dmx_r_play_fct *rplay_fct = HI_NULL;
    hi_s32 i;
    hi_u32 cnt = 0;

    if (memset_s(watch_handle, sizeof(watch_handle), 0, sizeof(watch_handle)) != HI_SUCCESS) {
        HI_ERR_DEMUX("memset failed.\n");
        ret = HI_FAILURE;
        goto out;
    }

    for (i = 0; i < watch_num; i++) {
        ret = DMX_R_PLAY_FCT_GET(watch_array[i], rplay_fct);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("get play fct failed, handle(%u).\n", watch_array[i]);
            goto out;
        }

        osal_mutex_lock(&rplay_fct->lock);
        if ((rplay_fct->play_fct_type != DMX_PLAY_TYPE_AUD)
            && (rplay_fct->play_fct_type != DMX_PLAY_TYPE_VID)) {
            watch_handle[cnt++] = watch_array[i];
        }
        osal_mutex_unlock(&rplay_fct->lock);

        dmx_r_put((struct dmx_r_base *)rplay_fct);
    }

    ret = _dmx_play_select_data_handle(watch_handle, cnt, valid_array, valid_num);
    if (ret == HI_SUCCESS) {
        goto out;
    }

    while (time_out_ms != 0) {
        ret = _dmx_play_select_data_handle_timeout(watch_handle, cnt, time_out_ms);
        if (ret < 0) {
            break;
        }
        time_out_ms = jiffies_to_msecs(ret);
        ret = _dmx_play_select_data_handle(watch_handle, cnt, valid_array, valid_num);
        if (ret == HI_SUCCESS) {
            break;
        } else if (ret == HI_ERR_DMX_NOAVAILABLE_DATA) {
            continue;
        } else {
            break;
        }
    }
out:
    return ret;
}

hi_s32 dmx_play_fct_set_eos_flag(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    osal_mutex_lock(&rplay_fct->lock);
    if ((rplay_fct->play_fct_type != DMX_PLAY_TYPE_AUD) &&
        (rplay_fct->play_fct_type != DMX_PLAY_TYPE_VID)) {
        HI_ERR_DEMUX("play fct not support opt, type(%u).\n", rplay_fct->play_fct_type);
        ret = HI_ERR_DMX_NOT_SUPPORT;
        goto out1;
    }

    ret = dmx_r_get_raw((struct dmx_r_base *)rplay_fct->rbuf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("get raw failed, ret(0x%08x).\n", ret);
        goto out1;
    }
    osal_mutex_lock(&(rplay_fct->rbuf->lock));
    rplay_fct->rbuf->eos_flag = HI_TRUE;
    osal_mutex_unlock(&(rplay_fct->rbuf->lock));
    dmx_r_put((struct dmx_r_base *)rplay_fct->rbuf);
    ret = HI_SUCCESS;
out1:
    osal_mutex_unlock(&rplay_fct->lock);
    dmx_r_put((struct dmx_r_base *)rplay_fct);
out0:
    return ret;
}

hi_s32 dmx_play_fct_set_callback(hi_handle play_handle, hi_handle user_handle, drv_dmx_callback cb)
{
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_PLAY_FCT_GET(play_handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    osal_mutex_lock(&rplay_fct->lock);

    rplay_fct->callback.user_handle = user_handle;
    rplay_fct->callback.cb = cb;

    ret = dmx_r_get_raw((struct dmx_r_base *)rplay_fct->rbuf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("get raw failed, ret(0x%08x).\n", ret);
        goto out1;
    }
    osal_mutex_lock(&(rplay_fct->rbuf->lock));
    rplay_fct->rbuf->callback.user_handle = user_handle;
    rplay_fct->rbuf->callback.cb = cb;
    osal_mutex_unlock(&(rplay_fct->rbuf->lock));
    dmx_r_put((struct dmx_r_base *)rplay_fct->rbuf);
    ret = HI_SUCCESS;

out1:
    osal_mutex_unlock(&rplay_fct->lock);
    dmx_r_put((struct dmx_r_base *)rplay_fct);
out0:
    return ret;
}

hi_s32 dmx_play_fct_get_bufhandle(hi_handle hadnle, hi_handle *buf_handle, dmx_play_type *type)
{
#ifdef DMX_TEE_SUPPORT
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    ret = DMX_R_PLAY_FCT_GET(hadnle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rplay_fct->ops->get_buf_handle(rplay_fct, buf_handle, type);

    dmx_r_put((struct dmx_r_base *)rplay_fct);
out:
    return ret;
#else
    return HI_ERR_DMX_NOT_SUPPORT;
#endif
}

hi_s32 dmx_play_cc_repeat_set(hi_handle handle, dmx_chan_cc_repeat_mode mode)
{
    hi_s32 ret;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;
    struct dmx_r_pid_ch *rpid_ch = HI_NULL;
    struct dmx_r_raw_pid_ch *rraw_pid_ch = HI_NULL;

    ret = DMX_R_PLAY_FCT_GET(handle, rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    osal_mutex_lock(&(rplay_fct->lock));

    if (rplay_fct->rpid_ch == HI_NULL) {
        osal_mutex_unlock(&(rplay_fct->lock));
        HI_ERR_DEMUX("paly fct did not attach pid channel yet!");
        ret = HI_ERR_DMX_UNMATCH_CHAN;
        goto err_exit;
    }

    rpid_ch = rplay_fct->rpid_ch;

    osal_mutex_lock(&(rpid_ch->lock));
    list_for_each_entry(rraw_pid_ch, &rpid_ch->raw_pid_ch_head, node1) {
        switch (mode) {
            case HI_DMX_CHAN_CC_REPEAT_MODE_RSV: {
                dmx_hal_pid_tab_set_ccdrop(rpid_ch->base.mgmt, rraw_pid_ch->raw_pid_chan_id, HI_FALSE, HI_FALSE);
                break;
            }
            case HI_DMX_CHAN_CC_REPEAT_MODE_DROP: {
                dmx_hal_pid_tab_set_ccdrop(rpid_ch->base.mgmt, rraw_pid_ch->raw_pid_chan_id, HI_FALSE, HI_TRUE);
                break;
            }
            default: {
                osal_mutex_unlock(&(rpid_ch->lock));
                osal_mutex_unlock(&(rplay_fct->lock));
                HI_ERR_DEMUX("Invalid cc repeat set mode.");
                ret = HI_ERR_DMX_INVALID_PARA;
                goto err_exit;
            }
        }
    }
    osal_mutex_unlock(&(rpid_ch->lock));

    osal_mutex_unlock(&(rplay_fct->lock));
    ret =  HI_SUCCESS;

err_exit:
    dmx_r_put((struct dmx_r_base *)rplay_fct);

out:
    return ret;
}


hi_void dmx_play_pusi_set(hi_bool no_pusi)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    dmx_hal_flt_set_sec_no_pusi(mgmt, no_pusi);
}
/*************************dmx_r_play_fct_xxx_impl*************************************************/
static inline hi_bool play_fct_staled(struct dmx_r_play_fct *rplay_fct)
{
    if (unlikely(rplay_fct->staled == HI_TRUE)) {
        HI_ERR_DEMUX("play_fct is staled.\n");
        return HI_TRUE;
    }

    return HI_FALSE;
}

static hi_s32 _play_fct_create_ts(struct dmx_mgmt *mgmt, struct dmx_r_play_fct *rplay_fct, struct dmx_r_buf *rbuf)
{
    hi_s32 ret;
    hi_u32 id;
    dmx_tee_mem_info tee_mem_info = {0};

    struct dmx_r_playfct_ts *new_rplay_fct_ts = (struct dmx_r_playfct_ts *)rplay_fct;

    new_rplay_fct_ts->ops = &g_dmx_play_fct_ts_ops;

    /* create an whole_ts channel */
    osal_mutex_lock(&mgmt->ts_chan_lock);
    id = find_first_zero_bit(mgmt->ts_bitmap, mgmt->ts_chan_cnt);
    if (!(id < mgmt->ts_chan_cnt)) {
        osal_mutex_unlock(&mgmt->ts_chan_lock);
        HI_ERR_DEMUX("there is no available whole ts channel now!\n");
        ret = HI_ERR_DMX_NO_RESOURCE;
        goto out;
    }
    set_bit(id, mgmt->ts_bitmap);
    osal_mutex_unlock(&mgmt->ts_chan_lock);

    /* set the whole_ts channel to struct object */
    new_rplay_fct_ts->play_ts.ts_chan_id = id;

    /* attention: here some of the base ops function will be replaced by sub ops function  */
    rplay_fct->ops->attach_pid_ch = new_rplay_fct_ts->ops->attach_pid_ch;
    rplay_fct->ops->detach_pid_ch = new_rplay_fct_ts->ops->detach_pid_ch;
    rplay_fct->ops->acquire_buf = new_rplay_fct_ts->ops->acquire_buf;
    rplay_fct->ops->release_buf = new_rplay_fct_ts->ops->release_buf;

    /* suspend & resume */
    rplay_fct->ops->suspend = new_rplay_fct_ts->ops->suspend;
    rplay_fct->ops->resume  = new_rplay_fct_ts->ops->resume;

    rplay_fct->rbuf  = rbuf;

    /* init the buffer */
    if (rbuf->secure_mode == DMX_SECURE_TEE) {
        ret = dmx_tee_create_play_chan(id, DMX_PLAY_TYPE_TS, rbuf->real_buf_size, &tee_mem_info);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("dmx_tee_create_play_chan failed, ret[0x%x]!\n", ret);
            goto out2;
        }
    }

    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->init(rbuf, DMX_BUF_TYPE_TS, &tee_mem_info);
    dmx_r_put((struct dmx_r_base *)rbuf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("create ts buf failed, ret[0x%x]!\n", ret);
        goto out1;
    }

    return ret;

out2:
    if (rbuf->secure_mode == DMX_SECURE_TEE) {
        dmx_tee_destroy_play_chan(id, DMX_PLAY_TYPE_TS, &tee_mem_info);
    }
out1:
    osal_mutex_lock(&mgmt->ts_chan_lock);
    clear_bit(id, mgmt->ts_bitmap);
    osal_mutex_unlock(&mgmt->ts_chan_lock);
out:
    return ret;
}

static hi_s32 _play_fct_create_pes(struct dmx_mgmt *mgmt, struct dmx_r_play_fct *rplay_fct, struct dmx_r_buf *rbuf,
    const dmx_play_attrs *attrs)
{
    hi_s32 ret;
    hi_u32 id;
    dmx_tee_mem_info tee_mem_info = {0};

    struct dmx_r_playfct_pes_sec *new_rplay_fct_pes = (struct dmx_r_playfct_pes_sec *)rplay_fct;

    new_rplay_fct_pes->ops = &g_dmx_play_fct_pes_ops;

    /* create an whole_ts channel */
    osal_mutex_lock(&mgmt->pes_sec_chan_lock);
    id = find_first_zero_bit(mgmt->pes_sec_bitmap, mgmt->pes_sec_chan_cnt);
    if (!(id < mgmt->pes_sec_chan_cnt)) {
        osal_mutex_unlock(&mgmt->pes_sec_chan_lock);
        HI_ERR_DEMUX("there is no available pes channel now!\n");
        ret = HI_ERR_DMX_NO_RESOURCE;
        goto out;
    }

    set_bit(id, mgmt->pes_sec_bitmap);
    osal_mutex_unlock(&mgmt->pes_sec_chan_lock);

    /* set the pes channel to struct object */
    new_rplay_fct_pes->play_pes_sec.pes_sec_chan_id = id;

    new_rplay_fct_pes->play_pes_sec.crc_mode = attrs->crc_mode;
    if (osal_mutex_init(&new_rplay_fct_pes->play_pes_sec.play_flt_list_lock) != 0) {
        HI_ERR_DEMUX("play_flt_list_lock mutex init err.\n");
        ret = HI_FAILURE;
        goto out1;
    }
    INIT_LIST_HEAD(&new_rplay_fct_pes->play_pes_sec.play_flt_head);
    new_rplay_fct_pes->play_pes_sec.play_flt_cnt = DMX_FLT_CNT_PER_PLAYFCT;
    bitmap_zero(new_rplay_fct_pes->play_pes_sec.play_flt_bitmap, DMX_FLT_CNT_PER_PLAYFCT);

    /* attention: here some of the base ops function will be replaced by sub ops function  */
    rplay_fct->ops->attach_pid_ch = new_rplay_fct_pes->ops->attach_pid_ch;
    rplay_fct->ops->detach_pid_ch = new_rplay_fct_pes->ops->detach_pid_ch;
    rplay_fct->ops->add_flt = new_rplay_fct_pes->ops->add_flt;
    rplay_fct->ops->del_flt = new_rplay_fct_pes->ops->del_flt;
    rplay_fct->ops->del_all_flt = new_rplay_fct_pes->ops->del_all_flt;
    rplay_fct->ops->update_flt = new_rplay_fct_pes->ops->update_flt;
    rplay_fct->ops->get_flt_attrs = new_rplay_fct_pes->ops->get_flt_attrs;
    rplay_fct->ops->acquire_buf = new_rplay_fct_pes->ops->acquire_buf;
    rplay_fct->ops->release_buf = new_rplay_fct_pes->ops->release_buf;

    /* suspend & resume */
    rplay_fct->ops->suspend = new_rplay_fct_pes->ops->suspend;
    rplay_fct->ops->resume = new_rplay_fct_pes->ops->resume;

    rplay_fct->rbuf = rbuf;

    /* init the buffer */
    if (rbuf->secure_mode == DMX_SECURE_TEE) {
        ret = dmx_tee_create_play_chan(id, DMX_PLAY_TYPE_PES, rbuf->real_buf_size, &tee_mem_info);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("dmx_tee_create_play_chan failed, ret[0x%x]!\n", ret);
            goto out1;
        }
    }

    /* get the buf obj */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->init(rbuf, DMX_BUF_TYPE_PES, &tee_mem_info);
    dmx_r_put((struct dmx_r_base *)rbuf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("create pes buf failed, ret[0x%x]!\n", ret);
        goto out2;
    }

    return ret;

out2:
    if (rbuf->secure_mode == DMX_SECURE_TEE) {
        dmx_tee_destroy_play_chan(id, DMX_PLAY_TYPE_PES, &tee_mem_info);
    }
out1:
    osal_mutex_lock(&mgmt->ts_chan_lock);
    clear_bit(id, mgmt->pes_sec_bitmap);
    osal_mutex_unlock(&mgmt->ts_chan_lock);
out:
    return ret;
}

static hi_s32 _play_fct_create_sec(struct dmx_mgmt *mgmt, struct dmx_r_play_fct *rplay_fct, struct dmx_r_buf *rbuf,
    const dmx_play_attrs *attrs)
{
    hi_s32 ret;
    hi_u32 id;
    dmx_tee_mem_info tee_mem_info = {0};

    struct dmx_r_playfct_pes_sec *new_rplay_fct_sec = (struct dmx_r_playfct_pes_sec *)rplay_fct;

    new_rplay_fct_sec->ops = &g_dmx_play_fct_sec_ops;

    /* create an whole_ts channel */
    osal_mutex_lock(&mgmt->pes_sec_chan_lock);
    id = find_first_zero_bit(mgmt->pes_sec_bitmap, mgmt->pes_sec_chan_cnt);
    if (!(id < mgmt->pes_sec_chan_cnt)) {
        osal_mutex_unlock(&mgmt->pes_sec_chan_lock);
        HI_ERR_DEMUX("there is no available sec channel now!\n");
        ret = HI_ERR_DMX_NO_RESOURCE;
        goto out;
    }
    set_bit(id, mgmt->pes_sec_bitmap);
    osal_mutex_unlock(&mgmt->pes_sec_chan_lock);

    /* set the pes channel to struct object */
    new_rplay_fct_sec->play_pes_sec.pes_sec_chan_id = id;
    new_rplay_fct_sec->play_pes_sec.crc_mode = attrs->crc_mode;

    if (osal_mutex_init(&new_rplay_fct_sec->play_pes_sec.play_flt_list_lock) != 0) {
        HI_ERR_DEMUX("play_flt_list_lock mutex init err.\n");
        ret = HI_FAILURE;
        goto out1;
    }
    INIT_LIST_HEAD(&new_rplay_fct_sec->play_pes_sec.play_flt_head);
    new_rplay_fct_sec->play_pes_sec.play_flt_cnt = DMX_FLT_CNT_PER_PLAYFCT;
    bitmap_zero(new_rplay_fct_sec->play_pes_sec.play_flt_bitmap, DMX_FLT_CNT_PER_PLAYFCT);

    /* attention: here some of the base ops function will be replaced by sub ops function  */
    rplay_fct->ops->set_attrs = new_rplay_fct_sec->ops->set_attrs;
    rplay_fct->ops->attach_pid_ch = new_rplay_fct_sec->ops->attach_pid_ch;
    rplay_fct->ops->detach_pid_ch = new_rplay_fct_sec->ops->detach_pid_ch;
    rplay_fct->ops->add_flt = new_rplay_fct_sec->ops->add_flt;
    rplay_fct->ops->del_flt = new_rplay_fct_sec->ops->del_flt;
    rplay_fct->ops->del_all_flt = new_rplay_fct_sec->ops->del_all_flt;
    rplay_fct->ops->update_flt = new_rplay_fct_sec->ops->update_flt;
    rplay_fct->ops->get_flt_attrs = new_rplay_fct_sec->ops->get_flt_attrs;
    rplay_fct->ops->acquire_buf = new_rplay_fct_sec->ops->acquire_buf;
    rplay_fct->ops->release_buf = new_rplay_fct_sec->ops->release_buf;

    /* suspend & resume */
    rplay_fct->ops->suspend = new_rplay_fct_sec->ops->suspend;
    rplay_fct->ops->resume = new_rplay_fct_sec->ops->resume;

    rplay_fct->rbuf = rbuf;

    /* init the buffer */
    if (rbuf->secure_mode == DMX_SECURE_TEE) {
        ret = dmx_tee_create_play_chan(id, DMX_PLAY_TYPE_SEC, rbuf->real_buf_size, &tee_mem_info);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("dmx_tee_create_play_chan failed, ret[0x%x]!\n", ret);
            goto out1;
        }
    }

    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->init(rbuf, DMX_BUF_TYPE_SEC, &tee_mem_info);
    dmx_r_put((struct dmx_r_base *)rbuf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("create sec buf failed, ret[0x%x]!\n", ret);
        goto out2;
    }

    return ret;

out2:
    dmx_tee_destroy_play_chan(id, DMX_PLAY_TYPE_SEC, &tee_mem_info);
out1:
    osal_mutex_lock(&mgmt->ts_chan_lock);
    clear_bit(id, mgmt->pes_sec_bitmap);
    osal_mutex_unlock(&mgmt->ts_chan_lock);
out:
    return ret;
}

static hi_s32 _play_fct_create_avscd(struct dmx_mgmt *mgmt, struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret;
    hi_u32 id;

    struct dmx_buf_attrs scd_buf_attrs = {0};
    dmx_tee_mem_info scd_mem_info = {0};
    struct dmx_r_buf *new_scd_rbuf = HI_NULL;
    struct dmx_r_playfct_avpes *rplayfct_avpes = (struct dmx_r_playfct_avpes *)rplay_fct;

    DMX_NULL_POINTER_RETURN(rplayfct_avpes);

    scd_buf_attrs.buf_size = DMX_VID_SCD_BUF_SIZE;
    scd_buf_attrs.secure_mode = DMX_SECURE_NONE;
    /* create the buffer one time */
    ret = dmx_mgmt_create_buf(&scd_buf_attrs, &new_scd_rbuf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("create recfct buf failed!\n");
        goto out0;
    }

    /* assign the buffer object */
    rplayfct_avpes->play_avpes.scd_rbuf = new_scd_rbuf;

    /* init the buffer */
    dmx_r_get_raw((struct dmx_r_base *)new_scd_rbuf);
    ret = new_scd_rbuf->ops->init(new_scd_rbuf, DMX_BUF_TYPE_SCD, &scd_mem_info);
    dmx_r_put((struct dmx_r_base *)new_scd_rbuf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("create scd buf object failed!!\n");
        goto out1;
    }

    /* create scd channel */
    osal_mutex_lock(&mgmt->scd_chan_lock);
    id = find_first_zero_bit(mgmt->scd_bitmap, mgmt->scd_chan_cnt);
    if (!(id < mgmt->scd_chan_cnt)) {
        osal_mutex_unlock(&mgmt->scd_chan_lock);
        HI_ERR_DEMUX("there is no available scd channel now!\n");
        ret = HI_ERR_DMX_NO_RESOURCE;
        goto out2;
    }
    set_bit(id, mgmt->scd_bitmap);
    osal_mutex_unlock(&mgmt->scd_chan_lock);

    rplayfct_avpes->play_avpes.av_pes_scd_id = id;

    return ret;

out2:
    new_scd_rbuf->ops->deinit(new_scd_rbuf);
out1:
    dmx_mgmt_destroy_buf((struct dmx_r_base *)new_scd_rbuf);
out0:
    return ret;
}

static hi_s32 _play_fct_create_avpes(struct dmx_mgmt *mgmt, struct dmx_r_play_fct *rplay_fct, struct dmx_r_buf *rbuf)
{
    hi_s32 ret;
    hi_u32 id;
    dmx_tee_mem_info tee_mem_info = {0};

    struct dmx_r_playfct_avpes *new_rplay_fct_av_pes = (struct dmx_r_playfct_avpes *)rplay_fct;

    new_rplay_fct_av_pes->ops = &g_dmx_play_fct_av_pes_ops;

    /* create an avpes channel */
    osal_mutex_lock(&mgmt->avr_chan_lock);
    if (mgmt->av_pes_used_cnt >= DMX_AVPES_CHAN_CNT) {
        osal_mutex_unlock(&mgmt->avr_chan_lock);
        HI_ERR_DEMUX("there is no available avpes channel now,used(%u)\n", mgmt->av_pes_used_cnt);
        ret = HI_ERR_DMX_NO_RESOURCE;
        goto out0;
    }
    id = find_first_zero_bit(mgmt->avr_bitmap, mgmt->av_pes_chan_cnt + mgmt->rec_chan_cnt);
    if (!(id < (mgmt->av_pes_chan_cnt + mgmt->rec_chan_cnt))) {
        osal_mutex_unlock(&mgmt->avr_chan_lock);
        HI_ERR_DEMUX("there is no available avr channel now!\n");
        ret = HI_ERR_DMX_NO_RESOURCE;
        goto out0;
    }
    set_bit(id, mgmt->avr_bitmap);
    mgmt->av_pes_used_cnt++;
    osal_mutex_unlock(&mgmt->avr_chan_lock);

    /* set the avpes channel to struct object */
    new_rplay_fct_av_pes->play_avpes.av_pes_chan_id = id;

    /* attention: here some of the base ops function will be replaced by sub ops function  */
    rplay_fct->ops->open = new_rplay_fct_av_pes->ops->open;
    rplay_fct->ops->get_status = new_rplay_fct_av_pes->ops->get_status;
    rplay_fct->ops->attach_pid_ch = new_rplay_fct_av_pes->ops->attach_pid_ch;
    rplay_fct->ops->detach_pid_ch = new_rplay_fct_av_pes->ops->detach_pid_ch;
    rplay_fct->ops->acquire_buf = new_rplay_fct_av_pes->ops->acquire_buf;
    rplay_fct->ops->release_buf = new_rplay_fct_av_pes->ops->release_buf;
    rplay_fct->ops->start_idx = new_rplay_fct_av_pes->ops->start_idx;
    rplay_fct->ops->recv_idx = new_rplay_fct_av_pes->ops->recv_idx;
    rplay_fct->ops->stop_idx = new_rplay_fct_av_pes->ops->stop_idx;
    rplay_fct->ops->close = new_rplay_fct_av_pes->ops->close;

    /* suspend & resume */
    rplay_fct->ops->suspend = new_rplay_fct_av_pes->ops->suspend;
    rplay_fct->ops->resume = new_rplay_fct_av_pes->ops->resume;

    rplay_fct->rbuf = rbuf;

    /* init the buffer */
    if (rbuf->secure_mode == DMX_SECURE_TEE) {
        ret = dmx_tee_create_play_chan(id, rplay_fct->play_fct_type, rbuf->real_buf_size, &tee_mem_info);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("dmx_tee_create_play_chan failed, ret[0x%x]!\n", ret);
            goto out1;
        }
    }

    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->init(rbuf, DMX_BUF_TYPE_AVPES, &tee_mem_info);
    dmx_r_put((struct dmx_r_base *)rbuf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("create avpes buf failed, ret[0x%x]!\n", ret);
        goto out2;
    }

    return ret;

out2:
    if (rbuf->secure_mode == DMX_SECURE_TEE) {
        dmx_tee_destroy_play_chan(id, rplay_fct->play_fct_type, &tee_mem_info);
    }
out1:
    osal_mutex_lock(&mgmt->ts_chan_lock);
    clear_bit(id, mgmt->avr_bitmap);
    mgmt->av_pes_used_cnt--;
    osal_mutex_unlock(&mgmt->ts_chan_lock);
out0:
    return ret;
}

static hi_s32 _play_fct_destroy_avpes(struct dmx_mgmt *mgmt, struct dmx_r_play_fct *rplay_fct);
static hi_s32 _dmx_r_play_fct_create_impl(struct dmx_r_play_fct *rplay_fct, const dmx_play_attrs *attrs)
{
    hi_u32 ret;

    struct dmx_r_buf  *new_rbuf = HI_NULL;
    struct dmx_buf_attrs     buf_attrs;
    struct dmx_mgmt        *mgmt = rplay_fct->base.mgmt;
    DMX_NULL_POINTER_RETURN(mgmt);

    buf_attrs.buf_size      = attrs->buf_size;
    buf_attrs.secure_mode   = attrs->secure_mode;
    buf_attrs.pes_ext_flag  = HI_FALSE;

    if (rplay_fct->play_fct_type == DMX_PLAY_TYPE_PES) {
        buf_attrs.pes_ext_flag = HI_TRUE;
    }

    /* create the buffer one time */
    ret = dmx_mgmt_create_buf(&buf_attrs, &new_rbuf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("create playfct buf failed!\n");
        goto out0;
    }

    switch (rplay_fct->play_fct_type) {
        case DMX_PLAY_TYPE_TS: {
            ret = _play_fct_create_ts(mgmt, rplay_fct, new_rbuf);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("_play_fct_creat_ts failed!\n");
                goto out1;
            }
            break;
        }
        case DMX_PLAY_TYPE_PES: {
            ret = _play_fct_create_pes(mgmt, rplay_fct, new_rbuf, attrs);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("_play_fct_create_pes failed!\n");
                goto out1;
            }
            break;
        }
        case DMX_PLAY_TYPE_SEC: {
            ret = _play_fct_create_sec(mgmt, rplay_fct, new_rbuf, attrs);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("_play_fct_create_sec failed!\n");
                goto out1;
            }
            break;
        }
        case DMX_PLAY_TYPE_VID:
        case DMX_PLAY_TYPE_AUD: {
            ret = _play_fct_create_avpes(mgmt, rplay_fct, new_rbuf);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("_play_fct_create_av_pes failed!\n");
                goto out1;
            }

            if (rplay_fct->vcodec_type != DMX_VCODEC_TYPE_MAX) {
                ret = _play_fct_create_avscd(mgmt, rplay_fct);
                if (ret != HI_SUCCESS) {
                    _play_fct_destroy_avpes(mgmt, rplay_fct);
                    HI_ERR_DEMUX("_play_fct_create_av_pes failed!\n");
                    goto out1;
                }
            }
            break;
        }

        default: {
            HI_ERR_DEMUX("invalid play type[0x%x]!\n", rplay_fct->play_fct_type);
            ret = HI_ERR_DMX_INVALID_PARA;
            goto out1;
        }
    }

    return ret;

out1:
    dmx_mgmt_destroy_buf((struct dmx_r_base *)new_rbuf);
    rplay_fct->rbuf = HI_NULL;

out0:
    return ret;
}

static hi_s32 dmx_r_play_fct_create_impl(struct dmx_r_play_fct *rplay_fct, const dmx_play_attrs *attrs)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_create_impl(rplay_fct, attrs);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_open_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret;

    struct dmx_r_buf *rbuf = rplay_fct->rbuf;

    DMX_NULL_POINTER_RETURN(rbuf);

    if (rplay_fct->rband != HI_NULL) {
        /* get the buf obj and put, attach port to buffer to support anti-pressure */
        dmx_r_get_raw((struct dmx_r_base *)rbuf);
        ret = rbuf->ops->attach(rbuf, rplay_fct->rband->port);
        dmx_r_put((struct dmx_r_base *)rbuf);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("buf attach port failed!\n");
            goto out;
        }
    }

    /* open the buffer */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->open(rbuf);
    dmx_r_put((struct dmx_r_base *)rbuf);

    /* set play status */
    rplay_fct->is_opened = HI_TRUE;

out:
    return ret;
}

static hi_s32 dmx_r_play_fct_open_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_open_impl(rplay_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_get_attrs_impl(struct dmx_r_play_fct *rplay_fct, dmx_play_attrs *attr)
{
    hi_s32 ret;
    struct dmx_buf_attrs buf_attrs;
    struct dmx_r_buf *rbuf = rplay_fct->rbuf;

    /* get buffer attrs */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->get_attrs(rbuf, &buf_attrs);
    if (ret == HI_SUCCESS) {
        attr->buf_handle  = buf_attrs.buf_handle;
        attr->buf_size    = buf_attrs.buf_size;
        attr->secure_mode = buf_attrs.secure_mode;
        attr->type        = rplay_fct->play_fct_type;
        attr->crc_mode    = rplay_fct->crc_mode;
        attr->live_play   = rplay_fct->live_play;
        attr->data_mode   = rplay_fct->data_mode;
    }
    dmx_r_put((struct dmx_r_base *)rbuf);

    return ret;
}

static hi_s32 dmx_r_play_fct_get_attrs_impl(struct dmx_r_play_fct *rplay_fct, dmx_play_attrs *attr)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_get_attrs_impl(rplay_fct, attr);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 dmx_r_play_fct_set_attrs_impl(struct dmx_r_play_fct *rplay_fct, const dmx_play_attrs *attr)
{
    hi_s32 ret;
    struct dmx_buf_attrs buf_attrs;

    struct dmx_r_buf *rbuf = rplay_fct->rbuf;

    DMX_NULL_POINTER_RETURN(rbuf);

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    /* get buffer attrs */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->get_attrs(rbuf, &buf_attrs);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("get rbuf attrs failed! ret = %#x\n", ret);
        goto exit;
    }

    if ((attr->buf_size != buf_attrs.buf_size) ||
        (attr->secure_mode != buf_attrs.secure_mode) ||
        (attr->type != rplay_fct->play_fct_type) ||
        (attr->data_mode != rplay_fct->data_mode)) {
        HI_ERR_DEMUX("not support change attr of buf size/chan type/chan output/secure mode!");
        ret = HI_ERR_DMX_NOT_SUPPORT;
        goto exit;
    }

    if (attr->crc_mode != rplay_fct->crc_mode) {
        HI_DBG_DEMUX("not support change crc mode.\n");
        ret = HI_SUCCESS;
        goto exit;
    }

    ret = HI_SUCCESS;

exit:
    dmx_r_put((struct dmx_r_base *)rbuf);

    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_get_status_impl(struct dmx_r_play_fct *rplay_fct, dmx_play_status *status)
{
    hi_s32 ret;
    struct dmx_buf_status buf_status = {0};

    struct dmx_r_buf *rbuf = rplay_fct->rbuf;
    DMX_NULL_POINTER_RETURN(rbuf);

    /* get the buf attr */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->get_status(rbuf, &buf_status);
    if (ret == HI_SUCCESS) {
        status->is_opened = rplay_fct->is_opened;
        status->buf_size = buf_status.buf_size;
        status->buf_used_size = buf_status.buf_used_size;
        status->read_ptr = buf_status.hw_read;
        status->write_ptr = buf_status.hw_write;
    }
    dmx_r_put((struct dmx_r_base *)rbuf);

    return ret;
}

static hi_s32 dmx_r_play_fct_get_status_impl(struct dmx_r_play_fct *rplay_fct, dmx_play_status *status)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    ret = _dmx_r_play_fct_get_status_impl(rplay_fct, status);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 dmx_r_play_fct_get_scrambled_flag_impl(struct dmx_r_play_fct *rplay_fct,
    hi_dmx_scrambled_flag *scramble_flag)
{
    hi_s32 ret;
    struct dmx_mgmt *mgmt = rplay_fct->base.mgmt;

    if (rplay_fct->is_opened == HI_FALSE) {
        HI_ERR_DEMUX("play fct[%#x] is not opened!\n", rplay_fct->play_fct_handle);
        ret = HI_ERR_DMX_NOT_OPEN_CHAN;
        goto out;
    }

    if (rplay_fct->rpid_ch == HI_NULL) {
        *scramble_flag = HI_DMX_SCRAMBLED_FLAG_NO;
        HI_DBG_DEMUX("please attach pid_ch first!\n");
        ret = HI_SUCCESS;
        goto out;
    }

    if (rplay_fct->rpid_ch->rdsc_fct == HI_NULL) {
        *scramble_flag = HI_DMX_SCRAMBLED_FLAG_NO;
        HI_DBG_DEMUX("please attach descrambler first!\n");
        ret = HI_SUCCESS;
        goto out;
    }

    dmx_hal_pid_get_dsc_type(mgmt, rplay_fct->rpid_ch->rdsc_fct->base.id, scramble_flag);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 dmx_r_play_fct_get_pid_ch_impl(struct dmx_r_play_fct *rplay_fct, hi_handle *pid_handle)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    if (rplay_fct->rpid_ch == HI_NULL) {
        HI_WARN_DEMUX("play facility not attach pid channel yet!\n");
        ret = HI_ERR_DMX_EMPTY;
        goto out;
    }

    *pid_handle = rplay_fct->rpid_ch->pid_ch_handle;
    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);
    return ret;
}

static hi_s32 dmx_r_play_fct_attach_pid_ch_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_pid_ch *rpid_ch)
{
    HI_ERR_DEMUX("some error may happened, not support!\n");

    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 dmx_r_play_fct_detach_pid_ch_impl(struct dmx_r_play_fct *rplay_fct)
{
    HI_ERR_DEMUX("some error may happened, not support!\n");

    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 dmx_r_play_fct_add_flt_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt)
{
    HI_ERR_DEMUX("some error may happened, not support!\n");

    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 dmx_r_play_fct_del_flt_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt)
{
    HI_ERR_DEMUX("some error may happened, not support!\n");

    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 dmx_r_play_fct_del_all_flt_impl(struct dmx_r_play_fct *rplay_fct)
{
    HI_ERR_DEMUX("some error may happened, not support!\n");

    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 dmx_r_play_fct_update_flt_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt,
    const dmx_filter_attrs *attrs)
{
    HI_ERR_DEMUX("some error may happened, not support!\n");

    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 dmx_r_play_fct_get_flt_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt,
    dmx_filter_attrs *attrs)
{
    HI_ERR_DEMUX("some error may happened, not support!\n");

    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 dmx_r_play_fct_get_handle_impl(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt,
                                             hi_handle *play_handle)
{
    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    *play_handle = rplay_fct->play_fct_handle;

    osal_mutex_unlock(&rplay_fct->lock);

    return HI_SUCCESS;
}

static hi_s32 dmx_r_play_fct_acquire_buf_impl(struct dmx_r_play_fct *rplay_fct, hi_u32 acq_num, hi_u32 time_out,
    hi_u32 *acqed_num, dmx_buffer *play_fct_buf)
{
    HI_ERR_DEMUX("some error may happened, not support!\n");

    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 dmx_r_play_fct_release_buf_impl(struct dmx_r_play_fct *rplay_fct, hi_u32 rel_num,
                                              dmx_buffer *play_fct_buf)
{
    HI_ERR_DEMUX("some error may happened, not support!\n");

    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 dmx_r_play_fct_reset_buf_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret;

    struct dmx_r_buf *rbuf = rplay_fct->rbuf;
    DMX_NULL_POINTER_RETURN(rbuf);

    if (unlikely(rplay_fct->is_opened != HI_TRUE)) {
        HI_WARN_DEMUX("play[%u] not open yet!\n", rplay_fct->base.id);
        return HI_ERR_DMX_NOT_OPEN_CHAN;
    }

    /* reest the buf */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->reset(rbuf, (struct dmx_r_base *)rplay_fct);
    dmx_r_put((struct dmx_r_base *)rbuf);

    return ret;
}

static hi_s32 dmx_r_play_fct_start_idx_impl(struct dmx_r_play_fct *rplay_fct)
{
    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 dmx_r_play_fct_recv_idx_impl(struct dmx_r_play_fct *rplay_fct, hi_u32 req_num, hi_u32 time_out,
    hi_u32 *reqed_num, dmx_index_data *index)
{
    HI_ERR_DEMUX("some error may happened, not support!\n");

    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 dmx_r_play_fct_stop_idx_impl(struct dmx_r_play_fct *rplay_fct)
{
    HI_ERR_DEMUX("some error may happened, not support!\n");

    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 _dmx_r_play_fct_close_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret;

    struct dmx_r_buf *rbuf = rplay_fct->rbuf;

    DMX_NULL_POINTER_RETURN(rbuf);

    /* close the buffer */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->close(rbuf);
    dmx_r_put((struct dmx_r_base *)rbuf);

    /* set the play status */
    rplay_fct->is_opened = HI_FALSE;

    return ret;
}

static hi_s32 dmx_r_play_fct_close_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_close_impl(rplay_fct);

    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _play_fct_destroy_ts(struct dmx_mgmt *mgmt, struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret;
    unsigned long mask;
    unsigned long *p = HI_NULL;
    struct dmx_r_playfct_ts *rsub_play_fct = HI_NULL;
    struct dmx_r_buf *rbuf = HI_NULL;

    rsub_play_fct = (struct dmx_r_playfct_ts *)rplay_fct;
    DMX_NULL_POINTER_RETURN(rsub_play_fct);
    rbuf = rplay_fct->rbuf;
    DMX_NULL_POINTER_RETURN(rbuf);

    mask = BIT_MASK(rsub_play_fct->play_ts.ts_chan_id);
    osal_mutex_lock(&mgmt->ts_chan_lock);
    p = ((unsigned long *)mgmt->ts_bitmap) + BIT_WORD(rsub_play_fct->play_ts.ts_chan_id);
    if (!(*p & mask)) {
        osal_mutex_unlock(&mgmt->ts_chan_lock);
        HI_ERR_DEMUX("whole ts channel(%d) is invalid.\n", rsub_play_fct->play_ts.ts_chan_id);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    clear_bit(rsub_play_fct->play_ts.ts_chan_id, mgmt->ts_bitmap);
    osal_mutex_unlock(&mgmt->ts_chan_lock);

    /* post desroy begin */
    _dmx_r_play_fct_close_impl(rplay_fct);
    _dmx_r_play_fct_ts_detach_pid_ch_impl(rplay_fct);

    if (rbuf->secure_mode == DMX_SECURE_TEE) {
        dmx_tee_mem_info ts_mem_info = {0};
        ts_mem_info.buf_id = rbuf->base.id;
        ts_mem_info.buf_phy_addr = rbuf->buf_phy_addr;
        ts_mem_info.buf_size = rbuf->real_buf_size;

        dmx_tee_destroy_play_chan(rsub_play_fct->play_ts.ts_chan_id, DMX_PLAY_TYPE_TS, &ts_mem_info);
    }

    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 _play_fct_destroy_pes(struct dmx_mgmt *mgmt, struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret;
    unsigned long mask;
    unsigned long *p = HI_NULL;

    struct dmx_r_playfct_pes_sec *rsub_play_fct = HI_NULL;
    struct dmx_r_buf *rbuf = HI_NULL;

    rsub_play_fct = (struct dmx_r_playfct_pes_sec *)rplay_fct;
    DMX_NULL_POINTER_RETURN(rsub_play_fct);
    rbuf = rplay_fct->rbuf;
    DMX_NULL_POINTER_RETURN(rbuf);

    /* clear the bitmap */
    mask = BIT_MASK(rsub_play_fct->play_pes_sec.pes_sec_chan_id);
    osal_mutex_lock(&mgmt->pes_sec_chan_lock);
    p = ((unsigned long *)mgmt->pes_sec_bitmap) + BIT_WORD(rsub_play_fct->play_pes_sec.pes_sec_chan_id);
    if (!(*p & mask)) {
        osal_mutex_unlock(&mgmt->pes_sec_chan_lock);
        HI_ERR_DEMUX("pes channel(%d) is invalid.\n", rsub_play_fct->play_pes_sec.pes_sec_chan_id);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    clear_bit(rsub_play_fct->play_pes_sec.pes_sec_chan_id, mgmt->pes_sec_bitmap);
    osal_mutex_unlock(&mgmt->pes_sec_chan_lock);

    /* post desroy begin */
    _dmx_r_play_fct_close_impl(rplay_fct);
    _dmx_r_play_fct_pes_detach_pid_ch_impl(rplay_fct);
    /* clear the remain flt */
    _dmx_r_play_fct_pes_del_all_flt_impl(rplay_fct);

    if (rbuf->secure_mode == DMX_SECURE_TEE) {
        dmx_tee_mem_info pes_mem_info = {0};
        pes_mem_info.buf_id = rbuf->base.id;
        pes_mem_info.buf_phy_addr = rbuf->buf_phy_addr;
        pes_mem_info.buf_size = rbuf->real_buf_size;

        dmx_tee_destroy_play_chan(rsub_play_fct->play_pes_sec.pes_sec_chan_id, DMX_PLAY_TYPE_PES, &pes_mem_info);
    }

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 _play_fct_destroy_sec(struct dmx_mgmt *mgmt, struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret;
    unsigned long mask;
    unsigned long *p = HI_NULL;

    struct dmx_r_playfct_pes_sec *rsub_play_fct = HI_NULL;
    struct dmx_r_buf *rbuf = HI_NULL;

    rsub_play_fct = (struct dmx_r_playfct_pes_sec *)rplay_fct;
    DMX_NULL_POINTER_RETURN(rsub_play_fct);
    rbuf = rplay_fct->rbuf;
    DMX_NULL_POINTER_RETURN(rbuf);

    /* clear the bitmap */
    mask = BIT_MASK(rsub_play_fct->play_pes_sec.pes_sec_chan_id);
    osal_mutex_lock(&mgmt->pes_sec_chan_lock);
    p = ((unsigned long *)mgmt->pes_sec_bitmap) + BIT_WORD(rsub_play_fct->play_pes_sec.pes_sec_chan_id);
    if (!(*p & mask)) {
        osal_mutex_unlock(&mgmt->pes_sec_chan_lock);
        HI_ERR_DEMUX("sec channel(%d) is invalid.\n", rsub_play_fct->play_pes_sec.pes_sec_chan_id);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    clear_bit(rsub_play_fct->play_pes_sec.pes_sec_chan_id, mgmt->pes_sec_bitmap);
    osal_mutex_unlock(&mgmt->pes_sec_chan_lock);

    /* post desroy begin */
    _dmx_r_play_fct_close_impl(rplay_fct);
    _dmx_r_play_fct_sec_detach_pid_ch_impl(rplay_fct);
    /* clear the remain flt */
    _dmx_r_play_fct_sec_del_all_flt_impl(rplay_fct);

    if (rbuf->secure_mode == DMX_SECURE_TEE) {
        dmx_tee_mem_info sec_mem_info = {0};
        sec_mem_info.buf_id = rbuf->base.id;
        sec_mem_info.buf_phy_addr = rbuf->buf_phy_addr;
        sec_mem_info.buf_size = rbuf->real_buf_size;

        dmx_tee_destroy_play_chan(rsub_play_fct->play_pes_sec.pes_sec_chan_id,
                                  DMX_PLAY_TYPE_SEC, &sec_mem_info);
    }

    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 _play_fct_destroy_avscd(struct dmx_mgmt *mgmt, struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret;

    unsigned long mask;
    unsigned long *p = HI_NULL;

    struct dmx_r_playfct_avpes *rsub_play_fct = HI_NULL;
    struct dmx_r_buf *scd_rbuf = HI_NULL;

    rsub_play_fct = (struct dmx_r_playfct_avpes *)rplay_fct;
    DMX_NULL_POINTER_RETURN(rsub_play_fct);
    scd_rbuf = rsub_play_fct->play_avpes.scd_rbuf;
    DMX_NULL_POINTER_RETURN(scd_rbuf);

    /* clear the scd index id */
    dmx_hal_scd_clear_chan(mgmt, rsub_play_fct->play_avpes.av_pes_scd_id, HI_TRUE);

    /* deinit the buffer */
    dmx_r_get_raw((struct dmx_r_base *)scd_rbuf);
    ret = scd_rbuf->ops->deinit(scd_rbuf);
    dmx_r_put((struct dmx_r_base *)scd_rbuf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("scd buf destroy failed!\n");
        goto out;
    }

    /* destroy the scd buffer */
    ret = dmx_mgmt_destroy_buf((struct dmx_r_base *)scd_rbuf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("destroy recfct buf failed!\n");
        goto out;
    }

    rsub_play_fct->play_avpes.scd_rbuf = HI_NULL;

    /* clear the bitmap */
    mask = BIT_MASK(rsub_play_fct->play_avpes.av_pes_scd_id);
    osal_mutex_lock(&mgmt->scd_chan_lock);
    p = ((unsigned long *)mgmt->scd_bitmap) + BIT_WORD(rsub_play_fct->play_avpes.av_pes_scd_id);
    if (!(*p & mask)) {
        osal_mutex_unlock(&mgmt->scd_chan_lock);
        HI_ERR_DEMUX("scd channel(%d) is invalid.\n", rsub_play_fct->play_avpes.av_pes_scd_id);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    clear_bit(rsub_play_fct->play_avpes.av_pes_scd_id, mgmt->scd_bitmap);
    osal_mutex_unlock(&mgmt->scd_chan_lock);

out:
    return ret;
}

static hi_s32 _play_fct_destroy_avpes(struct dmx_mgmt *mgmt, struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret;
    unsigned long mask;
    unsigned long *p = HI_NULL;

    struct dmx_r_playfct_avpes *rsub_play_fct = HI_NULL;
    struct dmx_r_buf *rbuf = HI_NULL;

    rsub_play_fct = (struct dmx_r_playfct_avpes *)rplay_fct;
    DMX_NULL_POINTER_RETURN(rsub_play_fct);
    rbuf = rplay_fct->rbuf;
    DMX_NULL_POINTER_RETURN(rbuf);

    /* clear the bitmap */
    mask = BIT_MASK(rsub_play_fct->play_avpes.av_pes_chan_id);
    osal_mutex_lock(&mgmt->avr_chan_lock);
    p = ((unsigned long *)mgmt->avr_bitmap) + BIT_WORD(rsub_play_fct->play_avpes.av_pes_chan_id);
    if (!(*p & mask)) {
        osal_mutex_unlock(&mgmt->avr_chan_lock);
        HI_ERR_DEMUX("av_pes channel(%d) is invalid.\n", rsub_play_fct->play_avpes.av_pes_chan_id);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    clear_bit(rsub_play_fct->play_avpes.av_pes_chan_id, mgmt->avr_bitmap);
    mgmt->av_pes_used_cnt--;
    osal_mutex_unlock(&mgmt->avr_chan_lock);

    /* post desroy begin */
    _dmx_r_play_fct_close_impl(rplay_fct);
    _dmx_r_play_fct_av_pes_detach_pid_ch_impl(rplay_fct);

    if (rbuf->secure_mode == DMX_SECURE_TEE) {
        dmx_tee_mem_info avpes_mem_info = {0};
        avpes_mem_info.buf_id = rbuf->base.id;
        avpes_mem_info.buf_phy_addr = rbuf->buf_phy_addr;
        avpes_mem_info.buf_size = rbuf->real_buf_size;

        dmx_tee_destroy_play_chan(rsub_play_fct->play_avpes.av_pes_chan_id,
                                  rplay_fct->play_fct_type, &avpes_mem_info);
    }

    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 _dmx_r_play_fct_destroy_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_u32 ret = HI_FAILURE;

    struct dmx_mgmt  *mgmt = HI_NULL;
    struct dmx_r_buf *rbuf = HI_NULL;

    rbuf = rplay_fct->rbuf;
    DMX_NULL_POINTER_RETURN(rbuf);

    mgmt = rplay_fct->base.mgmt;
    DMX_NULL_POINTER_RETURN(mgmt);

    switch (rplay_fct->play_fct_type) {
        case DMX_PLAY_TYPE_TS: {
            ret = _play_fct_destroy_ts(mgmt, rplay_fct);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("_play_fct_destroy_ts failed!\n");
            }
            break;
        }
        case DMX_PLAY_TYPE_PES: {
            ret = _play_fct_destroy_pes(mgmt, rplay_fct);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("_play_fct_destroy_pes failed!\n");
            }
            break;
        }
        case DMX_PLAY_TYPE_SEC: {
            ret = _play_fct_destroy_sec(mgmt, rplay_fct);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("_play_fct_destroy_sec failed!\n");
            }
            break;
        }
        case DMX_PLAY_TYPE_VID:
        case DMX_PLAY_TYPE_AUD: {
            ret = _play_fct_destroy_avpes(mgmt, rplay_fct);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("_play_fct_destroy_av_pes failed!\n");
            }
#ifdef HI_DEMUX_PROC_SUPPORT
            if (rplay_fct->save_es_handle) {
                hi_drv_file_close(rplay_fct->save_es_handle);
                rplay_fct->save_es_handle = HI_NULL;
            }
#endif
            if (rplay_fct->vcodec_type != DMX_VCODEC_TYPE_MAX) {
                ret = _play_fct_destroy_avscd(mgmt, rplay_fct);
                if (ret != HI_SUCCESS) {
                    HI_ERR_DEMUX("_play_fct_destroy_avscd failed!\n");
                }
            }
            break;
        }

        default: {
            HI_ERR_DEMUX("invalid play type[0x%x]!\n", rplay_fct->play_fct_type);
            ret = HI_ERR_DMX_INVALID_PARA;
            break;
        }
    }

    HI_DBG_DEMUX("run here destroy buf, address[%#x], size[%#x]!\n",
                 rbuf->buf_phy_addr, rbuf->real_buf_size);

    /* destroy the buffer object */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->deinit(rbuf);
    dmx_r_put((struct dmx_r_base *)rbuf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("deinit playfct buf failed!\n");
    }

    /* destroy the buffer */
    ret = dmx_mgmt_destroy_buf((struct dmx_r_base *)rbuf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("destroy playfct buf failed!\n");
        goto out;
    }

    rplay_fct->rbuf = HI_NULL;

out:
    return ret;
}

static hi_s32 dmx_r_play_fct_destroy_impl(struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_destroy_impl(rplay_fct);
    if (ret == HI_SUCCESS) {
        rplay_fct->staled = HI_TRUE;
    }

    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_pre_mmap_impl(struct dmx_r_play_fct *rplay_fct, hi_s64 *buf_handle, hi_u32 *buf_size,
    hi_void **buf_usr_addr)
{
    hi_s32 ret;
    struct dmx_mgmt *mgmt = rplay_fct->base.mgmt;
    struct dmx_r_buf *rbuf = rplay_fct->rbuf;
    DMX_NULL_POINTER_RETURN(mgmt);
    DMX_NULL_POINTER_RETURN(rbuf);

    /* get the buf obj */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->pre_mmap(rbuf, buf_handle, buf_size, buf_usr_addr);
    dmx_r_put((struct dmx_r_base *)rbuf);

    return ret;
}

static hi_s32 dmx_r_play_fct_pre_mmap_impl(struct dmx_r_play_fct *rplay_fct, hi_s64 *buf_handle, hi_u32 *buf_size,
    hi_void **buf_usr_addr)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_pre_mmap_impl(rplay_fct, buf_handle, buf_size, buf_usr_addr);

    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_play_fct_pst_mmap_impl(struct dmx_r_play_fct *rplay_fct, hi_void *buf_usr_addr)
{
    hi_s32 ret;

    struct dmx_r_buf *rbuf = rplay_fct->rbuf;

    DMX_NULL_POINTER_RETURN(rbuf);
    /* pst_map buffer */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->pst_mmap(rbuf, buf_usr_addr);
    dmx_r_put((struct dmx_r_base *)rbuf);

    return ret;
}

static hi_s32 dmx_r_play_fct_pst_mmap_impl(struct dmx_r_play_fct *rplay_fct, hi_void *buf_usr_addr)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);
    WARN_ON(rplay_fct->staled == HI_TRUE);

    ret = _dmx_r_play_fct_pst_mmap_impl(rplay_fct, buf_usr_addr);

    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

static hi_s32 dmx_r_play_fct_suspend_impl(struct dmx_r_play_fct *rplay_fct)
{
    HI_ERR_DEMUX("some error may happened, not support!\n");
    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 dmx_r_play_fct_resume_impl(struct dmx_r_play_fct *rplay_fct)
{
    HI_ERR_DEMUX("some error may happened, not support!\n");
    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 dmx_r_play_fct_get_buf_handle_impl(struct dmx_r_play_fct *rplay_fct,
    hi_handle *buf_handle, dmx_play_type *type)
{
    hi_s32 ret;

    osal_mutex_lock(&rplay_fct->lock);

    if (rplay_fct->secure_mode != DMX_SECURE_TEE) {
        HI_ERR_DEMUX("none secure buf can't get bufhandle.\n");
        ret = HI_ERR_DMX_NOT_SUPPORT;
        goto out;
    }

    *buf_handle = rplay_fct->rbuf->buf_handle;

    *type = rplay_fct->play_fct_type;

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rplay_fct->lock);

    return ret;
}

struct dmx_r_play_fct_ops g_dmx_play_fct_ops = {

    .create             = dmx_r_play_fct_create_impl,
    .get_pid_ch         = dmx_r_play_fct_get_pid_ch_impl,
    .open               = dmx_r_play_fct_open_impl,
    .get_attrs          = dmx_r_play_fct_get_attrs_impl,
    .set_attrs          = dmx_r_play_fct_set_attrs_impl,
    .get_status         = dmx_r_play_fct_get_status_impl,
    .get_scrambled_flag = dmx_r_play_fct_get_scrambled_flag_impl,
    .attach_pid_ch      = dmx_r_play_fct_attach_pid_ch_impl,
    .detach_pid_ch      = dmx_r_play_fct_detach_pid_ch_impl,
    .add_flt            = dmx_r_play_fct_add_flt_impl,
    .del_flt            = dmx_r_play_fct_del_flt_impl,
    .del_all_flt        = dmx_r_play_fct_del_all_flt_impl,
    .update_flt         = dmx_r_play_fct_update_flt_impl,
    .get_flt_attrs      = dmx_r_play_fct_get_flt_impl,
    .get_handle         = dmx_r_play_fct_get_handle_impl,
    .acquire_buf        = dmx_r_play_fct_acquire_buf_impl,
    .release_buf        = dmx_r_play_fct_release_buf_impl,
    .reset_buf          = dmx_r_play_fct_reset_buf_impl,
    .start_idx          = dmx_r_play_fct_start_idx_impl,
    .recv_idx           = dmx_r_play_fct_recv_idx_impl,
    .stop_idx           = dmx_r_play_fct_stop_idx_impl,
    .close              = dmx_r_play_fct_close_impl,
    .destroy            = dmx_r_play_fct_destroy_impl,
    .get_buf_handle     = dmx_r_play_fct_get_buf_handle_impl,

    .pre_mmap           = dmx_r_play_fct_pre_mmap_impl,
    .pst_mmap           = dmx_r_play_fct_pst_mmap_impl,

    .suspend            = dmx_r_play_fct_suspend_impl,
    .resume             = dmx_r_play_fct_resume_impl,
};

#ifdef HI_DEMUX_PROC_SUPPORT
static hi_u32 g_save_es_flag = 0;

hi_void dmx_play_fct_save_es(struct dmx_r_play_fct *rplay_fct, dmx_buffer *play_buf)
{
    struct tm now;
    hi_s32 ret;

    if (g_save_es_flag == 0) {
        return;
    }

    if (rplay_fct->secure_mode == DMX_SECURE_TEE) {
        if (rplay_fct->play_fct_type == DMX_PLAY_TYPE_VID) {
            HI_ERR_DEMUX("not support saving vid es data from secure buffer.\n");
            return;
        }
    }

    if (rplay_fct->play_fct_type != DMX_PLAY_TYPE_VID && rplay_fct->play_fct_type != DMX_PLAY_TYPE_AUD) {
        return;
    }

    if (play_buf[0].length == 0) {
        return;
    }

    if (rplay_fct->save_es_handle == HI_NULL) {
        hi_char str[DMX_FILE_NAME_LEN]  = {0};
        hi_char path[DMX_FILE_NAME_LEN] = {0};

        if (hi_drv_file_get_store_path(path, DMX_FILE_NAME_LEN) != HI_SUCCESS) {
            HI_ERR_DEMUX("get path failed\n");
            return;
        }

        time_to_tm(get_seconds(), 0, &now);

        if (rplay_fct->play_fct_type == DMX_PLAY_TYPE_VID) {
            ret = snprintf_s(str, sizeof(str), sizeof(str)  - 1, "%s/dmx_vid_%u-%02u_%02u_%02u.es",
                             path, rplay_fct->rpid_ch->pid, now.tm_hour, now.tm_min, now.tm_sec);
        } else {
            ret = snprintf_s(str, sizeof(str), sizeof(str) - 1, "%s/dmx_aud_%u-%02u_%02u_%02u.es",
                             path, rplay_fct->rpid_ch->pid, now.tm_hour, now.tm_min, now.tm_sec);
        }

        if (ret < 0) {
            HI_ERR_DEMUX("snprintf_s failed, ret = %d\n", ret);
            return;
        }

        rplay_fct->save_es_handle = hi_drv_file_open(str, 1);
        if (!rplay_fct->save_es_handle) {
            HI_ERR_DEMUX("open %s error\n", str);
            return;
        }
    }

    ret = hi_drv_file_write(rplay_fct->save_es_handle, (hi_u8 *)play_buf[0].ker_vir_addr, play_buf[0].length);
    if (ret != play_buf[0].length) {
        HI_ERR_DEMUX("hi_drv_file_write failed, ret = %d, length = %u\n", ret, play_buf[0].length);
    }
}

hi_s32 dmx_play_fct_start_save_es(hi_void)
{
    hi_char path[DMX_FILE_NAME_LEN] = {0};

    if (g_save_es_flag) {
        HI_ERR_DEMUX("already started\n");
        return HI_FAILURE;
    }

    if (hi_drv_file_get_store_path(path, DMX_FILE_NAME_LEN) != HI_SUCCESS) {
        HI_ERR_DEMUX("get path failed\n");
        return HI_FAILURE;
    }

    HI_PRINT("path = %s\n", path);

    g_save_es_flag = 1;

    return HI_SUCCESS;
}

hi_void dmx_play_fct_stop_save_es(hi_void)
{
    struct dmx_mgmt *rmgmt = HI_NULL;
    struct list_head *cur = HI_NULL;

    if (g_save_es_flag) {
        g_save_es_flag = 0;

        rmgmt = _get_dmx_mgmt();
        if (rmgmt->state != DMX_MGMT_OPENED) {
            return;
        }

        osal_mutex_lock(&rmgmt->play_fct_list_lock);
        list_for_each(cur, &rmgmt->play_fct_head) {
            struct dmx_r_play_fct *cur_play = list_entry(cur, struct dmx_r_play_fct, node);
            if (cur_play->save_es_handle) {
                hi_drv_file_close(cur_play->save_es_handle);
                cur_play->save_es_handle = HI_NULL;
            }
        }
        osal_mutex_unlock(&rmgmt->play_fct_list_lock);
    }
}
#endif /* HI_DEMUX_PROC_SUPPORT */

