/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __VFMW_OSAL__
#define __VFMW_OSAL__

#include "vfmw.h"
#include "securec.h"

#define OSAL_OK  0
#define OSAL_ERR 1

/* LiteOS ITCM For about 2.7K */
#define VFMW_ACC __attribute__((section(".itcm.text")))

#if defined(VFMW_ACC_SUPPORT) && (MDC_ITCM_CFG > 1)
#define VFMW_ACC_BASE               VFMW_ACC /* For about 7.3K */
#define VFMW_ACC_FRAME                       /* VFMW_ACC */ /* For about 1.5K */
#define VFMW_ACC_SLICE              VFMW_ACC /* For about 1.3K */
#define VFMW_ACC_SLICE_H265         VFMW_ACC /* For about 9.4K */
#define VFMW_ACC_SLICE_H265_ONE_NAL VFMW_ACC /* For about 8.8K */
#else
#define VFMW_ACC_BASE
#define VFMW_ACC_FRAME VFMW_ACC              /* For about 1.5K */
#define VFMW_ACC_SLICE              VFMW_ACC /* For about 1.3K */
#define VFMW_ACC_SLICE_H265
#define VFMW_ACC_SLICE_H265_ONE_NAL VFMW_ACC /* For about 8.8K */
#endif

#define VFMW_ACC_SLICE_H264 /* VFMW_ACC */
#define VFMW_ACC_SLICE_VP9  /* VFMW_ACC */
#define VFMW_ACC_SLICE_AVS2 /* VFMW_ACC */
#define VFMW_ACC_SLICE_AVS3 /* VFMW_ACC */

typedef enum {
    OS_RDONLY = 00000000,
    OS_WRONLY = 00000001,
    OS_RDWR = 00000002,
    OS_CREATE = 00000100,
    OS_TRUNC = 00001000,
    OS_APPEND = 00002000,
    OS_LARGEFILE = 0400000
} fo_mode;

typedef hi_void *OS_FILE;
typedef hi_s32 OS_SEMA;
typedef hi_s32 OS_EVENT;
typedef hi_s32 OS_ATOMIC;
typedef hi_s32 OS_LOCK;

struct OS_LIST_HEAD {
    struct OS_LIST_HEAD *next;
    struct OS_LIST_HEAD *prev;
};

hi_void list_add_(struct OS_LIST_HEAD *new,
                  struct OS_LIST_HEAD *prev,
                  struct OS_LIST_HEAD *next);

hi_void list_del_(struct OS_LIST_HEAD *prev,
                  struct OS_LIST_HEAD *next);

#define list_entry_(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

#define list_first_entry_(ptr, type, member) \
    list_entry_((ptr)->next, type, member)

#define list_next_entry_(pos, member) \
    list_entry_((pos)->member.next, __typeof__(*(pos)), member)

#define OS_LIST_INIT_HEAD(list) \
    (list)->next = (list);      \
    (list)->prev = (list);

#define OS_LIST_ADD(new, head) \
    list_add_(new, head, (head)->next);

#define OS_LIST_ADD_TAIL(new, head) \
    list_add_(new, (head)->prev, head);

#define OS_LIST_DEL(entry) \
    list_del_((entry)->prev, (entry)->next)

#define OS_LIST_EMPTY(head) \
    ((head)->next == (head))

#define OS_LIST_ENTRY(ptr, type, member) \
    list_entry_(ptr, type, member)

#define OS_LIST_FIRST_ENTRY(ptr, type, member) \
    list_first_entry_(ptr, type, member)

#define OS_LIST_FOR_EACH_SAFE(pos, n, head)                \
    for (pos = (head)->next, n = pos->next; pos != (head); \
         pos = n, n = pos->next)

#define OS_LIST_FOR_EACH_ENTRY(pos, head, member)             \
    for (pos = list_first_entry_(head, typeof(*pos), member); \
         &pos->member != (head);                              \
         pos = list_next_entry_(pos, member))

