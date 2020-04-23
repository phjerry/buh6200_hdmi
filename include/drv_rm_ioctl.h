/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: drv_rm_ioctl.h
 * Author: sm_rm
 * Create: 2015/11/25
 */

#ifndef __DRV_RM_IOCTL_H__
#define __DRV_RM_IOCTL_H__

#include "hi_drv_rm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_handle    rm_handle;
    hi_u32       pid;
} rm_create;

typedef struct {
    hi_handle          rm_handle;
    hi_drv_rm_event    rm_event;
} rm_event;

typedef struct {
    hi_handle    win_handle;
    hi_u32       pid;
} rm_win_info;

typedef enum {
    IOC_RM_CREATE = 0,
    IOC_RM_DESTROY,
    IOC_RM_ENABLE,
    IOC_RM_DISABLE,
    IOC_RM_QUERY,
    IOC_RM_ACQUIREWIN,
    IOC_RM_MAX,
} ioc_rm;

#define CMD_RM_CREATE        _IOWR(HI_ID_RM, IOC_RM_CREATE, rm_create)
#define CMD_RM_DESTROY       _IOWR(HI_ID_RM, IOC_RM_DESTROY, hi_handle)
#define CMD_RM_ENABLE        _IOWR(HI_ID_RM, IOC_RM_ENABLE, hi_handle)
#define CMD_RM_DISABLE       _IOWR(HI_ID_RM, IOC_RM_DISABLE, hi_handle)
#define CMD_RM_QUERY         _IOWR(HI_ID_RM, IOC_RM_QUERY, rm_event)
#define CMD_RM_ACQUIREWIN    _IOWR(HI_ID_RM, IOC_RM_ACQUIREWIN, rm_win_info)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif //__DRV_RM_IOCTL_H__

