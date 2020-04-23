/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux utils, such as session management, object management, etc.
 * Author: sdk
 * Create: 2017-06-01
 */
#include <linux/kernel.h>
#include <linux/ion.h>
#include <linux/dma-buf.h>
#include <linux/syscalls.h>
#include "linux/delay.h"
#include "linux/bitmap.h"
#include "linux/io.h"
#include "linux/interrupt.h"
#include "linux/printk.h"
#include "linux/ratelimit.h"

#include "hi_drv_module.h"
#include "hi_drv_mem.h"
#include "hi_drv_proc.h"
#include "hi_drv_mem.h"
#include "hi_type.h"
#include "hi_errno.h"
#include "hi_osal.h"

#include "drv_demux_utils.h"
#include "drv_demux_func.h"
#include "drv_demux_plyfct.h"
#include "drv_demux_recfct.h"

/*
 * R Obj life control functions.
 */
static struct dmx_r_base_ops g_dmx_rbase_ops;
static DMX_TRACE_LEVEL g_dmx_trace_level = 0x0;

struct dmx_r_base_ops *get_dmx_rbase_ops(hi_void)
{
    return &g_dmx_rbase_ops;
}

static hi_s32 dmx_r_base_get_impl(struct dmx_r_base *obj)
{
    WARN_ON(&g_dmx_rbase_ops != obj->ops);
    WARN_ON(osal_atomic_read(&obj->ref_count) == 0x0);

    if (osal_atomic_inc_return(&obj->ref_count) == 0) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void dmx_r_base_put_impl(struct dmx_r_base *obj)
{
    WARN_ON(&g_dmx_rbase_ops != obj->ops);
    WARN_ON(osal_atomic_read(&obj->ref_count) == 0x0);

    if (osal_atomic_dec_return(&obj->ref_count) == 0x0) {
        hi_s32 ret = obj->release(obj);
        WARN_ON(ret != HI_SUCCESS);

        HI_DBG_DEMUX("#    robj(0x%x) released.\n", obj);
    }
}

static struct dmx_r_base_ops g_dmx_rbase_ops = {
    .get       = dmx_r_base_get_impl,
    .put       = dmx_r_base_put_impl,
};

hi_s32 dmx_r_get_raw(struct dmx_r_base *obj)
{
    return obj->ops->get(obj);
}

hi_s32 dmx_r_get(hi_handle handle, struct dmx_r_base **obj)
{
    return dmx_slot_get_robj(handle, obj);
}

hi_void dmx_r_put(struct dmx_r_base *obj)
{
    obj->ops->put(obj);
}

/*
 * slot management functions.
 */
static struct dmx_slot_table g_dmx_slot_table = {
    .slot_cnt    = DMX_MAX_SLOT_CNT,
    .slot_bitmap = {0},
};

hi_s32 dmx_slot_table_init(hi_void)
{
    hi_s32 ret;
    ret = osal_mutex_init(&g_dmx_slot_table.lock);
    if (ret != 0) {
        HI_ERR_DEMUX("slot_table init err.\n");
    }

    return ret;
}

hi_void dmx_slot_table_destory(hi_void)
{
    osal_mutex_destory(&g_dmx_slot_table.lock);
}

hi_s32 dmx_slot_create(struct dmx_r_base *obj, struct dmx_slot **slot)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_slot_table *slot_table = &g_dmx_slot_table;
    struct dmx_slot *new_slot = HI_NULL;
    hi_u32 id;

    WARN_ON(&g_dmx_rbase_ops != obj->ops);

    osal_mutex_lock(&slot_table->lock);

    id = find_first_zero_bit(slot_table->slot_bitmap, slot_table->slot_cnt);
    if (!(id < slot_table->slot_cnt)) {
        HI_ERR_DEMUX("there is no available slot now!\n");
        ret = HI_ERR_DMX_BUSY;
        goto out;
    }

    new_slot = &slot_table->table[id];

    if (osal_mutex_init(&new_slot->lock) != 0) {
        HI_ERR_DEMUX("new_slot mutex init err.\n");
        ret = HI_FAILURE;
        goto out;
    }
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

static hi_s32 _dmx_slot_find(hi_handle handle, struct dmx_slot **slot)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_slot_table *slot_table = &g_dmx_slot_table;
    unsigned long mask, *p;
    hi_u32 id;

    id = HANDLE_2_ID(handle);
    if (unlikely(!(id < slot_table->slot_cnt))) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    mask = BIT_MASK(id);
    p = ((unsigned long *)slot_table->slot_bitmap) + BIT_WORD(id);
    if (!(*p & mask)) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    *slot = &slot_table->table[id];

    ret = HI_SUCCESS;

out:
    return ret;
}

