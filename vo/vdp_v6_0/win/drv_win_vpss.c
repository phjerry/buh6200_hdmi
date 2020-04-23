/*
 * * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * * Description: win_vpss component
 * * Author: vdp
 * * Create: 2019-07-2
 * *
 */

#include "hi_type.h"
#include "hi_drv_mem.h"
#include "hi_drv_proc.h"
#include "hi_osal.h"
#include "drv_win_vpss.h"
#include "drv_vpss_ext.h"
#include "drv_xdp_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define WIN_VPSS_MAX         32
#define WIN_VPSS_PREFIX      0xFFFF0000
#define VPSS_MAX_DEQUE_TIMES 20
#define VDP_FRAME_WIDTH_4K   3840
#define VDP_FRAME_HEIGHT_4K  2160

#define win_vpss_kmalloc(a)                       \
    ({                                            \
        hi_void *b = HI_NULL;                     \
        b = osal_kmalloc(HI_ID_WIN, a, OSAL_GFP_KERNEL); \
        if (b != HI_NULL) {                       \
            memset(b, 0, a);                      \
        }                                         \
        b;                                        \
    })
#define win_vpss_kfree(a) osal_kfree(HI_ID_WIN, a)

#define WIN_VPSS_SPIN_LOCK(flags)   spin_lock_irqsave(&g_win_vpss_context.win_vpss_spin_lock, (flags))
#define WIN_VPSS_SPIN_UNLOCK(flags) spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, (flags))

typedef struct osal_list_head win_vpss_list;

typedef struct tag_win_vpss {
    hi_handle h_vpss_instance;
    hi_handle h_vpss_port;
    hi_handle h_window;
    hi_handle h_source;
    hi_drv_vpss_instance_cfg vpss_instance_config;
    hi_drv_vpss_port_cfg vpss_port_config;
} win_vpss_control;

typedef struct {
    win_vpss_list node;
    hi_handle h_win_vpss;
    win_vpss_control vpss_control;
} win_vpss_node;

typedef struct {
    hi_bool is_win_vpss_init;
    hi_u32 win_vpss_num;
    hi_drv_vpss_callback_func vpss_reg_func;
    hi_bool win_vpss_used_handle[WIN_VPSS_MAX];
    win_vpss_list win_vpss_node_list;
    osal_spinlock win_vpss_spin_lock;
} win_vpss_context;

static vpss_export_func *g_vpss_func = HI_NULL;
static win_vpss_context g_win_vpss_context = {0};

