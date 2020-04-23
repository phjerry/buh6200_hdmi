/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux basic function impl.
 * Author: sdk
 * Create: 2017-05-31
 */
#include <linux/hisilicon/securec.h>
#include <linux/kernel.h>
#include "linux/delay.h"
#include "linux/bitmap.h"
#include "linux/io.h"
#include "linux/interrupt.h"
#include "linux/printk.h"
#include "linux/ratelimit.h"

#include "hi_drv_sys.h"
#include "hi_drv_module.h"
#include "hi_drv_mem.h"
#include "hi_drv_proc.h"
#include "hi_drv_mem.h"
#include "hi_type.h"
#include "hi_errno.h"

#include "hal_demux.h"
#include "drv_demux_dscfct.h"
#include "drv_demux_func.h"
#include "drv_demux_define.h"
#include "drv_demux_utils.h"
#include "hi_drv_keyslot.h"
#include "hi_drv_klad.h"

/*
 * obj is staled after Close, it should discard all possible call request after that.
 */
#define DMX_R_DSC_FCT_GET(handle, rdsc_fct) ({ \
    hi_s32 ret = HI_FAILURE; \
    ret = dmx_r_get(handle, (struct dmx_r_base **)&rdsc_fct); \
    if (ret == HI_SUCCESS) { \
        if (&g_dmx_dsc_fct_ops != rdsc_fct->ops) { \
            dmx_r_put((struct dmx_r_base*)rdsc_fct);\
            HI_ERR_DEMUX("handle is in active, but not a valid dsc_fct handle(0x%x).\n", handle); \
            ret = HI_ERR_DMX_INVALID_PARA; \
        } \
    } \
    ret;\
})

hi_s32 dmx_mgmt_create_dsc_fct(const dmx_dsc_attrs *attrs, struct dmx_r_dsc_fct **rdsc_fct)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    if (!attrs) {
        HI_ERR_DEMUX("invalid attrs(%#x).\n", attrs);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = mgmt->ops->create_dsc_fct(mgmt, attrs, rdsc_fct);
out:
    return ret;
}

hi_s32 dmx_mgmt_destroy_dsc_fct(struct dmx_r_base *obj)
{
    hi_s32 ret;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    ret = mgmt->ops->destroy_dsc_fct(mgmt, (struct dmx_r_dsc_fct *)obj);

    return ret;
}

/*************************dmx_mgmt_dsc_fct*************************************************/
hi_s32 dmx_dsc_fct_create(const dmx_dsc_attrs *attrs, hi_handle *handle, struct dmx_session *session)
{
    hi_s32 ret;
    struct dmx_slot  *slot = HI_NULL;
    struct dmx_r_dsc_fct *rdsc_fct = HI_NULL;

    ret = dmx_mgmt_create_dsc_fct(attrs, &rdsc_fct);
    if (ret != HI_SUCCESS || rdsc_fct == HI_NULL) {
        HI_ERR_DEMUX("dmx_mgmt_create_dsc_fct failed, ret[0x%x], rdsc_fct[0x%x]\n", ret, rdsc_fct);
        goto out0;
    }

    /* get the dsc_fct obj */
    dmx_r_get_raw((struct dmx_r_base *)rdsc_fct);

    /* create the buf and others */
    ret = rdsc_fct->ops->create(rdsc_fct, attrs);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    /* associate a slot to the object  */
    ret = dmx_slot_create((struct dmx_r_base *)rdsc_fct, &slot);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    /* associate an slot to the session. */
    slot->release = dmx_dsc_fct_destroy;
    ret = dmx_session_add_slot(session, slot);
    if (ret != HI_SUCCESS) {
        goto out3;
    }

    *handle = slot->handle;
    rdsc_fct->dsc_fct_handle = slot->handle;

    /* release the dsc_fct obj */
    dmx_r_put((struct dmx_r_base *)rdsc_fct);

    return HI_SUCCESS;

out3:
    dmx_slot_destroy(slot);
out2:
    rdsc_fct->ops->destroy(rdsc_fct);
out1:
    dmx_r_put((struct dmx_r_base *)rdsc_fct);

    dmx_r_put((struct dmx_r_base *)rdsc_fct);
out0:
    return ret;
}

hi_s32 dmx_dsc_fct_get_attrs(hi_handle handle, dmx_dsc_attrs *attrs)
{
    hi_s32 ret;
    struct dmx_r_dsc_fct *rdsc_fct = HI_NULL;

    ret = DMX_R_DSC_FCT_GET(handle, rdsc_fct);
    if (ret != HI_SUCCESS || rdsc_fct == HI_NULL) {
        HI_ERR_DEMUX("DMX_R_DSC_FCT_GET failed, ret[0x%x], rdsc_fct[0x%x]\n", ret, rdsc_fct);
        goto out;
    }

    ret = rdsc_fct->ops->get_attrs(rdsc_fct, attrs);

    dmx_r_put((struct dmx_r_base *)rdsc_fct);
out:
    return ret;
}

