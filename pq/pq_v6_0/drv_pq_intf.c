/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq ioctl define
 * Author: pq
 * Create: 2016-01-1
 */

#include <linux/miscdevice.h>
#include <linux/module.h>
#include "drv_pq.h"
#include "hi_drv_pq.h"
#include "drv_pq_define.h"
#include "hi_drv_sys.h"
#include "hi_drv_dev.h"
#include "drv_pq_vpss_thread.h"
#include "drv_pq_vdp_thread.h"
#include "hi_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

static osal_semaphore g_st_pq_semaphore;

static struct miscdevice g_pq_dev;

static hi_s32 pq_open(struct inode *node, struct file *filp)
{
    return HI_SUCCESS;
}

static hi_s32 pq_close(struct inode *node, struct file *filp)
{
    return HI_SUCCESS;
}

#ifdef PQ_ALG_TOOLS
static hi_s32 u32_pqioctl_s_register(struct file *filp, hi_void *arg)
{
    hi_s32 ret;
    hi_pq_register *reg;

    reg = (hi_pq_register *)arg;
    ret = pq_tools_set_register(reg);

    return ret;
}

static hi_s32 u32_pqioctl_g_register(struct file *filp, hi_void *arg)
{
    hi_s32 ret;
    hi_pq_register *reg;

    reg = (hi_pq_register *)arg;
    ret = pq_tools_get_register(reg);

    return ret;
}

static hi_s32 u32_pqioctl_g_bin_phy_addr(struct file *filp, hi_void *arg)
{
    hi_s32 ret;
    hi_u64 *addr;

    addr = (hi_u64 *)arg;
    ret = pq_tools_get_bin_phy_addr(addr);

    return ret;
}
#endif

#ifdef PQ_ALG_DCI
static hi_s32 u32_pqioctl_s_dci(struct file *filp, hi_void *arg)
{
    return HI_SUCCESS;
}

static hi_s32 u32_pqioctl_g_dci(struct file *filp, hi_void *arg)
{
    return HI_SUCCESS;
}

static hi_s32 u32_pqioctl_s_dci_bs(struct file *filp, hi_void *arg)
{
    return HI_SUCCESS;
}

static hi_s32 u32_pqioctl_g_dci_bs(struct file *filp, hi_void *arg)
{
    return HI_SUCCESS;
}

static hi_s32 u32_pqioctl_g_dci_hist(struct file *filp, hi_void *arg)
{
    return HI_SUCCESS;
}
#endif

#if defined(CHIP_TYPE_HI3796CV300) || defined(CHIP_TYPE_HI3751V900)
#ifdef PQ_ALG_VPSSCSC
static hi_s32 u32_pqioctl_s_vp_preview_brightness(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 level;

    level = *(hi_u32 *)arg;
    ret = drv_pq_set_vpbrightness(HI_DRV_PQ_VP_TYPE_PREVIEW, level);

    return ret;
}
static hi_s32 u32_pqioctl_g_vp_preview_brightness(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 *level;

    level = (hi_u32 *)arg;
    ret = drv_pq_get_vpbrightness(HI_DRV_PQ_VP_TYPE_PREVIEW, level);

    return ret;
}
static hi_s32 u32_pqioctl_s_vp_preview_contrast(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 level;

    level = *(hi_u32 *)arg;
    ret = drv_pq_set_vpcontrast(HI_DRV_PQ_VP_TYPE_PREVIEW, level);

    return ret;
}
static hi_s32 u32_pqioctl_g_vp_preview_contrast(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 *level;

    level = (hi_u32 *)arg;
    ret = drv_pq_get_vpcontrast(HI_DRV_PQ_VP_TYPE_PREVIEW, level);

    return ret;
}
static hi_s32 u32_pqioctl_s_vp_preview_hue(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 level;

    level = *(hi_u32 *)arg;
    ret = drv_pq_set_vphue(HI_DRV_PQ_VP_TYPE_PREVIEW, level);

    return ret;
}
static hi_s32 u32_pqioctl_g_vp_preview_hue(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 *level;

    level = (hi_u32 *)arg;
    ret = drv_pq_get_vphue(HI_DRV_PQ_VP_TYPE_PREVIEW, level);

    return ret;
}
static hi_s32 u32_pqioctl_s_vp_preview_saturation(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 level;

    level = *(hi_u32 *)arg;
    ret = drv_pq_set_vpsaturation(HI_DRV_PQ_VP_TYPE_PREVIEW, level);

    return ret;
}
static hi_s32 u32_pqioctl_g_vp_preview_saturation(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 *level;

    level = (hi_u32 *)arg;
    ret = drv_pq_get_vpsaturation(HI_DRV_PQ_VP_TYPE_PREVIEW, level);

    return ret;
}
static hi_s32 u32_pqioctl_s_vp_remote_brightness(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 level;

    level = *(hi_u32 *)arg;
    ret = drv_pq_set_vpbrightness(HI_DRV_PQ_VP_TYPE_REMOTE, level);

    return ret;
}
static hi_s32 u32_pqioctl_g_vp_remote_brightness(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 *level;

    level = (hi_u32 *)arg;
    ret = drv_pq_get_vpbrightness(HI_DRV_PQ_VP_TYPE_REMOTE, level);

    return ret;
}
static hi_s32 u32_pqioctl_s_vp_remote_contrast(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 level;

    level = *(hi_u32 *)arg;
    ret = drv_pq_set_vpcontrast(HI_DRV_PQ_VP_TYPE_REMOTE, level);

    return ret;
}
static hi_s32 u32_pqioctl_g_vp_remote_contrast(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 *level;

    level = (hi_u32 *)arg;
    ret = drv_pq_get_vpcontrast(HI_DRV_PQ_VP_TYPE_REMOTE, level);

    return ret;
}
static hi_s32 u32_pqioctl_s_vp_remote_hue(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 level;

    level = *(hi_u32 *)arg;
    ret = drv_pq_set_vphue(HI_DRV_PQ_VP_TYPE_REMOTE, level);

    return ret;
}
static hi_s32 u32_pqioctl_g_vp_remote_hue(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 *level;

    level = (hi_u32 *)arg;
    ret = drv_pq_get_vphue(HI_DRV_PQ_VP_TYPE_REMOTE, level);

    return ret;
}
static hi_s32 u32_pqioctl_s_vp_remote_saturation(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 level;

    level = *(hi_u32 *)arg;
    ret = drv_pq_set_vpsaturation(HI_DRV_PQ_VP_TYPE_REMOTE, level);

    return ret;
}
static hi_s32 u32_pqioctl_g_vp_remote_saturation(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 *level;

    level = (hi_u32 *)arg;
    ret = drv_pq_get_vpsaturation(HI_DRV_PQ_VP_TYPE_REMOTE, level);

    return ret;
}
static hi_s32 u32_pqioctl_s_vp_preview_video_param(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_pq_image_param *attr;
    attr = (hi_pq_image_param *)arg;

    ret = drv_pq_set_vpvideo_setting(HI_DRV_PQ_VP_TYPE_REMOTE, attr);

    return ret;
}
static hi_s32 u32_pqioctl_g_vp_preview_video_param(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_pq_image_param *attr;
    attr = (hi_pq_image_param *)arg;

    ret = drv_pq_get_vpvideo_setting(HI_DRV_PQ_VP_TYPE_REMOTE, attr);

    return ret;
}
static hi_s32 u32_pqioctl_s_vp_remote_video_param(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_pq_image_param *attr;
    attr = (hi_pq_image_param *)arg;

    ret = drv_pq_set_vpvideo_setting(HI_DRV_PQ_VP_TYPE_REMOTE, attr);

    return ret;
}

