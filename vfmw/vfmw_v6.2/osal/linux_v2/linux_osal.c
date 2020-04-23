/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2019-04-22
 */

#include "vfmw_osal.h"
#include "linux_osal.h"
#include "linux_proc.h"
#include "dbg.h"
#include <linux/ion.h>
#include <linux/dma-buf.h>
#ifdef VFMW_SEC_SUPPORT
#include "hi_drv_ssm.h"
#endif

#define PRN_OS PRN_ALWS

#define OS_MAX_CHAN      VFMW_CHAN_NUM
#define OS_MAX_SEMA      (OS_MAX_CHAN * 4 * 4)
#define OS_MAX_EVENT     (OS_MAX_CHAN * 4 * 1)
#define OS_MAX_ATOMIC    (OS_MAX_CHAN * 1 * 1)
#define OS_MAX_RW_LOCK   (OS_MAX_CHAN * 1 * 1)
#define OS_MAX_SPIN_LOCK (OS_MAX_CHAN * 4 * 1)
#define OS_MAX_LIST_HEAD (OS_MAX_CHAN * 1 * 1)

#define os_get_id_by_handle(handle)   ((handle) & 0xffff)
#define os_get_unid_by_handle(handle) ((handle) >> 16)
#define os_make_handle(id, unid)      ((id) + ((unid) << 16))

#define LOG_MSG_STEP_SIZE (100)
#define LOG_MSG_DFLT_NUM  (1000)
#define LOG_MSG_DFLT_SIZE (LOG_MSG_STEP_SIZE * LOG_MSG_DFLT_NUM)
#define LOG_MSG_MAX_NUM   (10000)
#define LOG_MSG_MAX_SIZE  (LOG_MSG_STEP_SIZE * LOG_MSG_MAX_NUM)

typedef struct {
    hi_bool rewind;
    hi_u32 num;
    hi_u32 max_num;
    hi_s8 *buf;
    vfmw_mem_desc mem;
    hi_s8 *msg[LOG_MSG_MAX_NUM];
} os_log;

static os_log *g_log = HI_NULL;
static osal_irq_spin_lock g_os_lock;
static osal_atomic g_os_atomic_unid;
static osal_atomic g_os_atomic_vm_num;
static osal_sema g_os_sema[OS_MAX_SEMA];
static osal_event g_os_event[OS_MAX_EVENT];

static osal_irq_spin_lock g_os_spin_lock[OS_MAX_SPIN_LOCK];

static hi_u16 g_sema_used[OS_MAX_SEMA] = { 0 };
static hi_u16 g_event_used[OS_MAX_EVENT] = { 0 };
static osal_atomic g_os_atomic[OS_MAX_ATOMIC];
static hi_u16 g_atomic_used[OS_MAX_ATOMIC] = { 0 };
static hi_u16 g_spin_lock_used[OS_MAX_SPIN_LOCK] = { 0 };

hi_void linux_spin_lock_irq(osal_irq_spin_lock *lock, hi_ulong *lock_flag);
hi_void linux_spin_unlock_irq(osal_irq_spin_lock *lock, hi_ulong *lock_flag);

/* get kernel idle obj id for obj set */
hi_s32 linux_get_idle_id(hi_u16 used[], hi_s32 num)
{
    hi_s32 id = 0;
    static hi_u16 unid = 1;
    hi_ulong lock_flag;

    linux_spin_lock_irq(&g_os_lock, &lock_flag);

    for (id = 0; id < num; id++) {
        if (used[id] == 0) {
            if (unid >= 0x7000) {
                unid = 1;
            }
            used[id] = unid++;
            break;
        }
    }

    linux_spin_unlock_irq(&g_os_lock, &lock_flag);

    if (id < num) {
        return id;
    } else {
        return -1;
    }
}

hi_u32 linux_get_time_in_ms(hi_void)
{
    hi_u32 curr_ms;
    hi_u64 sys_time;

    sys_time = osal_sched_clock();
    curr_ms = osal_div_u64(sys_time, 1000000); /* 1000000 ns to 1 ms */

    return curr_ms;
}

hi_u32 linux_get_time_in_us(hi_void)
{
    hi_u32 curr_us;
    hi_u64 sys_time;

    sys_time = osal_sched_clock();
    curr_us = osal_div_u64(sys_time, 1000); /* 1000 ns to 1 us */

    return curr_us;
}

hi_u64 linux_get_time_us64(hi_void)
{
    hi_u64 sys_time;

    sys_time = osal_sched_clock();
    sys_time = osal_div_u64(sys_time, 1000); /* 1000 ns to 1 us */

    return sys_time;
}

hi_s32 linux_get_unid(hi_void)
{
    return osal_atomic_inc_return(&g_os_atomic_unid);
}

hi_s32 linux_atomic_init(hi_s32 *handle, hi_s32 val)
{
    hi_s32 id;
    hi_s32 ret;

    id = linux_get_idle_id(g_atomic_used, OS_MAX_ATOMIC);
    if (id < 0) {
        return OSAL_ERR;
    }

    ret = osal_atomic_init(&g_os_atomic[id]);
    if (ret != 0) {
        g_atomic_used[id] = 0;
        return OSAL_ERR;
    }

    osal_atomic_set(&g_os_atomic[id], val);
    *handle = os_make_handle(id, g_atomic_used[id]);

    return OSAL_OK;
}

