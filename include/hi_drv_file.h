/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description:
 * Author: guoqingbo
 * Create: 2012/6/19
 */

#ifndef __HI_DRV_FILE_H__
#define __HI_DRV_FILE_H__

#include <linux/fs.h>

#include "hi_type.h"

struct file *hi_drv_file_open(const hi_char *name, hi_u32 flags);
hi_void hi_drv_file_close(struct file *file);
hi_s32  hi_drv_file_read(struct file *file, hi_u8 *buf, const hi_u32 len);
hi_s32  hi_drv_file_write(struct file *file, const hi_u8 *buf, const hi_u32 len);
loff_t  hi_drv_file_lseek(struct file *file, const hi_s32 offset, const hi_s32 whence);
hi_s32  hi_drv_file_get_store_path(hi_char *path, const hi_u32 len);

#endif /* __HI_DRV_FILE_H__ */

