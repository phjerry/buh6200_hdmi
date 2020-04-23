/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ioctl operation
 * Author: sdk
 * Create: 2019-05-17
 */

/* add include here */
#include <linux/uaccess.h>
#include "hi_osal.h"

#include "drv_gfx2d_device.h"

#include "hi_gfx_comm_k.h"
#include "hi_gfx_sys_k.h"
#include "drv_hifb_ext.h"

#include "drv_gfx2d_struct.h"
#include "drv_gfx2d_errcode.h"

#include "drv_gfx2d_debug.h"
#include "drv_ioctl_gfx2d.h"
#include "drv_gfx2d_ctl.h"
#include <linux/hisilicon/securec.h>

/* Macro Definition */
#define GFX2D_IOCTL_FUNC_ITEM_NUM_MAX 3
typedef hi_s32 (*drv_gfx2d_ioctl_func)(hi_void *arg);

/* Structure Definition */
typedef struct {
    hi_bool work_sync;
    hi_u32 time_out;
    drv_gfx2d_dev_id dev_id;
    drv_gfx2d_compose_surface *dest_surface;
    drv_gfx2d_compose_list compose_list;
} GFX2D_COMPOSE_CMD_HAL_S;

typedef struct {
    hi_u32 cmd;
    drv_gfx2d_ioctl_func ioctl_func;
} gfx2d_ioctl_func_item;

/* Global Variable declaration */
static hi_void gfx2d_print_input_info(GFX2D_COMPOSE_CMD_HAL_S *compose, hi_u32 fence_fd, hi_bool is_input);

/* API forward declarations */
static hi_s32 drv_gfx2d_compose(hi_void *arg);
static hi_s32 drv_gfx2d_wait_done(hi_void *arg);

static gfx2d_ioctl_func_item g_gfx2d_ioctl_func[GFX2D_IOCTL_FUNC_ITEM_NUM_MAX] = {
    { DRV_GFX2D_CMD_COMPOSE, drv_gfx2d_compose },
    { DRV_GFX2D_CMD_WAIT_DONE, drv_gfx2d_wait_done },
    { GFX2D_IOCTL_FUNC_ITEM_NUM_MAX - 1, NULL },
};

/* API realization */
static hi_slong gfx2d_ioctl(struct file *file, unsigned int cmd, hi_void *arg)
{
    hi_slong ret;
    hi_u8 gfx2d_cmd = _IOC_NR(cmd);
    hi_u32 start_times = 0;
    hi_u32 end_times = 0;
    hi_u32 cost_times;
    HI_GFX_TINIT();

    HI_GFX_TSTART(start_times);

    if (gfx2d_cmd >= GFX2D_IOCTL_FUNC_ITEM_NUM_MAX) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, gfx2d_cmd);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, GFX2D_IOCTL_FUNC_ITEM_NUM_MAX);
        return HI_FAILURE;
    }

    if (g_gfx2d_ioctl_func[gfx2d_cmd].ioctl_func == NULL) {
        GRAPHIC_DFX_MINI_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "pointer is null");
        return HI_FAILURE;
    }

    if (cmd != g_gfx2d_ioctl_func[gfx2d_cmd].cmd) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, cmd);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, g_gfx2d_ioctl_func[gfx2d_cmd].cmd);
        return HI_FAILURE;
    }

    ret = g_gfx2d_ioctl_func[gfx2d_cmd].ioctl_func(arg);

    HI_GFX_TSTART(end_times);
    cost_times = end_times - start_times;

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, cost_times);
    return ret;
}

long drv_gfx2d_dev_ioctl(struct file *ffile, unsigned int cmd, unsigned long arg)
{
    hi_s32 ret;
    hi_bool is_open;

    if (ffile == NULL) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, ffile);
        return HI_FAILURE;
    }

    if (ffile->f_path.dentry == NULL) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, ffile->f_path.dentry);
        return HI_FAILURE;
    }

    is_open = gfx2d_check_open();
    if (is_open == HI_FALSE) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, gfx2d_check_open, FAILURE_TAG);
        return HI_FAILURE;
    }

    ret = hi_drv_user_copy(ffile, cmd, arg, gfx2d_ioctl);

    return (long)ret;
}