hi_s32 dmx_dsc_fct_set_attrs(hi_handle handle, const dmx_dsc_attrs *attrs)
{
    hi_s32 ret;
    struct dmx_r_dsc_fct *rdsc_fct = HI_NULL;

    ret = DMX_R_DSC_FCT_GET(handle, rdsc_fct);
    if (ret != HI_SUCCESS || rdsc_fct == HI_NULL) {
        HI_ERR_DEMUX("DMX_R_DSC_FCT_GET failed, ret[0x%x], rdsc_fct[0x%x]\n", ret, rdsc_fct);
        goto out;
    }

    ret = rdsc_fct->ops->set_attrs(rdsc_fct, attrs);

    dmx_r_put((struct dmx_r_base *)rdsc_fct);
out:
    return ret;
}

hi_s32 dmx_dsc_fct_attach_keyslot(hi_handle handle, hi_handle ks_handle)
{
    hi_s32 ret;

    struct dmx_r_dsc_fct  *rdsc_fct  = HI_NULL;

    ret = DMX_R_DSC_FCT_GET(handle, rdsc_fct);
    if (ret != HI_SUCCESS || rdsc_fct == HI_NULL) {
        HI_ERR_DEMUX("DMX_R_DSC_FCT_GET failed, ret[0x%x], rdsc_fct[0x%x]\n", ret, rdsc_fct);
        goto out0;
    }

    ret = rdsc_fct->ops->attach_keyslot(rdsc_fct, ks_handle);

    dmx_r_put((struct dmx_r_base *)rdsc_fct);

out0:
    return ret;
}

hi_s32 dmx_dsc_fct_detach_keyslot(hi_handle handle)
{
    hi_s32 ret;

    struct dmx_r_dsc_fct  *rdsc_fct  = HI_NULL;

    ret = DMX_R_DSC_FCT_GET(handle, rdsc_fct);
    if (ret != HI_SUCCESS || rdsc_fct == HI_NULL) {
        HI_ERR_DEMUX("DMX_R_DSC_FCT_GET failed, ret[0x%x], rdsc_fct[0x%x]\n", ret, rdsc_fct);
        goto out0;
    }

    ret = rdsc_fct->ops->detach_keyslot(rdsc_fct);

    dmx_r_put((struct dmx_r_base *)rdsc_fct);

out0:
    return ret;
}

hi_s32 dmx_dsc_get_keyslot_handle(hi_handle handle, hi_handle *ks_handle)
{
    hi_s32 ret;

    struct dmx_r_dsc_fct  *rdsc_fct  = HI_NULL;

    ret = DMX_R_DSC_FCT_GET(handle, rdsc_fct);
    if (ret != HI_SUCCESS || rdsc_fct == HI_NULL) {
        HI_ERR_DEMUX("DMX_R_DSC_FCT_GET failed, ret[0x%x], rdsc_fct[0x%x]\n", ret, rdsc_fct);
        goto out0;
    }

    ret = rdsc_fct->ops->get_keyslot_handle(rdsc_fct, ks_handle);

    dmx_r_put((struct dmx_r_base *)rdsc_fct);

out0:
    return ret;
}

hi_s32 dmx_dsc_fct_attach(hi_handle handle, hi_handle ch_handle)
{
    hi_s32 ret;

    struct dmx_r_dsc_fct  *rdsc_fct  = HI_NULL;
    struct dmx_r_pid_ch   *rpid_ch   = HI_NULL;

    ret = DMX_R_DSC_FCT_GET(handle, rdsc_fct);
    if (ret != HI_SUCCESS || rdsc_fct == HI_NULL) {
        HI_ERR_DEMUX("DMX_R_DSC_FCT_GET failed, ret[0x%x], rdsc_fct[0x%x]\n", ret, rdsc_fct);
        goto out0;
    }

    ret = DMX_R_PID_CH_GET(ch_handle, rpid_ch);
    if (ret != HI_SUCCESS || rpid_ch == HI_NULL) {
        HI_ERR_DEMUX("DMX_R_PID_CH_GET failed, ret[0x%x], rdsc_fct[0x%x]\n", ret, rdsc_fct);
        goto out1;
    }

    ret = rdsc_fct->ops->attach(rdsc_fct, rpid_ch);

    dmx_r_put((struct dmx_r_base *)rpid_ch);
out1:
    dmx_r_put((struct dmx_r_base *)rdsc_fct);
out0:
    return ret;
}

hi_s32 dmx_dsc_fct_detach(hi_handle handle, hi_handle ch_handle)
{
    hi_s32 ret;

    struct dmx_r_dsc_fct  *rdsc_fct  = HI_NULL;
    struct dmx_r_pid_ch   *rpid_ch   = HI_NULL;

    ret = DMX_R_DSC_FCT_GET(handle, rdsc_fct);
    if (ret != HI_SUCCESS || rdsc_fct == HI_NULL) {
        HI_ERR_DEMUX("DMX_R_DSC_FCT_GET failed, ret[0x%x], rdsc_fct[0x%x]\n", ret, rdsc_fct);
        goto out0;
    }

    ret = DMX_R_PID_CH_GET(ch_handle, rpid_ch);
    if (ret != HI_SUCCESS || rpid_ch == HI_NULL) {
        HI_ERR_DEMUX("DMX_R_PID_CH_GET failed, ret[0x%x], rdsc_fct[0x%x]\n", ret, rdsc_fct);
        goto out1;
    }

    ret = rdsc_fct->ops->detach(rdsc_fct, rpid_ch);

    dmx_r_put((struct dmx_r_base *)rpid_ch);

out1:
    dmx_r_put((struct dmx_r_base *)rdsc_fct);
out0:
    return ret;
}

