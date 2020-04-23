/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: OS Abstract Layer.
* Author: p00370051
* Create: 2013/7/25
* Notes:
* History: 2019-03-29 p00370051 CSEC
*/

#ifndef __HI_OSAL_H__
#define __HI_OSAL_H__

#define HI_OSAL_VERSION    "2.0"

#include "osal_list.h"

#define OSAL_GFP_KERNEL    0x24000c0
#define OSAL_GFP_ATOMIC    0x2080020
#define OSAL_GFP_DMA       0x1
#define OSAL_GFP_ZERO      0x8000

typedef enum {
    OSAL_MMZ_TYPE,       /* non secure cma/mmz */
    OSAL_NSSMMU_TYPE,    /* non secure smmu  */
    OSAL_SECSMMU_TYPE,   /* secure smmu  */
    OSAL_SECMMZ_TYPE,    /* secure cma/mmz */
    OSAL_ERROR_TYPE,     /* no support mem type */
} osal_mem_type;

/*
 * alloc mem
 * buf_name: input,mem name
 * len: input, mem len
 * mem_type: input,memtype:cma smmu,sec-cma,sec-smmu
 * priv_data input,private data
 * priv_len: input,private data len
 * return:
 *      buffer_handle: phys_addr (liteOS)  , dmabuf (linux)
 *
 */
void *osal_mem_alloc(const char *buf_name, unsigned long len, osal_mem_type mem_type,
                     void *priv_data, unsigned long priv_len);

/* free mem */
void osal_mem_free(void *handle);

/* map cpu addr  */
void *osal_mem_kmap(void *handle, unsigned long offset, int cache);

/* unmap cpu addr  */
void osal_mem_kunmap(void *handle, void *virt, unsigned long offset);

/* map nssmmu addr */
unsigned long osal_mem_nssmmu_map(void *handle, unsigned long offset);

/* unmap nssmmu addr */
int osal_mem_nssmmu_unmap(void *handle, unsigned long nssmmu, unsigned long offset);

/* map secsmmu addr */
unsigned long osal_mem_secsmmu_map(void *handle, unsigned long offset);

/* unmap secsmmu */
int osal_mem_secsmmu_unmap(void *handle, unsigned long secsmmu, unsigned long offset);

/* get phys_addr by handle */
unsigned long osal_mem_phys(void *handle);

/* create fd  */
long osal_mem_create_fd(void *handle, int flags);

/* close fd  */
int osal_mem_close_fd(long fd);

/*
 * The osal_mem_handle_get is to get handle by fd and
 * handle refcount increased at the same time. At last,
 * the refcount should be decreased by osal_mem_ref_put().
 */
void *osal_mem_handle_get(long fd, unsigned int module_id);

/* mem ref ++  */
int osal_mem_ref_get(void *handle, unsigned int module_id);

/* mem ref --  */
void osal_mem_ref_put(void *handle, unsigned int module_id);

/* flush mem */
void osal_mem_flush(void *handle);

/* check buffer attr */
osal_mem_type osal_mem_get_attr(void *handle);

/* get mem handle by addr */
void *osal_mem_get_handle(unsigned long addr, unsigned long size, osal_mem_type mem_type);

/* get nssmmu pgt info  */
void osal_mem_get_nssmmu_pgtinfo(unsigned long *pt_addr, unsigned long *err_rd_addr,
                                 unsigned long *err_wr_addr);


void *osal_kmalloc(unsigned int module_id, unsigned long size, unsigned int osal_gfp_flag);
void osal_kfree(unsigned int module_id, const void *addr);
void *osal_vmalloc(unsigned int module_id, unsigned long size);
void osal_vfree(unsigned int module_id, const void *addr);

// atomic api
typedef struct osal_atomic_ {
    void *atomic;
} osal_atomic;

int osal_atomic_init(osal_atomic *atomic);
void osal_atomic_destory(osal_atomic *atomic);
int osal_atomic_read(osal_atomic *v);
void osal_atomic_set(osal_atomic *v, int i);
int osal_atomic_inc_return(osal_atomic *v);
int osal_atomic_dec_return(osal_atomic *v);

// semaphore api
typedef struct osal_semaphore_ {
    void *sem;
} osal_semaphore;

