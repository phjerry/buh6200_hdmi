/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: logo control release
 * Author: sdk
 * Create: 2019-08-17
 */

#ifdef CONFIG_HIFB_LOGO_SUPPORT
#include "drv_hifb_logo.h"
#include <linux/hisilicon/securec.h>
#include "hi_osal.h"

#include "drv_disp_ext.h"
#include "drv_hifb_debug.h"
#include "drv_hifb_blit.h"
#include "drv_hifb_config.h"
#include "drv_hifb_adp.h"
#include "drv_hifb_osr.h"
#include "drv_pdm_ext.h"

#define DRV_HIFB_LOGO_DISP0 0
#define DRV_HIFB_LOGO_DISP1 4
#define DRV_HIFB_LOGO_DISP_NUM 2
#define DRV_HIFB_LOGO_TIMER_OUT 1000
#define DRV_HIFB_LOGO_SCHED_PRI 99

#if defined(HI_LOG_SUPPORT) && (HI_LOG_SUPPORT == 0) && !defined(CONFIG_HIFB_LOGO_DEBUG)
#define HIFB_LOGO_DEBUG(fmt, args...)
#else
#define HIFB_LOGO_DEBUG osal_printk
#endif

static drv_hifb_logo_info g_logo_info[DRV_HIFB_LOGO_DISP_NUM] = {{0}};
static HIFB_LAYER_S *g_hifb_logo_layer = NULL;
static hi_void hifb_logo_set_layer_info(hi_u32 layer_id, struct fb_info *info, hi_disp_param *disp_para);
static hi_s32 hifb_logo_transition(hi_void *data);
static inline hi_void hifb_logo_wait(hi_u32 disp_id);
static hi_void hifb_logo_clear_surface(hi_u32 disp_id);
static hi_s32 hifb_logo_check_surface(hi_u32 disp_id);
static HIFB_DRV_OPS_S gs_stLogoHifbAdpCallBackFunction;

hi_void drv_hifb_logo_get_base_info(hi_u32 layer_id)
{
    hi_s32 ret;
    struct fb_info *info = NULL;
    hi_disp_param disp_para;
    hi_pdm_export_func *pdm_export_funcs = NULL;

    drv_hifb_get_layer(layer_id, &g_hifb_logo_layer);
    if (g_hifb_logo_layer == NULL) {
        HIFB_LOGO_DEBUG("g_hifb_logo_layer pointer is null\n");
        return;
    }

    if ((layer_id != 0) && (layer_id != 3)) { /* 3 is wbc or mipi layer */
        HIFB_LOGO_DEBUG("layer %d is not support\n", layer_id);
        return;
    }

    info = g_hifb_logo_layer->pstInfo;
    if (info == NULL) {
        HIFB_LOGO_DEBUG("info pointer is null\n");
        return;
    }

    ret = osal_exportfunc_get(HI_ID_PDM, (hi_void **)&pdm_export_funcs);
    if ((ret != HI_SUCCESS) || (pdm_export_funcs == NULL)) {
        HIFB_LOGO_DEBUG("call hi_drv_module_get_function failure\n");
        return;
    }

    if (pdm_export_funcs->pdm_get_disp_param == NULL) {
        HIFB_LOGO_DEBUG("call pdm_get_disp_param failure\n");
        return;
    }

    if (layer_id == 0) {
        ret = pdm_export_funcs->pdm_get_disp_param(HI_DRV_DISPLAY_0, &disp_para);
    } else {
        ret = pdm_export_funcs->pdm_get_disp_param(HI_DRV_DISPLAY_1, &disp_para);
    }
    if (ret == HI_FAILURE) {
        HIFB_LOGO_DEBUG("call pdm_get_disp_param failure\n");
        return;
    }
    hifb_logo_set_layer_info(layer_id, info, &disp_para);
}

static inline DRV_HIFB_COLOR_FMT_E HIFB_LOGO_ConvertBootFmtToLayerFmt(hi_u32 BootFmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    if (HI_PF_ARG1555 == BootFmt) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return DRV_HIFB_FMT_ARGB1555;
    } else if (HI_PF_ARG8888 == BootFmt) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return DRV_HIFB_FMT_ARGB8888;
    } else {
        return DRV_HIFB_FMT_BUTT;
    }
}

