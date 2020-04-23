/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: tsr2rcipher basic function impl.
 */

#include <linux/ion.h>
#include <linux/dma-buf.h>
#include <linux/hisilicon/securec.h>

#include "hi_type.h"
#include "hi_osal.h"

#include "drv_tsr2rcipher_func.h"
#include "drv_tsr2rcipher_define.h"
#include "drv_tsr2rcipher_utils.h"

#include "hal_tsr2rcipher.h"

#include "hi_drv_sys.h"
#include "hi_drv_module.h"
#include "hi_drv_mem.h"
#include "hi_drv_proc.h"
#include "hi_drv_keyslot.h"
#include "hi_drv_klad.h"
#include "hi_drv_dev.h"

static struct tsr2rcipher_r_ch_ops g_tsr2rcipher_ch_ops;

#define TSR2RCIPHER_R_CH_GET(handle, rch) ({                                                       \
    hi_s32 ret_ = HI_FAILURE;                                                                      \
    ret_ = tsr2rcipher_r_get(handle, (struct tsr2rcipher_r_base**)&rch);                           \
    if (ret_ == HI_SUCCESS) {                                                                      \
       if (&g_tsr2rcipher_ch_ops != rch->ops) {                                                    \
            tsr2rcipher_r_put((struct tsr2rcipher_r_base*)rch);                                    \
            HI_ERR_TSR2RCIPHER("handle is in active, but not a valid ch handle(0x%x).\n", handle); \
            ret_ = HI_ERR_TSR2RCIPHER_INVALID_PARA;                                                \
       }                                                                                           \
    }                                                                                              \
    ret_;                                                                                          \
})

static hi_void tsr2rcipher_hal_process_rx_int(struct tsr2rcipher_mgmt *mgmt)
{
    unsigned long lock_flag;
    struct list_head *node;

    HI_DBG_TSR2RCIPHER("in %s %d\n", __FUNCTION__, __LINE__);

    /* rx dscript read finish interrupt */
    if (tsc_hal_rx_get_dsc_rd_total_int_status(mgmt)) {
        HI_DBG_TSR2RCIPHER("in %s %d\n", __FUNCTION__, __LINE__);
        osal_spin_lock_irqsave(&mgmt->ch_list_lock2, &lock_flag);
        list_for_each(node, &mgmt->ch_head) {
            struct tsr2rcipher_r_ch *rch = list_entry(node, struct tsr2rcipher_r_ch, node);
            if (tsc_hal_rx_get_dsc_rd_int_status(mgmt, rch->base.id)) {
                tsc_hal_rx_cls_dsc_rd_int_status(mgmt, rch->base.id);
                HI_DBG_TSR2RCIPHER("dsc rd finish interrupt happened, ch_id[0x%x]\n", rch->base.id);
                rch->rx_wait_dsc_rd = HI_TRUE;
                wake_up_interruptible(&rch->rx_wait_queue);
            }
        }
        osal_spin_unlock_irqrestore(&mgmt->ch_list_lock2, &lock_flag);
    }

    /* rx pkt count interrupt */
    if (tsc_hal_rx_get_pkt_cnt_total_int_status(mgmt)) {
        osal_spin_lock_irqsave(&mgmt->ch_list_lock2, &lock_flag);
        list_for_each(node, &mgmt->ch_head) {
            struct tsr2rcipher_r_ch *rch = list_entry(node, struct tsr2rcipher_r_ch, node);

            if (tsc_hal_rx_get_pkt_cnt_int_status(mgmt, rch->base.id)) {
                tsc_hal_rx_cls_pkt_cnt_int_status(mgmt, rch->base.id);
                HI_DBG_TSR2RCIPHER(" finish interrupt happened, ch_id[0x%x]\n", rch->base.id);
                rch->rx_wait_pkt_cnt = HI_TRUE;
                wake_up_interruptible(&rch->rx_wait_queue);
            }
        }
        osal_spin_unlock_irqrestore(&mgmt->ch_list_lock2, &lock_flag);
    }
}

static hi_void tsr2rcipher_hal_process_tx_int(struct tsr2rcipher_mgmt *mgmt)
{
    unsigned long lock_flag;
    struct list_head *node;

    HI_DBG_TSR2RCIPHER("in %s %d\n", __FUNCTION__, __LINE__);

    /* tx dscript read finish interrupt */
    if (tsc_hal_tx_get_dsc_rd_total_int_status(mgmt)) {
        HI_DBG_TSR2RCIPHER("in %s %d\n", __FUNCTION__, __LINE__);
        osal_spin_lock_irqsave(&mgmt->ch_list_lock2, &lock_flag);
        list_for_each(node, &mgmt->ch_head) {
            struct tsr2rcipher_r_ch *rch = list_entry(node, struct tsr2rcipher_r_ch, node);
            if (tsc_hal_tx_get_dsc_rd_int_status(mgmt, rch->base.id)) {
                tsc_hal_tx_cls_dsc_rd_int_status(mgmt, rch->base.id);
                HI_DBG_TSR2RCIPHER("dsc rd finish interrupt happened, ch_id[0x%x]\n", rch->base.id);
                rch->tx_wait_dsc_rd = HI_TRUE;
                wake_up_interruptible(&rch->tx_wait_queue);
            }
        }
        osal_spin_unlock_irqrestore(&mgmt->ch_list_lock2, &lock_flag);
    }

    /* tx pkt count interrupt */
    if (tsc_hal_tx_get_pkt_cnt_total_int_status(mgmt)) {
        osal_spin_lock_irqsave(&mgmt->ch_list_lock2, &lock_flag);
        list_for_each(node, &mgmt->ch_head) {
            struct tsr2rcipher_r_ch *rch = list_entry(node, struct tsr2rcipher_r_ch, node);

            if (tsc_hal_tx_get_pkt_cnt_int_status(mgmt, rch->base.id)) {
                tsc_hal_tx_cls_pkt_cnt_int_status(mgmt, rch->base.id);
                HI_DBG_TSR2RCIPHER(" finish interrupt happened, ch_id[0x%x]\n", rch->base.id);
                rch->tx_wait_pkt_cnt = HI_TRUE;
                wake_up_interruptible(&rch->tx_wait_queue);
            }
        }
        osal_spin_unlock_irqrestore(&mgmt->ch_list_lock2, &lock_flag);
    }
}

static hi_void tsr2rcipher_hal_process_cipher_int(struct tsr2rcipher_mgmt *mgmt)
{
    hi_size_t lock_flag = 0;
    struct list_head *node = HI_NULL;

    HI_ERR_TSR2RCIPHER("in %s %d\n", __FUNCTION__, __LINE__);

    /* fixme: something to do about hardware */

    TSR2RCIPHER_UNUSED(lock_flag);
    TSR2RCIPHER_UNUSED(node);
}

static hi_s32 tsr2rcipher_isr(hi_s32 irq, hi_void *priv)
{
    struct tsr2rcipher_mgmt *mgmt = (struct tsr2rcipher_mgmt *)priv;

    /* disable all tsr2rcipher interrupt, notice: disable the same level interrupt, then must use the raw interrupt register to get the interrupt status */
    tsc_hal_top_set_int(mgmt, HI_FALSE, HI_FALSE, HI_FALSE);
    HI_DBG_TSR2RCIPHER("in %s %d\n", __FUNCTION__, __LINE__);

    if (tsc_hal_top_get_rx_raw_int_status(mgmt)) {
        HI_DBG_TSR2RCIPHER("in %s %d\n", __FUNCTION__, __LINE__);
        /* clear the rx interrupt */
        tsc_hal_top_cls_rx_int_status(mgmt);
        /* process the rx interrupt */
        tsr2rcipher_hal_process_rx_int(mgmt);
    }

    if (tsc_hal_top_get_tx_raw_int_status(mgmt)) {
        HI_DBG_TSR2RCIPHER("in %s %d\n", __FUNCTION__, __LINE__);
        /* clear the tx interrupt */
        tsc_hal_top_cls_tx_int_status(mgmt);
        /* process the tx interrupt */
        tsr2rcipher_hal_process_tx_int(mgmt);
    }

    if (tsc_hal_top_get_cipher_raw_int_status(mgmt)) {
        HI_DBG_TSR2RCIPHER("in %s %d\n", __FUNCTION__, __LINE__);
        /* clear the cipher interrupt */
        tsc_hal_top_cls_cipher_int_status(mgmt);
        /* process the cipher interrupt */
        tsr2rcipher_hal_process_cipher_int(mgmt);
    }

    /* resume all tsr2rcipher interrupt */
    tsc_hal_top_set_int(mgmt, mgmt->rx_int, mgmt->tx_int, mgmt->cipher_int);

    return OSAL_IRQ_HANDLED;
}

hi_s32 tsr2rcipher_monitor(hi_void *data)
{
    while (!osal_kthread_should_stop()) {
        HI_DBG_TSR2RCIPHER("tsr2rcipher monitor running!\n");
        osal_msleep(30); /* scheduled every 30 milliseconds */
    }

    return HI_SUCCESS;
}

/* obj is staled after Close, it should discard all possible call request after that. */

hi_s32 tsr2rcipher_mgmt_create_ch(const tsr2rcipher_attr *attr, struct tsr2rcipher_r_ch **rch)
{
    struct tsr2rcipher_mgmt *mgmt = tsr2rcipher_get_mgmt();

    if (mgmt == HI_NULL) {
        HI_ERR_TSR2RCIPHER("mgmt is null!\n");
        return HI_FAILURE;
    }

    return mgmt->ops->create_ch(mgmt, attr, rch);
}

