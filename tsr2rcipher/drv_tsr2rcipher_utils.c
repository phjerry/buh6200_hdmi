/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: tsr2rcipher utils, such as session management, object management, etc.
 */

#include "linux/bitmap.h"

#include "hi_type.h"
#include "hi_osal.h"

#include "drv_tsr2rcipher_utils.h"
#include "drv_tsr2rcipher_func.h"

#include "hi_drv_module.h"
#include "hi_drv_mem.h"
#include "hi_drv_proc.h"

/* R Obj life control functions. */
static struct tsr2rcipher_r_base_ops g_tsr2rcipher_rbase_ops;

struct tsr2rcipher_r_base_ops *get_tsr2rcipher_rbase_ops(hi_void)
{
    return &g_tsr2rcipher_rbase_ops;
}

static hi_s32 tsr2rcipher_r_base_get_impl(struct tsr2rcipher_r_base *obj)
{
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VALUE(&g_tsr2rcipher_rbase_ops != obj->ops, HI_FAILURE);
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VALUE(osal_atomic_read(&obj->ref_count) == 0, HI_FAILURE);

    if (osal_atomic_inc_return(&obj->ref_count) == 0) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void tsr2rcipher_r_base_put_impl(struct tsr2rcipher_r_base *obj)
{
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(&g_tsr2rcipher_rbase_ops != obj->ops);
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(osal_atomic_read(&obj->ref_count) == 0);

    if (osal_atomic_dec_return(&obj->ref_count) == 0) {
        hi_s32 ret = obj->release(obj);
        TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(ret != HI_SUCCESS);

        HI_DBG_TSR2RCIPHER("#    robj(0x%x) released.\n", obj);
    }
}

static struct tsr2rcipher_r_base_ops g_tsr2rcipher_rbase_ops = {
    .get = tsr2rcipher_r_base_get_impl,
    .put = tsr2rcipher_r_base_put_impl,
};

hi_s32 tsr2rcipher_r_get_raw(struct tsr2rcipher_r_base *obj)
{
    return obj->ops->get(obj);
}

hi_s32 tsr2rcipher_r_get(hi_handle handle, struct tsr2rcipher_r_base **obj)
{
    return tsr2rcipher_slot_get_robj(handle, obj);
}

hi_void tsr2rcipher_r_put(struct tsr2rcipher_r_base *obj)
{
    obj->ops->put(obj);
}

/* slot management functions. */
static struct tsr2rcipher_slot_table g_tsr2rcipher_slot_table = {
    .slot_cnt    = TSR2RCIPHER_MAX_SLOT_CNT,
    .slot_bitmap = {0},
};

hi_s32 tsr2rcipher_slot_create(struct tsr2rcipher_r_base *obj, struct tsr2rcipher_slot **slot)
{
    hi_s32 ret = HI_FAILURE;
    struct tsr2rcipher_slot_table *slot_table = &g_tsr2rcipher_slot_table;
    struct tsr2rcipher_slot *new_slot = HI_NULL;
    hi_u32 id;

    TSR2RCIPHER_CHECK_CONDITION_RETURN_VALUE(&g_tsr2rcipher_rbase_ops != obj->ops, HI_FAILURE);

    osal_mutex_lock(&slot_table->lock);

    id = find_first_zero_bit(slot_table->slot_bitmap, slot_table->slot_cnt);
    if (!(id < slot_table->slot_cnt)) {
        HI_ERR_TSR2RCIPHER("there is no available slot now!\n");
        ret = HI_ERR_TSR2RCIPHER_BUSY;
        goto out;
    }

    new_slot = &slot_table->table[id];

    osal_mutex_init(&new_slot->lock);
    new_slot->handle  = ID_2_HANDLE(id);
    new_slot->obj     = obj;
    new_slot->session = HI_NULL;
    new_slot->release = HI_NULL;

    set_bit(id, slot_table->slot_bitmap);

    *slot = new_slot;

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&slot_table->lock);

    return ret;
}

