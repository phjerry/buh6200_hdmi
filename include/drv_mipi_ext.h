/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: define for external use
* Author: mipi
* Create: 2019-07-31
 */
#ifndef __DRV_MIPI_EXT_H__
#define __DRV_MIPI_EXT_H__

#include "hi_type.h"
#include "hi_drv_mipi.h"
#include "drv_disp_ext.h"
#include "hi_drv_disp.h"

typedef struct {
    fn_intf_suspend intf_suspend;
    fn_intf_resume intf_resume;
    fn_intf_check_validate intf_check_validate;
    fn_intf_enable intf_enable;
    fn_intf_detach intf_detach;
    fn_intf_attach intf_attach;
    fn_intf_prepare intf_prepare;
    fn_intf_config intf_config;
    fn_intf_atomic_config intf_atomic_config;
} mipi_func_export;

hi_s32 exec_panel_init_cmd(hi_drv_mipi_id id);
hi_s32 check_mipi_mode(hi_drv_mipi_id id, hi_drv_mipi_mode mipi_mode);
hi_s32 mipi_tx_enable(hi_drv_mipi_id id, hi_drv_mipi_mode mipi_mode);
hi_s32 mipi_tx_disable(hi_drv_mipi_id id, hi_drv_mipi_mode mipi_mode);
hi_s32 mipi_tx_set_dev_cfg(hi_drv_mipi_id id, hi_drv_mipi_mode mipi_mode, const hi_drv_disp_intf_info *intf_info);

hi_s32 hi_drv_mipi_config(hi_drv_disp_intf intf, hi_u32 mode, hi_drv_disp_intf_info *intf_info);
hi_s32 hi_drv_mipi_prepare(hi_drv_disp_intf intf, hi_u32 mode,hi_drv_disp_intf_info *intf_info);
hi_s32 hi_drv_mipi_detach(hi_drv_disp_intf intf, hi_void *data);
hi_s32 hi_drv_mipi_attach(hi_drv_disp_intf intf, hi_void *data);
hi_s32 hi_drv_mipi_enable(hi_drv_disp_intf intf, hi_bool enable);
hi_s32 hi_drv_mipi_suspend(hi_drv_disp_intf intf, hi_void *pdev, hi_void *state);
hi_s32 hi_drv_mipi_resume(hi_drv_disp_intf intf, hi_void *pdev, hi_void *state);
hi_s32 hi_drv_mipi_atomic_config(hi_drv_disp_intf intf, hi_u32 mode, hi_drv_disp_intf_info *intf_info);
hi_s32 hi_drv_mipi_check_validate(hi_drv_disp_intf intf, hi_u32 set_mode, hi_drv_disp_intf_info *intf_info);

#ifndef MODULE

hi_s32 drv_mipi_mod_init(hi_void);
hi_void drv_mipi_mod_exit(hi_void);

#endif

#endif