static hi_s32 u32_pqioctl_g_vp_remote_video_param(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_pq_image_param *attr;
    attr = (hi_pq_image_param *)arg;

    ret = drv_pq_get_vpvideo_setting(HI_DRV_PQ_VP_TYPE_REMOTE, attr);

    return ret;
}
static hi_s32 u32_pqioctl_s_vp_preview_image_mode(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 mode;
    mode = *(hi_drv_pq_vp_mode *)arg;
    ret = drv_pq_set_vppreview_mode(mode);

    return ret;
}
static hi_s32 u32_pqioctl_g_vp_preview_image_mode(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_drv_pq_vp_mode *attr;
    attr = (hi_drv_pq_vp_mode *)arg;
    ret = drv_pq_get_vppreview_mode(attr);

    return ret;
}

static hi_s32 u32_pqioctl_s_vp_remote_image_mode(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 mode;
    mode = *(hi_drv_pq_vp_mode *)arg;
    ret = drv_pq_set_vpremote_mode(mode);

    return ret;
}
static hi_s32 u32_pqioctl_g_vp_remote_image_mode(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_drv_pq_vp_mode *attr;
    attr = (hi_drv_pq_vp_mode *)arg;
    ret = drv_pq_get_vpremote_mode(attr);

    return ret;
}
#endif
#endif

static hi_s32 u32_pqioctl_s_strength(struct file *filp, hi_void *arg)
{
    hi_s32 ret = HI_SUCCESS;
    hi_pq_module_strenght *module_strength;
    hi_u32 strength;

    module_strength = (hi_pq_module_strenght *)arg;
    strength = u1023_to_u100(module_strength->strength);

    if (module_strength->module == HI_PQ_MODULE_TNR) {
        ret = drv_pq_set_tnr_level(strength);
    } else if (module_strength->module == HI_PQ_MODULE_DB) {
        ret = drv_pq_set_db_level(strength);
    } else if (module_strength->module == HI_PQ_MODULE_DM) {
        ret = drv_pq_set_dm_level(strength);
    } else if (module_strength->module == HI_PQ_MODULE_SHARPNESS) {
        ret = drv_pq_set_sharpness(strength);
    } else if (module_strength->module == HI_PQ_MODULE_DCI) {
        ret = drv_pq_set_dci_level(strength);
    } else if (module_strength->module == HI_PQ_MODULE_DC) {
        ret = drv_pq_set_dc_level(strength);
    } else if (module_strength->module == HI_PQ_MODULE_ACM) {
        ret = drv_pq_set_acm_level(strength);
    } else if (module_strength->module == HI_PQ_MODULE_DR) {
        ret = drv_pq_set_dr_level(strength);
    } else if (module_strength->module == HI_PQ_MODULE_DS) {
        ret = drv_pq_set_ds_level(strength);
    } else if (module_strength->module == HI_PQ_MODULE_COCO) {
        ret = drv_pq_set_coco_level(strength);
    } else {
        HI_ERR_PQ("[%d] is not support to set strength!\n", module_strength->module);
        return HI_FAILURE;
    }

    return ret;
}

static hi_s32 u32_pqioctl_g_strength(struct file *filp, hi_void *arg)
{
    hi_s32 ret = HI_SUCCESS;
    hi_pq_module_strenght *module_strength;
    hi_u32 strength = 50;

    module_strength = (hi_pq_module_strenght *)arg;

    if (module_strength->module == HI_PQ_MODULE_TNR) {
        ret = drv_pq_get_tnr_level(&strength);
    } else if (module_strength->module == HI_PQ_MODULE_DB) {
        ret = drv_pq_get_db_level(&strength);
    } else if (module_strength->module == HI_PQ_MODULE_DM) {
        ret = drv_pq_get_dm_level(&strength);
    } else if (module_strength->module == HI_PQ_MODULE_SHARPNESS) {
        ret = drv_pq_get_sharpness(&strength);
    } else if (module_strength->module == HI_PQ_MODULE_DCI) {
        ret = drv_pq_get_dci_level(&strength);
    } else if (module_strength->module == HI_PQ_MODULE_DC) {
        ret = drv_pq_get_dc_level(&strength);
    } else if (module_strength->module == HI_PQ_MODULE_ACM) {
        ret = drv_pq_get_acm_level(&strength);
    } else if (module_strength->module == HI_PQ_MODULE_DR) {
        ret = drv_pq_get_dr_level(&strength);
    } else if (module_strength->module == HI_PQ_MODULE_DS) {
        ret = drv_pq_get_ds_level(&strength);
    } else if (module_strength->module == HI_PQ_MODULE_COCO) {
        ret = drv_pq_get_coco_level(&strength);
    } else {
        HI_ERR_PQ("[%d] is not support to get strength!\n", module_strength->module);
        return HI_FAILURE;
    }

    PQ_CHECK_RETURN_SUCCESS(ret);
    module_strength->strength = u100_to_u1023(strength);

    return HI_SUCCESS;
}