hi_s32 dmx_dsc_fct_set_key(hi_handle handle, dmx_dsc_key_type key_type, const hi_u8 *key, hi_u32 len)
{
    hi_s32 ret;

    struct dmx_r_dsc_fct  *rdsc_fct  = HI_NULL;

    ret = DMX_R_DSC_FCT_GET(handle, rdsc_fct);
    if (ret != HI_SUCCESS || rdsc_fct == HI_NULL) {
        HI_ERR_DEMUX("DMX_R_DSC_FCT_GET failed, ret[0x%x], rdsc_fct[0x%x]\n", ret, rdsc_fct);
        goto out;
    }

    ret = rdsc_fct->ops->set_key(rdsc_fct, key_type, key, len);

    dmx_r_put((struct dmx_r_base *)rdsc_fct);
out:
    return ret;
}

hi_s32 dmx_dsc_fct_set_iv(hi_handle handle, dmx_dsc_key_type ivtype, const hi_u8 *iv, hi_u32 len)
{
    hi_s32 ret;

    struct dmx_r_dsc_fct  *rdsc_fct  = HI_NULL;

    ret = DMX_R_DSC_FCT_GET(handle, rdsc_fct);
    if (ret != HI_SUCCESS || rdsc_fct == HI_NULL) {
        HI_ERR_DEMUX("DMX_R_DSC_FCT_GET failed, ret[0x%x], rdsc_fct[0x%x]\n", ret, rdsc_fct);
        goto out;
    }

    ret = rdsc_fct->ops->set_iv(rdsc_fct, ivtype, iv, len);

    dmx_r_put((struct dmx_r_base *)rdsc_fct);
out:
    return ret;
}

hi_s32 dmx_dsc_fct_destroy(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_slot *slot        = HI_NULL;
    struct dmx_r_dsc_fct *rdsc_fct = HI_NULL;

    ret = DMX_R_DSC_FCT_GET(handle, rdsc_fct);
    if (ret != HI_SUCCESS || rdsc_fct == HI_NULL) {
        HI_ERR_DEMUX("DMX_R_DSC_FCT_GET failed, ret[0x%x], rdsc_fct[0x%x]\n", ret, rdsc_fct);
        goto out0;
    }

    ret = dmx_slot_find(handle, &slot);
    if (ret != HI_SUCCESS || slot == HI_NULL) {
        goto out1;
    }

    dmx_session_del_slot(slot->session, slot);

    dmx_slot_destroy(slot);

    ret = rdsc_fct->ops->destroy(rdsc_fct);

    dmx_r_put((struct dmx_r_base *)rdsc_fct);
out1:
    dmx_r_put((struct dmx_r_base *)rdsc_fct);
out0:
    return ret;
}

hi_s32 dmx_dsc_get_dsc_key_handle(hi_handle pid_ch_handle, hi_handle *dsc_handle)
{
    hi_s32 ret;

    struct dmx_r_pid_ch *rpid_ch = HI_NULL;

    ret = DMX_R_PID_CH_GET(pid_ch_handle, rpid_ch);
    if (ret != HI_SUCCESS) {
        goto error_exit;
    }

    if (rpid_ch->rdsc_fct == HI_NULL || rpid_ch->rdsc_fct->dsc_fct_handle == HI_INVALID_HANDLE) {
        HI_ERR_DEMUX("pid channel do not attach descrambler! pid_ch_handle=%#x\n", pid_ch_handle);
        ret = HI_ERR_DMX_NOATTACH_KEY;
        goto pid_ch_put;
    }

    *dsc_handle = rpid_ch->rdsc_fct->dsc_fct_handle;

pid_ch_put:
    dmx_r_put((struct dmx_r_base *)rpid_ch);
error_exit:
    return ret;
}

hi_s32 dmx_dsc_get_free_cnt(hi_u32 *free_cnt)
{
    hi_u32 id;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    id = find_first_zero_bit(mgmt->dsc_fct_bitmap, mgmt->dsc_fct_cnt);
    if (!(id < mgmt->dsc_fct_cnt)) {
        *free_cnt = 0;
        HI_DBG_DEMUX("there is no available descrambler now!\n");
    } else {
        *free_cnt = mgmt->dsc_fct_cnt - id;
    }

    return HI_SUCCESS;
}

/*************************dmx_r_dsc_fct_xxx_impl*************************************************/
static inline hi_bool dsc_fct_staled(struct dmx_r_dsc_fct *rdsc_fct)
{
    if (unlikely(rdsc_fct->staled == HI_TRUE)) {
        HI_ERR_DEMUX("rdsc_fct is staled.\n");
        return HI_TRUE;
    }

    return HI_FALSE;
}