static hi_s32 window_vpss_check_null_func(vpss_export_func *vpss_func)
{
    if (vpss_func == HI_NULL) {
        WIN_ERROR("no VPSS module exist!\n");
        return HI_FAILURE;
    }

    if (vpss_func->vpss_get_instance_default_cfg == HI_NULL) {
        WIN_ERROR("vpss_func->vpss_get_instance_default_cfg is null\n");
        return HI_FAILURE;
    }

    if (vpss_func->vpss_create_instance == HI_NULL) {
        WIN_ERROR("vpss_func->vpss_create_instance is null\n");
        return HI_FAILURE;
    }

    if (vpss_func->vpss_destroy_instance == HI_NULL) {
        WIN_ERROR("vpss_func->vpss_destroy_instance is null\n");
        return HI_FAILURE;
    }

    if (vpss_func->vpss_get_port_default_cfg == HI_NULL) {
        WIN_ERROR("vpss_func->vpss_get_port_default_cfg is null\n");
        return HI_FAILURE;
    }

    if (vpss_func->vpss_create_port == HI_NULL) {
        WIN_ERROR("vpss_func->vpss_create_port is null\n");
        return HI_FAILURE;
    }

    if (vpss_func->vpss_destroy_port == HI_NULL) {
        WIN_ERROR("vpss_func->vpss_destroy_instance is null\n");
        return HI_FAILURE;
    }

    if (vpss_func->vpss_enable_port == HI_NULL) {
        WIN_ERROR("vpss_func->vpss_enable_port is null\n");
        return HI_FAILURE;
    }

    if (vpss_func->vpss_send_command == HI_NULL) {
        WIN_ERROR("vpss_send_command is null! \n ");
        return HI_FAILURE;
    }

    if (vpss_func->vpss_regist_callback == HI_NULL) {
        WIN_ERROR("vpss_func->vpss_regist_callback is null\n");
        return HI_FAILURE;
    }

    if ((vpss_func->vpss_suspend == HI_NULL) || (vpss_func->vpss_resume == HI_NULL)) {
        WIN_ERROR("vpss_func->vpss_suspend or resume is null\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 window_vpss_list_deinit(win_vpss_list *win_vpss_node_list)
{
    win_vpss_list *pos = HI_NULL;
    win_vpss_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    hi_s32 ret;
    win_vpss_node *vpss_node = HI_NULL;

    ret = HI_SUCCESS;
    osal_list_for_each_safe(pos, n, win_vpss_node_list)
    {
        vpss_node = osal_list_entry(pos, win_vpss_node, node);
        list_entry_times++;
        if (list_entry_times > WIN_VPSS_MAX) {
            WIN_ERROR("list error\n");
            ret = HI_FAILURE;
            break;
        }
        WIN_ERROR("win_vpss %d destroy error!!!!!\n", vpss_node->h_win_vpss);
        osal_list_del_init(pos);
        win_vpss_kfree(vpss_node);
        ret = HI_FAILURE;
    }

    return ret;
}

static hi_handle window_vpss_get_valid_handle(hi_void)
{
    hi_u32 i;

    for (i = 0; i < WIN_VPSS_MAX; i++) {
        if (g_win_vpss_context.win_vpss_used_handle[i] == HI_FALSE) {
            g_win_vpss_context.win_vpss_used_handle[i] = HI_TRUE;
            return i;
        }
    }
    return HI_INVALID_HANDLE;
}

static win_vpss_node *window_vpss_get_by_handle(hi_handle h_vpss)
{
    win_vpss_list *pos = HI_NULL;
    win_vpss_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    win_vpss_node *vpss_node = HI_NULL;

    osal_list_for_each_safe(pos, n, &(g_win_vpss_context.win_vpss_node_list))
    {
        vpss_node = osal_list_entry(pos, win_vpss_node, node);
        list_entry_times++;
        if (list_entry_times > WIN_VPSS_MAX) {
            WIN_ERROR("list error\n");
            vpss_node = HI_NULL;
            break;
        }
        if (vpss_node == HI_NULL) {
            WIN_ERROR("node is NULL\n");
            continue;
        }
        if (vpss_node->h_win_vpss == h_vpss) {
            break;
        }
        vpss_node = HI_NULL;
    }
    return vpss_node;
}

static win_vpss_node *window_vpss_get_by_win_handle(hi_handle h_window)
{
    win_vpss_list *pos = HI_NULL;
    win_vpss_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    win_vpss_node *vpss_node = HI_NULL;

    osal_list_for_each_safe(pos, n, &(g_win_vpss_context.win_vpss_node_list)) {
        vpss_node = osal_list_entry(pos, win_vpss_node, node);
        list_entry_times++;
        if (list_entry_times > WIN_VPSS_MAX) {
            WIN_ERROR("list error\n");
            vpss_node = HI_NULL;
            break;
        }
        if (vpss_node == HI_NULL) {
            WIN_ERROR("node is NULL\n");
            continue;
        }
        if (vpss_node->vpss_control.h_window == h_window) {
            break;
        }
        vpss_node = HI_NULL;
    }
    return vpss_node;
}

static win_vpss_node *window_vpss_check_same_source(hi_handle h_window, hi_handle h_source, hi_handle *src_handle)
{
    win_vpss_list *pos = HI_NULL;
    win_vpss_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    win_vpss_node *vpss_node = HI_NULL;

    osal_list_for_each_safe(pos, n, &(g_win_vpss_context.win_vpss_node_list)) {
        vpss_node = osal_list_entry(pos, win_vpss_node, node);
        list_entry_times++;
        if (list_entry_times > WIN_VPSS_MAX) {
            WIN_ERROR("list error\n");
            vpss_node = HI_NULL;
            break;
        }

        if (vpss_node == HI_NULL) {
            WIN_ERROR("node is NULL\n");
            continue;
        }

        if (((vpss_node->vpss_control.h_source == h_source) ||
             (vpss_node->vpss_control.h_window == h_source)) &&
            (vpss_node->vpss_control.h_window != h_window)) {
            *src_handle = vpss_node->vpss_control.h_window;
            break;
        }

        vpss_node = HI_NULL;
    }

    return vpss_node;
}

static win_vpss_node *window_vpss_get_special_node_and_delete(hi_handle h_vpss)
{
    win_vpss_list *pos = HI_NULL;
    win_vpss_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    win_vpss_node *vpss_node = HI_NULL;
    hi_s32 ret = HI_FAILURE;

    osal_list_for_each_safe(pos, n, &(g_win_vpss_context.win_vpss_node_list)) {
        vpss_node = osal_list_entry(pos, win_vpss_node, node);
        list_entry_times++;
        if (list_entry_times > WIN_VPSS_MAX) {
            WIN_ERROR("list error\n");
            ret = HI_FAILURE;
            vpss_node = HI_NULL;
            break;
        }

        if (vpss_node == HI_NULL) {
            WIN_ERROR("node is NULL\n");
            continue;
        }

        if (vpss_node->h_win_vpss == h_vpss) {
            osal_list_del_init(pos);
            ret = HI_SUCCESS;
            break;
        }

        vpss_node = HI_NULL;
    }

    return vpss_node;
}

static hi_s32 window_vpss_create_instance(win_vpss_node *win_vpss_instance)
{
    hi_s32 ret;

    memset(&win_vpss_instance->vpss_control.vpss_instance_config, 0, sizeof(hi_drv_vpss_instance_cfg));
    ret = g_vpss_func->vpss_get_instance_default_cfg(&win_vpss_instance->vpss_control.vpss_instance_config);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("vpss_get_default_cfg failed ! ret = 0x%x \n", ret);
        return HI_FAILURE;
    }
    /* set vpss bypass mode */
    win_vpss_instance->vpss_control.vpss_instance_config.work_mode = HI_DRV_VPSS_WORKMODE_AUTO;

    ret = g_vpss_func->vpss_create_instance(&win_vpss_instance->vpss_control.h_vpss_instance,
                                            &win_vpss_instance->vpss_control.vpss_instance_config);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("vpss_create_vpss failed ! ret = 0x%x \n", ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 window_vpss_create_port(win_vpss_node *win_vpss_port)
{
    hi_s32 ret;

    memset(&win_vpss_port->vpss_control.vpss_port_config, 0, sizeof(hi_drv_vpss_port_cfg));
    ret = g_vpss_func->vpss_get_port_default_cfg(&win_vpss_port->vpss_control.vpss_port_config);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("vpss_get_default_port_cfg failed ! ret = 0x%x \n", ret);
        return HI_FAILURE;
    }

    ret = g_vpss_func->vpss_create_port(win_vpss_port->vpss_control.h_vpss_instance,
                                        &win_vpss_port->vpss_control.vpss_port_config,
                                        &win_vpss_port->vpss_control.h_vpss_port);

    if (ret != HI_SUCCESS) {
        WIN_ERROR("vpss_create_port failed ! ret = 0x%x \n", ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 window_vpss_get_prepared(win_vpss_node **win_vpss_new, hi_handle h_window, hi_handle h_source)
{
    hi_ulong flags;
    hi_handle win_vpss_handle;

    osal_spin_lock_irqsave(&g_win_vpss_context.win_vpss_spin_lock, &flags);
    win_vpss_handle = window_vpss_get_valid_handle();
    if (win_vpss_handle == HI_INVALID_HANDLE) {
        WIN_ERROR("cannot get valid win_vpss handle\n");
        osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);
        return HI_FAILURE;
    }
    osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);

    *win_vpss_new = (win_vpss_node *)win_vpss_kmalloc(sizeof(win_vpss_node));
    if ((*win_vpss_new) == HI_NULL) {
        g_win_vpss_context.win_vpss_used_handle[win_vpss_handle] = HI_FALSE;
        WIN_ERROR("win_vpss malloc failed\n");
        return HI_FAILURE;
    }

    memset(*win_vpss_new, 0, sizeof(win_vpss_node));
    (*win_vpss_new)->h_win_vpss = win_vpss_handle;
    (*win_vpss_new)->vpss_control.h_source = h_source;
    (*win_vpss_new)->vpss_control.h_window = h_window;

    return HI_SUCCESS;
}

static hi_s32 window_vpss_regist_port(hi_handle port_handle, hi_handle src_handle, hi_handle sink_handle)
{
    hi_s32 ret;

    ret = g_vpss_func->vpss_regist_callback(port_handle, sink_handle, src_handle,
                                            &(g_win_vpss_context.vpss_reg_func));
    if (ret != HI_SUCCESS) {
        WIN_ERROR("vpss_regist_hook failed ! ret = 0x%x \n", ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 window_vpss_enable_port(win_vpss_node *win_vpss_new)
{
    hi_s32 ret;

    ret = g_vpss_func->vpss_enable_port(win_vpss_new->vpss_control.h_vpss_port, HI_TRUE);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("vpss_enable_port failed ! ret = 0x%x \n", ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 window_vpss_disable_port(win_vpss_node *win_vpss_del)
{
    hi_s32 ret;

    /* disable_port */
    ret = g_vpss_func->vpss_enable_port(win_vpss_del->vpss_control.h_vpss_port, HI_FALSE);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("call vpss_enable_port failed !\n");
    }

    return ret;
}

static hi_s32 window_vpss_destroy_port(win_vpss_node *win_vpss_del)
{
    hi_s32 ret;

    /* destroy_port */
    ret = g_vpss_func->vpss_destroy_port(win_vpss_del->vpss_control.h_vpss_port);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("call vpss_destroy_port failed !\n");
        ret = HI_FAILURE;
    }
    win_vpss_del->vpss_control.h_vpss_port = HI_INVALID_HANDLE;
    return ret;
}

static hi_s32 window_vpss_destroy_instance(win_vpss_node *win_vpss_del)
{
    hi_s32 ret;

    ret = g_vpss_func->vpss_destroy_instance(win_vpss_del->vpss_control.h_vpss_instance);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("call vpss_destroy_vpss failed !\n");
        return HI_FAILURE;
    }
    win_vpss_del->vpss_control.h_vpss_instance = HI_INVALID_HANDLE;
    return HI_SUCCESS;
}

static hi_void window_vpss_deque_the_rest_frame(win_vpss_node *win_vpss_del)
{
    hi_drv_video_frame *frame = HI_NULL;
    hi_u32 vpss_deque_times = 0;

    frame = (hi_drv_video_frame *)win_vpss_kmalloc(sizeof(hi_drv_video_frame));
    if (frame != HI_NULL) {
        while (g_vpss_func->vpss_dequeue_frame(win_vpss_del->vpss_control.h_vpss_instance, frame) == HI_SUCCESS) {
            vpss_deque_times++;
            if (vpss_deque_times > VPSS_MAX_DEQUE_TIMES) {
                WIN_ERROR("vpss_deque_times is %d, max times is %d\n", vpss_deque_times, VPSS_MAX_DEQUE_TIMES);
                break;
            }
        }
        win_vpss_kfree(frame);
    } else {
        WIN_ERROR("frame malloc failed\n");
    }
    return;
}

static hi_void window_vpss_try_to_destroy_instance(win_vpss_node *win_vpss_del)
{
    win_vpss_node *vpss_node = HI_NULL;
    win_vpss_list *pos = HI_NULL;
    win_vpss_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;

    osal_list_for_each_safe(pos, n, &(g_win_vpss_context.win_vpss_node_list))
    {
        vpss_node = osal_list_entry(pos, win_vpss_node, node);
        list_entry_times++;
        if (list_entry_times > WIN_VPSS_MAX) {
            WIN_ERROR("list error\n");
            break;
            return;
        }
        if (vpss_node == HI_NULL) {
            WIN_ERROR("node is NULL\n");
            continue;
        }

        if ((((vpss_node->vpss_control.h_source == win_vpss_del->vpss_control.h_source) &&
            (vpss_node->vpss_control.h_source != HI_INVALID_HANDLE)) ||
            (vpss_node->vpss_control.h_window == win_vpss_del->vpss_control.h_source)) &&
            (vpss_node->vpss_control.h_window != win_vpss_del->vpss_control.h_window)) {
            WIN_INFO("source already exists, no need to destroy instance!\n");
            return;
        }
    }

    if (g_vpss_func->vpss_destroy_instance(win_vpss_del->vpss_control.h_vpss_instance) != HI_SUCCESS) {
        WIN_ERROR("call vpss_destroy_vpss failed !\n");
        return;
    }
    win_vpss_del->vpss_control.h_vpss_instance = HI_INVALID_HANDLE;
    return;
}

hi_void window_vpss_set_captured_port_cfg(hi_drv_vpss_port_cfg *port_cfg, hi_drv_video_frame *frame)
{
    hi_drv_rect rect_4K = { 0, 0, VDP_FRAME_WIDTH_4K, VDP_FRAME_HEIGHT_4K };

    /* 8bit yuv420 */
    port_cfg->bit_width = HI_DRV_PIXEL_BITWIDTH_8BIT;
    port_cfg->format = HI_DRV_PIXEL_FMT_NV21;
    port_cfg->cmp_info.cmp_mode = HI_DRV_COMPRESS_MODE_OFF;
    port_cfg->hdr_follow_src = HI_FALSE;
    port_cfg->out_hdr_type = HI_DRV_HDR_TYPE_SDR;

    port_cfg->out_color_desp.color_space = HI_DRV_COLOR_CS_YUV;
    port_cfg->out_color_desp.quantify_range = HI_DRV_COLOR_LIMITED_RANGE;
    port_cfg->out_color_desp.color_primary = HI_DRV_COLOR_PRIMARY_BT709;
    port_cfg->out_color_desp.transfer_type = HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR;
    port_cfg->out_color_desp.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT709;

    port_cfg->user_crop_en = HI_FALSE;
    memset(&port_cfg->in_offset_rect, 0, sizeof(hi_drv_crop_rect));
    memset(&port_cfg->crop_rect, 0, sizeof(hi_drv_rect));
    if ((frame->width > VDP_FRAME_WIDTH_4K) || (frame->height > VDP_FRAME_HEIGHT_4K)) {
        port_cfg->video_rect = rect_4K;
        port_cfg->out_rect = rect_4K;
    } else {
        memset(&port_cfg->video_rect, 0, sizeof(hi_drv_rect));
        memset(&port_cfg->out_rect, 0, sizeof(hi_drv_rect));
    }
    return;
}

static hi_s32 window_vpss_check_create_para(hi_handle h_window)
{
    if (h_window == HI_INVALID_HANDLE) {
        WIN_ERROR("win_vpss create failed\n");
        return HI_FAILURE;
    }

    if (g_win_vpss_context.is_win_vpss_init == HI_FALSE) {
        WIN_ERROR("win_vpss initiation is undone.\n");
        return HI_FAILURE;
    }

    if (g_vpss_func == HI_NULL) {
        WIN_ERROR("g_vpss_func is null\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 win_vpss_init(win_vpss_export_function *vpss_reg_func)
{
    hi_s32 i;
    hi_s32 ret;

    if (g_win_vpss_context.is_win_vpss_init == HI_TRUE) {
        return HI_SUCCESS;
    }

    memset(&g_win_vpss_context, 0, sizeof(win_vpss_context));
    OSAL_INIT_LIST_HEAD(&(g_win_vpss_context.win_vpss_node_list));
    ret = osal_spin_lock_init(&g_win_vpss_context.win_vpss_spin_lock);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("win vpss spin_lock initiation failed!\n");
        return HI_FAILURE;
    }

    for (i = 0; i < WIN_VPSS_MAX; i++) {
        g_win_vpss_context.win_vpss_used_handle[i] = HI_FALSE;
    }

    g_win_vpss_context.vpss_reg_func.sink_queue_frame = vpss_reg_func->vpss_queue_frame;
    g_win_vpss_context.vpss_reg_func.sink_dequeue_frame = vpss_reg_func->vpss_dequeue_frame;
    g_win_vpss_context.vpss_reg_func.src_complete = vpss_reg_func->vpss_complete_frame;
    g_win_vpss_context.vpss_reg_func.get_port_cfg_by_frame = vpss_reg_func->vpss_get_out_config;

    g_win_vpss_context.is_win_vpss_init = HI_TRUE;

    return HI_SUCCESS;
}

hi_s32 win_vpss_deinit(hi_void)
{
    hi_ulong flags;
    hi_s32 ret;

    if (g_win_vpss_context.is_win_vpss_init == HI_FALSE) {
        return HI_SUCCESS;
    }

    osal_spin_lock_irqsave(&g_win_vpss_context.win_vpss_spin_lock, &flags);
    ret = window_vpss_list_deinit(&(g_win_vpss_context.win_vpss_node_list));
    if (ret != HI_SUCCESS) {
        WIN_ERROR("win deinit vpss list error\n");
    }
    osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);

    osal_spin_lock_destory(&g_win_vpss_context.win_vpss_spin_lock);
    memset(&g_win_vpss_context, 0, sizeof(win_vpss_context));
    g_vpss_func = HI_NULL;
    return HI_SUCCESS;
}

hi_handle win_vpss_create(hi_handle h_window, hi_handle h_source, hi_bool is_passive)
{
    hi_s32 ret;
    win_vpss_node *win_vpss_new = HI_NULL;
    hi_ulong flags;

    if (g_vpss_func == HI_NULL) {
        ret = hi_drv_module_get_func(HI_ID_VPSS, (hi_void **)&g_vpss_func);
        if (ret != HI_SUCCESS) {
            WIN_ERROR("no VPSS module exist!\n");
            return HI_FAILURE;
        }
    }

    if (window_vpss_check_null_func(g_vpss_func) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (window_vpss_check_create_para(h_window) != HI_SUCCESS) {
        WIN_ERROR("create parameter is invalid, h_win:%d\n", h_window);
        return HI_INVALID_HANDLE;
    }

    ret = window_vpss_get_prepared(&win_vpss_new, h_window, h_source);
    if (ret != HI_SUCCESS) {
        return HI_INVALID_HANDLE;
    }

    if (window_vpss_create_instance(win_vpss_new) != HI_SUCCESS) {
        goto WIN_VPSS_FREE;
    }

    if (window_vpss_create_port(win_vpss_new) != HI_SUCCESS) {
        goto WIN_VPSS_INSTANCE_DESTROY;
    }

    if (is_passive != HI_TRUE) {
        ret = window_vpss_regist_port(win_vpss_new->vpss_control.h_vpss_port, win_vpss_new->vpss_control.h_window,
                                      win_vpss_new->vpss_control.h_window);
        if (ret != HI_SUCCESS) {
            goto WIN_VPSS_PORT_DESTROY;
        }
    }

    if (window_vpss_enable_port(win_vpss_new) != HI_SUCCESS) {
        goto WIN_VPSS_PORT_DESTROY;
    }

    osal_spin_lock_irqsave(&g_win_vpss_context.win_vpss_spin_lock, &flags);
    g_win_vpss_context.win_vpss_num++;
    osal_list_add_tail(&(win_vpss_new->node), &(g_win_vpss_context.win_vpss_node_list));
    osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);
    return win_vpss_new->h_win_vpss;

WIN_VPSS_PORT_DESTROY:
    g_vpss_func->vpss_destroy_port(win_vpss_new->vpss_control.h_vpss_port);
WIN_VPSS_INSTANCE_DESTROY:
    g_vpss_func->vpss_destroy_instance(win_vpss_new->vpss_control.h_vpss_instance);
WIN_VPSS_FREE:
    osal_spin_lock_irqsave(&g_win_vpss_context.win_vpss_spin_lock, &flags);
    g_win_vpss_context.win_vpss_used_handle[win_vpss_new->h_win_vpss] = HI_FALSE;
    osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);
    win_vpss_kfree(win_vpss_new);
    return HI_INVALID_HANDLE;
}

hi_handle win_vpss_update_src(hi_handle h_window, hi_handle h_source)
{
    win_vpss_node *vpss_node_updated = HI_NULL;
    win_vpss_node *vpss_node_tmp = HI_NULL;
    hi_s32 ret = HI_SUCCESS;
    hi_ulong flags;
    hi_handle src_handle = HI_INVALID_HANDLE;

    if (window_vpss_check_create_para(h_window) != HI_SUCCESS) {
        return HI_INVALID_HANDLE;
    }

    /* update source handle */
    osal_spin_lock_irqsave(&g_win_vpss_context.win_vpss_spin_lock, &flags);
    vpss_node_updated = window_vpss_get_by_win_handle(h_window);
    if (vpss_node_updated == HI_NULL) {
        osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);
        goto ERR_UPDATE_SRC;
    }
    osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);

    /* find vpss with same source */
    vpss_node_tmp = window_vpss_check_same_source(h_window, h_source, &src_handle);

    if (vpss_node_tmp != HI_NULL) {
        /* destroy old instance and port when with same source */
        ret = window_vpss_destroy_port(vpss_node_updated);
        if (ret != HI_SUCCESS) {
            goto ERR_UPDATE_SRC;
        }
        ret = window_vpss_destroy_instance(vpss_node_updated);
        if (ret != HI_SUCCESS) {
            goto ERR_UPDATE_SRC;
        }

        /* update new instance handle and create new port */
        vpss_node_updated->vpss_control.h_vpss_instance = vpss_node_tmp->vpss_control.h_vpss_instance;
        ret = window_vpss_create_port(vpss_node_updated);
        if (ret != HI_SUCCESS) {
            goto ERR_UPDATE_SRC;
        }
        ret = window_vpss_regist_port(vpss_node_updated->vpss_control.h_vpss_port, src_handle, h_window);
        if (ret != HI_SUCCESS) {
            window_vpss_destroy_port(vpss_node_updated);
            goto ERR_UPDATE_SRC;
        }
        ret =  window_vpss_enable_port(vpss_node_updated);
        if (ret != HI_SUCCESS) {
            window_vpss_destroy_port(vpss_node_updated);
            goto ERR_UPDATE_SRC;
        }
    }

    vpss_node_updated->vpss_control.h_source = h_source;

    return vpss_node_updated->h_win_vpss;

ERR_UPDATE_SRC:
    WIN_ERROR("win_vpss_update_src failed !\n");
    return HI_INVALID_HANDLE;
}

hi_void win_vpss_destroy(hi_handle h_vpss)
{
    win_vpss_node *win_vpss_del;
    hi_ulong flags;

    if (g_win_vpss_context.is_win_vpss_init == HI_FALSE) {
        WIN_ERROR("cannot destroy win_vpss before initiation.\n");
        return;
    }
    if (g_vpss_func == HI_NULL) {
        WIN_ERROR("g_vpss_func is null\n");
        return;
    }

    osal_spin_lock_irqsave(&g_win_vpss_context.win_vpss_spin_lock, &flags);
    win_vpss_del = window_vpss_get_special_node_and_delete(h_vpss);
    if (win_vpss_del == HI_NULL) {
        WIN_ERROR("cannot destroy win_vpss by handle.\n");
        osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);
        return;
    }

    g_win_vpss_context.win_vpss_used_handle[h_vpss] = HI_FALSE;
    if (g_win_vpss_context.win_vpss_num > 0) {
        g_win_vpss_context.win_vpss_num--;
    }
    osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);

    /* deque_the_rest_frame */
    window_vpss_deque_the_rest_frame(win_vpss_del);

    if (win_vpss_del->vpss_control.h_vpss_port != HI_INVALID_HANDLE) {
        window_vpss_disable_port(win_vpss_del);
        window_vpss_destroy_port(win_vpss_del);
        win_vpss_del->vpss_control.h_vpss_port = HI_INVALID_HANDLE;
    }

    if (win_vpss_del->vpss_control.h_vpss_instance != HI_INVALID_HANDLE) {
        window_vpss_try_to_destroy_instance(win_vpss_del);
    }

    win_vpss_kfree(win_vpss_del);
    return;
}

hi_s32 win_vpss_send_command(hi_handle h_vpss,
                             win_vpss_set_command vpss_command, hi_void *p_args)
{
    hi_s32 ret = HI_SUCCESS;
    win_vpss_node *win_vpss_send_cmd;

    if ((h_vpss == HI_INVALID_HANDLE) || (g_vpss_func == HI_NULL)) {
        return HI_SUCCESS;
    }

    win_vpss_send_cmd = window_vpss_get_by_handle(h_vpss);
    if (win_vpss_send_cmd == HI_NULL) {
        WIN_ERROR("cannot get win_vpss by handle\n");
        return HI_FAILURE;
    }

    switch (vpss_command) {
        case WIN_VPSS_START:
        case WIN_VPSS_STOP:
        case WIN_VPSS_RESET:
        case WIN_VPSS_PAUSE:
        case WIN_VPSS_WAKEUP:
        case WIN_VPSS_HOLD:
            ret = g_vpss_func->vpss_send_command(win_vpss_send_cmd->vpss_control.h_vpss_instance,
                                                 (hi_drv_vpss_user_command)vpss_command, p_args);
            break;
        default:
            WIN_ERROR("command not support!, cmd = %d\n", vpss_command);
            ret = HI_FAILURE;
            break;
    }

    if (ret != HI_SUCCESS) {
        WIN_ERROR("send error!!!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 win_vpss_queue_frame(hi_handle h_vpss, hi_drv_video_frame *src_frame)
{
    hi_s32 ret;
    win_vpss_node *win_vpss_que;
    hi_ulong flags;

    if (src_frame == HI_NULL) {
        WIN_ERROR("queue_frame is empty in win_vpss\n");
        return HI_FAILURE;
    }

    osal_spin_lock_irqsave(&g_win_vpss_context.win_vpss_spin_lock, &flags);
    win_vpss_que = window_vpss_get_by_handle(h_vpss);
    if (win_vpss_que == HI_NULL) {
        WIN_ERROR("cannot get win_vpss by handle\n");
        osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);
        return HI_FAILURE;
    }

    if (win_vpss_que->vpss_control.h_window == HI_INVALID_HANDLE) {
        WIN_ERROR("h_window is null! \n ");
        osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);
        return HI_FAILURE;
    }

    ret = g_vpss_func->vpss_queue_frame(win_vpss_que->vpss_control.h_vpss_instance, src_frame);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("win_vpss_queue_frame failed! ret = 0x%x \n", ret);
        osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);
        return ret;
    }
    osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);

    return HI_SUCCESS;
}