static hi_s32 u32_pqioctl_s_fleshtone(struct file *filp, hi_void *arg)
{
    hi_s32 ret;
    hi_u32 fleshtone;

    fleshtone = *(hi_drv_pq_vp_mode *)arg;
    ret = drv_pq_set_flesh_tone_level(fleshtone);

    return ret;
}

static hi_s32 u32_pqioctl_g_fleshtone(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 *level;

    level = (hi_u32 *)arg;
    ret = drv_pq_get_flesh_tone_level(level);

    return ret;
}

static hi_s32 u32_pqioctl_s_six_base_color(struct file *filp, hi_void *arg)
{
    hi_s32 ret;
    hi_pq_six_base_color six_base_color;
    hi_pq_six_base_color *attr;

    attr = (hi_pq_six_base_color *)arg;
    six_base_color.red = u1023_to_u100(attr->red);
    six_base_color.green = u1023_to_u100(attr->green);
    six_base_color.blue = u1023_to_u100(attr->blue);
    six_base_color.cyan = u1023_to_u100(attr->cyan);
    six_base_color.magenta = u1023_to_u100(attr->magenta);
    six_base_color.yellow = u1023_to_u100(attr->yellow);

    ret = drv_pq_set_six_base_color(&six_base_color);

    return ret;
}

static hi_s32 u32_pqioctl_g_six_base_color(struct file *filp, hi_void *arg)
{
    hi_s32 ret;
    hi_pq_six_base_color six_base_color;
    hi_pq_six_base_color *attr;
    attr = (hi_pq_six_base_color *)arg;

    ret = drv_pq_get_six_base_color(&six_base_color);
    PQ_CHECK_RETURN_SUCCESS(ret);

    attr->red = u100_to_u1023(six_base_color.red);
    attr->green = u100_to_u1023(six_base_color.green);
    attr->blue = u100_to_u1023(six_base_color.blue);
    attr->cyan = u100_to_u1023(six_base_color.cyan);
    attr->magenta = u100_to_u1023(six_base_color.magenta);
    attr->yellow = u100_to_u1023(six_base_color.yellow);

    return ret;
}

static hi_s32 u32_pqioctl_s_color_enhance_mode(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 level;

    level = *(hi_u32 *)arg;
    ret = drv_pq_set_color_enhance_mode(level);

    return ret;
}
static hi_s32 u32_pqioctl_g_color_enhance_mode(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 *level;

    level = (hi_u32 *)arg;
    ret = drv_pq_get_color_enhance_mode(level);

    return ret;
}

static hi_s32 u32_pqioctl_s_module(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_pq_module_enable *attr;
    attr = (hi_pq_module_enable *)arg;

    ret = drv_pq_set_pq_module(attr->module, attr->enable);

    return ret;
}
static hi_s32 u32_pqioctl_g_module(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_pq_module_enable *attr;
    attr = (hi_pq_module_enable *)arg;

    ret = drv_pq_get_pq_module(attr->module, &(attr->enable));

    return ret;
}
static hi_s32 u32_pqioctl_s_demo(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_pq_demo_enable *attr;
    attr = (hi_pq_demo_enable *)arg;

    ret = drv_pq_set_demo_en(attr->module, attr->enable);

    return ret;
}

static hi_s32 u32_pqioctl_g_demo(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_pq_demo_enable *attr;
    attr = (hi_pq_demo_enable *)arg;

    ret = drv_pq_get_demo_en(attr->module, &(attr->enable));

    return ret;
}

static hi_s32 u32_pqioctl_s_hdroffset_param(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_pq_hdr_offset *attr;
    attr = (hi_pq_hdr_offset *)arg;

    ret = drv_pq_set_hdr_offset_para(attr);

    return ret;
}

static hi_s32 u32_pqioctl_s_gfxhdroffset_param(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_pq_hdr_offset *attr;
    attr = (hi_pq_hdr_offset *)arg;

    ret = drv_pq_set_gfx_hdr_offset_para(attr);

    return ret;
}

static hi_s32 u32_pqioctl_s_graph_sd_param(struct file *filp, hi_void *arg)
{
    hi_s32 ret;
    hi_pq_image_param param = {0};
    hi_pq_image_param *attr;

    attr = (hi_pq_image_param *)arg;
    param.brightness = u1023_to_u100(attr->brightness);
    param.contrast = u1023_to_u100(attr->contrast);
    param.hue = u1023_to_u100(attr->hue);
    param.saturation = u1023_to_u100(attr->saturation);
    param.wcg_temperature = u1023_to_u100(attr->wcg_temperature);
    ret = drv_pq_set_sd_picture_setting(&param);

    return ret;
}

static hi_s32 u32_pqioctl_g_graph_sd_param(struct file *filp, hi_void *arg)
{
    hi_s32 ret;
    hi_pq_image_param param = {0};
    hi_pq_image_param *attr;
    attr = (hi_pq_image_param *)arg;

    ret = drv_pq_get_sd_picture_setting(&param);
    PQ_CHECK_RETURN_SUCCESS(ret);

    attr->brightness = u100_to_u1023(param.brightness);
    attr->contrast = u100_to_u1023(param.contrast);
    attr->hue = u100_to_u1023(param.hue);
    attr->saturation = u100_to_u1023(param.saturation);
    attr->wcg_temperature = u100_to_u1023(param.wcg_temperature);

    return ret;
}