int osal_sem_init(osal_semaphore *sem, int val);
int osal_sem_down(osal_semaphore *sem);
int osal_sem_down_interruptible(osal_semaphore *sem);
int osal_sem_trydown(osal_semaphore *sem);
void osal_sem_up(osal_semaphore *sem);
// notice:must be called when kmod exit, other wise will lead to memory leak;
void osal_sem_destory(osal_semaphore *sem);

// mutex api
typedef struct osal_mutex_ {
    void *mutex;
} osal_mutex;

int osal_mutex_init(osal_mutex *mutex);
int osal_mutex_lock(osal_mutex *mutex);
int osal_mutex_lock_interruptible(osal_mutex *mutex);
int osal_mutex_trylock(osal_mutex *mutex);
void osal_mutex_unlock(osal_mutex *mutex);
int osal_mutex_is_locked(osal_mutex *mutex);
// notice:must be called when kmod exit, other wise will lead to memory leak;
void osal_mutex_destory(osal_mutex *mutex);

// spin lock api
typedef struct osal_spinlock_ {
    void *lock;
} osal_spinlock;

int osal_spin_lock_init(osal_spinlock *lock);
void osal_spin_lock(osal_spinlock *lock);
int osal_spin_trylock(osal_spinlock *lock);
void osal_spin_unlock(osal_spinlock *lock);
void osal_spin_lock_irqsave(osal_spinlock *lock, unsigned long *flags);
void osal_spin_unlock_irqrestore(osal_spinlock *lock, unsigned long *flags);
// notice:must be called when kmod exit, other wise will lead to memory leak;
void osal_spin_lock_destory(osal_spinlock *lock);

// wait api
typedef int (*osal_wait_condition_func)(const void *param);

typedef struct osal_wait_ {
    void *wait;
} osal_wait;
#define ERESTARTSYS        512

int osal_wait_init(osal_wait *wait);
int osal_wait_interruptible(osal_wait *wait, osal_wait_condition_func func, const void *param);
int osal_wait_uninterruptible(osal_wait *wait, osal_wait_condition_func func, const void *param);
int osal_wait_timeout_interruptible(osal_wait *wait,
    osal_wait_condition_func func, const void *param, unsigned long ms);
int osal_wait_timeout_uninterruptible(osal_wait *wait,
    osal_wait_condition_func func, const void *param, unsigned long ms);
void osal_wait_wakeup(osal_wait *wait);  // same as wake_up_all
void osal_wait_destroy(osal_wait *wait);
unsigned long osal_msecs_to_jiffies(const unsigned int m);

// workqueue api
typedef struct osal_workqueue_ {
    void *work;
    void (*handler)(struct osal_workqueue_ *workqueue);
} osal_workqueue;
typedef void (*osal_workqueue_handler)(osal_workqueue *workqueue);

int osal_workqueue_init(osal_workqueue *work, osal_workqueue_handler handler);
int osal_workqueue_schedule(osal_workqueue *work);
void osal_workqueue_destroy(osal_workqueue *work);
int osal_workqueue_flush(osal_workqueue *work);

// timer
typedef struct osal_timer_ {
    void *timer;
    void (*handler)(unsigned long);
    unsigned long data;
} osal_timer;

typedef struct osal_timeval_ {
    long tv_sec;
    long tv_usec;
} osal_timeval;

int osal_timer_init(osal_timer *timer);
int osal_timer_set(osal_timer *timer, unsigned long interval);  // ms
int osal_timer_del(osal_timer *timer);
int osal_timer_destory(osal_timer *timer);

unsigned long osal_msleep(unsigned int msecs);
void osal_msleep_uninterruptible(unsigned int msecs);
void osal_udelay(unsigned int usecs);
void osal_mdelay(unsigned int msecs);
unsigned long long osal_sched_clock(void);
unsigned int osal_get_tickcount(void);
unsigned long long osal_get_jiffies(void);
void osal_get_timeofday(osal_timeval *tv);
int osal_time_after(unsigned long a, unsigned long b); // if a after b, return true
int osal_time_before(unsigned long a, unsigned long b);
int osal_get_pid(void);

typedef struct osal_rtc_time_ {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
} osal_rtc_time;

/* Return values for the timer callback function */
typedef enum hiOSAL_HRTIMER_RESTART_E {
    OSAL_HRTIMER_NORESTART, /* < The timer will not be restarted. */
    OSAL_HRTIMER_RESTART /* < The timer must be restarted. */
} osal_hrtimer_restart;

