/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux basic function impl.
 * Author: sdk
 * Create: 2017-12-04
 */
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
#include "drv_demux_rmxfct.h"
#include "drv_demux_func.h"
#include "drv_demux_define.h"
#include "drv_demux_utils.h"

/* obj is staled after Close, it should discard all possible call request after that. */
#define DMX_R_RMX_PUMP_GET(handle, rrmx_pump) ({ \
    hi_s32 ret = HI_FAILURE; \
    ret = dmx_r_get(handle, (struct dmx_r_base **)&rrmx_pump); \
    if (ret == HI_SUCCESS) { \
        if (&g_dmx_rmx_pump_ops != rrmx_pump->ops) { \
            dmx_r_put((struct dmx_r_base*)rrmx_pump);\
            HI_ERR_DEMUX("handle is in active, but not a valid rrmx_pump handle(0x%x).\n", handle); \
            ret = HI_ERR_DMX_INVALID_PARA; \
        } \
    } \
    ret;\
})

static hi_s32 rmx_check_attrs(const struct dmx_mgmt *mgmt, const dmx_rmx_pump_attrs *attrs)
{
    if (attrs->pump_type >= HI_MPI_RMX_PUMP_TYPE_MAX) {
        HI_ERR_DEMUX("invalid pump type(%u).\n", attrs->pump_type);
        return HI_ERR_DMX_INVALID_PARA;
    }

    if (!(attrs->in_port_id >= DMX_IF_PORT_0 && attrs->in_port_id < DMX_IF_PORT_0 + mgmt->if_port_cnt) &&
        !(attrs->in_port_id >= DMX_TSI_PORT_0 && attrs->in_port_id < DMX_TSI_PORT_0 + mgmt->tsi_port_cnt) &&
        !(attrs->in_port_id >= DMX_RAM_PORT_0 && attrs->in_port_id < DMX_RAM_PORT_0 + mgmt->ram_port_cnt)) {
        HI_ERR_DEMUX("invalid port id(%d).\n", attrs->in_port_id);
        return HI_ERR_DMX_INVALID_PARA;
    }

    if (attrs->pump_type == DMX_RMX_PUMP_TYPE_PID && attrs->pid >= DMX_INVALID_PID) {
        HI_ERR_DEMUX("invalid pid(%d).\n", attrs->pid);
        return HI_ERR_DMX_INVALID_PARA;
    }

    if (attrs->pump_type == DMX_RMX_PUMP_TYPE_REMAP_PID &&
        (attrs->pid >= DMX_INVALID_PID || attrs->remap_pid >= DMX_INVALID_PID)) {
        HI_ERR_DEMUX("invalid pid(%d) or remap pid(%d).\n", attrs->pid, attrs->remap_pid);
        return HI_ERR_DMX_INVALID_PARA;
    }

    return HI_SUCCESS;
}

hi_s32 dmx_mgmt_create_rmx_pump(const dmx_rmx_pump_attrs *attrs, struct dmx_r_rmx_pump **rrmx_pump)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    if (attrs == HI_NULL) {
        HI_ERR_DEMUX("invalid attrs(%#x).\n", attrs);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = rmx_check_attrs(mgmt, attrs);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid attrs.\n");
        goto out;
    }

    ret = mgmt->ops->create_rmx_pump(mgmt, attrs, rrmx_pump);
out:
    return ret;
}

hi_s32 dmx_mgmt_destroy_rmx_pump(struct dmx_r_base *obj)
{
    hi_s32 ret;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    ret = mgmt->ops->destroy_rmx_pump(mgmt, (struct dmx_r_rmx_pump *)obj);

    return ret;
}

hi_s32 dmx_rmx_pump_get_attrs(hi_handle rmx_pump_handle, dmx_rmx_pump_attrs *pump_attrs)
{
    hi_s32 ret;
    struct dmx_r_rmx_pump  *rrmx_pump  = HI_NULL;

    ret = DMX_R_RMX_PUMP_GET(rmx_pump_handle, rrmx_pump);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rrmx_pump->ops->get_attrs(rrmx_pump, pump_attrs);

    dmx_r_put((struct dmx_r_base *)rrmx_pump);
out:
    return ret;
}

hi_s32 dmx_rmx_pump_set_attrs(hi_handle rmx_pump_handle, const dmx_rmx_pump_attrs *pump_attrs)
{
    hi_s32 ret;
    struct dmx_r_rmx_pump *rrmx_pump = HI_NULL;

    ret = DMX_R_RMX_PUMP_GET(rmx_pump_handle, rrmx_pump);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rrmx_pump->ops->set_attrs(rrmx_pump, pump_attrs);

    dmx_r_put((struct dmx_r_base *)rrmx_pump);
out:
    return ret;
}

/*************************dmx_r_rmx_pump_xxx_impl*************************************************/
static inline hi_bool rmx_pump_staled(struct dmx_r_rmx_pump *rrmx_pump)
{
    if (unlikely(rrmx_pump->staled == HI_TRUE)) {
        HI_ERR_DEMUX("rrmx_pump is staled.\n");
        return HI_TRUE;
    }

    return HI_FALSE;
}