static hi_s32 get_dsc_type_id(dmx_dsc_key_alg dsc_key_alg_type, hi_u32 *type_id)
{
    hi_u32 type = 0;

    DMX_NULL_POINTER_RETURN(type_id);

    switch (dsc_key_alg_type) {
        case DMX_DESCRAMBLER_TYPE_CSA3:
            type = 0x10;
            break;

        case DMX_DESCRAMBLER_TYPE_AES_IPTV:
            type = 0x20;
            break;

        case DMX_DESCRAMBLER_TYPE_AES_ECB:
            type = 0x21;
            break;

        case DMX_DESCRAMBLER_TYPE_AES_CI:
        case DMX_DESCRAMBLER_TYPE_AES_CBC:
        case DMX_DESCRAMBLER_TYPE_AES_CISSA:
            type = 0x22;
            break;

        case DMX_DESCRAMBLER_TYPE_AES_ECB_L:
            type = 0x24;
            break;

        case DMX_DESCRAMBLER_TYPE_AES_CBC_L:
            type = 0x25;
            break;

        case DMX_DESCRAMBLER_TYPE_DES_IPTV:
            type = 0x30;
            break;

        case DMX_DESCRAMBLER_TYPE_DES_ECB:
            type = 0x32;
            break;

        case DMX_DESCRAMBLER_TYPE_DES_CI:
        case DMX_DESCRAMBLER_TYPE_DES_CBC:
            type = 0x33;
            break;

        case DMX_DESCRAMBLER_TYPE_AES_NS:
            type = 0x40;
            break;

        case DMX_DESCRAMBLER_TYPE_SMS4_NS:
            type = 0x41;
            break;

        case DMX_DESCRAMBLER_TYPE_SMS4_IPTV:
            type = 0x50;
            break;

        case DMX_DESCRAMBLER_TYPE_SMS4_ECB:
            type = 0x51;
            break;

        case DMX_DESCRAMBLER_TYPE_SMS4_CBC:
            type = 0x53;
            break;

        case DMX_DESCRAMBLER_TYPE_TDES_IPTV:
            type = 0x70;
            break;

        case DMX_DESCRAMBLER_TYPE_TDES_ECB:
            type = 0x71;
            break;

        case DMX_DESCRAMBLER_TYPE_TDES_CBC:
            type = 0x73;
            break;

        case DMX_DESCRAMBLER_TYPE_MULTI2_IPTV:
            type = 0x80;
            break;

        case DMX_DESCRAMBLER_TYPE_MULTI2_ECB:
            type = 0x81;
            break;

        case DMX_DESCRAMBLER_TYPE_MULTI2_CBC:
            type = 0x83;
            break;

        case DMX_DESCRAMBLER_TYPE_ASA:
            type = 0x91;
            break;

        case DMX_DESCRAMBLER_TYPE_CSA2:
        default :
            type = 0;
    }

    *type_id = type;

    return HI_SUCCESS;
}

static hi_s32 _dmx_r_dsc_fct_create_impl(struct dmx_r_dsc_fct *rdsc_fct, const dmx_dsc_attrs *attrs)
{
    struct dmx_mgmt   *mgmt = rdsc_fct->base.mgmt;

    DMX_NULL_POINTER_RETURN(mgmt);

    if ((rdsc_fct->keyslot_attached == HI_FALSE) && (rdsc_fct->keyslot_handle == HI_INVALID_HANDLE)) {
        return HI_SUCCESS;
    }

    if (attrs->alg == DMX_DESCRAMBLER_TYPE_CSA2) {
        if ((attrs->entropy_reduction != DMX_CA_ENTROPY_CLOSE) && (attrs->entropy_reduction != DMX_CA_ENTROPY_OPEN)) {
            HI_ERR_DEMUX("entropy_reduction=%d\n", attrs->entropy_reduction);
            return HI_ERR_DMX_INVALID_PARA;
        }
        dmx_hal_mdscset_entropy_reduction(mgmt, rdsc_fct->base.id, attrs->entropy_reduction);
    }

    dmx_hal_mdscset_en(mgmt, HI_TRUE, HI_TRUE, HI_TRUE);
    dmx_hal_mdscdis_ca_core(mgmt, HI_FALSE);

    return HI_SUCCESS;
}

static hi_s32 _dmx_r_dsc_fct_get_attrs_impl(struct dmx_r_dsc_fct *rdsc_fct, dmx_dsc_attrs *attrs)
{
    hi_s32 ret;

    attrs->ca_type           = rdsc_fct->ca_type;
    attrs->entropy_reduction = rdsc_fct->entropy_reduction;
    attrs->alg               = rdsc_fct->alg;

    ret = HI_SUCCESS;

    return ret;
}