static hi_s32 u32_pqioctl_s_graph_hd_param(struct file *filp, hi_void *arg)
{
    hi_s32 ret;
    hi_pq_image_param param = {0};
    hi_pq_image_param *attr;
    attr = (hi_pq_image_param *)arg;

    param.brightness = u1023_to_u100(attr->brightness);
    param.contrast = u1023_to_u100(attr->contrast);
    param.hue = u1023_to_u100(attr->hue);
    param.saturation = u1023_to_u100(attr->saturation);
    param.wcg_temperature = u1023_to_u100(attr->wcg_temperature);
    ret = drv_pq_set_hd_picture_setting(&param);

    return ret;
}

static hi_s32 u32_pqioctl_g_graph_hd_param(struct file *filp, hi_void *arg)
{
    hi_s32 ret;
    hi_pq_image_param *attr = HI_NULL;
    hi_pq_image_param param = {0};
    attr = (hi_pq_image_param *)arg;

    ret = drv_pq_get_hd_picture_setting(&param);
    PQ_CHECK_RETURN_SUCCESS(ret);

    attr->brightness = u100_to_u1023(param.brightness);
    attr->contrast = u100_to_u1023(param.contrast);
    attr->hue = u100_to_u1023(param.hue);
    attr->saturation = u100_to_u1023(param.saturation);
    attr->wcg_temperature = u100_to_u1023(param.wcg_temperature);

    return ret;
}

static hi_s32 u32_pqioctl_s_video_sd_param(struct file *filp, hi_void *arg)
{
    hi_s32 ret;
    hi_pq_image_param param = {0};
    hi_pq_image_param *attr;
    attr = (hi_pq_image_param *)arg;

    param.brightness = u1023_to_u100(attr->brightness);
    param.contrast = u1023_to_u100(attr->contrast);
    param.hue = u1023_to_u100(attr->hue);
    param.saturation = u1023_to_u100(attr->saturation);
    param.wcg_temperature = u1023_to_u100(attr->wcg_temperature);
    ret = drv_pq_set_sd_video_setting(&param);

    return ret;
}

static hi_s32 u32_pqioctl_g_video_sd_param(struct file *filp, hi_void *arg)
{
    hi_s32 ret;
    hi_pq_image_param param = {0};
    hi_pq_image_param *attr;
    attr = (hi_pq_image_param *)arg;

    ret = drv_pq_get_sd_video_setting(&param);
    PQ_CHECK_RETURN_SUCCESS(ret);

    attr->brightness = u100_to_u1023(param.brightness);
    attr->contrast = u100_to_u1023(param.contrast);
    attr->hue = u100_to_u1023(param.hue);
    attr->saturation = u100_to_u1023(param.saturation);
    attr->wcg_temperature = u100_to_u1023(param.wcg_temperature);

    return ret;
}
static hi_s32 u32_pqioctl_s_video_hd_param(struct file *filp, hi_void *arg)
{
    hi_s32 ret;
    hi_pq_image_param param = {0};
    hi_pq_image_param *attr;
    attr = (hi_pq_image_param *)arg;

    param.brightness = u1023_to_u100(attr->brightness);
    param.contrast = u1023_to_u100(attr->contrast);
    param.hue = u1023_to_u100(attr->hue);
    param.saturation = u1023_to_u100(attr->saturation);
    param.wcg_temperature = u1023_to_u100(attr->wcg_temperature);
    ret = drv_pq_set_hd_video_setting(&param);

    return ret;
}
static hi_s32 u32_pqioctl_g_video_hd_param(struct file *filp, hi_void *arg)
{
    hi_s32 ret;
    hi_pq_image_param param = {0};
    hi_pq_image_param *attr;
    attr = (hi_pq_image_param *)arg;

    ret = drv_pq_get_hd_video_setting(&param);
    PQ_CHECK_RETURN_SUCCESS(ret);

    attr->brightness = u100_to_u1023(param.brightness);
    attr->contrast = u100_to_u1023(param.contrast);
    attr->hue = u100_to_u1023(param.hue);
    attr->saturation = u100_to_u1023(param.saturation);
    attr->wcg_temperature = u100_to_u1023(param.wcg_temperature);

    return ret;
}

static hi_s32 u32_pqioctl_s_sd_temperature(struct file *filp, hi_void *arg)
{
    hi_s32 ret;
    hi_pq_color_temperature color_temperature;
    hi_pq_color_temperature *attr;

    attr = (hi_pq_color_temperature *)arg;
    color_temperature.red_gain = u1023_to_u100(attr->red_gain);
    color_temperature.green_gain = u1023_to_u100(attr->green_gain);
    color_temperature.blue_gain = u1023_to_u100(attr->blue_gain);
    color_temperature.red_offset = u1023_to_u100(attr->red_offset);
    color_temperature.green_offset = u1023_to_u100(attr->green_offset);
    color_temperature.blue_offset = u1023_to_u100(attr->blue_offset);

    ret = drv_pq_set_temperature(HI_PQ_DISPLAY_0, &color_temperature);

    return ret;
}

static hi_s32 u32_pqioctl_g_sd_temperature(struct file *filp, hi_void *arg)
{
    hi_s32 ret;
    hi_pq_color_temperature color_temperature;
    hi_pq_color_temperature *attr;
    attr = (hi_pq_color_temperature *)arg;

    ret = drv_pq_get_temperature(HI_PQ_DISPLAY_0, &color_temperature);
    PQ_CHECK_RETURN_SUCCESS(ret);

    attr->red_gain = u100_to_u1023(color_temperature.red_gain);
    attr->green_gain = u100_to_u1023(color_temperature.green_gain);
    attr->blue_gain = u100_to_u1023(color_temperature.blue_gain);
    attr->red_offset = u100_to_u1023(color_temperature.red_offset);
    attr->green_offset = u100_to_u1023(color_temperature.green_offset);
    attr->blue_offset = u100_to_u1023(color_temperature.blue_offset);

    return ret;
}

