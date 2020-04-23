/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: SSM function file for Hisilicon SSM
 * Author: ssm group
 * Create: 2019/12/11
 * Notes:
 */

#ifndef __DRV_SSM_INTF_H__
#define __DRV_SSM_INTF_H__

#include "hi_drv_ssm.h"

typedef struct {
    hi_handle       ssm_handle;
    hi_ssm_intent   intent;
} ssm_ioctl_create;

typedef struct {
    hi_u32    ssm_handle;
} ssm_ioctl_destroy;

typedef struct {
    hi_ssm_resource_info    res_info;
    hi_handle               ssm_handle;
} ssm_ioctl_add_resource;

typedef struct {
    hi_ssm_buffer_attach_info attach_info;
    hi_u64                    secure_info_addr;
} ssm_ioctl_attach_buffer;

typedef struct {
    hi_handle      ssm_handle;
    hi_ssm_intent  intent;
} ssm_ioctl_get_intent;

#ifdef SSM_TEST_SUPPORT
typedef struct {
    hi_ssm_buffer_check_info check_info;
} ssm_ioctl_check_info;
#endif

#define CMD_SSM_CREATE           _IOWR(HI_ID_SSM, 0, ssm_ioctl_create)
#define CMD_SSM_DESTROY          _IOWR(HI_ID_SSM, 1, ssm_ioctl_destroy)
#define CMD_SSM_ADD_RESOURCE     _IOWR(HI_ID_SSM, 2, ssm_ioctl_add_resource)
#define CMD_SSM_ATTACH_BUFFER    _IOWR(HI_ID_SSM, 3, ssm_ioctl_attach_buffer)
#define CMD_SSM_GET_INTENT       _IOWR(HI_ID_SSM, 4, ssm_ioctl_get_intent)

#ifdef SSM_TEST_SUPPORT
#define CMD_SSM_CHECK_BUF_INFO   _IOWR(HI_ID_SSM, 5, ssm_ioctl_check_info)
#endif

#endif