hi_void linux_atomic_exit(hi_s32 handle)
{
    hi_s32 id = os_get_id_by_handle(handle);
    hi_u32 unid = os_get_unid_by_handle(handle);

    if (handle == 0 ||
        id >= OS_MAX_ATOMIC ||
        g_atomic_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        osal_dump_stack();
        return;
    }

    osal_atomic_destory(&g_os_atomic[id]);
    g_atomic_used[id] = 0;
}

hi_void linux_atomic_set(hi_s32 handle, hi_s32 val)
{
    hi_s32 id = os_get_id_by_handle(handle);
    hi_u32 unid = os_get_unid_by_handle(handle);

    if (id >= OS_MAX_ATOMIC ||
        g_atomic_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        osal_dump_stack();
        return;
    }

    osal_atomic_set(&g_os_atomic[id], val);
}

hi_s32 linux_atomic_read(hi_s32 handle)
{
    hi_s32 id = os_get_id_by_handle(handle);
    hi_u32 unid = os_get_unid_by_handle(handle);

    if (id >= OS_MAX_ATOMIC ||
        g_atomic_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        osal_dump_stack();
        return -1;
    }

    return osal_atomic_read(&g_os_atomic[id]);
}

hi_s32 linux_atomic_inc_return(hi_s32 handle)
{
    hi_s32 id = os_get_id_by_handle(handle);
    hi_u32 unid = os_get_unid_by_handle(handle);

    if (id >= OS_MAX_ATOMIC ||
        g_atomic_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        osal_dump_stack();
        return -1;
    }

    return osal_atomic_inc_return(&g_os_atomic[id]);
}

hi_s32 linux_atomic_dec_return(hi_s32 handle)
{
    hi_s32 id = os_get_id_by_handle(handle);
    hi_u32 unid = os_get_unid_by_handle(handle);

    if (id >= OS_MAX_ATOMIC ||
        g_atomic_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        return -1;
    }

    return osal_atomic_dec_return(&g_os_atomic[id]);
}

hi_void *linux_create_thread(hi_void *task_func, hi_void *param, hi_char *task_name)
{
    hi_void *task;

    task = kthread_create(task_func, param, task_name);
    if (IS_ERR(task)) {
        dprint(PRN_ERROR, "can not create thread!\n");
        return HI_NULL;
    }

    wake_up_process(task);

    return task;
}

hi_s32 linux_stop_task(hi_void *param)
{
    vfmw_osal_task *task = (vfmw_osal_task *)param;

    if (!task) {
        dprint(PRN_FATAL, "param is invalid\n");
        return OSAL_ERR;
    }

    kthread_stop(task);

    return OSAL_OK;
}

hi_s32 linux_kthread_should_stop(hi_void)
{
    return kthread_should_stop();
}

hi_s32 linux_request_irq(hi_u32 irq, vfmw_irq_handler_t handler,
                         unsigned long flags, const char *name, void *dev)
{
    /* para5 (dev) must be same as last para2 (handler) of osal_free_irq */
    if (osal_irq_request(irq, (osal_irq_handler)handler, HI_NULL, name, dev) != 0) {
        dprint(PRN_ERROR, "%s request irq failed !\n", name);
        return OSAL_ERR;
    }

    return OSAL_OK;
}

hi_void linux_free_irq(hi_u32 irq, const char *name, void *dev)
{
    osal_irq_free(irq, dev);

    return;
}

hi_s32 linux_init_event(hi_s32 *handle, hi_s32 init_val)
{
    hi_s32 id;
    hi_s32 ret;
    osal_event *event = HI_NULL;

    id = linux_get_idle_id(g_event_used, OS_MAX_EVENT);
    if (id < 0) {
        return OSAL_ERR;
    }

    event = &g_os_event[id];
    event->flag = init_val;

    ret = osal_wait_init(&(event->wait));
    if (ret != 0) {
        g_event_used[id] = 0;
        return OSAL_ERR;
    }

    *handle = os_make_handle(id, g_event_used[id]);

    return OSAL_OK;
}

hi_void linux_exit_event(hi_s32 handle)
{
    hi_s32 id = os_get_id_by_handle(handle);
    hi_u32 unid = os_get_unid_by_handle(handle);
    osal_event *event = NULL;

    if (handle == 0 ||
        id >= OS_MAX_EVENT ||
        g_event_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        osal_dump_stack();
        return;
    }

    event = &g_os_event[id];
    osal_wait_destroy(&(event->wait));
    g_event_used[id] = 0;
}

hi_s32 linux_give_event(hi_s32 handle)
{
    osal_event *event = HI_NULL;
    hi_s32 id = os_get_id_by_handle(handle);
    hi_u32 unid = os_get_unid_by_handle(handle);

    if (id >= OS_MAX_EVENT ||
        g_event_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        osal_dump_stack();
        return OSAL_ERR;
    }

    event = &g_os_event[id];
    event->flag = 1;
    osal_wait_wakeup(&(event->wait));

    return OSAL_OK;
}

hi_s32 linux_wait_call_back(const hi_void *args)
{
    osal_event *event = NULL;

    event = (osal_event *)args;

    return event->flag == 0 ? 0 : 1;
}

