/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __LINUX_PROC__
#define __LINUX_PROC__
#include "hi_drv_sys.h"
#include "hi_osal.h"

#define NAME_SIZE 16

#define OS_VA_LIST  va_list
#define OS_VA_START va_start
#define OS_VA_END   va_end

#define OS_PROC_FILE_S    hi_void
#define OS_PROC_PRINT     osal_proc_print

#define PROC_FILE  OS_PROC_FILE_S

/* #define PROC_PRINT OS_PROC_PRINT */
typedef hi_s32 (*proc_read_fn)(hi_void *seqfile, hi_void *private);

#define OS_PROC_CMD osal_proc_cmd

hi_s32 linux_proc_create(hi_u8 *proc_name, proc_read_fn read, hi_void *cmd_list, hi_u32 cmd_cnt);
hi_void linux_proc_destroy(hi_u8 *proc_name);
hi_s32 linux_proc_init(hi_void);
hi_void linux_proc_exit(hi_void);
hi_void linux_proc_dump(hi_void *page, hi_s32 page_count, hi_s32 *used_bytes, hi_s8 from_shr,
                        const hi_s8 *format, ...);

#endif
