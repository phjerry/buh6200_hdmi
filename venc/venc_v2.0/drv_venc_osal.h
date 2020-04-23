/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: encoder
* Author: sdk
* Create: 2019-08-13
*/

#ifndef  __DRV_VENC_OSAL_H__
#define  __DRV_VENC_OSAL_H__

#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/delay.h>

#include "drv_venc_osal_ext.h"

typedef struct {
    osal_wait queue_head;
    hi_s32 flag;
} kern_event;

typedef kern_event vedu_osal_event;

hi_s32 venc_drv_osal_init_event(vedu_osal_event* event, hi_s32 init_val);
hi_s32 venc_drv_osal_deinit_event(vedu_osal_event *event);
hi_void venc_drv_osal_give_event(vedu_osal_event* event);
hi_s32 venc_drv_osal_wait_event(vedu_osal_event* event, hi_u32 ms_wait_time);

#endif /* __DRV_VENC_OSAL_H__ */