#ifdef CONFIG_COMPAT
static hi_slong gfx2d_compat_ioctl(struct file *file, unsigned int cmd, hi_void *arg)
{
    hi_slong ret;

    ret = gfx2d_ioctl(file, cmd, arg);

    return ret;
}

long drv_gfx2d_dev_compat_ioctl(struct file *ffile, unsigned int cmd, unsigned long arg)
{
    hi_s32 ret;
    hi_bool is_open;

    if (ffile == NULL) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, ffile);
        return HI_FAILURE;
    }

    if (ffile->f_path.dentry == NULL) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, ffile->f_path.dentry);
        return HI_FAILURE;
    }

    is_open = gfx2d_check_open();
    if (is_open == HI_FALSE) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, gfx2d_check_open, FAILURE_TAG);
        return HI_FAILURE;
    }

    ret = hi_drv_user_copy(ffile, cmd, arg, gfx2d_compat_ioctl);

    return (long)ret;
}
#endif

static hi_void drv_gfx2d_surface_conv_fd_to_addr(drv_gfx2d_compose_surface *surface)
{
    int i;

    for (i = 0; i < DRV_GFX2D_MAX_SURFACE_NUM; i++) {
        surface->phy_addr[i] = drv_gfx_mem_get_smmu_from_fd(surface->mem_handle[i], HIGFX_GFX2D_ID);
    }

    surface->histogram_phy_addr = drv_gfx_mem_get_smmu_from_fd(surface->histogram_mem_handle, HIGFX_GFX2D_ID);

    surface->palette_phy_addr = drv_gfx_mem_get_smmu_from_fd(surface->palette_mem_handle, HIGFX_GFX2D_ID);
}

static hi_void drv_gfx2d_list_conv_fd_to_addr(drv_gfx2d_compose_list *compose_list)
{
    int i;

    for (i = 0; i < compose_list->compose_cnt; i++) {
        drv_gfx2d_surface_conv_fd_to_addr(&compose_list->compose[i].compose_surface);
    }
}