static hi_s32 _dmx_r_dsc_fct_set_attrs_impl(struct dmx_r_dsc_fct *rdsc_fct, const dmx_dsc_attrs *attrs)
{
    hi_s32 ret;

    if (attrs->entropy_reduction != rdsc_fct->entropy_reduction) {
        HI_ERR_DEMUX("dsc_fct not support change entropy_reduction yet!\n");
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    rdsc_fct->ca_type = attrs->ca_type;
    rdsc_fct->alg     = attrs->alg;

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 _dmx_r_dsc_fct_attach_keyslot_impl(struct dmx_r_dsc_fct *rdsc_fct, hi_handle ks_handle)
{
    hi_s32 ret;
    struct dmx_mgmt *mgmt = rdsc_fct->base.mgmt;
    struct list_head *pid_ch_pos = HI_NULL;
    struct list_head *raw_pid_chan_pos = HI_NULL;

    if ((rdsc_fct->keyslot_handle != HI_INVALID_HANDLE) && (rdsc_fct->keyslot_attached == HI_TRUE)) {
        HI_ERR_DEMUX("This descrambler has already attach keyslot!\n");
        return HI_ERR_DMX_ATTACHED_KEY;
    }

    /* if rdsc_fct->keyslot_handle == HI_INVALID_HANDLE meanse dsc did not attach keyslot */
    rdsc_fct->keyslot_handle  = ks_handle;
    rdsc_fct->keyslot_attached = HI_TRUE;
    rdsc_fct->base.id = KS_HANDLE_2_ID(ks_handle);

    dmx_hal_mdscset_entropy_reduction(mgmt, rdsc_fct->base.id, DMX_CA_ENTROPY_CLOSE);
    dmx_hal_mdscset_en(mgmt, HI_TRUE, HI_TRUE, HI_TRUE);
    dmx_hal_mdscdis_ca_core(mgmt, HI_FALSE);

    /* if dsc attach pid channel first, we should set register for each pid channel */
    osal_mutex_lock(&rdsc_fct->pid_ch_list_lock);
    /* Find pid channel by rdsc_fct */
    list_for_each(pid_ch_pos, &rdsc_fct->pid_ch_head) {
        struct dmx_r_pid_ch *rpid_ch = list_entry(pid_ch_pos, struct dmx_r_pid_ch, node2);
        osal_mutex_lock(&rpid_ch->raw_pid_ch_list_lock);
        /* Find raw pid channel by pid channel */
        list_for_each(raw_pid_chan_pos, &rpid_ch->raw_pid_ch_head) {
            struct dmx_r_raw_pid_ch *rraw_pid_ch = list_entry(raw_pid_chan_pos, struct dmx_r_raw_pid_ch, node1);
            /* enable the descrambler */
            dmx_hal_pid_cw_en_set(rpid_ch->base.mgmt, rraw_pid_ch->raw_pid_chan_id, HI_TRUE);
            dmx_hal_pid_set_cw_id(rpid_ch->base.mgmt, rraw_pid_ch->raw_pid_chan_id, rdsc_fct->base.id);
            /* enable ts descrambler support */
            dmx_hal_pid_set_dsc_type(rpid_ch->base.mgmt, rdsc_fct->base.id, HI_TRUE, HI_FALSE);
        }
        osal_mutex_unlock(&rpid_ch->raw_pid_ch_list_lock);
    }
    osal_mutex_unlock(&rdsc_fct->pid_ch_list_lock);

    ret = HI_SUCCESS;

    return ret;
}

static hi_s32 _dmx_r_dsc_fct_detach_keyslot_impl(struct dmx_r_dsc_fct *rdsc_fct)
{
    hi_s32 ret;

    rdsc_fct->keyslot_attached = HI_FALSE;
    rdsc_fct->base.id = 0;

    ret = HI_SUCCESS;

    return ret;
}

static hi_s32 _dmx_r_dsc_fct_attach_impl(struct dmx_r_dsc_fct *rdsc_fct, struct dmx_r_pid_ch *rpid_ch)
{
    hi_s32 ret;

    /* get the pid_ch obj */
    dmx_r_get_raw((struct dmx_r_base *)rpid_ch);

    ret = rpid_ch->ops->attach(rpid_ch, (struct dmx_r_base *)rdsc_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    osal_mutex_lock(&rdsc_fct->pid_ch_list_lock);
    list_add_tail(&rpid_ch->node2, &rdsc_fct->pid_ch_head);
    osal_mutex_unlock(&rdsc_fct->pid_ch_list_lock);

    return HI_SUCCESS;

out:
    /* put_pid_ch obj */
    dmx_r_put((struct dmx_r_base *)rpid_ch);
    return ret;
}

static hi_s32 _dmx_r_dsc_fct_detach_impl(struct dmx_r_dsc_fct *rdsc_fct, struct dmx_r_pid_ch *rpid_ch)
{
    hi_s32 ret;

    ret = rpid_ch->ops->detach(rpid_ch, (struct dmx_r_base *)rdsc_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    osal_mutex_lock(&rdsc_fct->pid_ch_list_lock);
    list_del(&rpid_ch->node2);
    osal_mutex_unlock(&rdsc_fct->pid_ch_list_lock);

    /* put the pid_ch obj */
    dmx_r_put((struct dmx_r_base *)rpid_ch);

out:
    return ret;
}

static hi_s32 _dmx_r_dsc_fct_detach_all_pid_ch_impl(struct dmx_r_dsc_fct *rdsc_fct)
{
    hi_s32 ret;
    struct list_head *node;
    struct list_head *tmp_node;

    osal_mutex_lock(&rdsc_fct->pid_ch_list_lock);
    list_for_each_safe(node, tmp_node, &rdsc_fct->pid_ch_head) {
        struct dmx_r_pid_ch *rpid_ch = list_entry(node, struct dmx_r_pid_ch, node2);
        ret = rpid_ch->ops->detach(rpid_ch, (struct dmx_r_base *)rdsc_fct);
        if (ret != HI_SUCCESS) {
            goto out;
        }

        /* delete the entry from the list */
        list_del(&rpid_ch->node2);

        /* put the pid_ch obj */
        dmx_r_put((struct dmx_r_base *)rpid_ch);
    }
    osal_mutex_unlock(&rdsc_fct->pid_ch_list_lock);

    return HI_SUCCESS;

out:
    osal_mutex_unlock(&rdsc_fct->pid_ch_list_lock);
    return ret;
}

static hi_s32 _dmx_r_dsc_fct_set_key_impl(struct dmx_r_dsc_fct *rdsc_fct, dmx_dsc_key_type key_type,
                                          const hi_u8 *key, hi_u32 len)
{
    hi_s32 ret;
    hi_u32 i;
    hi_u8 tmp_key[DMX_SYS_KEY_LEN] = {0};
    hi_u32 type = 0;
    struct dmx_mgmt *mgmt = rdsc_fct->base.mgmt;

    /* this code is temporarily useless */
    ret = get_dsc_type_id(rdsc_fct->alg, &type);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("get dsc_type ID failed!\n");
        goto out;
    }

    if (key_type == DMX_DSC_KEY_SYS) {
        if (len != DMX_SYS_KEY_LEN) {
            HI_ERR_DEMUX("system key len:%u, is not 32 Bytes!\n", len);
            ret = HI_ERR_DMX_INVALID_PARA;
            goto out;
        }

        /* multi2 alg need reverse system key */
        for (i = 0; i < len; i++) {
            tmp_key[i] = key[len - i - 1];
        }

        /* set multi2 system key */
        dmx_hal_mdsc_multi2_sys_key_cfg(mgmt, tmp_key, len);
    }

    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 _dmx_r_dsc_fct_set_iv_impl(struct dmx_r_dsc_fct *rdsc_fct, dmx_dsc_key_type ivtype,
                                         const hi_u8 *iv, hi_u32 len)
{
    hi_s32 ret;
    hi_u8 tmp_iv[DMX_MAX_KEY_LEN] = {0};
    klad_clear_iv_param iv_para = {0};
    klad_export_func *klad_ops = HI_NULL;
    /* the revert order of 0x445642544d4350544145534349535341 from ETSI TS 103 127 V1.1.1 (2013-05) */
    hi_u8 aes_cissa_iv[DMX_MAX_KEY_LEN] = { 0x41, 0x53, 0x53, 0x49, 0x43, 0x53, 0x45, 0x41,
        0x54, 0x50, 0x43, 0x4d, 0x54, 0x42, 0x56, 0x44 };

    if (rdsc_fct->keyslot_handle == HI_INVALID_HANDLE) {
        HI_ERR_DEMUX("keyslot handle is invallid, rdsc_fct=%#x!\n", rdsc_fct);
        ret = HI_FAILURE;
        goto out;
    }

    ret = memcpy_s(tmp_iv, sizeof(tmp_iv), iv, len);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("memcpy_s failed!\n");
        goto out;
    }

    if (rdsc_fct->alg == DMX_DESCRAMBLER_TYPE_AES_CISSA) {
        HI_WARN_DEMUX("Constant IV value for AES_CISSA\n");

        ret = memcpy_s(tmp_iv, sizeof(tmp_iv), aes_cissa_iv, sizeof(aes_cissa_iv));
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("memcpy_s failed!\n");
            goto out;
        }
    }

    iv_para.ks_handle = rdsc_fct->keyslot_handle;
    if (ivtype == DMX_DSC_KEY_ODD) {
        iv_para.is_odd = HI_TRUE;
    } else {
        iv_para.is_odd = HI_FALSE;
    }

    len = len > DMX_MAX_KEY_LEN ? DMX_MAX_KEY_LEN : len;
    ret = memcpy_s(iv_para.iv, HI_KLAD_MAX_IV_LEN, tmp_iv, len);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("memcpy_s failed!\n");
        goto out;
    }

    ret = hi_drv_module_get_func(HI_ID_KLAD, (hi_void **)&klad_ops);
    if (ret != HI_SUCCESS || klad_ops == HI_NULL) {
        HI_ERR_DEMUX("get klad ops failed!\n");
        ret = HI_FAILURE;
        goto out;
    }

    ret = klad_ops->klad_set_clear_iv(&iv_para);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("set clear iv failed!\n");
        goto out;
    }

    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 _dmx_r_dsc_fct_destroy_impl(struct dmx_r_dsc_fct *rdsc_fct)
{
    hi_u32 ret;

    struct dmx_mgmt   *mgmt = rdsc_fct->base.mgmt;

    DMX_NULL_POINTER_RETURN(mgmt);
    /* do something about hardware */
    if ((rdsc_fct->keyslot_attached == HI_TRUE) && (rdsc_fct->keyslot_handle != HI_INVALID_HANDLE)) {
        dmx_hal_mdscset_en(mgmt, HI_FALSE, HI_FALSE, HI_FALSE);
        rdsc_fct->keyslot_attached = HI_FALSE;
    }
    /* post desroy begin */
    _dmx_r_dsc_fct_detach_all_pid_ch_impl(rdsc_fct);

    ret = HI_SUCCESS;

    return ret;
}

static hi_s32 dmx_r_dsc_fct_create_impl(struct dmx_r_dsc_fct *rdsc_fct, const dmx_dsc_attrs *attrs)
{
    hi_s32 ret;

    osal_mutex_lock(&rdsc_fct->lock);

    WARN_ON(rdsc_fct->staled == HI_TRUE);

    ret = _dmx_r_dsc_fct_create_impl(rdsc_fct, attrs);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    /* Set the initialization iv value to zero. */
    if ((rdsc_fct->keyslot_attached == HI_TRUE) && (rdsc_fct->keyslot_handle != HI_INVALID_HANDLE)) {
        hi_u8 iv[DMX_MAX_KEY_LEN] = {0};

        ret = _dmx_r_dsc_fct_set_iv_impl(rdsc_fct, DMX_DSC_KEY_EVEN, iv, DMX_MAX_KEY_LEN);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("set init 0 even iv failed!\n");
            ret = HI_FAILURE;
            goto out;
        }

        ret = _dmx_r_dsc_fct_set_iv_impl(rdsc_fct, DMX_DSC_KEY_ODD, iv, DMX_MAX_KEY_LEN);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("set init 0 odd iv failed!\n");
            ret = HI_FAILURE;
            goto out;
        }
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rdsc_fct->lock);

    return ret;
}