static hi_void hifb_logo_set_layer_info(hi_u32 layer_id, struct fb_info *info, hi_disp_param *disp_para)
{
    HIFB_PAR_S *par = NULL;
    HIFB_RECT stInRect = {0};
    DRV_HIFB_COLOR_FMT_E enHifbFmt = DRV_HIFB_FMT_BUTT;
    hi_u32 u32PixDepth = 0;
    hi_u32 CmpStride = 0;
    hi_u32 layer_stride = 0;
    hi_s32 ret;
    par = (HIFB_PAR_S *)(info->par);
    if (par == NULL) {
        HIFB_LOGO_DEBUG("info pointer is null\n");
        return;
    }
    info->var.xres = disp_para->virt_screen_width;
    info->var.yres = disp_para->virt_screen_height;
    info->var.xres_virtual = info->var.xres;
    info->var.yres_virtual = info->var.yres * CONFIG_HIFB_LAYER_BUFFER_MAX_NUM;
    info->var.xoffset = 0;
    info->var.yoffset = 0;

    par->stExtendInfo.stPos.s32XPos = 0;
    par->stExtendInfo.stPos.s32YPos = 0;
    par->stExtendInfo.DisplayWidth = info->var.xres;
    par->stExtendInfo.DisplayHeight = info->var.yres;

    enHifbFmt = HIFB_LOGO_ConvertBootFmtToLayerFmt((hi_u32)disp_para->pixel_format);
    if (enHifbFmt >= DRV_HIFB_FMT_PUYVY) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, enHifbFmt);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, DRV_HIFB_FMT_PUYVY);
        return;
    }

    info->var.bits_per_pixel = DRV_HIFB_MEM_GetBppByFmt(enHifbFmt);
    if (0 == info->var.bits_per_pixel) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, info->var.bits_per_pixel);
        return;
    }

    ret = HIFB_PARA_CHECK_SUPPORT(HIFB_PARA_CHECK_WIDTH | HIFB_PARA_CHECK_HEIGHT | HIFB_PARA_CHECK_BITSPERPIXEL,
                                  disp_para->virt_screen_width, disp_para->virt_screen_height, 0, 0,
                                  info->var.bits_per_pixel);
    if (HI_SUCCESS != ret) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, info->var.bits_per_pixel);
        return;
    }

    info->var.red = s_stArgbBitField[enHifbFmt].stRed;
    info->var.green = s_stArgbBitField[enHifbFmt].stGreen;
    info->var.blue = s_stArgbBitField[enHifbFmt].stBlue;
    info->var.transp = s_stArgbBitField[enHifbFmt].stTransp;

    info->fix.line_length = CONIFG_HIFB_GetMaxStride(info->var.xres_virtual, info->var.bits_per_pixel, &CmpStride,
                                                     CONFIG_HIFB_STRIDE_ALIGN);

    u32PixDepth = info->var.bits_per_pixel >> 3;
    layer_stride = (info->var.xres_virtual * u32PixDepth + CONFIG_HIFB_STRIDE_ALIGN - 1) &
                   (~(CONFIG_HIFB_STRIDE_ALIGN - 1));
    par->stExtendInfo.enColFmt = enHifbFmt;
    par->stExtendInfo.stPos.s32XPos = 0;
    par->stExtendInfo.stPos.s32YPos = 0;
    par->stExtendInfo.DisplayWidth = info->var.xres;
    par->stExtendInfo.DisplayHeight = info->var.yres;

    stInRect.x = par->stExtendInfo.stPos.s32XPos;
    stInRect.y = par->stExtendInfo.stPos.s32YPos;
    stInRect.w = par->stExtendInfo.DisplayWidth;
    stInRect.h = par->stExtendInfo.DisplayHeight;
    ret = gs_stLogoHifbAdpCallBackFunction.DRV_HIFB_ADP_SetLayerRect(layer_id, &stInRect);
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_ADP_SetLayerRect, ret);
    }
    ret = gs_stLogoHifbAdpCallBackFunction.DRV_HIFB_ADP_SetLayerStride(layer_id, layer_stride);
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_ADP_SetLayerStride, ret);
    }
    ret = gs_stLogoHifbAdpCallBackFunction.DRV_HIFB_ADP_SetLayerDataFmt(layer_id, par->stExtendInfo.enColFmt);
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_ADP_SetLayerDataFmt, ret);
    }
    return;
}

