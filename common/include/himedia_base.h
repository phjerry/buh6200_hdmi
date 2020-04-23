/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author: p00370051
* Create: 2012-12-22
* Notes:
* History: 2019-03-29 p00370051 CSEC
*/
#ifndef _HIMEDIA_BASE_H_
#define _HIMEDIA_BASE_H_

#include "himedia.h"

// bus
int himedia_bus_init(void);
void himedia_bus_exit(void);

// device
int himedia_device_register(pm_basedev *pdev);
void himedia_device_unregister(pm_basedev *pdev);
int himedia_device_add(pm_basedev *pdev);
void himedia_device_del(pm_basedev *pdev);
void himedia_device_put(pm_basedev *pdev);
pm_basedev *himedia_device_alloc(const char *name, int id);

// driver
int himedia_driver_register(pm_basedrv *drv);
void himedia_driver_unregister(pm_basedrv *drv);
void himedia_driver_release(pm_basedrv *drv);
pm_basedrv *himedia_driver_alloc(const char *name, struct module *owner,
    pm_baseops *ops);

#endif