hi_s32 tsr2rcipher_mgmt_destroy_ch(struct tsr2rcipher_r_base *obj)
{
    struct tsr2rcipher_mgmt *mgmt = tsr2rcipher_get_mgmt();

    if (mgmt == HI_NULL) {
        HI_ERR_TSR2RCIPHER("mgmt is null!\n");
        return HI_FAILURE;
    }

    return mgmt->ops->destroy_ch(mgmt, (struct tsr2rcipher_r_ch *)obj);
}

hi_s32 tsr2rcipher_ch_create(const tsr2rcipher_attr *attr, hi_handle *handle, struct tsr2rcipher_session *session)
{
    hi_s32 ret;
    struct tsr2rcipher_slot *slot = HI_NULL;
    struct tsr2rcipher_r_ch *rch = HI_NULL;

    ret = tsr2rcipher_mgmt_create_ch(attr, &rch);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    tsr2rcipher_r_get_raw((struct tsr2rcipher_r_base *)rch);

    ret = rch->ops->create(rch);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    /* associate a slot to the object  */
    ret = tsr2rcipher_slot_create((struct tsr2rcipher_r_base *)rch, &slot);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    /* associate an slot to the session. */
    slot->release = tsr2rcipher_ch_destroy;
    ret = tsr2rcipher_session_add_slot(session, slot);
    if (ret != HI_SUCCESS) {
        goto out3;
    }

    *handle = slot->handle;

    /* save the ch_handle */
    rch->ch_handle = slot->handle;

    tsr2rcipher_r_put((struct tsr2rcipher_r_base *)rch);

    return HI_SUCCESS;

out3:
    tsr2rcipher_slot_destroy(slot);
out2:
    rch->ops->destroy(rch);
out1:
    tsr2rcipher_r_put((struct tsr2rcipher_r_base *)rch);

    tsr2rcipher_r_put((struct tsr2rcipher_r_base *)rch);
out0:
    return ret;
}

hi_s32 tsr2rcipher_ch_get_attr(hi_handle handle, tsr2rcipher_attr *attr)
{
    hi_s32 ret = HI_FAILURE;
    struct tsr2rcipher_r_ch *rch = HI_NULL;

    ret = TSR2RCIPHER_R_CH_GET(handle, rch);
    if (ret != HI_SUCCESS || rch == HI_NULL) {
        HI_ERR_TSR2RCIPHER("TSR2RCIPHER_R_CH_GET failed, ret[0x%x], rch[0x%x]\n", ret, rch);
        goto out;
    }

    ret = rch->ops->get_attr(rch, attr);

    tsr2rcipher_r_put((struct tsr2rcipher_r_base *)rch);
out:
    return ret;
}

hi_s32 tsr2rcipher_ch_set_attr(hi_handle handle, const tsr2rcipher_attr *attr)
{
    hi_s32 ret = HI_FAILURE;
    struct tsr2rcipher_r_ch *rch = HI_NULL;

    ret = TSR2RCIPHER_R_CH_GET(handle, rch);
    if (ret != HI_SUCCESS || rch == HI_NULL) {
        HI_ERR_TSR2RCIPHER("TSR2RCIPHER_R_CH_GET failed, ret[0x%x], rch[0x%x]\n", ret, rch);
        goto out;
    }

    ret = rch->ops->set_attr(rch, attr);

    tsr2rcipher_r_put((struct tsr2rcipher_r_base *)rch);
out:
    return ret;
}

hi_s32 tsr2rcipher_ch_get_ks_handle(hi_handle tsc_handle, hi_handle *ks_handle)
{
    hi_s32 ret;
    struct tsr2rcipher_r_ch *rch = HI_NULL;

    ret = TSR2RCIPHER_R_CH_GET(tsc_handle, rch);
    if (ret != HI_SUCCESS || rch == HI_NULL) {
        HI_ERR_TSR2RCIPHER("TSR2RCIPHER_R_CH_GET failed, ret[0x%x], rch[0x%x]\n", ret, rch);
        goto out;
    }

    ret = rch->ops->get_ks_handle(rch, ks_handle);

    tsr2rcipher_r_put((struct tsr2rcipher_r_base *)rch);
out:
    return ret;
}

hi_s32 tsr2rcipher_ch_attach_ks(hi_handle tsc_handle, hi_handle ks_handle)
{
    hi_s32 ret;
    struct tsr2rcipher_r_ch *rch = HI_NULL;

    ret = TSR2RCIPHER_R_CH_GET(tsc_handle, rch);
    if (ret != HI_SUCCESS || rch == HI_NULL) {
        HI_ERR_TSR2RCIPHER("TSR2RCIPHER_R_CH_GET failed, ret[0x%x], rch[0x%x]\n", ret, rch);
        goto out;
    }

    ret = rch->ops->attach_ks(rch, ks_handle);

    tsr2rcipher_r_put((struct tsr2rcipher_r_base *)rch);
out:
    return ret;
}

hi_s32 tsr2rcipher_ch_detach_ks(hi_handle tsc_handle, hi_handle ks_handle)
{
    hi_s32 ret;
    struct tsr2rcipher_r_ch *rch = HI_NULL;

    ret = TSR2RCIPHER_R_CH_GET(tsc_handle, rch);
    if (ret != HI_SUCCESS || rch == HI_NULL) {
        HI_ERR_TSR2RCIPHER("TSR2RCIPHER_R_CH_GET failed, ret[0x%x], rch[0x%x]\n", ret, rch);
        goto out;
    }

    ret = rch->ops->detach_ks(rch, ks_handle);

    tsr2rcipher_r_put((struct tsr2rcipher_r_base *)rch);
out:
    return ret;
}

hi_s32 tsr2rcipher_ch_set_iv(hi_handle handle, tsr2rcipher_iv_type iv_type, const hi_u8 *iv, hi_u32 iv_len)
{
    hi_s32 ret = HI_FAILURE;
    struct tsr2rcipher_r_ch *rch = HI_NULL;

    ret = TSR2RCIPHER_R_CH_GET(handle, rch);
    if (ret != HI_SUCCESS || rch == HI_NULL) {
        HI_ERR_TSR2RCIPHER("TSR2RCIPHER_R_CH_GET failed, ret[0x%x], rdsc_fct[0x%x]\n", ret, rch);
        goto out;
    }

    ret = rch->ops->set_iv(rch, iv_type, iv, iv_len);

    tsr2rcipher_r_put((struct tsr2rcipher_r_base *)rch);
out:
    return ret;
}

hi_s32 tsr2rcipher_ch_encrypt(hi_handle handle, tsr2rcipher_mem_handle src_mem_handle, tsr2rcipher_mem_handle dst_mem_handle, hi_u32 data_len)
{
    hi_s32 ret = HI_FAILURE;

    struct tsr2rcipher_r_ch *rch = HI_NULL;

    ret = TSR2RCIPHER_R_CH_GET(handle, rch);
    if (ret != HI_SUCCESS || rch == HI_NULL) {
        HI_ERR_TSR2RCIPHER("TSR2RCIPHER_R_CH_GET failed, ret[0x%x], rdsc_fct[0x%x]\n", ret, rch);
        goto out;
    }

    ret = rch->ops->encrypt(rch, src_mem_handle, dst_mem_handle, data_len);

    tsr2rcipher_r_put((struct tsr2rcipher_r_base *)rch);
out:
    return ret;

}

hi_s32 tsr2rcipher_ch_decrypt(hi_handle handle, tsr2rcipher_mem_handle src_mem_handle, tsr2rcipher_mem_handle dst_mem_handle, hi_u32 data_len)
{
    hi_s32 ret = HI_FAILURE;

    struct tsr2rcipher_r_ch *rch = HI_NULL;

    ret = TSR2RCIPHER_R_CH_GET(handle, rch);
    if (ret != HI_SUCCESS || rch == HI_NULL) {
        HI_ERR_TSR2RCIPHER("TSR2RCIPHER_R_CH_GET failed, ret[0x%x], rdsc_fct[0x%x]\n", ret, rch);
        goto out;
    }

    ret = rch->ops->decrypt(rch, src_mem_handle, dst_mem_handle, data_len);

    tsr2rcipher_r_put((struct tsr2rcipher_r_base *)rch);
out:
    return ret;
}

hi_s32 tsr2rcipher_ch_destroy(hi_handle handle)
{
    hi_s32 ret;
    struct tsr2rcipher_slot *slot = HI_NULL;
    struct tsr2rcipher_r_ch *rch = HI_NULL;

    ret = TSR2RCIPHER_R_CH_GET(handle, rch);
    if (ret != HI_SUCCESS || rch == HI_NULL) {
        HI_ERR_TSR2RCIPHER("TSR2RCIPHER_R_CH_GET failed, ret[0x%x], rdsc_fct[0x%x]\n", ret, rch);
        goto out0;
    }

    ret = tsr2rcipher_slot_find(handle, &slot);
    if (ret != HI_SUCCESS || slot == HI_NULL) {
        goto out1;
    }

    tsr2rcipher_session_del_slot(slot->session, slot);

    tsr2rcipher_slot_destroy(slot);

    ret = rch->ops->destroy(rch);

    tsr2rcipher_r_put((struct tsr2rcipher_r_base *)rch);
out1:
    tsr2rcipher_r_put((struct tsr2rcipher_r_base *)rch);
out0:
    return ret;
}