typedef enum {
    CACHE_MEM_TYPE,    /* context mem */
    NO_CACHE_MEM_TYPE, /* segbuff and msg pool */
    CHAN_SHARE_MEM_TYPE,
    GLOBAL_SHARE_MEM_TYPE,
    DEV_HAL_MEM_TYPE,
    STREAM_MEM_TYPE,
    MAX_MEM_TYPE
} mem_manage_type;

typedef struct {
    hi_u64 dma_buf;
    hi_u8 is_cached;
    hi_u8 *vir_addr;
    UADDR phy_addr;
    hi_char *mem_name;
    hi_s32 len;
    hi_s32 id;
    mem_manage_type type;
} osal_mem;

typedef hi_s32 (*vfmw_irq_handler_t)(hi_s32, hi_void *);
typedef hi_u32 (*fn_osal_get_time_in_ms)(hi_void);
typedef hi_u32 (*fn_osal_get_time_in_us)(hi_void);
typedef hi_u64 (*fn_osal_get_time_us64)(hi_void);
typedef hi_s32 (*fn_osal_set_time_base)(hi_u64, hi_u64);
typedef hi_u32 (*fn_osal_get_rand)(hi_void);
typedef hi_s32 (*fn_osal_get_unid)(hi_void);
typedef hi_s32 (*fn_osal_atomic_init)(hi_s32 *, hi_s32);
typedef hi_void (*fn_osal_atomic_exit)(hi_s32);
typedef hi_void (*fn_osal_atomic_set)(hi_s32, hi_s32);
typedef hi_s32 (*fn_osal_atomic_read)(hi_s32);
typedef hi_void (*fn_osal_atomic_add)(hi_s32, hi_s32);
typedef hi_s32 (*fn_osal_atomic_add_ret)(hi_s32, hi_s32);
typedef hi_void (*fn_osal_atomic_sub)(hi_s32, hi_s32);
typedef hi_s32 (*fn_osal_atomic_sub_test)(hi_s32, hi_s32);
typedef hi_s32 (*fn_osal_atomic_sub_ret)(hi_s32, hi_s32);
typedef hi_void (*fn_osal_atomic_inc)(hi_s32);
typedef hi_s32 (*fn_osal_atomic_inc_test)(hi_s32);
typedef hi_s32 (*fn_osal_atomic_inc_ret)(hi_s32);
typedef hi_void (*fn_osal_atomic_dec)(hi_s32);
typedef hi_s32 (*fn_osal_atomic_dec_test)(hi_s32);
typedef hi_s32 (*fn_osal_atomic_dec_ret)(hi_s32);
typedef hi_s32 (*fn_osal_spin_lock_init)(hi_s32 *);
typedef hi_void (*fn_osal_spin_lock_exit)(hi_s32);
typedef hi_s32 (*fn_osal_spin_lock)(hi_s32, hi_ulong *);
typedef hi_s32 (*fn_osal_spin_unlock)(hi_s32, hi_ulong *);
typedef hi_s32 (*fn_osal_sema_init)(hi_s32 *);
typedef hi_void (*fn_osal_sema_exit)(hi_s32);
typedef hi_s32 (*fn_osal_sema_down)(hi_s32);
typedef hi_s32 (*fn_osal_sema_try)(hi_s32);
typedef hi_void (*fn_osal_sema_up)(hi_s32);
typedef hi_void *(*fn_osal_file_open)(const char *, hi_s32, hi_s32);
typedef hi_void (*fn_osal_file_close)(hi_void *);
typedef hi_s32 (*fn_osal_file_read)(char *, hi_u32, hi_void *);
typedef hi_s32 (*fn_osal_file_write)(char *, hi_u32, hi_void *);
typedef hi_void *(*fn_osal_memset)(hi_void *, hi_s32, hi_ulong);
typedef hi_void *(*fn_osal_memcpy)(hi_void *, const hi_void *, hi_ulong);
typedef hi_void *(*fn_osal_memmov)(hi_void *, const hi_void *, hi_ulong);
typedef hi_s32 (*fn_osal_memcmp)(hi_void *, const hi_void *, hi_ulong);
typedef hi_s32 (*fn_osal_log_on)(hi_u32);
typedef hi_void (*fn_osal_log_off)(hi_void);
typedef hi_void (*fn_osal_rd_log)(hi_void);
typedef hi_s32 (*fn_osal_wr_log)(const char *, ...);
typedef hi_void (*fn_osal_print)(const char *, ...);
typedef hi_s32 (*fn_osal_snprintf)(char *, hi_ulong, const char *, ...);
typedef hi_s32 (*fn_osal_sscanf)(const char *, const char *, ...);
typedef hi_s32 (*fn_osal_strncmp)(const char *, const char *, hi_u32);
typedef hi_u32 (*fn_osal_strlcpy)(char *, const char *, hi_u32);
typedef hi_s8 *(*fn_osal_strsep)(char **, const char *);
typedef long (*fn_osal_simple_strtol)(const char *, char **, hi_u32);
typedef hi_s8 *(*fn_osal_strstr)(const char *, const char *);
typedef hi_u32 (*fn_osal_strlen)(const char *);
typedef hi_void *(*fn_osal_ioremap)(UADDR, hi_ulong);
typedef hi_void (*fn_osal_iounmap)(hi_void *);
typedef hi_void (*fn_osal_mb)(hi_void);
typedef hi_void (*fn_osal_udelay)(hi_ulong);
typedef hi_void (*fn_osal_msleep)(hi_u32);
typedef hi_s32 (*fn_osal_event_init)(hi_s32 *, hi_s32);
typedef hi_void (*fn_osal_event_exit)(hi_s32);
typedef hi_s32 (*fn_osal_event_give)(hi_s32);
typedef hi_s32 (*fn_osal_event_wait)(hi_s32, hi_s32);
typedef hi_void (*fn_osal_flush_cache)(hi_void *, UADDR, hi_u32);
typedef hi_s32 (*fn_osal_mem_alloc)(hi_char *, hi_u32, hi_u32, hi_u32, vfmw_mem_desc *);
typedef hi_s32 (*fn_osal_mem_free)(vfmw_mem_desc *);
typedef hi_u8 *(*fn_osal_reg_map)(UADDR, hi_u32);
typedef hi_void (*fn_osal_reg_unmap)(hi_u8 *);
typedef hi_u8 *(*fn_osal_mmap)(mem_record *);
typedef hi_void (*fn_osal_munmap)(mem_record *);
typedef hi_u8 *(*fn_osal_mmap_cache)(mem_record *);
typedef hi_u8 *(*fn_osal_ns_mmap)(UADDR, hi_u32);
typedef hi_void (*fn_osal_ns_munmap)(hi_u8 *);
typedef hi_void (*fn_osal_get_page_table_addr)(UADDR *, UADDR *, UADDR *);
typedef hi_void (*fn_osal_get_sec_page_table_addr)(UADDR *, UADDR *, UADDR *);
typedef hi_void *(*fn_osal_create_thread)(hi_void *, hi_void *, hi_char *);
typedef hi_s32 (*fn_osal_stop_task)(hi_void *);
typedef hi_s32 (*fn_osal_kthread_should_stop)(hi_void);
typedef hi_s32 (*fn_osal_request_irq)(hi_u32, vfmw_irq_handler_t, hi_ulong, const char *, hi_void *);
typedef hi_void (*fn_osal_free_irq)(hi_u32, const char *, hi_void *);
typedef hi_void *(*fn_osal_phys_to_virt)(UADDR);
typedef hi_void (*fn_osal_dump_stack)(hi_void);
typedef hi_void *(*fn_osal_alloc_vir_mem)(hi_s32);
typedef hi_void (*fn_osal_free_vir_mem)(hi_void *);
typedef hi_s32 (*fn_osal_get_vir_mem_num)(hi_void);
typedef hi_s32 (*fn_osal_proc_init)(hi_void);
typedef hi_void (*fn_osal_proc_exit)(hi_void);
typedef hi_s32 (*fn_osal_proc_create)(hi_u8 *, hi_void *, hi_void *, hi_u32);
typedef hi_void (*fn_osal_proc_destroy)(hi_u8 *);
typedef hi_void (*fn_osal_wakeup_ree)(hi_void);
typedef hi_u8 *(*fn_osal_mmap_mmz)(mem_record *);
typedef hi_void (*fn_osal_munmap_mmz)(mem_record *);
typedef hi_void (*fn_osal_dump_proc)(hi_void *, hi_s32, hi_s32 *, hi_s8, const hi_char *, ...);
typedef hi_u64 (*fn_osal_do_div)(hi_u64, hi_u32);
typedef hi_void (*fn_osal_read_osal)(hi_void *, hi_s32, hi_s32 *);
typedef hi_s32 (*fn_osal_mem_init)(hi_u32, hi_u32);
typedef hi_s32 (*fn_osal_mem_deinit)(hi_void);
typedef hi_void *(*fn_osal_alloc_vir)(hi_char *, hi_s32);
typedef hi_void (*fn_osal_free_vir)(hi_void *);
typedef hi_s32 (*fn_osal_malloc)(osal_mem *);
typedef hi_s32 (*fn_osal_free)(osal_mem *);
typedef hi_u8 *(*fn_osal_pre_alloc_map)(osal_mem *);
typedef hi_void (*fn_osal_pre_alloc_unmap)(osal_mem *);
typedef hi_u8 *(*fn_osal_iommu_mmap)(mem_record *);
typedef hi_void (*fn_osal_iommu_unmap)(mem_record *);
typedef hi_s32 (*fn_osal_alloc_mem)(hi_u8 *, hi_u32, mem_record *);
typedef hi_void (*fn_osal_free_mem)(mem_record *);
typedef hi_u8 *(*fn_osal_map_mem)(mem_record *);
typedef hi_void (*fn_osal_unmap_mem)(mem_record *);
typedef hi_s64 (*fn_osal_get_mem_fd)(hi_void *);
typedef hi_void (*fn_osal_put_mem_fd)(hi_s64);
typedef hi_void *(*fn_osal_get_dma_buf)(hi_s64);
typedef hi_void (*fn_osal_put_dma_buf)(hi_void *);
typedef hi_u64 (*fn_osal_get_phy)(hi_void *, hi_bool);
typedef hi_void (*fn_osal_put_phy)(hi_void *, hi_u64, hi_bool);

