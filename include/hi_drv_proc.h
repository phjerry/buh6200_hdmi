/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2006-2019. All rights reserved.
 * Description:
 * Author: guoqingbo
 * Create: 2006-8-3
 */

#ifndef __HI_DRV_PROC_H__
#define __HI_DRV_PROC_H__

#include <linux/seq_file.h>

#include "hi_type.h"

#ifdef HI_PROC_SUPPORT
#define HI_PROC_PRINT(arg...) seq_printf(arg)

hi_void hi_drv_proc_echo_helper(const hi_char *fmt, ...);
hi_void hi_drv_proc_echo_helper_vargs(hi_char *buf, hi_u32 size, const hi_char *fmt, va_list args);

#else
#define HI_PROC_PRINT(arg...) do { \
} while (0)

static inline hi_void hi_drv_proc_echo_helper(const hi_char *fmt, ...) { }
static inline hi_void hi_drv_proc_echo_helper_vargs(hi_char *buf, hi_u32 size, const hi_char *fmt, va_list args) { }
#endif

#define HI_PROC_MAX_ENTRY_NAME_LEN (31)

typedef hi_s32 (*hi_proc_ctrl_fn)(hi_u32, hi_u32);
typedef hi_s32 (*hi_proc_read_fn)(struct seq_file *, hi_void *);
typedef hi_s32 (*hi_proc_write_fn)(struct file *file, const char __user *buf, size_t count, loff_t *pos);
typedef hi_s32 (*hi_proc_ioctl_fn)(struct seq_file *, hi_u32 cmd, hi_u32 arg);

typedef struct {
    hi_char                entry_name[HI_PROC_MAX_ENTRY_NAME_LEN + 1];
    struct proc_dir_entry *entry;
    hi_proc_read_fn        read;
    hi_proc_write_fn       write;
    hi_proc_ioctl_fn       ioctl;
    hi_void               *data;
} hi_proc_item;

typedef struct {
    hi_proc_read_fn  read;
    hi_proc_write_fn write;
    hi_proc_ioctl_fn ioctl;
} hi_proc_fn_set;

typedef hi_proc_item *(*hi_proc_add_module_fn)(const hi_char *, hi_proc_fn_set *, hi_void *);
typedef hi_void (*hi_proc_remove_module_fn)(const hi_char *);

typedef struct {
    hi_proc_add_module_fn add_module_fn;
    hi_proc_remove_module_fn remove_module_fn;
} hi_proc_module_fn;

hi_s32  hi_drv_proc_register_para(hi_proc_module_fn *para);
hi_void hi_drv_proc_unregister_para(hi_void);

ssize_t hi_drv_proc_module_write(struct file *file, const char __user *buf, size_t count,
    loff_t *pos, hi_proc_ctrl_fn ctrl_fn);

hi_proc_item *hi_drv_proc_add_module(const hi_char *, hi_proc_fn_set *, hi_void *);
hi_void hi_drv_proc_remove_module(const hi_char *);

#endif /* __HI_DRV_PROC_H__ */