static hi_s32 dmx_r_dsc_fct_get_attrs_impl(struct dmx_r_dsc_fct *rdsc_fct, dmx_dsc_attrs *attrs)
{
    hi_s32 ret;

    osal_mutex_lock(&rdsc_fct->lock);

    WARN_ON(rdsc_fct->staled == HI_TRUE);

    ret = _dmx_r_dsc_fct_get_attrs_impl(rdsc_fct, attrs);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rdsc_fct->lock);

    return ret;
}

static hi_s32 dmx_r_dsc_fct_set_attrs_impl(struct dmx_r_dsc_fct *rdsc_fct, const dmx_dsc_attrs *attrs)
{
    hi_s32 ret;

    osal_mutex_lock(&rdsc_fct->lock);

    WARN_ON(rdsc_fct->staled == HI_TRUE);

    ret = _dmx_r_dsc_fct_set_attrs_impl(rdsc_fct, attrs);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rdsc_fct->lock);

    return ret;
}

static hi_s32 dmx_r_dsc_fct_attach_keyslot_impl(struct dmx_r_dsc_fct *rdsc_fct, hi_handle ks_handle)
{
    hi_s32 ret;

    osal_mutex_lock(&rdsc_fct->lock);

    WARN_ON(rdsc_fct->staled == HI_TRUE);

    ret = _dmx_r_dsc_fct_attach_keyslot_impl(rdsc_fct, ks_handle);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    /* set the initialization iv value to zero. */
    if ((rdsc_fct->keyslot_attached == HI_TRUE) && (rdsc_fct->keyslot_handle != HI_INVALID_HANDLE)) {
        hi_u8 iv[DMX_MAX_KEY_LEN] = {0};

        ret = _dmx_r_dsc_fct_set_iv_impl(rdsc_fct, DMX_DSC_KEY_EVEN, iv, DMX_MAX_KEY_LEN);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("set init 0 even iv failed!\n");
            ret = HI_FAILURE;
            goto out;
        }

        ret = _dmx_r_dsc_fct_set_iv_impl(rdsc_fct, DMX_DSC_KEY_ODD, iv, DMX_MAX_KEY_LEN);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("set init 0 odd iv failed!\n");
            ret = HI_FAILURE;
            goto out;
        }
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rdsc_fct->lock);

    return ret;
}