hi_s32 linux_wait_event(hi_s32 handle, hi_s32 ms_wait_time)
{
    hi_s32 ret;
    osal_event *event = HI_NULL;
    hi_s32 id = os_get_id_by_handle(handle);
    hi_u32 unid = os_get_unid_by_handle(handle);

    if (id >= OS_MAX_EVENT ||
        g_event_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        osal_dump_stack();
        return OSAL_ERR;
    }

    event = &g_os_event[id];

    ret = osal_wait_timeout_interruptible(&event->wait,
                                          linux_wait_call_back, event, ms_wait_time);
    event->flag = 0;

    return (ret == 0) ? OSAL_OK : OSAL_ERR;
}

hi_u8 *linux_ioremap(UADDR phy_addr, hi_u32 len)
{
    return (hi_u8 *)osal_ioremap_nocache(phy_addr, len);
}

hi_void linux_iounmap(hi_u8 *vir_addr)
{
    osal_iounmap(vir_addr);
    return;
}

hi_void *linux_fopen(const char *file_name, int flags, int mode)
{
    return osal_klib_fopen(file_name, flags, mode);
}

hi_void linux_fclose(hi_void *fp)
{
    osal_klib_fclose(fp);
}

hi_s32 linux_fread(char *buf, hi_u32 len, hi_void *fp)
{
    return osal_klib_fread(buf, len, fp);
}

hi_s32 linux_fwrite(char *buf, hi_u32 len, hi_void *fp)
{
    return osal_klib_fwrite(buf, len, fp);
}

hi_s32 linux_log_on(hi_u32 msg_num)
{
    hi_u32 i;
    hi_s32 log_size;
    hi_s32 ret = OSAL_ERR;
    hi_ulong lock_flag;
    os_log *log = HI_NULL;

    log_size = LOG_MSG_DFLT_SIZE;
    if (msg_num > 0) {
        if (msg_num > LOG_MSG_MAX_NUM) {
            OS_PRINT("MsgNum(%d) out of range[1, %d].\n", msg_num, LOG_MSG_MAX_NUM);
            return OSAL_ERR;
        }
        log_size = msg_num * LOG_MSG_STEP_SIZE;
    }

    linux_spin_lock_irq(&g_os_lock, &lock_flag);
    log = g_log;
    g_log = HI_NULL;
    linux_spin_unlock_irq(&g_os_lock, &lock_flag);

    if (log != HI_NULL) {
        if (log->mem.length == log_size) {
            ret = OSAL_OK;
            goto out;
        }
        OS_KMEM_FREE(&log->mem);
        OS_FREE_VIR(log);
        log = HI_NULL;
    }

    log = OS_ALLOC_VIR("pLog", sizeof(os_log));
    if (log == HI_NULL) {
        OS_PRINT("Alloc g_log size %d failed.\n", sizeof(os_log));
        return OSAL_ERR;
    }
    VFMW_CHECK_SEC_FUNC(memset_s(log, sizeof(os_log), 0, sizeof(os_log)));

    ret = OS_KMEM_ALLOC("LogBuf", log_size, 16, 1, &log->mem); /* 16 :a number */
    if (ret != OSAL_OK ||
        log->mem.vir_addr == 0 ||
        log->mem.length < log_size) {
        OS_FREE_VIR(log);
        OS_PRINT("Alloc Log buffer size %d failed.\n", log_size);
        return OSAL_ERR;
    }

    log->buf = (hi_u8 *)(UINT64_PTR(log->mem.vir_addr));
    log->max_num = log_size / LOG_MSG_STEP_SIZE;

    for (i = 0; i < log->max_num; i++) {
        log->msg[i] = log->buf + i * LOG_MSG_STEP_SIZE;
    }

    ret = OSAL_OK;
    OS_PRINT("Turn on print log(%d) for msg num %d/size %d.\n", log_size, log->max_num, LOG_MSG_STEP_SIZE);

out:
    linux_spin_lock_irq(&g_os_lock, &lock_flag);
    g_log = log;
    linux_spin_unlock_irq(&g_os_lock, &lock_flag);

    return ret;
}

hi_void linux_log_off(hi_void)
{
    hi_ulong lock_flag;
    os_log *log = HI_NULL;

    linux_spin_lock_irq(&g_os_lock, &lock_flag);
    log = g_log;
    g_log = HI_NULL;
    linux_spin_unlock_irq(&g_os_lock, &lock_flag);

    if (log != HI_NULL && log->mem.length > 0) {
        OS_KMEM_FREE(&log->mem);
    }

    if (log != HI_NULL) {
        OS_FREE_VIR(log);
        log = HI_NULL;
        OS_PRINT("Turn off print log.\n");
    }
}

hi_s32 linux_wr_log(const char *msg, ...)
{
    va_list args;
    hi_s8 *start = HI_NULL;
    hi_s32 wr_size = 0;
    hi_ulong lock_flag;

    linux_spin_lock_irq(&g_os_lock, &lock_flag);

    if (g_log == HI_NULL || g_log->buf == HI_NULL) {
        goto out;
    }

    start = g_log->msg[g_log->num];

    va_start(args, msg);
    wr_size = vsnprintf_s(start, LOG_MSG_MAX_NUM, LOG_MSG_STEP_SIZE - 1, msg, args);
    va_end(args);

    if (wr_size < 0 || wr_size >= LOG_MSG_STEP_SIZE) {
        OS_PRINT("print log error occur.\n");
        goto out;
    }

    start[wr_size] = '\0';
    g_log->msg[g_log->num] = start;

    if (g_log->num + 1 == g_log->max_num) {
        g_log->num = 0;
        g_log->rewind = HI_TRUE;
    } else {
        g_log->num++;
    }

out:
    linux_spin_unlock_irq(&g_os_lock, &lock_flag);

    return wr_size;
}