/* hrtimer struct */
typedef struct osal_hrtimer {
    void *timer;
    osal_hrtimer_restart (*handler)(void *timer);
    unsigned long interval; /* Unit ms */
} osal_hrtimer;

int osal_hrtimer_create(osal_hrtimer *hrtimer);
int osal_hrtimer_start(osal_hrtimer *hrtimer);
int osal_hrtimer_destory(osal_hrtimer *hrtimer);
void osal_rtc_time_to_tm(unsigned long time, osal_rtc_time *tm);
void osal_rtc_tm_to_time(osal_rtc_time *tm, unsigned long *time);
int osal_rtc_valid_tm(osal_rtc_time *tm);

// task api
typedef enum osal_task_priority_ {
    OSAL_TASK_PRIORITY_HIGH,  /* Highest  */
    OSAL_TASK_PRIORITY_MIDDLE,
    OSAL_TASK_PRIORITY_LOW,
    OSAL_TASK_PRIORITY_MAX,
}osal_task_priority;

typedef struct osal_task_ {
    void *task;
} osal_task;
typedef int (*osal_kthread_handler)(void *data);

osal_task *osal_kthread_create(osal_kthread_handler handler,
                               void *data,
                               const char *name,
                               unsigned int stack_size); // 0 mean default
void osal_kthread_set_priority(osal_task *task, osal_task_priority priority);
void osal_kthread_set_affinity(osal_task *task, int cpu_mask);
int osal_kthread_should_stop(void);
void osal_kthread_destroy(osal_task *task, unsigned int stop_flag);
void osal_kthread_schedule(unsigned int sleep_ns);
void osal_kthread_set_uninterrupt(void);
void osal_kthread_set_running(void);
void osal_kneon_begin(void);
void osal_kneon_end(void);

// fence
#define OSAL_FENCE_ERR_SUCCESS     0
#define OSAL_FENCE_ERR_FAIL        -1
#define OSAL_FENCE_ERR_NOT_READY   -2

int osal_fence_create(unsigned int module_id, int *fd, unsigned int private_size);
int osal_fence_destroy(unsigned int module_id, int fd);
int osal_fence_acquire_private(unsigned int module_id, int fd, void **private);
int osal_fence_release_private(unsigned int module_id, int fd, void *private);
int osal_fence_signal(int fd);
int osal_fence_wait(int fd, unsigned int ms);
int osal_fence_trywait(int fd);
int osal_fence_get_num(void);

// interrupt api
#define OSAL_CPU_0 (1<<1)
#define OSAL_CPU_1 (1<<2)
#define OSAL_CPU_2 (1<<3)
#define OSAL_CPU_3 (1<<4)

enum osal_irqreturn {
    OSAL_IRQ_NONE = (0 << 0),
    OSAL_IRQ_HANDLED = (1 << 0),
    OSAL_IRQ_WAKE_THREAD = (1 << 1),
};

typedef int (*osal_irq_handler)(int, void *);
int osal_irq_request(unsigned int irq,
                     osal_irq_handler irq_handler,
                     osal_irq_handler thread_handler,
                     const char *name,
                     void *dev);
int osal_irq_set_irq_type(unsigned int irq, unsigned int type);
void osal_irq_free(unsigned int irq, void *dev);
int osal_irq_set_affinity(unsigned int irq, const char *name, int cpu_mask);
void osal_irq_enable(unsigned int irq);
void osal_irq_disable(unsigned int irq);
int osal_in_interrupt(void);

// tasklet api
typedef struct osal_tasklet_ {
    void *tasklet;
    void (*handler)(unsigned long data);
    unsigned long data;
} osal_tasklet;
typedef void (*osal_tasklet_handler)(unsigned long data);

int osal_tasklet_init(osal_tasklet *tasklet);
int osal_tasklet_schedule(osal_tasklet *tasklet);
int osal_tasklet_kill(osal_tasklet *tasklet);
int osal_tasklet_update(osal_tasklet *tasklet);

// addr translate
void *osal_ioremap(unsigned long phys_addr, unsigned long size);
void *osal_ioremap_nocache(unsigned long phys_addr, unsigned long size);
void *osal_ioremap_cached(unsigned long phys_addr, unsigned long size);
void osal_iounmap(void *addr);

void *osal_phys_to_virt(unsigned long addr);
unsigned long osal_virt_to_phys(const void *virt_addr);