static hi_s32 dmx_r_dsc_fct_detach_keyslot_impl(struct dmx_r_dsc_fct *rdsc_fct)
{
    hi_s32 ret;

    osal_mutex_lock(&rdsc_fct->lock);

    WARN_ON(rdsc_fct->staled == HI_TRUE);

    ret = _dmx_r_dsc_fct_detach_keyslot_impl(rdsc_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rdsc_fct->lock);

    return ret;
}


static hi_s32 dmx_r_dsc_fct_get_keyslot_handle_impl(struct dmx_r_dsc_fct *rdsc_fct, hi_handle *ks_handle)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&rdsc_fct->lock);

    WARN_ON(rdsc_fct->staled == HI_TRUE);

    if (rdsc_fct->keyslot_handle == HI_INVALID_HANDLE) {
        HI_ERR_DEMUX("keyslot is not attached!\n");
        ret = HI_ERR_DMX_NOATTACH_KEY;
        goto out;
    }

    *ks_handle = rdsc_fct->keyslot_handle;

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rdsc_fct->lock);

    return ret;
}

static hi_s32 dmx_r_dsc_fct_attach_impl(struct dmx_r_dsc_fct *rdsc_fct, struct dmx_r_pid_ch *rpid_ch)
{
    hi_s32 ret;
    hi_u8 iv[DMX_MAX_KEY_LEN] = {0};

    osal_mutex_lock(&rdsc_fct->lock);

    WARN_ON(rdsc_fct->staled == HI_TRUE);

    ret = _dmx_r_dsc_fct_attach_impl(rdsc_fct, rpid_ch);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    /* set the initialization iv value to zero. */
    if (rdsc_fct->keyslot_handle != HI_INVALID_HANDLE) {
        ret = _dmx_r_dsc_fct_set_iv_impl(rdsc_fct, DMX_DSC_KEY_EVEN, iv, DMX_MAX_KEY_LEN);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("set init 0 even iv failed!\n");
            ret = HI_FAILURE;
            goto out;
        }

        ret = _dmx_r_dsc_fct_set_iv_impl(rdsc_fct, DMX_DSC_KEY_ODD, iv, DMX_MAX_KEY_LEN);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("set init 0 odd iv failed!\n");
            ret = HI_FAILURE;
            goto out;
        }
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rdsc_fct->lock);

    return ret;
}

