/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description:
 * Author: guoqingbo
 * Create: 2009-12-17
 */

#ifndef __DRV_LOG_H__
#define __DRV_LOG_H__

#include "hi_debug.h"
#include "hi_osal.h"

#include "hi_drv_module.h"

#define LOG_DEV_NAME "sys/log"

#define LOG_MAX_FILE_NAME_LENTH 256

#define LOG_MSG_FROM_USER   0
#define LOG_MSG_FROM_KERNEL 1

typedef struct {
    hi_u32 start_phy_addr;        /* start physic address */ /* CNcomment: 缓冲区起始物理地址 */
    hi_u8 *start_virt_addr;       /* start virtual address */ /* CNcomment: 缓冲区起始虚拟地址 */
    hi_u32 buf_size;              /* buffer size */ /* CNcomment: 缓冲区大小 */
    hi_u32 write;                 /* write offset */ /* CNcomment: 写地址偏移 */
    hi_u32 read;                  /* read offset */ /* CNcomment: 读地址偏移 */
    hi_u32 reset_cnt;             /* reset count */ /* CNcomment: 复位次数 */
    hi_u32 write_cnt;             /* write count */ /* CNcomment: 写入次数 */
    osal_wait wq_no_data;         /* wait queque no data */ /* CNcomment: 没有数据等待队列 */
} log_buf_info;

#ifdef HI_PROC_SUPPORT
hi_s32  drv_log_add_proc(hi_void);
hi_void drv_log_remove_proc(hi_void);
#else
static inline hi_s32 drv_log_add_proc(hi_void)
{
    return HI_SUCCESS;
}

static inline hi_void drv_log_remove_proc(hi_void)
{
    return;
}
#endif

#ifdef HI_LOG_SUPPORT
hi_s32  drv_log_init(hi_void);
hi_void drv_log_exit(hi_void);
#else
static inline hi_s32 drv_log_init(hi_void)
{
    return HI_SUCCESS;
}

static inline hi_void drv_log_exit(hi_void)
{
    return;
}
#endif

hi_s32 drv_log_ioctl(unsigned int cmd, hi_void *arg, hi_void *private_data);

#ifdef CONFIG_COMPAT
hi_s32 drv_log_compat_ioctl(unsigned int cmd, hi_void *arg, hi_void *private_data);
#endif

hi_s32  hi_drv_log_add_module(const hi_char *name, hi_u32 module_id);
hi_s32  hi_drv_log_remove_module(hi_u32 module_id);

#endif /* __DRV_LOG_H__ */