static hi_s32 _dmx_r_rmx_pump_create_impl(struct dmx_r_rmx_pump *rrmx_pump, const dmx_rmx_pump_attrs *attrs)
{
    hi_u32 ret;
    struct dmx_mgmt   *mgmt = rrmx_pump->base.mgmt;

    DMX_NULL_POINTER_RETURN(mgmt);

    ret = HI_SUCCESS;

    return ret;
}

static hi_s32 dmx_r_rmx_pump_create_impl(struct dmx_r_rmx_pump *rrmx_pump, const dmx_rmx_pump_attrs *attrs)
{
    hi_s32 ret;

    osal_mutex_lock(&rrmx_pump->lock);

    WARN_ON(rrmx_pump->staled == HI_TRUE);

    ret = _dmx_r_rmx_pump_create_impl(rrmx_pump, attrs);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rrmx_pump->lock);

    return ret;
}

static hi_s32 _dmx_r_rmx_pump_get_attrs_impl(struct dmx_r_rmx_pump *rrmx_pump, dmx_rmx_pump_attrs *attrs)
{
    attrs->in_port_id   = rrmx_pump->in_port_id;
    attrs->pid        = rrmx_pump->pid;
    attrs->pump_type   = rrmx_pump->pump_type;
    attrs->remap_pid   = rrmx_pump->remap_pid;

    return HI_SUCCESS;
}

static hi_s32 dmx_r_rmx_pump_get_attrs_impl(struct dmx_r_rmx_pump *rrmx_pump, dmx_rmx_pump_attrs *attrs)
{
    hi_s32 ret;

    osal_mutex_lock(&rrmx_pump->lock);

    WARN_ON(rrmx_pump->staled == HI_TRUE);

    ret = _dmx_r_rmx_pump_get_attrs_impl(rrmx_pump, attrs);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rrmx_pump->lock);

    return ret;
}

static hi_s32 _dmx_r_rmx_pump_set_attrs_impl(struct dmx_r_rmx_pump *rrmx_pump, const dmx_rmx_pump_attrs *attrs)
{
    hi_s32 ret;

    rrmx_pump->in_port_id = attrs->in_port_id;
    rrmx_pump->pid      = attrs->pid;
    rrmx_pump->pump_type = attrs->pump_type;
    rrmx_pump->remap_pid = attrs->remap_pid;

    /* todo something about hardware */
    ret = HI_SUCCESS;

    return ret;
}

static hi_s32 dmx_r_rmx_pump_set_attrs_impl(struct dmx_r_rmx_pump *rrmx_pump, const dmx_rmx_pump_attrs *attrs)
{
    hi_s32 ret;

    osal_mutex_lock(&rrmx_pump->lock);

    WARN_ON(rrmx_pump->staled == HI_TRUE);

    ret = _dmx_r_rmx_pump_set_attrs_impl(rrmx_pump, attrs);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rrmx_pump->lock);

    return ret;
}

static hi_s32 _dmx_r_rmx_pump_destroy_impl(struct dmx_r_rmx_pump *rrmx_pump)
{
    hi_u32 ret;

    struct dmx_mgmt   *mgmt = rrmx_pump->base.mgmt;

    DMX_NULL_POINTER_RETURN(mgmt);

    ret = HI_SUCCESS;

    return ret;
}

static hi_s32 dmx_r_rmx_pump_destroy_impl(struct dmx_r_rmx_pump *rrmx_pump)
{
    hi_s32 ret;

    osal_mutex_lock(&rrmx_pump->lock);

    WARN_ON(rrmx_pump->staled == HI_TRUE);

    ret = _dmx_r_rmx_pump_destroy_impl(rrmx_pump);
    if (ret == HI_SUCCESS) {
        rrmx_pump->staled = HI_TRUE;
    }

    osal_mutex_unlock(&rrmx_pump->lock);

    return ret;
}