hi_s32 win_vpss_dequeue_frame(hi_handle h_vpss, hi_drv_video_frame *src_frame)
{
    hi_s32 ret;
    win_vpss_node *win_vpss_deque;
    hi_ulong flags;

    osal_spin_lock_irqsave(&g_win_vpss_context.win_vpss_spin_lock, &flags);
    win_vpss_deque = window_vpss_get_by_handle(h_vpss);
    if (win_vpss_deque == HI_NULL) {
        WIN_ERROR("cannot get win_vpss by handle\n");
        osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);
        return HI_FAILURE;
    }

    if (win_vpss_deque->vpss_control.h_window == HI_INVALID_HANDLE) {
        WIN_ERROR("h_window is null! \n ");
        osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);
        return HI_FAILURE;
    }

    ret = g_vpss_func->vpss_dequeue_frame(win_vpss_deque->vpss_control.h_vpss_instance, src_frame);
    if (ret != HI_SUCCESS) {
        WIN_INFO("win_vpss_dequeue_frame failed! ret = 0x%x \n", ret);
        osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);
        return ret;
    }
    osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);

    return HI_SUCCESS;
}

hi_s32 win_vpss_set_frame_proginterleave_mode(hi_handle h_vpss, hi_drv_win_forceframeprog_flag frame_mode)
{
    win_vpss_node *win_vpss_frame_mode = HI_NULL;
    hi_ulong flags;
    hi_s32 ret;
    hi_drv_vpss_instance_cfg vpss_instance_config;

    if (g_vpss_func == HI_NULL) {
        WIN_ERROR("g_vpss_func is null\n");
        return HI_FAILURE;
    }

    osal_spin_lock_irqsave(&g_win_vpss_context.win_vpss_spin_lock, &flags);

    win_vpss_frame_mode = window_vpss_get_by_handle(h_vpss);
    if (win_vpss_frame_mode == HI_NULL) {
        osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);
        WIN_ERROR("cannot get win_vpss by handle\n");
        return HI_FAILURE;
    }
    osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);

    ret = g_vpss_func->vpss_get_instance_cfg(win_vpss_frame_mode->vpss_control.h_vpss_instance,
                                             &vpss_instance_config);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("vpss_get_instance_cfg failed! ret = 0x%x \n", ret);
        return ret;
    }

    if (frame_mode == WIN_FRAME_FORCE_PROG) {
        vpss_instance_config.prog_detect = HI_DRV_VPSS_PROG_DETECT_FORCE_PROGRESSIVE;
    } else if (frame_mode == WIN_FRAME_FORCE_INTERLEAVE) {
        vpss_instance_config.prog_detect = HI_DRV_VPSS_PROG_DETECT_FORCE_INTERLACE;
    } else if (frame_mode == WIN_FRAME_FORCE_CLOSE) {
        vpss_instance_config.prog_detect = HI_DRV_VPSS_PROG_DETECT_OFF;
    } else {
        vpss_instance_config.prog_detect = HI_DRV_VPSS_PROG_DETECT_AUTO;
    }

    ret = g_vpss_func->vpss_set_instance_cfg(win_vpss_frame_mode->vpss_control.h_vpss_instance,
                                             &vpss_instance_config);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("vpss_set_instance_cfg failed! ret = 0x%x \n", ret);
    }
    return ret;
}

