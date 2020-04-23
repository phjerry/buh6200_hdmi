/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: SSM function file for Hisilicon SSM
 * Author: ssm group
 * Create: 2019/12/11
 * Notes:
 */

#ifndef __DRV_SSM_H__
#define __DRV_SSM_H__

#include "hi_drv_ssm.h"

#define SSM_CHECK_POINTER_RETURN_IF_FAIL(pointer) \
    do { \
        if ((pointer) == HI_NULL) { \
            HI_ERROR_SSM("pointer is null\n"); \
            return HI_FAILURE; \
        } \
    } while (0)

typedef struct {
    hi_u64 buf_addr_begin;
    hi_u64 buf_addr_end;
} ssm_teec_buf_addr;

hi_void drv_ssm_error_handler(struct file *ssm_filp);

hi_s32 drv_ssm_teec_init(hi_void);

hi_void drv_ssm_teec_deinit(hi_void);

hi_s32 drv_ssm_teec_create(struct file *ssm_filp, hi_ssm_intent intent, hi_handle *handle);

hi_s32 drv_ssm_teec_destroy(hi_handle ssm_handle);

hi_s32 drv_ssm_teec_add_resource(hi_handle ssm_handle, hi_ssm_resource_info res_info);

hi_s32 drv_ssm_teec_attach_buffer(hi_ssm_buffer_attach_info attach_info, hi_u64 *addr);

hi_s32 drv_ssm_teec_get_intent(hi_handle ssm_handle, hi_ssm_intent *get_intent);

#ifdef SSM_TEST_SUPPORT
hi_s32 drv_ssm_teec_check_buffer(hi_ssm_buffer_check_info check_info);
#endif

#endif
