/******************************************************************************
*
* Copyright (C) 2017 Hisilicon Technologies Co., Ltd.  All rights reserved.
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon),
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
******************************************************************************
File Name     : drv_hifb_fence.c
Version         : Initial Draft
Author          :
Created        : 2017/07/20
Description   :
Function List :


History         :
Date                  Author                 Modification
2017/07/20            sdk                    Created file
******************************************************************************/

/*********************************add include here******************************/
#include <linux/ctype.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/time.h>
#include <linux/fb.h>
#include <linux/version.h>
#include <asm/uaccess.h>
#include <linux/file.h>
#include <asm/types.h>
#include <asm/stat.h>
#include <asm/fcntl.h>
#include <linux/sync_file.h>
#include <linux/syscalls.h>

#include "hi_type.h"
#include "dec_sync.h"
#include "channel.h"

#ifdef HI_DEC_SYNC_SUPPORT
/********************** Global Variable declaration **************************/
dec_sync_info *g_dec_sync_info[MAX_CHANNEL_NUM];

/******************************* API declaration *****************************/

hi_s32 drv_dec_fence_init(hi_s32 id, hi_void *args_in)
{
    hi_char queue_name[256] = {'\0'};
    OMXVDEC_CHAN_CTX *chan_info;

    if (id < 0 || id >= MAX_CHANNEL_NUM) {
        return HI_FAILURE;
    }

    chan_info = (OMXVDEC_CHAN_CTX *)args_in;
    g_dec_sync_info[id] = &chan_info->sync_info;

    if (g_dec_sync_info[id]->lock_inited == HI_FALSE) {
        spin_lock_init(&g_dec_sync_info[id]->lock);
        g_dec_sync_info[id]->lock_inited = HI_TRUE;
    }

    if (chan_info->release_queue == HI_NULL) {
        snprintf(queue_name, sizeof(queue_name), "dec_sync_queue_%d", id);
        queue_name[sizeof(queue_name) - 1] = '\0';

        chan_info->release_queue = create_singlethread_workqueue(queue_name);
        if (chan_info->release_queue == HI_NULL) {
            omx_print(OMX_FATAL, "Create work queue fail\n");
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

hi_void drv_dec_fence_deinit(hi_s32 id, hi_void *args_in)
{
    OMXVDEC_CHAN_CTX *chan_info = HI_NULL;

    if (id < 0 || id >= MAX_CHANNEL_NUM) {
        return;
    }

    if (args_in == HI_NULL) {
        return;
    }

    if (g_dec_sync_info[id] != HI_NULL) {
        g_dec_sync_info[id] = HI_NULL;
    }

    chan_info = (OMXVDEC_CHAN_CTX *)args_in;
    if (chan_info->release_queue != HI_NULL) {
        destroy_workqueue(chan_info->release_queue);
    }

    return;
}

hi_s32 drv_dec_fence_create(hi_s32 id, struct hi_sync_fence **file)
{
    return HI_SUCCESS;
}

hi_s32 drv_dec_get_fd(hi_s32 id)
{
    hi_s32 fence_fd = -1;

    fence_fd = get_unused_fd_flags(O_CLOEXEC);
    if (fence_fd < 0) {
        return HI_FAILURE;
    }

    g_dec_sync_info[id]->release_fence_fd = fence_fd;

    return fence_fd;
}

hi_void drv_dec_fence_bind(hi_s32 fd, struct hi_sync_fence *fence)
{
    if (fence != HI_NULL) {
        hi_sync_fence_install(fence, fd);
    }
}

hi_void drv_dec_fence_destroy(hi_s32 fence_fd)
{
    if (fence_fd < 0) {
        return;
    }

    sys_close(fence_fd);
}
#endif