/* tsr2rcipher_r_ch_xxx_impl */
static inline hi_bool tsr2rcipher_ch_staled(struct tsr2rcipher_r_ch *rch)
{
    if (unlikely(rch->staled == HI_TRUE)) {
        HI_ERR_TSR2RCIPHER("rch is staled.\n");
        return HI_TRUE;
    }

    return HI_FALSE;
}

static hi_s32 _tsr2rcipher_r_ch_create_impl(struct tsr2rcipher_r_ch *rch)
{
    struct tsr2rcipher_mgmt *mgmt = rch->base.mgmt;

    TSR2RCIPHER_CHECK_POINTER_RETURN(mgmt);

    /* rx configure */
    tsc_hal_rx_config(mgmt, rch->base.id, TSC_BUF_TYPE_LINK);

    /* tx configure */
    tsc_hal_tx_config(mgmt, rch->base.id, TSC_BUF_TYPE_LINK);

    return HI_SUCCESS;
}

static hi_s32 tsr2rcipher_r_ch_create_impl(struct tsr2rcipher_r_ch *rch)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&rch->lock);

    if (rch->staled == HI_TRUE) {
        HI_FATAL_TSR2RCIPHER("staled fatal error!\n");
        goto out;
    }

    ret = _tsr2rcipher_r_ch_create_impl(rch);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rch->lock);

    return ret;
}

static hi_s32 _tsr2rcipher_r_ch_get_attr_impl(struct tsr2rcipher_r_ch *rch, tsr2rcipher_attr *attr)
{
    hi_s32 ret;

    attr->alg          = rch->alg;
    attr->mode         = rch->mode;
    attr->is_crc_check = rch->is_crc_check;
    attr->is_create_ks = rch->is_create_ks;
    attr->is_odd_key   = rch->is_odd_key;

    ret = HI_SUCCESS;

    return ret;
}

static hi_s32 tsr2rcipher_r_ch_get_attr_impl(struct tsr2rcipher_r_ch *rch, tsr2rcipher_attr *attr)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&rch->lock);

    if (rch->staled == HI_TRUE) {
        HI_FATAL_TSR2RCIPHER("staled fatal error!\n");
        goto out;
    }

    ret = _tsr2rcipher_r_ch_get_attr_impl(rch, attr);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rch->lock);

    return ret;
}

static hi_s32 tsr2rcipher_get_key_id_and_core_type(tsr2rcipher_alg key_alg_type, hi_u32 *type_id, enum tsc_core_type *core_type)
{
    hi_u32 type = 0;
    enum tsc_core_type tmp_core = TSC_CORE_TYPE_MAX;

    TSR2RCIPHER_CHECK_POINTER_RETURN(type_id);
    TSR2RCIPHER_CHECK_POINTER_RETURN(core_type);

    switch (key_alg_type) {
        case TSR2RCIPHER_ALG_AES_IPTV: {
            type = 0x20;
            tmp_core = TSC_CORE_TYPE_AES;
            break;
        }
        case TSR2RCIPHER_ALG_AES_ECB: {
            type = 0x21;
            tmp_core = TSC_CORE_TYPE_AES;
            break;
        }
        case TSR2RCIPHER_ALG_AES_CBC: {
            type = 0x22;
            tmp_core = TSC_CORE_TYPE_AES;
            break;
        }
        case TSR2RCIPHER_ALG_AES_CTR: {
            type = 0x26;
            tmp_core = TSC_CORE_TYPE_AES;
            break;
        }
        case TSR2RCIPHER_ALG_SMS4_IPTV: {
            type = 0x50;
            tmp_core = TSC_CORE_TYPE_SMS4;
            break;
        }
        case TSR2RCIPHER_ALG_SMS4_ECB: {
            type = 0x51;
            tmp_core = TSC_CORE_TYPE_SMS4;
            break;
        }
        case TSR2RCIPHER_ALG_SMS4_CBC: {
            type = 0x53;
            tmp_core = TSC_CORE_TYPE_SMS4;
            break;
        }
        case TSR2RCIPHER_ALG_MAX:
        default: {
            HI_WARN_TSR2RCIPHER("No algorithm type specified!\n");
            type = 0x84;
            break;
        }
    }

    *type_id = type;
    *core_type = tmp_core;

    return HI_SUCCESS;
}

static hi_s32 _tsr2rcipher_r_ch_set_attr_impl(struct tsr2rcipher_r_ch *rch, const tsr2rcipher_attr *attr)
{
    hi_s32 ret;
    hi_u32 type = 0;
    enum tsc_core_type core_type;

    rch->alg          = attr->alg;
    rch->mode         = attr->mode;
    rch->is_crc_check = attr->is_crc_check;
    rch->is_odd_key   = attr->is_odd_key;

    ret = tsr2rcipher_get_key_id_and_core_type(rch->alg, &type, &core_type);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSR2RCIPHER("get key id and core type failed!\n");
        goto out;
    }

    rch->alg_code  = type;
    rch->core_type = core_type;

out:
    return ret;
}

static hi_s32 tsr2rcipher_r_ch_set_attr_impl(struct tsr2rcipher_r_ch *rch, const tsr2rcipher_attr *attr)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&rch->lock);

    if (rch->staled == HI_TRUE) {
        HI_FATAL_TSR2RCIPHER("staled fatal error!\n");
        goto out;
    }

    ret = _tsr2rcipher_r_ch_set_attr_impl(rch, attr);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rch->lock);

    return ret;
}

static hi_s32 _tsr2rcipher_r_ch_get_ks_handle_impl(struct tsr2rcipher_r_ch *rch, hi_handle *ks_handle)
{
    if (rch->ks_handle == HI_INVALID_HANDLE) {
        HI_ERR_TSR2RCIPHER("keyslot handle is invalid!\n");
        return HI_FAILURE;
    }
    *ks_handle = rch->ks_handle;

    return HI_SUCCESS;
}

static hi_s32 tsr2rcipher_r_ch_get_ks_handle_impl(struct tsr2rcipher_r_ch *rch, hi_handle *ks_handle)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&rch->lock);

    if (rch->staled == HI_TRUE) {
        HI_FATAL_TSR2RCIPHER("staled fatal error!\n");
        goto out;
    }

    ret = _tsr2rcipher_r_ch_get_ks_handle_impl(rch, ks_handle);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rch->lock);

    return ret;
}

static hi_s32 _tsr2rcipher_r_ch_attach_ks_impl(struct tsr2rcipher_r_ch *rch, hi_handle ks_handle)
{
    if (rch->ks_handle != HI_INVALID_HANDLE) {
        HI_ERR_TSR2RCIPHER("current tsr2rcipher instance is already attached the keyslot!\n");
        return HI_FAILURE;
    }
    rch->ks_handle = ks_handle;

    return HI_SUCCESS;
}

static hi_s32 tsr2rcipher_r_ch_attach_ks_impl(struct tsr2rcipher_r_ch *rch, hi_handle ks_handle)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&rch->lock);

    if (rch->staled == HI_TRUE) {
        HI_FATAL_TSR2RCIPHER("staled fatal error!\n");
        goto out;
    }

    ret = _tsr2rcipher_r_ch_attach_ks_impl(rch, ks_handle);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rch->lock);

    return ret;
}

static hi_s32 _tsr2rcipher_r_ch_detach_ks_impl(struct tsr2rcipher_r_ch *rch, hi_handle ks_handle)
{
    if (rch->is_create_ks == HI_TRUE) {
        HI_ERR_TSR2RCIPHER("This keyslot is created by callback, it will be detached when destroying tsr2rcipher!\n");
        return HI_FAILURE;
    }
    if (rch->ks_handle != ks_handle) {
        HI_ERR_TSR2RCIPHER("current tsr2rcipher instance is not attached this keyslot!\n");
        return HI_FAILURE;
    }
    rch->ks_handle = HI_INVALID_HANDLE;

    return HI_SUCCESS;
}

static hi_s32 tsr2rcipher_r_ch_detach_ks_impl(struct tsr2rcipher_r_ch *rch, hi_handle ks_handle)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&rch->lock);

    if (rch->staled == HI_TRUE) {
        HI_FATAL_TSR2RCIPHER("staled fatal error!\n");
        goto out;
    }

    ret = _tsr2rcipher_r_ch_detach_ks_impl(rch, ks_handle);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rch->lock);

    return ret;
}