typedef struct {
    hi_s32 check1;

    fn_osal_get_time_in_ms get_time_in_ms;
    fn_osal_get_time_in_us get_time_in_us;
    fn_osal_get_time_us64 get_time_us64;
    fn_osal_set_time_base set_time_base;
    fn_osal_get_rand get_rand;
    fn_osal_get_unid get_unid;
    fn_osal_atomic_init atomic_init;
    fn_osal_atomic_exit atomic_exit;
    fn_osal_atomic_set atomic_set_;
    fn_osal_atomic_read atomic_read_;
    fn_osal_atomic_inc_ret atomic_inc_ret;
    fn_osal_atomic_dec_ret atomic_dec_ret;
    fn_osal_spin_lock_init os_spin_lock_init;
    fn_osal_spin_lock_exit os_spin_lock_exit;
    fn_osal_spin_lock os_spin_lock;
    fn_osal_spin_unlock os_spin_unlock;
    fn_osal_sema_init sema_init;
    fn_osal_sema_exit sema_exit;
    fn_osal_sema_down sema_down;
    fn_osal_sema_try sema_try;
    fn_osal_sema_up sema_up;
    fn_osal_file_open file_open;
    fn_osal_file_close file_close;
    fn_osal_file_read file_read;
    fn_osal_file_write file_write;
    fn_osal_memset os_memset;
    fn_osal_memcpy os_memcpy;
    fn_osal_memmov os_memmov;
    fn_osal_memcmp os_memcmp;
    fn_osal_log_on log_on;
    fn_osal_log_off log_off;
    fn_osal_rd_log rd_log;
    fn_osal_wr_log wr_log;
    fn_osal_print print;
    fn_osal_strncmp strncmp;
    fn_osal_simple_strtol simple_strtol;
    fn_osal_strlcpy strlcpy;
    fn_osal_strsep strsep;
    fn_osal_strstr strstr;
    fn_osal_strlen strlen;
    fn_osal_ioremap ioremap;
    fn_osal_iounmap iounmap;
    fn_osal_mb os_mb;
    fn_osal_udelay udelay;
    fn_osal_msleep msleep;
    fn_osal_event_init event_init;
    fn_osal_event_exit event_exit;
    fn_osal_event_give event_give;
    fn_osal_event_wait event_wait;
    fn_osal_flush_cache flush_cache;
    fn_osal_mem_alloc mem_alloc;
    fn_osal_mem_free mem_free;
    fn_osal_reg_map reg_map;
    fn_osal_reg_unmap reg_unmap;
    fn_osal_mmap mmap;
    fn_osal_munmap munmap;
    fn_osal_mmap_cache mmap_cache;
    fn_osal_ns_mmap ns_mmap;
    fn_osal_ns_munmap ns_munmap;
    fn_osal_get_page_table_addr get_page_table_addr;
    fn_osal_get_sec_page_table_addr get_sec_page_table_addr;
    fn_osal_create_thread create_thread;
    fn_osal_stop_task stop_thread;
    fn_osal_kthread_should_stop thread_should_stop;
    fn_osal_request_irq request_irq;
    fn_osal_free_irq free_irq;
    fn_osal_phys_to_virt phys_to_virt;
    fn_osal_dump_stack dump_stack;
    fn_osal_alloc_vir_mem alloc_vir_mem;
    fn_osal_free_vir_mem free_vir_mem;
    fn_osal_get_vir_mem_num get_vir_mem_num;
    fn_osal_proc_init proc_init;
    fn_osal_proc_exit proc_exit;
    fn_osal_proc_create proc_create;
    fn_osal_proc_destroy proc_destroy;
    fn_osal_wakeup_ree wakeup_ree;
    fn_osal_mmap_mmz mmap_mmz;
    fn_osal_munmap_mmz munmap_mmz;
    fn_osal_dump_proc dump_proc;
    fn_osal_do_div os_div;
    fn_osal_read_osal read_osal;
    fn_osal_alloc_vir alloc_vir;
    fn_osal_free_vir free_vir;
    fn_osal_mem_init mem_init;
    fn_osal_mem_deinit mem_deinit;
    fn_osal_malloc malloc;
    fn_osal_free free;
    fn_osal_pre_alloc_map pre_alloc_map;
    fn_osal_pre_alloc_unmap pre_alloc_unmap;
    fn_osal_iommu_mmap iommu_mmap;
    fn_osal_iommu_unmap iommu_unmap;
    fn_osal_alloc_mem alloc_mem;
    fn_osal_free_mem free_mem;
    fn_osal_map_mem map_mem;
    fn_osal_unmap_mem unmap_mem;
    fn_osal_get_mem_fd get_mem_fd;
    fn_osal_put_mem_fd put_mem_fd;
    fn_osal_get_phy get_mem_phy;
    fn_osal_put_phy put_mem_phy;
    fn_osal_get_dma_buf get_dma_buf;
    fn_osal_put_dma_buf put_dma_buf;

    hi_s32 check2;
} vfmw_osal_ops;