hi_s32 drv_init_logo_wait_queue(hi_void)
{
    hi_u32 i;

    for (i = 0; i < DRV_HIFB_LOGO_DISP_NUM; i++) {
        if (osal_wait_init(&(g_logo_info[i].wait_queue)) != 0) {
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

hi_void drv_deinit_logo_wait_queue(hi_void)
{
    hi_u32 i;

    for (i = 0; i < DRV_HIFB_LOGO_DISP_NUM; i++) {
        if (g_logo_info[i].wait_queue.wait != HI_NULL) {
            osal_wait_destroy(&(g_logo_info[i].wait_queue));
        }
    }
    return;
}

hi_void drv_hifb_logo_init(hi_ulong smem_start, hi_char *screen_base, hi_u32 stride, hi_u32 size, hi_u32 layer_id)
{
    hi_s32 disp_id;
    osal_task *task = NULL;

    DRV_HIFB_ADP_GetCallBackFunction(&gs_stLogoHifbAdpCallBackFunction);

    if ((layer_id != DRV_HIFB_LOGO_DISP0) && (layer_id != DRV_HIFB_LOGO_DISP1)) {
        return;
    } else {
        disp_id = (layer_id == DRV_HIFB_LOGO_DISP0) ? 0 : 1;
    }

    if (g_logo_info[disp_id].finish_transition == HI_TRUE) {
        return;
    }
    g_logo_info[disp_id].finish_transition = HI_TRUE;

    g_logo_info[disp_id].disp_id = disp_id;
    g_logo_info[disp_id].surface.is_mute = HI_TRUE;
    g_logo_info[disp_id].surface.stride = stride;
    g_logo_info[disp_id].surface.size = size;
    g_logo_info[disp_id].surface.screen_base = screen_base;
    g_logo_info[disp_id].surface.smem_start = smem_start;

    task = osal_kthread_create(hifb_logo_transition, &g_logo_info[disp_id], "hifb_logo_transition_thread", 0);
    if ((task == NULL) || (IS_ERR(task))) {
        HIFB_LOGO_DEBUG("call kthread_create failure\n");
        return;
    }

    osal_kthread_set_priority(task, OSAL_TASK_PRIORITY_HIGH);

    g_logo_info[disp_id].has_been_inited = HI_TRUE;
    g_logo_info[disp_id].pthread_task = task;

    return;
}

hi_void drv_hifb_logo_deinit(hi_u32 layer_id)
{
    hi_s32 disp_id;
    disp_id = (layer_id == DRV_HIFB_LOGO_DISP0) ? 0 : 1;

    if (g_logo_info[disp_id].has_been_inited == HI_FALSE) {
        return;
    }
    g_logo_info[disp_id].has_been_inited = HI_FALSE;

    if (g_logo_info[disp_id].pthread_task == NULL) {
        return;
    }

    if (!IS_ERR(g_logo_info[disp_id].pthread_task)) {
        osal_kthread_destroy(g_logo_info[disp_id].pthread_task, HI_TRUE);
        g_logo_info[disp_id].pthread_task = NULL;
    }

    g_logo_info[disp_id].need_wait_up = HI_TRUE;
    osal_wait_wakeup(&(g_logo_info[disp_id].wait_queue));
    return;
}

static hi_s32 hifb_logo_transition(hi_void *data)
{
    hi_bool is_mute = HI_FALSE;
    hi_s32 ret;
    hi_u32 disp_id;
    drv_hifb_logo_info *logo_info = (drv_hifb_logo_info *)data;
    HIFB_DRV_OPS_S callback_function;
    DRV_HIFB_ADP_GetCallBackFunction(&callback_function);

    if (callback_function.drv_hifb_adp_set_up_mute == NULL) {
        HIFB_LOGO_DEBUG("drv_hifb_adp_set_up_mute pointer is null\n");
        return HI_FAILURE;
    }

    if (data == NULL) {
        HIFB_LOGO_DEBUG("data pointer is null\n");
        return HI_FAILURE;
    }

    disp_id = logo_info->disp_id;
    if (disp_id >= DRV_HIFB_LOGO_DISP_NUM) {
        HIFB_LOGO_DEBUG("disp_id(%d) >= [%d]\n", disp_id, DRV_HIFB_LOGO_DISP_NUM);
        return HI_FAILURE;
    }

    hifb_logo_clear_surface(disp_id);

    while (!osal_kthread_should_stop()) {
        if (osal_try_to_freeze() == HI_TRUE) {
            continue;
        }

        if (callback_function.drv_hifb_adp_set_up_mute != NULL) {
            callback_function.drv_hifb_adp_get_up_mute(disp_id, &is_mute);
        }

        if (is_mute == HI_FALSE) {
            hifb_logo_wait(disp_id);
            continue;
        }

        ret = hifb_logo_check_surface(disp_id);
        if (ret == HI_SUCCESS) {
            callback_function.drv_hifb_adp_set_up_mute(disp_id, HI_FALSE);
        }

        hifb_logo_wait(disp_id);
    }

    return HI_SUCCESS;
}

hi_s32 hifb_check_wait_up(const void *param)
{
    hi_u32 *disp_id = (hi_u32 *)param;

    if (disp_id == HI_NULL) {
        return 0;
    }
    if (g_logo_info[*disp_id].need_wait_up == HI_TRUE) {
        return 1;
    }
    return 0;
}

static inline hi_void hifb_logo_wait(hi_u32 disp_id)
{
    osal_wait_timeout_interruptible(&g_logo_info[disp_id].wait_queue, hifb_check_wait_up, (hi_void *)&disp_id,
                                    DRV_HIFB_LOGO_TIMER_OUT);
    g_logo_info[disp_id].need_wait_up = HI_FALSE;
}

static hi_void hifb_logo_clear_surface(hi_u32 disp_id)
{
    hi_s32 ret;
    HIFB_DRV_TDEOPS_S callback_function = {0};
    HIFB_SURFACE_S surface = {0};
    HIFB_BLIT_OPT_S opt = {0};

    DRV_HIFB_GFX2D_GetCallBackFunction(&callback_function);
    if (callback_function.DRV_HIFB_GFX2D_ClearRect == NULL) {
        HIFB_LOGO_DEBUG("DRV_HIFB_GFX2D_ClearRect pointer is null\n");
        return;
    }

    if (g_logo_info[disp_id].surface.stride == 0) {
        HIFB_LOGO_DEBUG("g_logo_info[disp_id].surface.stride == 0\n");
        return;
    }

    return;

    surface.u32Width = g_logo_info[disp_id].surface.stride / 4; /* 4 for argb8888 bytes */
    surface.u32Height = g_logo_info[disp_id].surface.size / g_logo_info[disp_id].surface.stride;
    surface.u32Pitch = g_logo_info[disp_id].surface.stride;
    surface.enFmt = DRV_HIFB_FMT_ARGB8888;
    surface.u32PhyAddr = g_logo_info[disp_id].surface.smem_start;

    opt.bBlock = HI_TRUE;
    ret = callback_function.DRV_HIFB_GFX2D_ClearRect(&surface, &opt);
    if (ret == HI_FAILURE) {
        HIFB_LOGO_DEBUG("call DRV_HIFB_GFX2D_ClearRect failure ret[0x%x]\n", ret);
    }
}

static hi_s32 hifb_logo_check_surface(hi_u32 disp_id)
{
    hi_s32 loop;
    hi_u32 stride;
    hi_u32 height;
    hi_u32 layer_id;
    hi_u32 cur_disp_addr;
    hi_char *disp_vir = NULL;
    hi_char *cur_disp_vir = NULL;
    HIFB_DRV_OPS_S callback_function;
    DRV_HIFB_ADP_GetCallBackFunction(&callback_function);

    layer_id = (disp_id == 0) ? (DRV_HIFB_LOGO_DISP0) : (DRV_HIFB_LOGO_DISP1);
    if (callback_function.DRV_HIFB_ADP_GetLayerAddr == NULL) {
        HIFB_LOGO_DEBUG("DRV_HIFB_ADP_GetLayerAddr pointer is null\n");
        return HI_FAILURE;
    }
    callback_function.DRV_HIFB_ADP_GetLayerAddr(layer_id, &cur_disp_addr);

    if ((g_logo_info[disp_id].surface.stride == 0) || (g_logo_info[disp_id].surface.size == 0)) {
        HIFB_LOGO_DEBUG("stride == 0 or size == 0\n");
        return HI_FAILURE;
    }

    stride = g_logo_info[disp_id].surface.stride;
    height = g_logo_info[disp_id].surface.size / g_logo_info[disp_id].surface.stride;
    height = height / CONFIG_HIFB_LAYER_BUFFER_MAX_NUM;

    if (g_logo_info[disp_id].surface.screen_base == NULL) {
        HIFB_LOGO_DEBUG("disp_buf pointer is null\n");
        return HI_FAILURE;
    }

    if (cur_disp_addr < g_logo_info[disp_id].surface.smem_start) {
        HIFB_LOGO_DEBUG("[0x%x] != [0x%lx]\n", cur_disp_addr, g_logo_info[disp_id].surface.smem_start);
        return HI_FAILURE;
    }

    disp_vir = g_logo_info[disp_id].surface.screen_base;
    for (loop = 0; loop < height; loop++) {
        cur_disp_vir = disp_vir + loop * g_logo_info[disp_id].surface.stride;
        if (cur_disp_vir[0] == 0) {
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}
#endif