static hi_s32 drv_gfx2d_compose(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 size;
    gfx2d_cmd_compose *compose = (gfx2d_cmd_compose *)arg;
    GFX2D_COMPOSE_CMD_HAL_S hal_compose = {0};
    drv_hifb_settings_info hifb_settings_info;
    hfib_export_func *hifb_func = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(arg, HI_FAILURE);

    memset_s(&hal_compose, sizeof(GFX2D_COMPOSE_CMD_HAL_S), 0x0, sizeof(GFX2D_COMPOSE_CMD_HAL_S));
    memset_s(&hifb_settings_info, sizeof(drv_hifb_settings_info), 0x0, sizeof(drv_hifb_settings_info));

    if (compose->is_compose_support != GFX2D_SUPPORT_MARK) {
        return DRV_GFX2D_ERR_UNSUPPORT;
    }

    if (compose->dev_id >= DRV_GFX2D_DEV_ID_BUTT) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, compose->dev_id);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, DRV_GFX2D_DEV_ID_BUTT);
        return DRV_GFX2D_ERR_INVALID_DEVID;
    }

    if ((compose->compose_list.compose_cnt == 0) || (compose->compose_list.compose_cnt > GFX2D_MAX_LAYERS)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, compose->compose_list.compose_cnt);
        return DRV_GFX2D_ERR_INVALID_COMPOSECNT;
    }

    GRAPHIC_CHECK_UINT_MULTIPLICATION_REVERSAL_RETURN(sizeof(drv_gfx2d_compose_msg), compose->compose_list.compose_cnt,
                                                      DRV_GFX2D_ERR_INVALID_COMPOSECNT);
    size = sizeof(drv_gfx2d_compose_msg) * compose->compose_list.compose_cnt;

    hal_compose.compose_list.compose = HI_GFX_KMALLOC(HIGFX_GFX2D_ID, size, GFP_KERNEL);
    if (hal_compose.compose_list.compose == NULL) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, size);
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HI_GFX_KMALLOC, DRV_GFX2D_ERR_NO_MEM);
        return DRV_GFX2D_ERR_NO_MEM;
    }
    memset_s(hal_compose.compose_list.compose, size, 0, size);

    hal_compose.work_sync = compose->work_sync;
    hal_compose.time_out = compose->time_out;
    hal_compose.dev_id = compose->dev_id;
    hal_compose.dest_surface = &(compose->dest_surface);

    if (compose->compose_list.compose == 0) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, compose->compose_list.compose);
        HI_GFX_KFREE(HIGFX_GFX2D_ID, hal_compose.compose_list.compose);
        return -EFAULT;
    }

    if (osal_copy_from_user(hal_compose.compose_list.compose, (hi_void *)(uintptr_t)(compose->compose_list.compose),
                            size)) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        HI_GFX_KFREE(HIGFX_GFX2D_ID, hal_compose.compose_list.compose);
        return -EFAULT;
    }

    hal_compose.compose_list.compose_cnt = compose->compose_list.compose_cnt;
    hal_compose.compose_list.background_color = compose->compose_list.back_color;

    drv_gfx2d_list_conv_fd_to_addr(&hal_compose.compose_list);
    drv_gfx2d_surface_conv_fd_to_addr(hal_compose.dest_surface);

    ret = osal_exportfunc_get(HI_ID_FB, (hi_void **)&hifb_func);
    if ((ret == HI_SUCCESS) && (hifb_func != NULL) && (hifb_func->drv_hifb_get_settings_info != NULL)) {
        hifb_func->drv_hifb_get_settings_info(0, &hifb_settings_info);
    }

    if (((hal_compose.dest_surface->phy_addr[0] <= hifb_settings_info.will_work_addr) &&
         (hifb_settings_info.will_work_addr < hal_compose.dest_surface->phy_addr[0] + hifb_settings_info.buffer_size)) ||
        ((hal_compose.dest_surface->phy_addr[0] <= hifb_settings_info.work_addr) &&
         (hifb_settings_info.work_addr < hal_compose.dest_surface->phy_addr[0] + hifb_settings_info.buffer_size))) {
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "========================================");
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, hifb_settings_info.timeline_value);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, hifb_settings_info.fence_value);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, hifb_settings_info.fence_refresh_cnt);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, hifb_settings_info.reg_update_cnt);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, hifb_settings_info.decompress_state);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, hal_compose.dest_surface->phy_addr[0]);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, hifb_settings_info.will_work_addr);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, hifb_settings_info.work_addr);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, hifb_settings_info.layer_stride);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, hifb_settings_info.buffer_size);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "========================================");
    }

    gfx2d_print_input_info(&hal_compose, hal_compose.dest_surface->release_fence_fd, HI_FALSE);

    ret = GFX2D_CTL_Compose(hal_compose.dev_id, &hal_compose.compose_list, hal_compose.dest_surface,
                            hal_compose.work_sync, hal_compose.time_out);
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, GFX2D_CTL_Compose, ret);
        HI_GFX_KFREE(HIGFX_GFX2D_ID, hal_compose.compose_list.compose);
        return -EFAULT;
    }

    if (osal_copy_to_user((hi_void *)(uintptr_t)compose->compose_list.compose, hal_compose.compose_list.compose,
                          size)) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        HI_GFX_KFREE(HIGFX_GFX2D_ID, hal_compose.compose_list.compose);
        gfx2d_print_input_info(&hal_compose, hal_compose.dest_surface->release_fence_fd, HI_TRUE);
        return -EFAULT;
    }

    gfx2d_print_input_info(&hal_compose, hal_compose.dest_surface->release_fence_fd, HI_TRUE);

    compose->dest_surface.release_fence_fd = hal_compose.dest_surface->release_fence_fd;

    HI_GFX_KFREE(HIGFX_GFX2D_ID, hal_compose.compose_list.compose);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 drv_gfx2d_wait_done(hi_void *arg)
{
    hi_s32 ret;
    gfx2d_cmd_wait_done *wait_done = (gfx2d_cmd_wait_done *)arg;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(arg, HI_FAILURE);

    if (wait_done->is_compose_support != GFX2D_SUPPORT_MARK) {
        return DRV_GFX2D_ERR_UNSUPPORT;
    }

    if (wait_done->dev_id >= DRV_GFX2D_DEV_ID_BUTT) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, DRV_GFX2D_DEV_ID_BUTT);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, wait_done->dev_id);
        return DRV_GFX2D_ERR_INVALID_DEVID;
    }

    ret = gfx2d_wait_done(wait_done->dev_id, wait_done->time_out);
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, gfx2d_wait_done, ret);
        return ret;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return ret;
}