hi_s32 win_vpss_get_frame_proginterleave_mode(hi_handle h_vpss, hi_drv_win_forceframeprog_flag *frame_mode)
{
    win_vpss_node *win_vpss_frame_mode = HI_NULL;
    hi_ulong flags;
    hi_s32 ret;
    hi_drv_vpss_instance_cfg vpss_instance_config;

    if (g_vpss_func == HI_NULL) {
        WIN_ERROR("g_vpss_func is null\n");
        return HI_FAILURE;
    }

    osal_spin_lock_irqsave(&g_win_vpss_context.win_vpss_spin_lock, &flags);

    win_vpss_frame_mode = window_vpss_get_by_handle(h_vpss);
    if (win_vpss_frame_mode == HI_NULL) {
        osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);
        WIN_ERROR("cannot get win_vpss by handle\n");
        return HI_FAILURE;
    }
    osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);

    ret = g_vpss_func->vpss_get_instance_cfg(win_vpss_frame_mode->vpss_control.h_vpss_instance,
                                             &vpss_instance_config);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("vpss_get_instance_cfg failed! ret = 0x%x \n", ret);
        return ret;
    }

    if (vpss_instance_config.prog_detect == HI_DRV_VPSS_PROG_DETECT_FORCE_PROGRESSIVE) {
        *frame_mode = WIN_FRAME_FORCE_PROG;
    } else if (vpss_instance_config.prog_detect == HI_DRV_VPSS_PROG_DETECT_FORCE_INTERLACE) {
        *frame_mode = WIN_FRAME_FORCE_INTERLEAVE;
    } else if (vpss_instance_config.prog_detect == HI_DRV_VPSS_PROG_DETECT_OFF) {
        *frame_mode = WIN_FRAME_FORCE_CLOSE;
    } else {
        *frame_mode = WIN_FRAME_FORCE_MAX;
    }

    return ret;
}