static hi_s32 u32_pqioctl_s_hd_temperature(struct file *filp, hi_void *arg)
{
    hi_s32 ret;
    hi_pq_color_temperature color_temperature;
    hi_pq_color_temperature *attr;

    attr = (hi_pq_color_temperature *)arg;
    color_temperature.red_gain = u1023_to_u100(attr->red_gain);
    color_temperature.green_gain = u1023_to_u100(attr->green_gain);
    color_temperature.blue_gain = u1023_to_u100(attr->blue_gain);
    color_temperature.red_offset = u1023_to_u100(attr->red_offset);
    color_temperature.green_offset = u1023_to_u100(attr->green_offset);
    color_temperature.blue_offset = u1023_to_u100(attr->blue_offset);
    ret = drv_pq_set_temperature(HI_PQ_DISPLAY_1, &color_temperature);

    return ret;
}

static hi_s32 u32_pqioctl_g_hd_temperature(struct file *filp, hi_void *arg)
{
    hi_s32 ret;
    hi_pq_color_temperature color_temperature;
    hi_pq_color_temperature *attr;
    attr = (hi_pq_color_temperature *)arg;

    ret = drv_pq_get_temperature(HI_PQ_DISPLAY_1, &color_temperature);
    PQ_CHECK_RETURN_SUCCESS(ret);

    attr->red_gain = u100_to_u1023(color_temperature.red_gain);
    attr->green_gain = u100_to_u1023(color_temperature.green_gain);
    attr->blue_gain = u100_to_u1023(color_temperature.blue_gain);
    attr->red_offset = u100_to_u1023(color_temperature.red_offset);
    attr->green_offset = u100_to_u1023(color_temperature.green_offset);
    attr->blue_offset = u100_to_u1023(color_temperature.blue_offset);

    return ret;
}

#ifdef PQ_ALG_HDR
#if defined(PQ_HDR_TM_512_SUPPORT)
static hi_s32 u32_pqioctl_s_hdr_tmlut(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_pq_hdr_tm_lut *attr;
    attr = (hi_pq_hdr_tm_lut *)arg;

    ret = pq_mng_set_hdr_tm_lut(attr);

    return ret;
}
static hi_s32 u32_pqioctl_g_hdr_tmlut(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_pq_hdr_tm_lut *attr;

    attr = (hi_pq_hdr_tm_lut *)arg;
    ret = pq_mng_get_hdr_tm_lut(attr);

    return ret;
}
#endif

#if defined(PQ_HDR_TM_64_SUPPORT)
static hi_s32 u32_pqioctl_s_hdr_mode(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_pq_hdr_para_mode *mode;
    mode = (hi_pq_hdr_para_mode *)arg;
    ret = pq_mng_tool_set_hdrpara_mode(mode);

    return ret;
}

static hi_s32 u32_pqioctl_g_hdr_mode(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_pq_hdr_para_mode *mode;
    mode = (hi_pq_hdr_para_mode *)arg;
    ret = pq_mng_tool_get_hdrpara_mode(mode);

    return ret;
}

static hi_s32 u32_pqioctl_s_hdr_tmap(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_pq_hdr_tmap *attr;
    attr = (hi_pq_hdr_tmap *)arg;
    ret = pq_mng_tool_set_hdr_tmap(attr);
    return ret;
}

static hi_s32 u32_pqioctl_g_hdr_tmap(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_pq_hdr_tmap *attr;
    attr = (hi_pq_hdr_tmap *)arg;
    ret = pq_mng_tool_get_hdr_tmap(attr);

    return ret;
}
#endif

#if defined(PQ_HDR_SM_64_SUPPORT)
static hi_s32 u32_pqioctl_s_hdr_smap(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_pq_hdr_smap *attr;
    attr = (hi_pq_hdr_smap *)arg;
    ret = pq_mng_tool_set_hdr_smap(attr);

    return ret;
}

static hi_s32 u32_pqioctl_g_hdr_smap(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_pq_hdr_smap *attr;
    attr = (hi_pq_hdr_smap *)arg;
    ret = pq_mng_tool_get_hdr_smap(attr);

    return ret;
}
#endif
#endif

#ifdef PQ_ALG_LCACM
#if defined(PQ_HDR_ACM_SUPPORT)
static hi_s32 u32_pqioctl_s_hdr_acm_mode(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_pq_hdr_acm_mode *mode;
    mode = (hi_pq_hdr_acm_mode *)arg;
    ret = pq_mng_tool_set_hdr_acm_mode(mode);

    return ret;
}

static hi_s32 u32_pqioctl_g_hdr_acm_mode(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_pq_hdr_acm_mode *mode;
    mode = (hi_pq_hdr_acm_mode *)arg;
    ret = pq_mng_tool_get_hdr_acm_mode(mode);

    return ret;
}

static hi_s32 u32_pqioctl_s_hdr_acm_regcfg(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_pq_hdr_acm_regcfg *attr;
    attr = (hi_pq_hdr_acm_regcfg *)arg;
    ret = pq_mng_tool_set_hdr_acm_reg_cfg(attr);
    return ret;
}

static hi_s32 u32_pqioctl_g_hdr_acm_regcfg(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_pq_hdr_acm_regcfg *attr;
    attr = (hi_pq_hdr_acm_regcfg *)arg;
    ret = pq_mng_tool_get_hdr_acm_reg_cfg(attr);

    return ret;
}
#endif
#endif

#ifdef PQ_ALG_DCI
#if defined(PQ_DCI_27_SUPPORT)
static hi_s32 u32_pqioctl_s_hdr_dyn_mode(struct file *filp, hi_void *arg)
{
    return HI_SUCCESS;
}