hi_void linux_rd_log(hi_void)
{
    hi_u32 i;
    hi_u32 first_num;
    hi_u32 total_num;
    hi_ulong lock_flag;

    linux_spin_lock_irq(&g_os_lock, &lock_flag);

    if (g_log == HI_NULL || g_log->buf == HI_NULL) {
        OS_PRINT("Print log not enable.\n");
        goto out;
    }

    if (g_log->rewind == HI_FALSE) {
        first_num = 0;
        total_num = g_log->num;
    } else {
        first_num = g_log->num;
        total_num = g_log->max_num;
    }

    OS_PRINT("read log FirstNum %d TotalNum %d.\n", first_num, total_num);
    for (i = first_num; i < total_num; i++) {
        OS_PRINT("%s", g_log->msg[i]);
    }
    for (i = 0; i < first_num; i++) {
        OS_PRINT("%s", g_log->msg[i]);
    }

out:
    linux_spin_unlock_irq(&g_os_lock, &lock_flag);

    return;
}

hi_s32 linux_sema_init(hi_s32 *handle)
{
    hi_s32 id;
    hi_s32 ret;

    id = linux_get_idle_id(g_sema_used, OS_MAX_SEMA);
    if (id < 0) {
        return OSAL_ERR;
    }

    ret = osal_sem_init(&g_os_sema[id], 1);
    if (ret != 0) {
        g_sema_used[id] = 0;
        return OSAL_ERR;
    }

    *handle = os_make_handle(id, g_sema_used[id]);

    return OSAL_OK;
}

hi_void linux_sema_exit(hi_s32 handle)
{
    hi_s32 id = os_get_id_by_handle(handle);
    hi_u32 unid = os_get_unid_by_handle(handle);

    if (handle == 0 ||
        id >= OS_MAX_SEMA ||
        g_sema_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        osal_dump_stack();
        return;
    }

    osal_sem_destory(&g_os_sema[id]);
    g_sema_used[id] = 0;
}

hi_s32 linux_sema_down(hi_s32 handle)
{
    hi_s32 ret;
    hi_s32 id = os_get_id_by_handle(handle);
    hi_u32 unid = os_get_unid_by_handle(handle);

    if (id >= OS_MAX_SEMA ||
        g_sema_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        osal_dump_stack();
        return OSAL_ERR;
    }

    ret = osal_sem_down(&g_os_sema[id]);

    return ret;
}

hi_s32 linux_sema_try(hi_s32 handle)
{
    hi_s32 ret;
    hi_s32 id = os_get_id_by_handle(handle);
    hi_u32 unid = os_get_unid_by_handle(handle);

    if (id >= OS_MAX_SEMA ||
        g_sema_used[id] != unid) {
        dprint(PRN_OS, "%s: Handle = %x error \n", __func__, handle);
        osal_dump_stack();
        return OSAL_ERR;
    }

    ret = osal_sem_trydown(&g_os_sema[id]);

    return ret == 0 ? OSAL_OK : OSAL_ERR;
}

hi_void linux_sema_up(hi_s32 handle)
{
    hi_s32 id = os_get_id_by_handle(handle);
    hi_u32 unid = os_get_unid_by_handle(handle);

    if (id >= OS_MAX_SEMA ||
        g_sema_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        osal_dump_stack();
        return;
    }

    osal_sem_up(&g_os_sema[id]);
}

inline hi_s32 linux_spin_lock_irq_init(osal_irq_spin_lock *lock)
{
    hi_s32 ret;

    ret = osal_spin_lock_init(&lock->irq_lock);
    if (ret != 0) {
        return OSAL_ERR;
    }

    lock->is_init = 1;

    return OSAL_OK;
}

inline hi_void linux_spin_lock_irq(osal_irq_spin_lock *lock, hi_ulong *lock_flag)
{
    if (lock->is_init == 0) {
        osal_spin_lock_init(&lock->irq_lock);
        lock->is_init = 1;
    }
    osal_spin_lock_irqsave(&lock->irq_lock, lock_flag);

    return;
}

inline hi_void linux_spin_unlock_irq(osal_irq_spin_lock *lock, hi_ulong *lock_flag)
{
    osal_spin_unlock_irqrestore(&lock->irq_lock, lock_flag);

    return;
}

hi_void *linux_alloc_vir_mem(hi_s32 size)
{
    hi_void *vir = HI_NULL;

    VFMW_ASSERT_RET(size != 0, HI_NULL);

    vir = osal_vmalloc(HI_ID_VFMW, size);
    if (vir != HI_NULL) {
        osal_atomic_inc_return(&g_os_atomic_vm_num);
    }

    return vir;
}

hi_void linux_free_vir_mem(hi_void *p)
{
    if (p) {
        osal_vfree(HI_ID_VFMW, p);
        osal_atomic_dec_return(&g_os_atomic_vm_num);
    } else {
        dprint(PRN_OS, "%s: p = null error\n", __func__);
        osal_dump_stack();
    }
}

hi_s32 linux_get_vir_mem_num(hi_void)
{
    return osal_atomic_read(&g_os_atomic_vm_num);
}