hi_s32 dmx_slot_find(hi_handle handle, struct dmx_slot **slot)
{
    hi_s32 ret;
    struct dmx_slot_table *slot_table = &g_dmx_slot_table;

    osal_mutex_lock(&slot_table->lock);

    ret = _dmx_slot_find(handle, slot);

    osal_mutex_unlock(&slot_table->lock);

    return ret;
}

hi_s32 dmx_slot_get_robj(hi_handle handle, struct dmx_r_base **obj)
{
    hi_s32 ret;
    struct dmx_slot_table *slot_table = &g_dmx_slot_table;
    struct dmx_slot *slot = HI_NULL;

    osal_mutex_lock(&slot_table->lock);

    ret = _dmx_slot_find(handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    osal_mutex_lock(&slot->lock);

    WARN_ON(&g_dmx_rbase_ops != slot->obj->ops);

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

hi_s32 dmx_slot_destroy(struct dmx_slot *slot)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_slot_table *slot_table = &g_dmx_slot_table;
    unsigned long mask, *p;
    hi_u32 id;

    id = HANDLE_2_ID(slot->handle);

    osal_mutex_lock(&slot_table->lock);

    osal_mutex_lock(&slot->lock);

    mask = BIT_MASK(id);
    p = ((unsigned long *)slot_table->slot_bitmap) + BIT_WORD(id);
    if (!(*p & mask)) {
        HI_ERR_DEMUX("invalid slot table id(%d).\n", id);
        ret = HI_ERR_DMX_INVALID_PARA;
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

/*
 * session management functions.
 */
struct session_worker {
    struct work_struct worker;
    struct dmx_slot *slot;
};

static hi_void session_worker_fn(struct work_struct *work)
{
    hi_s32 ret;
    struct session_worker *helper = container_of(work, struct session_worker, worker);

    ret = helper->slot->release(helper->slot->handle);
    WARN_ON(ret != HI_SUCCESS);
}

static hi_s32 dmx_session_add_slot_impl(struct dmx_session *session, struct dmx_slot *slot)
{
    hi_s32 ret = HI_FAILURE;
    struct obj_node_helper *new_obj = HI_NULL;

    DMX_NULL_POINTER_RETURN(slot->release);

    osal_mutex_lock(&session->lock);

    if (session->state != DMX_SESSION_ACTIVED) {
        goto out;
    }

    new_obj = HI_KZALLOC(HI_ID_DEMUX, sizeof(struct obj_node_helper), GFP_KERNEL);
    if (!new_obj) {
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
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

static hi_s32 dmx_session_del_slot_impl(struct dmx_session *session, struct dmx_slot *slot)
{
    hi_s32 ret = HI_FAILURE;
    struct list_head *node, *tmp;

    DMX_NULL_POINTER_RETURN(slot->release);

    osal_mutex_lock(&session->lock);

    list_for_each_safe(node, tmp, &session->head) {
        struct obj_node_helper *obj = list_entry(node, struct obj_node_helper, node);

        if (obj->key == (hi_void *)slot) {
            list_del(node);

            HI_KFREE(HI_ID_DEMUX, obj);

            ret = HI_SUCCESS;

            break;
        }
    }

    osal_mutex_unlock(&session->lock);

    return ret;
}

static struct dmx_session_ops g_dmx_session_ops;
static hi_void dmx_session_release_impl(struct dmx_session *session)
{
    WARN_ON(&g_dmx_session_ops != session->ops);
    WARN_ON(session->state != DMX_SESSION_INACTIVED);

    while (!list_empty(&session->head)) {
        /* revert search enry. */
        struct obj_node_helper *obj = list_last_entry(&session->head, struct obj_node_helper, node);
        struct session_worker rel_helper = {
            .slot = (struct dmx_slot *)obj->key,
        };

        DMX_NULL_POINTER_VOID(rel_helper.slot->release);

        INIT_WORK_ONSTACK(&rel_helper.worker, session_worker_fn);

        schedule_work(&rel_helper.worker);

        flush_scheduled_work();

        destroy_work_on_stack(&rel_helper.worker);
    }
}

static struct dmx_session_ops g_dmx_session_ops = {
    .add_slot = dmx_session_add_slot_impl,
    .del_slot = dmx_session_del_slot_impl,
    .release  = dmx_session_release_impl,
};

static inline hi_s32 get_session(struct dmx_session *session)
{
    WARN_ON(&g_dmx_session_ops != session->ops);
    WARN_ON(osal_atomic_read(&session->ref_count) == 0x0);

    if (osal_atomic_inc_return(&session->ref_count) == 0) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static inline hi_void put_session(struct dmx_session *session)
{
    WARN_ON(osal_atomic_read(&session->ref_count) == 0x0);

    if (osal_atomic_dec_return(&session->ref_count) == 0x0) {
        HI_DBG_DEMUX("@session(0x%x) released.\n", session);
        HI_KFREE(HI_ID_DEMUX, session);
    }
}

hi_s32 dmx_session_create(struct dmx_session **new_session)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_session *session = HI_NULL;

    session = HI_KZALLOC(HI_ID_DEMUX, sizeof(struct dmx_session), GFP_KERNEL);
    if (!session) {
        HI_ERR_DEMUX("alloc session memory failed.\n");
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out;
    }

    ret = osal_atomic_init(&session->ref_count);
    if (ret != 0) {
        HI_ERR_DEMUX("session ref init err.\n");
        goto out;
    }

    osal_atomic_set(&session->ref_count, 1);
    if (osal_mutex_init(&session->lock) != 0) {
        HI_ERR_DEMUX("session mutex init err.\n");
        ret = HI_FAILURE;
        goto out;
    }
    session->state = DMX_SESSION_ACTIVED;
    session->ops = &g_dmx_session_ops;
    INIT_LIST_HEAD(&session->head);

    *new_session = session;

    HI_DBG_DEMUX("@session(0x%x) created.\n", session);

    return HI_SUCCESS;

out:
    return ret;
}

hi_s32 dmx_session_add_slot(struct dmx_session *session, struct dmx_slot *slot)
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

hi_s32 dmx_session_del_slot(struct dmx_session *session, struct dmx_slot *slot)
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

hi_s32 dmx_session_destroy(struct dmx_session *session)
{
    hi_s32 ret;

    ret = get_session(session);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    /* close the mgmt depend on release all resource firstly. */
    session->state = DMX_SESSION_INACTIVED;
    session->ops->release(session);

    session->ops = HI_NULL;

    if (osal_atomic_dec_return(&session->ref_count) != 0) {
        HI_DBG_DEMUX("session ref_count is not 0.\n");
    }

    put_session(session);
    osal_mutex_destory(&session->lock);
    osal_atomic_destory(&session->ref_count);

    ret = HI_SUCCESS;
out:
    return ret;
}

/*
 * general common functions.
 */
hi_u32 dmx_get_queue_lenth(const hi_u32 read, const hi_u32 write, const hi_u32 size)
{
    return (read > write) ? (size + write - read) : (write - read);
}

DMX_TRACE_LEVEL dmx_drv_utils_get_trace_level(hi_void)
{
    return g_dmx_trace_level;
}

hi_void dmx_drv_utils_enable_trace_level(DMX_TRACE_LEVEL dmx_trace_level)
{
    g_dmx_trace_level = (hi_u32)g_dmx_trace_level | ((hi_u32)dmx_trace_level);

    return;
}

hi_void dmx_drv_utils_disable_trace_level(DMX_TRACE_LEVEL dmx_trace_level)
{
    g_dmx_trace_level = (hi_u32)g_dmx_trace_level & (~((hi_u32)dmx_trace_level));

    return;
}

/* control function entry by proc file system */
hi_s32 drv_dmx_osr_debug_ctrl(hi_u32 cmd, DMX_TRACE_CMD_CTRl cmd_ctrl, hi_u32 param)
{
    hi_s32 ret = HI_SUCCESS;
    switch (cmd) {
#ifdef HI_DEMUX_PROC_SUPPORT
        case DMX_TRACE_CMD_SAVE_ES: {
            if (cmd_ctrl == DMX_TRACE_CMD_START) {
                ret = dmx_play_fct_start_save_es();
                if (ret == HI_SUCCESS) {
                    HI_PRINT("begin save es\n");
                }
            } else {
                HI_PRINT("stop save es\n");
                dmx_play_fct_stop_save_es();
            }
            break;
        }

        case DMX_TRACE_CMD_SAVE_ALLTS: {
            if (cmd_ctrl == DMX_TRACE_CMD_START) {
                ret = dmx_rec_fct_start_save_all_ts(param);
                if (ret == HI_SUCCESS) {
                    HI_PRINT("begin save all ts\n");
                }
            } else {
                dmx_rec_fct_stop_save_all_ts();
                HI_PRINT("stop save all ts\n");
            }
            break;
        }

        case DMX_TRACE_CMD_SAVE_IPTS: {
            if (cmd_ctrl == DMX_TRACE_CMD_START) {
                ret = dmx_ram_port_start_save_ip_ts(param);
                if (ret == HI_SUCCESS) {
                    HI_PRINT("begin save ip ts\n");
                }
            } else {
                dmx_ram_port_stop_save_ip_ts();
                HI_PRINT("stop save ip ts\n");
            }
            break;
        }

        case DMX_TRACE_CMD_SAVE_DMXTS: {
            if (cmd_ctrl == DMX_TRACE_CMD_START) {
                ret = dmx_rec_fct_start_save_dmx_ts(param);
                if (ret == HI_SUCCESS) {
                    HI_PRINT("begin save dmx ts\n");
                }
            } else {
                dmx_rec_fct_stop_save_dmx_ts();
                HI_PRINT("stop save dmx ts\n");
            }
            break;
        }
#endif /* HI_DEMUX_PROC_SUPPORT */
        case DMX_TRACE_CMD_ALL_TRACE: {
            if (cmd_ctrl == DMX_TRACE_CMD_START) {
                HI_PRINT("start all trace\n");
                dmx_drv_utils_enable_trace_level(DMX_TRACE_MASK);
            } else {
                HI_PRINT("stop all trace\n");
                dmx_drv_utils_disable_trace_level(DMX_TRACE_MASK);
            }
            break;
        }

        case DMX_TRACE_CMD_GLB_TRACE: {
            if (cmd_ctrl == DMX_TRACE_CMD_START) {
                HI_PRINT("start glb trace\n");
                dmx_drv_utils_enable_trace_level(DMX_TRACE_GLB);
            } else {
                HI_PRINT("stop glb trace\n");
                dmx_drv_utils_disable_trace_level(DMX_TRACE_GLB);
            }
            break;
        }

        case DMX_TRACE_CMD_MGMT_TRACE: {
            if (cmd_ctrl == DMX_TRACE_CMD_START) {
                HI_PRINT("start mgmt trace\n");
                dmx_drv_utils_enable_trace_level(DMX_TRACE_MGMT);
            } else {
                HI_PRINT("stop mgmt trace\n");
                dmx_drv_utils_disable_trace_level(DMX_TRACE_MGMT);
            }
            break;
        }

        case DMX_TRACE_CMD_RAMPORT_TRACE: {
            if (cmd_ctrl == DMX_TRACE_CMD_START) {
                HI_PRINT("start ramport trace\n");
                dmx_drv_utils_enable_trace_level(DMX_TRACE_RAM_PORT);
            } else {
                HI_PRINT("stop ramport trace\n");
                dmx_drv_utils_disable_trace_level(DMX_TRACE_RAM_PORT);
            }
            break;
        }

        case DMX_TRACE_CMD_PID_TRACE: {
            if (cmd_ctrl == DMX_TRACE_CMD_START) {
                HI_PRINT("start pid trace\n");
                dmx_drv_utils_enable_trace_level(DMX_TRACE_PID);
            } else {
                HI_PRINT("stop pid trace\n");
                dmx_drv_utils_disable_trace_level(DMX_TRACE_PID);
            }
            break;
        }

        case DMX_TRACE_CMD_BAND_TRACE: {
            if (cmd_ctrl == DMX_TRACE_CMD_START) {
                HI_PRINT("start band trace\n");
                dmx_drv_utils_enable_trace_level(DMX_TRACE_BAND);
            } else {
                HI_PRINT("stop band trace\n");
                dmx_drv_utils_disable_trace_level(DMX_TRACE_BAND);
            }
            break;
        }

        default: {
            HI_PRINT("unknow command:0x%x\n", cmd);
            return HI_FAILURE;
        }
    }

    return ret;
}

/* memory functions */
#ifdef DMX_SMMU_SUPPORT
static hi_s32 dmx_alloc_buf(const hi_char *buf_name, hi_u32 buf_size, dmx_mem_info *buf_info_ptr)
{
    hi_s32 ret;
    hi_void *priv_data = HI_NULL;
    size_t priv_len = 0;
    struct dma_buf *buf = HI_NULL;
    dma_addr_t phy_addr;
    hi_mem_handle_t mem_handle;

    /* will align page size */
    buf = osal_mem_alloc(buf_name, buf_size, OSAL_NSSMMU_TYPE, priv_data, priv_len);
    if (IS_ERR_OR_NULL(buf)) {
        HI_ERR_DEMUX("osal_mem_alloc fail\n");
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out0;
    }

    phy_addr = osal_mem_nssmmu_map(buf, 0);
    if (phy_addr == 0) {
        HI_ERR_DEMUX("osal_mem_nssmmu_map fail\n");
        ret = HI_ERR_DMX_MMAP_FAILED;
        goto out1;
    }

    dma_buf_begin_cpu_access(buf, DMA_BIDIRECTIONAL);

    mem_handle = osal_mem_create_fd(buf, O_CLOEXEC);
    if (mem_handle < 0) {
        HI_ERR_DEMUX("osal_mem_create_fd fail\n");
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out2;
    }

    buf_info_ptr->handle = mem_handle;
    buf_info_ptr->dmx_buf_obj = buf;
    buf_info_ptr->buf_vir_addr = 0;
    buf_info_ptr->buf_phy_addr = phy_addr;
    buf_info_ptr->buf_size = buf_size;
    buf_info_ptr->user_map_flag = HI_FALSE;

    HI_DBG_DEMUX("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                 buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return HI_SUCCESS;

out2:
    if (hi_dma_buf_nssmmu_unmap(buf, 0, phy_addr) != 0) {
        HI_ERR_DEMUX("hi_dma_buf_nssmmu_unmap fail\n");
    }
out1:
    hi_ion_free(buf);
out0:
    return ret;
}

static hi_void dmx_release_buf(const dmx_mem_info *buf_info_ptr)
{
    hi_s32 ret;

    ret = osal_mem_nssmmu_unmap((struct dma_buf *)buf_info_ptr->dmx_buf_obj, buf_info_ptr->buf_phy_addr, 0);
    if (ret != 0) {
        HI_ERR_DEMUX("osal_mem_nssmmu_unmap fail\n");
        return;
    }

    /* close the buffer handle in the kernel */
    ret = osal_mem_close_fd(buf_info_ptr->handle);
    if (ret != HI_SUCCESS && ret != -EBADF) {
        HI_ERR_DEMUX("close buffer handle[%d] failed\n", buf_info_ptr->handle);
    }

    osal_mem_free((struct dma_buf *)buf_info_ptr->dmx_buf_obj);

    HI_DBG_DEMUX("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                 buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return;
}

hi_s32 dmx_tee_alloc_buf(const hi_char *buf_name, hi_u32 buf_size, dmx_mem_info *buf_info_ptr)
{
    hi_s32 ret;
    hi_void *priv_data = HI_NULL;
    size_t priv_len = 0;
    struct dma_buf *buf = HI_NULL;
    dma_addr_t phy_addr;
    hi_mem_handle_t mem_handle;

    /* will align page size */
    buf = osal_mem_alloc(buf_name, buf_size, OSAL_SECSMMU_TYPE, priv_data, priv_len);
    if (IS_ERR_OR_NULL(buf)) {
        HI_ERR_DEMUX("osal_mem_alloc fail\n");
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out0;
    }

    phy_addr = osal_mem_secsmmu_map(buf, 0);
    if (phy_addr == 0) {
        HI_ERR_DEMUX("osal_mem_secsmmu_map fail\n");
        ret = HI_ERR_DMX_MMAP_FAILED;
        goto out1;
    }

    dma_buf_begin_cpu_access(buf, DMA_BIDIRECTIONAL);

    mem_handle = osal_mem_create_fd(buf, O_CLOEXEC);
    if (mem_handle < 0) {
        HI_ERR_DEMUX("osal_mem_create_fd fail\n");
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out2;
    }

    buf_info_ptr->handle = mem_handle;
    buf_info_ptr->dmx_buf_obj = buf;
    buf_info_ptr->buf_vir_addr = 0;
    buf_info_ptr->buf_phy_addr = phy_addr;
    buf_info_ptr->buf_size = buf_size;
    buf_info_ptr->user_map_flag = HI_FALSE;

    HI_DBG_DEMUX("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                 buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return HI_SUCCESS;

out2:
    if (osal_mem_secsmmu_unmap(buf, phy_addr, 0) != 0) {
        HI_ERR_DEMUX("hi_dma_buf_nssmmu_unmap fail\n");
    }
out1:
    hi_ion_free(buf);
out0:
    return ret;
}

hi_void dmx_tee_release_buf(const dmx_mem_info *buf_info_ptr)
{
    hi_s32 ret;

    DMX_NULL_POINTER_VOID(buf_info_ptr);
    DMX_NULL_POINTER_VOID(buf_info_ptr->dmx_buf_obj);

    ret = osal_mem_secsmmu_unmap((struct dma_buf *)buf_info_ptr->dmx_buf_obj, buf_info_ptr->buf_phy_addr, 0);
    if (ret != 0) {
        HI_ERR_DEMUX("osal_mem_secsmmu_unmap fail\n");
        return;
    }

    /* close the buffer handle in the kernel */
    ret = osal_mem_close_fd(buf_info_ptr->handle);
    if (ret != HI_SUCCESS && ret != -EBADF) {
        HI_ERR_DEMUX("close buffer handle[%d] failed\n", buf_info_ptr->handle);
    }

    hi_ion_free((struct dma_buf *)buf_info_ptr->dmx_buf_obj);

    HI_DBG_DEMUX("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                 buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return;
}
#else
static hi_s32 dmx_alloc_buf(const hi_char *buf_name, hi_u32 buf_size, dmx_mem_info *buf_info_ptr)
{
    hi_s32 ret;
    hi_void *priv_data = HI_NULL;
    size_t priv_len = 0;
    struct dma_buf *buf = HI_NULL;
    dma_addr_t phy_addr;
    hi_mem_handle_t mem_handle;

    /* will align page size */
    buf = osal_mem_alloc(buf_name, buf_size, OSAL_MMZ_TYPE, priv_data, priv_len);
    if (IS_ERR_OR_NULL(buf)) {
        HI_ERR_DEMUX("osal_mem_alloc fail\n");
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        got out0;
    }

    phy_addr = osal_mem_phys(buf);
    if (phy_addr == 0) {
        HI_ERR_DEMUX("osal_mem_phys fail\n");
        ret = HI_ERR_DMX_MMAP_FAILED;
        goto out1;
    }

    dma_buf_begin_cpu_access(buf, DMA_BIDIRECTIONAL);

    mem_handle = osal_mem_create_fd(buf, O_CLOEXEC);
    if (mem_handle < 0) {
        HI_ERR_DEMUX("osal_mem_create_fd fail\n");
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out1;
    }

    buf_info_ptr->handle = mem_handle;
    buf_info_ptr->dmx_buf_obj = buf;
    buf_info_ptr->buf_vir_addr = 0;
    buf_info_ptr->buf_phy_addr = phy_addr;
    buf_info_ptr->buf_size = buf_size;
    buf_info_ptr->user_map_flag = HI_FALSE;

    HI_DBG_DEMUX("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                 buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return HI_SUCCESS;

out1:
    osal_mem_free(buf);
out0:
    return ret;
}

static hi_void dmx_release_buf(const dmx_mem_info *buf_info_ptr)
{
    hi_s32 ret;

    /* close the buffer handle in the kernel */
    ret = osal_mem_close_fd(buf_info_ptr->handle);
    if (ret != HI_SUCCESS && ret != -EBADF) {
        HI_ERR_DEMUX("close buffer handle[%d] failed\n", buf_info_ptr->handle);
    }

    osal_mem_alloc((struct dma_buf *)buf_info_ptr->dmx_buf_obj);

    HI_DBG_DEMUX("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                 buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return;
}

hi_s32 dmx_tee_alloc_buf(const hi_char *buf_name, hi_u32 buf_size, dmx_mem_info *buf_info_ptr)
{
    hi_s32 ret;
    hi_void *priv_data = HI_NULL;
    size_t priv_len = 0;
    struct dma_buf *buf = HI_NULL;
    dma_addr_t phy_addr;
    hi_mem_handle_t mem_handle;

    /* ion_alloc() will align page size */
    buf = osal_mem_alloc(buf_name, buf_size, OSAL_SECMMZ_TYPE, priv_data, priv_len);
    if (IS_ERR_OR_NULL(buf)) {
        HI_ERR_DEMUX("osal_mem_alloc fail\n");
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out0;
    }

    phy_addr = osal_mem_phys(buf);
    if (phy_addr == 0) {
        HI_ERR_DEMUX("osal_mem_phys fail\n");
        ret = HI_ERR_DMX_MMAP_FAILED;
        goto out0;
    }

    dma_buf_begin_cpu_access(buf, DMA_BIDIRECTIONAL);

    mem_handle = osal_mem_create_fd(buf, O_CLOEXEC);
    if (mem_handle < 0) {
        HI_ERR_DEMUX("osal_mem_create_fd fail\n");
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out1;
    }

    buf_info_ptr->handle = mem_handle;
    buf_info_ptr->dmx_buf_obj = buf;
    buf_info_ptr->buf_vir_addr = 0;
    buf_info_ptr->buf_phy_addr = phy_addr;
    buf_info_ptr->buf_size = buf_size;
    buf_info_ptr->user_map_flag = HI_FALSE;

    HI_DBG_DEMUX("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                 buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return HI_SUCCESS;

out1:
    hi_ion_free(buf);
out0:
    return ret;
}

hi_void dmx_tee_release_buf(const dmx_mem_info *buf_info_ptr)
{
    hi_s32 ret;

    DMX_NULL_POINTER_VOID(buf_info_ptr);
    DMX_NULL_POINTER_VOID(buf_info_ptr->dmx_buf_obj);

    /* close the buffer handle in the kernel */
    ret = osal_mem_close_fd(buf_info_ptr->handle);
    if (ret != HI_SUCCESS && ret != -EBADF) {
        HI_ERR_DEMUX("close buffer handle[%d] failed\n", buf_info_ptr->handle);
    }

    hi_ion_free((struct dma_buf *)buf_info_ptr->dmx_buf_obj);

    HI_DBG_DEMUX("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                 buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return;
}
#endif

static hi_s32 dmx_alloc_mmz(const hi_char *buf_name, hi_u32 buf_size, dmx_mem_info *buf_info_ptr)
{
    hi_s32 ret;
    hi_void *priv_data = HI_NULL;
    size_t priv_len = 0;
    struct dma_buf *buf = HI_NULL;
    dma_addr_t phy_addr;
    hi_mem_handle_t mem_handle;

    /* ion_alloc() will align page size */
    buf = osal_mem_alloc(buf_name, buf_size, OSAL_MMZ_TYPE, priv_data, priv_len);
    if (IS_ERR_OR_NULL(buf)) {
        HI_ERR_DEMUX("osal_mem_alloc fail\n");
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out0;
    }

    phy_addr = osal_mem_phys(buf);
    if (phy_addr == 0) {
        HI_ERR_DEMUX("osal_mem_phys fail\n");
        ret = HI_ERR_DMX_MMAP_FAILED;
        goto out1;
    }

    dma_buf_begin_cpu_access(buf, DMA_BIDIRECTIONAL);

    mem_handle = osal_mem_create_fd(buf, O_CLOEXEC);
    if (mem_handle < 0) {
        HI_ERR_DEMUX("osal_mem_create_fd fail\n");
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out1;
    }

    buf_info_ptr->handle = mem_handle;
    buf_info_ptr->dmx_buf_obj = buf;
    buf_info_ptr->buf_vir_addr = 0;
    buf_info_ptr->buf_phy_addr = phy_addr;
    buf_info_ptr->buf_size = buf_size;
    buf_info_ptr->user_map_flag = HI_FALSE;

    HI_DBG_DEMUX("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                 buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return HI_SUCCESS;

out1:
    osal_mem_free(buf);
out0:
    return ret;
}

static hi_void dmx_release_mmz(const dmx_mem_info *buf_info_ptr)
{
    hi_s32 ret;

    /* close the buffer handle in the kernel */
    ret = osal_mem_close_fd(buf_info_ptr->handle);
    if (ret != HI_SUCCESS && ret != -EBADF) {
        HI_ERR_DEMUX("close buffer handle[%d] failed\n", buf_info_ptr->handle);
    }

    osal_mem_free((struct dma_buf *)buf_info_ptr->dmx_buf_obj);

    HI_DBG_DEMUX("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                 buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return;
}

static hi_s32 dmx_map_buf(dmx_mem_info *buf_info_ptr, hi_bool is_cached)
{
    hi_void *vir_addr = HI_NULL;

    vir_addr = osal_mem_kmap((struct dma_buf *)buf_info_ptr->dmx_buf_obj, 0, is_cached);
    if (IS_ERR_OR_NULL(vir_addr)) {
        return HI_ERR_DMX_MMAP_FAILED;
    }

    buf_info_ptr->buf_vir_addr = (hi_u8 *)vir_addr;

    return HI_SUCCESS;
}

static hi_void dmx_unmap_buf(const dmx_mem_info *buf_info_ptr)
{
    osal_mem_kunmap((struct dma_buf *)buf_info_ptr->dmx_buf_obj, buf_info_ptr->buf_vir_addr, 0);

    return;
}

hi_s32 dmx_alloc_and_map_mmz(const hi_char *buf_name, hi_u32 buf_size, hi_bool is_cached, dmx_mem_info *buf_info_ptr)
{
    hi_s32 ret;

    DMX_NULL_POINTER_RETURN(buf_name);
    DMX_NULL_POINTER_RETURN(buf_info_ptr);

    ret = dmx_alloc_mmz(buf_name, buf_size, buf_info_ptr);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = dmx_map_buf(buf_info_ptr, is_cached);
    if (ret != HI_SUCCESS) {
        dmx_release_buf(buf_info_ptr);
        return ret;
    }

    HI_DBG_DEMUX("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                 buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return ret;
}

hi_void dmx_unmap_and_release_mmz(dmx_mem_info *buf_info_ptr)
{
    DMX_NULL_POINTER_VOID(buf_info_ptr);
    DMX_NULL_POINTER_VOID(buf_info_ptr->dmx_buf_obj);
    DMX_NULL_POINTER_VOID(buf_info_ptr->buf_vir_addr);

    dmx_unmap_buf(buf_info_ptr);

    dmx_release_mmz(buf_info_ptr);

    HI_DBG_DEMUX("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                 buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return;
}

hi_s32 dmx_alloc_and_map_buf(const hi_char *buf_name, hi_u32 buf_size, hi_bool is_cached, dmx_mem_info *buf_info_ptr)
{
    hi_s32 ret;

    DMX_NULL_POINTER_RETURN(buf_name);
    DMX_NULL_POINTER_RETURN(buf_info_ptr);

    ret = dmx_alloc_buf(buf_name, buf_size, buf_info_ptr);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = dmx_map_buf(buf_info_ptr, is_cached);
    if (ret != HI_SUCCESS) {
        dmx_release_buf(buf_info_ptr);
        return ret;
    }

    HI_DBG_DEMUX("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                 buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return ret;
}

hi_void dmx_unmap_and_release_buf(dmx_mem_info *buf_info_ptr)
{
    DMX_NULL_POINTER_VOID(buf_info_ptr);
    DMX_NULL_POINTER_VOID(buf_info_ptr->dmx_buf_obj);
    DMX_NULL_POINTER_VOID(buf_info_ptr->buf_vir_addr);

    dmx_unmap_buf(buf_info_ptr);

    dmx_release_buf(buf_info_ptr);

    HI_DBG_DEMUX("Handle[%#x], Phyaddr[0x%x], VirAddr[0x%lx], size[%d]\n", buf_info_ptr->handle,
                 buf_info_ptr->buf_phy_addr, buf_info_ptr->buf_vir_addr, buf_info_ptr->buf_size);

    return;
}

hi_s32 dmx_get_buf_fd_by_phyaddr(hi_u64 phy_addr, hi_u32 buf_size, hi_mem_handle_t *fd)
{
    hi_void *buf_obj = HI_NULL;

    DMX_NULL_POINTER_RETURN(fd);

    buf_obj = osal_mem_get_handle(phy_addr, buf_size, OSAL_SECSMMU_TYPE);
    if (IS_ERR_OR_NULL(buf_obj)) {
        HI_ERR_DEMUX("get buf obj failed, buf addr %llx, size %u.\n", phy_addr, buf_size);
        return HI_FAILURE;
    }

    *fd = osal_mem_create_fd(buf_obj, O_CLOEXEC);
    if (*fd < 0) {
        HI_ERR_DEMUX("get buf fd failed.\n");
        return HI_FAILURE;
    }

    osal_mem_ref_put(buf_obj, HI_ID_DEMUX);

    return HI_SUCCESS;
}

hi_s32 dmx_put_buf_fd(hi_mem_handle_t fd)
{
    return osal_mem_close_fd(fd);
}