hi_s32 win_vpss_suspend(hi_void)
{
    hi_s32 ret;

    ret = hi_drv_module_get_func(HI_ID_VPSS, (hi_void **)&g_vpss_func);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("no VPSS module exist!\n");
        return HI_FAILURE;
    }

    ret = g_vpss_func->vpss_suspend();
    if (ret != HI_SUCCESS) {
        WIN_ERROR("win_vpss_suspend failed! ret = 0x%x \n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 win_vpss_resume(hi_void)
{
    hi_s32 ret;

    ret = hi_drv_module_get_func(HI_ID_VPSS, (hi_void **)&g_vpss_func);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("no VPSS module exist!\n");
        return HI_FAILURE;
    }

    ret = g_vpss_func->vpss_resume();
    if (ret != HI_SUCCESS) {
        WIN_ERROR("win_vpss_resume failed! ret = 0x%x \n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_void win_vpss_proc_info(hi_handle h_vpss, struct seq_file *p)
{
    win_vpss_node *win_vpss_proc;
    hi_ulong flags;

    win_vpss_list *pos = HI_NULL;
    win_vpss_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    win_vpss_node *vpss_node = HI_NULL;

    osal_spin_lock_irqsave(&g_win_vpss_context.win_vpss_spin_lock, &flags);
    win_vpss_proc = window_vpss_get_by_handle(h_vpss);
    if (win_vpss_proc == HI_NULL) {
        WIN_ERROR("cannot get win_vpss by handle\n");
        osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);
        return;
    }

    HI_PROC_PRINT(p, "------------------------------------window vpss info---------------------------------\n");
    HI_PROC_PRINT(p, "%-40s:%-10d/%-10d\n", "win_vpss number", g_win_vpss_context.is_win_vpss_init,
                  g_win_vpss_context.win_vpss_num);

    osal_list_for_each_safe(pos, n, &(g_win_vpss_context.win_vpss_node_list))
    {
        vpss_node = osal_list_entry(pos, win_vpss_node, node);
        list_entry_times++;
        if (list_entry_times > WIN_VPSS_MAX) {
            WIN_ERROR("list error\n");
            vpss_node = HI_NULL;
            break;
        }
        if (vpss_node == HI_NULL) {
            WIN_ERROR("node is NULL\n");
            continue;
        }
        HI_PROC_PRINT(p, "%-40s:%08x/%08x/%08x\n", "win_vpss handle/window handle/source handle",
                      vpss_node->h_win_vpss, vpss_node->vpss_control.h_window, vpss_node->vpss_control.h_source);
        HI_PROC_PRINT(p, "%-40s:%08x/%08x\n", "win_vpss instance handle/port handle",
                      vpss_node->vpss_control.h_vpss_instance, vpss_node->vpss_control.h_vpss_port);

        vpss_node = HI_NULL;
    }

    osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);
    return;
}

hi_void win_vpss_proc_set_bypass(hi_handle h_vpss, hi_bool is_vpss_bypss_mode)
{
    win_vpss_node *win_vpss_proc;
    hi_ulong flags;
    hi_s32 ret;
    hi_drv_vpss_instance_cfg vpss_instance_config;

    if (g_vpss_func == HI_NULL) {
        WIN_ERROR("g_vpss_func is null\n");
        return;
    }

    osal_spin_lock_irqsave(&g_win_vpss_context.win_vpss_spin_lock, &flags);

    win_vpss_proc = window_vpss_get_by_handle(h_vpss);
    if (win_vpss_proc == HI_NULL) {
        WIN_ERROR("cannot get win_vpss by handle\n");
        osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);
        return;
    }
    osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);

    ret = g_vpss_func->vpss_get_instance_cfg(win_vpss_proc->vpss_control.h_vpss_instance,
                                             &vpss_instance_config);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("vpss_get_instance_cfg failed! ret = 0x%x \n", ret);
        return;
    }

    if (is_vpss_bypss_mode != HI_TRUE) {
        vpss_instance_config.work_mode = HI_DRV_VPSS_WORKMODE_AUTO;
    } else {
        vpss_instance_config.work_mode = HI_DRV_VPSS_WORKMODE_BYPASS;
    }

    ret = g_vpss_func->vpss_set_instance_cfg(win_vpss_proc->vpss_control.h_vpss_instance,
                                             &vpss_instance_config);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("vpss_set_instance_cfg failed! ret = 0x%x \n", ret);
    }
    return;
}