static hi_s32 u32_pqioctl_g_hdr_dyn_mode(struct file *filp, hi_void *arg)
{
    return HI_SUCCESS;
}
#endif
#endif

static hi_s32 u32_pqioctl_s_image_mode(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 mode;
    mode = *(hi_pq_image_mode *)arg;
    ret = drv_pq_set_image_mode(mode);

    return ret;
}
static hi_s32 u32_pqioctl_g_image_mode(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_pq_image_mode *attr;
    attr = (hi_pq_image_mode *)arg;
    ret = drv_pq_get_image_mode(attr);

    return ret;
}

static hi_s32 u32_pqioctl_s_demo_mode(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 level;

    level = *(hi_u32 *)arg;
    ret = drv_pq_set_demo_disp_mode(REG_TYPE_VPSS, level);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = drv_pq_set_demo_disp_mode(REG_TYPE_VDP, level);
    PQ_CHECK_RETURN_SUCCESS(ret);

    return ret;
}
static hi_s32 u32_pqioctl_g_demo_mode(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 *level;

    level = (hi_u32 *)arg;
    ret = drv_pq_get_demo_disp_mode(level);

    return ret;
}
static hi_s32 u32_pqioctl_s_demo_coordinate(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_u32 coordinate;

    coordinate = u1023_to_u100(*(hi_u32 *)arg);
    ret = drv_pq_set_demo_coordinate(REG_TYPE_VPSS, coordinate);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = drv_pq_set_demo_coordinate(REG_TYPE_VDP, coordinate);
    PQ_CHECK_RETURN_SUCCESS(ret);

    return ret;
}
static hi_s32 u32_pqioctl_g_demo_coordinate(struct file *filp, hi_void *arg)
{
    hi_s32 ret;
    hi_u32 coordinate;

    ret = drv_pq_get_demo_coordinate(&coordinate);
    PQ_CHECK_RETURN_SUCCESS(ret);
    *(hi_u32 *)arg = u100_to_u1023(coordinate);

    return ret;
}

static hi_s32 u32_pqioctl_s_default_param(struct file *filp, hi_void *arg)
{
    hi_s32 ret;

    hi_bool default_code;

    default_code = *(hi_bool *)arg;
    ret = hi_drv_pq_set_default_param(default_code);

    return ret;
}

static hi_s32 pqioctl_g_vpss_thread_info(struct file *filp, hi_void *arg)
{
    hi_drv_pq_vpss_thread_info *vpss_info = (hi_drv_pq_vpss_thread_info *)arg;
    return drv_pq_get_vpss_thread_info(vpss_info);
}

static hi_s32 pqioctl_g_vpss_alg_thread_status(struct file *filp, hi_void *arg)
{
    hi_bool *status = (hi_bool *)arg;
    return drv_pq_get_alg_thread_status(status);
}

static hi_s32 pqioctl_s_vpss_alg_thread_status(struct file *filp, hi_void *arg)
{
    hi_bool status = *((hi_bool *)arg);
    return drv_pq_set_alg_thread_status(status);
}

static hi_s32 pqioctl_g_vdp_thread_info(struct file *filp, hi_void *arg)
{
    hi_drv_pq_vdp_thread_info *vdp_info = (hi_drv_pq_vdp_thread_info *)arg;
    return drv_pq_get_vdp_thread_info(vdp_info);
}

static hi_s32 pqioctl_g_vdp_alg_thread_status(struct file *filp, hi_void *arg)
{
    hi_bool *status = (hi_bool *)arg;
    return drv_pq_get_vdp_alg_thread_status(status);
}

static hi_s32 pqioctl_s_vdp_alg_thread_status(struct file *filp, hi_void *arg)
{
    hi_bool status = *((hi_bool *)arg);
    return drv_pq_set_vdp_alg_thread_status(status);
}

/* PQ 命令实现结构 */
typedef struct {
    unsigned int cmd;
    hi_s32 (*pf_ioctrl_cmd_function)(struct file *filp, hi_void *arg); /* PQ CMD对应的实现函数 */
} pq_ioctrl_cmd_fun;