#ifdef VFMW_MMU_SUPPORT
osal_mem_type linux_get_mem_type(vfmw_mem_mode mode)
{
    osal_mem_type type;

    switch (mode) {
        case MEM_CMA_CMA:
        case MEM_MMU_MMU:
            type = OSAL_NSSMMU_TYPE;
            break;
        case MEM_CMA_SEC:
        case MEM_MMU_SEC:
            type = OSAL_SECSMMU_TYPE;
            break;
        default:
            type = OSAL_ERROR_TYPE;
            break;
    }

    return type;
}
#else
osal_mem_type linux_get_mem_type(vfmw_mem_mode mode)
{
    osal_mem_type type;

    switch (mode) {
        case MEM_CMA_CMA:
        case MEM_MMU_MMU:
            type = OSAL_MMZ_TYPE;
            break;
        case MEM_CMA_SEC:
        case MEM_MMU_SEC:
            type = OSAL_SECMMZ_TYPE;
            break;
        default:
            type = OSAL_ERROR_TYPE;
            break;
    }

    return type;
}
#endif

hi_s32 linux_alloc_mem(hi_u8 *name, hi_u32 len, mem_record *mem)
{
    osal_mem_type type;
    hi_u32 sec = 0;
    hi_void *priv_data = HI_NULL;
#ifdef VFMW_SEC_SUPPORT
    hi_tee_ssm_buf_attach_pre_params attach_param = {0};
#endif
    size_t priv_len = 0;
    hi_void *buf = HI_NULL;
    dma_addr_t phy_addr;

    VFMW_ASSERT_RET(len != 0, OSAL_ERR);
    VFMW_ASSERT_RET(mem != HI_NULL, OSAL_ERR);

    type = linux_get_mem_type(mem->mode);

#ifdef VFMW_SEC_SUPPORT
    if (mem->mode == MEM_CMA_SEC ||
        mem->mode == MEM_MMU_SEC) {
        sec = 1;
        priv_data = &attach_param;
        priv_len = sizeof(attach_param);
    }
#endif

    if (sec == 1) {
#ifdef VFMW_SEC_SUPPORT
        attach_param.module_handle = mem->vdec_handle;
        attach_param.session_handle = mem->ssm_handle;
        if (!OS_STRNCMP(name, "es_buffer", OS_STRLEN(name))) {
            attach_param.buf_id = HI_SSM_BUFFER_ID_MCIPHER_VID_ES_BUF;
        } else if ((!OS_STRNCMP(name, "VFMW_DecBuf", strlen(name))) ||
                   (!OS_STRNCMP(name, "VFMW_DispBuf", strlen(name)))) {
            attach_param.buf_id = HI_SSM_BUFFER_ID_VID_FRM_BUF;
        } else {
            attach_param.buf_id = HI_SSM_BUFFER_ID_INTERNAL_BUF_VDEC;
        }
#endif
    }

    buf = osal_mem_alloc(name, len, type, priv_data, priv_len);
    if (buf == HI_NULL) {
        dprint(PRN_OS, "%s osal_mem_alloc fail\n", __func__);
        return OSAL_ERR;
    }

#ifdef VFMW_MMU_SUPPORT
    if (sec == 0) {
        phy_addr = osal_mem_nssmmu_map(buf, 0);
    } else {
        phy_addr = osal_mem_secsmmu_map(buf, 0);
    }
#else
    if (sec == 0) {
        phy_addr = osal_mem_phys(buf);
    }
#endif

    osal_mem_flush(buf);

    mem->phy_addr = phy_addr;
    mem->length = len;
    mem->dma_buf = PTR_UINT64(buf);

    return OSAL_OK;
}

hi_void linux_free_mem(mem_record *mem)
{
    hi_u32 sec = 0;

#ifdef VFMW_SEC_SUPPORT
    if (mem->mode == MEM_CMA_SEC ||
        mem->mode == MEM_MMU_SEC) {
        sec = 1;
    }
#endif

#ifdef VFMW_MMU_SUPPORT
    if (sec == 0) {
        osal_mem_nssmmu_unmap(UINT64_PTR(mem->dma_buf), mem->phy_addr, 0);
    } else {
        osal_mem_secsmmu_unmap(UINT64_PTR(mem->dma_buf), mem->phy_addr, 0);
    }
#endif

    osal_mem_free(UINT64_PTR(mem->dma_buf));
}

hi_s32 linux_kernel_mem_malloc(hi_char *name, hi_u32 len,
                               hi_u32 align, hi_u32 cache, vfmw_mem_desc *mem)
{
    mem_record mem_rec = {0};
    hi_void *vir_addr = HI_NULL;
    hi_u32 sec = 0;
    hi_s32 ret;

    mem_rec.mode = mem->mode;
    mem_rec.vdec_handle = mem->vdec_handle;
    mem_rec.ssm_handle = mem->ssm_handle;

    ret = linux_alloc_mem((hi_char *)name, len, &mem_rec);
    if (ret != OSAL_OK) {
        dprint(PRN_OS, "%s linux_alloc_mem fail\n", __func__);
        return ret;
    }

#ifdef VFMW_SEC_SUPPORT
    if (mem->mode == MEM_CMA_SEC ||
        mem->mode == MEM_MMU_SEC) {
        sec = 1;
    }
#endif

    if (sec == 0) {
        vir_addr = osal_mem_kmap(UINT64_PTR(mem_rec.dma_buf), 0, cache);
    } else {
        vir_addr = HI_NULL;
    }

    mem->phy_addr = mem_rec.phy_addr;
    mem->vir_addr = PTR_UINT64(vir_addr);
    mem->dma_buf = mem_rec.dma_buf;
    mem->length = mem_rec.length;

    return OSAL_OK;
}

