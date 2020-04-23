/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: Interaction between the PVR kernel mode and user mode.
 * Author: sdk
 * Create: 2019-08-16
 */

#include "hi_osal.h"
#include <linux/module.h>
#include "hi_type.h"
#include "hi_debug.h"
#include "hi_errno.h"
#include "linux/hisilicon/securec.h"
#include "hi_drv_module.h"
#include "hi_drv_sys.h"
#include "hi_drv_dev.h"
#include "hi_drv_pvr.h"
#include "drv_pvr_debug.h"
#ifdef HI_TEE_PVR_SUPPORT
#include "drv_pvr_tee_func.h"
#endif

#define PVR_NAME                "HI_PVR"
static pvr_chan_info            g_pvr_play_info[PVR_PLAY_MAX_CHN_NUM];
static pvr_chan_info            g_pvr_rec_info[PVR_REC_MAX_CHN_NUM];
static hi_u32                   g_pvr_decode_ability;

static osal_semaphore g_pvr_mutex = {HI_NULL};

static hi_s32 hi_drv_pvr_create_play(hi_u32 *chn)
{
    hi_u32 i = 0;

    for (i = 0; i < PVR_PLAY_MAX_CHN_NUM; i++) {
        if (g_pvr_play_info[i].is_use == HI_FALSE) {
            g_pvr_play_info[i].is_use = HI_TRUE;
            g_pvr_play_info[i].owner = task_tgid_nr(current);
            g_pvr_play_info[i].ability_used = 0;
            *chn = i;
            return HI_SUCCESS;
        }
    }

    hi_err_pvr("Create Play Chanel failed!\n");
    return HI_ERR_PVR_NO_CHN_LEFT;
}

static hi_s32 hi_drv_pvr_destroy_play(const hi_u32 chn)
{
    if (chn >= PVR_PLAY_MAX_CHN_NUM) {
        hi_err_pvr("invalid chanid(%u)! \n", chn);
        return HI_FAILURE;
    }

    if (g_pvr_play_info[chn].is_use == HI_TRUE) {
        g_pvr_play_info[chn].is_use = HI_FALSE;
        g_pvr_play_info[chn].owner = 0;
        g_pvr_decode_ability += g_pvr_play_info[chn].ability_used;
        g_pvr_play_info[chn].ability_used = 0;
    } else {
        hi_warn_pvr("Play Chanel(%u) has already released! uesd: %d\n", chn, g_pvr_play_info[chn].is_use);
    }

    return HI_SUCCESS;
}

static hi_s32 hi_drv_pvr_create_rec(hi_u32 *chn)
{
    hi_u32 i = 0;

    for (i = 0; i < PVR_REC_MAX_CHN_NUM; i++) {
        if (g_pvr_rec_info[i].is_use == HI_FALSE) {
            g_pvr_rec_info[i].is_use = HI_TRUE;
            g_pvr_rec_info[i].owner = task_tgid_nr(current);
            g_pvr_rec_info[i].ability_used = 0;
            *chn = i;
            return HI_SUCCESS;
        }
    }

    hi_err_pvr("Create Rec Chanel failed!\n");
    return HI_ERR_PVR_NO_CHN_LEFT;
}

static hi_s32 hi_drv_pvr_destroy_rec(const hi_u32 chn)
{
    if (chn >= PVR_REC_MAX_CHN_NUM) {
        hi_err_pvr("invalid chan id (%u)! \n", chn);
        return HI_FAILURE;
    }

    if (g_pvr_rec_info[chn].is_use == HI_TRUE) {
        g_pvr_rec_info[chn].is_use = HI_FALSE;
        g_pvr_rec_info[chn].owner = 0;
        g_pvr_rec_info[chn].ability_used = 0;
    } else {
        hi_warn_pvr("Play Chanel(%u) has already released! uesd: %d\n", chn, g_pvr_rec_info[chn].is_use);
    }

    return HI_SUCCESS;
}

static hi_s32 hi_drv_pvr_acquire_decode_ability(hi_void *private_data, pvr_decode_ability_info *decode_ability_info)
{
    pid_t pid = (pid_t)(uintptr_t) (*((struct pvr_intf **)private_data));

    if (decode_ability_info->chn >= PVR_PLAY_MAX_CHN_NUM) {
        hi_err_pvr("invalid ChanId(%u)! \n", decode_ability_info->chn);
        return HI_FAILURE;
    }

    if ((pid == g_pvr_play_info[decode_ability_info->chn].owner) &&
        (g_pvr_decode_ability >= decode_ability_info->value)) {
        g_pvr_play_info[decode_ability_info->chn].ability_used += decode_ability_info->value;
        g_pvr_decode_ability -= decode_ability_info->value;
        return HI_SUCCESS;
    }

    hi_err_pvr("get DecodeAbility failed! need/surplus(%u/%u)\n", decode_ability_info->value, g_pvr_decode_ability);
    return HI_FAILURE;
}