/* PQ IOCTRL CMD对应实现函数结构 */
pq_ioctrl_cmd_fun g_pq_ioctrl_cmd_function[] = {
#ifdef PQ_ALG_TOOLS
    { HIIOC_PQ_S_REGISTER, u32_pqioctl_s_register },
    { HIIOC_PQ_G_REGISTER, u32_pqioctl_g_register },
    { HIIOC_PQ_G_BIN_ADDR, u32_pqioctl_g_bin_phy_addr },
#endif

#ifdef PQ_ALG_DCI
    { HIIOC_PQ_S_DCI,      u32_pqioctl_s_dci },
    { HIIOC_PQ_G_DCI,      u32_pqioctl_g_dci },
    { HIIOC_PQ_S_DCI_BS,   u32_pqioctl_s_dci_bs },
    { HIIOC_PQ_G_DCI_BS,   u32_pqioctl_g_dci_bs },
    { HIIOC_PQ_G_DCI_HIST, u32_pqioctl_g_dci_hist },
#endif
#if defined(CHIP_TYPE_HI3796CV300) || defined(CHIP_TYPE_HI3751V900)
#ifdef PQ_ALG_VPSSCSC
    { HIIOC_PQ_S_VP_PREVIEW_IMAGE_MODE, u32_pqioctl_s_vp_preview_image_mode },
    { HIIOC_PQ_G_VP_PREVIEW_IMAGE_MODE, u32_pqioctl_g_vp_preview_image_mode },
    { HIIOC_PQ_S_VP_REMOTE_IMAGE_MODE,  u32_pqioctl_s_vp_remote_image_mode },
    { HIIOC_PQ_G_VP_REMOTE_IMAGE_MODE,  u32_pqioctl_g_vp_remote_image_mode },
#endif
#endif
    { HIIOC_PQ_S_STRENGTH,                 u32_pqioctl_s_strength },
    { HIIOC_PQ_G_STRENGTH,                 u32_pqioctl_g_strength },

    { HIIOC_PQ_S_FLESHTONE,           u32_pqioctl_s_fleshtone },
    { HIIOC_PQ_G_FLESHTONE,           u32_pqioctl_g_fleshtone },
    { HIIOC_PQ_S_SIXBASECOLOR,        u32_pqioctl_s_six_base_color },
    { HIIOC_PQ_G_SIXBASECOLOR,        u32_pqioctl_g_six_base_color },
    { HIIOC_PQ_S_COLOR_ENHANCE_MODE,  u32_pqioctl_s_color_enhance_mode },
    { HIIOC_PQ_G_COLOR_ENHANCE_MODE,  u32_pqioctl_g_color_enhance_mode },
    { HIIOC_PQ_S_MODULE,              u32_pqioctl_s_module },
    { HIIOC_PQ_G_MODULE,              u32_pqioctl_g_module },
    { HIIOC_PQ_S_DEMO,                u32_pqioctl_s_demo },
    { HIIOC_PQ_G_DEMO,                u32_pqioctl_g_demo },
    { HIIOC_PQ_S_GRAPH_SD_PARAM,      u32_pqioctl_s_graph_sd_param },
    { HIIOC_PQ_G_GRAPH_SD_PARAM,      u32_pqioctl_g_graph_sd_param },
    { HIIOC_PQ_S_GRAPH_HD_PARAM,      u32_pqioctl_s_graph_hd_param },
    { HIIOC_PQ_G_GRAPH_HD_PARAM,      u32_pqioctl_g_graph_hd_param },
    { HIIOC_PQ_S_VIDEO_SD_PARAM,      u32_pqioctl_s_video_sd_param },
    { HIIOC_PQ_G_VIDEO_SD_PARAM,      u32_pqioctl_g_video_sd_param },
    { HIIOC_PQ_S_VIDEO_HD_PARAM,      u32_pqioctl_s_video_hd_param },
    { HIIOC_PQ_G_VIDEO_HD_PARAM,      u32_pqioctl_g_video_hd_param },
    { HIIOC_PQ_S_SD_TEMPERATURE,      u32_pqioctl_s_sd_temperature },
    { HIIOC_PQ_G_SD_TEMPERATURE,      u32_pqioctl_g_sd_temperature },
    { HIIOC_PQ_S_HD_TEMPERATURE,      u32_pqioctl_s_hd_temperature },
    { HIIOC_PQ_G_HD_TEMPERATURE,      u32_pqioctl_g_hd_temperature },
    { HIIOC_PQ_S_HDROFFSET_PARAM,     u32_pqioctl_s_hdroffset_param },
    { HIIOC_PQ_S_GFX_HDROFFSET_PARAM, u32_pqioctl_s_gfxhdroffset_param },
#ifdef PQ_ALG_HDR
#if defined(PQ_HDR_TM_512_SUPPORT)
    { HIIOC_PQ_S_HDR_TMLUT, u32_pqioctl_s_hdr_tmlut },
    { HIIOC_PQ_G_HDR_TMLUT, u32_pqioctl_g_hdr_tmlut },
#endif
#if defined(PQ_HDR_TM_64_SUPPORT)
    { HIIOC_PQ_S_HDR_PARA_MODE, u32_pqioctl_s_hdr_mode },
    { HIIOC_PQ_G_HDR_PARA_MODE, u32_pqioctl_g_hdr_mode },
    { HIIOC_PQ_S_HDR_TMAP,      u32_pqioctl_s_hdr_tmap },
    { HIIOC_PQ_G_HDR_TMAP,      u32_pqioctl_g_hdr_tmap },
#endif

#if defined(PQ_HDR_SM_64_SUPPORT)
    { HIIOC_PQ_S_HDR_SMAP, u32_pqioctl_s_hdr_smap },
    { HIIOC_PQ_G_HDR_SMAP, u32_pqioctl_g_hdr_smap },
#endif
#endif

#ifdef PQ_ALG_LCACM
#if defined(PQ_HDR_ACM_SUPPORT)
    { HIIOC_PQ_S_HDR_ACM_MODE,   u32_pqioctl_s_hdr_acm_mode },
    { HIIOC_PQ_G_HDR_ACM_MODE,   u32_pqioctl_g_hdr_acm_mode },
    { HIIOC_PQ_S_HDR_ACM_REGCFG, u32_pqioctl_s_hdr_acm_regcfg },
    { HIIOC_PQ_G_HDR_ACM_REGCFG, u32_pqioctl_g_hdr_acm_regcfg },
#endif
#endif
#ifdef PQ_ALG_DCI
#if defined(PQ_DCI_27_SUPPORT)
    { HIIOC_PQ_S_HDR_DYN_MODE, u32_pqioctl_s_hdr_dyn_mode },
    { HIIOC_PQ_G_HDR_DYN_MODE, u32_pqioctl_g_hdr_dyn_mode },
#endif
#endif
    { HIIOC_PQ_S_IMAGE_MODE,      u32_pqioctl_s_image_mode },
    { HIIOC_PQ_G_IMAGE_MODE,      u32_pqioctl_g_image_mode },
    { HIIOC_PQ_S_DEMO_MODE,       u32_pqioctl_s_demo_mode },
    { HIIOC_PQ_G_DEMO_MODE,       u32_pqioctl_g_demo_mode },
    { HIIOC_PQ_S_DEMO_COORDINATE, u32_pqioctl_s_demo_coordinate },
    { HIIOC_PQ_G_DEMO_COORDINATE, u32_pqioctl_g_demo_coordinate },
    { HIIOC_PQ_S_DEFAULT_PARAM,   u32_pqioctl_s_default_param },

    { HIIOC_PQ_G_VPSS_INFO,         pqioctl_g_vpss_thread_info },
    { HIIOC_PQ_G_APITHREAD_STATUS,  pqioctl_g_vpss_alg_thread_status},
    { HIIOC_PQ_S_APITHREAD_STATUS,  pqioctl_s_vpss_alg_thread_status},

    { HIIOC_PQ_G_VDP_INFO,         pqioctl_g_vdp_thread_info },
    { HIIOC_PQ_G_VDP_APITHREAD_STATUS,  pqioctl_g_vdp_alg_thread_status},
    { HIIOC_PQ_S_VDP_APITHREAD_STATUS,  pqioctl_s_vdp_alg_thread_status},
};