hi_s32 linux_kernel_mem_free(vfmw_mem_desc *mem)
{
    mem_record mem_rec = {0};

    VFMW_ASSERT_RET(mem != HI_NULL, OSAL_ERR);
    VFMW_ASSERT_RET(mem->dma_buf != 0, OSAL_ERR);

    if (mem->vir_addr != HI_NULL) {
        osal_mem_kunmap(UINT64_PTR(mem->dma_buf), UINT64_PTR(mem->vir_addr), 0);
    }

    mem_rec.dma_buf = mem->dma_buf;
    mem_rec.phy_addr = mem->phy_addr;
    mem_rec.vir_addr = UINT64_PTR(mem->vir_addr);
    mem_rec.mode = mem->mode;

    linux_free_mem(&mem_rec);

    return OSAL_OK;
}

hi_u8 *linux_kernel_mmap(mem_record *mem)
{
    hi_void *vir_addr = NULL;

    VFMW_ASSERT_RET(mem != HI_NULL, HI_NULL);
    VFMW_ASSERT_RET(mem->dma_buf != 0, HI_NULL);

    vir_addr = osal_mem_kmap(UINT64_PTR(mem->dma_buf), 0, mem->is_cached);

    return vir_addr;
}

hi_u8 *linux_kernel_mmap_cache(mem_record *mem)
{
    hi_void *vir_addr = NULL;

    VFMW_ASSERT_RET(mem != HI_NULL, HI_NULL);
    VFMW_ASSERT_RET(mem->dma_buf != 0, HI_NULL);

    vir_addr = osal_mem_kmap(UINT64_PTR(mem->dma_buf), 0, mem->is_cached);

    return vir_addr;
}

hi_void linux_kernel_unmap(mem_record *mem)
{
    osal_mem_kunmap(UINT64_PTR(mem->dma_buf), mem->vir_addr, 0);
}

hi_u8 *linux_map_mem(mem_record *mem)
{
    hi_void *vir_addr = HI_NULL;

    VFMW_ASSERT_RET(mem != HI_NULL, HI_NULL);
    VFMW_ASSERT_RET(mem->dma_buf != 0, HI_NULL);

    vir_addr = osal_mem_kmap(UINT64_PTR(mem->dma_buf), 0, mem->is_cached);

    return (hi_u8 *)vir_addr;
}

hi_void linux_unmap_mem(mem_record *mem)
{
    VFMW_ASSERT(mem != HI_NULL);
    VFMW_ASSERT(mem->dma_buf != 0);

    osal_mem_kunmap(UINT64_PTR(mem->dma_buf), mem->vir_addr, 0);
}

hi_void linux_kernel_get_page_table_addr(UADDR *pt_addr, UADDR *err_rdaddr, UADDR *err_wraddr)
{
#ifdef VFMW_MMU_SUPPORT
    osal_mem_get_nssmmu_pgtinfo((hi_ulong *)pt_addr, (hi_ulong *)err_rdaddr, (hi_ulong *)err_wraddr);
#endif
}

hi_s64 linux_get_mem_fd(hi_void *buf)
{
    if (buf == HI_NULL) {
        return HI_INVALID_HANDLE;
    }

    return osal_mem_create_fd(buf, O_CLOEXEC);
}

hi_void linux_put_mem_fd(hi_s64 fd)
{
    if (fd == -1) {
        return;
    }

    osal_mem_close_fd(fd);
}

hi_void *linux_get_dma_buf(hi_s64 fd)
{
    struct dma_buf *buf;

    buf = osal_mem_handle_get(fd, HI_ID_VFMW);
    if (IS_ERR(buf)) {
        return HI_NULL;
    }

    return buf;
}

hi_void linux_put_dma_buf(hi_void *buf)
{
    if (buf == HI_NULL) {
        return;
    }

    osal_mem_ref_put(buf, HI_ID_VFMW);
}

hi_u64 linux_get_phy(hi_void *buf, hi_bool is_sec)
{
    hi_bool is_smmu = HI_FALSE;

    if (buf == HI_NULL) {
        return 0;
    }

#ifdef HI_SMMU_SUPPORT
    is_smmu = HI_TRUE;
#endif

    if (is_smmu == HI_TRUE && is_sec == HI_FALSE) {
        return osal_mem_nssmmu_map(buf, 0);
    } else if (is_smmu == HI_TRUE && is_sec == HI_TRUE) {
        return osal_mem_secsmmu_map(buf, 0);
    } else if (is_smmu == HI_FALSE && is_sec == HI_FALSE) {
        return osal_mem_phys(buf);
    } else {
        return 0;
    }
}

hi_void linux_put_phy(hi_void *buf, hi_u64 phy, hi_bool is_sec)
{
    hi_bool is_smmu = HI_FALSE;

    if (buf == HI_NULL) {
        return;
    }

#ifdef HI_SMMU_SUPPORT
    is_smmu = HI_TRUE;
#endif

    if (is_smmu == HI_TRUE && is_sec == HI_FALSE) {
        osal_mem_nssmmu_unmap(buf, phy, 0);
    } else if (is_smmu == HI_TRUE && is_sec == HI_TRUE) {
        osal_mem_secsmmu_unmap(buf, phy, 0);
    }
}