hi_s32 win_vpss_transfer_frame(hi_handle h_vpss, hi_drv_video_frame *frame)
{
    hi_s32 ret = HI_FAILURE;
    win_vpss_node *win_vpss_capture;
    hi_ulong flags;
    hi_drv_vpss_port_cfg port_cfg;
    hi_u32 delay = 0;

    osal_spin_lock_irqsave(&g_win_vpss_context.win_vpss_spin_lock, &flags);
    /* vpss for capture must exist at this time */
    win_vpss_capture = window_vpss_get_by_handle(h_vpss);
    osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);

    ret = g_vpss_func->vpss_get_port_cfg(win_vpss_capture->vpss_control.h_vpss_port, &port_cfg);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("vpss_get_instance_cfg failed! ret = 0x%x \n", ret);
        return HI_FAILURE;
    }

    window_vpss_set_captured_port_cfg(&port_cfg, frame);

    ret = g_vpss_func->vpss_set_port_cfg(win_vpss_capture->vpss_control.h_vpss_port, &port_cfg);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("vpss_set_port_cfg failed! ret = 0x%x \n", ret);
        return HI_FAILURE;
    }

    ret = g_vpss_func->vpss_queue_frame(win_vpss_capture->vpss_control.h_vpss_instance, frame);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("win_vpss_queue_frame failed! ret = 0x%x \n", ret);
        return HI_FAILURE;
    }

    ret = g_vpss_func->vpss_send_command(win_vpss_capture->vpss_control.h_vpss_instance,
                                         (hi_drv_vpss_user_command)WIN_VPSS_START, HI_NULL);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("vpss_send_command failed! ret = 0x%x \n", ret);
        return HI_FAILURE;
    }

    ret = HI_FAILURE;
    while (ret != HI_SUCCESS) {
        /* return failture when time is up */
        if (delay++ < 10) { /* 10 is times */
            osal_msleep_uninterruptible(10); /* 10 is delay time */
            WIN_WARN("wait to acquire frame !");
        } else {
            WIN_ERROR("vpss_acquire_frame time out!");
            return HI_FAILURE;
        }
        ret = g_vpss_func->vpss_acquire_frame(win_vpss_capture->vpss_control.h_vpss_port, frame);
    }

    return ret;
}

