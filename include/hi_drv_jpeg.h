/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description:jpeg module init define
 */

#ifndef __HI_DRV_JPEG_H__
#define __HI_DRV_JPEG_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef hi_s32 (*jpeg_module_init)(hi_void);
typedef hi_void (*jpeg_module_exit)(hi_void);
#ifndef HI_BUILD_IN_BOOT
typedef hi_s32 (*jpeg_module_open)(struct inode *finode, struct file *ffile);
typedef hi_s32 (*jpeg_module_close)(struct inode *finode, struct file *ffile);
#endif
typedef hi_void (*jpeg_module_get_status)(hi_u32 *int_status);

typedef struct {
    jpeg_module_init drv_jpeg_module_init;
    jpeg_module_exit drv_jpeg_module_exit;
#ifndef HI_BUILD_IN_BOOT
    jpeg_module_open drv_jpeg_module_open;
    jpeg_module_close drv_jpeg_module_close;
#endif
    jpeg_module_get_status drv_jpeg_module_get_status;
} hi_jpeg_export_func;

#ifndef HI_BUILD_IN_BOOT
hi_s32 hi_drv_jpeg_module_init(hi_void);
hi_s32 hi_drv_jpeg_module_register(hi_void);
hi_void hi_drv_jpeg_module_exit(hi_void);
#else
hi_void hi_drv_jpeg_dev_open(hi_void);
hi_void hi_drv_jpeg_dev_close(hi_void);
hi_void hi_drv_jpeg_start_decompress(hi_void *arg);
hi_void hi_drv_jpeg_get_dec_status(hi_u32 *status);
hi_void hi_drv_jpeg_set_dec_status(hi_u32 status);
hi_void hi_drv_jpeg_cfg_int_mask(hi_void);
hi_void hi_drv_jpeg_set_reg_base(hi_void);

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
