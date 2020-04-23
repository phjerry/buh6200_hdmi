/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description:  used for multi-win operation,such as add/del,  look-up, disp/virt/ai win relation
                       -ship management.
* Author: image
* Create: 2019-04-12
 */

#include "hi_type.h"
#include "hi_errno.h"

#include "hi_drv_video.h"
#include "hi_drv_disp.h"
#include "hi_drv_win.h"
#include "osal_list.h"
#include "drv_xdp_osal.h"
#include "drv_window_priv.h"
#include "drv_disp_isr.h"
#include "linux/hisilicon/securec.h"

win_management_info g_win_manage_instance = { 0 };

/* should consider that */
hi_s32 winmanage_init(hi_void)
{
    hi_s32 ret;

    memset_s(&g_win_manage_instance,
             sizeof(g_win_manage_instance),
             0,
             sizeof(g_win_manage_instance));

    ret = osal_spin_lock_init(&g_win_manage_instance.cross_win_spinlock);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("winmanage lock initiation failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 winmanage_deinit(hi_void)
{
    /* 1) register cross-win isr function.
       2) pay attention: xdp ctrl is cross-win-gfx, so if no window, isr is needed yet */
    osal_spin_lock_destory(&g_win_manage_instance.cross_win_spinlock);

    memset_s(&g_win_manage_instance,
             sizeof(g_win_manage_instance),
             0,
             sizeof(g_win_manage_instance));

    return HI_SUCCESS;
}

hi_s32 winmanage_allocate_handle(hi_drv_display disp_chn, hi_handle *win_handle)
{
    hi_u32 i = 0;
    hi_ulong lock_flags = 0;
    hi_s32 ret = HI_FAILURE;

    osal_spin_lock_irqsave(&g_win_manage_instance.cross_win_spinlock, &lock_flags);

    for (i = 0; i < WINDOW_MAX_NUMBER; i++) {
        if (!g_win_manage_instance.win_array[(hi_u32)disp_chn][i]) {
            *win_handle = HI_HANDLE_INIT(HI_ID_WIN, disp_chn,
                                         ((g_win_manage_instance.win_creat_count & 0xff) << 8) | i); /* 8 is digit */
            ret = HI_SUCCESS;
            break;
        }
    }

    if (i >= WINDOW_MAX_NUMBER) {
        WIN_ERROR("win num reach max limit.\n");
        ret = HI_ERR_VO_BUFQUE_FULL;
    }

    osal_spin_unlock_irqrestore(&g_win_manage_instance.cross_win_spinlock, &lock_flags);
    return ret;
}

hi_s32 winmanage_add_window(win_descriptor *win_descp,
                                        hi_drv_display disp_chn)
{
    hi_u32 i = 0;
    hi_ulong lock_flags = 0;

    i = HI_HANDLE_GET_CHAN_ID(win_descp->win_index) & 0x00ff;
    disp_chn = (hi_u32)HI_HANDLE_GET_PRIVATE_DATA(win_descp->win_index);

    if ((HI_HANDLE_GET_MODULE_ID(win_descp->win_index) != HI_ID_WIN) ||
        (disp_chn >= HI_DRV_DISPLAY_BUTT) ||
        (i > WINDOW_MAX_NUMBER)) {
        WIN_ERROR("Invalid window index = 0x%x\n", win_descp->win_index);
        return HI_ERR_VO_WIN_NOT_EXIST;
    }

    osal_spin_lock_irqsave(&g_win_manage_instance.cross_win_spinlock, &lock_flags);

    g_win_manage_instance.win_array[(hi_u32)disp_chn][i] = win_descp;
    g_win_manage_instance.win_handle_array[(hi_u32)disp_chn][i] = win_descp->win_index;
    g_win_manage_instance.current_win_num[(hi_u32)disp_chn]++;
    g_win_manage_instance.win_creat_count++;

    osal_spin_unlock_irqrestore(&g_win_manage_instance.cross_win_spinlock, &lock_flags);
    return HI_SUCCESS;
}

hi_s32 winmanage_del_window(hi_u32 win_handle)
{
    hi_u32 i;
    hi_ulong lock_flags = 0;
    hi_drv_display disp_chn;

    i = HI_HANDLE_GET_CHAN_ID(win_handle) & 0x00ff;
    disp_chn = (hi_u32)HI_HANDLE_GET_PRIVATE_DATA(win_handle);

    if ((HI_HANDLE_GET_MODULE_ID(win_handle) != HI_ID_WIN) ||
        (disp_chn >= HI_DRV_DISPLAY_BUTT) ||
        (i > WINDOW_MAX_NUMBER)) {
        WIN_ERROR("Invalid window index = 0x%x\n", win_handle);
        return HI_ERR_VO_WIN_NOT_EXIST;
    }

    osal_spin_lock_irqsave(&g_win_manage_instance.cross_win_spinlock, &lock_flags);
    if (g_win_manage_instance.win_array[disp_chn][i]) {
        g_win_manage_instance.win_array[disp_chn][i] = HI_NULL;
        g_win_manage_instance.win_handle_array[disp_chn][i] = 0;
        WIN_INFO("del valid win: 0x%x, chan:%d,index:%d\n", win_handle, disp_chn, i);
        g_win_manage_instance.current_win_num[disp_chn]--;
    }

    osal_spin_unlock_irqrestore(&g_win_manage_instance.cross_win_spinlock, &lock_flags);
    return HI_SUCCESS;
}

win_descriptor *winmanage_get_window(hi_u32 win_handle)
{
    hi_u32 i;
    hi_drv_display disp_chn;

    i = HI_HANDLE_GET_CHAN_ID(win_handle) & 0x00ff;
    disp_chn = (hi_u32)HI_HANDLE_GET_PRIVATE_DATA(win_handle);

    if ((HI_HANDLE_GET_MODULE_ID(win_handle) != HI_ID_WIN) ||
        (disp_chn >= HI_DRV_DISPLAY_BUTT) ||
        (i > WINDOW_MAX_NUMBER)) {
        WIN_ERROR("Invalid window index = 0x%x,i:%d, chn:%d, module:%d\n", win_handle,
                  i, disp_chn, (HI_HANDLE_GET_MODULE_ID(win_handle) != HI_ID_WIN));
        return HI_NULL;
    }

    if (g_win_manage_instance.win_array[disp_chn][i]) {
        return g_win_manage_instance.win_array[disp_chn][i];
    } else {
        WIN_ERROR("can't get valid win: 0x%x, chan:%d,index:%d\n", win_handle, disp_chn, i);
        return HI_NULL;
    }
}

hi_s32 winmanage_get_win_num(hi_drv_display disp_chn)
{
    return g_win_manage_instance.current_win_num[disp_chn];
}

hi_handle *winmanage_get_win_handle(hi_drv_display disp_chn)
{
    return g_win_manage_instance.win_handle_array[disp_chn];
}

hi_s32 winmanage_bind_windows(hi_void)
{
    return HI_SUCCESS;
}

hi_void winmanage_unbind_windows(win_descriptor *win_descp)
{
    return;
}

hi_void winmanage_get_lock(hi_ulong *spin_lock_flags)
{
    osal_spin_lock_irqsave(&g_win_manage_instance.cross_win_spinlock, spin_lock_flags);

    return;
}

hi_void winmanage_rls_lock(hi_ulong *spin_lock_flags)
{
    osal_spin_unlock_irqrestore(&g_win_manage_instance.cross_win_spinlock, spin_lock_flags);
    return;
}