hi_s32 win_vpss_release_transferd_frame(hi_handle h_vpss, hi_drv_video_frame *frame)
{
    hi_s32 ret = HI_FAILURE;
    win_vpss_node *win_vpss_capture;
    hi_ulong flags;
    hi_u32 delay = 0;

    osal_spin_lock_irqsave(&g_win_vpss_context.win_vpss_spin_lock, &flags);
    win_vpss_capture = window_vpss_get_by_handle(h_vpss);
    if (win_vpss_capture == HI_NULL) {
        WIN_ERROR("cannot get win_vpss by handle\n");
        osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);
        return HI_FAILURE;
    }
    osal_spin_unlock_irqrestore(&g_win_vpss_context.win_vpss_spin_lock, &flags);

    ret = g_vpss_func->vpss_release_frame(win_vpss_capture->vpss_control.h_vpss_port, frame);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("vpss_release_frame failed! ret = 0x%x \n", ret);
        goto VPSS_DESTROY;
    }
    ret = HI_FAILURE;
    while (ret != HI_SUCCESS) {
        /* return failture when time is up */
        if (delay++ < 10) { /* 10 is times */
            osal_msleep_uninterruptible(10); /* 10 is delay time */
            WIN_WARN("wait to deque frame!");
        } else {
            WIN_ERROR("vpss_dequeue_frame time out!");
            ret = HI_FAILURE;
            goto VPSS_DESTROY;
        }
        ret = g_vpss_func->vpss_dequeue_frame(win_vpss_capture->vpss_control.h_vpss_instance, frame);
    }

    ret = g_vpss_func->vpss_send_command(win_vpss_capture->vpss_control.h_vpss_instance,
                                         (hi_drv_vpss_user_command)WIN_VPSS_STOP, HI_NULL);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("vpss_send_command failed! ret = 0x%x \n", ret);
        goto VPSS_DESTROY;
    }

VPSS_DESTROY:
    win_vpss_destroy(win_vpss_capture->h_win_vpss);
    return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
