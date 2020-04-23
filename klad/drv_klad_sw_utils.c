/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
* Description: klad basic utils impl.
* Author: SDK team
* Create: 2019-08-21
*/

#include <linux/kernel.h>
#include "linux/delay.h"
#include "linux/bitmap.h"
#include "linux/io.h"
#include "linux/interrupt.h"
#include "linux/ratelimit.h"

#include "hi_drv_sys.h"
#include "hi_drv_mem.h"

#include "drv_klad_sw_utils.h"
#include "drv_klad_sw_func.h"
#include "drv_klad_sw_define.h"
#include "linux/hisilicon/securec.h"
#include "hi_osal.h"

/*
 * KLAD export functions.
 */
static struct klad_sw_session_ops g_klad_sw_session_ops;

static inline hi_s32 get_sw_session(struct klad_sw_session *sw_session)
{
    WARN_ON(&g_klad_sw_session_ops != sw_session->ops);
    WARN_ON(atomic_read(&sw_session->ref_count) == 0);

    if (!atomic_inc_not_zero(&sw_session->ref_count)) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static inline hi_void put_sw_session(struct klad_sw_session *sw_session)
{
    WARN_ON(atomic_read(&sw_session->ref_count) == 0);

    if (atomic_dec_and_test(&sw_session->ref_count)) {
        HI_DBG_KLAD("@sw_session(0x%p) released.\n", sw_session);
        HI_KFREE(HI_ID_KLAD, sw_session);
    }
}

hi_s32 klad_sw_session_create(struct klad_sw_session **new_sw_session)
{
    hi_s32 ret;
    struct klad_sw_session *sw_session = HI_NULL;

    ret = klad_slot_mgmt_init();
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    sw_session = osal_kmalloc(HI_ID_KLAD, sizeof(struct klad_sw_session), OSAL_GFP_KERNEL);
    if (sw_session == HI_NULL) {
        HI_ERR_KLAD("alloc sw_session memory failed.\n");
        ret = HI_ERR_KLAD_NO_MEMORY;
        goto out1;
    }
    ret = memset_s(sw_session, sizeof(struct klad_sw_session), 0, sizeof(struct klad_sw_session));
    if (ret != EOK) {
        HI_ERR_KLAD("Call memset_s failed.\n");
        ret = HI_FAILURE;
        goto out1;
    }

    atomic_set(&sw_session->ref_count, 1);

    ret = osal_mutex_init(&sw_session->lock);
    if (ret != HI_SUCCESS) {
        HI_ERR_KLAD("Init mutex failed.\n");
        goto out1;
    }
    sw_session->state = KLAD_SW_SESSION_ACTIVED;
    sw_session->ops = &g_klad_sw_session_ops;
    INIT_LIST_HEAD(&sw_session->head);

    *new_sw_session = sw_session;

    HI_DBG_KLAD("@sw_session(0x%p) created.\n", sw_session);

    return HI_SUCCESS;

out1:
    klad_slot_mgmt_exit();
out0:
    return ret;
}

hi_s32 klad_sw_session_add_slot(struct klad_sw_session *sw_session, struct klad_slot *slot)
{
    hi_s32 ret;

    ret = get_sw_session(sw_session);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = sw_session->ops->add_slot(sw_session, slot);

    put_sw_session(sw_session);
out:
    return ret;
}

hi_s32 klad_sw_session_del_slot(struct klad_sw_session *sw_session, struct klad_slot *slot)
{
    hi_s32 ret;

    ret = get_sw_session(sw_session);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = sw_session->ops->del_slot(sw_session, slot);

    put_sw_session(sw_session);
out:
    return ret;
}

hi_s32 klad_sw_session_destroy(struct klad_sw_session *sw_session)
{
    hi_s32 ret;

    ret = get_sw_session(sw_session);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    /* close the mgmt depend on release all resource firstly. */
    sw_session->state = KLAD_SW_SESSION_INACTIVED;
    sw_session->ops->release(sw_session);

    sw_session->ops = HI_NULL;

    atomic_dec(&sw_session->ref_count);

    put_sw_session(sw_session);

    klad_slot_mgmt_exit();

    ret = HI_SUCCESS;

out:
    return ret;
}

static struct klad_sw_r_base_ops g_klad_rbase_ops;

struct klad_sw_r_base_ops *get_klad_sw_r_base_ops(hi_void)
{
    return &g_klad_rbase_ops;
}

hi_s32 klad_slot_create(struct klad_slot **slot)
{
    hi_s32 ret;
    struct klad_slot_table *slot_table = get_slot_table();
    struct klad_slot *new_slot = HI_NULL;
    hi_u32 id;

    __mutex_lock(&slot_table->lock);

    id = find_first_zero_bit(slot_table->slot_bitmap, slot_table->slot_cnt);
    if (!(id < slot_table->slot_cnt)) {
        HI_ERR_KLAD("there is no available slot now!\n");
        ret = HI_ERR_KLAD_NO_RESOURCE;
        goto out;
    }

    new_slot = &slot_table->table[id];

    osal_mutex_init(&new_slot->lock);
    new_slot->handle  = HI_HANDLE_INIT(HI_ID_KLAD, 0, id);
    new_slot->release = HI_NULL;

    set_bit(id, slot_table->slot_bitmap);

    *slot = new_slot;

    ret = HI_SUCCESS;

out:
    __mutex_unlock(&slot_table->lock);

    return ret;
}

static hi_s32 __klad_slot_find(hi_handle handle, struct klad_slot **slot)
{
    hi_s32 ret;
    struct klad_slot_table *slot_table = get_slot_table();
    unsigned long mask;
    unsigned long *p = HI_NULL;
    hi_u32 id;

    id = HI_HANDLE_GET_CHAN_ID(handle);
    if (unlikely(!(id < slot_table->slot_cnt))) {
        HI_ERR_KLAD("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_KLAD_INVALID_PARAM;
        goto out;
    }

    mask = BIT_MASK(id);
    p = ((unsigned long *)slot_table->slot_bitmap) + BIT_WORD(id);
    if (!(*p & mask)) {
        HI_ERR_KLAD("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_KLAD_INVALID_PARAM;
        goto out;
    }

    *slot = &slot_table->table[id];

    ret = HI_SUCCESS;

out:
    return ret;
}

hi_s32 klad_slot_find(hi_handle handle, struct klad_slot **slot)
{
    hi_s32 ret;
    struct klad_slot_table *slot_table = get_slot_table();

    __mutex_lock(&slot_table->lock);

    ret = __klad_slot_find(handle, slot);

    __mutex_unlock(&slot_table->lock);

    return ret;
}

hi_s32 klad_slot_get_robj(hi_handle handle, struct klad_sw_r_base **obj)
{
    hi_s32 ret;
    struct klad_slot_table *slot_table = get_slot_table();
    struct klad_slot *slot = HI_NULL;

    __mutex_lock(&slot_table->lock);

    ret = __klad_slot_find(handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    __mutex_lock(&slot->lock);

    WARN_ON(&g_klad_rbase_ops != slot->obj->ops);

    ret = slot->obj->ops->get(slot->obj);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    *obj = slot->obj;

out1:
    __mutex_unlock(&slot->lock);
out0:
    __mutex_unlock(&slot_table->lock);

    return ret;
}

hi_s32 klad_slot_bind(hi_handle handle, struct klad_sw_r_base *obj)
{
    hi_s32 ret;
    struct klad_slot_table *slot_table = get_slot_table();
    struct klad_slot *slot = HI_NULL;

    __mutex_lock(&slot_table->lock);

    ret = __klad_slot_find(handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    __mutex_lock(&slot->lock);

    slot->obj = obj;

    __mutex_unlock(&slot->lock);
out0:
    __mutex_unlock(&slot_table->lock);

    return ret;
}


hi_s32 klad_slot_destroy(struct klad_slot *slot)
{
    hi_s32 ret;
    struct klad_slot_table *slot_table = get_slot_table();
    unsigned long mask;
    unsigned long *p = HI_NULL;
    hi_u32 id;

    id = HI_HANDLE_GET_CHAN_ID(slot->handle);

    __mutex_lock(&slot_table->lock);

    __mutex_lock(&slot->lock);

    mask = BIT_MASK(id);
    p = ((unsigned long *)slot_table->slot_bitmap) + BIT_WORD(id);
    if (!(*p & mask)) {
        HI_ERR_KLAD("invalid slot table id(%d).\n", id);
        ret = HI_ERR_KLAD_INVALID_PARAM;
        goto out;
    }

    clear_bit(id, slot_table->slot_bitmap);

    ret = HI_SUCCESS;

out:
    __mutex_unlock(&slot->lock);

    __mutex_unlock(&slot_table->lock);

    return ret;
}

hi_s32 klad_sw_r_get_raw(struct klad_sw_r_base *obj)
{
    return obj->ops->get(obj);
}

hi_s32 klad_sw_r_get(hi_handle handle, struct klad_sw_r_base **obj)
{
    return klad_slot_get_robj(handle, obj);
}

hi_void klad_sw_r_put(struct klad_sw_r_base *obj)
{
    obj->ops->put(obj);
}

static hi_s32 klad_sw_session_add_slot_impl(struct klad_sw_session *sw_session, struct klad_slot *slot)
{
    hi_s32 ret;

    WARN_ON(slot->release == HI_NULL);

    __mutex_lock(&sw_session->lock);

    if (sw_session->state != KLAD_SW_SESSION_ACTIVED) {
        ret = HI_FAILURE;
        goto out;
    }
    slot->sw_session = sw_session;

    list_add_tail(&slot->node, &sw_session->head);

    ret = HI_SUCCESS;

out:
    __mutex_unlock(&sw_session->lock);

    return ret;
}

static hi_s32 klad_sw_session_del_slot_impl(struct klad_sw_session *sw_session, struct klad_slot *slot)
{
    hi_s32 ret = HI_FAILURE;
    struct list_head *node = HI_NULL;
    struct list_head *tmp = HI_NULL;

    WARN_ON(slot->release == HI_NULL);

    __mutex_lock(&sw_session->lock);

    list_for_each_safe(node, tmp, &sw_session->head) {
        struct klad_slot *obj = list_entry(node, struct klad_slot, node);
        if (obj == slot) {
            list_del(node);
            ret = HI_SUCCESS;
            break;
        }
    }

    __mutex_unlock(&sw_session->lock);

    return ret;
}

static hi_void klad_sw_session_release_impl(struct klad_sw_session *sw_session)
{
    WARN_ON(&g_klad_sw_session_ops != sw_session->ops);
    WARN_ON(sw_session->state != KLAD_SW_SESSION_INACTIVED);

    /*
     * it will wait work finished for each slot and no new slot add in,
     * so no need to hold the sw_session lock.
     */
    while (!list_empty(&sw_session->head)) {
        /* revert search enry. */
        (void *)list_last_entry(&sw_session->head, struct klad_slot, node);
    }
}

static struct klad_sw_session_ops g_klad_sw_session_ops = {
    .add_slot  = klad_sw_session_add_slot_impl,
    .del_slot  = klad_sw_session_del_slot_impl,
    .release   = klad_sw_session_release_impl,
};

/*
 * R Base functions.
 */
static hi_s32 klad_sw_r_base_get_impl(struct klad_sw_r_base *obj)
{
    WARN_ON(&g_klad_rbase_ops != obj->ops);
    WARN_ON(atomic_read(&obj->ref_count) == 0);

    if (!atomic_inc_not_zero(&obj->ref_count)) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void klad_sw_r_base_put_impl(struct klad_sw_r_base *obj)
{
    WARN_ON(&g_klad_rbase_ops != obj->ops);
    WARN_ON(atomic_read(&obj->ref_count) == 0);

    if (atomic_dec_and_test(&obj->ref_count)) {
        hi_s32 ret = obj->release(obj);
        WARN_ON(ret != HI_SUCCESS);

        HI_DBG_KLAD("#    robj(0x%p) released.\n", obj);
    }
}

static struct klad_sw_r_base_ops g_klad_rbase_ops = {
    .get       = klad_sw_r_base_get_impl,
    .put       = klad_sw_r_base_put_impl,
};

static struct klad_slot_table g_klad_slot_table = {
    .lock        = {0},
    .slot_cnt    = KLAD_SW_SLOT_MAX_CNT,
    .slot_bitmap = {0},
};

struct klad_slot_table *get_slot_table(hi_void)
{
    return &g_klad_slot_table;
}

