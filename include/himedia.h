/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: Implement all function for add UMAP device into OS.
* Author: p00370051
* Create: 2006-03-24
* Notes:
* History: 2019-03-29 p00370051 CSEC
*/

#ifndef __HIMEDIA_H__
#define __HIMEDIA_H__

#include <linux/module.h>
#include <linux/device.h>
#include <linux/major.h>
#include <asm/types.h>
#include <linux/fs.h>
#include <asm/atomic.h>

#include "hi_type.h"
#include "hi_module.h"

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

#define HIMEDIA_DEVICE_NAME_MAX_LEN 64

#define HIMEDIA_DEVICE_MAJOR  218
#define HIMEDIA_DYNAMIC_MINOR 255

typedef struct tag_pm_basedev {
    hi_s32 id;
    const hi_s8 *name;
    struct device dev;
} pm_basedev;

#define TO_PM_BASEDEV(x) container_of((x), pm_basedev, dev)

typedef struct tag_pm_baseops {
    hi_s32 (*probe)(pm_basedev *);
    hi_s32 (*remove)(pm_basedev *);
    hi_void (*shutdown)(pm_basedev *);
    hi_s32 (*prepare)(pm_basedev *);
    hi_void (*complete)(pm_basedev *);
    hi_s32 (*suspend)(pm_basedev *, pm_message_t state);
    hi_s32 (*suspend_late)(pm_basedev *, pm_message_t state);
    hi_s32 (*resume_early)(pm_basedev *);
    hi_s32 (*resume)(pm_basedev *);
} pm_baseops;

typedef struct tag_pm_basedrv {
    hi_s32(*probe)
    (pm_basedev *);
    hi_s32 (*remove)(pm_basedev *);
    hi_void (*shutdown)(pm_basedev *);
    hi_s32 (*suspend)(pm_basedev *, pm_message_t state);
    hi_s32 (*suspend_late)(pm_basedev *, pm_message_t state);
    hi_s32 (*resume_early)(pm_basedev *);
    hi_s32 (*resume)(pm_basedev *);
    struct device_driver driver;
} pm_basedrv;

#define to_himedia_basedrv(drv) container_of((drv), pm_basedrv, driver)

typedef struct hi_umap_device {
    hi_char devfs_name[HIMEDIA_DEVICE_NAME_MAX_LEN]; /* devfs */
    hi_s32 minor;
    struct module *owner;
    struct file_operations *fops;
    pm_baseops *drvops;
} umap_device, *ptr_umap_device;

typedef struct tag_pm_device {
    hi_u32 minor;
    const hi_s8 *name;
    struct module *owner;
    const struct file_operations *app_ops;
    pm_baseops *base_ops;
    struct list_head list;
    struct device *app_device;
    pm_basedev *base_device;
    pm_basedrv *base_driver;
    umap_device *umap_dev;
    hi_void *dev;
} pm_device;


hi_s32 drv_pm_mod_init(hi_void);
hi_void drv_pm_mod_exit(hi_void);
hi_s32 hi_drv_pm_register(pm_device *);
hi_s32 hi_drv_pm_un_register(pm_device *);

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif /* __HIMEDIA_H__ */

