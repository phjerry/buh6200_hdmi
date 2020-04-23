/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ioctl
 * Create: 2019-06-29
 */

#include "drv_hifb_ioctl.h"
#include "hi_osal.h"

#ifdef __LITEOS__
#include "hifb_liteos.h"
#else
#include "hifb.h"
#endif
#include "drv_hifb_osr.h"
#include "drv_hifb_refresh.h"
#include "drv_hifb_param_convert.h"

#ifndef __LITEOS__
#define HIFBIOGET_SCREENINFO _IOR(HIFB_IOC_TYPE, 110, hi_u32)
#define HIFBIOPUT_SCREENINFO _IOW(HIFB_IOC_TYPE, 111, hi_u32)
#endif
#define DRV_HIFB_IOCTLCMD_NUM_MAX 173
#define DRV_HIFB_IOCTLFUNC_ITEM_NUM_MAX 27

typedef hi_s32 (*drv_hifb_ioctl_func_ptr)(struct fb_info *info, hi_ulong arg);

typedef struct {
    hi_u32 cmd;
    drv_hifb_ioctl_func_ptr ioctl_func;
} drv_hifb_ioctl_func_item;

static HIFB_DRV_OPS_S g_hifb_adp_func;

static hi_s32 drv_hifb_get_show(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_set_show(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_get_decompress(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_set_decompress(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_get_colorkey(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_set_colorkey(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_get_alpha(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_set_alpha(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_get_screen_position(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_set_screen_position(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_get_screen_info(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_set_screen_info(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_get_vscreen_info(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_set_vscreen_info(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_get_layer_info(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_set_layer_info(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_scrolltext_create(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_scrolltext_fill(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_scrolltext_pause(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_scrolltext_resume(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_scrolltext_destroy(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_get_stereo_depth(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_set_stereo_depth(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_refresh(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_vblank(struct fb_info *info, hi_ulong arg);
static hi_s32 drv_hifb_get_dma_buf(struct fb_info *info, hi_ulong arg);

static hi_s32 g_ioctl_func_index[DRV_HIFB_IOCTLCMD_NUM_MAX] = {
    0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* index is 89  */
    1,  2,  3,  4,  5,  6,  7, 8, 9, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                               /* index is 109 */
    11, 12, 13, 14, 15, 16, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                               /* index is 129 */
    17, 18, 19, 20, 21, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                               /* index is 149 */
    22, 23, 0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                               /* index is 169 */
    24, 25, 26                                                                                       /* index is 172 */
};

static drv_hifb_ioctl_func_item g_ioctl_func[DRV_HIFB_IOCTLFUNC_ITEM_NUM_MAX] = {
    { 0, NULL },
    { HIFBIOGET_SHOW, drv_hifb_get_show },
    { HIFBIOPUT_SHOW, drv_hifb_set_show },
    { HIFBIOGET_DECOMPRESS, drv_hifb_get_decompress },
    { HIFBIOPUT_DECOMPRESS, drv_hifb_set_decompress },
    { HIFBIOGET_COLORKEY, drv_hifb_get_colorkey },
    { HIFBIOPUT_COLORKEY, drv_hifb_set_colorkey },
    { HIFBIOGET_ALPHA, drv_hifb_get_alpha },
    { HIFBIOPUT_ALPHA, drv_hifb_set_alpha },
    { HIFBIOGET_SCREENPOS, drv_hifb_get_screen_position },
    { HIFBIOPUT_SCREENPOS, drv_hifb_set_screen_position },
    { HIFBIOGET_SCREENINFO, drv_hifb_get_screen_info },
    { HIFBIOPUT_SCREENINFO, drv_hifb_set_screen_info },
    { HIFBIOGET_VSCREENINFO, drv_hifb_get_vscreen_info },
    { HIFBIOPUT_VSCREENINFO, drv_hifb_set_vscreen_info },
    { HIFBIOGET_LAYERINFO, drv_hifb_get_layer_info },
    { HIFBIOPUT_LAYERINFO, drv_hifb_set_layer_info },
    { HIFBIO_SCROLLTEXT_CREATE, drv_hifb_scrolltext_create },
    { HIFBIO_SCROLLTEXT_FILL, drv_hifb_scrolltext_fill },
    { HIFBIO_SCROLLTEXT_PAUSE, drv_hifb_scrolltext_pause },
    { HIFBIO_SCROLLTEXT_RESUME, drv_hifb_scrolltext_resume },
    { HIFBIO_SCROLLTEXT_DESTORY, drv_hifb_scrolltext_destroy },
    { HIFBIOPUT_STEREODEPTH, drv_hifb_get_stereo_depth },
    { HIFBIOGET_STEREODEPTH, drv_hifb_set_stereo_depth },
    { HIFBIOPUT_REFRESH, drv_hifb_refresh },
    { HIFBIOGET_VBLANK, drv_hifb_vblank },
    { HIFBIOGET_DMABUF, drv_hifb_get_dma_buf }
};

hi_s32 drv_hifb_ioctl(struct fb_info *info, hi_u32 cmd, unsigned long arg)
{
    hi_s32 ref_count = 0;
    hi_u8 tmp_cmd = _IOC_NR(cmd);
    HIFB_PAR_S *par = NULL;

    if (info == NULL) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d info is null\n", __FUNCTION__, __LINE__);
        return -EFAULT;
    }

    par = (HIFB_PAR_S *)info->par;
    if (par == NULL) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d par is null\n", __FUNCTION__, __LINE__);
        return -EFAULT;
    }

    ref_count = osal_atomic_read(&par->stBaseInfo.ref_count);
    if (ref_count <= 0) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d ref_count small zero\n", __FUNCTION__, __LINE__);
        return -EINVAL;
    }

    if (par->stBaseInfo.u32LayerID >= HIFB_LAYER_ID_BUTT) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d layer id %d\n", __FUNCTION__, __LINE__, par->stBaseInfo.u32LayerID);
        return -EFAULT;
    }

    if ((tmp_cmd < 1) || (tmp_cmd > DRV_HIFB_IOCTLCMD_NUM_MAX)) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d tmp_cmd %d\n", __FUNCTION__, __LINE__, tmp_cmd);
        return HI_FAILURE;
    }

    if ((g_ioctl_func_index[tmp_cmd] < 1) || (g_ioctl_func_index[tmp_cmd] > DRV_HIFB_IOCTLFUNC_ITEM_NUM_MAX)) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d tmp_cmd %d\n", __FUNCTION__, __LINE__, tmp_cmd);
        return HI_FAILURE;
    }

    if (g_ioctl_func[g_ioctl_func_index[tmp_cmd]].ioctl_func == NULL) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d ioctl func is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    if (cmd != g_ioctl_func[g_ioctl_func_index[tmp_cmd]].cmd) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d ioctl func is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    DRV_HIFB_ADP_GetCallBackFunction(&g_hifb_adp_func);

    return g_ioctl_func[g_ioctl_func_index[tmp_cmd]].ioctl_func(info, arg);
}

static hi_s32 drv_hifb_get_show(struct fb_info *info, hi_ulong arg)
{
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    HIFB_PAR_S *par = HI_NULL;

    if ((info == HI_NULL) || (argp == HI_NULL)) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d info or argp is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    par = (HIFB_PAR_S *)info->par;
    if (par == HI_NULL) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d par is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    if (osal_copy_to_user(argp, &par->stExtendInfo.bShow, sizeof(hi_bool))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 drv_hifb_set_show(struct fb_info *info, hi_ulong arg)
{
    hi_s32 ret;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    hi_bool show = HI_FALSE;
    HIFB_PAR_S *par = HI_NULL;

    if ((info == HI_NULL) || (argp == HI_NULL)) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d info or argp is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    par = (HIFB_PAR_S *)info->par;
    if (par == HI_NULL) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d par is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    ret = osal_copy_from_user(&show, argp, sizeof(hi_bool));
    if (ret != 0) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, ret);
        return -EFAULT;
    }

    /* reset the same status */
    if (show == par->stExtendInfo.bShow) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
        return HI_SUCCESS;
    }

    par->stRunInfo.bModifying = HI_TRUE;
    par->stExtendInfo.bShow = show;
    par->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_SHOW;
    par->stRunInfo.bModifying = HI_FALSE;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 drv_hifb_get_decompress(struct fb_info *info, hi_ulong arg)
{
    return HI_SUCCESS;
}

static hi_s32 drv_hifb_set_decompress(struct fb_info *info, hi_ulong arg)
{
    return HI_SUCCESS;
}

static hi_s32 drv_hifb_get_colorkey(struct fb_info *info, hi_ulong arg)
{
    HIFB_PAR_S *par = NULL;
    hifb_colorkey color_key;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;

    if ((info == NULL) || (argp == NULL)) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d info or argp is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    par = (HIFB_PAR_S *)info->par;
    if (par == NULL) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d par is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    memset(&color_key, 0x0, sizeof(color_key));
    color_key.enable = par->stExtendInfo.stCkey.bKeyEnable;
    color_key.key_value = par->stExtendInfo.stCkey.u32Key;

    if (osal_copy_to_user(argp, &color_key, sizeof(hifb_colorkey))) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d osal_copy_to_user failure\n", __FUNCTION__, __LINE__);
        return -EFAULT;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_hifb_set_colorkey(struct fb_info *info, hi_ulong arg)
{
    hi_u32 red_len, green_len, blue_len, red_offset, green_offset, blue_offset;
    hi_u8 red_mask, green_mask, blue_mask, red_key, green_key, blue_key;
    HIFB_PAR_S *par = NULL;
    hifb_colorkey color_key;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;

    if ((info == NULL) || (argp == NULL)) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d info or argp is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    par = (HIFB_PAR_S *)info->par;
    if (par == NULL) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d par is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    if (osal_copy_from_user(&color_key, argp, sizeof(hifb_colorkey))) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d osal_copy_from_user failure\n", __FUNCTION__, __LINE__);
        return -EFAULT;
    }

    if ((color_key.enable == HI_TRUE) && (par->stBaseInfo.bPreMul == HI_TRUE)) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d key enable and premul open\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    par->stRunInfo.bModifying = HI_TRUE;
    par->stExtendInfo.stCkey.u32Key = color_key.key_value;
    par->stExtendInfo.stCkey.bKeyEnable = color_key.enable;

    if (info->var.bits_per_pixel == 0) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d info->var.bits_per_pixel is zero\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    if (info->var.bits_per_pixel <= 8) {
        if (color_key.key_value >= (1 << info->var.bits_per_pixel)) {
            HI_PRINT("[module-gfx-hifb][err] : %s %d failure\n", __FUNCTION__, __LINE__);
            return HI_FAILURE;
        }
        par->stExtendInfo.stCkey.u8BlueMin = info->cmap.blue[color_key.key_value];
        par->stExtendInfo.stCkey.u8GreenMin = info->cmap.green[color_key.key_value];
        par->stExtendInfo.stCkey.u8RedMin = info->cmap.red[color_key.key_value];
        par->stExtendInfo.stCkey.u8BlueMax = par->stExtendInfo.stCkey.u8BlueMin;
        par->stExtendInfo.stCkey.u8GreenMax = par->stExtendInfo.stCkey.u8GreenMin;
        par->stExtendInfo.stCkey.u8RedMax = par->stExtendInfo.stCkey.u8RedMin;
    } else {
        g_hifb_adp_func.DRV_HIFB_ADP_ColorConvert(&info->var, &par->stExtendInfo.stCkey);

        if (par->stExtendInfo.enColFmt >= HIFB_MAX_PIXFMT_NUM) {
            HI_PRINT("[module-gfx-hifb][err] : %s %d failure\n", __FUNCTION__, __LINE__);
            return HI_FAILURE;
        }

        red_len = s_stArgbBitField[par->stExtendInfo.enColFmt].stRed.length;
        green_len = s_stArgbBitField[par->stExtendInfo.enColFmt].stGreen.length;
        blue_len = s_stArgbBitField[par->stExtendInfo.enColFmt].stBlue.length;

        red_mask = (0xff >> (8 - red_len));
        green_mask = (0xff >> (8 - green_len));
        blue_mask = (0xff >> (8 - blue_len));

        red_offset = s_stArgbBitField[par->stExtendInfo.enColFmt].stRed.offset;
        green_offset = s_stArgbBitField[par->stExtendInfo.enColFmt].stGreen.offset;
        blue_offset = s_stArgbBitField[par->stExtendInfo.enColFmt].stBlue.offset;

        if (par->stExtendInfo.enColFmt != DRV_HIFB_FMT_ABGR8888) {
            red_key = (par->stExtendInfo.stCkey.u32Key >> (blue_len + green_len)) & (red_mask);
            green_key = (par->stExtendInfo.stCkey.u32Key >> (blue_len)) & (green_mask);
            blue_key = (par->stExtendInfo.stCkey.u32Key) & (blue_mask);
        } else {
            red_key = (par->stExtendInfo.stCkey.u32Key >> red_offset) & (red_mask);
            green_key = (par->stExtendInfo.stCkey.u32Key >> green_offset) & (green_mask);
            blue_key = (par->stExtendInfo.stCkey.u32Key >> blue_offset) & (blue_mask);
        }

        /* add low bit with 0 value */
        par->stExtendInfo.stCkey.u8RedMin = red_key << (8 - red_len);
        par->stExtendInfo.stCkey.u8GreenMin = green_key << (8 - green_len);
        par->stExtendInfo.stCkey.u8BlueMin = blue_key << (8 - blue_len);

        /* add low bit with 1 value */
        par->stExtendInfo.stCkey.u8RedMax = par->stExtendInfo.stCkey.u8RedMin | (0xff >> red_len);
        par->stExtendInfo.stCkey.u8GreenMax = par->stExtendInfo.stCkey.u8GreenMin | (0xff >> green_len);
        par->stExtendInfo.stCkey.u8BlueMax = par->stExtendInfo.stCkey.u8BlueMin | (0xff >> blue_len);
    }

    par->stExtendInfo.stCkey.u8RedMask = 0xff;
    par->stExtendInfo.stCkey.u8BlueMask = 0xff;
    par->stExtendInfo.stCkey.u8GreenMask = 0xff;

    par->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_COLORKEY;
    par->stRunInfo.bModifying = HI_FALSE;

    return HI_SUCCESS;
}

static hi_s32 drv_hifb_get_alpha(struct fb_info *info, hi_ulong arg)
{
    HIFB_PAR_S *par = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    hifb_alpha alpha = {0};

    if ((info == NULL) || (argp == NULL)) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d info or argp is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    par = (HIFB_PAR_S *)info->par;
    if (par == NULL) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d par is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    alpha.alpha0 = par->stExtendInfo.stAlpha.u8Alpha0;
    alpha.alpha1 = par->stExtendInfo.stAlpha.u8Alpha1;
    alpha.global_alpha = par->stExtendInfo.stAlpha.u8GlobalAlpha;
    alpha.global_alpha_en = par->stExtendInfo.stAlpha.bAlphaEnable;
    alpha.pixel_alpha_en = par->stExtendInfo.stAlpha.bAlphaChannel;

    if (osal_copy_to_user(argp, &alpha, sizeof(hifb_alpha))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 drv_hifb_set_alpha(struct fb_info *info, hi_ulong arg)
{
    HIFB_PAR_S *par = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    hifb_alpha alpha = {0};

    if ((info == NULL) || (argp == NULL)) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d info or argp is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    par = (HIFB_PAR_S *)info->par;
    if (par == NULL) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d par is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    if (osal_copy_from_user(&alpha, argp, sizeof(hifb_alpha))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    if (alpha.global_alpha_en == HI_FALSE) {
        alpha.global_alpha |= 0xff;
    }

    par->stExtendInfo.stAlpha.u8Alpha0 = alpha.alpha0;
    par->stExtendInfo.stAlpha.u8Alpha1 = alpha.alpha1;
    par->stExtendInfo.stAlpha.u8GlobalAlpha = alpha.global_alpha;
    par->stExtendInfo.stAlpha.bAlphaEnable = alpha.pixel_alpha_en;
    par->stExtendInfo.stAlpha.bAlphaChannel = alpha.global_alpha_en;

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, par->stExtendInfo.stAlpha.u8Alpha0);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, par->stExtendInfo.stAlpha.u8Alpha1);

    par->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_ALPHA;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 drv_hifb_get_screen_position(struct fb_info *info, hi_ulong arg)
{
    HIFB_PAR_S *par = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    hifb_position position = {0};

    if ((info == NULL) || (argp == NULL)) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d info or argp is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    par = (HIFB_PAR_S *)info->par;
    if (par == NULL) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d par is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    position.x = par->stExtendInfo.stPos.s32XPos;
    position.y = par->stExtendInfo.stPos.s32YPos;

    if (osal_copy_to_user(argp, &position, sizeof(hifb_position))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 drv_hifb_set_screen_position(struct fb_info *info, hi_ulong arg)
{
    HIFB_PAR_S *par = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    hifb_position position = {0};

    if ((info == NULL) || (argp == NULL)) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d info or argp is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    par = (HIFB_PAR_S *)info->par;
    if (par == NULL) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d par is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    if (osal_copy_from_user(&position, argp, sizeof(hifb_position))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    if (position.x < 0 || position.y < 0) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, position.x);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, position.y);
        return HI_FAILURE;
    }

    par->stRunInfo.bModifying = HI_TRUE;
    par->stExtendInfo.stPos.s32XPos = position.x;
    par->stExtendInfo.stPos.s32YPos = position.y;
    par->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_INRECT;
    par->stRunInfo.bModifying = HI_FALSE;

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "=========================================================");
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, par->stExtendInfo.stPos.s32XPos);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, par->stExtendInfo.stPos.s32YPos);
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "=========================================================");

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 drv_hifb_get_screen_info(struct fb_info *info, hi_ulong arg)
{
    return HI_SUCCESS;
}

static hi_s32 drv_hifb_set_screen_info(struct fb_info *info, hi_ulong arg)
{
    return HI_SUCCESS;
}

static hi_s32 drv_hifb_get_vscreen_info(struct fb_info *info, hi_ulong arg)
{
    HIFB_PAR_S *par = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    hifb_vscreeninfo vscreen_info = {0};

    if ((info == NULL) || (argp == NULL)) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d info or argp is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    par = (HIFB_PAR_S *)info->par;
    if (par == NULL) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d par is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    DRV_HIFB_WaitVBlank(par->stBaseInfo.u32LayerID);
    vscreen_info.width = par->stDispInfo.stCanvasSur.u32Width;
    vscreen_info.height = par->stDispInfo.stCanvasSur.u32Height;
    vscreen_info.stride = par->stDispInfo.stCanvasSur.u32Pitch;
    vscreen_info.mem_size = par->stDispInfo.stCanvasSur.u32Pitch * par->stDispInfo.stCanvasSur.u32Height;
    vscreen_info.buf_mode = hifb_convert_buf_mode_drv_to_unf(par->stExtendInfo.enBufMode);

    if (osal_copy_to_user(argp, &vscreen_info, sizeof(vscreen_info))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 drv_hifb_set_vscreen_info(struct fb_info *info, hi_ulong arg)
{
    return HI_SUCCESS;
}

static hi_s32 hifb_get_bits_per_pixel(hifb_color_fmt fmt, hi_u32 *bits_per_pixel)
{
    if ((fmt == HIFB_COLOR_FMT_ARGB8888) || (fmt == HIFB_COLOR_FMT_ABGR8888) || (fmt == HIFB_COLOR_FMT_RGBA8888) ||
        (fmt == HIFB_COLOR_FMT_BGRA8888) || (fmt == HIFB_COLOR_FMT_ARGB2101010)) {
        *bits_per_pixel = 32; /* 32 bits in one pixel */
    } else if ((fmt == HIFB_COLOR_FMT_RGB888) || (fmt == HIFB_COLOR_FMT_BGR888)) {
        *bits_per_pixel = 24; /* 24 bits in one pixel */
    } else if ((fmt == HIFB_COLOR_FMT_ARGB1555) || (fmt == HIFB_COLOR_FMT_ABGR1555) ||
               (fmt == HIFB_COLOR_FMT_RGBA5551) || (fmt == HIFB_COLOR_FMT_BGRA5551)) {
        *bits_per_pixel = 16; /* 16 bits in one pixel */
    } else if (fmt == HIFB_COLOR_FMT_ARGB10101010) {
        *bits_per_pixel = 40; /* 40 bits in one pixel */
    } else {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, fmt);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 drv_hifb_get_layer_info(struct fb_info *info, hi_ulong arg)
{
    HIFB_PAR_S *par = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    hifb_layerinfo layer_info = {0};
    hi_u32 bits_per_pixel = 0;
    hi_s32 ret;

    if ((info == NULL) || (argp == NULL)) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d info or argp is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    par = (HIFB_PAR_S *)info->par;
    if (par == NULL) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d par is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    if (osal_copy_from_user(&layer_info, argp, sizeof(hifb_layerinfo))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    ret = hifb_get_bits_per_pixel(layer_info.surface.fmt, &bits_per_pixel);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    layer_info.surface.stride = CONIFG_HIFB_GetMaxStride(par->stExtendInfo.DisplayWidth, bits_per_pixel,
                                                         &layer_info.surface.cmp_stride, CONFIG_HIFB_STRIDE_ALIGN);

    if (osal_copy_to_user(argp, &layer_info, sizeof(hifb_layerinfo))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 hifb_check_layer_info(hifb_layerinfo *layer_info, hi_u32 layer_addr, struct fb_info *info,
                                    HIFB_PAR_S *par)
{
    hi_u32 bits_per_pixel, stride;
    hi_u32 cmp_stride = 0;
    hi_s32 ret;

    if ((layer_addr != info->fix.smem_start) &&
        (layer_addr != info->fix.smem_start + info->fix.line_length * par->stExtendInfo.DisplayHeight) &&
        (layer_addr != info->fix.smem_start + info->fix.line_length * par->stExtendInfo.DisplayHeight * 2)) { /* 2:
                                                                                                                 buffer
                                                                                                                 2 */
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, layer_addr);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, info->fix.smem_start);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT,
                              info->fix.smem_start + info->fix.line_length * par->stExtendInfo.DisplayHeight);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, info->fix.smem_start + info->fix.line_length *
                                                                               par->stExtendInfo.DisplayHeight *
                                                                               2); /* 2: buffer 2 */
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "input disp ddr is not alloc by fb");
        return HI_FAILURE;
    }

    ret = HIFB_PARA_CHECK_SUPPORT(HIFB_PARA_CHECK_WIDTH | HIFB_PARA_CHECK_HEIGHT, layer_info->surface.width,
                                  layer_info->surface.height, 0, 0, 0);
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, layer_info->surface.width);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, layer_info->surface.height);
        return ret;
    }

    ret = hifb_get_bits_per_pixel(layer_info->surface.fmt, &bits_per_pixel);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    stride = CONIFG_HIFB_GetMaxStride(par->stExtendInfo.DisplayWidth, bits_per_pixel, &cmp_stride,
                                      CONFIG_HIFB_STRIDE_ALIGN);
    if (((layer_info->compress_mode == HIFB_CMP_NONE) && (stride != layer_info->surface.stride)) ||
        ((layer_info->compress_mode == HIFB_CMP_HFBC) && (cmp_stride != layer_info->surface.stride))) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stride);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, cmp_stride);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, layer_info->surface.stride);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "input stride is not frome fb, input error");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 drv_hifb_set_layer_info(struct fb_info *info, hi_ulong arg)
{
    hi_s32 ret;
    hi_u32 layer_addr;
    HIFB_PAR_S *par = NULL;
    hifb_layerinfo layer_info = {0};
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;

    if ((info == NULL) || (argp == NULL)) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d info or argp is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    par = (HIFB_PAR_S *)info->par;
    if (par == NULL) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d par is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    if (osal_copy_from_user(&layer_info, argp, sizeof(hifb_layerinfo))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return HI_FAILURE;
    }

    layer_addr = drv_hifb_mem_get_smmu_from_fd(layer_info.surface.mem_handle);

    ret = hifb_check_layer_info(&layer_info, layer_addr, info, par);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if (layer_addr == par->stRunInfo.PreRefreshAddr) {
        par->stFrameInfo.RepeatFrameCnt++;
    }
    par->stRunInfo.PreRefreshAddr = layer_addr;

    ret = drv_hifb_refresh_flip(par, &layer_info, argp);
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_hifb_refresh_flip, ret);
        return HI_FAILURE;
    }
    par->stFrameInfo.RefreshFrame++;
    par->hifb_sync = HI_TRUE;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 drv_hifb_scrolltext_create(struct fb_info *info, hi_ulong arg)
{
    return HI_SUCCESS;
}

static hi_s32 drv_hifb_scrolltext_fill(struct fb_info *info, hi_ulong arg)
{
    return HI_SUCCESS;
}

static hi_s32 drv_hifb_scrolltext_pause(struct fb_info *info, hi_ulong arg)
{
    return HI_SUCCESS;
}

static hi_s32 drv_hifb_scrolltext_resume(struct fb_info *info, hi_ulong arg)
{
    return HI_SUCCESS;
}

static hi_s32 drv_hifb_scrolltext_destroy(struct fb_info *info, hi_ulong arg)
{
    return HI_SUCCESS;
}

static hi_s32 drv_hifb_get_stereo_depth(struct fb_info *info, hi_ulong arg)
{
    return HI_SUCCESS;
}

static hi_s32 drv_hifb_set_stereo_depth(struct fb_info *info, hi_ulong arg)
{
    return HI_SUCCESS;
}

static hi_s32 drv_hifb_refresh(struct fb_info *info, hi_ulong arg)
{
    return HI_SUCCESS;
}

static hi_s32 drv_hifb_vblank(struct fb_info *info, hi_ulong arg)
{
    hi_s32 ret;
    HIFB_PAR_S *par = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    hifb_vblank_info vblank_info = {0};
    hi_u64 current_us;
    osal_timeval now;

    if (info == NULL) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d info or argp is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    par = (HIFB_PAR_S *)info->par;
    if (par == NULL) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d par is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    ret = g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_WaitVBlank(par->stBaseInfo.u32LayerID);
    if (ret < 0) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_ADP_WaitVBlank, ret);
        return -EPERM;
    }

    osal_get_timeofday(&now);
    current_us = now.tv_sec * 1000000ll + now.tv_usec; /* 1000000ll: one second is 1000000us */
    vblank_info.time_val.tv_usec = current_us;
    if (osal_copy_to_user(argp, &vblank_info, sizeof(hifb_vblank_info))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, -EFAULT);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 drv_hifb_get_dma_buf(struct fb_info *info, hi_ulong arg)
{
    HIFB_PAR_S *par = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    hifb_dmabuf_export dmaexp = {0};

    if ((info == NULL) || (argp == NULL)) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d info or argp is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    par = (HIFB_PAR_S *)info->par;
    if (par == NULL) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d par is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    if (osal_copy_from_user(&dmaexp, argp, sizeof(hifb_dmabuf_export))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    if (IS_ERR(par->mem_info.dmabuf)) {
        HI_PRINT("[module-gfx-hifb][err] : %s %d dmabuf is error\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    dmaexp.mem_handle.mem_handle = osal_mem_create_fd(par->mem_info.dmabuf, dmaexp.flags);
    dmaexp.mem_handle.addr_offset = 0;

    if (osal_copy_to_user(argp, &dmaexp, sizeof(hifb_dmabuf_export))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

hi_s32 drv_hifb_compat_ioctl(struct fb_info *info, hi_u32 cmd, unsigned long arg)
{
    return drv_hifb_ioctl(info, cmd, arg);
}