void *osal_blockmem_vmap(unsigned long phys_addr, unsigned long size);
void osal_blockmem_vunmap(const void *virt_addr);
void osal_blockmem_free(unsigned long phys_addr, unsigned long size);

#define osal_readl(x) (*((volatile unsigned int *)(x)))
#define osal_writel(v, x) (*((volatile unsigned int *)(x)) = (v))

unsigned long osal_copy_from_user(void *to, const void *from, unsigned long n);
unsigned long osal_copy_to_user(void *to, const void *from, unsigned long n);

#define OSAL_VERIFY_READ   0
#define OSAL_VERIFY_WRITE  1
int osal_access_ok(int type, const void *addr, unsigned long size);

// cache
void osal_flush_dcache_area(void *kvirt, unsigned long phys_addr, unsigned long size);

//math
unsigned long long osal_div_u64(unsigned long long dividend, unsigned int divisor);
long long osal_div_s64(long long dividend, int divisor);
unsigned long long osal_div64_u64(unsigned long long dividend, unsigned long long divisor);
long long osal_div64_s64(long long dividend, long long divisor);
unsigned long long osal_div_u64_rem(unsigned long long dividend, unsigned int divisor);
long long osal_div_s64_rem(long long dividend, int divisor);
unsigned long long osal_div64_u64_rem(unsigned long long dividend, unsigned long long divisor);

#define osal_max(x, y) ({ \
    __typeof__(x) _max1 = (x); \
    __typeof__(y) _max2 = (y); \
    (void) (&_max1 == &_max2); \
    _max1 > _max2 ? _max1 : _max2; })

#define osal_min(x, y) ({ \
    __typeof__(x) _min1 = (x); \
    __typeof__(y) _min2 = (y); \
    (void) (&_min1 == &_min2); \
    _min1 < _min2 ? _min1 : _min2; })

#define osal_abs(x) ({                \
    long ret;                         \
    if (sizeof(x) == sizeof(long)) {  \
        long __x = (x);               \
        ret = (__x < 0) ?  (-__x) : (__x); \
    } else {                          \
        int __x = (x);                \
        ret = (__x < 0) ? (-__x) : (__x); \
    }                                 \
    ret;                              \
})

//  barrier
void osal_mb(void);
void osal_rmb(void);
void osal_wmb(void);
void osal_smp_mb(void);
void osal_smp_rmb(void);
void osal_smp_wmb(void);
void osal_isb(void);
void osal_dsb(void);
void osal_dmb(void);

// proc
#define OSAL_PROC_NAME_LENGTH      32
#define PROC_CMD_ALL_LENGTH_MAX    128
#define PROC_CMD_NUM_MAX           16
#define PROC_CMD_SINGEL_LENGTH_MAX 32