static hi_s32 dmx_r_dsc_fct_detach_impl(struct dmx_r_dsc_fct *rdsc_fct, struct dmx_r_pid_ch *rpid_ch)
{
    hi_s32 ret;

    osal_mutex_lock(&rdsc_fct->lock);

    WARN_ON(rdsc_fct->staled == HI_TRUE);

    ret = _dmx_r_dsc_fct_detach_impl(rdsc_fct, rpid_ch);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rdsc_fct->lock);

    return ret;
}

static hi_s32 dmx_r_dsc_fct_set_key_impl(struct dmx_r_dsc_fct *rdsc_fct, dmx_dsc_key_type key_type,
                                         const hi_u8 *key, hi_u32 len)
{
    hi_s32 ret;

    osal_mutex_lock(&rdsc_fct->lock);

    WARN_ON(rdsc_fct->staled == HI_TRUE);

    ret = _dmx_r_dsc_fct_set_key_impl(rdsc_fct, key_type, key, len);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rdsc_fct->lock);

    return ret;
}

static hi_s32 dmx_r_dsc_fct_set_iv_impl(struct dmx_r_dsc_fct *rdsc_fct, dmx_dsc_key_type ivtype,
                                        const hi_u8 *iv, hi_u32 len)
{
    hi_s32 ret;

    osal_mutex_lock(&rdsc_fct->lock);

    WARN_ON(rdsc_fct->staled == HI_TRUE);

    ret = _dmx_r_dsc_fct_set_iv_impl(rdsc_fct, ivtype, iv, len);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rdsc_fct->lock);

    return ret;
}

static hi_s32 dmx_r_dsc_fct_destroy_impl(struct dmx_r_dsc_fct *rdsc_fct)
{
    hi_s32 ret;

    DMX_NULL_POINTER_RETURN(rdsc_fct);

    osal_mutex_lock(&rdsc_fct->lock);

    WARN_ON(rdsc_fct->staled == HI_TRUE);

    ret = _dmx_r_dsc_fct_destroy_impl(rdsc_fct);
    if (ret == HI_SUCCESS) {
        rdsc_fct->staled = HI_TRUE;
    }

    osal_mutex_unlock(&rdsc_fct->lock);

    return ret;
}

static hi_s32 dmx_r_dsc_fct_suspend_impl(struct dmx_r_dsc_fct *rdsc_fct)
{
    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 dmx_r_dsc_fct_resume_impl(struct dmx_r_dsc_fct *rdsc_fct)
{
    return HI_ERR_DMX_NOT_SUPPORT;
}

struct dmx_r_dsc_fct_ops g_dmx_dsc_fct_ops = {
    .create              = dmx_r_dsc_fct_create_impl,
    .get_attrs           = dmx_r_dsc_fct_get_attrs_impl,
    .set_attrs           = dmx_r_dsc_fct_set_attrs_impl,
    .attach_keyslot      = dmx_r_dsc_fct_attach_keyslot_impl,
    .detach_keyslot      = dmx_r_dsc_fct_detach_keyslot_impl,
    .get_keyslot_handle  = dmx_r_dsc_fct_get_keyslot_handle_impl,
    .attach              = dmx_r_dsc_fct_attach_impl,
    .detach              = dmx_r_dsc_fct_detach_impl,
    .set_key             = dmx_r_dsc_fct_set_key_impl,
    .set_iv              = dmx_r_dsc_fct_set_iv_impl,
    .destroy             = dmx_r_dsc_fct_destroy_impl,

    .suspend             = dmx_r_dsc_fct_suspend_impl,
    .resume              = dmx_r_dsc_fct_resume_impl,
};

