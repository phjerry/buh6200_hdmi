/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "vfmw_intf.h"
#include "dbg.h"
#include "vfmw_osal.h"
#include "vfmw_adpt.h"
#include "hi_osal.h"

static osal_atomic g_mod_init = {NULL};

vfmw_export_fun g_vfmw_export_funcs = {
    .fn_vfmw_init = vfmw_init,
    .fn_vfmw_exit = vfmw_exit,
    .fn_vfmw_suspend = vfmw_suspend,
    .fn_vfmw_resume = vfmw_resume,
    .fn_vfmw_get_frame = vfmw_get_image,
    .fn_vfmw_rels_frame = vfmw_release_image,
    .fn_vfmw_control = vfmw_control,
};

hi_s32 vfmw_init(hi_void *args)
{
    hi_s32 ret;

    ret = vadpt_init(args);

    return ret;
}

hi_s32 vfmw_exit(hi_void *args)
{
    hi_s32 ret;

    ret = vadpt_exit(args);

    return ret;
}

hi_s32 vfmw_suspend(hi_void *args)
{
    hi_s32 ret;

    ret = vadpt_suspend(args);

    return ret;
}

hi_s32 vfmw_resume(hi_void *args)
{
    hi_s32 ret;

    ret = vadpt_resume(args);

    return ret;
}

hi_s32 vfmw_get_image(hi_s32 chan_id, hi_void *image, hi_void *ext_frm_info)
{
    hi_s32 ret;

    ret = vadpt_get_frame(chan_id, image, ext_frm_info);

    return ret;
}

hi_s32 vfmw_release_image(hi_s32 chan_id, const hi_void *image)
{
    hi_s32 ret;

    ret = vadpt_release_frame(chan_id, image);

    return ret;
}

hi_s32 vfmw_control(hi_s32 chan_id, hi_s32 cmd, hi_void *args, hi_u32 len)
{
    hi_s32 ret;

    ret = vadpt_control(chan_id, cmd, args, len);

    return ret;
}

/* os will check if Init */
hi_s32 vfmw_mod_init(hi_void)
{
    if (osal_atomic_init(&g_mod_init) != HI_SUCCESS) {
        return HI_SUCCESS;
    }

    osal_atomic_set(&g_mod_init, 0);

    if (osal_atomic_inc_return(&g_mod_init) != 1) {
        return HI_SUCCESS;
    }

    vadpt_open();

    return HI_SUCCESS;
}

hi_void vfmw_mod_exit(hi_void)
{
    vadpt_close();

    osal_atomic_set(&g_mod_init, 0);

    osal_atomic_destory(&g_mod_init);

    return;
}