hi_void linux_flush_cache(void *ptr, UADDR phy_addr, hi_u32 len)
{
    __flush_dcache_area((void *)ptr, (size_t)len);
}

hi_s32 linux_malloc(osal_mem *mem)
{
    hi_s32 ret = 0;
    vfmw_mem_desc vfmw_mem_desc = { 0 };
    hi_void *vir = HI_NULL;
    hi_u8 *name = "vfmw_mem";

    vfmw_mem_desc.length = mem->len;

    if (mem->is_cached == 0) {
        if (mem->mem_name != HI_NULL) {
            name = mem->mem_name;
        }
        ret = linux_kernel_mem_malloc(name, mem->len, 16, 0, &vfmw_mem_desc); /* 16 :a number */
        mem->phy_addr = vfmw_mem_desc.phy_addr;
        mem->vir_addr = UINT64_PTR(vfmw_mem_desc.vir_addr);
        mem->len = vfmw_mem_desc.length;
        mem->dma_buf = vfmw_mem_desc.dma_buf;
    } else {
        vir = linux_alloc_vir_mem(mem->len);
        ret = (vir == HI_NULL) ? OSAL_ERR : OSAL_OK;
        mem->phy_addr = PTR_UINT64(vir);
        mem->vir_addr = (hi_u8 *)(vir);
    }

    return ret;
}

hi_s32 linux_free(osal_mem *mem)
{
    hi_s32 ret = 0;
    vfmw_mem_desc vfmw_mem_desc = { 0 };

    if (mem->is_cached == 0) {
        vfmw_mem_desc.dma_buf = mem->dma_buf;
        vfmw_mem_desc.length = mem->len;
        vfmw_mem_desc.vir_addr = PTR_UINT64(mem->vir_addr);
        vfmw_mem_desc.phy_addr = mem->phy_addr;
        ret = linux_kernel_mem_free(&vfmw_mem_desc);
    } else {
        linux_free_vir_mem(mem->vir_addr);
        ret = OSAL_OK;
    }

    return ret;
}

hi_s32 linux_spin_lock_init(hi_s32 *handle)
{
    hi_s32 id;
    hi_s32 ret;

    id = linux_get_idle_id(g_spin_lock_used, OS_MAX_SPIN_LOCK);
    if (id < 0) {
        return OSAL_ERR;
    }

    ret = linux_spin_lock_irq_init(&g_os_spin_lock[id]);
    if (ret != OSAL_OK) {
        g_spin_lock_used[id] = 0;
        return OSAL_ERR;
    }

    *handle = os_make_handle(id, g_spin_lock_used[id]);

    return OSAL_OK;
}

hi_void linux_spin_lock_exit(hi_s32 handle)
{
    hi_s32 id = os_get_id_by_handle(handle);
    hi_u32 unid = os_get_unid_by_handle(handle);

    if (handle == 0 ||
        id >= OS_MAX_SPIN_LOCK ||
        g_spin_lock_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        osal_dump_stack();
        return;
    }

    osal_spin_lock_destory(&g_os_spin_lock[id].irq_lock);

    g_spin_lock_used[id] = 0;
}

hi_s32 linux_spin_lock(hi_s32 handle, hi_ulong *lock_flag)
{
    hi_s32 id = os_get_id_by_handle(handle);
    hi_u32 unid = os_get_unid_by_handle(handle);

    if (id >= OS_MAX_SPIN_LOCK ||
        g_spin_lock_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        osal_dump_stack();
        return OSAL_ERR;
    }

    linux_spin_lock_irq(&g_os_spin_lock[id], lock_flag);

    return OSAL_OK;
}

hi_s32 linux_spin_unlock(hi_s32 handle, hi_ulong *lock_flag)
{
    hi_s32 id = os_get_id_by_handle(handle);
    hi_u32 unid = os_get_unid_by_handle(handle);

    if (id >= OS_MAX_SPIN_LOCK ||
        g_spin_lock_used[id] != unid) {
        dprint(PRN_OS, "%s: Handle = %x error \n", __func__, handle);
        osal_dump_stack();
        return OSAL_ERR;
    }

    linux_spin_unlock_irq(&g_os_spin_lock[id], lock_flag);

    return OSAL_OK;
}

hi_void linux_mb(hi_void)
{
    osal_mb();
}

hi_void linux_udelay(unsigned long usecs)
{
    osal_udelay(usecs);
}

static hi_u64 linux_do_div(hi_u64 dividend, hi_u32 divison)
{
    return osal_div_u64(dividend, divison);
}

extern vfmw_osal_ops g_vfmw_osal_ops;