static hi_s32 hi_drv_pvr_release_decode_ability(hi_void *private_data, pvr_decode_ability_info *decode_ability_info)
{
    pid_t pid = (pid_t)(uintptr_t) (*((struct pvr_intf **)private_data));

    if (decode_ability_info->chn >= PVR_PLAY_MAX_CHN_NUM) {
        hi_err_pvr("invalid ChanId(%u)! \n", decode_ability_info->chn);
        return HI_FAILURE;
    }

    if ((pid == g_pvr_play_info[decode_ability_info->chn].owner)
        && (decode_ability_info->value <= g_pvr_play_info[decode_ability_info->chn].ability_used)) {
        g_pvr_play_info[decode_ability_info->chn].ability_used -= decode_ability_info->value;
        g_pvr_decode_ability += decode_ability_info->value;
        return HI_SUCCESS;
    }

    hi_err_pvr("release DecodeAbility failed! release/surplus(%u/%u)\n",
        decode_ability_info->value, g_pvr_decode_ability);
    return HI_FAILURE;
}

static hi_s32 hi_drv_pvr_drv_open(hi_void *private_data)
{
#ifdef HI_TEE_PVR_SUPPORT
    hi_s32 ret;
#endif

    *((struct pvr_intf**)private_data) = (void *)(uintptr_t)task_tgid_nr(current);

#ifdef HI_TEE_PVR_SUPPORT
    ret = pvr_teec_init();
    if (ret != HI_SUCCESS) {
        HI_PRINT("Initize the tee failed, ret = 0x%08x\n", ret);
        return ret;
    }
#endif

    return 0;
}

static hi_s32 hi_drv_pvr_drv_close(hi_void *private_data)
{
    hi_u32 i;
    pid_t pid = (pid_t) (uintptr_t) (*((struct pvr_intf **)private_data));

    for (i = 0; i < PVR_PLAY_MAX_CHN_NUM; i++) {
        if (pid == g_pvr_play_info[i].owner) {
            (hi_void)hi_drv_pvr_destroy_play(i);
        }
    }

    for (i = 0; i < PVR_REC_MAX_CHN_NUM; i++) {
        if (pid == g_pvr_rec_info[i].owner) {
            (hi_void)hi_drv_pvr_destroy_rec(i);
        }
    }
#ifdef HI_TEE_PVR_SUPPORT
    pvr_tee_release(private_data);
    (hi_void)pvr_teec_deinit();
#endif

    return 0;
}

static hi_s32 pvr_create_play(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_pvr_mutex);
    if (ret != HI_SUCCESS) {
        hi_err_pvr("osal_sem_down failed\n");
        return HI_FAILURE;
    }

    ret = hi_drv_pvr_create_play((hi_u32 *)arg);

    osal_sem_up(&g_pvr_mutex);
    return ret;
}

static hi_s32 pvr_destroy_play(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_pvr_mutex);
    if (ret != HI_SUCCESS) {
        hi_err_pvr("osal_sem_down failed\n");
        return HI_FAILURE;
    }

    ret = hi_drv_pvr_destroy_play(*((hi_u32 *)arg));

    osal_sem_up(&g_pvr_mutex);
    return ret;
}

static hi_s32 pvr_decode_ability(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    *((hi_u32 *)arg) = g_pvr_decode_ability;
    return 0;
}

static hi_s32 pvr_acquire_decode_ability(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    pvr_decode_ability_info *decode_ability_info = (pvr_decode_ability_info *)arg;

    ret = osal_sem_down_interruptible(&g_pvr_mutex);
    if (ret != HI_SUCCESS) {
        hi_err_pvr("osal_sem_down failed\n");
        return HI_FAILURE;
    }

    ret = hi_drv_pvr_acquire_decode_ability(private_data, decode_ability_info);

    osal_sem_up(&g_pvr_mutex);
    return ret;
}

static hi_s32 pvr_release_decode_ability(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    pvr_decode_ability_info *decode_ability_info = (pvr_decode_ability_info *)arg;

    ret = osal_sem_down_interruptible(&g_pvr_mutex);
    if (ret != HI_SUCCESS) {
        hi_err_pvr("osal_sem_down failed\n");
        return HI_FAILURE;
    }

    ret = hi_drv_pvr_release_decode_ability(private_data, decode_ability_info);

    osal_sem_up(&g_pvr_mutex);
    return ret;
}

static hi_s32 pvr_create_rec(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_pvr_mutex);
    if (ret != HI_SUCCESS) {
        hi_err_pvr("osal_sem_down failed\n");
        return HI_FAILURE;
    }

    ret = hi_drv_pvr_create_rec((hi_u32 *)arg);

    osal_sem_up(&g_pvr_mutex);
    return ret;
}

static hi_s32 pvr_destroy_rec(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_pvr_mutex);
    if (ret != HI_SUCCESS) {
        hi_err_pvr("osal_sem_down failed\n");
        return HI_FAILURE;
    }
    ret = hi_drv_pvr_destroy_rec(*((hi_u32 *)arg));

    osal_sem_up(&g_pvr_mutex);
    return ret;
}