static hi_s32 _tsr2rcipher_r_ch_set_iv_impl(struct tsr2rcipher_r_ch *rch, tsr2rcipher_iv_type iv_type,
                                            const hi_u8 *iv, hi_u32 iv_len)
{
    hi_s32 ret = HI_FAILURE;
    klad_clear_iv_param iv_para = {0};
    klad_export_func *klad_ops = HI_NULL;

    if (rch->ks_handle == HI_INVALID_HANDLE) {
        HI_ERR_TSR2RCIPHER("ks handle is invalid!\n");
        ret = HI_FAILURE;
        goto out;
    }

    rch->iv_type = iv_type;

    if (iv_type == TSR2RCIPHER_IV_ODD) {
        ret = memcpy_s(rch->odd_iv, iv_len, iv, iv_len);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSR2RCIPHER("memcpy_s failed!\n");
            goto out;
        }
        iv_para.is_odd = HI_TRUE;
    } else if (iv_type == TSR2RCIPHER_IV_EVEN) {
        ret = memcpy_s(rch->even_iv, iv_len, iv, iv_len);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSR2RCIPHER("memcpy_s failed!\n");
            goto out;
        }
        iv_para.is_odd = HI_FALSE;
    } else {
        HI_ERR_TSR2RCIPHER("invalid IV type[%#x]\n", iv_type);
        ret = HI_FAILURE;
        goto out;
    }

    ret = osal_exportfunc_get(HI_ID_KLAD, (hi_void **)&klad_ops);
    if (ret != HI_SUCCESS || klad_ops == HI_NULL) {
        HI_ERR_TSR2RCIPHER("get klad ops failed!\n");
        ret = HI_FAILURE;
        goto out;
    }

    iv_para.ks_handle = rch->ks_handle;
    ret = memcpy_s(iv_para.iv, iv_len, iv, iv_len);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSR2RCIPHER("memcpy_s failed!\n");
        goto out;
    }

    HI_INFO_TSR2RCIPHER("[Set_IV]: ks_handle[0x%x], iv_type[%d]!\n", rch->ks_handle, iv_type);

    ret = klad_ops->klad_set_clear_iv(&iv_para);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSR2RCIPHER("set clear iv failed!\n");
        goto out;
    }

    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 tsr2rcipher_r_ch_set_iv_impl(struct tsr2rcipher_r_ch *rch, tsr2rcipher_iv_type iv_type,
                                            const hi_u8 *iv, hi_u32 iv_len)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&rch->lock);

    if (rch->staled == HI_TRUE) {
        HI_FATAL_TSR2RCIPHER("staled fatal error!\n");
        goto out;
    }

    ret = _tsr2rcipher_r_ch_set_iv_impl(rch, iv_type, iv, iv_len);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rch->lock);

    return ret;
}

static hi_s32 _tsr2rcipher_get_phy_or_smmu_addr(hi_mem_handle_t buf_handle, struct dma_buf **dma, hi_u64 *addr)
{
    struct dma_buf *dma_buf = HI_NULL;

    dma_buf = dma_buf_get(buf_handle);
    if (IS_ERR_OR_NULL(dma_buf)) {
        HI_ERR_TSR2RCIPHER("get dma_buf[0x%x] failed from buf_handle[%d]!\n", dma_buf, buf_handle);
        return HI_FAILURE;
    }

#ifdef HI_SMMU_SUPPORT
    *addr = hi_dma_buf_nssmmu_map(dma_buf, 0);
#else
    *addr = hi_dma_buf_phy_get(dma_buf);
#endif

    *dma = dma_buf;

    return HI_SUCCESS;
}

static hi_void _tsr2rcipher_put_phy_or_smmu_addr(hi_mem_handle_t buf_handle, struct dma_buf *dma, hi_u64 addr)
{
    if (IS_ERR_OR_NULL(dma)) {
        HI_ERR_TSR2RCIPHER("dma_buf[0x%x] is invalid!\n", dma);
        return;
    }

#ifdef HI_SMMU_SUPPORT
    hi_dma_buf_nssmmu_unmap(dma, 0, addr);
#endif

    dma_buf_put(dma);
}

