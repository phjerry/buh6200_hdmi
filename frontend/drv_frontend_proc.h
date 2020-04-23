/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: frontend proc
 * Author: SDK
 * Created: 2017-06-30
 */
#ifndef __DRV_FRONTEND_PROC_H__
#define __DRV_FRONTEND_PROC_H__

#include "hi_drv_proc.h"
#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */

#define PROC_CMD_BUFFER_LEN 128
#define PROC_SIGNALINFO_LEN 20

hi_s32 drv_frontend_proc_read(hi_void *seqfile, hi_void *private);
hi_s32 drv_frontend_proc_read_reg(hi_void *seqfile, hi_void *private);
hi_s32 fe_drv_proc_add(hi_void);
hi_s32 fe_drv_proc_reg_add(hi_void);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif /* __cplusplus */

#endif