typedef struct osal_proc_cmd_{
    char name[OSAL_PROC_NAME_LENGTH];
    int (*handler)(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
} osal_proc_cmd;

typedef struct osal_proc_entry_ {
    char name[OSAL_PROC_NAME_LENGTH];
    int (*open)(void *private);
    int (*read)(void *seqfile, void *private);
    osal_proc_cmd *cmd_list;
    unsigned int cmd_cnt;
    void *private;
    void *proc_dir_entry;
    void *seqfile;
    struct osal_list_head node;
} osal_proc_entry;

osal_proc_entry *osal_proc_add(const char *name, unsigned long name_size);
void osal_proc_remove(const char *name, unsigned long name_size);
int osal_proc_print(void *seqfile, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
void osal_printk(const char *fmt, ...);
void osal_proc_echo(const char *fmt, ...); /* Can not be used in irq server */

// device api
#ifndef _IOC_TYPECHECK
#ifdef __LITEOS__
#include "asm/ioctl.h"
#else
#include "osal_ioctl.h"
#endif
#endif
#define OSAL_POLLIN        0x0001
#define OSAL_POLLPRI       0x0002
#define OSAL_POLLOUT       0x0004
#define OSAL_POLLERR       0x0008
#define OSAL_POLLHUP       0x0010
#define OSAL_POLLNVAL      0x0020
#define OSAL_POLLRDNORM    0x0040
#define OSAL_POLLRDBAND    0x0080
#define OSAL_POLLWRNORM    0x0100

#define OSAL_SEEK_SET      0
#define OSAL_SEEK_CUR      1
#define OSAL_SEEK_END      2

typedef struct osal_poll_ {
    void *poll_table;
    void *data;
} osal_poll;

typedef struct osal_ioctl_cmd_{
    unsigned int cmd;
    int (*handler)(unsigned int cmd, void *arg, void *private_data);
} osal_ioctl_cmd;

typedef struct osal_vm_ {
    void *vm;
} osal_vm;

typedef struct osal_fileops_ {
    int (*open)(void *private_data);
    int (*read)(char *buf, int size, long *offset, void *private_data);
    int (*write)(const char *buf, int size, long *offset, void *private_data);
    long (*llseek)(long offset, int whence, void *private_data);
    int (*release)(void *private_data);
    int (*poll)(osal_poll *osal_poll, void *private_data);
    int (*mmap)(osal_vm *vm, unsigned long start, unsigned long end, unsigned long vm_pgoff, void *private_data);
    osal_ioctl_cmd *cmd_list;
    unsigned int cmd_cnt;
} osal_fileops;

typedef struct osal_pmops_ {
    int (*pm_suspend)(void *private_data);
    int (*pm_resume_early)(void *private_data);
    int (*pm_resume)(void *private_data);
    int (*pm_lowpower_enter)(void *private_data);
    int (*pm_lowpower_exit)(void *private_data);
    int (*pm_poweroff)(void *private_data);
    void *private_data;
} osal_pmops;

#define OSAL_DEV_NAME_LEN 16
typedef struct osal_dev_ {
    char name[OSAL_DEV_NAME_LEN];
    int minor;
    osal_fileops *fops;
    osal_pmops *pmops;
    void *dev;
} osal_dev;

#define OSAL_NOCACHE 0
#define OSAL_CACHE   1

int osal_dev_register(osal_dev *dev);
void osal_dev_unregister(osal_dev *dev);
void osal_poll_wait(osal_poll *table, osal_wait *wait);
int osal_remap_pfn_range(osal_vm *vm, unsigned long addr, unsigned long pfn, unsigned long size, unsigned int cached);
int osal_try_to_freeze(void);
int osal_set_freezable(void);


// export function
int osal_exportfunc_register(unsigned int module_id, const char *name, void *func);
int osal_exportfunc_unregister(unsigned int module_id);
int osal_exportfunc_get(unsigned int module_id, void **func);


// kfile
#define OSAL_O_ACCMODE     00000003
#define OSAL_O_RDONLY      00000000
#define OSAL_O_WRONLY      00000001
#define OSAL_O_RDWR        00000002
#define OSAL_O_CREAT       00000100
#define OSAL_O_APPEND      00002000
#define OSAL_O_TRUNC       00001000
#define OSAL_O_CLOEXEC     02000000

void *osal_klib_fopen(const char *filename, int flags, int mode);
void osal_klib_fclose(void *filp);
int osal_klib_fwrite(const char *buf, unsigned long size, void *filp);
int osal_klib_fread(char *buf, unsigned long size, void *filp);
int osal_klib_user_fwrite(const char *buf, unsigned long size, void *filp);
int osal_klib_user_fread(char *buf, unsigned long size, void *filp);
void osal_klib_fsync(void *filp);
int osal_klib_fseek(long long offset, int whence, void *filp);
int osal_klib_get_store_path(char *path, unsigned int path_size);
void osal_klib_set_store_path(char *path);


// string
int osal_memncmp(const void *buf1, unsigned long size1,
    const void *buf2, unsigned long size2);
int osal_strncmp(const char *str1, unsigned long size1,
    const char *str2, unsigned long size2);
int osal_strncasecmp(const char *str1, unsigned long size1,
    const char *str2, unsigned long size2);

#define OSAL_BASE_DEC 10
#define OSAL_BASE_HEX 16

long osal_strtol(const char *, char **end, unsigned int base);
unsigned long  osal_strtoul(const char *, char **end, unsigned int base);
void osal_get_random_bytes(void *buf, int nbytes);


// debug
void osal_dump_stack(void);

/* gpio */
int osal_gpio_get_direction(unsigned int gpio);
int osal_gpio_set_direction(unsigned int gpio, unsigned int dir);
void osal_gpio_set_value(unsigned int gpio, int value);
int osal_gpio_get_value(unsigned int gpio);
int osal_gpio_irq_request(unsigned int gpio, osal_irq_handler handler,
    unsigned long flags, const char *name, void *dev);
void osal_gpio_irq_free(unsigned int gpio, void *dev);


#endif /* __HI_OSAL_H__ */