static hi_void gfx2d_print_input_info(GFX2D_COMPOSE_CMD_HAL_S *compose, hi_u32 fence_fd, hi_bool is_input)
{
    hi_u32 index = 0;
    hi_u32 pixel_component_cnt = 0;

    if (is_input == HI_FALSE) {
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "============================================");
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->dev_id);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->work_sync);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->time_out);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->dest_surface->surface_type);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->dest_surface->format);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->dest_surface->width);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->dest_surface->height);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, compose->dest_surface->color_data);
        for (index = 0; index < DRV_GFX2D_MAX_SURFACE_NUM; index++) {
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, index);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, compose->dest_surface->phy_addr[index]);
        }

        for (index = 0; index < DRV_GFX2D_MAX_SURFACE_NUM; index++) {
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, index);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->dest_surface->stride[index]);
        }

        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->dest_surface->premulti_en);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->dest_surface->compress_msg.compress_type);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, compose->dest_surface->histogram_phy_addr);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->dest_surface->alpha_ext.alpha_ext_en);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->dest_surface->alpha_ext.alpha0);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->dest_surface->alpha_ext.alpha1);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, compose->dest_surface->palette_phy_addr);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->dest_surface->acquire_fence_fd);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->dest_surface->release_fence_fd);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "============================================");

        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->compose_list.compose_cnt);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, compose->compose_list.background_color);
        for (index = 0; index < compose->compose_list.compose_cnt; index++) {
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, index);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->compose_list.compose[index].in_rect.x);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->compose_list.compose[index].in_rect.y);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->compose_list.compose[index].in_rect.width);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->compose_list.compose[index].in_rect.height);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->compose_list.compose[index].out_rect.x);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->compose_list.compose[index].out_rect.y);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->compose_list.compose[index].out_rect.width);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->compose_list.compose[index].out_rect.height);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->compose_list.compose[index].opt.blend.cov_blend);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT,
                                    compose->compose_list.compose[index].opt.blend.global_alpha_en);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT,
                                    compose->compose_list.compose[index].opt.blend.pixel_alpha_en);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT,
                                    compose->compose_list.compose[index].opt.blend.global_alpha);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->compose_list.compose[index].opt.resize.resize_en);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT,
                                    compose->compose_list.compose[index].opt.resize.filter.filter_mode);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT,
                                    compose->compose_list.compose[index].compose_surface.surface_type);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT,
                                    compose->compose_list.compose[index].compose_surface.format);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->compose_list.compose[index].compose_surface.width);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT,
                                    compose->compose_list.compose[index].compose_surface.height);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT,
                                    compose->compose_list.compose[index].compose_surface.color_data);

            for (pixel_component_cnt = 0; pixel_component_cnt < DRV_GFX2D_MAX_SURFACE_NUM; pixel_component_cnt++) {
                GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pixel_component_cnt);
                GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, compose->compose_list.compose[index]
                                                                    .compose_surface.phy_addr[pixel_component_cnt]);
            }

            for (pixel_component_cnt = 0; pixel_component_cnt < DRV_GFX2D_MAX_SURFACE_NUM; pixel_component_cnt++) {
                GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pixel_component_cnt);
                GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->compose_list.compose[index]
                                                                    .compose_surface.stride[pixel_component_cnt]);
            }

            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT,
                                    compose->compose_list.compose[index].compose_surface.premulti_en);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT,
                                    compose->compose_list.compose[index].compose_surface.compress_msg.compress_type);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT,
                                    compose->compose_list.compose[index].compose_surface.histogram_phy_addr);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT,
                                    compose->compose_list.compose[index].compose_surface.alpha_ext.alpha_ext_en);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT,
                                    compose->compose_list.compose[index].compose_surface.alpha_ext.alpha0);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT,
                                    compose->compose_list.compose[index].compose_surface.alpha_ext.alpha1);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT,
                                    compose->compose_list.compose[index].compose_surface.palette_phy_addr);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT,
                                    compose->compose_list.compose[index].compose_surface.acquire_fence_fd);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT,
                                    compose->compose_list.compose[index].compose_surface.release_fence_fd);
        }
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "============================================");
    } else {
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "============================================");
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, fence_fd);
        for (index = 0; index < DRV_GFX2D_MAX_ALPHASUM; index++) {
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, index);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->dest_surface->ext_info.alpha_sum[index]);
        }
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "============================================");
    }

    return;
}