static hi_s32 _tsr2rcipher_r_ch_encrypt_impl(struct tsr2rcipher_r_ch *rch, hi_u64 src_addr, hi_u64 dst_addr,
                                            hi_u32 data_len)
{
    hi_s32 ret = HI_FAILURE;
    unsigned long lock_flag;
    struct tsr2rcipher_mgmt *mgmt = rch->base.mgmt;

    TSR2RCIPHER_CHECK_POINTER_RETURN(mgmt);

    if (!tsc_hal_rx_get_dsp_tor_status(mgmt, rch->base.id)) {
        HI_ERR_TSR2RCIPHER("rx dsc is busy!\n");
        ret = HI_ERR_TSR2RCIPHER_BUSY;
        goto out;
    }
    tsc_hal_rx_set_buf(mgmt, rch->base.id, src_addr, data_len);

    if (!tsc_hal_tx_get_dsp_tor_status(mgmt, rch->base.id)) {
        HI_ERR_TSR2RCIPHER("tx dsc is busy!\n");
        ret = HI_ERR_TSR2RCIPHER_BUSY;
        goto out;
    }
    tsc_hal_tx_set_buf(mgmt, rch->base.id, dst_addr, data_len);

    /* mode configure */
    tsc_hal_set_mode_ctl(mgmt, rch, TSC_CRYPT_TYPE_EN);

    osal_spin_lock_irqsave(&mgmt->ch_list_lock2, &lock_flag);
    rch->tx_wait_dsc_rd = HI_FALSE;
    osal_spin_unlock_irqrestore(&mgmt->ch_list_lock2, &lock_flag);

    HI_INFO_TSR2RCIPHER("[Encrypt]:tsc_handle[0x%x],ks_handle[0x%x],alg_code[0x%x],mode[%d],crc[%d],key_type[%d]!\n",
                        rch->ch_handle, rch->ks_handle, rch->alg_code, rch->mode, rch->is_crc_check, rch->is_odd_key);

    /* enable */
    tsc_hal_en_mode_ctl(mgmt, rch->base.id);

    ret = wait_event_interruptible_timeout(rch->tx_wait_queue, rch->tx_wait_dsc_rd, msecs_to_jiffies(2000));
    if (ret == 0) {
        HI_ERR_TSR2RCIPHER("tsr2rcipher encrypt timeout!\n");
        tsc_hal_dis_mode_ctl(mgmt, rch->base.id);
        ret = HI_ERR_TSR2RCIPHER_TIMEOUT;
        goto out;
    }

    osal_spin_lock_irqsave(&mgmt->ch_list_lock2, &lock_flag);
    rch->tx_wait_dsc_rd = HI_FALSE;
    osal_spin_unlock_irqrestore(&mgmt->ch_list_lock2, &lock_flag);

    /* disable */
    tsc_hal_dis_mode_ctl(mgmt, rch->base.id);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 tsr2rcipher_r_ch_encrypt_impl(struct tsr2rcipher_r_ch *rch, tsr2rcipher_mem_handle src_mem_handle,
                                            tsr2rcipher_mem_handle dst_mem_handle, hi_u32 data_len)
{
    hi_s32 ret = HI_FAILURE;
    struct dma_buf *src_dma = HI_NULL;
    struct dma_buf *dst_dma = HI_NULL;
    hi_u64 src_addr = 0;
    hi_u64 dst_addr = 0;

    osal_mutex_lock(&rch->lock);

    if (rch->staled == HI_TRUE) {
        HI_FATAL_TSR2RCIPHER("staled fatal error!\n");
        goto out0;
    }

    ret = _tsr2rcipher_get_phy_or_smmu_addr(src_mem_handle.mem_handle, &src_dma, &src_addr);
    if (ret != HI_SUCCESS) {
        goto out0;
    }
    src_addr += src_mem_handle.addr_offset;

    ret = _tsr2rcipher_get_phy_or_smmu_addr(dst_mem_handle.mem_handle, &dst_dma, &dst_addr);
    if (ret != HI_SUCCESS) {
        goto out1;
    }
    dst_addr += dst_mem_handle.addr_offset;

    if (src_addr % TSR2RCIPHER_ADDR_ALIGN || dst_addr % TSR2RCIPHER_ADDR_ALIGN) {
        HI_ERR_TSR2RCIPHER("src_phy_addr[0x%llx] and dst_phy_addr[0x%llx] must be 0x%x align!\n",
                            src_addr, dst_addr, TSR2RCIPHER_ADDR_ALIGN);
        ret = HI_FAILURE;
        goto out2;
    }

    ret = _tsr2rcipher_r_ch_encrypt_impl(rch, src_addr, dst_addr, data_len);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    ret = HI_SUCCESS;

out2:
    _tsr2rcipher_put_phy_or_smmu_addr(dst_mem_handle.mem_handle, dst_dma, dst_addr);
out1:
    _tsr2rcipher_put_phy_or_smmu_addr(src_mem_handle.mem_handle, src_dma, src_addr);
out0:
    osal_mutex_unlock(&rch->lock);
    return ret;
}

static hi_s32 _tsr2rcipher_r_ch_decrypt_impl(struct tsr2rcipher_r_ch *rch, hi_u64 src_addr, hi_u64 dst_addr,
                                            hi_u32 data_len)
{
    hi_s32 ret = HI_FAILURE;
    unsigned long lock_flag;
    struct tsr2rcipher_mgmt *mgmt = rch->base.mgmt;

    TSR2RCIPHER_CHECK_POINTER_RETURN(mgmt);

    if (!tsc_hal_rx_get_dsp_tor_status(mgmt, rch->base.id)) {
        HI_ERR_TSR2RCIPHER("rx dsc is busy!\n");
        ret = HI_ERR_TSR2RCIPHER_BUSY;
        goto out;
    }
    tsc_hal_rx_set_buf(mgmt, rch->base.id, src_addr, data_len);

    if (!tsc_hal_tx_get_dsp_tor_status(mgmt, rch->base.id)) {
        HI_ERR_TSR2RCIPHER("tx dsc is busy!\n");
        ret = HI_ERR_TSR2RCIPHER_BUSY;
        goto out;
    }
    tsc_hal_tx_set_buf(mgmt, rch->base.id, dst_addr, data_len);

    /* mode configure */
    tsc_hal_set_mode_ctl(mgmt, rch, TSC_CRYPT_TYPE_DE);

    osal_spin_lock_irqsave(&mgmt->ch_list_lock2, &lock_flag);
    rch->tx_wait_dsc_rd = HI_FALSE;
    osal_spin_unlock_irqrestore(&mgmt->ch_list_lock2, &lock_flag);

    HI_INFO_TSR2RCIPHER("[Decrypt]:tsc_handle[0x%x],ks_handle[0x%x],alg_code[0x%x],mode[%d],crc[%d],key_type[%d]!\n",
                        rch->ch_handle, rch->ks_handle, rch->alg_code, rch->mode, rch->is_crc_check, rch->is_odd_key);

    /* enable */
    tsc_hal_en_mode_ctl(mgmt, rch->base.id);

    ret = wait_event_interruptible_timeout(rch->tx_wait_queue, rch->tx_wait_dsc_rd, msecs_to_jiffies(2000));
    if (ret == 0) {
        HI_ERR_TSR2RCIPHER("tsr2rcipher decrypt timeout!\n");
        tsc_hal_dis_mode_ctl(mgmt, rch->base.id);
        ret = HI_ERR_TSR2RCIPHER_TIMEOUT;
        goto out;
    }

    osal_spin_lock_irqsave(&mgmt->ch_list_lock2, &lock_flag);
    rch->tx_wait_dsc_rd = HI_FALSE;
    osal_spin_unlock_irqrestore(&mgmt->ch_list_lock2, &lock_flag);

    /* disable */
    tsc_hal_dis_mode_ctl(mgmt, rch->base.id);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 tsr2rcipher_r_ch_decrypt_impl(struct tsr2rcipher_r_ch *rch, tsr2rcipher_mem_handle src_mem_handle,
                                            tsr2rcipher_mem_handle dst_mem_handle, hi_u32 data_len)
{
    hi_s32 ret = HI_FAILURE;
    struct dma_buf *src_dma = HI_NULL;
    struct dma_buf *dst_dma = HI_NULL;
    hi_u64 src_addr = 0;
    hi_u64 dst_addr = 0;

    osal_mutex_lock(&rch->lock);

    if (rch->staled == HI_TRUE) {
        HI_FATAL_TSR2RCIPHER("staled fatal error!\n");
        goto out0;
    }

    ret = _tsr2rcipher_get_phy_or_smmu_addr(src_mem_handle.mem_handle, &src_dma, &src_addr);
    if (ret != HI_SUCCESS) {
        goto out0;
    }
    src_addr += src_mem_handle.addr_offset;

    ret = _tsr2rcipher_get_phy_or_smmu_addr(dst_mem_handle.mem_handle, &dst_dma, &dst_addr);
    if (ret != HI_SUCCESS) {
        goto out1;
    }
    dst_addr += dst_mem_handle.addr_offset;

    if (src_addr % TSR2RCIPHER_ADDR_ALIGN || dst_addr % TSR2RCIPHER_ADDR_ALIGN) {
        HI_ERR_TSR2RCIPHER("src_phy_addr[0x%llx] and dst_phy_addr[0x%llx] must be 0x%x align!\n",
                            src_addr, dst_addr, TSR2RCIPHER_ADDR_ALIGN);
        ret = HI_FAILURE;
        goto out2;
    }

    ret = _tsr2rcipher_r_ch_decrypt_impl(rch, src_addr, dst_addr, data_len);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    ret = HI_SUCCESS;

out2:
    _tsr2rcipher_put_phy_or_smmu_addr(dst_mem_handle.mem_handle, dst_dma, dst_addr);
out1:
    _tsr2rcipher_put_phy_or_smmu_addr(src_mem_handle.mem_handle, src_dma, src_addr);
out0:
    osal_mutex_unlock(&rch->lock);
    return ret;
}

static hi_s32 _tsr2rcipher_r_ch_destroy_impl(struct tsr2rcipher_r_ch *rch)
{
    struct tsr2rcipher_mgmt *mgmt = rch->base.mgmt;

    TSR2RCIPHER_CHECK_POINTER_RETURN(mgmt);

    /* rx deconfigure */
    tsc_hal_rx_de_config(mgmt, rch->base.id);

    /* tx deconfigure */
    tsc_hal_tx_de_config(mgmt, rch->base.id);

    return HI_SUCCESS;
}

static hi_s32 tsr2rcipher_r_ch_destroy_impl(struct tsr2rcipher_r_ch *rch)
{
    hi_s32 ret = HI_FAILURE;

    TSR2RCIPHER_CHECK_POINTER_RETURN(rch);

    osal_mutex_lock(&rch->lock);

    if (rch->staled == HI_TRUE) {
        HI_FATAL_TSR2RCIPHER("staled fatal error!\n");
        goto out;
    }

    ret = _tsr2rcipher_r_ch_destroy_impl(rch);
    if (ret == HI_SUCCESS) {
        rch->staled = HI_TRUE;
    }

out:
    osal_mutex_unlock(&rch->lock);

    return ret;
}

static hi_s32 tsr2rcipher_r_ch_suspend_impl(struct tsr2rcipher_r_ch *rch)
{
    osal_mutex_lock(&rch->lock);

    /* disable the channel interrupt */
    tsc_dofunc_no_return(_tsr2rcipher_r_ch_destroy_impl(rch));

    osal_mutex_unlock(&rch->lock);

    return HI_SUCCESS;
}

static hi_s32 tsr2rcipher_r_ch_resume_impl(struct tsr2rcipher_r_ch *rch)
{
    osal_mutex_lock(&rch->lock);

    /* enable the channel interrupt */
    tsc_dofunc_no_return(_tsr2rcipher_r_ch_create_impl(rch));

    osal_mutex_unlock(&rch->lock);

    return HI_SUCCESS;
}

static struct tsr2rcipher_r_ch_ops g_tsr2rcipher_ch_ops = {
    .create        = tsr2rcipher_r_ch_create_impl,
    .get_attr      = tsr2rcipher_r_ch_get_attr_impl,
    .set_attr      = tsr2rcipher_r_ch_set_attr_impl,
    .get_ks_handle = tsr2rcipher_r_ch_get_ks_handle_impl,
    .attach_ks     = tsr2rcipher_r_ch_attach_ks_impl,
    .detach_ks     = tsr2rcipher_r_ch_detach_ks_impl,
    .set_iv        = tsr2rcipher_r_ch_set_iv_impl,
    .encrypt       = tsr2rcipher_r_ch_encrypt_impl,
    .decrypt       = tsr2rcipher_r_ch_decrypt_impl,
    .destroy       = tsr2rcipher_r_ch_destroy_impl,

    .suspend       = tsr2rcipher_r_ch_suspend_impl,
    .resume        = tsr2rcipher_r_ch_resume_impl,
};

hi_s32 tsr2rcipher_mgmt_init(hi_void)
{
    struct tsr2rcipher_mgmt *mgmt = _tsr2rcipher_get_mgmt();

    osal_mutex_init(&mgmt->lock);
    osal_atomic_init(&mgmt->ref_count);
    osal_atomic_set(&mgmt->ref_count, 0);

    return mgmt->ops->init(mgmt);
}

hi_void tsr2rcipher_mgmt_exit(hi_void)
{
    struct tsr2rcipher_mgmt *mgmt = tsr2rcipher_get_mgmt();

    if (mgmt == HI_NULL) {
        HI_ERR_TSR2RCIPHER("mgmt is null!\n");
        return;
    }

    if (mgmt->ops->exit(mgmt) != HI_SUCCESS) {
        mgmt->ops->show_info(mgmt);
        /* session must release all resource. */
        HI_ERR_TSR2RCIPHER("mgmt exit failed!\n");
    }

    osal_atomic_destory(&mgmt->ref_count);
    osal_mutex_destory(&mgmt->lock);
}

hi_s32 tsr2rcipher_mgmt_get_cap(tsr2rcipher_capability *cap)
{
    struct tsr2rcipher_mgmt *mgmt = tsr2rcipher_get_mgmt();

    if (mgmt == HI_NULL) {
        HI_ERR_TSR2RCIPHER("mgmt is null!\n");
        return HI_FAILURE;
    }

    return mgmt->ops->get_cap(mgmt, cap);
}

hi_s32 tsr2rcipher_mgmt_suspend(hi_void)
{
    struct tsr2rcipher_mgmt *mgmt = _tsr2rcipher_get_mgmt();

    return mgmt->ops->suspend(mgmt);
}

hi_s32 tsr2rcipher_mgmt_resume(hi_void)
{
    struct tsr2rcipher_mgmt *mgmt = _tsr2rcipher_get_mgmt();

    return mgmt->ops->resume(mgmt);
}

#ifdef TSR2RCIPHER_PROC_SUPPORT
static hi_s32 tsr2rcipher_proc_read(hi_void *seqfile, hi_void *private)
{
    struct tsr2rcipher_mgmt *mgmt = tsr2rcipher_get_mgmt();
    struct list_head *pos;

    if (mgmt == HI_NULL) {
        HI_WARN_TSR2RCIPHER("mgmt is null!\n");
        return HI_SUCCESS;
    }

    osal_proc_print(seqfile, "------------------TSR2RCIPHER STATUS------------------\n");
    osal_proc_print(seqfile, "ChnId    SlotId    Alg    Mode    KeyType    IVType    CRC\n");

    osal_mutex_lock(&mgmt->ch_list_lock);
    list_for_each(pos, &mgmt->ch_head) {
        struct tsr2rcipher_r_ch *rch = list_entry(pos, struct tsr2rcipher_r_ch, node);
        hi_u32 slot_id = HANDLE_2_ID(rch->ch_handle);
        hi_char alg[TSR2RCIPHER_STR_LEN] = {0};
        hi_char mode[TSR2RCIPHER_STR_LEN] = {0};
        hi_char iv_type[TSR2RCIPHER_STR_LEN] = {0};
        hi_char key_type[TSR2RCIPHER_STR_LEN] = {0};

        if (rch->alg == TSR2RCIPHER_ALG_AES_ECB) {
            if (strncpy_s(alg, sizeof(alg) - 1, "AES_ECB", strlen("AES_ECB"))) {
                HI_ERR_TSR2RCIPHER("strncpy_s failed!\n");
            }
        } else if (rch->alg == TSR2RCIPHER_ALG_AES_CBC) {
            if (strncpy_s(alg, sizeof(alg) - 1, "AES_CBC", strlen("AES_CBC"))) {
                HI_ERR_TSR2RCIPHER("strncpy_s failed!\n");
            }
        } else if (rch->alg == TSR2RCIPHER_ALG_AES_IPTV) {
            if (strncpy_s(alg, sizeof(alg) - 1, "AES_IPTV", strlen("AES_IPTV"))) {
                HI_ERR_TSR2RCIPHER("strncpy_s failed!\n");
            }
        } else if (rch->alg == TSR2RCIPHER_ALG_AES_CTR) {
            if (strncpy_s(alg, sizeof(alg) - 1, "AES_CTR", strlen("AES_CTR"))) {
                HI_ERR_TSR2RCIPHER("strncpy_s failed!\n");
            }
        } else if (rch->alg == TSR2RCIPHER_ALG_SMS4_ECB) {
            if (strncpy_s(alg, sizeof(alg) - 1, "SMS4_ECB", strlen("SMS4_ECB"))) {
                HI_ERR_TSR2RCIPHER("strncpy_s failed!\n");
            }
        } else if (rch->alg == TSR2RCIPHER_ALG_SMS4_CBC) {
            if (strncpy_s(alg, sizeof(alg) - 1, "SMS4_CBC", strlen("SMS4_CBC"))) {
                HI_ERR_TSR2RCIPHER("strncpy_s failed!\n");
            }
        } else if (rch->alg == TSR2RCIPHER_ALG_SMS4_IPTV) {
            if (strncpy_s(alg, sizeof(alg) - 1, "SMS4_IPTV", strlen("SMS4_IPTV"))) {
                HI_ERR_TSR2RCIPHER("strncpy_s failed!\n");
            }
        } else {
            if (strncpy_s(alg, sizeof(alg) - 1, "NU", strlen("NU"))) {
                HI_ERR_TSR2RCIPHER("strncpy_s failed!\n");
            }
        }

        if (rch->mode == TSR2RCIPHER_MODE_PAYLOAD) {
            if (strncpy_s(mode, sizeof(mode) - 1, "PL", strlen("PL"))) {
                HI_ERR_TSR2RCIPHER("strncpy_s failed!\n");
            }
        } else if (rch->mode == TSR2RCIPHER_MODE_RAW) {
            if (strncpy_s(mode, sizeof(mode) - 1, "Raw", strlen("Raw"))) {
                HI_ERR_TSR2RCIPHER("strncpy_s failed!\n");
            }
        } else {
            if (strncpy_s(mode, sizeof(mode) - 1, "NU", strlen("NU"))) {
                HI_ERR_TSR2RCIPHER("strncpy_s failed!\n");
            }
        }

        if (rch->is_odd_key == HI_TRUE) {
            if (strncpy_s(key_type, sizeof(key_type) - 1, "Odd", strlen("Odd"))) {
                HI_ERR_TSR2RCIPHER("strncpy_s failed!\n");
            }
        } else {
            if (strncpy_s(key_type, sizeof(key_type) - 1, "Even", strlen("Even"))) {
                HI_ERR_TSR2RCIPHER("strncpy_s failed!\n");
            }
        }

        if (rch->iv_type == TSR2RCIPHER_IV_EVEN) {
            if (strncpy_s(iv_type, sizeof(iv_type) - 1, "Even", strlen("Even"))) {
                HI_ERR_TSR2RCIPHER("strncpy_s failed!\n");
            }
        } else if (rch->iv_type == TSR2RCIPHER_IV_ODD) {
            if (strncpy_s(iv_type, sizeof(iv_type) - 1, "Odd", strlen("Odd"))) {
                HI_ERR_TSR2RCIPHER("strncpy_s failed!\n");
            }
        } else {
            if (strncpy_s(iv_type, sizeof(iv_type) - 1, "NU", strlen("NU"))) {
                HI_ERR_TSR2RCIPHER("strncpy_s failed!\n");
            }
        }

        osal_proc_print(seqfile, "  %-3d      %-4d %-9s  %-3s      %-4s      %-4s      %-2s\n",
            rch->base.id, slot_id, alg, mode, key_type, iv_type, (rch->is_crc_check == HI_TRUE) ? "*" : "--");
    }
    osal_mutex_unlock(&mgmt->ch_list_lock);

    osal_proc_print(seqfile, "-------------------------END--------------------------\n");

    return HI_SUCCESS;
}

static hi_void tsr2rcipher_add_proc(hi_void)
{
    osal_proc_entry *item = HI_NULL;

    item = osal_proc_add(HI_MOD_TSR2RCIPHER_NAME, strlen(HI_MOD_TSR2RCIPHER_NAME));
    if (item == HI_NULL) {
        HI_ERR_TSR2RCIPHER("add tsr2rcipher proc failed.\n");
    } else {
        item->read = tsr2rcipher_proc_read;
    }

    return;
}

static hi_void tsr2rcipher_remove_proc(hi_void)
{
    osal_proc_remove(HI_MOD_TSR2RCIPHER_NAME, strlen(HI_MOD_TSR2RCIPHER_NAME));

    return;
}
#endif

static inline hi_s32 __tsr2rcipher_mgmt_init_impl(struct tsr2rcipher_mgmt *mgmt)
{
    mgmt->ch_cnt = TSR2RCIPHER_CH_CNT;
    osal_mutex_init(&mgmt->ch_list_lock);
    osal_spin_lock_init(&mgmt->ch_list_lock2);
    INIT_LIST_HEAD(&mgmt->ch_head);
    bitmap_zero(mgmt->ch_bitmap, mgmt->ch_cnt);
    mgmt->rx_int     = HI_FALSE;
    mgmt->tx_int     = HI_FALSE;
    mgmt->cipher_int = HI_FALSE;

#ifdef TSR2RCIPHER_PROC_SUPPORT
    tsr2rcipher_add_proc();
#endif

    return HI_SUCCESS;
}

static inline hi_void __tsr2rcipher_mgmt_exit_impl(struct tsr2rcipher_mgmt *mgmt)
{
#ifdef TSR2RCIPHER_PROC_SUPPORT
    tsr2rcipher_remove_proc();
#endif

    osal_spin_lock_destory(&mgmt->ch_list_lock2);
    osal_mutex_destory(&mgmt->ch_list_lock);
}

static inline hi_s32 _tsr2rcipher_mgmt_init_impl(struct tsr2rcipher_mgmt *mgmt)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 i;

    TSR2RCIPHER_CHECK_CONDITION_RETURN_VALUE(osal_atomic_read(&mgmt->ref_count) != 0, HI_FAILURE);
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VALUE(mgmt->state != TSR2RCIPHER_MGMT_CLOSED, HI_FAILURE);

    ret = __tsr2rcipher_mgmt_init_impl(mgmt);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    mgmt->io_base = ioremap_nocache(TSR2RCIPHER_REGS_BASE, 0xC000);
    if (mgmt->io_base == HI_NULL) {
        HI_ERR_TSR2RCIPHER("ioremap failed!\n");
        goto out1;
    }

    /* init the tsr2rcipher clock */
    tsc_hal_init_hw(mgmt);

    /* enable mmu */
    tsc_hal_en_mmu(mgmt);

    /* disable allchn */
    for (i = 0; i < TSR2RCIPHER_CH_CNT; i++) {
        tsc_hal_dis_mode_ctl(mgmt, i);
    }

    /* these code need to be changed when merge back to trunk by kingble */
    if (osal_irq_request(TSR2RCIPHER_IRQ_NUM, tsr2rcipher_isr, HI_NULL, "int_tsc_ree", (hi_void *)mgmt) != 0) {
        HI_ERR_TSR2RCIPHER("request_irq irq(%d) failed.\n", TSR2RCIPHER_IRQ_NUM);
        ret = HI_FAILURE;
        goto out2;
    }

    /* enable the interrupt */
    mgmt->rx_int     = HI_TRUE;
    mgmt->tx_int     = HI_TRUE;
    mgmt->cipher_int = HI_FALSE;

    /* enable the total interrupt */
    tsc_hal_top_set_int(mgmt, mgmt->rx_int, mgmt->tx_int, mgmt->cipher_int);

    hi_drv_sys_set_irq_affinity(HI_ID_TSR2RCIPHER, TSR2RCIPHER_IRQ_NUM, "int_tsc_ree");

    /* create tsr2rcipher private workqueue */
    mgmt->tsr2rcipher_queue = create_workqueue("tsc_queue");
    if (mgmt->tsr2rcipher_queue == HI_NULL) {
        HI_ERR_TSR2RCIPHER("create workqueue failed!\n");
        goto out2;
    }

    /* exception monitor */
    mgmt->monitor = osal_kthread_create(tsr2rcipher_monitor, mgmt, "tsc_monitor", 0);
    if (mgmt->monitor == HI_NULL) {
        HI_ERR_TSR2RCIPHER("create kthread failed!\n");
        goto out2;
    }

    mgmt->state = TSR2RCIPHER_MGMT_OPENED;

    return HI_SUCCESS;

out2:
    tsc_hal_de_init_hw(mgmt);
    iounmap(mgmt->io_base);
out1:
    __tsr2rcipher_mgmt_exit_impl(mgmt);
out0:
    return ret;
}

static hi_s32 tsr2rcipher_mgmt_init_impl(struct tsr2rcipher_mgmt *mgmt)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state == TSR2RCIPHER_MGMT_CLOSED) {
        ret = _tsr2rcipher_mgmt_init_impl(mgmt);
        if (ret == HI_SUCCESS) {
            osal_atomic_inc_return(&mgmt->ref_count);
        }
    } else if (mgmt->state == TSR2RCIPHER_MGMT_OPENED) {
        if (osal_atomic_read(&mgmt->ref_count) == 0) {
            HI_ERR_TSR2RCIPHER("ref count is 0!\n");
            goto out;
        }

        osal_atomic_inc_return(&mgmt->ref_count);

        ret = HI_SUCCESS;
    } else {
        HI_ERR_TSR2RCIPHER("mgmt state is invalid!\n");
    }