static hi_s32 dmx_r_rmx_pump_suspend_impl(struct dmx_r_rmx_pump *rrmx_pump)
{
    osal_mutex_lock(&rrmx_pump->lock);

    WARN_ON(rrmx_pump->staled == HI_TRUE);

    osal_mutex_unlock(&rrmx_pump->lock);

    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 dmx_r_rmx_pump_resume_impl(struct dmx_r_rmx_pump *rrmx_pump)
{
    osal_mutex_lock(&rrmx_pump->lock);

    WARN_ON(rrmx_pump->staled == HI_TRUE);

    osal_mutex_unlock(&rrmx_pump->lock);

    return HI_ERR_DMX_NOT_SUPPORT;
}

struct dmx_r_rmx_pump_ops g_dmx_rmx_pump_ops = {
    .create         = dmx_r_rmx_pump_create_impl,
    .get_attrs      = dmx_r_rmx_pump_get_attrs_impl,
    .set_attrs      = dmx_r_rmx_pump_set_attrs_impl,
    .destroy        = dmx_r_rmx_pump_destroy_impl,

    .suspend        = dmx_r_rmx_pump_suspend_impl,
    .resume         = dmx_r_rmx_pump_resume_impl,
};

/* rmx_fct_create&destroy */
/* declaration the function define below */
static hi_s32 rmx_fct_destroy_pump(hi_handle rmx_pump_handle);

static hi_s32 rmx_fct_create_pump(struct dmx_r_rmx_fct *rrmx_fct, const dmx_rmx_pump_attrs *attrs,
                                  hi_handle *rmx_pump_handle, struct dmx_session *session)
{
    hi_s32 ret = HI_FAILURE;
    hi_s32 pump_id;
    struct dmx_slot  *slot = HI_NULL;
    struct dmx_r_rmx_pump *new_rrmx_pump = HI_NULL;

    osal_mutex_lock(&rrmx_fct->pump_bitmap_lock);

    pump_id = find_first_zero_bit(rrmx_fct->pump_bitmap, rrmx_fct->pump_cnt);
    if (!(pump_id < rrmx_fct->pump_cnt)) {
        HI_ERR_DEMUX("there is no available pump now!\n");
        ret = HI_ERR_DMX_NO_RESOURCE;
        goto out0;
    }

    ret = dmx_mgmt_create_rmx_pump(attrs, &new_rrmx_pump);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    new_rrmx_pump->base.id = pump_id;

    /* get the rmx_pump obj */
    dmx_r_get_raw((struct dmx_r_base *)new_rrmx_pump);

    ret = new_rrmx_pump->ops->create(new_rrmx_pump, attrs);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    /* associate a slot to the object  */
    ret = dmx_slot_create((struct dmx_r_base *)new_rrmx_pump, &slot);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    /* associate an slot to the session. */
    slot->release = rmx_fct_destroy_pump;
    ret = dmx_session_add_slot(session, slot);
    if (ret != HI_SUCCESS) {
        goto out3;
    }

    new_rrmx_pump->rmx_pump_handle = slot->handle;
    new_rrmx_pump->rrmx_fct       = rrmx_fct;

    /* set the rmxpump bitmap */
    set_bit(pump_id, rrmx_fct->pump_bitmap);
    osal_mutex_unlock(&rrmx_fct->pump_bitmap_lock);

    *rmx_pump_handle = slot->handle;

    /* release the rmx_pump obj */
    dmx_r_put((struct dmx_r_base *)new_rrmx_pump);
    return HI_SUCCESS;

out3:
    dmx_slot_destroy(slot);
out2:
    new_rrmx_pump->ops->destroy(new_rrmx_pump);
out1:
    /* the first time put rmx_pum object, and second time destroy */
    dmx_r_put((struct dmx_r_base *)new_rrmx_pump);

    dmx_r_put((struct dmx_r_base *)new_rrmx_pump);
out0:
    osal_mutex_unlock(&rrmx_fct->pump_bitmap_lock);
    return ret;
}

static hi_s32 rmx_fct_destroy_pump(hi_handle rmx_pump_handle)
{
    hi_s32 ret;
    unsigned long mask = 0;
    unsigned long *p = HI_NULL;

    struct dmx_slot *slot = HI_NULL;
    struct dmx_r_rmx_pump *rrmx_pump = HI_NULL;

    ret = DMX_R_RMX_PUMP_GET(rmx_pump_handle, rrmx_pump);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    DMX_NULL_POINTER_GOTO(rrmx_pump->rrmx_fct, out1);

    ret = dmx_slot_find(rmx_pump_handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    dmx_session_del_slot(slot->session, slot);

    dmx_slot_destroy(slot);

    osal_mutex_lock(&rrmx_pump->rrmx_fct->pump_bitmap_lock);

    mask = BIT_MASK(rrmx_pump->base.id);
    p = ((unsigned long *)rrmx_pump->rrmx_fct->pump_bitmap) + BIT_WORD(rrmx_pump->base.id);
    if (!(*p & mask)) {
        HI_ERR_DEMUX("rrmx_pump(%d) is invalid.\n", rrmx_pump->base.id);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out3;
    }

    /* clear the rmxpump bitmap */
    clear_bit(rrmx_pump->base.id, rrmx_pump->rrmx_fct->pump_bitmap);

    /* set the pumphandle invalid after destroy it */
    rrmx_pump->rmx_pump_handle = HI_INVALID_HANDLE;

    ret = rrmx_pump->ops->destroy(rrmx_pump);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("destroy rmx_pump failed!\n");
        goto out3;
    }

out3:
    osal_mutex_unlock(&rrmx_pump->rrmx_fct->pump_bitmap_lock);
out2:
    /* the first time put, and the second time destroy */
    dmx_r_put((struct dmx_r_base *)rrmx_pump);
out1:
    dmx_r_put((struct dmx_r_base *)rrmx_pump);
out0:
    return ret;
}


/*
 * DEMUX RmxFct functions.
 */

/*
 * obj is staled after Close, it should discard all possible call request after that.
 */

#define DMX_R_RMX_FCT_GET(handle, rrmx_fct) ({ \
    hi_s32 ret = HI_FAILURE; \
    ret = dmx_r_get(handle, (struct dmx_r_base **)&rrmx_fct); \
    if (ret == HI_SUCCESS) { \
        if (&g_dmx_rmx_fct_ops != rrmx_fct->ops) { \
            dmx_r_put((struct dmx_r_base*)rrmx_fct);\
            HI_ERR_DEMUX("handle is in active, but not a valid rmx_fct handle(0x%x).\n", handle); \
            ret = HI_ERR_DMX_INVALID_PARA; \
        } \
    } \
    ret;\
})