hi_u32 pq_drv_poll_common(struct file *filp, struct poll_table_struct *wait)
{
    return drv_pq_vdp_poll(filp, wait)|drv_pq_vpss_poll(filp, wait);
}

static hi_slong pq_ioctl(struct file *filp, unsigned int cmd, hi_void *arg)
{
    hi_s32 ret;
    hi_s32 ioctrl_cmd_idex = 0;
    hi_s32 ioctrl_cmd_func_total;
    hi_bool match = HI_FALSE;
    hi_bool flag = 0;

    if ((arg == HI_NULL) || (filp == HI_NULL)) {
        HI_ERR_PQ("pq_ioctl arg NULL \n");
        return -ENOIOCTLCMD;
    }

    drv_pq_get_init_flag(&flag);
    PQ_CHECK_INIT_RE_FAIL(flag);

    ioctrl_cmd_func_total = sizeof(g_pq_ioctrl_cmd_function) / sizeof(pq_ioctrl_cmd_fun);
    for (ioctrl_cmd_idex = 0; ioctrl_cmd_idex < ioctrl_cmd_func_total; ioctrl_cmd_idex++) {
        if (cmd == g_pq_ioctrl_cmd_function[ioctrl_cmd_idex].cmd) {
            break;
        }
    }

    if (ioctrl_cmd_idex >= ioctrl_cmd_func_total) {
        HI_ERR_PQ("no such IOCTL command: %x\n", cmd);
        return -ENOIOCTLCMD;
    }

    if (HI_NULL != g_pq_ioctrl_cmd_function[ioctrl_cmd_idex].pf_ioctrl_cmd_function) {
        match = HI_TRUE;
        ret = osal_sem_down_interruptible(&g_st_pq_semaphore);
        PQ_CHECK_RETURN_SUCCESS(ret);

        ret = g_pq_ioctrl_cmd_function[ioctrl_cmd_idex].pf_ioctrl_cmd_function(filp, arg);
        osal_sem_up(&g_st_pq_semaphore);
        if (ret != HI_SUCCESS) {
            match = HI_FALSE;
            HI_ERR_PQ("st_pq_ioctrl_cmd_function failed! [%i]\n", ret);
        }
    } else {
        match = HI_FALSE;
        HI_ERR_PQ("st_pq_ioctrl_cmd_function is NULL! \n");
    }

    if (match == HI_FALSE) {
        HI_ERR_PQ("no such IOCTL command: %x\n", cmd);
        return -ENOIOCTLCMD;
    }

    return ret;
}

long pq_ioctl_intf(struct file *filp, unsigned int cmd, unsigned long arg)
{
    if (filp == HI_NULL) {
        HI_ERR_PQ("filp valid\n");
        return -EFAULT;
    }

    if (filp->f_path.dentry == HI_NULL) {
        HI_ERR_PQ("filp valid\n");
        return -EFAULT;
    }

    return (long)hi_drv_user_copy(filp, cmd, arg, pq_ioctl);
}

static struct file_operations g_pq_drv_ops = {
    .open = pq_open,
    .unlocked_ioctl = pq_ioctl_intf,
    .compat_ioctl = pq_ioctl_intf,
    .release = pq_close,
    .poll = pq_drv_poll_common,
};

hi_s32 pq_drv_mod_init(hi_void)
{
    hi_s32 ret;

    g_pq_dev.name = HI_DEV_PQ_NAME;
    g_pq_dev.minor = MISC_DYNAMIC_MINOR;
    g_pq_dev.fops = &g_pq_drv_ops;

    ret = misc_register(&g_pq_dev);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("PQ device register fail!\n");
        return ret;
    }

    ret = drv_pq_create_proc();
    if (ret != HI_SUCCESS) {
        misc_deregister(&g_pq_dev);
        HI_ERR_PQ("PQ proc create fail!\n");
        return ret;
    }

    ret = osal_sem_init(&g_st_pq_semaphore, 1);
    if (ret != HI_SUCCESS) {
        misc_deregister(&g_pq_dev);
        drv_pq_remove_proc();
        HI_ERR_PQ("PQ osal_sem_init fail!\n");
        return ret;
    }

#ifndef HI_MCE_SUPPORT
    ret = hi_drv_pq_init();
    if (ret != HI_SUCCESS) {
        misc_deregister(&g_pq_dev);
        osal_sem_destory(&g_st_pq_semaphore);
        drv_pq_remove_proc();
        HI_ERR_PQ("PQ hal init fail!\n");
        return ret;
    }
#endif

#ifdef MODULE
    HI_PRINT("Load hi_pq.ko success.\t\t(%s)\n", VERSION_STRING);
#endif

    return HI_SUCCESS;
}

hi_void pq_drv_module_exit(hi_void)
{
#ifndef HI_MCE_SUPPORT
    hi_drv_pq_deinit();
#endif

    osal_sem_destory(&g_st_pq_semaphore);
    drv_pq_remove_proc();
    misc_deregister(&g_pq_dev);

#ifdef MODULE
    HI_PRINT("remove hi_pq.ko success.     \t(%s)\n", VERSION_STRING);
#endif
}

#ifdef MODULE
module_init(pq_drv_mod_init);
module_exit(pq_drv_module_exit);
#else
EXPORT_SYMBOL(pq_drv_mod_init);
EXPORT_SYMBOL(pq_drv_module_exit);
#endif

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