out:
    osal_mutex_unlock(&mgmt->lock);

    return ret;
}

static inline hi_s32 _tsr2rcipher_mgmt_exit_impl(struct tsr2rcipher_mgmt *mgmt)
{
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VALUE(osal_atomic_read(&mgmt->ref_count) != 1, HI_FAILURE);
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VALUE(mgmt->state != TSR2RCIPHER_MGMT_OPENED, HI_FAILURE);

    mgmt->state = TSR2RCIPHER_MGMT_CLOSED;

    /* disable all the interrupt */
    tsc_hal_top_set_int(mgmt, HI_FALSE, HI_FALSE, HI_FALSE);

    /* destroy tsr2rcipher private workqueue */
    if (mgmt->tsr2rcipher_queue) {
        destroy_workqueue(mgmt->tsr2rcipher_queue);
    }

    /* stop monit thread */
    osal_kthread_destroy(mgmt->monitor, HI_TRUE);

    /* free the irq */
    osal_irq_free(TSR2RCIPHER_IRQ_NUM, (hi_void *)mgmt);

    /* disable mmu */
    tsc_hal_dis_mmu(mgmt);

    /* deinit hardware clock */
    tsc_hal_de_init_hw(mgmt);

    /* unmap the base register address */
    iounmap(mgmt->io_base);

    __tsr2rcipher_mgmt_exit_impl(mgmt);

    return HI_SUCCESS;
}