#ifdef HI_TEE_PVR_SUPPORT
static hi_s32 pvr_teec_ioctl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    ret = osal_sem_down_interruptible(&g_pvr_mutex);
    if (ret != 0) {
        hi_err_pvr("down_interruptible failed, ret = %d\n", ret);
        return ret;
    }
    ret = hi_drv_pvr_teec_ioctl(private_data, cmd, arg);
    osal_sem_up(&g_pvr_mutex);
    return ret;
}
#endif

static hi_s32 pvr_drv_open(hi_void *private_data)
{
    return hi_drv_pvr_drv_open(private_data);
}

static hi_s32 pvr_drv_close(hi_void *private_data)
{
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_pvr_mutex);
    if (ret != 0) {
        hi_err_pvr("down_interruptible failed, ret = %d\n", ret);
        return ret;
    }

    ret = hi_drv_pvr_drv_close(private_data);

    osal_sem_up(&g_pvr_mutex);
    return ret;
}

static osal_ioctl_cmd g_pvr_cmd_list[] = {
#ifdef HI_TEE_PVR_SUPPORT
    {CMD_PVR_TEE_REC_OPEN,               pvr_teec_ioctl},
    {CMD_PVR_TEE_REC_CLOSE,              pvr_teec_ioctl},
    {CMD_PVR_TEE_REC_COPY_REE_TEST,      pvr_teec_ioctl},
    {CMD_PVR_TEE_REC_PROCESS_TS_DATA,    pvr_teec_ioctl},
    {CMD_PVR_TEE_REC_GET_STATE,          pvr_teec_ioctl},
    {CMD_PVR_TEE_REC_GET_ADDRINFO,       pvr_teec_ioctl},
    {CMD_PVR_TEE_PLAY_COPY_FROM_REE,     pvr_teec_ioctl},
    {CMD_PVR_TEE_PLAY_PROCESS_DATA,      pvr_teec_ioctl},
#endif
    {CMD_PVR_CREATE_PLAY_CHN,            pvr_create_play},
    {CMD_PVR_DESTROY_PLAY_CHN,           pvr_destroy_play},
    {CMD_PVR_GET_DECODE_ABILITY,         pvr_decode_ability},
    {CMD_PVR_ACQUIRE_DECODE_ABILITY,     pvr_acquire_decode_ability},
    {CMD_PVR_RELEASE_DECODE_ABILITY,     pvr_release_decode_ability},
    {CMD_PVR_CREATE_REC_CHN,             pvr_create_rec},
    {CMD_PVR_DESTROY_REC_CHN,            pvr_destroy_rec}
};

static osal_fileops g_pvr_file_ops = {
    .open           = pvr_drv_open,
    .release        = pvr_drv_close,
    .cmd_list       = g_pvr_cmd_list,
    .cmd_cnt        = sizeof(g_pvr_cmd_list) / sizeof(osal_ioctl_cmd),
};

static osal_dev g_pvr_dev = {
    .name = HI_DEV_PVR_NAME,
    .minor = HI_DEV_PVR_MINOR,
    .fops = &g_pvr_file_ops,
};

hi_s32 pvr_drv_mod_init(hi_void)
{
    hi_s32 ret;
    hi_u32 i;

    ret = osal_sem_init(&g_pvr_mutex, 1);
    if (ret != HI_SUCCESS) {
        hi_err_pvr("osal_sem_init failed\n");
        return HI_FAILURE;
    }

    ret = osal_exportfunc_register(HI_ID_PVR, PVR_NAME, HI_NULL);
    if (ret != HI_SUCCESS) {
        osal_sem_destory(&g_pvr_mutex);
        return HI_FAILURE;
    }

    if (osal_dev_register(&g_pvr_dev) != 0) {
        hi_fatal_pvr("register failed\n");
        osal_dev_unregister(&g_pvr_dev);
        osal_sem_destory(&g_pvr_mutex);
        return HI_FAILURE;
    }

    for (i = 0; i < PVR_PLAY_MAX_CHN_NUM; i++) {
        g_pvr_play_info[i].is_use = HI_FALSE;
        g_pvr_play_info[i].owner = 0;
        g_pvr_play_info[i].ability_used = 0;
    }

    for (i = 0; i < PVR_REC_MAX_CHN_NUM; i++) {
        g_pvr_rec_info[i].is_use = HI_FALSE;
        g_pvr_rec_info[i].owner = 0;
        g_pvr_rec_info[i].ability_used = 0;
    }

    g_pvr_decode_ability = PVR_PLAY_DECODE_ABILITY;

#ifdef MODULE
    HI_PRINT("Load hi_pvr.ko success.  \t(%s)\n", VERSION_STRING);
#endif

    return HI_SUCCESS;
}

hi_void pvr_drv_mod_exit(hi_void)
{
    osal_exportfunc_unregister(HI_ID_PVR);
    osal_dev_unregister(&g_pvr_dev);
    osal_sem_destory(&g_pvr_mutex);
}

#ifdef MODULE
module_init(pvr_drv_mod_init);
module_exit(pvr_drv_mod_exit);
#endif

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");

