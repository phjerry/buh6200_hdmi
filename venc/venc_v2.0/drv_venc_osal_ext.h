/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2012-04-22
 */
#ifndef  __DRV_VENC_OSAL_EXT_H__
#define  __DRV_VENC_OSAL_EXT_H__

#include "hi_type.h"
#include "hi_venc_type.h"
#include "hi_osal.h"

#if defined(VENC_EXTRA_TYPE_DEFINE)
    #define UINT64 hi_u64
    #define SINT64 hi_s64
    #define UINT32 hi_u32
    #define SINT32 hi_s32
    #define UINT16 hi_u16
    #define SINT16 hi_s16
    #define UINT8  hi_u8
    #define SINT8  hi_s8
    #define VOID   hi_void
    #define UADDR  hi_u64
    #define ULONG  hi_size_t
    #define HANDLE hi_handle
    #define VIRT_ADDR   hi_void*
    typedef const void CONSTVOID;
#else
    typedef unsigned long long UINT64;
    typedef long long SINT64;
    typedef unsigned int UINT32;
    typedef int SINT32;
    typedef unsigned short UINT16;
    typedef short SINT16;
    typedef char SINT8;
    typedef unsigned char UINT8;
    typedef unsigned int UADDR;
    typedef const void CONSTVOID;
    typedef unsigned long ULONG;
    typedef unsigned int HANDLE;
#ifndef VOID
    typedef void VOID;
#endif

#endif

typedef enum {
    MEM_CMA_CMA = 0,
    MEM_CMA_SEC,
    MEM_MMU_MMU,
    MEM_MMU_SEC,
} mem_mode;

typedef struct {
    hi_s32 fd;
    hi_u64 dma_buf;
    hi_bool skip_close_fd;
    hi_u8* start_vir_addr;    /* VirAddr */
    UADDR  start_phy_addr;    /* PhyAddr or SmmuAddr */
    hi_u32 size;
    mem_mode mode;
} venc_buffer;

#define UINT64_PTR(ptr) ((hi_void *)(hi_ulong)(ptr))
#define PTR_UINT64(ptr) ((hi_u64)(hi_ulong)(ptr))

typedef unsigned long   vedu_lock_flag;

typedef hi_void* (*fn_osal_mem_set)(hi_void* s, hi_u32 size, hi_s32 c, unsigned long n);
typedef hi_void* (*fn_osal_mem_cpy)(hi_void* d, hi_u32 size, hi_void* s, unsigned long n);
typedef SINT32 (*fn_osal_printk)(const char* fmt, ...);
typedef hi_void* (*fn_osal_kernel_register_map)(unsigned long phy_addr, unsigned long size);
typedef hi_void (*fn_osal_kernel_register_unmap)(hi_void* vir_addr);

typedef VOID *(*fn_osal_alloc_vir_mem)(SINT32 size);
typedef VOID (*fn_osal_free_vir_mem)(const VOID *vir_mem);

typedef struct {
    fn_osal_mem_set fun_osal_mem_set;
    fn_osal_mem_cpy fun_osal_mem_cpy;
    fn_osal_printk fun_osal_printk;
    fn_osal_kernel_register_map fun_osal_ioremap ;
    fn_osal_kernel_register_unmap fun_osal_iounmap;

    fn_osal_alloc_vir_mem fun_osal_alloc_vir_mem;
    fn_osal_free_vir_mem fun_osal_free_vir_mem;
} venc_osal_func_ptr;

#define VENC_CHECK_NEQ(func, ref) \
do { \
    hi_s32 rst = func; \
    if (rst != (ref)) { \
        HI_ERR_VENC("check  err; ret = 0x%x\n", rst);   \
    } \
} while (0)

#define VENC_CHECK_NEQ_VOID(func, ref) \
do { \
    hi_s32 rst = func; \
    if (rst != (ref)) { \
        HI_ERR_VENC("check  err; ret = 0x%x\n", rst);   \
        return; \
    } \
} while (0)

#define VENC_CHECK_NEQ_RET(func, ref, ret) \
do { \
    hi_s32 rst = func; \
    if (rst != (ref)) { \
        HI_ERR_VENC("check  err; ret = 0x%x\n", rst);   \
        return ret; \
    } \
} while (0)

hi_void venc_drv_osal_irq_free(hi_u32 irq);
hi_s32  venc_drv_osal_irq_int(hi_u32 irq, hi_void (*ptr_call_back)(hi_void));
hi_s32  venc_drv_osal_lock_create (osal_spinlock** lock);
hi_void venc_drv_osal_lock_destory(osal_spinlock* lock);
hi_void venc_drv_osal_lock(osal_spinlock* lock, vedu_lock_flag* flag);
hi_void venc_drv_osal_unlock(osal_spinlock* lock, vedu_lock_flag* flag);
hi_s32  venc_drv_osal_create_task(hi_void** task, hi_u8 task_name[], hi_void* task_function);
hi_s32  venc_drv_osal_delete_task(hi_void* task);
hi_void venc_drv_osal_sem_init(osal_semaphore *sem);
hi_void venc_drv_osal_sem_deinit(osal_semaphore *sem);

struct file* venc_drv_osal_fopen(const char* filename, int flags, int mode);
void venc_drv_osal_fclose(struct file* filp);
int venc_drv_osal_fwrite(const char* buf, int len, struct file* filp);

#ifdef HI_SMMU_SUPPORT
hi_void hi_drv_venc_get_smmu_addr(hi_ulong *smmu_page_base_addr, hi_ulong *smmu_err_read_addr,
    hi_ulong *smmu_err_write_addr);
#endif

hi_s32 hi_drv_venc_alloc_and_map(const char *buf_name, hi_u32 cache, hi_u32 len, hi_u32 align,
    venc_buffer *venc_buf);
hi_s32 hi_drv_venc_unmap_and_release(venc_buffer *venc_buf);

hi_s32 hi_drv_venc_mmz_alloc_and_map(const char *buf_name, hi_u32 cache, hi_u32 size, int align,
    venc_buffer *venc_buf);
hi_s32 hi_drv_venc_mmz_unmap_and_release(venc_buffer *venc_buf);

hi_s32 hi_drv_venc_map(venc_buffer *venc_buf);
hi_void hi_drv_venc_unmap(venc_buffer *venc_buf);

hi_s32 hi_drv_venc_handle_map(venc_buffer *venc_buf);
hi_s32 hi_drv_venc_handle_unmap(venc_buffer *venc_buf);

hi_void init_venc_interface(hi_void);

#endif /* __DRV_VENC_OSAL_EXT_H__ */

