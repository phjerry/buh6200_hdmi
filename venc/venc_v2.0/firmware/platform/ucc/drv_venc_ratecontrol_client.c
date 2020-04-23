/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2012-04-22
 */
#include "drv_venc_ratecontrol_client.h"
#include "drv_venc_efl.h"
#include "linux/hisilicon/securec.h"


#define HI_VENC_CHECK_SEC_FUNC(func) \
{\
    if((func) != EOK) { \
        HI_ERR_VENC("check  err \n"); \
    } \
}

static rc_user_task g_rc_user_task;
vedu_osal_event g_rc_event;
vedu_osal_event g_rc_task_event;
osal_spinlock *g_rc_lock = HI_NULL;

static hi_void rc_client_init_task_struct(hi_void)
{
    vedu_lock_flag flag;
    venc_drv_osal_lock(g_rc_lock, &flag);
    HI_VENC_CHECK_SEC_FUNC(memset_s(&(g_rc_user_task), sizeof(rc_user_task), 0, sizeof(rc_user_task))) ;
    g_rc_user_task.task_state = RC_TASK_WAIT;
    g_rc_user_task.result_state = RC_RESULT_WAIT;
    g_rc_user_task.rc_calculate_result = HI_FAILURE;
    venc_drv_osal_unlock(g_rc_lock, &flag);

    return;
}

static hi_s32 rc_client_get_task_state(hi_void)
{
    hi_s32 task_state;
    vedu_lock_flag flag;
    venc_drv_osal_lock(g_rc_lock, &flag);
    task_state = g_rc_user_task.task_state;
    venc_drv_osal_unlock(g_rc_lock, &flag);

    return task_state;
}

hi_void rc_client_init_task_state(hi_void)
{
    rc_client_init_task_struct();
    venc_drv_osal_init_event(&g_rc_event, 0);
    venc_drv_osal_init_event(&g_rc_task_event, 0);

    return;
}

hi_void rc_client_deinit_task_state(hi_void)
{
    rc_client_init_task_struct();
    venc_drv_osal_deinit_event(&g_rc_event);
    venc_drv_osal_deinit_event(&g_rc_task_event);

    return;
}

static hi_s32 rc_client_get_result_data(vedu_rc *rc_result_dest)
{
    if (rc_result_dest == HI_NULL) {
        HI_ERR_VENC("RC data ptr is NULL!\n");
        return HI_FAILURE;
    }

    if (g_rc_user_task.result_state != RC_RESULT_READY) {
        HI_WARN_VENC("RC result is not ready!\n ");
        return HI_FAILURE;
    }
    HI_VENC_CHECK_SEC_FUNC(memcpy_s(rc_result_dest, sizeof(vedu_rc), &(g_rc_user_task.rc_client_data), \
                           sizeof(vedu_rc)));

    return HI_SUCCESS;
}

hi_s32 rc_client_task_start(vedu_rc *rc_data_src)
{
    vedu_lock_flag flag;
    hi_s32 ret;
    if (rc_data_src == HI_NULL) {
        HI_ERR_VENC("RC data ptr from kernel is NULL!\n");
        return HI_FAILURE;
    }
    venc_drv_osal_lock(g_rc_lock, &flag);
    HI_VENC_CHECK_SEC_FUNC(memcpy_s(&(g_rc_user_task.rc_client_data), sizeof(vedu_rc),
        rc_data_src,  sizeof(vedu_rc)));
    g_rc_user_task.task_state = RC_TASK_READY;
    venc_drv_osal_unlock(g_rc_lock, &flag);

    venc_drv_osal_give_event(&g_rc_task_event);

    ret = venc_drv_osal_wait_event(&g_rc_event, 20); /* 20: wait time 20 ms */
    if (ret != HI_SUCCESS) {
        HI_INFO_VENC("Wait RC calculate time out!\n");
    } else {
        venc_drv_osal_lock(g_rc_lock, &flag);
        if (g_rc_user_task.rc_calculate_result != HI_SUCCESS) {
            HI_WARN_VENC("RC calculate failed or the result didn't set back to the kernel in time.\n");
            ret = HI_FAILURE;
        } else {
            ret = rc_client_get_result_data(rc_data_src);
            if (ret != HI_SUCCESS) {
                HI_ERR_VENC("Get RC result data error.\n");
            }
        }
        venc_drv_osal_unlock(g_rc_lock, &flag);
    }
    rc_client_init_task_struct();

    return ret;
}

hi_s32 rc_client_get_task(venc_ioctl_rc_info *rc_info)
{
    vedu_rc *rc_data = HI_NULL;
    vedu_lock_flag flag;

    if (rc_info == HI_NULL) {
        HI_ERR_VENC("RC Data is NULL\n");
        return HI_FAILURE;
    }

    rc_data = &(rc_info->rc_data);
    if (RC_TASK_READY != rc_client_get_task_state()) {
        if (0 != venc_drv_osal_wait_event(&g_rc_task_event, 300)) { /* 300: wait time 300 ms */
            HI_INFO_VENC("Waiting time out! The task is not ready yet!\n");
            return HI_FAILURE;
        }

        if (RC_TASK_READY != rc_client_get_task_state()) {
            return HI_FAILURE;
        }
    }

    venc_drv_osal_lock(g_rc_lock, &flag);
    HI_VENC_CHECK_SEC_FUNC(memcpy_s(rc_data, sizeof(vedu_rc),
                               &(g_rc_user_task.rc_client_data),  sizeof(vedu_rc)));
    g_rc_user_task.task_state = RC_TASK_WAIT;
    venc_drv_osal_unlock(g_rc_lock, &flag);

    return HI_SUCCESS;
}

hi_s32 rc_client_set_result(venc_ioctl_rc_info *rc_info)
{
    vedu_rc *rc_data = HI_NULL;
    vedu_lock_flag flag;
    hi_s32 ret = HI_SUCCESS;

    if (rc_info == HI_NULL) {
        HI_ERR_VENC("RC calculate result info is NULL\n");
        return  HI_FAILURE;
    }

    rc_data = &(rc_info->rc_data);
    venc_drv_osal_lock(g_rc_lock, &flag);
    if (rc_info->rc_calculate_info != HI_SUCCESS) {
        HI_ERR_VENC("RC Calculate failed, can't set RC result.\n");
        g_rc_user_task.rc_calculate_result = HI_FAILURE;
        ret = HI_FAILURE;
    } else {
        HI_VENC_CHECK_SEC_FUNC(memcpy_s(&(g_rc_user_task.rc_client_data),
                                   sizeof(vedu_rc),  rc_data,  sizeof(vedu_rc)));
        g_rc_user_task.rc_calculate_result = HI_SUCCESS;
        g_rc_user_task.result_state = RC_RESULT_READY;
    }
    venc_drv_osal_unlock(g_rc_lock, &flag);
    venc_drv_osal_give_event(&g_rc_event);

    return ret;
}