static inline hi_s32 tsr2rcipher_mgmt_rel_and_exit(struct tsr2rcipher_mgmt *mgmt)
{
    if (list_empty(&mgmt->ch_head)) { /*FIXME: add more checking */
        return _tsr2rcipher_mgmt_exit_impl(mgmt);
    } else {
        HI_ERR_TSR2RCIPHER("attention: some list may not empty!\n");
    }

    return HI_FAILURE;
}

static hi_s32 tsr2rcipher_mgmt_exit_impl(struct tsr2rcipher_mgmt *mgmt)
{
    hi_s32 ret = HI_FAILURE;
    unsigned long start = jiffies, end = start + HZ; /* 1s */

    do {
        osal_mutex_lock(&mgmt->lock);

        if (mgmt->state == TSR2RCIPHER_MGMT_OPENED) {
            if (osal_atomic_read(&mgmt->ref_count) == 0) {
                HI_ERR_TSR2RCIPHER("ref count fatal error!\n");
                osal_mutex_unlock(&mgmt->lock);
                return HI_FAILURE;
            }

            if (osal_atomic_read(&mgmt->ref_count) == 1) {
                ret = tsr2rcipher_mgmt_rel_and_exit(mgmt);
                if (ret == HI_SUCCESS) {
                    osal_atomic_dec_return(&mgmt->ref_count);
                }
            } else {
                osal_atomic_dec_return(&mgmt->ref_count);
                ret = HI_SUCCESS;
            }
        } else if (mgmt->state == TSR2RCIPHER_MGMT_CLOSED) {
            if (osal_atomic_read(&mgmt->ref_count)) {
                HI_ERR_TSR2RCIPHER("ref count fatal error!\n");
                osal_mutex_unlock(&mgmt->lock);
                return HI_FAILURE;
            }
            ret = HI_SUCCESS;
        } else {
            HI_ERR_TSR2RCIPHER("mgmt state fatal error!\n");
            osal_mutex_unlock(&mgmt->lock);
            return HI_FAILURE;
        }

        osal_mutex_unlock(&mgmt->lock);

        osal_msleep(10); /* 10ms polling */
    } while (ret != HI_SUCCESS && time_in_range(jiffies, start, end));

    return ret;
}

static hi_s32 tsr2rcipher_mgmt_get_cap_impl(struct tsr2rcipher_mgmt *mgmt, tsr2rcipher_capability *cap)
{
    cap->ts_chan_cnt = mgmt->ch_cnt;
    return HI_SUCCESS;
}

static hi_void __tsr2rcipher_mgmt_suspend_ch_impl(struct tsr2rcipher_mgmt *mgmt)
{
    hi_s32 ret;
    struct list_head *node = HI_NULL;

    osal_mutex_lock(&mgmt->ch_list_lock);
    list_for_each(node, &mgmt->ch_head) {
        struct tsr2rcipher_r_ch *rch = list_entry(node, struct tsr2rcipher_r_ch, node);
        ret = rch->ops->suspend(rch);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSR2RCIPHER("suspend rch(%d) failed.\n", rch->base.id);
        }
    }
    osal_mutex_unlock(&mgmt->ch_list_lock);
}

static hi_void __tsr2rcipher_mgmt_suspend_reg_impl(struct tsr2rcipher_mgmt *mgmt)
{
    /* disable all the interrupt */
    tsc_hal_top_set_int(mgmt, HI_FALSE, HI_FALSE, HI_FALSE);

    /* disable mmu */
    tsc_hal_dis_mmu(mgmt);

    /* deinit hardware */
    tsc_hal_de_init_hw(mgmt);
}

static hi_void _tsr2rcipher_mgmt_suspend_impl(struct tsr2rcipher_mgmt *mgmt)
{
    /* channel suspend */
    __tsr2rcipher_mgmt_suspend_ch_impl(mgmt);

    /* general register suspend */
    __tsr2rcipher_mgmt_suspend_reg_impl(mgmt);
}

static hi_s32 tsr2rcipher_mgmt_suspend_impl(struct tsr2rcipher_mgmt *mgmt)
{
    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != TSR2RCIPHER_MGMT_OPENED) {
        HI_WARN_TSR2RCIPHER("mgmt has not opened.\n");
        goto out;
    }

    osal_atomic_inc_return(&mgmt->ref_count);
    _tsr2rcipher_mgmt_suspend_impl(mgmt);
    osal_atomic_dec_return(&mgmt->ref_count);

out:
    osal_mutex_unlock(&mgmt->lock);

    return HI_SUCCESS;
}

static hi_void __tsr2rcipher_mgmt_resume_reg_impl(struct tsr2rcipher_mgmt *mgmt)
{
    hi_u32 i;

    /* init hardware */
    tsc_hal_init_hw(mgmt);

    /* enable mmu */
    tsc_hal_en_mmu(mgmt);

    /* disable allchn */
    for (i = 0; i < TSR2RCIPHER_CH_CNT; i++) {
        tsc_hal_dis_mode_ctl(mgmt, i);
    }

    /* enable all the interrupt */
    tsc_hal_top_set_int(mgmt, mgmt->rx_int, mgmt->tx_int, mgmt->cipher_int);
}

static hi_void __tsr2rcipher_mgmt_resume_ch_impl(struct tsr2rcipher_mgmt *mgmt)
{
    hi_s32 ret;
    struct list_head *node = HI_NULL;

    osal_mutex_lock(&mgmt->ch_list_lock);
    list_for_each(node, &mgmt->ch_head) {
        struct tsr2rcipher_r_ch *rch = list_entry(node, struct tsr2rcipher_r_ch, node);
        ret = rch->ops->resume(rch);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSR2RCIPHER("resume rch(%d) failed.\n", rch->base.id);
        }
    }
    osal_mutex_unlock(&mgmt->ch_list_lock);
}

static hi_void _tsr2rcipher_mgmt_resume_impl(struct tsr2rcipher_mgmt *mgmt)
{
    /* general register resume */
    __tsr2rcipher_mgmt_resume_reg_impl(mgmt);

    /* channel resume */
    __tsr2rcipher_mgmt_resume_ch_impl(mgmt);
}

static hi_s32 tsr2rcipher_mgmt_resume_impl(struct tsr2rcipher_mgmt *mgmt)
{
    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != TSR2RCIPHER_MGMT_OPENED) {
        HI_WARN_TSR2RCIPHER("mgmt has not opened.\n");
        goto out;
    }

    osal_atomic_inc_return(&mgmt->ref_count);
    _tsr2rcipher_mgmt_resume_impl(mgmt);
    osal_atomic_dec_return(&mgmt->ref_count);

out:
    osal_mutex_unlock(&mgmt->lock);

    return HI_SUCCESS;
}

static inline hi_s32 _tsr2rcipher_mgmt_create_ch_impl(struct tsr2rcipher_mgmt *mgmt, const tsr2rcipher_attr *attr,
                                                    struct tsr2rcipher_r_ch **rch)
{
    hi_s32 ret;
    hi_u32 id;
    hi_u32 type = 0;
    unsigned long lock_flag;
    enum tsc_core_type core_type;
    struct tsr2rcipher_r_ch *new_rch = HI_NULL;

