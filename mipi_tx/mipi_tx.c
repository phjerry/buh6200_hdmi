/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: mipi tx module
* Author: sdk
* Create: 2019-11-20
*/
#include "hi_drv_mipi.h"
#include "hi_type.h"
#include "hi_errno.h"
#include "drv_mipi_ext.h"
#include "drv_mipi_tx_define.h"
#include "mipi_panel_func.h"

hi_s32 hi_drv_mipi_config(hi_drv_disp_intf intf, hi_u32 mode, hi_drv_disp_intf_info *intf_info)
{
    hi_s32 ret1, ret2;
    hi_drv_mipi_id id1 = intf.un_intf.mipi.mipi_id;
    hi_drv_mipi_id id2;
    hi_drv_mipi_mode mipi_mode = intf.un_intf.mipi.mipi_mode;

    mipi_func_enter();

    id2 = id1;
    ret2 = HI_SUCCESS;

    ret1 = mipi_tx_set_dev_cfg(id1, mipi_mode, intf_info);

    if (mipi_mode == HI_DRV_MIPI_MODE_DOUBLE) {
        id2 = (id1 == HI_DRV_MIPI_ID_0) ? HI_DRV_MIPI_ID_1 : HI_DRV_MIPI_ID_0;
        ret2 = mipi_tx_set_dev_cfg(id2, mipi_mode, intf_info);
    }

    if ((ret1 != HI_SUCCESS) || (ret2 != HI_SUCCESS)) {
        hi_err_mipi("mipi(%d-%d) enable failed, r1 = %d, r2 = %d.\n", id1, id2, ret1, ret2);
        return HI_FAILURE;
    }

    ret1 = panel_peripherial_init((panel_id)id1);
    if (ret1 != HI_SUCCESS) {
        hi_err_mipi("panel init err id = %d, r1 = %d.\n", id1, ret1);
        return HI_FAILURE;
    }

    /* fix enable funciont */
    ret1 = hi_drv_mipi_enable(intf, HI_TRUE);
    if (ret1 != HI_SUCCESS) {
        hi_err_mipi("hi_drv_mipi_enable err  ret = %d.\n", ret1);
        return HI_FAILURE;
    }

    mipi_func_exit();

    return HI_SUCCESS;
}

hi_s32 hi_drv_mipi_atomic_config(hi_drv_disp_intf intf, hi_u32 mode, hi_drv_disp_intf_info *intf_info)
{
    return HI_SUCCESS;
}

hi_s32 hi_drv_mipi_prepare(hi_drv_disp_intf intf, hi_u32 mode, hi_drv_disp_intf_info *intf_info)
{
    return HI_SUCCESS;
}

hi_s32 hi_drv_mipi_detach(hi_drv_disp_intf intf, hi_void *data)
{
    return HI_SUCCESS;
}

hi_s32 hi_drv_mipi_attach(hi_drv_disp_intf intf, hi_void *data)
{
    return HI_SUCCESS;
}

hi_s32 hi_drv_mipi_enable(hi_drv_disp_intf intf, hi_bool enable)
{
    hi_s32 ret1, ret2;
    hi_drv_mipi_id id1 = intf.un_intf.mipi.mipi_id;
    hi_drv_mipi_id id2;
    hi_drv_mipi_mode mipi_mode = intf.un_intf.mipi.mipi_mode;

    mipi_func_enter();

    id2 = id1;
    ret2 = HI_SUCCESS;

    ret1 = (enable == HI_TRUE) ? mipi_tx_enable(id1, mipi_mode) : mipi_tx_disable(id1, mipi_mode);
    if (mipi_mode == HI_DRV_MIPI_MODE_DOUBLE) {
        id2 = (id1 == HI_DRV_MIPI_ID_0) ? HI_DRV_MIPI_ID_1 : HI_DRV_MIPI_ID_0;
        ret2 = (enable == HI_TRUE) ? mipi_tx_enable(id2, mipi_mode) : mipi_tx_disable(id2, mipi_mode);
    }

    if ((ret1 != HI_SUCCESS) || (ret2 != HI_SUCCESS)) {
        hi_err_mipi("mipi(%d-%d) enable failed, r1 = %d, r2 = %d.\n", id1, id2, ret1, ret2);
        return HI_FAILURE;
    }

    ret1 = exec_panel_init_cmd(id1);
    if (ret1 != HI_SUCCESS) {
        hi_err_mipi("mipi(%d) init cmd exec failed, r1 = %d.\n", id1, ret1);
        return HI_FAILURE;
    }

    mipi_func_exit();

    return HI_SUCCESS;
}

hi_s32 hi_drv_mipi_suspend(hi_drv_disp_intf intf, hi_void *pdev, hi_void *state)
{
    return HI_SUCCESS;
}

hi_s32 hi_drv_mipi_resume(hi_drv_disp_intf intf, hi_void *pdev, hi_void *state)
{
    return HI_SUCCESS;
}

hi_s32 hi_drv_mipi_check_validate(hi_drv_disp_intf intf, hi_u32 set_mode, hi_drv_disp_intf_info *intf_info)
{
    mipi_func_enter();

    if (intf_info == HI_NULL) {
        hi_err_mipi("intf_info is null.\n");
        return HI_ERR_MIPI_NULL_PTR;
    }

    if (intf.intf_type != HI_DRV_DISP_INTF_TYPE_MIPI) {
        hi_err_mipi("intf type is %d, not mipi.\n", intf.intf_type);
        return HI_ERR_MIPI_INVALID_PARA;
    }

    /* logic support rgb16/18/24 if we should use hi_drv_pixel_format */
    mipi_func_exit();
    return check_mipi_mode(intf.un_intf.mipi.mipi_id, intf.un_intf.mipi.mipi_mode);
}
