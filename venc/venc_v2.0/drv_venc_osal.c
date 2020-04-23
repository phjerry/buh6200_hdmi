/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description:  encoder
 * Author: sdk
 * Create: 2019-07-18
 */

#include <linux/interrupt.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/semaphore.h>
#include <linux/spinlock.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/rwlock.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/ion.h>
#include <linux/dma-buf.h>
#include "hi_drv_mem.h"
#include "hi_drv_dev.h"
#include "drv_venc_osal.h"
#include "drv_venc_ratecontrol.h"
#include "linux/hisilicon/securec.h"

#define VENC_OASL_ASSERT_RET(cond, ret)                            \
    do {                                                           \
        if (!(cond)) {                                             \
            HI_ERR_VENC("Assert Warning: condition %s not match.\n", __func__, __LINE__, #cond); \
            return ret;                                            \
        }                                                          \
    } while (0)

#define HI_ERR_VENC(fmt...) HI_ERR_PRINT(HI_ID_VENC, fmt)

static hi_handle g_venc_irq;

venc_osal_func_ptr g_venc_osal_func_ptr;

static hi_void (*venc_call_back)(hi_void);

static hi_void *venc_kernel_register_map(unsigned long phy_addr, unsigned long size)
{
    return ioremap (phy_addr, size);
}

static hi_void venc_kernel_register_unmap(hi_void *vir_addr)
{
    iounmap(vir_addr);
    return;
}

static hi_s32 venc_drv_osal_venc_isr(hi_s32 irq, hi_void *dev_id)
{
    (*venc_call_back)();
    return OSAL_IRQ_HANDLED;
}

hi_s32 venc_drv_osal_irq_int(hi_u32 irq, hi_void (*call_back)(hi_void))
{
    hi_s32 ret;

    if (call_back == NULL) {
        HI_ERR_VENC("parameter is invalid!\n");
        return HI_FAILURE;
    }

    venc_call_back = call_back;
    ret = osal_irq_request(irq, venc_drv_osal_venc_isr, HI_NULL, "venc", &g_venc_irq);

    return ret;
}

hi_void venc_drv_osal_irq_free(hi_u32 irq)
{
    osal_irq_free(irq, &g_venc_irq);
}

hi_void venc_drv_osal_sem_init(osal_semaphore *sem)
{
    if (osal_sem_init(sem, 1) != HI_SUCCESS) {
        HI_ERR_VENC("osal_sem_init failed!\n");
    }
}

hi_void venc_drv_osal_sem_deinit(osal_semaphore *sem)
{
    osal_sem_destory(sem);
}

hi_s32 venc_drv_osal_lock_create(osal_spinlock **lock)
{
    hi_s32 ret;
    osal_spinlock *spin_lock = NULL;

    if (lock == NULL) {
        HI_ERR_VENC("parameter is invalid!\n");
        return HI_FAILURE;
    }

    spin_lock = (osal_spinlock *)HI_KMALLOC(HI_ID_VENC, sizeof(osal_spinlock), GFP_KERNEL);
    if (spin_lock == NULL) {
        return HI_FAILURE;
    }

    ret = memset_s(spin_lock, sizeof(osal_spinlock), 0, sizeof(osal_spinlock));
    if (ret != HI_SUCCESS) {
        goto ERR_OUT;
    }

    ret = osal_spin_lock_init(spin_lock);
    if (ret < 0) {
        goto ERR_OUT;
    }

    *lock = spin_lock;

    return HI_SUCCESS;

ERR_OUT:
    HI_KFREE(HI_ID_VENC, lock);
    *lock = NULL;
    return HI_FAILURE;

}


hi_void venc_drv_osal_lock_destory(osal_spinlock *lock)
{
    if (lock != NULL) {
        osal_spin_lock_destory(lock);
        HI_KFREE(HI_ID_VENC, lock);
        lock = NULL;
    }
}

hi_void venc_drv_osal_lock(osal_spinlock *lock, vedu_lock_flag *flag)
{
    if (lock == NULL || flag == NULL) {
        HI_ERR_VENC("parameter is invalid!\n");
        return;
    }

    osal_spin_lock_irqsave(lock, flag);
}

hi_void venc_drv_osal_unlock(osal_spinlock *lock, vedu_lock_flag *flag)
{
    if (lock == NULL || flag == NULL) {
        HI_ERR_VENC("parameter is invalid!\n");
        return;
    }

    osal_spin_unlock_irqrestore(lock, flag);
}

hi_s32 venc_drv_osal_create_task(hi_void **task, hi_u8 task_name[], hi_void *task_function)
{
    osal_task *task_create = NULL;

    if (task == NULL || task_name == NULL || task_function == NULL) {
        HI_ERR_VENC("parameter is invalid!\n");
        return HI_FAILURE;
    }

    task_create = osal_kthread_create((osal_kthread_handler)task_function, NULL, task_name, 10240); /* 10240: not use */
    if (task_create == NULL) {
        HI_ERR_VENC("osal_kthread_create failed!\n");
        return HI_FAILURE;
    }

    *task = (hi_void *)task_create;

    return HI_SUCCESS;
}

hi_s32 venc_drv_osal_delete_task(hi_void *task)
{
    if (task == NULL) {
        HI_ERR_VENC("parameter is invalid!\n");
        return HI_FAILURE;
    }

    osal_kthread_destroy((osal_task *)task, HI_FALSE);   /* HI_FALSE: Don't call kthread_stop */

    return HI_SUCCESS;
}

#if 1
/************************************************************************/
/* 初始化事件                                                           */
/************************************************************************/
hi_s32 venc_drv_osal_init_event(vedu_osal_event *event, hi_s32 init_val)
{
    if (event == NULL) {
        HI_ERR_VENC("parameter is invalid!\n");
        return HI_FAILURE;
    }

    event->flag = init_val;
    if (osal_wait_init(&(event->queue_head)) < 0) {
        HI_ERR_VENC("osal_wait_init failed!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/************************************************************************/
/* 去初始化事件                                                           */
/************************************************************************/
hi_s32 venc_drv_osal_deinit_event(vedu_osal_event *event)
{
    if (event == NULL) {
        HI_ERR_VENC("parameter is invalid!\n");
        return HI_FAILURE;
    }

    osal_wait_destroy(&(event->queue_head));
    return HI_SUCCESS;
}


/************************************************************************/
/* 发出事件唤醒                                                             */
/************************************************************************/
hi_void venc_drv_osal_give_event(vedu_osal_event *event)
{
    if (event == NULL) {
        HI_ERR_VENC("parameter is invalid!\n");
        return;
    }

    event->flag = 1;
    osal_wait_wakeup(&(event->queue_head));
    return;
}

static hi_s32 event_condition_test(const hi_void *param)
{
    return *((hi_s32 *)param) != 0;
}

/************************************************************************/
/* 等待事件                                                             */
/* 事件发生返回OSAL_OK，超时返回OSAL_ERR 若condition不满足就阻塞等待    */
/* 被唤醒返回 0 ，超时返回非-1                                          */
/************************************************************************/
hi_s32 venc_drv_osal_wait_event(vedu_osal_event *event, hi_u32 wait_time_ms)
{
    hi_s32 l_ret;
    hi_s32 ret;
    if (event == NULL) {
        HI_ERR_VENC("parameter is invalid!\n");
        return HI_FAILURE;
    }

    if (wait_time_ms != 0xffffffff) {
        /* wait_time_ms/10 */
        l_ret = osal_wait_timeout_interruptible(&event->queue_head, event_condition_test, &event->flag,
            osal_msecs_to_jiffies(wait_time_ms));
        if (l_ret != 0) {
            ret = HI_SUCCESS;
        } else {
            ret = HI_FAILURE;
        }
        event->flag = 0; /* (event->flag>0)? (event->flag-1): 0; */

        return ret;
    } else {
        l_ret = osal_wait_interruptible(&event->queue_head, event_condition_test, &event->flag);
        event->flag = 0;
        return (l_ret == 0) ? HI_SUCCESS : HI_FAILURE;
    }
}
#endif

/************************************************************************/
/* 文件tell position                                                    */
/************************************************************************/
struct file *venc_drv_osal_fopen(const char *file_name, int flags, int mode)
{
    struct file *filp = NULL;

    if (file_name == NULL) {
        HI_ERR_VENC("parameter is invalid!\n");
        return NULL;
    }

    filp = (struct file *)osal_klib_fopen(file_name, flags, mode);

    return filp;
}

void venc_drv_osal_fclose(struct file *filp)
{
    if (filp == NULL) {
        HI_ERR_VENC("parameter is invalid!\n");
        return;
    }

    osal_klib_fclose(filp);
}

int venc_drv_osal_fwrite(const char *buf, int len, struct file *filp)
{
    int writelen;
    mm_segment_t oldfs;

    if (filp == NULL || buf == NULL) {
        return -ENOENT;
    }

    if (((filp->f_flags & O_ACCMODE) & (O_WRONLY | O_RDWR)) == 0) {
        return -EACCES;
    }

    oldfs = get_fs();
    set_fs(KERNEL_DS);
    writelen = vfs_write(filp, buf, len, &filp->f_pos);
    set_fs(oldfs);

    return writelen;
}

/*********************************** for SMMU begain************************************/
#ifdef HI_SMMU_SUPPORT
hi_void hi_drv_venc_get_smmu_addr(hi_ulong *smmu_page_base_addr, hi_ulong *smmu_err_read_addr,
    hi_ulong *smmu_err_write_addr)
{
    hi_ulong addr = 0;
    hi_ulong err_rdaddr = 0;
    hi_ulong err_wraddr = 0;

    hi_drv_nssmmu_get_page_table_addr((hi_ulong *)&addr, (hi_ulong *)&err_rdaddr, (hi_ulong *)&err_wraddr);

    *smmu_page_base_addr = addr;
    *smmu_err_read_addr  = err_rdaddr;
    *smmu_err_write_addr = err_wraddr;
}
#endif

void venc_caculate_mask(hi_u32 smmu, hi_u32 sec, hi_u32 *mask)
{
    if (smmu == 0 && sec == 0) {
        *mask = 1 << ION_HEAP_ID_CMA;
    } else if (smmu == 0 && sec == 1) {
        *mask = 1 << ION_HEAP_ID_SEC_CMA;
    } else if (smmu == 1 && sec == 0) {
        *mask = 1 << ION_HEAP_ID_SMMU;
    } else {
        *mask = 1 << ION_HEAP_ID_SEC_SMMU;
    }
}

hi_s32 map_vir_addr_and_fd(venc_buffer *venc_buf, struct dma_buf *buf, dma_addr_t phy_addr, hi_u32 len)
{
    hi_s32 fd;
    hi_void *vir_addr = dma_buf_kmap(buf, 0);

    dma_buf_begin_cpu_access(buf, DMA_FROM_DEVICE);

    venc_buf->start_phy_addr = (UADDR)phy_addr;
    venc_buf->start_vir_addr = (hi_u8 *)vir_addr;
    venc_buf->size = len;
    venc_buf->dma_buf = PTR_UINT64(buf);
    fd = hi_dma_buf_fd(buf, O_CLOEXEC);
    if (fd < 0) {
        HI_ERR_VENC("get dma buf fd fail\n");
        return HI_FAILURE;
    }

    venc_buf->fd = fd;
    venc_buf->skip_close_fd = HI_FALSE;

    return HI_SUCCESS;
}

hi_s32 hi_drv_venc_alloc_and_map(const char *buf_name, hi_u32 cache, hi_u32 len, hi_u32 align,
    venc_buffer *venc_buf)
{
    hi_u32 smmu;
    hi_u32 sec = 0;
    hi_u32 mask;
    hi_u32 flag;
    hi_void *priv_data = HI_NULL;
    size_t priv_len = 0;
    struct dma_buf *buf = HI_NULL;
    dma_addr_t phy_addr;

    VENC_OASL_ASSERT_RET(len != 0, HI_FAILURE);
    VENC_OASL_ASSERT_RET(venc_buf != HI_NULL, HI_FAILURE);

#ifdef HI_SMMU_SUPPORT
    smmu = 1;
#endif

#ifdef HI_TEE_SUPPORT
    if (venc_buf->mode == MEM_CMA_SEC || venc_buf->mode == MEM_MMU_SEC) {
        sec = 1;
    }
#endif

    venc_caculate_mask(smmu, sec, &mask);

    flag = cache;

    /* ion_alloc() will align page size */
    buf = hi_ion_alloc(len, mask, flag, buf_name, priv_data, priv_len);
    if (buf == HI_NULL) {
        HI_ERR_VENC("hi_ion_alloc fail\n");
        return HI_FAILURE;
    }

    if (smmu == 0 && sec == 0) {
        phy_addr = hi_dma_buf_phy_get(buf);
    } else if (smmu == 1 && sec == 0) {
        phy_addr = hi_dma_buf_nssmmu_map(buf, 0);
    } else if (smmu == 1 && sec == 1) {
        phy_addr = hi_dma_buf_secsmmu_map(buf, 0);
    }

    return map_vir_addr_and_fd(venc_buf, buf, phy_addr, len);
}

hi_s32 hi_drv_venc_unmap_and_release(venc_buffer *venc_buf)
{
    VENC_OASL_ASSERT_RET(venc_buf != HI_NULL, HI_FAILURE);
    VENC_OASL_ASSERT_RET(venc_buf->fd != 0, HI_FAILURE);

    if (venc_buf->start_vir_addr != HI_NULL) {
        dma_buf_kunmap((struct dma_buf *)UINT64_PTR(venc_buf->dma_buf), 0, venc_buf->start_vir_addr);
    }

    if (venc_buf->skip_close_fd != HI_TRUE) {
        hi_close_fd(venc_buf->fd);
    }
    hi_ion_free((struct dma_buf *)UINT64_PTR(venc_buf->dma_buf));

    return HI_SUCCESS;
}

hi_s32 hi_drv_venc_mmz_alloc_and_map(const char *buf_name, hi_u32 cache, hi_u32 len, int align,
    venc_buffer *venc_buf)
{
    hi_u32 sec = 0;
    hi_u32 mask;
    hi_u32 flag;
    hi_s32 fd;
    hi_void *priv_data = HI_NULL;
    size_t priv_len = 0;
    struct dma_buf *buf = HI_NULL;
    dma_addr_t phy_addr;
    hi_void *vir_addr = NULL;

    VENC_OASL_ASSERT_RET(len != 0, HI_FAILURE);
    VENC_OASL_ASSERT_RET(venc_buf != HI_NULL, HI_FAILURE);


#ifdef HI_TEE_SUPPORT
    if (venc_buf->mode == MEM_CMA_SEC ||
        venc_buf->mode == MEM_MMU_SEC) {
        sec = 1;
    }
#endif

    if (sec == 0) {
        mask = 1 << ION_HEAP_ID_CMA;
    } else if (sec == 1) {
        mask = 1 << ION_HEAP_ID_SEC_CMA;
    }

    flag = cache;

    /* ion_alloc() will align page size */
    buf = hi_ion_alloc(len, mask, flag, buf_name, priv_data, priv_len);
    if (buf == HI_NULL) {
        HI_ERR_VENC("hi_ion_alloc fail\n");
        return HI_FAILURE;
    }

    phy_addr = hi_dma_buf_phy_get(buf);

    vir_addr = dma_buf_kmap(buf, 0);

    dma_buf_begin_cpu_access(buf, DMA_FROM_DEVICE);

    venc_buf->start_phy_addr = (UADDR)phy_addr;
    venc_buf->start_vir_addr = (hi_u8 *)vir_addr;
    venc_buf->size = len;
    venc_buf->dma_buf = PTR_UINT64(buf);
    fd = hi_dma_buf_fd(buf, O_CLOEXEC);
    if (fd < 0) {
        HI_ERR_VENC("get dma buf fd fail\n");
        return HI_FAILURE;
    }

    venc_buf->fd = fd;
    venc_buf->skip_close_fd = HI_FALSE;

    return HI_SUCCESS;
}

hi_s32 hi_drv_venc_mmz_unmap_and_release(venc_buffer *venc_buf)
{
    hi_s32 ret;
    ret = hi_drv_venc_unmap_and_release(venc_buf);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("MMZ umap and release fail!\n");
    }

    return ret;
}

hi_s32 hi_drv_venc_map(venc_buffer *venc_buf)
{
    if (venc_buf->dma_buf == 0) {
        HI_ERR_VENC("dma_buf is NULL\n");
        return HI_FAILURE;
    }

    venc_buf->start_vir_addr = dma_buf_kmap((struct dma_buf *)UINT64_PTR(venc_buf->dma_buf), 0);
    return HI_SUCCESS;
}


hi_void hi_drv_venc_unmap(venc_buffer *venc_buf)
{
    dma_buf_kunmap((struct dma_buf *)UINT64_PTR(venc_buf->dma_buf), 0, venc_buf->start_vir_addr);
}

/* Must be called in the thread that applied for fd  */
hi_s32 hi_drv_venc_handle_map(venc_buffer *venc_buf)
{
    struct dma_buf *dma_info = HI_NULL;
    hi_u64 phy_addr;

    if (venc_buf == HI_NULL) {
        HI_ERR_VENC("venc_buf is NULL\n");
        return HI_FAILURE;
    }

    dma_info = dma_buf_get(venc_buf->fd);
    if (IS_ERR(dma_info)) {
        HI_ERR_VENC("get dma buf failed! fd = %d ret = %d\n", venc_buf->fd, PTR_ERR(dma_info));
        return HI_FAILURE;
    }

    phy_addr = hi_dma_buf_nssmmu_map(dma_info, 0);
    if (phy_addr == HI_NULL) {
        HI_ERR_VENC("get phy_addr_base failed\n");
        dma_buf_put(dma_info);
        return HI_FAILURE;
    }

    // No need to check
    venc_buf->start_vir_addr = dma_buf_kmap(dma_info, 0);
    venc_buf->start_phy_addr = phy_addr;
    venc_buf->dma_buf = (hi_u64)dma_info;

    return HI_SUCCESS;
}

hi_s32 hi_drv_venc_handle_unmap(venc_buffer *venc_buf)
{
    struct dma_buf *dma_info = HI_NULL;

    if (venc_buf == HI_NULL) {
        HI_ERR_VENC("venc_buf is NULL\n");
        return HI_FAILURE;
    }
    dma_info = (struct dma_buf *)venc_buf->dma_buf;

    if (dma_info == HI_NULL) {
        HI_ERR_VENC("smmu unmap failed!\n");
        return HI_FAILURE;
    }

    hi_dma_buf_nssmmu_unmap(dma_info, 0, 0);
    dma_buf_put(dma_info);

    venc_buf->start_vir_addr = HI_NULL;
    venc_buf->start_phy_addr = 0;
    venc_buf->dma_buf = 0;

    return HI_SUCCESS;
}

/************************************************************************/
/* Application virtual memory (may non physical continuity)                                       */
/************************************************************************/
static VOID *osal_vmalloc_vir_mem(SINT32 size)
{
    return vmalloc(size);
}
/************************************************************************/
/* release virtual memory  (may non physical continuity)                                      */
/************************************************************************/
static VOID osal_vfree_vir_mem(const VOID *p)
{
    if (p != NULL) {
        vfree(p);

        p = NULL;
    }
}
#if 0
hi_s32 hi_drv_venc_alloc(const char *bufname, char *zone_name, hi_u32 size, int align, venc_buffer *buf)
{
    hi_s32 s32Ret;
#ifndef HI_SMMU_SUPPORT
    hi_mmz_buffer mmz_buf;
    VENC_CHECK_NEQ_RET(memset_s(&mmz_buf, sizeof(hi_mmz_buffer), 0, sizeof(hi_mmz_buffer)), HI_SUCCESS, HI_FAILURE);
#else
    hi_smmu_buffer smmu_buf;
    VENC_CHECK_NEQ_RET(memset_s(&smmu_buf, sizeof(hi_smmu_buffer), 0, sizeof(hi_smmu_buffer)), HI_SUCCESS, HI_FAILURE);
#endif

    if (buf == NULL) {
        HI_ERR_VENC("parameter is invalid!\n");
        return HI_FAILURE;
    }

#ifndef HI_SMMU_SUPPORT
    s32Ret = hi_drv_mmz_alloc(bufname, zone_name, size, align, &mmz_buf);
    buf->start_vir_addr  = mmz_buf.virt_addr;
    buf->start_phy_addr  = mmz_buf.phys_addr;
    buf->size          = mmz_buf.size;
#else
    s32Ret = hi_drv_smmu_alloc(bufname, size, align, &smmu_buf);
    buf->start_vir_addr  = smmu_buf.virt_addr;
    buf->start_phy_addr  = smmu_buf.phys_addr;
    buf->size          = smmu_buf.size;
#endif

    return s32Ret;
}

hi_void hi_drv_venc_release(venc_buffer* buf)
{
#ifndef HI_SMMU_SUPPORT
    hi_mmz_buffer mmz_buf;
    VENC_CHECK_NEQ_VOID(memset_s(&mmz_buf, sizeof(hi_mmz_buffer), 0, sizeof(hi_mmz_buffer)), HI_SUCCESS);
#else
    hi_smmu_buffer smmu_buf;
    VENC_CHECK_NEQ_VOID(memset_s(&smmu_buf, sizeof(hi_smmu_buffer), 0, sizeof(hi_smmu_buffer)), HI_SUCCESS);
#endif

    if (buf == NULL) {
        HI_ERR_VENC("parameter is invalid!\n");
        return;
    }

#ifndef HI_SMMU_SUPPORT
    mmz_buf.virt_addr = buf->start_vir_addr;
    mmz_buf.phys_addr = buf->start_phy_addr;
    mmz_buf.size      = buf->size;
    hi_drv_mmz_release(&mmz_buf);
#else
    smmu_buf.virt_addr = buf->start_vir_addr;
    smmu_buf.phys_addr = buf->start_phy_addr;
    smmu_buf.size      = buf->size;
    hi_drv_smmu_release(&smmu_buf);
#endif
}
#endif
/*********************************** for SMMU end ************************************/
hi_void init_venc_interface(hi_void)
{
    g_venc_osal_func_ptr.fun_osal_printk = printk;
    g_venc_osal_func_ptr.fun_osal_mem_set = (fn_osal_mem_set)memset_s;
    g_venc_osal_func_ptr.fun_osal_mem_cpy = (fn_osal_mem_cpy)memcpy_s;
    g_venc_osal_func_ptr.fun_osal_ioremap = venc_kernel_register_map;
    g_venc_osal_func_ptr.fun_osal_iounmap = venc_kernel_register_unmap;
    g_venc_osal_func_ptr.fun_osal_alloc_vir_mem = osal_vmalloc_vir_mem;
    g_venc_osal_func_ptr.fun_osal_free_vir_mem  = osal_vfree_vir_mem;
}