static hi_s32 _tsr2rcipher_slot_find(hi_handle handle, struct tsr2rcipher_slot **slot)
{
    hi_s32 ret = HI_FAILURE;
    struct tsr2rcipher_slot_table *slot_table = &g_tsr2rcipher_slot_table;
    unsigned long mask, *p;
    hi_u32 id;

    id = HANDLE_2_ID(handle);

    if (unlikely(!(id < slot_table->slot_cnt))) {
        HI_ERR_TSR2RCIPHER("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSR2RCIPHER_INVALID_PARA;
        goto out;
    }

    mask = BIT_MASK(id);
    p = ((unsigned long *)slot_table->slot_bitmap) + BIT_WORD(id);
    if (!(*p & mask)) {
        HI_ERR_TSR2RCIPHER("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSR2RCIPHER_INVALID_PARA;
        goto out;
    }

    *slot = &slot_table->table[id];

    ret = HI_SUCCESS;

out:
    return ret;
}

hi_s32 tsr2rcipher_slot_find(hi_handle handle, struct tsr2rcipher_slot **slot)
{
    hi_s32 ret = HI_FAILURE;
    struct tsr2rcipher_slot_table *slot_table = &g_tsr2rcipher_slot_table;

    osal_mutex_lock(&slot_table->lock);

    ret = _tsr2rcipher_slot_find(handle, slot);

    osal_mutex_unlock(&slot_table->lock);

    return ret;
}

hi_s32 tsr2rcipher_slot_get_robj(hi_handle handle, struct tsr2rcipher_r_base **obj)
{
    hi_s32 ret = HI_FAILURE;
    struct tsr2rcipher_slot_table *slot_table = &g_tsr2rcipher_slot_table;
    struct tsr2rcipher_slot *slot = HI_NULL;

    osal_mutex_lock(&slot_table->lock);

    ret = _tsr2rcipher_slot_find(handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    osal_mutex_lock(&slot->lock);

    if (&g_tsr2rcipher_rbase_ops != slot->obj->ops) {
        HI_FATAL_TSR2RCIPHER("ops fatal error!\n");
        goto out1;
    }

    ret = slot->obj->ops->get(slot->obj);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    *obj = slot->obj;

out1:
    osal_mutex_unlock(&slot->lock);
out0:
    osal_mutex_unlock(&slot_table->lock);

    return ret;
}

hi_s32 tsr2rcipher_slot_destroy(struct tsr2rcipher_slot *slot)
{
    hi_s32 ret = HI_FAILURE;
    struct tsr2rcipher_slot_table *slot_table = &g_tsr2rcipher_slot_table;
    unsigned long mask;
    unsigned long *p = HI_NULL;
    hi_u32 id;

    id = HANDLE_2_ID(slot->handle);

    osal_mutex_lock(&slot_table->lock);

    osal_mutex_lock(&slot->lock);

    mask = BIT_MASK(id);
    p = ((unsigned long *)slot_table->slot_bitmap) + BIT_WORD(id);
    if (!(*p & mask)) {
        HI_ERR_TSR2RCIPHER("invalid slot table id(%d).\n", id);
        ret = HI_ERR_TSR2RCIPHER_INVALID_PARA;
        goto out;
    }

    clear_bit(id, slot_table->slot_bitmap);

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&slot->lock);
    if (ret == HI_SUCCESS) {
        osal_mutex_destory(&slot->lock);
    }
    osal_mutex_unlock(&slot_table->lock);

    return ret;
}

/* session management functions. */
struct session_worker {
    struct work_struct worker;
    struct tsr2rcipher_slot *slot;
};

static hi_void session_worker_fn(struct work_struct *work)
{
    hi_s32 ret = HI_FAILURE;
    struct session_worker *helper = container_of(work, struct session_worker, worker);

    ret = helper->slot->release(helper->slot->handle);
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(ret != HI_SUCCESS);
}

static hi_s32 tsr2rcipher_session_add_slot_impl(struct tsr2rcipher_session *session, struct tsr2rcipher_slot *slot)
{
    hi_s32 ret = HI_FAILURE;
    struct obj_node_helper *new_obj = HI_NULL;

    TSR2RCIPHER_CHECK_CONDITION_RETURN_VALUE(slot->release == HI_NULL, HI_FAILURE);

    osal_mutex_lock(&session->lock);

    if (session->state != TSR2RCIPHER_SESSION_ACTIVED) {
        goto out;
    }

    new_obj = HI_KZALLOC(HI_ID_TSR2RCIPHER, sizeof(struct obj_node_helper), GFP_KERNEL);
    if (new_obj == HI_NULL) {
        ret = HI_ERR_TSR2RCIPHER_ALLOC_MEM_FAILED;
        goto out;
    }

    new_obj->key = (hi_void *)slot;

    slot->session = session;

    list_add_tail(&new_obj->node, &session->head);

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&session->lock);

    return ret;
}

static hi_s32 tsr2rcipher_session_del_slot_impl(struct tsr2rcipher_session *session, struct tsr2rcipher_slot *slot)
{
    hi_s32 ret = HI_FAILURE;
    struct list_head *node, *tmp;

    TSR2RCIPHER_CHECK_CONDITION_RETURN_VALUE(slot->release == HI_NULL, HI_FAILURE);

    osal_mutex_lock(&session->lock);

    list_for_each_safe(node, tmp, &session->head) {
        struct obj_node_helper *obj = list_entry(node, struct obj_node_helper, node);

        if (obj->key == (hi_void *)slot) {
            list_del(node);

            HI_KFREE(HI_ID_TSR2RCIPHER, obj);

            ret = HI_SUCCESS;

            break;
        }
    }

    osal_mutex_unlock(&session->lock);

    return ret;
}

static struct tsr2rcipher_session_ops g_tsr2rcipher_session_ops;

static hi_void tsr2rcipher_session_release_impl(struct tsr2rcipher_session *session)
{
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(&g_tsr2rcipher_session_ops != session->ops);
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(session->state != TSR2RCIPHER_SESSION_INACTIVED);

    /* FIXME: it will wait work finished for each slot and no new slot add in, so no need to hold the session lock. */
    while (!list_empty(&session->head)) {
        /* revert search enry. */
        struct obj_node_helper *obj = list_last_entry(&session->head, struct obj_node_helper, node);
        struct session_worker rel_helper = {
            .slot = (struct tsr2rcipher_slot *)obj->key,
        };

        TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(HI_NULL == rel_helper.slot->release);

        INIT_WORK_ONSTACK(&rel_helper.worker, session_worker_fn);

        schedule_work(&rel_helper.worker);

        flush_scheduled_work();

        destroy_work_on_stack(&rel_helper.worker);
    }
}

static struct tsr2rcipher_session_ops g_tsr2rcipher_session_ops = {
    .add_slot = tsr2rcipher_session_add_slot_impl,
    .del_slot = tsr2rcipher_session_del_slot_impl,
    .release  = tsr2rcipher_session_release_impl,
};

static inline hi_s32 tsr2rcipher_get_session(struct tsr2rcipher_session *session)
{
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VALUE(&g_tsr2rcipher_session_ops != session->ops, HI_FAILURE);
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VALUE(osal_atomic_read(&session->ref_count) == 0, HI_FAILURE);

    if (osal_atomic_inc_return(&session->ref_count) == 0) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static inline hi_void tsr2rcipher_put_session(struct tsr2rcipher_session *session)
{
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(osal_atomic_read(&session->ref_count) == 0);

    if (osal_atomic_dec_return(&session->ref_count) == 0) {
        HI_DBG_TSR2RCIPHER("@session(0x%x) released.\n", session);
        osal_atomic_destory(&session->ref_count);
        HI_KFREE(HI_ID_TSR2RCIPHER, session);
    }
}

hi_s32 tsr2rcipher_session_create(struct tsr2rcipher_session **new_session)
{
    hi_s32 ret;
    struct tsr2rcipher_session *session = HI_NULL;

    session = HI_KZALLOC(HI_ID_TSR2RCIPHER, sizeof(struct tsr2rcipher_session), GFP_KERNEL);
    if (session == HI_NULL) {
        HI_ERR_TSR2RCIPHER("alloc session memory failed.\n");
        ret = HI_ERR_TSR2RCIPHER_ALLOC_MEM_FAILED;
        goto out;
    }

    osal_atomic_init(&session->ref_count);
    osal_atomic_set(&session->ref_count, 1);
    osal_mutex_init(&session->lock);
    session->state = TSR2RCIPHER_SESSION_ACTIVED;
    session->ops = &g_tsr2rcipher_session_ops;
    INIT_LIST_HEAD(&session->head);

    *new_session = session;

    HI_DBG_TSR2RCIPHER("@session(0x%x) created.\n", session);

    return HI_SUCCESS;

out:
    return ret;
}

hi_s32 tsr2rcipher_session_add_slot(struct tsr2rcipher_session *session, struct tsr2rcipher_slot *slot)
{
    hi_s32 ret = HI_FAILURE;

    ret = tsr2rcipher_get_session(session);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = session->ops->add_slot(session, slot);

    tsr2rcipher_put_session(session);
out:
    return ret;
}

hi_s32 tsr2rcipher_session_del_slot(struct tsr2rcipher_session *session, struct tsr2rcipher_slot *slot)
{
    hi_s32 ret = HI_FAILURE;

    ret = tsr2rcipher_get_session(session);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = session->ops->del_slot(session, slot);

    tsr2rcipher_put_session(session);
out:
    return ret;
}

hi_s32 tsr2rcipher_session_destroy(struct tsr2rcipher_session *session)
{
    hi_s32 ret;

    ret = tsr2rcipher_get_session(session);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    /* close the mgmt depend on release all resource firstly. */
    session->state = TSR2RCIPHER_SESSION_INACTIVED;
    session->ops->release(session);

    session->ops = HI_NULL;

    osal_mutex_destory(&session->lock);
    osal_atomic_dec_return(&session->ref_count);

    tsr2rcipher_put_session(session);

    ret = HI_SUCCESS;
out:
    return ret;
}

/* general common functions. */
hi_u32 tsr2rcipher_get_queue_lenth(const hi_u32 read, const hi_u32 write, const hi_u32 size)
{
    return (read > write) ? (size + write - read) : (write - read);
}

hi_s32 tsr2rcipher_slot_init(hi_void)
{
    osal_mutex_init(&g_tsr2rcipher_slot_table.lock);

    return HI_SUCCESS;
}

hi_s32 tsr2rcipher_slot_exit(hi_void)
{
    osal_mutex_destory(&g_tsr2rcipher_slot_table.lock);

    return HI_SUCCESS;
}