hi_s32 dmx_mgmt_create_rmx_fct(const dmx_rmx_attrs *attrs, struct dmx_r_rmx_fct **rrmx_fct)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    if (!attrs) {
        HI_ERR_DEMUX("invalid attrs(%#x).\n", attrs);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = mgmt->ops->create_rmx_fct(mgmt, attrs, rrmx_fct);
out:
    return ret;
}

hi_s32 dmx_mgmt_destroy_rmx_fct(struct dmx_r_base *obj)
{
    hi_s32 ret;

    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    ret = mgmt->ops->destroy_rmx_fct(mgmt, (struct dmx_r_rmx_fct *)obj);

    return ret;
}

/*************************dmx_mgmt_rmx_fct*************************************************/
hi_s32 dmx_rmx_fct_create(const dmx_rmx_attrs *attrs, hi_handle *rmx_handle, struct dmx_session *session)
{
    hi_s32 ret;
    struct dmx_slot  *slot = HI_NULL;
    struct dmx_r_rmx_fct *rrmx_fct = HI_NULL;

    ret = dmx_mgmt_create_rmx_fct(attrs, &rrmx_fct);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    /* get the rmx_fct obj */
    dmx_r_get_raw((struct dmx_r_base *)rrmx_fct);

    ret = rrmx_fct->ops->create(rrmx_fct, attrs);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    /* associate a slot to the object  */
    ret = dmx_slot_create((struct dmx_r_base *)rrmx_fct, &slot);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    /* associate an slot to the session. */
    slot->release = dmx_rmx_fct_destroy;
    ret = dmx_session_add_slot(session, slot);
    if (ret != HI_SUCCESS) {
        goto out3;
    }

    *rmx_handle = slot->handle;
    rrmx_fct->rmx_fct_handle = slot->handle;

    /* release the rec_fct obj */
    dmx_r_put((struct dmx_r_base *)rrmx_fct);

    return HI_SUCCESS;

out3:
    dmx_slot_destroy(slot);
out2:
    rrmx_fct->ops->destroy(rrmx_fct);
out1:
    dmx_r_put((struct dmx_r_base *)rrmx_fct);

    dmx_r_put((struct dmx_r_base *)rrmx_fct);
out0:
    return ret;
}

