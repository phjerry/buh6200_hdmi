/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
* Description: tsio basic utils impl.
* Author: guoqingbo
* Create: 2016-08-12
*/

#include <linux/ion.h>
#include <linux/syscalls.h>
#include "linux/io.h"
#include "linux/hisilicon/securec.h"
#include "hi_drv_sys.h"
#include "hi_drv_mem.h"
#include "drv_tsio_utils.h"
#include "drv_tsio_func.h"
#include "drv_tsio_define.h"

/*
 * general common functions.
 */
hi_u32 tsio_get_queue_lenth(const hi_u32 read, const hi_u32 write, const hi_u32 size)
{
    return (read > write) ? (size + write - read) : (write - read);
}

/*
 * TSIO export functions.
 */
static struct tsio_session_ops g_tsio_session_ops;

static inline hi_s32 get_session(struct tsio_session *session)
{
    TSIO_FATAL_ERROR(&g_tsio_session_ops != session->ops);
    TSIO_FATAL_ERROR(osal_atomic_read(&session->ref_count) == 0);

    if (osal_atomic_inc_return(&session->ref_count) == 0) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static inline hi_void put_session(struct tsio_session *session)
{
    TSIO_FATAL_ERROR(osal_atomic_read(&session->ref_count) == 0);

    if (osal_atomic_dec_return(&session->ref_count) == 0) {
        HI_DBG_TSIO("@session(0x%x) released.\n", session);
        HI_KFREE(HI_ID_TSIO, session);
    }
}

hi_s32 tsio_session_create(struct tsio_session **new_session)
{
    hi_s32 ret;
    struct tsio_session *session = HI_NULL;

    ret = tsio_mgmt_init();
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    session = HI_KZALLOC(HI_ID_TSIO, sizeof(struct tsio_session), GFP_KERNEL);
    if (session == HI_NULL) {
        HI_ERR_TSIO("alloc session memory failed.\n");
        ret = HI_ERR_TSIO_NO_MEMORY;
        goto out1;
    }

    ret = osal_atomic_init(&session->ref_count);
    if (ret != 0) {
        HI_KFREE(HI_ID_TSIO, session);
        HI_ERR_TSIO("ref_count init error!\n");
        goto out1;
    }

    osal_atomic_set(&session->ref_count, 1);
    osal_mutex_init(&session->lock);
    session->state = TSIO_SESSION_ACTIVED;
    session->ops = &g_tsio_session_ops;
    INIT_LIST_HEAD(&session->head);

    *new_session = session;

    HI_DBG_TSIO("@session(0x%x) created.\n", session);
    return HI_SUCCESS;

out1:
    tsio_mgmt_exit();
out0:
    return ret;
}

hi_s32 tsio_session_add_slot(struct tsio_session *session, struct tsio_slot *slot)
{
    hi_s32 ret;

    ret = get_session(session);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = session->ops->add_slot(session, slot);

    put_session(session);
out:
    return ret;
}

hi_s32 tsio_session_del_slot(struct tsio_session *session, struct tsio_slot *slot)
{
    hi_s32 ret;

    ret = get_session(session);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = session->ops->del_slot(session, slot);

    put_session(session);
out:
    return ret;
}

hi_s32 tsio_session_destroy(struct tsio_session *session)
{
    hi_s32 ret;

    ret = get_session(session);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    /* close the mgmt depend on release all resource firstly. */
    session->state = TSIO_SESSION_INACTIVED;
    session->ops->release(session);

    session->ops = HI_NULL;

    ret = osal_atomic_dec_return(&session->ref_count);
    if (ret != 0) {
        HI_DBG_TSIO("ref_count is not 0.\n");
    }

    put_session(session);

    osal_atomic_destory(&session->ref_count);
    osal_mutex_destory(&session->lock);

    tsio_mgmt_exit();

    ret = HI_SUCCESS;

out:
    return ret;
}

static struct tsio_r_base_ops g_tsio_rbase_ops;

struct tsio_r_base_ops *get_tsio_r_base_ops(hi_void)
{
    return &g_tsio_rbase_ops;
}

hi_s32 tsio_slot_create(struct tsio_r_base *obj, struct tsio_slot **slot)
{
    hi_s32 ret;
    struct tsio_slot_table *slot_table = tsio_get_slot_table();
    struct tsio_slot *new_slot = HI_NULL;
    hi_u32 id;

    TSIO_FATAL_ERROR(&g_tsio_rbase_ops != obj->ops);

    osal_mutex_lock(&slot_table->lock);

    id = find_first_zero_bit(slot_table->slot_bitmap, slot_table->slot_cnt);
    if (!(id < slot_table->slot_cnt)) {
        HI_ERR_TSIO("there is no available slot now!\n");
        ret = HI_ERR_TSIO_NO_RESOURCE;
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

static hi_s32 _tsio_slot_find(hi_handle handle, struct tsio_slot **slot)
{
    hi_s32 ret;
    struct tsio_slot_table *slot_table = tsio_get_slot_table();
    unsigned long mask;
    unsigned long *p = HI_NULL;
    hi_u32 id;

    id = HANDLE_2_ID(handle);
    if (unlikely(!(id < slot_table->slot_cnt))) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    mask = BIT_MASK(id);
    p = ((unsigned long *)slot_table->slot_bitmap) + BIT_WORD(id);
    if (!(*p & mask)) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    *slot = &slot_table->table[id];

    ret = HI_SUCCESS;

out:
    return ret;
}

hi_s32 tsio_slot_find(hi_handle handle, struct tsio_slot **slot)
{
    hi_s32 ret;
    struct tsio_slot_table *slot_table = tsio_get_slot_table();

    osal_mutex_lock(&slot_table->lock);

    ret = _tsio_slot_find(handle, slot);

    osal_mutex_unlock(&slot_table->lock);

    return ret;
}

hi_s32 tsio_slot_get_robj(hi_handle handle, struct tsio_r_base **obj)
{
    hi_s32 ret;
    struct tsio_slot_table *slot_table = tsio_get_slot_table();
    struct tsio_slot *slot = HI_NULL;

    osal_mutex_lock(&slot_table->lock);

    ret = _tsio_slot_find(handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    osal_mutex_lock(&slot->lock);

    TSIO_FATAL_ERROR(&g_tsio_rbase_ops != slot->obj->ops);

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

hi_s32 tsio_slot_destroy(struct tsio_slot *slot)
{
    hi_s32 ret;
    struct tsio_slot_table *slot_table = tsio_get_slot_table();
    unsigned long mask;
    unsigned long *p = HI_NULL;
    hi_u32 id;

    id = HANDLE_2_ID(slot->handle);

    osal_mutex_lock(&slot_table->lock);

    osal_mutex_lock(&slot->lock);

    mask = BIT_MASK(id);
    p = ((unsigned long *)slot_table->slot_bitmap) + BIT_WORD(id);
    if (!(*p & mask)) {
        HI_ERR_TSIO("invalid slot table id(%d).\n", id);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    clear_bit(id, slot_table->slot_bitmap);

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&slot->lock);
    osal_mutex_destory(&slot->lock);
    osal_mutex_unlock(&slot_table->lock);

    return ret;
}

hi_s32 tsio_r_get_raw(struct tsio_r_base *obj)
{
    return obj->ops->get(obj);
}

hi_s32 tsio_r_get(hi_handle handle, struct tsio_r_base **obj)
{
    return tsio_slot_get_robj(handle, obj);
}

hi_void tsio_r_put(struct tsio_r_base *obj)
{
    obj->ops->put(obj);
}

/*
 * TSIO session functions.
 */
struct sess_worker_helper {
    struct work_struct worker;
    struct tsio_slot *slot;
};

static hi_void sess_worker_helper_fn(struct work_struct *work)
{
    hi_s32 ret;
    struct sess_worker_helper *helper = container_of(work, struct sess_worker_helper, worker);

    ret = helper->slot->release(helper->slot->handle);
    TSIO_FATAL_ERROR(ret != HI_SUCCESS);
}

static hi_s32 tsio_session_add_slot_impl(struct tsio_session *session, struct tsio_slot *slot)
{
    hi_s32 ret;
    struct obj_node_helper *new_obj = HI_NULL;

    TSIO_FATAL_ERROR(slot->release == HI_NULL);

    osal_mutex_lock(&session->lock);

    if (session->state != TSIO_SESSION_ACTIVED) {
        ret = HI_FAILURE;
        goto out;
    }

    new_obj = HI_KZALLOC(HI_ID_TSIO, sizeof(struct obj_node_helper), GFP_KERNEL);
    if (new_obj == HI_NULL) {
        ret = HI_ERR_TSIO_NO_MEMORY;
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

static hi_s32 tsio_session_del_slot_impl(struct tsio_session *session, struct tsio_slot *slot)
{
    hi_s32 ret = HI_FAILURE;
    struct list_head *node = HI_NULL;
    struct list_head *tmp = HI_NULL;

    TSIO_FATAL_ERROR(slot->release == HI_NULL);

    osal_mutex_lock(&session->lock);

    list_for_each_safe(node, tmp, &session->head) {
        struct obj_node_helper *obj = list_entry(node, struct obj_node_helper, node);

        if (obj->key == (hi_void *)slot) {
            list_del(node);

            HI_KFREE(HI_ID_TSIO, obj);

            ret = HI_SUCCESS;

            break;
        }
    }

    osal_mutex_unlock(&session->lock);

    return ret;
}

static hi_void tsio_session_release_impl(struct tsio_session *session)
{
    TSIO_FATAL_ERROR(&g_tsio_session_ops != session->ops);
    TSIO_FATAL_ERROR(session->state != TSIO_SESSION_INACTIVED);

    /*
     * it will wait work finished for each slot and no new slot add in,
     * so no need to hold the session lock.
     */
    while (!list_empty(&session->head)) {
        /* revert search enry. */
        struct obj_node_helper *obj = list_last_entry(&session->head, struct obj_node_helper, node);
        struct sess_worker_helper rel_helper = {
            .slot = (struct tsio_slot *)obj->key,
        };

        TSIO_FATAL_ERROR(rel_helper.slot->release == HI_NULL);

        INIT_WORK_ONSTACK(&rel_helper.worker, sess_worker_helper_fn);

        schedule_work(&rel_helper.worker);

        flush_scheduled_work();

        destroy_work_on_stack(&rel_helper.worker);
    }
}

static struct tsio_session_ops g_tsio_session_ops = {
    .add_slot  = tsio_session_add_slot_impl,
    .del_slot  = tsio_session_del_slot_impl,
    .release   = tsio_session_release_impl,
};

/*
 * R Base functions.
 */
static hi_s32 tsio_r_base_get_impl(struct tsio_r_base *obj)
{
    TSIO_FATAL_ERROR(&g_tsio_rbase_ops != obj->ops);
    TSIO_FATAL_ERROR(osal_atomic_read(&obj->ref_count) == 0);

    if (osal_atomic_inc_return(&obj->ref_count) == 0) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void tsio_r_base_put_impl(struct tsio_r_base *obj)
{
    TSIO_FATAL_ERROR(&g_tsio_rbase_ops != obj->ops);
    TSIO_FATAL_ERROR(osal_atomic_read(&obj->ref_count) == 0);

    if (osal_atomic_dec_return(&obj->ref_count) == 0) {
        hi_s32 ret = obj->release(obj);
        TSIO_FATAL_ERROR(ret != HI_SUCCESS);

        HI_DBG_TSIO("#    robj(0x%x) released.\n", obj);
    }
}

static struct tsio_r_base_ops g_tsio_rbase_ops = {
    .get       = tsio_r_base_get_impl,
    .put       = tsio_r_base_put_impl,
};

static struct tsio_slot_table g_tsio_slot_table = {
    .slot_cnt    = TSIO_MAX_SLOT_CNT,
    .slot_bitmap = {0},
};

struct tsio_slot_table *tsio_get_slot_table(hi_void)
{
    return &g_tsio_slot_table;
}

hi_s32 tsio_slot_table_init(hi_void)
{
    hi_s32 ret;
    ret = osal_mutex_init(&g_tsio_slot_table.lock);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("slot_table init err.\n");
    }

    return ret;
}

hi_void tsio_slot_table_destory(hi_void)
{
    osal_mutex_destory(&g_tsio_slot_table.lock);
}

/* memory functions */
#ifdef HI_SMMU_SUPPORT
static hi_s32 tsio_alloc_buf(const hi_char *buf_name, hi_u32 buf_size, hi_bool is_cached, tsio_mem_info *buf_info_ptr)
{
    hi_s32 ret;
    hi_u32 mask;
    hi_u32 flag = 0;
    hi_void *priv_data = HI_NULL;
    size_t priv_len = 0;
    struct dma_buf *buf = HI_NULL;
    dma_addr_t phy_addr;
    hi_mem_handle_t mem_handle;

    mask = 1 << ION_HEAP_ID_SMMU;
    flag |= is_cached;

    /* ion_alloc() will align page size */
    buf = hi_ion_alloc(buf_size, mask, flag, buf_name, priv_data, priv_len);
    if (IS_ERR_OR_NULL(buf)) {
        HI_ERR_TSIO("hi_ion_alloc fail\n");
        ret = HI_ERR_TSIO_NO_MEMORY;
        goto out0;
    }

    phy_addr = hi_dma_buf_nssmmu_map(buf, 0);
    if (phy_addr == 0) {
        HI_ERR_TSIO("hi_dma_buf_nssmmu_map fail\n");
        ret = HI_ERR_TSIO_NO_MEMORY;
        goto out1;
    }

    dma_buf_begin_cpu_access(buf, DMA_BIDIRECTIONAL);

    mem_handle = hi_dma_buf_fd(buf, O_CLOEXEC);
    if (mem_handle < 0) {
        HI_ERR_TSIO("hi_dma_buf_fd get fail\n");
        ret = HI_FAILURE;
        goto out2;
    }

    buf_info_ptr->handle = mem_handle;
    buf_info_ptr->tsio_buf_obj = buf;
    buf_info_ptr->buf_vir_addr = 0;
    buf_info_ptr->buf_phy_addr = phy_addr;
    buf_info_ptr->buf_size = buf_size;
    buf_info_ptr->is_usr_map = HI_FALSE;

    HI_DBG_TSIO("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return HI_SUCCESS;

out2:
    if (hi_dma_buf_nssmmu_unmap(buf, 0, phy_addr) != 0) {
        HI_ERR_TSIO("hi_dma_buf_nssmmu_unmap fail\n");
    }
out1:
    hi_ion_free(buf);
out0:
    return ret;
}

static hi_void tsio_release_buf(const tsio_mem_info *buf_info_ptr)
{
    hi_s32 ret;

    ret = hi_dma_buf_nssmmu_unmap((struct dma_buf *)buf_info_ptr->tsio_buf_obj, 0, buf_info_ptr->buf_phy_addr);
    if (ret != 0) {
        HI_ERR_TSIO("hi_dma_buf_nssmmu_unmap fail\n");
        return;
    }

    /* user space didn't mmap the memory */
    if (buf_info_ptr->is_usr_map == HI_FALSE) {
        ret = sys_close(buf_info_ptr->handle);
        if (ret != HI_SUCCESS && ret != -EBADF) {
            HI_ERR_TSIO("sys_close buffer handle[%d] failed\n", buf_info_ptr->handle);
        }
    }

    hi_ion_free((struct dma_buf *)buf_info_ptr->tsio_buf_obj);

    HI_DBG_TSIO("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return;
}
#else
static hi_s32 tsio_alloc_buf(const hi_char *buf_name, hi_u32 buf_size, hi_bool is_cached, tsio_mem_info *buf_info_ptr)
{
    hi_s32 ret;
    hi_u32 mask;
    hi_u32 flag = 0;
    hi_void *priv_data = HI_NULL;
    size_t priv_len = 0;
    struct dma_buf *buf = HI_NULL;
    dma_addr_t phy_addr;
    hi_mem_handle_t mem_handle;

    mask = 1 << ION_HEAP_ID_CMA;
    flag |= is_cached;

    /* ion_alloc() will align page size */
    buf = hi_ion_alloc(buf_size, mask, flag, buf_name, priv_data, priv_len);
    if (IS_ERR_OR_NULL(buf)) {
        HI_ERR_TSIO("hi_ion_alloc fail\n");
        ret = HI_ERR_TSIO_NO_MEMORY;
        goto out0;
    }

    phy_addr = hi_dma_buf_phy_get(buf);
    if (phy_addr == 0) {
        HI_ERR_TSIO("hi_dma_buf_phy_get fail\n");
        ret = HI_ERR_TSIO_NO_MEMORY;
        goto out1;
    }

    dma_buf_begin_cpu_access(buf, DMA_BIDIRECTIONAL);

    mem_handle = hi_dma_buf_fd(buf, O_CLOEXEC);
    if (mem_handle < 0) {
        HI_ERR_TSIO("hi_dma_buf_fd get fail\n");
        ret = HI_FAILURE;
        goto out1;
    }

    buf_info_ptr->handle = mem_handle;
    buf_info_ptr->tsio_buf_obj = buf;
    buf_info_ptr->buf_vir_addr = 0;
    buf_info_ptr->buf_phy_addr = phy_addr;
    buf_info_ptr->buf_size = buf_size;
    buf_info_ptr->is_usr_map = HI_FALSE;

    HI_DBG_TSIO("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return HI_SUCCESS;

out1:
    hi_ion_free(buf);
out0:
    return ret;
}

static hi_void tsio_release_buf(const tsio_mem_info *buf_info_ptr)
{
    hi_s32 ret;

    /* user space didn't mmap the memory */
    if (buf_info_ptr->is_usr_map == HI_FALSE) {
        ret = sys_close(buf_info_ptr->handle);
        if (ret != HI_SUCCESS && ret != -EBADF) {
            HI_ERR_TSIO("sys_close buffer handle[%d] failed\n", buf_info_ptr->handle);
        }
    }

    hi_ion_free((struct dma_buf *)buf_info_ptr->tsio_buf_obj);

    HI_DBG_TSIO("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return;
}
#endif

static hi_s32 tsio_alloc_mmz(const hi_char *buf_name, hi_u32 buf_size, hi_bool is_cached, tsio_mem_info *buf_info_ptr)
{
    hi_s32 ret;
    hi_u32 mask;
    hi_u32 flag = 0;
    hi_void *priv_data = HI_NULL;
    size_t priv_len = 0;
    struct dma_buf *buf = HI_NULL;
    dma_addr_t phy_addr;
    hi_mem_handle_t mem_handle;

    mask = 1 << ION_HEAP_ID_CMA;
    flag |= is_cached;

    /* ion_alloc() will align page size */
    buf = hi_ion_alloc(buf_size, mask, flag, buf_name, priv_data, priv_len);
    if (IS_ERR_OR_NULL(buf)) {
        HI_ERR_TSIO("hi_ion_alloc fail\n");
        ret = HI_ERR_TSIO_NO_MEMORY;
        goto out0;
    }

    phy_addr = hi_dma_buf_phy_get(buf);
    if (phy_addr == 0) {
        HI_ERR_TSIO("hi_dma_buf_phy_get fail\n");
        ret = HI_ERR_TSIO_NO_MEMORY;
        goto out1;
    }
    dma_buf_begin_cpu_access(buf, DMA_BIDIRECTIONAL);

    mem_handle = hi_dma_buf_fd(buf, O_CLOEXEC);
    if (mem_handle < 0) {
        HI_ERR_TSIO("hi_dma_buf_fd get fail\n");
        ret = HI_FAILURE;
        goto out1;
    }

    buf_info_ptr->handle = mem_handle;
    buf_info_ptr->tsio_buf_obj = buf;
    buf_info_ptr->buf_vir_addr = 0;
    buf_info_ptr->buf_phy_addr = phy_addr;
    buf_info_ptr->buf_size = buf_size;
    buf_info_ptr->is_usr_map = HI_FALSE;

    HI_DBG_TSIO("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return HI_SUCCESS;

out1:
    hi_ion_free(buf);
out0:
    return ret;
}

static hi_void tsio_release_mmz(const tsio_mem_info *buf_info_ptr)
{
    hi_s32 ret;

    /* user space didn't mmap the memory */
    if (buf_info_ptr->is_usr_map == HI_FALSE) {
        ret = sys_close(buf_info_ptr->handle);
        if (ret != HI_SUCCESS && ret != -EBADF) {
            HI_ERR_TSIO("sys_close buffer handle[%d] failed\n", buf_info_ptr->handle);
        }
    }

    hi_ion_free((struct dma_buf *)buf_info_ptr->tsio_buf_obj);

    HI_DBG_TSIO("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return;
}

static hi_s32 tsio_map_buf(tsio_mem_info *buf_info_ptr)
{
    hi_void *vir_addr = HI_NULL;

    vir_addr = dma_buf_kmap((struct dma_buf *)buf_info_ptr->tsio_buf_obj, 0);
    if (IS_ERR_OR_NULL(vir_addr)) {
        return HI_ERR_DMX_MMAP_FAILED;
    }

    buf_info_ptr->buf_vir_addr = (hi_u8 *)vir_addr;

    return HI_SUCCESS;
}

static hi_void tsio_unmap_buf(const tsio_mem_info *buf_info_ptr)
{
    dma_buf_kunmap((struct dma_buf *)buf_info_ptr->tsio_buf_obj, 0, buf_info_ptr->buf_vir_addr);

    return;
}

hi_s32 tsio_alloc_and_map_mmz(const hi_char *buf_name, hi_u32 buf_size,
    hi_bool is_cached, tsio_mem_info *buf_info_ptr)
{
    hi_s32 ret;

    if (buf_name == HI_NULL) {
        HI_ERR_TSIO("buf_name is null.\n");
        return HI_FAILURE;
    }

    if (buf_info_ptr == HI_NULL) {
        HI_ERR_TSIO("buf_info_ptr is null.\n");
        return HI_FAILURE;
    }

    ret = tsio_alloc_mmz(buf_name, buf_size, is_cached, buf_info_ptr);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = tsio_map_buf(buf_info_ptr);
    if (ret != HI_SUCCESS) {
        tsio_release_buf(buf_info_ptr);
        return ret;
    }

    HI_DBG_TSIO("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return ret;
}

hi_void tsio_unmap_and_release_mmz(tsio_mem_info *buf_info_ptr)
{
    if (buf_info_ptr == HI_NULL) {
        HI_ERR_TSIO("buf_info_ptr is null.\n");
        return;
    }

    if (buf_info_ptr->tsio_buf_obj == HI_NULL) {
        HI_ERR_TSIO("dmx_buf_obj is null.\n");
        return;
    }

    if (buf_info_ptr->buf_vir_addr == HI_NULL) {
        HI_ERR_TSIO("buf_vir_addr is null.\n");
        return;
    }

    tsio_unmap_buf(buf_info_ptr);

    tsio_release_mmz(buf_info_ptr);

    HI_DBG_TSIO("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return;
}

hi_s32 tsio_alloc_and_map_buf(const hi_char *buf_name, hi_u32 buf_size,
    hi_bool is_cached, tsio_mem_info *buf_info_ptr)
{
    hi_s32 ret;

    if (buf_name == HI_NULL) {
        HI_ERR_TSIO("buf_name is null.\n");
        return HI_FAILURE;
    }

    if (buf_info_ptr == HI_NULL) {
        HI_ERR_TSIO("buf_info_ptr is null.\n");
        return HI_FAILURE;
    }

    ret = tsio_alloc_buf(buf_name, buf_size, is_cached, buf_info_ptr);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = tsio_map_buf(buf_info_ptr);
    if (ret != HI_SUCCESS) {
        tsio_release_buf(buf_info_ptr);
        return ret;
    }

    HI_DBG_TSIO("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return ret;
}

hi_void tsio_unmap_and_release_buf(tsio_mem_info *buf_info_ptr)
{
    if (buf_info_ptr == HI_NULL) {
        HI_ERR_TSIO("buf_info_ptr is null.\n");
        return;
    }

    if (buf_info_ptr->tsio_buf_obj == HI_NULL) {
        HI_ERR_TSIO("dmx_buf_obj is null.\n");
        return;
    }

    if (buf_info_ptr->buf_vir_addr == HI_NULL) {
        HI_ERR_TSIO("buf_vir_addr is null.\n");
        return;
    }

    tsio_unmap_buf(buf_info_ptr);

    tsio_release_buf(buf_info_ptr);

    HI_DBG_TSIO("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return;
}

