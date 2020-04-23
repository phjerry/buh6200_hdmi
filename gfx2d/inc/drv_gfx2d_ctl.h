/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: gfx2d ccontrol header
 * Author: sdk
 * Create: 2018-01-01
 */

#ifndef _GFX2D_CTL_H_
#define _GFX2D_CTL_H_

/* ********************************add include here******************************************** */
#include "drv_gfx2d_config.h"
#include "drv_gfx2d_struct.h"

/**********************************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* ****************************** API declaration ********************************************* */
/*******************************************************************************
 * Function:      GFX2D_CTL_Init
 * Description:   ctl层初始化
 * Input:         in_para1     parameter description
 *                in_para2     parameter description
 * Output:        out_para1    parameter description
 *                out_para2    parameter description
 * Return:        HI_OK:                         success
 *                HI_ERR_SYSM_NOMEMORY:          Allocate memory fail.
 *                HI_ERR_SYSM_NODISKSPACE:       Disk space full.
 *                HI_ERR_SYSM_FILEWRITE:         File write error.
 * Others:        in_para1 should be none-zero
 *******************************************************************************/
hi_s32 GFX2D_CTL_Init(hi_void);

/*******************************************************************************
 * Function     : GFX2D_CTL_Deinit
 * Description  : ctl层去初始化
 * Input        : NA
 * Output       : NA
 * Return       : NA
 * Others       : NA
 *******************************************************************************/
hi_void GFX2D_CTL_Deinit(hi_void);

/*******************************************************************************
 * Function:      GFX2D_CTL_Open
 * Description:
 * Input:         in_para1     parameter description
 *                in_para2     parameter description
 * Output:        out_para1    parameter description
 *                out_para2    parameter description
 * Return:        HI_OK:                         success
 *                HI_ERR_SYSM_NOMEMORY:          Allocate memory fail.
 *                HI_ERR_SYSM_NODISKSPACE:       Disk space full.
 *                HI_ERR_SYSM_FILEWRITE:         File write error.
 * Others:        in_para1 should be none-zero
 *******************************************************************************/
hi_s32 GFX2D_CTL_Open(hi_void);

/*******************************************************************************
 * Function:      GFX2D_CTL_Close
 * Description:
 * Input:         in_para1     parameter description
 *                in_para2     parameter description
 * Output:        out_para1    parameter description
 *                out_para2    parameter description
 * Return:        HI_OK:                         success
 *                HI_ERR_SYSM_NOMEMORY:          Allocate memory fail.
 *                HI_ERR_SYSM_NODISKSPACE:       Disk space full.
 *                HI_ERR_SYSM_FILEWRITE:         File write error.
 * Others:        in_para1 should be none-zero
 *******************************************************************************/
hi_s32 GFX2D_CTL_Close(hi_void);

/*******************************************************************************
 * Function:      GFX2D_CTL_ComposeSync
 * Description:
 * Input:         in_para1     parameter description
 *                in_para2     parameter description
 * Output:        out_para1    parameter description
 *                out_para2    parameter description
 * Return:        HI_OK:                         success
 *                HI_ERR_SYSM_NOMEMORY:          Allocate memory fail.
 *                HI_ERR_SYSM_NODISKSPACE:       Disk space full.
 *                HI_ERR_SYSM_FILEWRITE:         File write error.
 * Others:        in_para1 should be none-zero
 *******************************************************************************/
hi_s32 GFX2D_CTL_Compose(const drv_gfx2d_dev_id dev_id, drv_gfx2d_compose_list *compose_list,
                         drv_gfx2d_compose_surface *pstDstSurface, hi_bool work_sync, const hi_u32 time_out);

/*******************************************************************************
 * Function:      gfx2d_wait_done
 * Description:
 * Input:         in_para1     parameter description
 *                in_para2     parameter description
 * Output:        out_para1    parameter description
 *                out_para2    parameter description
 * Return:        HI_OK:                         success
 *                HI_ERR_SYSM_NOMEMORY:          Allocate memory fail.
 *                HI_ERR_SYSM_NODISKSPACE:       Disk space full.
 *                HI_ERR_SYSM_FILEWRITE:         File write error.
 * Others:        in_para1 should be none-zero
 *******************************************************************************/
hi_s32 gfx2d_wait_done(const drv_gfx2d_dev_id dev_id, const hi_u32 time_out);

/*******************************************************************************
 * Function:      GFX2D_CTL_ReadProc
 * Description:
 * Input:         in_para1     parameter description
 *                in_para2     parameter description
 * Output:        out_para1    parameter description
 *                out_para2    parameter description
 * Return:        HI_OK:                         success
 *                HI_ERR_SYSM_NOMEMORY:          Allocate memory fail.
 *                HI_ERR_SYSM_NODISKSPACE:       Disk space full.
 *                HI_ERR_SYSM_FILEWRITE:         File write error.
 * Others:        in_para1 should be none-zero
 *******************************************************************************/
hi_s32 gfx2d_read_ctl_proc(hi_void *p, hi_void *v);

hi_s32 GFX2D_CTL_Resume(hi_void);

hi_bool gfx2d_check_open(hi_void);

hi_s32 gfx2d_ctl_init_mutex(hi_void);

hi_void gfx2d_ctl_deinit_mutex(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _GFX2D_CTL_H_ */