extern vfmw_osal_ops g_vfmw_osal_ops;

#define OS_GET_RAND           g_vfmw_osal_ops.get_rand
#define OS_STRSTR             g_vfmw_osal_ops.strstr
#define OS_IOREMAP            g_vfmw_osal_ops.ioremap
#define OS_IOUNMAP            g_vfmw_osal_ops.iounmap
#define OS_GET_UNID           g_vfmw_osal_ops.get_unid
#define OS_ATOMIC_INIT        g_vfmw_osal_ops.atomic_init
#define OS_ATOMIC_EXIT        g_vfmw_osal_ops.atomic_exit
#define OS_ATOMIC_SET         g_vfmw_osal_ops.atomic_set_
#define OS_ATOMIC_READ        g_vfmw_osal_ops.atomic_read_
#define OS_ATOMIC_INC_RET     g_vfmw_osal_ops.atomic_inc_ret
#define OS_ATOMIC_DEC_RET     g_vfmw_osal_ops.atomic_dec_ret
#define OS_MB                 g_vfmw_osal_ops.os_mb
#define OS_UDELAY             g_vfmw_osal_ops.udelay
#define OS_MSLEEP             g_vfmw_osal_ops.msleep
#define OS_EVENT_INIT         g_vfmw_osal_ops.event_init
#define OS_EVENT_EXIT         g_vfmw_osal_ops.event_exit
#define OS_EVENT_GIVE         g_vfmw_osal_ops.event_give
#define OS_EVENT_WAIT         g_vfmw_osal_ops.event_wait
#define OS_FLUSH_CACHE        g_vfmw_osal_ops.flush_cache
#define OS_KMEM_ALLOC         g_vfmw_osal_ops.mem_alloc
#define OS_KMEM_FREE          g_vfmw_osal_ops.mem_free
#define OS_KMAP_REG           g_vfmw_osal_ops.reg_map
#define OS_KUNMAP_REG         g_vfmw_osal_ops.reg_unmap
#define OS_KMAP_MMZ           g_vfmw_osal_ops.mmap_mmz
#define OS_KUNMAP_MMZ         g_vfmw_osal_ops.munmap_mmz
#define OS_KNS_MAP            g_vfmw_osal_ops.ns_mmap
#define OS_KNSUN_MAP          g_vfmw_osal_ops.ns_munmap
#define OS_GET_MMU_TABLE      g_vfmw_osal_ops.get_page_table_addr
#define OS_GET_SEC_MMU_TABLE  g_vfmw_osal_ops.get_sec_page_table_addr
#define OS_CREATE_THREAD      g_vfmw_osal_ops.create_thread
#define OS_STOP_THREAD        g_vfmw_osal_ops.stop_thread
#define OS_THREAD_SHOULD_STOP g_vfmw_osal_ops.thread_should_stop
#define OS_REQUEST_IRQ        g_vfmw_osal_ops.request_irq
#define OS_GET_VIR_MEM_NUM    g_vfmw_osal_ops.get_vir_mem_num
#define OS_FREE_IRQ           g_vfmw_osal_ops.free_irq
#define OS_PHYS_TO_VIRT       g_vfmw_osal_ops.phys_to_virt
#define OS_DUMP_STACK         g_vfmw_osal_ops.dump_stack
#define OS_GET_TIME_MS        g_vfmw_osal_ops.get_time_in_ms
#define OS_GET_TIME_US        g_vfmw_osal_ops.get_time_in_us
#define OS_GET_TIME_US64      g_vfmw_osal_ops.get_time_us64
#define OS_SET_TIME_BASE      g_vfmw_osal_ops.set_time_base
#define OS_SPIN_LOCK_INIT     g_vfmw_osal_ops.os_spin_lock_init
#define OS_SPIN_LOCK_EXIT     g_vfmw_osal_ops.os_spin_lock_exit
#define OS_SPIN_LOCK          g_vfmw_osal_ops.os_spin_lock
#define OS_SPIN_UNLOCK        g_vfmw_osal_ops.os_spin_unlock
#define OS_SEMA_INIT          g_vfmw_osal_ops.sema_init
#define OS_SEMA_EXIT          g_vfmw_osal_ops.sema_exit
#define OS_SEMA_DOWN          g_vfmw_osal_ops.sema_down
#define OS_SEMA_TRY           g_vfmw_osal_ops.sema_try
#define OS_SEMA_UP            g_vfmw_osal_ops.sema_up
#define OS_MEMCMP             g_vfmw_osal_ops.os_memcmp
#define OS_LOG_ON             g_vfmw_osal_ops.log_on
#define OS_LOG_OFF            g_vfmw_osal_ops.log_off
#define OS_RD_LOG             g_vfmw_osal_ops.rd_log
#define OS_WR_LOG             g_vfmw_osal_ops.wr_log
#define OS_SIMPLE_STRTOL      g_vfmw_osal_ops.simple_strtol
#define OS_STRLCPY            g_vfmw_osal_ops.strlcpy
#define OS_STRSEP             g_vfmw_osal_ops.strsep
#define OS_PRINT              g_vfmw_osal_ops.print
#define OS_STRLEN             g_vfmw_osal_ops.strlen
#define OS_STRNCMP            g_vfmw_osal_ops.strncmp
#define OS_FOPEN              g_vfmw_osal_ops.file_open
#define OS_FCLOSE             g_vfmw_osal_ops.file_close
#define OS_FREAD              g_vfmw_osal_ops.file_read
#define OS_FWRITE             g_vfmw_osal_ops.file_write
#define OS_PROC_INIT          g_vfmw_osal_ops.proc_init
#define OS_PROC_EXIT          g_vfmw_osal_ops.proc_exit
#define OS_PROC_CREATE        g_vfmw_osal_ops.proc_create
#define OS_PROC_DESTROY       g_vfmw_osal_ops.proc_destroy
#define OS_MMAP               g_vfmw_osal_ops.mmap
#define OS_KMMAP_CACHE        g_vfmw_osal_ops.mmap_cache
#define OS_UNMAP              g_vfmw_osal_ops.munmap
#define OS_WAKEUP_REE         g_vfmw_osal_ops.wakeup_ree
#define OS_DUMP_PROC          g_vfmw_osal_ops.dump_proc
#define OS_DO_DIV             g_vfmw_osal_ops.os_div
#define OS_READ_OSAL          g_vfmw_osal_ops.read_osal
#define OS_ALLOC_VIR          g_vfmw_osal_ops.alloc_vir
#define OS_FREE_VIR           g_vfmw_osal_ops.free_vir
#define OS_MEM_INIT           g_vfmw_osal_ops.mem_init
#define OS_MEM_DEINIT         g_vfmw_osal_ops.mem_deinit
#define OS_MALLOC             g_vfmw_osal_ops.malloc
#define OS_FREE               g_vfmw_osal_ops.free
#define OS_PRE_ALLOC_MAP      g_vfmw_osal_ops.pre_alloc_map
#define OS_PRE_ALLOC_UNMAP    g_vfmw_osal_ops.pre_alloc_unmap
#define OS_IOMMU_MMAP         g_vfmw_osal_ops.iommu_mmap
#define OS_IOMMU_UNMAP        g_vfmw_osal_ops.iommu_unmap
#define OS_ALLOC_MEM          g_vfmw_osal_ops.alloc_mem
#define OS_FREE_MEM           g_vfmw_osal_ops.free_mem
#define OS_MAP_MEM            g_vfmw_osal_ops.map_mem
#define OS_UNMAP_MEM          g_vfmw_osal_ops.unmap_mem
#define OS_GET_MEM_FD         g_vfmw_osal_ops.get_mem_fd
#define OS_PUT_MEM_FD         g_vfmw_osal_ops.put_mem_fd
#define OS_GET_PHY            g_vfmw_osal_ops.get_mem_phy
#define OS_PUT_PHY            g_vfmw_osal_ops.put_mem_phy
#define OS_GET_DMA_BUF        g_vfmw_osal_ops.get_dma_buf
#define OS_PUT_DMA_BUF        g_vfmw_osal_ops.put_dma_buf

hi_void os_intf_init(hi_void);
hi_void os_intf_exit(hi_void);

#endif
