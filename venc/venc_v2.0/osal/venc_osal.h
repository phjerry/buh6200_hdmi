/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: encoder
* Author: sdk
* Create: 2019-08-13
*/

#ifndef __VENC_OSAL_H__
#define __VENC_OSAL_H__
#include <linux/wait.h>
#include <linux/sched.h>

#if defined(VENC_EXTRA_TYPE_DEFINE)
#include "hi_type.h"
#endif

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
    #define UADDR  hi_u32
    #define ULONG  hi_size_t
    #define HANDLE hi_handle
    #define VIRT_ADDR   hi_void*
    typedef const void CONSTVOID;
#else
    typedef  unsigned long long UINT64;
    typedef  long long          SINT64;
    typedef  unsigned int       UINT32;
    typedef  int                SINT32;
    typedef  unsigned short     UINT16;
    typedef  short              SINT16;
    typedef  char               SINT8;
    typedef  unsigned char      UINT8;
    typedef  unsigned int       UADDR;
    typedef  const void         CONSTVOID;
    typedef  unsigned long      ULONG;
    typedef  unsigned int       HANDLE;
#ifndef  VOID
    typedef  void               VOID;
#endif

#endif

typedef struct {
    wait_queue_head_t   queue_head;
    hi_s32              flag;
} kern_event;

typedef enum {
    MEM_CMA_CMA = 0,
    MEM_CMA_SEC,
    MEM_MMU_MMU,
    MEM_MMU_SEC,
} mem_mode;

typedef struct {
    hi_u8* start_vir_addr;    /* VirAddr */
    UADDR  start_phy_addr;    /* PhyAddr or SmmuAddr */
    hi_u32 size;
    mem_mode mode;
    hi_u64 fd;
} venc_buffer;

typedef unsigned long vedu_lock_flag;
typedef kern_event  vedu_osal_event;

VOID venc_drv_osal_irq_free(hi_u32 Irq);
SINT32 venc_drv_osal_irq_int(hi_u32 Irq, hi_void (*ptrCallBack)(hi_void));
SINT32 venc_drv_osal_lock_create (osal_spinlock** phLock);
VOID venc_drv_osal_lock_destory(osal_spinlock* hLock);
VOID venc_drv_osal_lock(osal_spinlock* hLock, vedu_lock_flag *pFlag);
VOID venc_drv_osal_unlock(osal_spinlock* hLock, vedu_lock_flag *pFlag);
SINT32 venc_drv_osal_create_task(hi_void **phTask, hi_u8 TaskName[], hi_void* pTaskFunction);
SINT32 venc_drv_osal_delete_task(hi_void *hTask);

SINT32 venc_drv_osal_init_event(vedu_osal_event *pEvent, hi_s32 InitVal);
VOID venc_drv_osal_give_event(vedu_osal_event *pEvent);
SINT32 venc_drv_osal_wait_event(vedu_osal_event *pEvent, hi_u32 msWaitTime);

struct file *venc_drv_osal_fopen(const char *filename, int flags, int mode);
VOID venc_drv_osal_fclose(struct file *filp);
SINT32 venc_drv_osal_fwrite(const char *buf, int len, struct file *filp);

SINT32 hi_drv_venc_alloc_and_map(const char *buf_name, hi_u32 cache, hi_u32 len, hi_u32 align, venc_buffer *venc_buf);
VOID hi_drv_venc_unmap_and_release(venc_buffer *venc_buf);
SINT32 hi_drv_venc_map(venc_buffer *venc_buf);
VOID hi_drv_venc_unmap(venc_buffer *venc_buf);

VOID init_venc_interface(hi_void);

#endif  /* __VENC_OSAL_H__ */