hi_void osal_intf_init(hi_void)
{
    hi_s32 ret;
    vfmw_osal_ops *ops = &g_vfmw_osal_ops;

    VFMW_CHECK_SEC_FUNC(memset_s(ops, sizeof(vfmw_osal_ops), 0, sizeof(vfmw_osal_ops)));

    ops->check1 = 0x1234abcd;
    ops->check2 = 0x1234abcd;

    ret = linux_spin_lock_irq_init(&g_os_lock);
    if (ret != OSAL_OK) {
        dprint(PRN_OS, "%s call linux_spin_lock_irq_init failed!\n", __func__);
    }

    osal_atomic_init(&g_os_atomic_unid);
    osal_atomic_set(&g_os_atomic_unid, 1);
    osal_atomic_init(&g_os_atomic_vm_num);
    osal_atomic_set(&g_os_atomic_vm_num, 0);

    ops->get_time_in_ms = linux_get_time_in_ms;
    ops->get_time_in_us = linux_get_time_in_us;
    ops->get_time_us64 = linux_get_time_us64;
    ops->get_rand = get_random_int;
    ops->get_unid = linux_get_unid;
    ops->atomic_init = linux_atomic_init;
    ops->atomic_exit = linux_atomic_exit;
    ops->atomic_set_ = linux_atomic_set;
    ops->atomic_read_ = linux_atomic_read;
    ops->atomic_inc_ret = linux_atomic_inc_return;
    ops->atomic_dec_ret = linux_atomic_dec_return;
    ops->os_spin_lock_init = linux_spin_lock_init;
    ops->os_spin_lock_exit = linux_spin_lock_exit;
    ops->os_spin_lock = linux_spin_lock;
    ops->os_spin_unlock = linux_spin_unlock;
    ops->sema_init = linux_sema_init;
    ops->sema_exit = linux_sema_exit;
    ops->sema_down = linux_sema_down;
    ops->sema_try = linux_sema_try;
    ops->sema_up = linux_sema_up;
    ops->file_open = linux_fopen;
    ops->file_close = linux_fclose;
    ops->file_read = linux_fread;
    ops->file_write = linux_fwrite;
    ops->os_memcmp = (fn_osal_memcmp)memcmp;
    ops->log_on = linux_log_on;
    ops->log_off = linux_log_off;
    ops->rd_log = linux_rd_log;
    ops->wr_log = linux_wr_log;
#ifndef HI_ADVCA_FUNCTION_RELEASE
    ops->print = osal_printk;
#endif
    ops->strncmp = (fn_osal_strncmp)strncmp;
    ops->strlen = (fn_osal_strlen)strlen;
    ops->strlcpy = (fn_osal_strlcpy)strlcpy;
    ops->strsep = (fn_osal_strsep)strsep;
    ops->simple_strtol = (fn_osal_simple_strtol)simple_strtol;
    ops->strstr = (fn_osal_strstr)strstr;
    ops->msleep = osal_msleep_uninterruptible;
    ops->os_mb = linux_mb;
    ops->udelay = linux_udelay;
    ops->event_init = linux_init_event;
    ops->event_exit = linux_exit_event;
    ops->event_give = linux_give_event;
    ops->event_wait = linux_wait_event;
    ops->flush_cache = linux_flush_cache;
    ops->mem_alloc = linux_kernel_mem_malloc;
    ops->mem_free = linux_kernel_mem_free;
    ops->reg_map = linux_ioremap;
    ops->reg_unmap = linux_iounmap;
    ops->mmap = linux_kernel_mmap;
    ops->munmap = linux_kernel_unmap;
    ops->mmap_cache = linux_kernel_mmap_cache;
    ops->get_page_table_addr = linux_kernel_get_page_table_addr;
    ops->create_thread = linux_create_thread;
    ops->stop_thread = linux_stop_task;
    ops->thread_should_stop = linux_kthread_should_stop;
    ops->phys_to_virt = (fn_osal_phys_to_virt)osal_phys_to_virt;
    ops->dump_stack = osal_dump_stack;
    ops->alloc_vir_mem = linux_alloc_vir_mem;
    ops->free_vir_mem = linux_free_vir_mem;
    ops->get_vir_mem_num = linux_get_vir_mem_num;
    ops->request_irq = linux_request_irq;
    ops->free_irq = linux_free_irq;
    ops->proc_init = (fn_osal_proc_init)linux_proc_init;
    ops->proc_exit = (fn_osal_proc_exit)linux_proc_exit;
#ifdef VFMW_PROC_SUPPORT
    ops->proc_create = (fn_osal_proc_create)linux_proc_create;
    ops->proc_destroy = (fn_osal_proc_destroy)linux_proc_destroy;
#endif
    ops->dump_proc = (fn_osal_dump_proc)linux_proc_dump;
    ops->os_div = linux_do_div;
    ops->malloc = linux_malloc;
    ops->free = linux_free;
    ops->alloc_mem = linux_alloc_mem;
    ops->free_mem = linux_free_mem;
    ops->map_mem = linux_map_mem;
    ops->unmap_mem = linux_unmap_mem;
    ops->get_mem_fd = linux_get_mem_fd;
    ops->put_mem_fd = linux_put_mem_fd;
    ops->get_mem_phy = linux_get_phy;
    ops->put_mem_phy = linux_put_phy;
    ops->get_dma_buf = linux_get_dma_buf;
    ops->put_dma_buf = linux_put_dma_buf;
}

hi_void osal_intf_exit(hi_void)
{
    linux_log_off();
    osal_atomic_destory(&g_os_atomic_unid);
    osal_atomic_destory(&g_os_atomic_vm_num);
    osal_spin_lock_destory(&g_os_lock.irq_lock);
    VFMW_CHECK_SEC_FUNC(memset_s(&g_vfmw_osal_ops, sizeof(vfmw_osal_ops), 0, sizeof(vfmw_osal_ops)));
}