hi_s32 dmx_rmx_fct_open(hi_handle rmx_handle)
{
    hi_s32 ret;
    struct dmx_r_rmx_fct *rrmx_fct = HI_NULL;

    ret = DMX_R_RMX_FCT_GET(rmx_handle, rrmx_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rrmx_fct->ops->open(rrmx_fct);

    dmx_r_put((struct dmx_r_base *)rrmx_fct);
out:
    return ret;
}

hi_s32 dmx_rmx_fct_get_attrs(hi_handle rmx_handle, dmx_rmx_attrs *attrs)
{
    hi_s32 ret;
    struct dmx_r_rmx_fct *rrmx_fct = HI_NULL;

    ret = DMX_R_RMX_FCT_GET(rmx_handle, rrmx_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rrmx_fct->ops->get_attrs(rrmx_fct, attrs);

    dmx_r_put((struct dmx_r_base *)rrmx_fct);
out:
    return ret;
}

hi_s32 dmx_rmx_fct_set_attrs(hi_handle rmx_handle, const dmx_rmx_attrs *attrs)
{
    hi_s32 ret;
    struct dmx_r_rmx_fct *rrmx_fct = HI_NULL;

    ret = DMX_R_RMX_FCT_GET(rmx_handle, rrmx_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rrmx_fct->ops->set_attrs(rrmx_fct, attrs);

    dmx_r_put((struct dmx_r_base *)rrmx_fct);
out:
    return ret;
}

hi_s32 dmx_rmx_fct_get_status(hi_handle rmx_handle, dmx_rmx_status *status)
{
    hi_s32 ret;
    struct dmx_r_rmx_fct *rrmx_fct = HI_NULL;

    ret = DMX_R_RMX_FCT_GET(rmx_handle, rrmx_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rrmx_fct->ops->get_status(rrmx_fct, status);

    dmx_r_put((struct dmx_r_base *)rrmx_fct);
out:
    return ret;
}

hi_s32 dmx_rmx_fct_add_pump(hi_handle rmx_handle, dmx_rmx_pump_attrs *pump_attrs, hi_handle *rmx_pump_handle,
                          struct dmx_session *session)
{
    hi_s32 ret;
    hi_handle  tmp_rmx_pump_hanle = HI_INVALID_HANDLE;
    struct dmx_r_rmx_fct  *rrmx_fct  = HI_NULL;
    struct dmx_r_rmx_pump *rrmx_pump  = HI_NULL;

    ret = DMX_R_RMX_FCT_GET(rmx_handle, rrmx_fct);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = rmx_fct_create_pump(rrmx_fct, pump_attrs, &tmp_rmx_pump_hanle, session);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    ret = DMX_R_RMX_PUMP_GET(tmp_rmx_pump_hanle, rrmx_pump);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    ret = rrmx_fct->ops->add_pump(rrmx_fct, pump_attrs, rrmx_pump);
    if (ret != HI_SUCCESS) {
        goto out3;
    }
    dmx_r_put((struct dmx_r_base *)rrmx_pump);
    dmx_r_put((struct dmx_r_base *)rrmx_fct);

    *rmx_pump_handle = tmp_rmx_pump_hanle;

    return HI_SUCCESS;
out3:
    rmx_fct_destroy_pump(tmp_rmx_pump_hanle);
out2:
    dmx_r_put((struct dmx_r_base *)rrmx_pump);
out1:
    dmx_r_put((struct dmx_r_base *)rrmx_fct);
out0:
    return ret;
}

hi_s32 dmx_rmx_fct_del_pump(hi_handle rmx_pump_handle)
{
    hi_s32 ret;

    struct dmx_r_rmx_pump *rrmx_pump  = HI_NULL;

    ret = DMX_R_RMX_PUMP_GET(rmx_pump_handle, rrmx_pump);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    if (!rrmx_pump || !rrmx_pump->rrmx_fct) {
        ret = HI_ERR_DMX_DETACH_FAILED;
        HI_ERR_DEMUX("please add pump first!\n");
        goto out1;
    }

    ret = rrmx_pump->rrmx_fct->ops->del_pump(rrmx_pump->rrmx_fct, rrmx_pump);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("delete filter failed!\n");
        goto out1;
    }

    ret = rmx_fct_destroy_pump(rmx_pump_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("destroy pump failed!\n");
        goto out1;
    }

out1:
    dmx_r_put((struct dmx_r_base *)rrmx_pump);
out0:
    return ret;
}

hi_s32 dmx_rmx_fct_del_all_pump(hi_handle rmx_handle)
{
    hi_s32 ret;
    struct dmx_r_rmx_fct  *rrmx_fct  = HI_NULL;

    ret = DMX_R_RMX_FCT_GET(rmx_handle, rrmx_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rrmx_fct->ops->del_all_pump(rrmx_fct);

    dmx_r_put((struct dmx_r_base *)rrmx_fct);
out:
    return ret;
}

hi_s32 dmx_rmx_fct_close(hi_handle rmx_handle)
{
    hi_s32 ret;
    struct dmx_r_rmx_fct *rrmx_fct = HI_NULL;

    ret = DMX_R_RMX_FCT_GET(rmx_handle, rrmx_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rrmx_fct->ops->close(rrmx_fct);

    dmx_r_put((struct dmx_r_base *)rrmx_fct);
out:
    return ret;
}

hi_s32 dmx_rmx_fct_destroy(hi_handle rmx_handle)
{
    hi_s32 ret;
    struct dmx_slot *slot = HI_NULL;
    struct dmx_r_rmx_fct *rrmx_fct = HI_NULL;

    ret = DMX_R_RMX_FCT_GET(rmx_handle, rrmx_fct);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = dmx_slot_find(rmx_handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    dmx_session_del_slot(slot->session, slot);

    dmx_slot_destroy(slot);

    ret = rrmx_fct->ops->destroy(rrmx_fct);

    dmx_r_put((struct dmx_r_base *)rrmx_fct);
out1:
    dmx_r_put((struct dmx_r_base *)rrmx_fct);
out0:
    return ret;
}

/*************************dmx_r_dmx_fct_xxx_impl*************************************************/
static inline hi_bool rmx_fct_staled(struct dmx_r_rmx_fct *rrmx_fct)
{
    if (unlikely(rrmx_fct->staled == HI_TRUE)) {
        HI_ERR_DEMUX("rmx_fct is staled.\n");
        return HI_TRUE;
    }

    return HI_FALSE;
}

static hi_s32 _dmx_r_rmx_fct_create_impl(struct dmx_r_rmx_fct *rrmx_fct, const dmx_rmx_attrs *attrs)
{
    hi_u32 ret;
    struct dmx_mgmt   *mgmt = rrmx_fct->base.mgmt;

    DMX_NULL_POINTER_RETURN(mgmt);

    /* todo something about hardware */

    ret = HI_SUCCESS;

    return ret;
}

static hi_s32 dmx_r_rmx_fct_create_impl(struct dmx_r_rmx_fct *rrmx_fct, const dmx_rmx_attrs *attrs)
{
    hi_s32 ret;

    osal_mutex_lock(&rrmx_fct->lock);

    WARN_ON(rrmx_fct->staled == HI_TRUE);

    ret = _dmx_r_rmx_fct_create_impl(rrmx_fct, attrs);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rrmx_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_rmx_fct_open_impl(struct dmx_r_rmx_fct *rrmx_fct)
{
    hi_s32 ret;
    hi_u32 index = 0;

    struct dmx_r_rmx_pump *rrmx_pump = HI_NULL;
    struct dmx_r_rmx_pump *tmp_rrmx_pump = HI_NULL;

    struct dmx_mgmt       *mgmt = rrmx_fct->base.mgmt;

    DMX_NULL_POINTER_RETURN(mgmt);

    /* enable the rmx */
    for_each_set_bit(index, rrmx_fct->port_bitmap, rrmx_fct->src_port_cnt) {
        rmx_port_info *rmx_port_info = &rrmx_fct->port_info[index];

        WARN_ON(rmx_port_info->ref == 0);

        dmx_hal_rmx_en_detect_port(mgmt, rrmx_fct->base.id, index);

        if (rmx_port_info->port_id < DMX_IF_PORT_0 + mgmt->if_port_cnt) {
            dmx_hal_rmx_set_port(mgmt, rrmx_fct->base.id, index, DMX_PORT_IF_TYPE,
                                 rmx_port_info->port_id - DMX_IF_PORT_0);
        } else if (rmx_port_info->port_id >= DMX_TSI_PORT_0 &&
                   rmx_port_info->port_id < DMX_TSI_PORT_0 + mgmt->tsi_port_cnt) {
            dmx_hal_rmx_set_port(mgmt, rrmx_fct->base.id, index, DMX_PORT_TSI_TYPE,
                                 rmx_port_info->port_id - DMX_TSI_PORT_0);
        } else if (rmx_port_info->port_id >= DMX_RAM_PORT_0 &&
                   rmx_port_info->port_id < DMX_RAM_PORT_0 + mgmt->ram_port_cnt) {
            dmx_hal_rmx_set_port(mgmt, rrmx_fct->base.id, index, DMX_PORT_RAM_TYPE,
                                 rmx_port_info->port_id - DMX_RAM_PORT_0);
        } else {
            WARN(1, "Invalid rmx port id[%#x]!\n", rmx_port_info->port_id);
        }
    }

    list_for_each_entry_safe(rrmx_pump, tmp_rrmx_pump, &rrmx_fct->pump_head, node1) {
        osal_mutex_lock(&rrmx_pump->lock);

        switch (rrmx_pump->pump_type) {
            case DMX_RMX_PUMP_TYPE_PID:
                dmx_hal_rmx_set_pid_tab(mgmt, rrmx_fct->base.id, rrmx_pump->base.id, rrmx_pump->rmx_port_id,
                                        rrmx_pump->pid);
                break;

            case DMX_RMX_PUMP_TYPE_REMAP_PID:
                dmx_hal_rmx_set_remap_pid_tab(mgmt, rrmx_fct->base.id, rrmx_pump->base.id, rrmx_pump->rmx_port_id,
                                              rrmx_pump->pid, rrmx_pump->remap_pid);
                break;

            case DMX_RMX_PUMP_TYPE_ALLPASS_PORT:
                dmx_hal_rmx_set_port_all_pass(mgmt, rrmx_fct->base.id, rrmx_pump->rmx_port_id);
                break;

            default:
                WARN(1, "Invalid pump type[%#x]!\n", rrmx_pump->pump_type);
                break;
        }

        osal_mutex_unlock(&rrmx_pump->lock);
    }

    ret = HI_SUCCESS;

    return ret;
}

static hi_s32 dmx_r_rmx_fct_open_impl(struct dmx_r_rmx_fct *rrmx_fct)
{
    hi_s32 ret;

    osal_mutex_lock(&rrmx_fct->lock);

    WARN_ON(rrmx_fct->staled == HI_TRUE);

    ret = _dmx_r_rmx_fct_open_impl(rrmx_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rrmx_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_rmx_fct_get_attrs_impl(struct dmx_r_rmx_fct *rrmx_fct, dmx_rmx_attrs *attrs)
{
    hi_s32 ret;

    attrs->out_port_id   = rrmx_fct->out_port_id;

    ret = HI_SUCCESS;

    return ret;
}

static hi_s32 dmx_r_rmx_fct_get_attrs_impl(struct dmx_r_rmx_fct *rrmx_fct, dmx_rmx_attrs *attrs)
{
    hi_s32 ret;

    osal_mutex_lock(&rrmx_fct->lock);

    WARN_ON(rrmx_fct->staled == HI_TRUE);

    ret = _dmx_r_rmx_fct_get_attrs_impl(rrmx_fct, attrs);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rrmx_fct->lock);

    return ret;
}

static hi_s32 dmx_r_rmx_fct_set_attrs_impl(struct dmx_r_rmx_fct *rrmx_fct, const dmx_rmx_attrs *attrs)
{
    osal_mutex_lock(&rrmx_fct->lock);

    WARN_ON(rrmx_fct->staled == HI_TRUE);

    HI_ERR_DEMUX("rrmx_fct not support setattrs yet!\n");

    osal_mutex_unlock(&rrmx_fct->lock);

    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 _dmx_r_rmx_fct_get_status_impl(struct dmx_r_rmx_fct *rrmx_fct, dmx_rmx_status *status)
{
    status->out_port_id = rrmx_fct->out_port_id;

    return HI_SUCCESS;
}

static hi_s32 dmx_r_rmx_fct_get_status_impl(struct dmx_r_rmx_fct *rrmx_fct, dmx_rmx_status *status)
{
    hi_s32 ret;

    osal_mutex_lock(&rrmx_fct->lock);

    WARN_ON(rrmx_fct->staled == HI_TRUE);

    ret = _dmx_r_rmx_fct_get_status_impl(rrmx_fct, status);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rrmx_fct->lock);

    return ret;
}

static hi_s32 rmx_port_acquire(struct dmx_r_rmx_fct  *rrmx_fct, const dmx_rmx_pump_attrs *pump_attrs,
                               hi_u32 *rmx_port_id)
{
    hi_s32 ret = HI_ERR_DMX_NO_RESOURCE;
    hi_u32 index;

    /* reused */
    for_each_set_bit(index, rrmx_fct->port_bitmap, rrmx_fct->src_port_cnt) {
        rmx_port_info *rmx_port_info = &rrmx_fct->port_info[index];

        WARN_ON(rmx_port_info->ref == 0);

        if (pump_attrs->in_port_id == rmx_port_info->port_id) {
            rmx_port_info->ref++;

            *rmx_port_id = index;

            ret = HI_SUCCESS;
            break;
        }
    }

    /* new */
    if (ret != HI_SUCCESS) {
        index = find_first_zero_bit(rrmx_fct->port_bitmap, rrmx_fct->src_port_cnt);
        if (index < rrmx_fct->src_port_cnt) {
            rmx_port_info *rmx_port_info = &rrmx_fct->port_info[index];

            WARN_ON(rmx_port_info->ref != 0x0);

            rmx_port_info->port_id = pump_attrs->in_port_id;
            rmx_port_info->ref = 1;

            *rmx_port_id = index;
            set_bit(index, rrmx_fct->port_bitmap);

            ret = HI_SUCCESS;
        }
    }

    return ret ;
}

static hi_s32 rmx_port_release(struct dmx_r_rmx_fct  *rrmx_fct, hi_u32 rmx_port_id)
{
    rmx_port_info *rmx_port_info = &rrmx_fct->port_info[rmx_port_id];

    WARN_ON(rmx_port_id >= rrmx_fct->src_port_cnt);

    rmx_port_info->ref--;

    if (rmx_port_info->ref == 0) {
        clear_bit(rmx_port_id, rrmx_fct->port_bitmap);
    }

    return HI_SUCCESS;
}

static hi_s32 _dmx_r_rmx_fct_add_pump_impl(struct dmx_r_rmx_fct *rrmx_fct, const dmx_rmx_pump_attrs *pump_attrs,
        struct dmx_r_rmx_pump *rrmx_pump)
{
    hi_s32 ret;

    /* fixme: add pump to rmxfct */
    /* get the pump obj */
    dmx_r_get_raw((struct dmx_r_base *)rrmx_pump);

    ret = rrmx_pump->ops->set_attrs(rrmx_pump, pump_attrs);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rmx_port_acquire(rrmx_fct, pump_attrs, &rrmx_pump->rmx_port_id);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    list_add_tail(&rrmx_pump->node1, &rrmx_fct->pump_head);

    rrmx_pump->rrmx_fct = rrmx_fct;

    return HI_SUCCESS;

out:
    dmx_r_put((struct dmx_r_base *)rrmx_pump);
    return ret;
}

static hi_s32 dmx_r_rmx_fct_add_pump_impl(struct dmx_r_rmx_fct *rrmx_fct, const dmx_rmx_pump_attrs *pump_attrs,
                                        struct dmx_r_rmx_pump *rrmx_pump)
{
    hi_s32 ret;

    osal_mutex_lock(&rrmx_fct->lock);

    WARN_ON(rrmx_fct->staled == HI_TRUE);

    ret = _dmx_r_rmx_fct_add_pump_impl(rrmx_fct, pump_attrs, rrmx_pump);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rrmx_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_rmx_fct_del_pump_impl(struct dmx_r_rmx_fct *rrmx_fct, struct dmx_r_rmx_pump *rrmx_pump)
{
    hi_s32 ret;

    ret = rmx_port_release(rrmx_fct, rrmx_pump->rmx_port_id);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    list_del(&rrmx_pump->node1);

    /* put the pump obj */
    dmx_r_put((struct dmx_r_base *)rrmx_pump);

out:
    return ret;
}

static hi_s32 dmx_r_rmx_fct_del_pump_impl(struct dmx_r_rmx_fct *rrmx_fct, struct dmx_r_rmx_pump *rrmx_pump)
{
    hi_s32 ret;

    osal_mutex_lock(&rrmx_fct->lock);

    WARN_ON(rrmx_fct->staled == HI_TRUE);

    ret = _dmx_r_rmx_fct_del_pump_impl(rrmx_fct, rrmx_pump);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rrmx_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_rmx_fct_del_all_pump_impl(struct dmx_r_rmx_fct *rrmx_fct)
{
    hi_s32 ret;

    struct list_head *node;
    struct list_head *tmp_node;

    list_for_each_safe(node, tmp_node, &rrmx_fct->pump_head) {
        struct dmx_r_rmx_pump *rrmx_pump = list_entry(node, struct dmx_r_rmx_pump, node1);

        ret = rmx_port_release(rrmx_fct, rrmx_pump->rmx_port_id);
        if (ret != HI_SUCCESS) {
            goto out;
        }

        /*
         * if the pumphandle has been destroyed it has been setted as invalid in rmx_fct_destroy_pump,
         * avoid destroy it repeatedly
         */
        if (rrmx_pump->rmx_pump_handle != HI_INVALID_HANDLE) {
            /* destroy the pump handle and del the reference of the object */
            ret = rmx_fct_destroy_pump(rrmx_pump->rmx_pump_handle);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("destroy pump handle failed!\n");
                goto out;
            }
        }

        /* delete the entry from the list */
        list_del(&rrmx_pump->node1);

        /* put the pump obj */
        dmx_r_put((struct dmx_r_base *)rrmx_pump);
    }

    ret = HI_SUCCESS;
out:

    return ret;
}

static hi_s32 dmx_r_rmx_fct_del_all_pump_impl(struct dmx_r_rmx_fct *rrmx_fct)
{
    hi_s32 ret;

    osal_mutex_lock(&rrmx_fct->lock);

    WARN_ON(rrmx_fct->staled == HI_TRUE);

    ret = _dmx_r_rmx_fct_del_all_pump_impl(rrmx_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rrmx_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_rmx_fct_close_impl(struct dmx_r_rmx_fct *rrmx_fct)
{
    /* disable the rmx port */
    hi_u32 index = 0;

    struct dmx_r_rmx_pump *rrmx_pump = HI_NULL;
    struct dmx_r_rmx_pump *tmp_rrmx_pump = HI_NULL;

    struct dmx_mgmt       *mgmt = rrmx_fct->base.mgmt;

    DMX_NULL_POINTER_RETURN(mgmt);

    list_for_each_entry_safe(rrmx_pump, tmp_rrmx_pump, &rrmx_fct->pump_head, node1) {
        WARN_ON(rrmx_pump->rmx_port_id >= rrmx_fct->src_port_cnt);

        switch (rrmx_pump->pump_type) {
            case DMX_RMX_PUMP_TYPE_PID:
            case DMX_RMX_PUMP_TYPE_REMAP_PID:
                dmx_hal_rmx_un_set_pid_tab(mgmt, rrmx_fct->base.id, rrmx_pump->base.id);
                break;

            case DMX_RMX_PUMP_TYPE_ALLPASS_PORT:
                dmx_hal_rmx_un_set_port_all_pass(mgmt, rrmx_fct->base.id, rrmx_pump->rmx_port_id);
                break;

            default:
                WARN(1, "Invalid pump type[%#x]!\n", rrmx_pump->pump_type);
                break;
        }
    }

    /* disable the rmx */
    for_each_set_bit(index, rrmx_fct->port_bitmap, rrmx_fct->src_port_cnt) {
        dmx_hal_rmx_unset_port(mgmt, rrmx_fct->base.id, index);
        dmx_hal_rmx_dis_detect_port(mgmt, rrmx_fct->base.id, index);
    }

    return HI_SUCCESS;
}

static hi_s32 dmx_r_rmx_fct_close_impl(struct dmx_r_rmx_fct *rrmx_fct)
{
    hi_s32 ret;

    osal_mutex_lock(&rrmx_fct->lock);

    WARN_ON(rrmx_fct->staled == HI_TRUE);

    ret = _dmx_r_rmx_fct_close_impl(rrmx_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rrmx_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_rmx_fct_destroy_impl(struct dmx_r_rmx_fct *rrmx_fct)
{
    hi_u32 ret;

    struct dmx_mgmt   *mgmt = HI_NULL;

    mgmt = rrmx_fct->base.mgmt;

    DMX_NULL_POINTER_RETURN(mgmt);

    /* post desroy begin */
    _dmx_r_rmx_fct_close_impl(rrmx_fct);
    _dmx_r_rmx_fct_del_all_pump_impl(rrmx_fct);

    ret = HI_SUCCESS;

    return ret;
}

static hi_s32 dmx_r_rmx_fct_destroy_impl(struct dmx_r_rmx_fct *rrmx_fct)
{
    hi_s32 ret;

    osal_mutex_lock(&rrmx_fct->lock);

    WARN_ON(rrmx_fct->staled == HI_TRUE);

    ret = _dmx_r_rmx_fct_destroy_impl(rrmx_fct);
    if (ret == HI_SUCCESS) {
        rrmx_fct->staled = HI_TRUE;
    }

    osal_mutex_unlock(&rrmx_fct->lock);

    return ret;
}

static hi_s32 dmx_r_rmx_fct_suspend_impl(struct dmx_r_rmx_fct *rrmx_fct)
{
    osal_mutex_lock(&rrmx_fct->lock);

    WARN_ON(rrmx_fct->staled == HI_TRUE);

    _dmx_r_rmx_fct_close_impl(rrmx_fct);

    osal_mutex_unlock(&rrmx_fct->lock);

    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 dmx_r_rmx_fct_resume_impl(struct dmx_r_rmx_fct *rrmx_fct)
{
    osal_mutex_lock(&rrmx_fct->lock);

    WARN_ON(rrmx_fct->staled == HI_TRUE);

    _dmx_r_rmx_fct_open_impl(rrmx_fct);

    osal_mutex_unlock(&rrmx_fct->lock);

    return HI_ERR_DMX_NOT_SUPPORT;
}

struct dmx_r_rmx_fct_ops g_dmx_rmx_fct_ops = {
    .create         = dmx_r_rmx_fct_create_impl,
    .open           = dmx_r_rmx_fct_open_impl,
    .get_attrs      = dmx_r_rmx_fct_get_attrs_impl,
    .set_attrs      = dmx_r_rmx_fct_set_attrs_impl,
    .get_status     = dmx_r_rmx_fct_get_status_impl,
    .add_pump       = dmx_r_rmx_fct_add_pump_impl,
    .del_pump       = dmx_r_rmx_fct_del_pump_impl,
    .del_all_pump   = dmx_r_rmx_fct_del_all_pump_impl,
    .close          = dmx_r_rmx_fct_close_impl,
    .destroy        = dmx_r_rmx_fct_destroy_impl,

    .suspend        = dmx_r_rmx_fct_suspend_impl,
    .resume         = dmx_r_rmx_fct_resume_impl,
};

