/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description:  used for multi-win operation,such as add/del,  look-up, disp/virt/ai win relation
                       -ship management.
* Author: image
* Create: 2019-04-12
 */

#ifndef __DRV_WIN_MNG_
#define __DRV_WIN_MNG_

#include "hi_type.h"
#include "hi_errno.h"

#include "hi_drv_video.h"
#include "hi_drv_disp.h"
#include "hi_drv_win.h"

#include "drv_xdp_osal.h"
#include "drv_window_priv.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* should consider that */
hi_s32 winmanage_init(hi_void);
hi_s32 winmanage_deinit(hi_void);
hi_s32 winmanage_allocate_handle(hi_drv_display disp_chn, hi_handle *win_handle);
hi_s32 winmanage_add_window(win_descriptor *win_descp,
                                       hi_drv_display disp_chn);
hi_s32 winmanage_del_window(hi_u32 win_handle);
win_descriptor *winmanage_get_window(hi_u32 win_handle);
hi_s32 winmanage_get_win_num(hi_drv_display disp_chn);
hi_handle *winmanage_get_win_handle(hi_drv_display disp_chn);
hi_s32 winmanage_bind_windows(hi_void);
hi_void winmanage_unbind_windows(win_descriptor *win_descp);

hi_void winmanage_get_lock(hi_ulong *spin_lock_flags);
hi_void winmanage_rls_lock(hi_ulong *spin_lock_flags);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif


