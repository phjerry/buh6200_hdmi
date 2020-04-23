/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description:  osr manage
 * Author: sdk
 * Create: 2019-03-18
 */

#include <linux/hisilicon/securec.h>
#include "hi_osal.h"

#include "hi_drv_osal.h"
#include "hi_drv_tde.h"
#include "drv_tde_struct.h"
#include "drv_ioctl_tde.h"
#include "tde_osictl.h"
#include "tde_osilist.h"
#include "tde_hal.h"
#include "tde_handle.h"
#include "wmalloc.h"
#include "tde_adp.h"
#include "hi_gfx_comm_k.h"
#include "hi_reg_common.h"
#include "tde_debug.h"
#include "drv_tde_intf.h"
#ifdef HI_TEE_SUPPORT
#include "hi_drv_ssm.h"
#endif

/* **************************** Macro Definition ******************************************* */
#define TDE_NAME "HI_TDE"

/* ************************** Enum Definition ********************************************** */

/* ************************** Structure Definition ***************************************** */

/* ********************* Global Variable declaration *************************************** */
STATIC osal_spinlock s_taskletlock;
STATIC osal_spinlock s_TdeRefLock;

extern struct miscdevice gfx_dev;

#ifdef TDE_HWC_COOPERATE
STATIC hi_u32 g_TdeIrqHandle = 0;
#endif

typedef hi_s32 (*TdeCtlPtrFunc)(unsigned long *argp, struct file *ffile);

typedef struct {
    hi_u32 u32TdeCmd;
    TdeCtlPtrFunc tde_ctl_ptr_func;
} TDE_CTL_FUNC_DISPATCH_ITEM;

STATIC hi_s32 TdeOsrBeginJob(unsigned long *argp, struct file *ffile);
STATIC hi_s32 TdeOsrBitBlit(unsigned long *argp, struct file *ffile);
STATIC hi_s32 TdeOsrSolidDraw(unsigned long *argp, struct file *ffile);
STATIC hi_s32 TdeOsrQuickCopy(unsigned long *argp, struct file *ffile);
STATIC hi_s32 TdeOsrQuickResize(unsigned long *argp, struct file *ffile);
STATIC hi_s32 TdeOsrQuickFill(unsigned long *argp, struct file *ffile);
STATIC hi_s32 TdeOsrQuickDeflicker(unsigned long *argp, struct file *ffile);
STATIC hi_s32 TdeOsrMbBitBlit(unsigned long *argp, struct file *ffile);
STATIC hi_s32 TdeOsrEndJob(unsigned long *argp, struct file *ffile);
STATIC hi_s32 TdeOsrWaitForDone(unsigned long *argp, struct file *ffile);
STATIC hi_s32 TdeOsrCancelJob(unsigned long *argp, struct file *ffile);
STATIC hi_s32 TdeOsrBitMapMaskRop(unsigned long *argp, struct file *ffile);
STATIC hi_s32 TdeOsrBitMapMaskBlend(unsigned long *argp, struct file *ffile);
STATIC hi_s32 TdeOsrWaitAllDone(unsigned long *argp, struct file *ffile);
STATIC hi_s32 TdeOsrReset(unsigned long *argp, struct file *ffile);

STATIC hi_s32 TdeOsrSetDeflickerLevel(unsigned long *argp, struct file *ffile);
STATIC hi_s32 TdeOsrGetDeflickerLevel(unsigned long *argp, struct file *ffile);
STATIC hi_s32 TdeOsrSetAlphaThreshholdValue(unsigned long *argp, struct file *ffile);
STATIC hi_s32 TdeOsrGetAlphaThreshholdValue(unsigned long *argp, struct file *ffile);
STATIC hi_s32 TdeOsrSetAlphaThreshholdState(unsigned long *argp, struct file *ffile);
STATIC hi_s32 TdeOsrGetAlphaThreshholdState(unsigned long *argp, struct file *ffile);
STATIC hi_s32 TdeOsrPatternFill(unsigned long *argp, struct file *ffile);
STATIC hi_s32 TdeOsrEnableRegionDeflicker(unsigned long *argp, struct file *ffile);

TDE_CTL_FUNC_DISPATCH_ITEM g_paTdeCtlFuncDispatchItem[] = {
    { 0, NULL },
    { TDE_BEGIN_JOB, TdeOsrBeginJob },
    { TDE_BIT_BLIT, TdeOsrBitBlit },
    { TDE_SOLID_DRAW, TdeOsrSolidDraw },
    { TDE_QUICK_COPY, TdeOsrQuickCopy },
    { TDE_QUICK_RESIZE, TdeOsrQuickResize },
    { TDE_QUICK_FILL, TdeOsrQuickFill },
    { TDE_QUICK_DEFLICKER, TdeOsrQuickDeflicker },
    { TDE_MB_BITBLT, TdeOsrMbBitBlit },
    { TDE_END_JOB, TdeOsrEndJob },
    { TDE_WAITFORDONE, TdeOsrWaitForDone },
    { TDE_CANCEL_JOB, TdeOsrCancelJob },
    { TDE_BITMAP_MASKROP, TdeOsrBitMapMaskRop },
    { TDE_BITMAP_MASKBLEND, TdeOsrBitMapMaskBlend },
    { TDE_WAITALLDONE, TdeOsrWaitAllDone },
    { TDE_RESET, TdeOsrReset },
    { 0, NULL },
    { TDE_SET_DEFLICKERLEVEL, TdeOsrSetDeflickerLevel },
    { TDE_GET_DEFLICKERLEVEL, TdeOsrGetDeflickerLevel },
    { TDE_SET_ALPHATHRESHOLD_VALUE, TdeOsrSetAlphaThreshholdValue },
    { TDE_GET_ALPHATHRESHOLD_VALUE, TdeOsrGetAlphaThreshholdValue },
    { TDE_SET_ALPHATHRESHOLD_STATE, TdeOsrSetAlphaThreshholdState },
    { TDE_GET_ALPHATHRESHOLD_STATE, TdeOsrGetAlphaThreshholdState },
    { TDE_PATTERN_FILL, TdeOsrPatternFill },
    { TDE_ENABLE_REGIONDEFLICKER, TdeOsrEnableRegionDeflicker },
    { 0, NULL },
    { 0, NULL },
    { 0, NULL }
};

#define TDE_IOCTL_FUNC_NUM (sizeof(g_paTdeCtlFuncDispatchItem) / sizeof(g_paTdeCtlFuncDispatchItem[0]))

STATIC int tde_osr_isr(int irq, void *dev_id);
STATIC void tde_tasklet_func(unsigned long int_status);

STATIC osal_atomic g_TDECount = {HI_NULL};

#ifdef TDE_TIME_COUNT
osal_timeval g_stTimeStart;
osal_timeval g_stTimeEnd;
hi_u64 g_u64TimeDiff;
#endif

static osal_tasklet tde_tasklet;

#ifdef CONFIG_TDE_TDE_EXPORT_FUNC
STATIC hi_tde_export_func g_tde_export_func = { .drv_tde_module_init = hi_drv_tde_module_init,
                                                .drv_tde_module_exit = hi_drv_tde_module_exit,
                                                .drv_tde_module_open = drv_tde_open,
                                                .drv_tde_module_close = drv_tde_close,
                                                .drv_tde_module_begin_job = drv_tde_begin_job,
                                                .drv_tde_module_end_job = drv_tde_end_job,
                                                .drv_tde_module_cancel_job = drv_tde_cancel_job,
                                                .drv_tde_module_wait_for_done = drv_tde_wait_for_done,
                                                .drv_tde_module_wait_all_done = drv_tde_wait_all_done,
                                                .drv_tde_module_quick_copy = drv_tde_quick_copy,
                                                .drv_tde_module_quick_fill = drv_tde_quick_fill,
                                                .drv_tde_module_blit = drv_tde_blit,
                                                .drv_tde_module_set_deflicker_level = drv_tde_set_deflicker_level,
                                                .drv_tde_module_enable_region_deflicker =
                                                    drv_tde_enable_region_deflicker,
                                                .drv_tde_module_calc_scale_rect = drv_tde_calc_scale_rect,
                                                .drv_tde_module_quick_copyex = drv_tde_quick_copyex,
#ifdef CONFIG_TDE_BLIT_EX
                                                .drv_tde_module_single_blitex = drv_tde_single_blitex,
#endif
                                                .drv_tde_module_lock_working_flag = drv_tde_lock_working_flag,
                                                .drv_tde_module_unlock_working_flag = drv_tde_unlock_working_flag,
                                                .drv_tde_module_get_working_flag = drv_tde_get_working_flag,
                                                .drv_tde_module_set_working_flag = drv_tde_set_working_flag };
#endif

/* ****************************** API declaration ****************************************** */

/* ****************************** API release ********************************************** */
STATIC hi_void DRV_TDE_PrintSurfaceInfo(hi_tde_surface *pstSurface, hi_tde_mb_surface *pstMbSurface,
                                        hi_tde_rect *pstRect, hi_tde_opt *opt, hi_tde_mb_opt *mb_opt)
{
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");

    if (NULL != pstSurface) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, pstSurface->phy_addr);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->color_fmt);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->width);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->height);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->stride);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_VOID, pstSurface->clut_phy_addr);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->is_ycbcr_clut);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->alpha_max_is_255);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->support_alpha_ext_1555);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->alpha0);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->alpha1);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, pstSurface->cbcr_phy_addr);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->cbcr_stride);
    }

    if (NULL != pstRect) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstRect->pos_x);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstRect->pos_y);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstRect->width);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstRect->height);
    }

    if (NULL != pstMbSurface) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstMbSurface->mb_color_fmt);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, pstMbSurface->y_addr);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstMbSurface->y_width);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstMbSurface->y_height);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstMbSurface->y_stride);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, pstMbSurface->cbcr_phy_addr);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstMbSurface->cbcr_stride);
    }

    if (NULL != opt) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->resize);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->alpha_blending_cmd);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->rop_color);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->rop_alpha);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_mode);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.alpha.component_min);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.alpha.component_max);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.alpha.is_component_out);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.alpha.is_component_ignore);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.alpha.component_mask);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.alpha.component_reserved);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.alpha.component_reserved1);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.alpha.component_reserved2);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.red.component_min);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.red.component_max);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.red.is_component_out);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.red.is_component_ignore);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.red.component_mask);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.red.component_reserved);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.red.component_reserved1);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.red.component_reserved2);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.green.component_min);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.green.component_max);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.green.is_component_out);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.green.is_component_ignore);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.green.component_mask);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.green.component_reserved);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.green.component_reserved1);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.green.component_reserved2);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.blue.component_min);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.blue.component_max);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.blue.is_component_out);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.blue.is_component_ignore);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.blue.component_mask);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.blue.component_reserved);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.blue.component_reserved1);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.argb_color_key.blue.component_reserved2);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.alpha.component_min);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.alpha.component_max);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.alpha.is_component_out);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.alpha.is_component_ignore);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.alpha.component_mask);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.alpha.component_reserved);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.alpha.component_reserved1);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.alpha.component_reserved2);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.y.component_min);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.y.component_max);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.y.is_component_out);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.y.is_component_ignore);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.y.component_mask);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.y.component_reserved);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.y.component_reserved1);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.y.component_reserved2);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.cb.component_min);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.cb.component_max);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.cb.is_component_out);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.cb.is_component_ignore);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.cb.component_mask);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.cb.component_reserved);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.cb.component_reserved1);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.cb.component_reserved2);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.cr.component_min);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.cr.component_max);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.cr.is_component_out);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.cr.is_component_ignore);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.cr.component_mask);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.cr.component_reserved);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.cr.component_reserved1);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.ycbcr_color_key.cr.component_reserved2);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.clut_color_key.alpha.component_min);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.clut_color_key.alpha.component_max);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.clut_color_key.alpha.is_component_out);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.clut_color_key.alpha.is_component_ignore);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.clut_color_key.alpha.component_mask);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.clut_color_key.alpha.component_reserved);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.clut_color_key.alpha.component_reserved1);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.clut_color_key.alpha.component_reserved2);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.clut_color_key.clut.component_min);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.clut_color_key.clut.component_max);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.clut_color_key.clut.is_component_out);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.clut_color_key.clut.is_component_ignore);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.clut_color_key.clut.component_mask);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.clut_color_key.clut.component_reserved);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.clut_color_key.clut.component_reserved1);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_key_value.clut_color_key.clut.component_reserved2);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->clip_mode);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->clip_rect.pos_x);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->clip_rect.pos_y);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->clip_rect.width);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->clip_rect.height);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->deflicker_mode);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->resize);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->filter_mode);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->mirror);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->clut_reload);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->global_alpha);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->out_alpha_from);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->color_resize);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->blend_opt.global_alpha_en);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->blend_opt.pixel_alpha_en);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->blend_opt.src1_alpha_premulti);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->blend_opt.src2_alpha_premulti);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->blend_opt.blend_cmd);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->blend_opt.src1_blend_mode);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->blend_opt.src2_blend_mode);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->csc_opt.src_csc_user_en);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->csc_opt.src_csc_param_reload_en);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->csc_opt.dst_csc_user_en);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, opt->csc_opt.dst_csc_param_reload_en);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, opt->csc_opt.src_csc_param_addr);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, opt->csc_opt.dst_csc_param_addr);
    }

    if (NULL != mb_opt) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, mb_opt->clip_mode);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, mb_opt->clip_rect.pos_x);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, mb_opt->clip_rect.pos_y);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, mb_opt->clip_rect.height);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, mb_opt->clip_rect.width);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, mb_opt->is_deflicker);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, mb_opt->resize_en);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, mb_opt->is_set_out_alpha);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, mb_opt->out_alpha);
    }

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");
    return;
}

static hi_s32 drv_tde_init_ssm_iommu(hi_void)
{
#ifdef HI_TEE_SUPPORT
    return hi_drv_ssm_iommu_config(LOGIC_MOD_ID_HWC);
#else
    return HI_SUCCESS;
#endif
}

static hi_s32 drv_tde_osal_init(hi_void)
{
    hi_s32 ret;

    ret = osal_spin_lock_init(&s_taskletlock);
    if (ret != HI_SUCCESS) {
        s_taskletlock.lock = HI_NULL;
        return ret;
    }

    ret = osal_spin_lock_init(&s_TdeRefLock);
    if (ret != HI_SUCCESS) {
        s_TdeRefLock.lock = HI_NULL;
        return ret;
    }

    ret = osal_atomic_init(&g_TDECount);
    if (ret != 0) {
        g_TDECount.atomic = HI_NULL;
        return ret;
    }

    return HI_SUCCESS;
}

static hi_void drv_tde_osal_deinit(hi_void)
{
    if (s_taskletlock.lock != HI_NULL) {
        osal_spin_lock_destory(&s_taskletlock);
    }

    if (s_TdeRefLock.lock != HI_NULL) {
        osal_spin_lock_destory(&s_TdeRefLock);
    }

    if (g_TDECount.atomic != HI_NULL) {
        osal_atomic_destory(&g_TDECount);
    }
}
/***************************************************************************************
* func          : drv_tde_module_init_k
* description   : init tde mod
                  CNcomment: 加载驱动初始化 CNend\n
* param[in]     :
* retval        : HI_SUCCESS 成功
* retval        : HI_FAILURE 失败
* others:       : NA
***************************************************************************************/
hi_s32 drv_tde_module_init_k(hi_void)
{
    hi_s32 ret = HI_FAILURE;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);

    if (tde_hal_init(TDE_REG_BASEADDR) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_init, FAILURE_TAG);
        return -1;
    }

    if (drv_tde_init_ssm_iommu() != HI_SUCCESS) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_init, FAILURE_TAG);
        return -1;
    }

#ifndef TDE_HWC_COOPERATE
    ret = hi_drv_osal_request_irq(TDE_INTNUM, (irq_handler_t)tde_osr_isr, IRQF_PROBE_SHARED, "tde", HI_NULL);
#else
    ret = hi_drv_osal_request_irq(TDE_INTNUM, (irq_handler_t)tde_osr_isr, IRQF_SHARED, "tde", &g_TdeIrqHandle);
#endif

    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_irq_request, FAILURE_TAG);
        tde_hal_release();
        return -1;
    }

    ret = HI_GFX_SetIrq(HIGFX_TDE_ID, TDE_INTNUM, "tde");
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HI_GFX_SetIrq, ret);
        drv_tde_module_exit_k();
        return ret;
    }

    ret = hi_gfx_module_register(HIGFX_TDE_ID, TDE_NAME, &g_tde_export_func);
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, hi_gfx_module_register, ret);
        drv_tde_module_exit_k();
        return ret;
    }

    ret = tde_list_init();
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_list_init, ret);
        drv_tde_module_exit_k();
        return ret;
    }

    ret = drv_tde_osal_init();
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_osal_init, ret);
        drv_tde_module_exit_k();
        return ret;
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return 0;
}

/***************************************************************************************
* func          : drv_tde_module_exit_k
* description   : dinit tde mod
                  CNcomment: 去初始化 CNend\n
* param[in]     :
* retval        : HI_SUCCESS 成功
* retval        : HI_FAILURE 失败
* others:       : NA
***************************************************************************************/
hi_void drv_tde_module_exit_k(hi_void)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    hi_gfx_module_unregister(HIGFX_TDE_ID);

    tde_list_term();

#ifndef TDE_HWC_COOPERATE
    hi_drv_osal_free_irq(TDE_INTNUM, "tde", NULL);
#else
    hi_drv_osal_free_irq(TDE_INTNUM, "tde", &g_TdeIrqHandle);
#endif
    tde_hal_release();
    drv_tde_tasklet_dinit();
    drv_tde_osal_deinit();
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return;
}

hi_s32 drv_tde_dev_open(struct inode *finode, struct file *ffile)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);

    osal_atomic_inc_return(&g_TDECount);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

STATIC hi_void tde_conv_surface_fd_to_addr(hi_tde_surface *surface)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    if (surface != HI_NULL) {
        surface->clut_phy_addr = drv_gfx_mem_get_smmu_from_fd(surface->clut_mem_handle, HIGFX_TDE_ID);
        surface->cbcr_phy_addr = drv_gfx_mem_get_smmu_from_fd(surface->cbcr_mem_handle, HIGFX_TDE_ID);
        surface->phy_addr = drv_gfx_mem_get_smmu_from_fd(surface->y_mem_handle, HIGFX_TDE_ID);
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return;
}

STATIC hi_void tde_conv_mb_surface_fd_to_addr(hi_tde_mb_surface *mbsurface)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    if (mbsurface != HI_NULL) {
        mbsurface->y_addr = drv_gfx_mem_get_smmu_from_fd(mbsurface->y_mem_handle, HIGFX_TDE_ID);
        mbsurface->cbcr_phy_addr = drv_gfx_mem_get_smmu_from_fd(mbsurface->cbcr_mem_handle, HIGFX_TDE_ID);
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return;
}

STATIC hi_void tde_conv_csc_opt_fd_to_addr(hi_tde_csc_opt *opt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    if (opt != HI_NULL) {
        opt->src_csc_param_addr = drv_gfx_mem_get_smmu_from_fd(opt->src_csc_param_mem_handle, HIGFX_TDE_ID);
        opt->dst_csc_param_addr = drv_gfx_mem_get_smmu_from_fd(opt->dst_csc_param_mem_handle, HIGFX_TDE_ID);
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return;
}

hi_s32 drv_tde_dev_release(struct inode *finode, struct file *ffile)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);

    if (g_TDECount.atomic == HI_NULL) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
        return 0;
    }
    if (osal_atomic_dec_return(&g_TDECount) == 0) {
        if (NULL != ffile) {
            tde_list_free_pending_job((hi_void *)ffile);
        }
    }

    if (osal_atomic_read(&g_TDECount) < 0) {
        osal_atomic_set(&g_TDECount, 0);
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return 0;
}

typedef union {
    drv_tde_bitblit_cmd stBlitCmd;
    drv_tde_solid_draw_cmd stDrawCmd;
    drv_tde_quick_deflicker_cmd stDeflickerCmd;
    drv_tde_quick_copy_cmd stCopyCmd;
    drv_tde_quick_resize_cmd stResizeCmd;
    drv_tde_quick_fill_cmd stFillCmd;
    drv_tde_end_job_cmd stEndCmd;
    drv_tde_mb_blit_cmd stMbBlitCmd;
    drv_tde_bitmap_maskrop_cmd stMaskRopCmd;
    drv_tde_bitmap_maskblend_cmd stMaskBlendCmd;
    drv_tde_pattern_fill_cmd stPatternCmd;
} TDE_IOCTL_CMD_U;

long drv_tde_dev_ioctl(struct file *ffile, unsigned int cmd, unsigned long arg)
{
    hi_u8 u8Cmd = 0;
    void __user *argp = (void __user *)(uintptr_t)arg;

    u8Cmd = _IOC_NR(cmd);
    if (u8Cmd < 1 || u8Cmd >= TDE_IOCTL_FUNC_NUM) {
        return -EFAULT;
    }
    GRAPHIC_CHECK_LEFT_UNEQUAL_RIGHT_RETURN_VALUE(cmd, g_paTdeCtlFuncDispatchItem[u8Cmd].u32TdeCmd, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(g_paTdeCtlFuncDispatchItem[u8Cmd].tde_ctl_ptr_func, HI_FAILURE);

    return g_paTdeCtlFuncDispatchItem[u8Cmd].tde_ctl_ptr_func(argp, ffile);
}

STATIC hi_s32 TdeOsrBeginJob(unsigned long *argp, struct file *ffile)
{
    hi_s32 Handle = -1;
    hi_s32 Ret = HI_FAILURE;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    if (argp == HI_NULL) {
        return -EFAULT;
    }

    Ret = tde_begin_job(&Handle, (hi_void *)ffile);
    if (Ret < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_begin_job, Ret);
        return Ret;
    }

    if (osal_copy_to_user(argp, &Handle, sizeof(hi_s32))) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

STATIC hi_s32 TdeOsrBitBlit(unsigned long *argp, struct file *ffile)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_tde_surface *pstBackGround = NULL;
    hi_tde_rect *back_ground_rect = NULL;
    hi_tde_surface *fore_ground_surface = NULL;
    hi_tde_rect *fore_ground_rect = NULL;
    hi_tde_opt *opt = HI_NULL;

    TDE_IOCTL_CMD_U unCmd;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    memset_s(&unCmd, sizeof(TDE_IOCTL_CMD_U), 0, sizeof(TDE_IOCTL_CMD_U));

    if (argp == HI_NULL) {
        return -EFAULT;
    }

    if (osal_copy_from_user(&(unCmd.stBlitCmd), argp, sizeof(drv_tde_bitblit_cmd))) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    tde_conv_surface_fd_to_addr(&unCmd.stBlitCmd.back_ground_surface);
    tde_conv_surface_fd_to_addr(&unCmd.stBlitCmd.fore_ground_surface);
    tde_conv_surface_fd_to_addr(&unCmd.stBlitCmd.dst_surface);
    tde_conv_csc_opt_fd_to_addr(&unCmd.stBlitCmd.option.csc_opt);

    pstBackGround = ((unCmd.stBlitCmd.null_indicator >> 1) & 1) ? NULL : &unCmd.stBlitCmd.back_ground_surface;
    back_ground_rect = ((unCmd.stBlitCmd.null_indicator >> 2) & 1)
                            ? NULL
                            : &unCmd.stBlitCmd.back_ground_rect; /* 2 bit of null_indicator */
    fore_ground_surface = ((unCmd.stBlitCmd.null_indicator >> 3) & 1)
                                ? NULL
                                : &unCmd.stBlitCmd.fore_ground_surface; /* 3 bit of null_indicator */
    fore_ground_rect = ((unCmd.stBlitCmd.null_indicator >> 4) & 1)
                            ? NULL
                            : &unCmd.stBlitCmd.fore_ground_rect;                         /* 4 bit of null_indicator */
    opt = ((unCmd.stBlitCmd.null_indicator >> 7) & 1) ? NULL : &unCmd.stBlitCmd.option; /* 7 bit of null_indicator */

    DRV_TDE_PrintSurfaceInfo(pstBackGround, NULL, back_ground_rect, NULL, NULL);
    DRV_TDE_PrintSurfaceInfo(fore_ground_surface, NULL, fore_ground_rect, NULL, NULL);
    DRV_TDE_PrintSurfaceInfo(&unCmd.stBlitCmd.dst_surface, NULL, &unCmd.stBlitCmd.dst_rect, opt, NULL);

    Ret = drv_tde_blit(unCmd.stBlitCmd.handle, pstBackGround, back_ground_rect, fore_ground_surface, fore_ground_rect,
                       &unCmd.stBlitCmd.dst_surface, &unCmd.stBlitCmd.dst_rect, opt);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

STATIC hi_s32 TdeOsrSolidDraw(unsigned long *argp, struct file *ffile)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_tde_surface *fore_ground_surface = NULL;
    hi_tde_rect *fore_ground_rect = NULL;
    hi_tde_fill_color *fill_color = NULL;
    hi_tde_opt *opt = NULL;

    TDE_IOCTL_CMD_U unCmd;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    memset_s(&unCmd, sizeof(TDE_IOCTL_CMD_U), 0, sizeof(TDE_IOCTL_CMD_U));

    if (argp == HI_NULL) {
        return -EFAULT;
    }

    if (osal_copy_from_user(&(unCmd.stDrawCmd), argp, sizeof(drv_tde_solid_draw_cmd))) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    tde_conv_surface_fd_to_addr(&unCmd.stDrawCmd.fore_ground_surface);
    tde_conv_surface_fd_to_addr(&unCmd.stDrawCmd.dst_surface);
    tde_conv_csc_opt_fd_to_addr(&unCmd.stDrawCmd.option.csc_opt);

    fore_ground_surface = ((unCmd.stDrawCmd.null_indicator >> 1) & 1) ? NULL : &unCmd.stDrawCmd.fore_ground_surface;
    fore_ground_rect = ((unCmd.stDrawCmd.null_indicator >> 2) & 1)
                            ? NULL
                            : &unCmd.stDrawCmd.fore_ground_rect; /* 2 bit of null_indicator */
    fill_color = ((unCmd.stDrawCmd.null_indicator >> 5) & 1) ? NULL : &unCmd.stDrawCmd.fill_color; /* 5 bit of
                                                                                                      null_indicator */
    opt = ((unCmd.stDrawCmd.null_indicator >> 6) & 1) ? NULL : &unCmd.stDrawCmd.option; /* 6 bit of null_indicator */

    DRV_TDE_PrintSurfaceInfo(fore_ground_surface, NULL, fore_ground_rect, NULL, NULL);
    DRV_TDE_PrintSurfaceInfo(&unCmd.stDrawCmd.dst_surface, NULL, &unCmd.stBlitCmd.dst_rect, opt, NULL);

    Ret = tde_hal_solid_draw(unCmd.stDrawCmd.handle, fore_ground_surface, fore_ground_rect,
                             &unCmd.stDrawCmd.dst_surface, &unCmd.stDrawCmd.dst_rect, fill_color, opt);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

STATIC hi_s32 TdeOsrQuickCopy(unsigned long *argp, struct file *ffile)
{
    hi_s32 Ret = HI_SUCCESS;

    TDE_IOCTL_CMD_U unCmd;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    memset_s(&unCmd, sizeof(TDE_IOCTL_CMD_U), 0, sizeof(TDE_IOCTL_CMD_U));

    if (argp == HI_NULL) {
        return -EFAULT;
    }

    if (osal_copy_from_user(&(unCmd.stCopyCmd), argp, sizeof(drv_tde_quick_copy_cmd))) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    tde_conv_surface_fd_to_addr(&unCmd.stCopyCmd.src_surface);
    tde_conv_surface_fd_to_addr(&unCmd.stCopyCmd.dst_surface);

    DRV_TDE_PrintSurfaceInfo(&unCmd.stCopyCmd.src_surface, NULL, &unCmd.stCopyCmd.src_rect, NULL, NULL);
    DRV_TDE_PrintSurfaceInfo(&unCmd.stCopyCmd.dst_surface, NULL, &unCmd.stBlitCmd.dst_rect, NULL, NULL);

    Ret = drv_tde_quick_copy(unCmd.stCopyCmd.handle, &unCmd.stCopyCmd.src_surface, &unCmd.stCopyCmd.src_rect,
                             &unCmd.stCopyCmd.dst_surface, &unCmd.stCopyCmd.dst_rect);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

STATIC hi_s32 TdeOsrQuickResize(unsigned long *argp, struct file *ffile)
{
    hi_s32 Ret = HI_SUCCESS;

    TDE_IOCTL_CMD_U unCmd;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    memset_s(&unCmd, sizeof(TDE_IOCTL_CMD_U), 0, sizeof(TDE_IOCTL_CMD_U));

    if (argp == HI_NULL) {
        return -EFAULT;
    }

    if (osal_copy_from_user(&(unCmd.stResizeCmd), argp, sizeof(drv_tde_quick_resize_cmd))) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    tde_conv_surface_fd_to_addr(&unCmd.stResizeCmd.src_surface);
    tde_conv_surface_fd_to_addr(&unCmd.stResizeCmd.dst_surface);

    Ret = tde_hal_quick_resize(unCmd.stResizeCmd.handle, &unCmd.stResizeCmd.src_surface, &unCmd.stResizeCmd.src_rect,
                               &unCmd.stResizeCmd.dst_surface, &unCmd.stResizeCmd.dst_rect);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}
STATIC hi_s32 TdeOsrQuickFill(unsigned long *argp, struct file *ffile)
{
    hi_s32 Ret = HI_SUCCESS;

    TDE_IOCTL_CMD_U unCmd;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    memset_s(&unCmd, sizeof(TDE_IOCTL_CMD_U), 0, sizeof(TDE_IOCTL_CMD_U));
    // HI_UNUSED(ffile);

    if (argp == HI_NULL) {
        return -EFAULT;
    }

    if (osal_copy_from_user(&(unCmd.stFillCmd), argp, sizeof(drv_tde_quick_fill_cmd))) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    tde_conv_surface_fd_to_addr(&unCmd.stFillCmd.dst_surface);

    Ret = drv_tde_quick_fill(unCmd.stFillCmd.handle, &unCmd.stFillCmd.dst_surface, &unCmd.stFillCmd.dst_rect,
                             unCmd.stFillCmd.fill_data);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

STATIC hi_s32 TdeOsrQuickDeflicker(unsigned long *argp, struct file *ffile)
{
    hi_s32 Ret = HI_SUCCESS;

    TDE_IOCTL_CMD_U unCmd;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    memset_s(&unCmd, sizeof(TDE_IOCTL_CMD_U), 0, sizeof(TDE_IOCTL_CMD_U));

    if (argp == HI_NULL) {
        return -EFAULT;
    }

    if (osal_copy_from_user(&(unCmd.stDeflickerCmd), argp, sizeof(drv_tde_quick_deflicker_cmd))) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    tde_conv_surface_fd_to_addr(&unCmd.stDeflickerCmd.src_surface);
    tde_conv_surface_fd_to_addr(&unCmd.stDeflickerCmd.dst_surface);

    Ret = tde_hal_quick_flicker(unCmd.stDeflickerCmd.handle, &unCmd.stDeflickerCmd.src_surface,
                                &unCmd.stDeflickerCmd.src_rect, &unCmd.stDeflickerCmd.dst_surface,
                                &unCmd.stDeflickerCmd.dst_rect);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

STATIC hi_s32 TdeOsrMbBitBlit(unsigned long *argp, struct file *ffile)
{
    hi_s32 Ret = HI_SUCCESS;

    TDE_IOCTL_CMD_U unCmd;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    memset_s(&unCmd, sizeof(TDE_IOCTL_CMD_U), 0, sizeof(TDE_IOCTL_CMD_U));

    if (argp == HI_NULL) {
        return -EFAULT;
    }

    if (osal_copy_from_user(&(unCmd.stMbBlitCmd), argp, sizeof(drv_tde_mb_blit_cmd))) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    tde_conv_mb_surface_fd_to_addr(&unCmd.stMbBlitCmd.mb_src_surface);
    tde_conv_surface_fd_to_addr(&unCmd.stMbBlitCmd.dst_surface);

    DRV_TDE_PrintSurfaceInfo(NULL, &unCmd.stMbBlitCmd.mb_src_surface, &unCmd.stMbBlitCmd.mb_src_rect, NULL,
                             &unCmd.stMbBlitCmd.mb_option);
    DRV_TDE_PrintSurfaceInfo(&unCmd.stMbBlitCmd.dst_surface, NULL, &unCmd.stMbBlitCmd.dst_rect, NULL, NULL);

    Ret = tde_mb_blit(unCmd.stMbBlitCmd.handle, &unCmd.stMbBlitCmd.mb_src_surface, &unCmd.stMbBlitCmd.mb_src_rect,
                      &unCmd.stMbBlitCmd.dst_surface, &unCmd.stMbBlitCmd.dst_rect, &unCmd.stMbBlitCmd.mb_option);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

STATIC hi_s32 TdeOsrEndJob(unsigned long *argp, struct file *ffile)
{
    hi_s32 Ret = HI_SUCCESS;
    TDE_IOCTL_CMD_U unCmd;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    memset_s(&unCmd, sizeof(TDE_IOCTL_CMD_U), 0, sizeof(TDE_IOCTL_CMD_U));
    // HI_UNUSED(ffile);

    if (argp == HI_NULL) {
        return -EFAULT;
    }

    if (osal_copy_from_user(&(unCmd.stEndCmd), argp, sizeof(drv_tde_end_job_cmd))) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    Ret = drv_tde_end_job(unCmd.stEndCmd.handle, unCmd.stEndCmd.is_block, unCmd.stEndCmd.time_out,
                          unCmd.stEndCmd.is_sync, NULL, NULL);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

STATIC hi_s32 TdeOsrWaitForDone(unsigned long *argp, struct file *ffile)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_s32 handle = -1;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    // HI_UNUSED(ffile);

    if (argp == HI_NULL) {
        return -EFAULT;
    }

    if (osal_copy_from_user(&handle, argp, sizeof(hi_s32))) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    Ret = drv_tde_wait_for_done(handle, TDE_MAX_WAIT_TIMEOUT);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

STATIC hi_s32 TdeOsrCancelJob(unsigned long *argp, struct file *ffile)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_s32 handle = -1;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    // HI_UNUSED(ffile);

    if (argp == HI_NULL) {
        return -EFAULT;
    }

    if (osal_copy_from_user(&handle, argp, sizeof(hi_s32))) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    Ret = drv_tde_cancel_job(handle);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

STATIC hi_s32 TdeOsrBitMapMaskRop(unsigned long *argp, struct file *ffile)
{
#ifdef CONFIG_TDE_MASKROP_SUPPORT
    hi_s32 Ret = HI_SUCCESS;

    TDE_IOCTL_CMD_U unCmd;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    memset_s(&unCmd, sizeof(TDE_IOCTL_CMD_U), 0, sizeof(TDE_IOCTL_CMD_U));

    if (argp == HI_NULL) {
        return -EFAULT;
    }

    if (osal_copy_from_user(&(unCmd.stMaskRopCmd), argp, sizeof(drv_tde_bitmap_maskrop_cmd))) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    tde_conv_surface_fd_to_addr(&unCmd.stMaskRopCmd.back_ground_surface);
    tde_conv_surface_fd_to_addr(&unCmd.stMaskRopCmd.fore_ground_surface);
    tde_conv_surface_fd_to_addr(&unCmd.stMaskRopCmd.mask_surface);
    tde_conv_surface_fd_to_addr(&unCmd.stMaskRopCmd.dst_surface);

    Ret = tde_hal_bitmap_mask_rop(unCmd.stMaskRopCmd.handle, &unCmd.stMaskRopCmd.back_ground_surface,
                                  &unCmd.stMaskRopCmd.back_ground_rect, &unCmd.stMaskRopCmd.fore_ground_surface,
                                  &unCmd.stMaskRopCmd.fore_ground_rect, &unCmd.stMaskRopCmd.mask_surface,
                                  &unCmd.stMaskRopCmd.mask_rect, &unCmd.stMaskRopCmd.dst_surface,
                                  &unCmd.stMaskRopCmd.dst_rect, unCmd.stMaskRopCmd.rop_color,
                                  unCmd.stMaskRopCmd.rop_alpha);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
#else
    GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "mask rop not support");
    return HI_FAILURE;
#endif
}

STATIC hi_s32 TdeOsrBitMapMaskBlend(unsigned long *argp, struct file *ffile)
{
#ifdef CONFIG_TDE_MASKBLEND_SUPPORT
    hi_s32 Ret = HI_SUCCESS;

    TDE_IOCTL_CMD_U unCmd;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    memset_s(&unCmd, sizeof(TDE_IOCTL_CMD_U), 0, sizeof(TDE_IOCTL_CMD_U));

    if (argp == HI_NULL) {
        return -EFAULT;
    }

    if (osal_copy_from_user(&(unCmd.stMaskBlendCmd), argp, sizeof(drv_tde_bitmap_maskblend_cmd))) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    tde_conv_surface_fd_to_addr(&unCmd.stMaskBlendCmd.back_ground_surface);
    tde_conv_surface_fd_to_addr(&unCmd.stMaskBlendCmd.fore_ground_surface);
    tde_conv_surface_fd_to_addr(&unCmd.stMaskBlendCmd.mask_surface);
    tde_conv_surface_fd_to_addr(&unCmd.stMaskBlendCmd.dst_surface);

    Ret = tde_hal_bitmap_mask_blend(unCmd.stMaskBlendCmd.handle, &unCmd.stMaskBlendCmd.back_ground_surface,
                                    &unCmd.stMaskBlendCmd.back_ground_rect, &unCmd.stMaskBlendCmd.fore_ground_surface,
                                    &unCmd.stMaskBlendCmd.fore_ground_rect, &unCmd.stMaskBlendCmd.mask_surface,
                                    &unCmd.stMaskBlendCmd.mask_rect, &unCmd.stMaskBlendCmd.dst_surface,
                                    &unCmd.stMaskBlendCmd.dst_rect, unCmd.stMaskBlendCmd.alpha,
                                    unCmd.stMaskBlendCmd.blend_mode);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
#else
    GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "mask blend not support");
    return HI_FAILURE;
#endif
}

STATIC hi_s32 TdeOsrWaitAllDone(unsigned long *argp, struct file *ffile)
{
    hi_s32 Ret = HI_SUCCESS;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    // HI_UNUSED(argp);
    // HI_UNUSED(ffile);

    Ret = drv_tde_wait_all_done(HI_FALSE);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

STATIC hi_s32 TdeOsrReset(unsigned long *argp, struct file *ffile)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    // HI_UNUSED(argp);
    // HI_UNUSED(ffile);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

STATIC hi_s32 TdeOsrSetDeflickerLevel(unsigned long *argp, struct file *ffile)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_tde_deflicker_level deflicker_level;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    // HI_UNUSED(ffile);

    if (argp == HI_NULL) {
        return -EFAULT;
    }

    if (osal_copy_from_user(&deflicker_level, argp, sizeof(hi_tde_deflicker_level))) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    Ret = drv_tde_set_deflicker_level(deflicker_level);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

STATIC hi_s32 TdeOsrGetDeflickerLevel(unsigned long *argp, struct file *ffile)
{
    hi_tde_deflicker_level deflicker_level = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    // HI_UNUSED(ffile);

    if (argp == HI_NULL) {
        return -EFAULT;
    }

    if (TdeOsiGetDeflickerLevel(&deflicker_level) != HI_SUCCESS) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, deflicker_level, FAILURE_TAG);
        return HI_FAILURE;
    }

    if (osal_copy_to_user(argp, &deflicker_level, sizeof(hi_tde_deflicker_level))) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

STATIC hi_s32 TdeOsrSetAlphaThreshholdValue(unsigned long *argp, struct file *ffile)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_u8 threshold_value = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    // HI_UNUSED(ffile);

    if (argp == HI_NULL) {
        return -EFAULT;
    }

    if (osal_copy_from_user(&threshold_value, argp, sizeof(hi_u8))) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    Ret = tde_set_alpha_threshold_value(threshold_value);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

STATIC hi_s32 TdeOsrGetAlphaThreshholdValue(unsigned long *argp, struct file *ffile)
{
    hi_u8 threshold_value = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    // HI_UNUSED(ffile);
    if (argp == HI_NULL) {
        return -EFAULT;
    }

    if (tde_get_alpha_threshold_value(&threshold_value)) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_get_alpha_threshold_value, FAILURE_TAG);
        return HI_FAILURE;
    }

    if (osal_copy_to_user(argp, &threshold_value, sizeof(hi_u8))) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

STATIC hi_s32 TdeOsrSetAlphaThreshholdState(unsigned long *argp, struct file *ffile)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_bool alpha_threshold_en = HI_FALSE;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    // HI_UNUSED(ffile);
    if (argp == HI_NULL) {
        return -EFAULT;
    }

    if (osal_copy_from_user(&alpha_threshold_en, argp, sizeof(hi_bool))) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    Ret = tde_set_alpha_threshold_state(alpha_threshold_en);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

STATIC hi_s32 TdeOsrGetAlphaThreshholdState(unsigned long *argp, struct file *ffile)
{
    hi_bool alpha_threshold_en = HI_FALSE;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    // HI_UNUSED(ffile);
    if (argp == HI_NULL) {
        return -EFAULT;
    }

    tde_get_alpha_threshold_state(&alpha_threshold_en);

    if (osal_copy_to_user(argp, &alpha_threshold_en, sizeof(hi_bool))) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

STATIC hi_s32 TdeOsrPatternFill(unsigned long *argp, struct file *ffile)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_tde_rect *back_ground_rect = NULL;
    hi_tde_rect *fore_ground_rect = NULL;
    hi_tde_rect *dst_rect = NULL;
    hi_tde_pattern_fill_opt *opt = NULL;
    hi_tde_surface *pstBackGround = NULL;
    hi_tde_surface *fore_ground_surface = NULL;
    hi_tde_surface *dst_surface = NULL;

    TDE_IOCTL_CMD_U unCmd;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    memset_s(&unCmd, sizeof(TDE_IOCTL_CMD_U), 0, sizeof(TDE_IOCTL_CMD_U));

    if (argp == HI_NULL) {
        return -EFAULT;
    }

    if (osal_copy_from_user(&(unCmd.stPatternCmd), argp, sizeof(drv_tde_pattern_fill_cmd))) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    tde_conv_surface_fd_to_addr(&unCmd.stPatternCmd.back_ground_surface);
    tde_conv_surface_fd_to_addr(&unCmd.stPatternCmd.fore_ground_surface);
    tde_conv_surface_fd_to_addr(&unCmd.stPatternCmd.dst_surface);
    tde_conv_csc_opt_fd_to_addr(&unCmd.stPatternCmd.option.csc_opt);

    pstBackGround = ((unCmd.stPatternCmd.null_indicator >> 1) & 1) ? NULL : &unCmd.stPatternCmd.back_ground_surface;
    back_ground_rect = ((unCmd.stPatternCmd.null_indicator >> 2) & 1)
                            ? NULL
                            : &unCmd.stPatternCmd.back_ground_rect; /* 2 bit of null_indicator */
    fore_ground_surface = ((unCmd.stPatternCmd.null_indicator >> 3) & 1)
                                ? NULL
                                : &unCmd.stPatternCmd.fore_ground_surface; /* 3 bit of null_indicator */
    fore_ground_rect = ((unCmd.stPatternCmd.null_indicator >> 4) & 1)
                            ? NULL
                            : &unCmd.stPatternCmd.fore_ground_rect; /* 4 bit of null_indicator */
    dst_surface = ((unCmd.stPatternCmd.null_indicator >> 5) & 1) ? NULL : &unCmd.stPatternCmd.dst_surface; /* 5 bit of
                                                                                                              null_indicator
                                                                                                            */
    dst_rect = ((unCmd.stPatternCmd.null_indicator >> 6) & 1) ? NULL : &unCmd.stPatternCmd.dst_rect;       /* 6 bit of
                                                                                                        null_indicator
                                                                                                      */
    opt = ((unCmd.stPatternCmd.null_indicator >> 7) & 1) ? NULL : &unCmd.stPatternCmd.option; /* 7 bit of null_indicator */

    Ret = tde_hal_pattern_fill(unCmd.stPatternCmd.handle, pstBackGround, back_ground_rect, fore_ground_surface,
                               fore_ground_rect, dst_surface, dst_rect, opt);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

STATIC hi_s32 TdeOsrEnableRegionDeflicker(unsigned long *argp, struct file *ffile)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_bool region_deflicker = HI_FALSE;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);

    if (argp == HI_NULL) {
        return -EFAULT;
    }

    if (osal_copy_from_user(&region_deflicker, argp, sizeof(hi_bool))) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    Ret = drv_tde_enable_region_deflicker(region_deflicker);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return Ret;
}

hi_s32 drv_tde_tasklet_init(hi_void)
{
    hi_s32 ret;

    tde_tasklet.handler = tde_tasklet_func;
    ret = osal_tasklet_init(&tde_tasklet);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
    osal_tasklet_update(&tde_tasklet);
    return HI_SUCCESS;
}

hi_void drv_tde_tasklet_dinit(hi_void)
{
    if (tde_tasklet.tasklet != HI_NULL) {
        osal_tasklet_kill(&tde_tasklet);
    }
}

#ifdef TDE_COREDUMP_DEBUG
extern volatile hi_u32 *g_base_vir_addr = NULL;

#define TDE_READ_REG(base, offset) (*(volatile unsigned int *)((unsigned int)(base) + (offset)))
#endif

STATIC int tde_osr_isr(int irq, void *dev_id)
{
    hi_u32 int_status;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
#ifdef TDE_TIME_COUNT
    (hi_void) osal_get_timeofday(&g_stTimeStart);
#endif
    int_status = tde_hal_ctl_int_status();
    if (int_status & TDE_DRV_INT_ERROR) {
        hi_u32 i;
#ifdef TDE_COREDUMP_DEBUG
        hi_u32 u32ReadStats = 0;
        for (i = 0; i < 74; i++) {                                         /* valid area len 74 * 4 */
            u32ReadStats = TDE_READ_REG(g_base_vir_addr, (0x800 + i * 4)); /* offset 0x800 + i *4  */
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, (0x800 + i * 4));  /* offset 0x800 + i *4  */
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, u32ReadStats);
        }
#endif

        U_PERI_CRG337 unTempValue;
        // unTempValue.u32 = g_reg_crg->PERI_CRG37.u32;
        unTempValue.bits.tde_srst_req = 0x1;
        // g_reg_crg->PERI_CRG37.u32 = unTempValue.u32;
        for (i = 0; i < 100; i++) { /* wait 100 loops */
            ;
        }
        unTempValue.bits.tde_srst_req = 0x0;
        // g_reg_crg->PERI_CRG37.u32 = unTempValue.u32;

        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "tde interrupts coredump");
        tde_hal_resume_init();

        return OSAL_IRQ_HANDLED;
    }
#ifdef TDE_HWC_COOPERATE
    if (int_status & TDE_DRV_INT_TIMEOUT) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "tde hardware timeout");
        tde_hal_resume_init();
        return OSAL_IRQ_HANDLED;
    }
#endif
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, int_status);

    tde_tasklet.data = tde_tasklet.data | ((unsigned long)int_status);
    osal_tasklet_update(&tde_tasklet);

    osal_tasklet_schedule(&tde_tasklet);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return OSAL_IRQ_HANDLED;
}

STATIC void tde_tasklet_func(unsigned long int_status)
{
    hi_size_t lockflags;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    tde_lock(&s_taskletlock, lockflags);
    tde_tasklet.data &= (~int_status);
    tde_unlock(&s_taskletlock, lockflags);

#ifdef TDE_TIME_COUNT
    (hi_void) osal_get_timeofday(&g_stTimeEnd);

    g_u64TimeDiff = (g_stTimeEnd.tv_sec - g_stTimeStart.tv_sec) * 1000000 /* 1000000 us */
                    + (g_stTimeEnd.tv_usec - g_stTimeStart.tv_usec);
#endif

#ifdef TDE_HWC_COOPERATE
    if (int_status & TDE_DRV_INT_NODE) {
        tde_list_node_comp();
    }
    tde_list_comp();
#else
    if (int_status & TDE_DRV_INT_NODE) {
        tde_list_node_comp();
    }
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
}

hi_s32 drv_tde_suspend(struct device *dev)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);

    drv_tde_wait_all_done(HI_FALSE);

    tde_hal_suspend();

    HI_PRINT("tde suspend ok\n");

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

hi_s32 drv_tde_resume(struct device *dev)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);

    if (osal_atomic_read(&g_TDECount) > 0) {
        tde_hal_resume_init();
    }

    HI_PRINT("tde resume ok\n");

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

hi_s32 drv_tde_resume_early(struct device *dev)
{
    return HI_SUCCESS;
}

/*****************************************************************************
 Prototype       : drv_tde_open
 Description     : open TDE equipment
 Input           :
 Output          : None
 Return Value    :
*****************************************************************************/
hi_s32 drv_tde_open(hi_void)
{
    hi_s32 Ret = HI_SUCCESS;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    Ret = drv_tde_dev_open(NULL, NULL);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return Ret;
}

/*****************************************************************************
 Prototype       : drv_tde_close
 Description     : close TDE equipment
 Input           :
 Output          : None
 Return Value    :
*****************************************************************************/
hi_s32 drv_tde_close(hi_void)
{
    hi_s32 Ret = HI_SUCCESS;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    Ret = drv_tde_dev_release(NULL, NULL);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return Ret;
}

EXPORT_SYMBOL(drv_tde_open);
EXPORT_SYMBOL(drv_tde_close);
EXPORT_SYMBOL(drv_tde_module_init_k);
EXPORT_SYMBOL(drv_tde_module_exit_k);

EXPORT_SYMBOL(drv_tde_dev_ioctl);
EXPORT_SYMBOL(drv_tde_dev_open);
EXPORT_SYMBOL(drv_tde_dev_release);
