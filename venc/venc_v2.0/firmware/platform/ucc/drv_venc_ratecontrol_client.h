/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2012-04-22
 */
#ifndef __DRV_VENC_RATECONTROL_CLIENT_H__
#define __DRV_VENC_RATECONTROL_CLIENT_H__

#include "drv_venc_ratecontrol.h"
#include "drv_venc_ioctl.h"
#include "hi_type.h"


#define RC_TASK_READY       1
#define RC_TASK_WAIT         0
#define RC_RESULT_READY       1
#define RC_RESULT_WAIT         0

typedef struct {
    vedu_rc rc_client_data;
    hi_s32 task_state;
    hi_s32 result_state;
    hi_s32 rc_calculate_result;
}rc_user_task;

hi_void rc_client_init_task_state(hi_void);
hi_void rc_client_deinit_task_state(hi_void);
hi_s32 rc_client_task_start(vedu_rc *rc_data_src);

hi_s32 rc_client_get_task(venc_ioctl_rc_info *rc_info);
hi_s32 rc_client_set_result(venc_ioctl_rc_info *rc_info);



#endif /* __DRV_VENC_RATECONTROL_CLIENT_H__ */

