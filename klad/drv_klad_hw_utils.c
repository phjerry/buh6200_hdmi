/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
* Description: klad basic utils impl.
* Author: SDK team
* Create: 2019-09-17
*/

#include <linux/kernel.h>
#include "linux/delay.h"
#include "linux/bitmap.h"
#include "linux/io.h"
#include "linux/interrupt.h"
#include "linux/ratelimit.h"

#include "hi_drv_sys.h"
#include "hi_drv_mem.h"

#include "drv_klad_hw_utils.h"
#include "drv_klad_hw_func.h"
#include "drv_klad_hw_define.h"
#include "linux/hisilicon/securec.h"
#include "drv_klad_sw_func.h"

/*
 * KLAD export functions.
 */
static struct klad_hw_session_ops g_klad_hw_session_ops;

static inline hi_s32 get_hw_session(struct klad_hw_session *hw_session)
{
    WARN_ON(&g_klad_hw_session_ops != hw_session->ops);
    WARN_ON(atomic_read(&hw_session->ref_count) == 0);

    if (!atomic_inc_not_zero(&hw_session->ref_count)) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static inline hi_void put_hw_session(struct klad_hw_session *hw_session)
{
    WARN_ON(atomic_read(&hw_session->ref_count) == 0);

    if (atomic_dec_and_test(&hw_session->ref_count)) {
        HI_DBG_KLAD("@hw_session(0x%p) released.\n", hw_session);
        HI_KFREE(HI_ID_KLAD, hw_session);
        hw_session = HI_NULL;
    }
}

hi_s32 klad_hw_session_create(struct klad_hw_session **new_hw_session)
{
    hi_s32 ret;
    struct klad_hw_session *hw_session = HI_NULL;

    WARN_ON(new_hw_session == HI_NULL);

    ret = klad_slot_mgmt_init();
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    hw_session = osal_kmalloc(HI_ID_KLAD, sizeof(struct klad_hw_session), OSAL_GFP_KERNEL);
    if (hw_session == HI_NULL) {
        HI_ERR_KLAD("alloc hw_session memory failed.\n");
        ret = HI_ERR_KLAD_NO_MEMORY;
        goto out1;
    }

    ret = memset_s(hw_session, sizeof(struct klad_hw_session), 0, sizeof(struct klad_hw_session));
    if (ret != EOK) {
        HI_ERR_KLAD("Call memset_s failed.\n");
        ret = HI_FAILURE;
        goto out1;
    }

    atomic_set(&hw_session->ref_count, 1);
    osal_mutex_init(&hw_session->lock);
    hw_session->state = KLAD_HW_SESSION_ACTIVED;
    hw_session->ops = &g_klad_hw_session_ops;
    INIT_LIST_HEAD(&hw_session->head);

    *new_hw_session = hw_session;

    HI_DBG_KLAD("@hw_session(0x%p) created.\n", hw_session);

    return HI_SUCCESS;

out1:
    klad_slot_mgmt_exit();
out0:
    return ret;
}

hi_s32 klad_hw_session_add_slot(struct klad_hw_session *hw_session, struct klad_r_base *slot)
{
    hi_s32 ret;

    WARN_ON(hw_session == HI_NULL);
    WARN_ON(slot == HI_NULL);

    ret = get_hw_session(hw_session);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = hw_session->ops->add_slot(hw_session, slot);

    put_hw_session(hw_session);
out:
    return ret;
}

hi_s32 klad_hw_session_del_slot(struct klad_hw_session *hw_session, struct klad_r_base *slot)
{
    hi_s32 ret;

    WARN_ON(hw_session == HI_NULL);
    WARN_ON(slot == HI_NULL);

    ret = get_hw_session(hw_session);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = hw_session->ops->del_slot(hw_session, slot);

    put_hw_session(hw_session);
out:
    return ret;
}

hi_s32 klad_hw_session_destroy(struct klad_hw_session *hw_session)
{
    hi_s32 ret;

    WARN_ON(hw_session == HI_NULL);

    ret = get_hw_session(hw_session);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    /* close the mgmt depend on release all resource firstly. */
    hw_session->state = KLAD_HW_SESSION_INACTIVED;
    hw_session->ops->release(hw_session);

    hw_session->ops = HI_NULL;

    atomic_dec(&hw_session->ref_count);

    put_hw_session(hw_session);

    klad_slot_mgmt_exit();

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 klad_hw_session_add_slot_impl(struct klad_hw_session *hw_session, struct klad_r_base *slot)
{
    hi_s32 ret;
    struct obj_node_helper *new_obj = HI_NULL;

    WARN_ON(slot->release == HI_NULL);

    __mutex_lock(&hw_session->lock);

    if (hw_session->state != KLAD_HW_SESSION_ACTIVED) {
        ret = HI_FAILURE;
        goto out;
    }
    new_obj = osal_kmalloc(HI_ID_KLAD, sizeof(struct obj_node_helper), OSAL_GFP_KERNEL);
    if (new_obj == HI_NULL) {
        ret = HI_ERR_TSIO_NO_MEMORY;
        goto out;
    }

    ret = memset_s(new_obj, sizeof(struct obj_node_helper), 0, sizeof(struct obj_node_helper));
    if (ret != EOK) {
        HI_ERR_KLAD("Call memset_s failed.\n");
        ret = HI_FAILURE;
        goto out;
    }

    new_obj->key = (hi_void *)slot;
    slot->hw_session = hw_session;

    list_add_tail(&new_obj->node, &hw_session->head);

    ret = HI_SUCCESS;

out:
    __mutex_unlock(&hw_session->lock);

    return ret;
}

static hi_s32 klad_hw_session_del_slot_impl(struct klad_hw_session *hw_session, struct klad_r_base *slot)
{
    hi_s32 ret = HI_FAILURE;
    struct list_head *node = HI_NULL;
    struct list_head *tmp = HI_NULL;

    WARN_ON(slot->release == HI_NULL);

    __mutex_lock(&hw_session->lock);

    list_for_each_safe(node, tmp, &hw_session->head) {
        struct obj_node_helper *obj = list_entry(node, struct obj_node_helper, node);
        if (obj->key == (hi_void *)slot) {
            list_del(node);
            HI_KFREE(HI_ID_KLAD, obj);
            obj = HI_NULL;
            ret = HI_SUCCESS;
            break;
        }
    }

    __mutex_unlock(&hw_session->lock);

    return ret;
}

static hi_void klad_hw_session_release_impl(struct klad_hw_session *hw_session)
{
    struct list_head *node = HI_NULL;
    struct list_head *tmp = HI_NULL;

    WARN_ON(&g_klad_hw_session_ops != hw_session->ops);
    WARN_ON(hw_session->state != KLAD_HW_SESSION_INACTIVED);

    __mutex_lock(&hw_session->lock);

    list_for_each_safe(node, tmp, &hw_session->head) {
        struct obj_node_helper *obj = list_entry(node, struct obj_node_helper, node);
        klad_r_put((struct klad_r_base *)obj->key);
    }

    __mutex_unlock(&hw_session->lock);
}

static struct klad_hw_session_ops g_klad_hw_session_ops = {
    .add_slot  = klad_hw_session_add_slot_impl,
    .del_slot  = klad_hw_session_del_slot_impl,
    .release   = klad_hw_session_release_impl,
};