    id = find_first_zero_bit(mgmt->ch_bitmap, mgmt->ch_cnt);
    if (!(id < mgmt->ch_cnt)) {
        HI_ERR_TSR2RCIPHER("there is no available ch now!\n");
        ret = HI_ERR_TSR2RCIPHER_NO_RESOURCE;
        goto out0;
    }

    new_rch = HI_KZALLOC(HI_ID_TSR2RCIPHER, sizeof(struct tsr2rcipher_r_ch), GFP_KERNEL);
    if (new_rch == HI_NULL) {
        ret = HI_ERR_TSR2RCIPHER_ALLOC_MEM_FAILED;
        goto out0;
    }

    osal_atomic_init(&new_rch->base.ref_count);
    osal_atomic_set(&new_rch->base.ref_count, 1);
    new_rch->base.ops     = get_tsr2rcipher_rbase_ops();
    new_rch->base.release = tsr2rcipher_mgmt_destroy_ch;
    new_rch->base.id      = id;
    new_rch->base.mgmt    = mgmt;

    osal_mutex_init(&new_rch->lock);
    new_rch->ops = &g_tsr2rcipher_ch_ops;

    new_rch->staled = HI_FALSE;

    /* ch attris */
    new_rch->alg          = attr->alg;
    new_rch->mode         = attr->mode;
    new_rch->is_crc_check = attr->is_crc_check;
    new_rch->is_create_ks = attr->is_create_ks;
    new_rch->is_odd_key   = attr->is_odd_key;

    ret = tsr2rcipher_get_key_id_and_core_type(new_rch->alg, &type, &core_type);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSR2RCIPHER("get key id and core type failed!\n");
        goto out1;
    }

    new_rch->alg_code  = type;
    new_rch->core_type = core_type;

    if (new_rch->is_create_ks == HI_TRUE) {
        hi_u32 ks_handle;
        ks_export_func *ks_ops = HI_NULL;

        ret = osal_exportfunc_get(HI_ID_KEYSLOT, (hi_void **)&ks_ops);
        if (ret != HI_SUCCESS || ks_ops == HI_NULL) {
            HI_ERR_TSR2RCIPHER("get key slot callback failed!\n");
            goto out1;
        }

        ret = ks_ops->ext_ks_create(HI_KEYSLOT_TYPE_TSCIPHER, &ks_handle);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSR2RCIPHER("create key slot failed!\n");
            goto out1;
        }
        new_rch->ks_handle = ks_handle;
    } else {
        new_rch->ks_handle = HI_INVALID_HANDLE;
    }

    /* wait queue */
    init_waitqueue_head(&new_rch->rx_wait_queue);
    new_rch->rx_wait_dsc_rd = 0;
    new_rch->rx_wait_pkt_cnt = 0;

    init_waitqueue_head(&new_rch->tx_wait_queue);
    new_rch->tx_wait_dsc_rd = 0;
    new_rch->tx_wait_pkt_cnt = 0;
    new_rch->tx_wait_afull = 0;

    osal_spin_lock_irqsave(&mgmt->ch_list_lock2, &lock_flag);
    list_add_tail(&new_rch->node, &mgmt->ch_head);
    osal_spin_unlock_irqrestore(&mgmt->ch_list_lock2, &lock_flag);

    *rch = new_rch;

    HI_DBG_TSR2RCIPHER("#    robj(0x%x) created.\n", new_rch);

    set_bit(id, mgmt->ch_bitmap);

    return HI_SUCCESS;

out1:
    HI_KFREE(HI_ID_TSR2RCIPHER, new_rch);
out0:
    return ret;
}

static hi_s32 tsr2rcipher_mgmt_create_ch_impl(struct tsr2rcipher_mgmt *mgmt, const tsr2rcipher_attr *attr,
                                            struct tsr2rcipher_r_ch **rch)
{
    hi_s32 ret = HI_FAILURE;

    TSR2RCIPHER_DBG_ENTER();
    TSR2RCIPHER_CHECK_POINTER_RETURN(mgmt);
    TSR2RCIPHER_CHECK_POINTER_RETURN(attr);
    TSR2RCIPHER_CHECK_POINTER_RETURN(rch);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != TSR2RCIPHER_MGMT_OPENED) {
        HI_ERR_TSR2RCIPHER("mgmt has not opened.\n");
        goto out;
    }

    osal_mutex_lock(&mgmt->ch_list_lock);
    ret = _tsr2rcipher_mgmt_create_ch_impl(mgmt, attr, rch);
    osal_mutex_unlock(&mgmt->ch_list_lock);

out:
    osal_mutex_unlock(&mgmt->lock);

    TSR2RCIPHER_DBG_EXIT();
    return ret;
}

static inline hi_s32 _tsr2rcipher_mgmt_destroy_ch_impl(struct tsr2rcipher_mgmt *mgmt, struct tsr2rcipher_r_ch *rch)
{
    hi_s32 ret = HI_FAILURE;
    unsigned long mask;
    unsigned long *p = HI_NULL;
    ks_export_func *ks_ops = HI_NULL;
    unsigned long lock_flag;

    mask = BIT_MASK(rch->base.id);
    p = ((unsigned long *)mgmt->ch_bitmap) + BIT_WORD(rch->base.id);
    if (!(*p & mask)) {
        HI_ERR_TSR2RCIPHER("rch(%d) is invalid.\n", rch->base.id);
        ret = HI_ERR_TSR2RCIPHER_INVALID_PARA;
        goto out;
    }

    if (rch->is_create_ks == HI_TRUE && rch->ks_handle != HI_INVALID_HANDLE) {
        ret = osal_exportfunc_get(HI_ID_KEYSLOT, (hi_void **)&ks_ops);
        if (ret != HI_SUCCESS || ks_ops == HI_NULL) {
            HI_ERR_TSR2RCIPHER("get key slot callback failed!\n");
        } else {
            ret = ks_ops->ext_ks_destory(HI_KEYSLOT_TYPE_TSCIPHER, rch->ks_handle);
            if (ret != HI_SUCCESS) {
                HI_ERR_TSR2RCIPHER("destroy key slot failed!\n");
            }
        }
    }

    clear_bit(rch->base.id, mgmt->ch_bitmap);
    osal_mutex_destory(&rch->lock);
    osal_atomic_destory(&rch->base.ref_count);

    osal_spin_lock_irqsave(&mgmt->ch_list_lock2, &lock_flag);
    list_del(&rch->node);
    osal_spin_unlock_irqrestore(&mgmt->ch_list_lock2, &lock_flag);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 tsr2rcipher_mgmt_destroy_ch_impl(struct tsr2rcipher_mgmt *mgmt, struct tsr2rcipher_r_ch *rch)
{
    hi_s32 ret = HI_FAILURE;

    TSR2RCIPHER_DBG_ENTER();
    TSR2RCIPHER_CHECK_POINTER_RETURN(mgmt);
    TSR2RCIPHER_CHECK_POINTER_RETURN(rch);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != TSR2RCIPHER_MGMT_OPENED) {
        HI_ERR_TSR2RCIPHER("mgmt has not opened.\n");
        goto out;
    }

    osal_mutex_lock(&mgmt->ch_list_lock);
    ret = _tsr2rcipher_mgmt_destroy_ch_impl(mgmt, rch);
    if (ret == HI_SUCCESS) {
        HI_KFREE(HI_ID_TSR2RCIPHER, rch);
    }
    osal_mutex_unlock(&mgmt->ch_list_lock);

out:
    osal_mutex_unlock(&mgmt->lock);

    TSR2RCIPHER_DBG_EXIT();
    return ret;
}

static hi_void tsr2rcipher_mgmt_show_info_impl(struct tsr2rcipher_mgmt *mgmt)
{
    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != TSR2RCIPHER_MGMT_OPENED) {
        HI_ERR_TSR2RCIPHER("mgmt has not opened.\n");
        goto out;
    }

    osal_mutex_lock(&mgmt->ch_list_lock);
    if (!list_empty(&mgmt->ch_head)) {
        HI_ERR_TSR2RCIPHER("ch_head is not empty.\n");
    }
    osal_mutex_unlock(&mgmt->ch_list_lock);

out:
    osal_mutex_unlock(&mgmt->lock);
}

static struct tsr2rcipher_mgmt_ops g_tsr2rcipher_mgmt_ops = {
    .init       = tsr2rcipher_mgmt_init_impl,
    .exit       = tsr2rcipher_mgmt_exit_impl,

    .get_cap    = tsr2rcipher_mgmt_get_cap_impl,

    .create_ch  = tsr2rcipher_mgmt_create_ch_impl,
    .destroy_ch = tsr2rcipher_mgmt_destroy_ch_impl,

    .suspend    = tsr2rcipher_mgmt_suspend_impl,
    .resume     = tsr2rcipher_mgmt_resume_impl,

    .show_info  = tsr2rcipher_mgmt_show_info_impl,
};

static struct tsr2rcipher_mgmt g_tsr2rcipher_mgmt = {
    .state     = TSR2RCIPHER_MGMT_CLOSED,
    .ops       = &g_tsr2rcipher_mgmt_ops,
};

struct tsr2rcipher_mgmt *_tsr2rcipher_get_mgmt(hi_void)
{
    return &g_tsr2rcipher_mgmt;
}

struct tsr2rcipher_mgmt *tsr2rcipher_get_mgmt(hi_void)
{
    struct tsr2rcipher_mgmt *mgmt = _tsr2rcipher_get_mgmt();

    if (osal_atomic_read(&mgmt->ref_count) == 0) {
        HI_FATAL_TSR2RCIPHER("ref count fatal error!\n");
        return HI_NULL;
    }

    return mgmt;
}

