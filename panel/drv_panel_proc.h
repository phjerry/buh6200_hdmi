/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel proc module
* Author: sdk
* Create: 2019-11-23
*/

#ifndef __DRV_PANEL_PROC_H__
#define __DRV_PANEL_PROC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    /* for hi_drv_panel_isr_main */
    hi_u32 func_call_cnt;
    hi_u32 isr_start_line;
    hi_u32 isr_finish_line;
} drv_panel_debug_info;

hi_s32 drv_panel_proc_read(hi_void *s, hi_void *data);

hi_s32 drv_panel_proc_add(hi_void);

hi_void drv_panel_proc_del(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif



