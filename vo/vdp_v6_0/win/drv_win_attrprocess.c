/*
 * * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * * Description: win_attrprocess Component
 * * Author: image team
 * * Create: 2019-07-2
 * *
 */

#include "hi_type.h"
#include "drv_win_attrprocess.h"
// define LOCAL_TEST
#ifndef LOCAL_TEST
#include "linux/hisilicon/securec.h"
#include "drv_xdp_osal.h"
#include "hi_drv_mem.h"
#include "hi_osal.h"
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#define MAX_WIN_ATTR_NUM 64
#define ALG_RATIO_PIX_RATIO1 1024
#define MAX_RATIO_RANGE 256
#define VDP_HEIGHT_ALIGN 0xffffffff
#define VDP_WIDTH_ALIGN 0xffffffff
#define WIN_INRECT_MIN_WIDTH 64
#define WIN_INRECT_MIN_HEIGHT 64
#define MIN_WINDOW_NUM 64
#define AFD_LENGTH 10000

#ifdef LOCAL_TEST
#define WIN_ERROR(fmt...)    \
    do { \
    } while (0)
#endif

#ifndef LOCAL_TEST
typedef struct osal_list_head win_attr_list;
typedef osal_spinlock win_attr_spin;
typedef struct {
    win_attr_list node;
    hi_handle win_attr_handle;
    drv_win_attr_cfg attr_cfg;
    drv_win_attr_out_config out_cfg;
    hi_bool calculated;
} win_attr_node;

typedef struct {
    hi_bool b_win_attr_init;
    hi_u32 win_attr_num;
    win_attr_spin win_attr_spin_lock;
    win_attr_list win_attr_node_list;
} win_attr_context;
#endif

typedef struct {
    hi_s64 ow;
    hi_s64 ox;
    hi_s64 iw;
    hi_s64 ix;
    hi_s64 valid_width;
    hi_s32 disp_flag;
}win_attr_io_x_width;

typedef struct {
    hi_s64 oy;
    hi_s64 oh;
    hi_s64 iy;
    hi_s64 ih;
    hi_s64 valid_height;
    hi_s32 disp_flag;
}win_attr_io_y_height;

typedef struct{
    hi_u64 aspect_width;
    hi_u64 aspect_height;
    hi_u64 device_width;
    hi_u64 device_height;
    hi_drv_asp_rat_mode e_asp_mode;
    hi_rect in_wnd;
    hi_rect src_rect;
    hi_rect out_wnd;
    hi_rect screen_rect;
}win_attr_algratio_para;

typedef struct {
    hi_rect crop_wnd;
    hi_rect out_wnd;
    hi_rect out_scren_rect;
    hi_u64 zme_h;
    hi_u64 zme_w;
    hi_bool en_asp;
    hi_bool en_crop;
    hi_u64 bg_color;
    hi_u64 bg_alpha;
}win_attr_algratio_outpara;

typedef struct {
    hi_u64                      crop_left;
    hi_u64                      crop_right;
    hi_u64                      crop_top;
    hi_u64                      crop_bottom;
} win_attr_afd_src_crop;

typedef struct {
    hi_u64                      shoot_protect_left;
    hi_u64                      shoot_protect_right;
    hi_u64                      shoot_protect_top;
    hi_u64                      shoot_protect_bottom;

    hi_u64                      add_letter_box_left;
    hi_u64                      add_letter_box_right;
    hi_u64                      add_letter_box_top;
    hi_u64                      add_letter_box_bottom;
} win_attr_afd_out;

typedef struct {
    hi_drv_video_afd_type     e_afd;
    win_attr_afd_src_crop         src_crop_info;
    win_attr_afd_out              out_info4_3; // Out 4:3
    win_attr_afd_out              out_info16_9; //  Out 16:9
} win_attr_afd;

static const win_attr_afd g_input4x3_afd_configuration[] = {
    {
        HI_DRV_AFD_TYPE_SAME_AS_SRC,
        .src_crop_info = {0, 0, 0, 0, },
        .out_info4_3 =
        {   // Out4x3
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        .out_info16_9 =
        {   // Out16x9
            0, 0, 0, 0,
            1250, 1250, 0, 0,
        },
    },
    {
        HI_DRV_AFD_TYPE_4_3,
        .src_crop_info = {0, 0, 0, 0, },
        .out_info4_3 =
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        .out_info16_9 =
        {
            0, 0, 0, 0,
            1250, 1250, 0, 0,
        },
    },
    {
        HI_DRV_AFD_TYPE_16_9,
        .src_crop_info = {0, 0, 1250, 1250, },
        .out_info4_3 =
        {
            0, 0, 0, 0,
            0, 0, 1250, 1250,
        },
        .out_info16_9 =
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
    },
    {
        HI_DRV_AFD_TYPE_14_9,
        .src_crop_info = {0, 0, 625, 625, },
        .out_info4_3 =
        {
            0, 0, 0, 0,
            0, 0, 625, 625,
        },
        .out_info16_9 =
        {
            0, 0, 0, 0,
            625, 625, 0, 0,
        },
    },
    {
        12,
        .src_crop_info = {0, 0, 0, 0, },
        .out_info4_3 =
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        .out_info16_9 =
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
    },
    {
        HI_DRV_AFD_TYPE_4_3_SHOOT_PRO_14_9,
        .src_crop_info = {0, 0, 0, 0, },
        .out_info4_3 =
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        .out_info16_9 =
        {
            0, 0, 625, 625,
            625, 625, 0, 0,
        },
    },
    {
        HI_DRV_AFD_TYPE_16_9_SHOOT_PRO_14_9,
        .src_crop_info = {0, 0, 1250, 1250, },
        .out_info4_3 =
        {
            0, 0, 0, 0,
            0, 0, 1250, 1250,
        },
        .out_info16_9 =
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
    },
    {
        HI_DRV_AFD_TYPE_16_9_SHOOT_PRO_4_3,
        .src_crop_info = {0, 0, 1250, 1250, },
        .out_info4_3 =
        {
            1250, 1250, 0, 0,
            0, 0, 0, 0,
        },
        .out_info16_9 =
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
    },
};

static const win_attr_afd g_input16x9_afd_configuration[] = {
    {
        HI_DRV_AFD_TYPE_SAME_AS_SRC, // 0x0
        .src_crop_info = {0, 0, 0, 0, },
        .out_info4_3 =
        {
            0, 0, 0, 0,
            0, 0, 1250, 1250,
        },
        .out_info16_9 =
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
    },
    {
        HI_DRV_AFD_TYPE_4_3, // 0x1
        .src_crop_info = {1250, 1250, 0, 0, },
        .out_info4_3 =
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        .out_info16_9 =
        {
            0, 0, 0, 0,
            1250, 1250, 0, 0,
        },
    },
    {
        HI_DRV_AFD_TYPE_16_9,
        .src_crop_info = {0, 0, 0, 0, },
        .out_info4_3 =
        {
            0, 0, 0, 0,
            0, 0, 1250, 1250,
        },
        .out_info16_9 =
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
    },
    {
        HI_DRV_AFD_TYPE_14_9,
        .src_crop_info = {625, 625, 0, 0, },
        .out_info4_3 =
        {
            0, 0, 0, 0,
            0, 0, 625, 625,
        },
        .out_info16_9 =
        {
            0, 0, 0, 0,
            625, 625, 0, 0,
        },
    },
    {
        12,
        .src_crop_info = {0, 0, 0, 0, },
        .out_info4_3 =
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        .out_info16_9 =
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
    },
    {
        HI_DRV_AFD_TYPE_4_3_SHOOT_PRO_14_9, // 0x5
        .src_crop_info = {1250, 1250, 0, 0, },
        .out_info4_3 =
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        .out_info16_9 =
        {
            0, 0, 625, 625,
            625, 625, 0, 0,
        },
    },
    {
        HI_DRV_AFD_TYPE_16_9_SHOOT_PRO_14_9,
        .src_crop_info = {0, 0, 0, 0, },
        .out_info4_3 =
        {
            625, 625, 0, 0,
            0, 0, 625, 625,
        },
        .out_info16_9 =
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
    },
    {
        HI_DRV_AFD_TYPE_16_9_SHOOT_PRO_4_3,
        .src_crop_info = {0, 0, 0, 0, },
        .out_info4_3 =
        {
            1250, 1250, 0, 0,
            0, 0, 0, 0,
        },
        .out_info16_9 =
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
    },
};

#ifndef LOCAL_TEST
static win_attr_context g_win_attr_context;

hi_s32 drv_win_attrprocess_init(void)
{
    hi_s32 ret;
    // load ko
    if (g_win_attr_context.b_win_attr_init == HI_TRUE) {
        WIN_ERROR("win attr has been inited, init flag is %d\n", g_win_attr_context.b_win_attr_init);
        return HI_SUCCESS;
    }

    ret = memset_s(&g_win_attr_context, sizeof(win_attr_context), 0, sizeof(win_attr_context));
    if (ret != EOK) {
        WIN_ERROR("memset_s fail in win_attrprocess_init!\n");
        return HI_FAILURE;
    }
    OSAL_INIT_LIST_HEAD(&(g_win_attr_context.win_attr_node_list));
    ret = osal_spin_lock_init(&(g_win_attr_context.win_attr_spin_lock));
    if (ret != HI_SUCCESS) {
        WIN_ERROR("win attr spin_lock initiation failed!\n");
        return HI_FAILURE;
    }
    g_win_attr_context.b_win_attr_init = HI_TRUE;
    return HI_SUCCESS;
}

static win_attr_node *win_attr_get_node(win_attr_list *win_list, hi_handle attr_handle)
{
    win_attr_list *pos = HI_NULL;
    win_attr_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    win_attr_node *win_node = HI_NULL;

    osal_list_for_each_safe(pos, n, win_list)
    {
        win_node = osal_list_entry(pos, win_attr_node, node);
        list_entry_times++;
        if (list_entry_times > MAX_WIN_ATTR_NUM) {
            WIN_ERROR("list error\n");
            win_node = HI_NULL;
            break;
        }
        if (win_node->win_attr_handle == attr_handle) {
            break;
        } else {
            win_node = HI_NULL;
        }
    }

    return win_node;
}

static hi_bool win_attr_check_handle_free(win_attr_list *win_attr_node_list, hi_handle win_attr_handle)
{
    win_attr_list *pos = HI_NULL;
    win_attr_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    hi_bool check_result = HI_TRUE;
    win_attr_node *win_node = HI_NULL;

    osal_list_for_each_safe(pos, n, win_attr_node_list)
    {
        win_node = osal_list_entry(pos, win_attr_node, node);
        list_entry_times++;
        if (list_entry_times > MAX_WIN_ATTR_NUM) {
            WIN_ERROR("list error\n");
            check_result = HI_FALSE;
            break;
        }

        if (win_node->win_attr_handle == win_attr_handle) {
            check_result = HI_FALSE;
            break;
        }
    }

    return check_result;
}

hi_handle win_attr_get_free_handle(void)
{
    hi_u32 try_get_count = 0;
    hi_handle win_attr_handle;
    unsigned long flags;

    osal_spin_lock_irqsave(&g_win_attr_context.win_attr_spin_lock, &flags);
    while (1) {
        g_win_attr_context.win_attr_num++;
        win_attr_handle = g_win_attr_context.win_attr_num;
        try_get_count++;
        if (win_attr_check_handle_free(
                &(g_win_attr_context.win_attr_node_list), g_win_attr_context.win_attr_num) == HI_TRUE) {
            break;
        }
        if (try_get_count > MAX_WIN_ATTR_NUM) {
            break;
        }
    }
    osal_spin_unlock_irqrestore(&g_win_attr_context.win_attr_spin_lock, &flags);
    if (try_get_count > MAX_WIN_ATTR_NUM) {
        WIN_ERROR("try %d times, can not find valid handle\n", try_get_count);
        return HI_INVALID_HANDLE;
    } else {
        return win_attr_handle;
    }
}

hi_handle drv_win_attrprocess_create(void)
{
    win_attr_node *win_node = HI_NULL;
    hi_handle win_attr_handle;
    hi_s32 ret;
    unsigned long flags;

    if (g_win_attr_context.b_win_attr_init != HI_TRUE) {
        WIN_ERROR("can not create attr process when not init!!!!!!\n");
        return HI_INVALID_HANDLE;
    }

    win_attr_handle = win_attr_get_free_handle();
    if (win_attr_handle == HI_INVALID_HANDLE) {
        return HI_INVALID_HANDLE;
    }

    win_node = (win_attr_node *)osal_kmalloc(HI_ID_WIN, sizeof(win_attr_node), OSAL_GFP_KERNEL);
    if (win_node == HI_NULL) {
        WIN_ERROR("win attr node malloc failed\n");
        return HI_INVALID_HANDLE;
    }

    ret = memset_s(win_node, sizeof(win_attr_node), 0, sizeof(win_attr_node));
    if (ret != EOK) {
        WIN_ERROR("win attr node memset_s failed\n");
        osal_kfree(HI_ID_WIN, win_node);
        win_node = HI_NULL;
        return HI_INVALID_HANDLE;
    }
    osal_spin_lock_irqsave(&g_win_attr_context.win_attr_spin_lock, &flags);
    osal_list_add_tail(&(win_node->node), &(g_win_attr_context.win_attr_node_list));
    win_node->win_attr_handle = win_attr_handle;
    osal_spin_unlock_irqrestore(&g_win_attr_context.win_attr_spin_lock, &flags);

    return win_node->win_attr_handle;
}

hi_s32 drv_win_attrprocess_destory(hi_handle h_win_attr)
{
    unsigned long flags;
    win_attr_list *pos = HI_NULL;
    win_attr_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    hi_s32 ret = HI_FAILURE;
    win_attr_node *win_node = HI_NULL;

    if (g_win_attr_context.b_win_attr_init != HI_TRUE) {
        WIN_ERROR("can not destroy win attr when not init!!!!!!\n");
        return HI_FAILURE;
    }

    osal_spin_lock_irqsave(&g_win_attr_context.win_attr_spin_lock, &flags);
    osal_list_for_each_safe(pos, n, &(g_win_attr_context.win_attr_node_list))
    {
        win_node = osal_list_entry(pos, win_attr_node, node);
        list_entry_times++;
        if (list_entry_times > MAX_WIN_ATTR_NUM) {
            WIN_ERROR("list error\n");
            ret = HI_FAILURE;
            break;
        }
        if (win_node->win_attr_handle == h_win_attr) {
            osal_list_del_init(pos);
            ret = HI_SUCCESS;
            break;
        }
    }
    osal_spin_unlock_irqrestore(&g_win_attr_context.win_attr_spin_lock, &flags);

    if (ret == HI_SUCCESS) {
        ret = memset_s(win_node, sizeof(win_attr_node), 0, sizeof(win_attr_node));
        if (ret != EOK) {
            WIN_ERROR("memset_s fail in win_attrprocess_destory!\n");
            osal_kfree(HI_ID_WIN, win_node);
            win_node = HI_NULL;
            return HI_FAILURE;
        }
        osal_kfree(HI_ID_WIN, win_node);
        return HI_SUCCESS;
    } else {
        WIN_ERROR("destroy win_attr error!!!!!!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 win_attr_list_deinit(win_attr_list *win_attr_node_list)
{
    win_attr_list *pos = HI_NULL;
    win_attr_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    hi_s32 ret;
    win_attr_node *win_node = HI_NULL;

    ret = HI_SUCCESS;
    osal_list_for_each_safe(pos, n, win_attr_node_list)
    {
        win_node = osal_list_entry(pos, win_attr_node, node);
        list_entry_times++;
        if (list_entry_times > MAX_WIN_ATTR_NUM) {
            WIN_ERROR("list error\n");
            ret = HI_FAILURE;
            break;
        }

        WIN_ERROR("win attr %d destroy error!!!!!\n", win_node->win_attr_handle);
        osal_list_del_init(pos);
        osal_kfree(HI_ID_WIN, win_node);
        ret = HI_FAILURE;
    }

    return ret;
}

hi_s32 drv_win_attrprocess_deinit(void)
{
    hi_s32 ret;

    if (g_win_attr_context.b_win_attr_init != HI_TRUE) {
        WIN_ERROR("please init win_attr first, init flag is %d\n", g_win_attr_context.b_win_attr_init);
        return HI_FAILURE;
    }

    ret = win_attr_list_deinit(&(g_win_attr_context.win_attr_node_list));
    g_win_attr_context.b_win_attr_init = HI_FALSE;
    osal_spin_lock_destory(&g_win_attr_context.win_attr_spin_lock);
    return ret;
}
#endif

static hi_void win_attr_win_revise_in_rect(hi_rect *p_dst_rect, hi_s64 src_width, hi_s64 src_height)
{
    if ((p_dst_rect->width == 0) || (p_dst_rect->height == 0)) {
        p_dst_rect->width = src_width;
        p_dst_rect->height = src_height;
    }

    if (p_dst_rect->width > src_width)
        p_dst_rect->width =  src_width;

    if (p_dst_rect->height > src_height)
        p_dst_rect->height =  src_height;

    if (p_dst_rect->y > src_height - MIN_WINDOW_NUM)
        p_dst_rect->y =    src_height - MIN_WINDOW_NUM;

    if (p_dst_rect->x > src_width - MIN_WINDOW_NUM)
        p_dst_rect->x =  src_width - MIN_WINDOW_NUM;

    if ((p_dst_rect->y + p_dst_rect->height) > src_height)
        p_dst_rect->height = src_height - p_dst_rect->y;

    if ((p_dst_rect->x + p_dst_rect->width) > src_width)
        p_dst_rect->width = src_width - p_dst_rect->x;

    if (p_dst_rect->width < MIN_WINDOW_NUM)
        p_dst_rect->width = MIN_WINDOW_NUM;

    if (p_dst_rect->height < MIN_WINDOW_NUM)
        p_dst_rect->height = MIN_WINDOW_NUM;
}

static hi_void win_attr_adjust_video_out_rect(hi_rect *p_disp_rect,
    hi_rect *p_video_out_rect,
    hi_rect *actual_resolution)
{
    hi_rect active_rect = {0};

    active_rect.x = actual_resolution->x;
    active_rect.y = actual_resolution->y;
    active_rect.width = actual_resolution->width;
    active_rect.height = actual_resolution->height;

    if ((p_disp_rect->x + p_disp_rect->width) < (active_rect.x + active_rect.width)) {
        active_rect.x = p_disp_rect->x;
        active_rect.width = p_disp_rect->width;
    }

    if ((p_disp_rect->y + p_disp_rect->height) < (active_rect.y + active_rect.height)) {
        active_rect.y = p_disp_rect->y;
        active_rect.height = p_disp_rect->height;
    }

    if (p_disp_rect->x > p_video_out_rect->x)
        p_disp_rect->x = p_video_out_rect->x;

    if (p_disp_rect->y > p_video_out_rect->y)
        p_disp_rect->y = p_video_out_rect->y;

    if (p_disp_rect->width < p_video_out_rect->width)
        p_disp_rect->width = p_video_out_rect->width;

    if (p_disp_rect->height < p_video_out_rect->height)
        p_disp_rect->height = p_video_out_rect->height;

    return;
}

static hi_void win_attr_config_rect(hi_rect *rect, hi_s64 x, hi_s64 y, hi_s64 width, hi_s64 height)
{
    rect->x = x;
    rect->y = y;
    rect->width = width;
    rect->height = height;
}

static hi_void win_attr_config_rect_param(hi_rect rect, hi_s64 *x, hi_s64 *y, hi_s64 *width, hi_s64 *height)
{
    *x = rect.x;
    *y = rect.y;
    *width = rect.width;
    *height = rect.height;
}

static hi_void win_attr_cal_actual_resolution(hi_rect *actual_resolution, hi_rect *screen, hi_drv_disp_offset *offset)
{
    actual_resolution->x = 0;
    actual_resolution->y = 0;

    actual_resolution->width = screen->width - offset->left - offset->right;
    actual_resolution->height = screen->height - offset->top - offset->bottom;

    if (actual_resolution->width < MIN_WINDOW_NUM)
        actual_resolution->width = MIN_WINDOW_NUM;

    if (actual_resolution->height < MIN_WINDOW_NUM)
        actual_resolution->height = MIN_WINDOW_NUM;
}

static hi_void win_attr_cal_video_offset(hi_rect *video_rect_offset, hi_rect disp_rect, hi_rect video_rect)
{
    video_rect_offset->x = disp_rect.x - video_rect.x ;
    video_rect_offset->y = disp_rect.y - video_rect.y;
    video_rect_offset->width = disp_rect.width - video_rect.width;
    video_rect_offset->height = disp_rect.height - video_rect.height;
}

static hi_void win_attr_cal_io_wx(win_attr_io_x_width *io,
    hi_rect video_rect,
    hi_rect in_rect,
    hi_rect actual_resolution)
{
    if (video_rect.x < 0) {
        io->valid_width = video_rect.width  + video_rect.x;

        if (io->valid_width < WIN_INRECT_MIN_WIDTH) {
            io->ow = WIN_INRECT_MIN_WIDTH;
            io->ox = 0;
            io->iw = in_rect.width * io->ow / video_rect.width;
            io->ix += in_rect.width - io->iw;

            io->disp_flag = HI_TRUE;
        } else {
            // >screen size
            io->ow = actual_resolution.width;
            if (io->valid_width < io->ow)
                io->ow = io->valid_width;

            io->ox = 0;
            io->iw = in_rect.width * io->ow / video_rect.width;
            io->ix += in_rect.width - in_rect.width * io->valid_width / video_rect.width;
        }
    }
    else if (video_rect.x < (actual_resolution.width - WIN_INRECT_MIN_WIDTH)) {
        io->valid_width = video_rect.width;

        if (io->valid_width < WIN_INRECT_MIN_WIDTH) {
            io->ow = WIN_INRECT_MIN_WIDTH;
            io->ox = video_rect.x;
            io->iw = in_rect.width;
            io->ix += 0;
            io->disp_flag = HI_TRUE;
        } else {
            io->ox = video_rect.x;
            io->ix += 0;

            if ((io->valid_width + io->ox) > actual_resolution.width) {
                io->ow = actual_resolution.width - video_rect.x;
                io->iw = in_rect.width * io->ow / video_rect.width;
            } else {
                io->ow = video_rect.width;
                io->iw = in_rect.width;
            }
        }
    }else {
        io->valid_width = actual_resolution.width - video_rect.x;
        io->ow = WIN_INRECT_MIN_WIDTH;
        io->ox = actual_resolution.width  - io->ow;
        io->iw = in_rect.width * io->ow / video_rect.width;
        io->ix += 0;
        io->disp_flag = HI_TRUE;
    }
}

static hi_void win_attr_cal_io_yh(win_attr_io_y_height *io,
    hi_rect video_rect,
    hi_rect in_rect,
    hi_rect actual_resolution)
{
    if (video_rect.y < 0) {
        io->valid_height = video_rect.height + video_rect.y;
        if (io->valid_height < WIN_INRECT_MIN_HEIGHT) {
            io->oy = 0;
            io->oh = WIN_INRECT_MIN_HEIGHT;
            io->ih = in_rect.height * io->oh / video_rect.height;
            io->iy += in_rect.height - io->ih;
            io->disp_flag = HI_TRUE;
        } else {
            io->oy = 0;
            io->oh = actual_resolution.height;
            if (io->valid_height < io->oh)
                io->oh = io->valid_height;

            io->ih = in_rect.height * io->oh / video_rect.height;
            io->iy += in_rect.height - in_rect.height * io->valid_height / video_rect.height;
        }
    } else if (video_rect.y < (actual_resolution.height - WIN_INRECT_MIN_HEIGHT)) {
        io->valid_height = video_rect.height;

        if (io->valid_height < WIN_INRECT_MIN_HEIGHT) {
            io->oy = video_rect.y;
            io->oh = WIN_INRECT_MIN_HEIGHT;
            io->ih = in_rect.height;
            io->iy += 0;
            io->disp_flag = HI_TRUE;
        } else {
            io->oy = video_rect.y;
            io->iy += 0;
            if ((io->valid_height + io->oy) > actual_resolution.height) {
                io->oh = actual_resolution.height - video_rect.y;
                io->ih = in_rect.height * io->oh / video_rect.height;
            } else {
                io->oh = video_rect.height;
                io->ih = in_rect.height;
            }
        }
    }else {
        io->valid_height = actual_resolution.height - video_rect.y;
        io->oh = WIN_INRECT_MIN_HEIGHT;
        io->oy = actual_resolution.height - io->oh;
        io->ih = in_rect.height * io->oh / video_rect.height;
        io->iy += 0;
        io->disp_flag = HI_TRUE;
    }
}

static hi_void win_attr_set_io_wx(win_attr_io_x_width io, hi_s64 *ow, hi_s64 *ox, hi_s64 *iw, hi_s64 *ix)
{
    *ow = io.ow;
    *ox = io.ox;
    *iw = io.iw;
    *ix = io.ix;
}

static hi_void win_attr_set_io_yh(win_attr_io_y_height io, hi_s64 *oh, hi_s64 *oy, hi_s64 *ih, hi_s64 *iy)
{
    *oh = io.oh;
    *oy = io.oy;
    *ih = io.ih;
    *iy = io.iy;
}

static hi_void win_attr_revise_video_rect_offset(hi_rect *video_rect_offset, hi_s64 valid_width)
{
    if (valid_width < WIN_INRECT_MIN_WIDTH) {
        video_rect_offset->x =
            video_rect_offset->x + (WIN_INRECT_MIN_WIDTH - valid_width);
        video_rect_offset->width =
            video_rect_offset->width - ((WIN_INRECT_MIN_WIDTH - valid_width) * 2); // 2 is index
    }
}

static hi_void win_attr_revise_video_rect_offset_height(hi_rect *video_rect_offset, hi_s64 valid_height)
{
    if (valid_height < WIN_INRECT_MIN_HEIGHT) {
        video_rect_offset->y =
            video_rect_offset->y + (WIN_INRECT_MIN_HEIGHT - valid_height);
        video_rect_offset->height =
            video_rect_offset->height - ((WIN_INRECT_MIN_HEIGHT - valid_height) * 2); // 2 is index
    }
}

static hi_void win_attr_config_xw(win_attr_io_x_width *io, hi_s64 ix, hi_s64 iw)
{
    io->ix = ix;
    io->iw = iw;
}

static hi_void win_attr_config_yh(win_attr_io_y_height *io, hi_s64 iy, hi_s64 ih)
{
    io->iy = iy;
    io->ih = ih;
}

static hi_void win_attr_revise_disp_rect(hi_rect *p_disp_rect)
{
    if (p_disp_rect->x < 0) {
        p_disp_rect->width += p_disp_rect->x;
        p_disp_rect->x = 0;
    }

    if (p_disp_rect->y < 0) {
        p_disp_rect->height += p_disp_rect->y;
        p_disp_rect->y = 0;
    }
}

static hi_s32 win_attr_revise_outofscreenwin_outrect(drv_win_attr_out_config *out_cfg, hi_rect *p_screen_rect,
                                                     hi_drv_disp_offset *p_offset_rect)
{
    hi_rect in_rect = out_cfg->src_crop_rect;
    hi_rect video_rect = out_cfg->win_video_rect;
    hi_rect disp_rect = out_cfg->win_out_rect;
    hi_rect out_rect = {0};
    hi_rect video_rect_offset;
    hi_rect actual_resolution = {0};
    hi_s64 ix, iy, iw, ih, ox, oy, ow, oh;
    hi_bool disp_flag;
    win_attr_io_x_width out_xw = {0};
    win_attr_io_y_height out_yh = {0};

    if (video_rect.width == 0 || video_rect.height == 0) {
        WIN_ERROR("Denominator may be zero !\n");
        out_cfg->is_smaller_than_minimum = HI_TRUE;
        return HI_FAILURE;
    }

    win_attr_config_rect_param(out_cfg->src_crop_rect, &ix, &iy, &iw, &ih);
    win_attr_config_xw(&out_xw, ix, iw);
    win_attr_config_yh(&out_yh, iy, ih);
    win_attr_cal_video_offset(&video_rect_offset, disp_rect, video_rect);
    win_attr_cal_actual_resolution(&actual_resolution, p_screen_rect, p_offset_rect);

    /* stOutRect considered the offset already, so no need to - left. */
    video_rect.x  -= p_offset_rect->left;
    video_rect.y  -= p_offset_rect->top;

    win_attr_cal_io_wx(&out_xw, video_rect, in_rect, actual_resolution);
    win_attr_set_io_wx(out_xw, &ow, &ox, &iw, &ix);
    win_attr_revise_video_rect_offset(&video_rect_offset, out_xw.valid_width);
    disp_flag = out_xw.disp_flag;

    win_attr_cal_io_yh(&out_yh, video_rect, in_rect, actual_resolution);
    win_attr_set_io_yh(out_yh, &oh, &oy, &ih, &iy);
    win_attr_revise_video_rect_offset_height(&video_rect_offset, out_yh.valid_height);
    disp_flag = out_yh.disp_flag;

    ox  += p_offset_rect->left;
    oy  += p_offset_rect->top;

    /* pstInRect in fact is the output of vpss(zme,dei,crop dnr)
     * it does not confirm to  the align limit of inrect.
     * If we give a wrong use, and make a wrong align,
     * there will be no consistency bettween GFX and VIDEO, and the backgroud color will appears.
     */
    out_cfg->win_out_rect.x = ox + video_rect_offset.x;
    out_cfg->win_out_rect.y = oy + video_rect_offset.y;
    out_cfg->win_out_rect.width = ow + video_rect_offset.width;
    out_cfg->win_out_rect.height = oh + video_rect_offset.height;
    win_attr_revise_disp_rect(&out_cfg->win_out_rect);

    win_attr_config_rect(&out_cfg->src_crop_rect, ix, iy, iw, ih);
    win_attr_config_rect(&out_rect, ox, oy, ow, oh);

    win_attr_adjust_video_out_rect(&out_cfg->win_out_rect, &out_rect, &actual_resolution);

    out_cfg->win_video_rect = out_rect;
    out_cfg->is_smaller_than_minimum = disp_flag;

    return HI_SUCCESS;
}

static hi_void win_config_in_rect(const drv_win_attr_cfg *parameter, hi_rect *in_rect)
{
    *in_rect = parameter->src_crop_rect;
    win_attr_win_revise_in_rect(in_rect, parameter->src_width, parameter->src_height);
}

static hi_void win_attr_init_drv_para(
                drv_win_attr_cfg attr_cfg, win_attr_algratio_para *p_drv_para, drv_win_attr_out_config *out_cfg)
{
    /* need VDP AspRatio process */
    /* out out win Rect */
    p_drv_para->aspect_width = attr_cfg.src_asp_ratio.aspect_ratio_w;
    p_drv_para->aspect_height = attr_cfg.src_asp_ratio.aspect_ratio_h;
    p_drv_para->device_height = attr_cfg.out_device_asp_ratio.aspect_ratio_h;
    p_drv_para->device_width = attr_cfg.out_device_asp_ratio.aspect_ratio_w;
    p_drv_para->e_asp_mode = attr_cfg.win_out_asp_mode;

    p_drv_para->src_rect.x = 0;
    p_drv_para->src_rect.y = 0;
    p_drv_para->src_rect.width = attr_cfg.src_width;
    p_drv_para->src_rect.height = attr_cfg.src_height;
    p_drv_para->in_wnd = out_cfg->src_crop_rect;
    p_drv_para->out_wnd = attr_cfg.win_out_rect;
    p_drv_para->screen_rect = attr_cfg.out_physics_screen;

    if ((p_drv_para->out_wnd.width == 0) || (p_drv_para->out_wnd.height == 0)) {
        p_drv_para->out_wnd.width = p_drv_para->screen_rect.width;
        p_drv_para->out_wnd.height = p_drv_para->screen_rect.height;
    }
}

static hi_void win_attr_config_drv_para_out(
                            win_attr_algratio_para *p_drv_para,
                            hi_s64 left,
                            hi_s64 top,
                            hi_s64 right,
                            hi_s64 bottom)
{
    p_drv_para->out_wnd.x = left;
    p_drv_para->out_wnd.y = top;
    p_drv_para->out_wnd.width = p_drv_para->out_wnd.width - (left + right);
    p_drv_para->out_wnd.height = p_drv_para->out_wnd.height - (top + bottom);
}

static hi_void win_attr_config_drv_para_in(
                            win_attr_algratio_para *p_drv_para,
                            hi_s64 left,
                            hi_s64 top,
                            hi_s64 right,
                            hi_s64 bottom)
{
    p_drv_para->in_wnd.x = left;
    p_drv_para->in_wnd.y = top;
    p_drv_para->in_wnd.width = p_drv_para->in_wnd.width - (left + right);
    p_drv_para->in_wnd.height = p_drv_para->in_wnd.height - (top + bottom);
}

static void win_attr_afd_process(
                           hi_u64 afd_val,
                           win_attr_algratio_para *p_drv_para,
                           win_attr_algratio_outpara *p_out_para)
{
    const win_attr_afd *p_afd_configuration = HI_NULL;
    const win_attr_afd_out *p_out_info = HI_NULL;
    hi_u64 top, bottom, left, right;

    afd_val &= 0x7; // Low 3-bit value is valid

    if ((p_drv_para->aspect_width == 4) && (p_drv_para->aspect_height == 3)) { // 4 and 3 is aspect ratio
        p_afd_configuration = &g_input4x3_afd_configuration[afd_val];
    }
    else {
        p_afd_configuration = &g_input16x9_afd_configuration[afd_val];
    }

    if ((p_drv_para->device_width == 4) && (p_drv_para->device_height == 3)) { // 4 and 3 is aspect ratio
        p_out_info = &p_afd_configuration->out_info4_3;
    }
    else { // if (p_drv_para->device_width == 16 && p_drv_para->device_height == 9) // Out 16:9
        p_out_info = &p_afd_configuration->out_info16_9;
    }

    // Step 1: Source Image Crop
    left = p_drv_para->in_wnd.width * p_afd_configuration->src_crop_info.crop_left / AFD_LENGTH;
    right = p_drv_para->in_wnd.width * p_afd_configuration->src_crop_info.crop_right / AFD_LENGTH;
    top = p_drv_para->in_wnd.height * p_afd_configuration->src_crop_info.crop_top / AFD_LENGTH;
    bottom = p_drv_para->in_wnd.height * p_afd_configuration->src_crop_info.crop_bottom / AFD_LENGTH;

    win_attr_config_drv_para_in(p_drv_para, left, top, right, bottom);
    // Step 2: Shoot & Protect
    if ((p_out_info->shoot_protect_left != 0) || (p_out_info->shoot_protect_right != 0)) {
        left = p_drv_para->in_wnd.width * p_out_info->shoot_protect_left / AFD_LENGTH;
        right = p_drv_para->in_wnd.width * p_out_info->shoot_protect_right / AFD_LENGTH;

        p_drv_para->in_wnd.x = left;
        p_drv_para->in_wnd.width = (0 == left) ? p_drv_para->in_wnd.width
                                       : (p_drv_para->in_wnd.width - (left + right));
    }

    if ((p_out_info->shoot_protect_top != 0) || (p_out_info->shoot_protect_bottom != 0)) {
        top = p_drv_para->in_wnd.height * p_out_info->shoot_protect_top / AFD_LENGTH;
        bottom = p_drv_para->in_wnd.height * p_out_info->shoot_protect_bottom / AFD_LENGTH;

        p_drv_para->in_wnd.y = top;
        p_drv_para->in_wnd.height = (top == 0) ? p_drv_para->in_wnd.height
                                        : (p_drv_para->in_wnd.height - (top + bottom));
    }

    // step 3: add LetterBox Out
    left = p_drv_para->out_wnd.width * p_out_info->add_letter_box_left / AFD_LENGTH;
    right = p_drv_para->out_wnd.width * p_out_info->add_letter_box_right / AFD_LENGTH;
    top = p_drv_para->out_wnd.height * p_out_info->add_letter_box_top / AFD_LENGTH;
    bottom = p_drv_para->out_wnd.height * p_out_info->add_letter_box_bottom / AFD_LENGTH;

    win_attr_config_drv_para_out(p_drv_para, left, top, right, bottom);
    p_out_para->out_wnd = p_drv_para->out_wnd;

    return;
}

static hi_void win_attr_ratio_correct_aspectratio(hi_u64 *pw, hi_u64 *ph)
{
    hi_u64 w, h;
    hi_u64 ratio_range = 1;

    w = *pw;
    h = *ph;

    // we only support to 256:1 or 1:256
    if (w != 0 && h != 0) {
        ratio_range = ((w * ALG_RATIO_PIX_RATIO1) / h);
        if (ratio_range > MAX_RATIO_RANGE * ALG_RATIO_PIX_RATIO1) {
            w = MAX_RATIO_RANGE;
            h = 1;
        }

        if (ratio_range < ALG_RATIO_PIX_RATIO1 / MAX_RATIO_RANGE) {
            w = 1;
            h = MAX_RATIO_RANGE;
        }
    }

    while ((w > MAX_RATIO_RANGE) || (h > MAX_RATIO_RANGE)) {
        w >>= 1;
        h >>= 1;
    }

    if (!w)
        w = 1;
    if (!h)
        h = 1;

    *pw = w;
    *ph  = h;
}

hi_void win_attr_alg_ratio_letterbox(hi_u64 aspect_ratio_w, hi_u64 aspect_ratio_h, hi_rect *p_out_wnd, hi_s64 pixr1_out)
{
    hi_u64 v_w, v_h;
    hi_u64 ar_w = aspect_ratio_w;
    hi_u64 ar_h = aspect_ratio_h;

    if (ar_h == 0 || pixr1_out == 0) {
        WIN_ERROR("Denominator may be zero !\n");
        return;
    }

    v_w = (ar_w * p_out_wnd->height * ALG_RATIO_PIX_RATIO1) / (ar_h * pixr1_out);

    if (v_w <= p_out_wnd->width)  {
        p_out_wnd->x = (p_out_wnd->x + (p_out_wnd->width - v_w) / 2) ; // 2 is index
        p_out_wnd->width = v_w;
    } else {
        if (v_w == 0) {
            WIN_ERROR("Denominator may be zero !\n");
            return;
        }
        v_h = p_out_wnd->height * p_out_wnd->width / v_w;

        p_out_wnd->y = (p_out_wnd->y + (p_out_wnd->height - v_h) / 2); // 2 is index
        p_out_wnd->height = v_h;
    }
}

static hi_void win_attr_generate_wh(win_attr_algratio_para *p_drv_para,
    hi_s64 *ar_w, hi_s64 *ar_h, hi_s64 *sr_w, hi_s64 *sr_h)
{
    *ar_w = 0;
    *ar_h = 0;
    if ((*ar_w == 0) || (*ar_h == 0)) {
        // using ar_w and ar_h replace video output window ratio
        *ar_w = p_drv_para->aspect_width;
        *ar_h = p_drv_para->aspect_height;
        // HI_UNF_DISP_ASPECT_RATIO_AUTO&Square 0/0
        if ((*ar_w == 0) || (*ar_h == 0)) {
            *ar_w = p_drv_para->in_wnd.width;
            *ar_h = p_drv_para->in_wnd.height;
        }
    }
    win_attr_ratio_correct_aspectratio(ar_w, ar_h);

    // using sr_w and sr_h replace screen ratio
    *sr_w = p_drv_para->device_width;
    *sr_h = p_drv_para->device_height;
    if ((*sr_w == 0) || (*sr_h == 0)) {
        *sr_w = p_drv_para->screen_rect.width;
        *sr_h = p_drv_para->screen_rect.height;
    }
    win_attr_ratio_correct_aspectratio(sr_w, sr_h);
}

static hi_void win_attr_config_out_para(win_attr_algratio_outpara *p_out_para,
    win_attr_algratio_para *p_drv_para)
{
    p_out_para->zme_w = p_drv_para->out_wnd.width;
    p_out_para->zme_h = p_drv_para->out_wnd.height;

    p_out_para->out_wnd.height = p_drv_para->out_wnd.height;
    p_out_para->out_wnd.width = p_drv_para->out_wnd.width;
    p_out_para->out_wnd.x = p_drv_para->out_wnd.x;
    p_out_para->out_wnd.y = p_drv_para->out_wnd.y;

    p_out_para->out_scren_rect.x = 0;
    p_out_para->out_scren_rect.y = 0;
    p_out_para->out_scren_rect.height = p_drv_para->screen_rect.height;
    p_out_para->out_scren_rect.width = p_drv_para->screen_rect.width;
}

static hi_void win_attr_revise_out_para(win_attr_algratio_outpara *p_out_para)
{
    if (p_out_para->out_wnd.width < MIN_WINDOW_NUM)
        p_out_para->out_wnd.width = MIN_WINDOW_NUM;

    if (p_out_para->out_wnd.height < MIN_WINDOW_NUM)
        p_out_para->out_wnd.height = MIN_WINDOW_NUM;
}

static hi_void win_attr_generate_out_para_with_zoom(win_attr_algratio_para *p_drv_para,
    win_attr_algratio_outpara *p_out_para)
{
    if (p_out_para->zme_h > p_drv_para->out_wnd.height) {
        p_out_para->crop_wnd.x = 0;
        p_out_para->crop_wnd.y = (p_out_para->zme_h - p_drv_para->out_wnd.height) / 2; // 2 is index
        p_out_para->crop_wnd.height = p_drv_para->out_wnd.height;
        p_out_para->crop_wnd.width = p_out_para->zme_w;
    } else {
        if (p_out_para->zme_w > p_drv_para->out_wnd.width) {
            p_out_para->crop_wnd.x = (p_out_para->zme_w - p_drv_para->out_wnd.width) / 2; // 2 is index
        } else {
            p_out_para->crop_wnd.x = (p_drv_para->out_wnd.width - p_out_para->zme_w) / 2; // 2 is index
        }
        p_out_para->crop_wnd.y = 0;
        p_out_para->crop_wnd.height = p_out_para->zme_h;
        p_out_para->crop_wnd.width = p_drv_para->out_wnd.width;
    }

    p_out_para->en_crop = HI_TRUE;

    p_out_para->out_wnd.height = p_out_para->crop_wnd.height;
    p_out_para->out_wnd.width = p_out_para->crop_wnd.width;
    p_out_para->out_wnd.x = (p_drv_para->out_wnd.width - p_out_para->crop_wnd.width) / 2; // 2 is index
    p_out_para->out_wnd.y = (p_drv_para->out_wnd.height - p_out_para->crop_wnd.height) / 2; // 2 is index

    p_out_para->out_scren_rect.x = 0;
    p_out_para->out_scren_rect.y = 0;
    p_out_para->out_scren_rect.height = p_drv_para->screen_rect.height;
    p_out_para->out_scren_rect.width = p_drv_para->screen_rect.width;
}

static hi_void win_attr_generate_zwe(win_attr_algratio_outpara *p_out_para, hi_u64 zme_h, hi_u64 zme_w)
{
    p_out_para->zme_h = zme_h;
    p_out_para->zme_w = zme_w;
}

static hi_s32 win_attr_alg_ratio_process(win_attr_algratio_para *p_drv_para, win_attr_algratio_outpara *p_out_para)
{
    // screen ratio w h, aspect ratio w h, output format w h
    // output video w h
    // u32pixr1_out:tv pixel aspect ratio concluded from  tv resolution and  output aspect ratio.
    // pixel width / pixel height
    hi_s64 sr_w, sr_h, ar_w, ar_h, f_w, f_h, u32pixr1_out;

    /* In Customer mode cvrs==Ignore will not full the window */
    if ((p_drv_para->e_asp_mode == HI_DRV_ASP_RAT_MODE_MAX)
        || (p_drv_para->e_asp_mode == HI_DRV_ASP_RAT_MODE_FULL)) {
        win_attr_config_out_para(p_out_para, p_drv_para);
        return HI_SUCCESS;
    }

    win_attr_generate_wh(p_drv_para, &ar_w, &ar_h, &sr_w, &sr_h);
    // get output format w h
    f_w = p_drv_para->screen_rect.width;
    f_h = p_drv_para->screen_rect.height;

    if (sr_h == 0 || f_w == 0) {
        WIN_ERROR("Denominator may be zero !\n");
        return HI_FAILURE;
    }
    // In customer mode use logic pixel,too
    u32pixr1_out = (sr_w * f_h * ALG_RATIO_PIX_RATIO1) / (sr_h * f_w);

    if (p_drv_para->e_asp_mode == HI_DRV_ASP_RAT_MODE_LETTERBOX) {
        win_attr_alg_ratio_letterbox(ar_w, ar_h, &(p_drv_para->out_wnd), u32pixr1_out);
        win_attr_config_out_para(p_out_para, p_drv_para);
        win_attr_revise_out_para(p_out_para);
    } else {
        hi_u64 zme_h, zme_w, exp_h, exp_w;

        if (ar_w == 0 || u32pixr1_out == 0 || ar_h == 0) {
            WIN_ERROR("Denominator may be zero !\n");
            return HI_FAILURE;
        }

        exp_h = p_drv_para->out_wnd.width * (u32pixr1_out) * ar_h / ar_w;
        exp_w = ar_w * p_drv_para->out_wnd.height * ALG_RATIO_PIX_RATIO1 / u32pixr1_out / ar_h;
        if (exp_h > p_drv_para->out_wnd.height * ALG_RATIO_PIX_RATIO1) {
            zme_w = p_drv_para->out_wnd.width;
            zme_h = exp_h / ALG_RATIO_PIX_RATIO1;
            if (p_drv_para->e_asp_mode == HI_DRV_ASP_RAT_MODE_COMBINED) {
                zme_h = p_drv_para->out_wnd.height
                          + (zme_h - p_drv_para->out_wnd.height) / 2; // 2 is index
                zme_w = ar_w * zme_h * ALG_RATIO_PIX_RATIO1 / u32pixr1_out / ar_h;
            }
        } else {
            zme_w = exp_w ;
            zme_h = p_drv_para->out_wnd.height;
            if (p_drv_para->e_asp_mode == HI_DRV_ASP_RAT_MODE_COMBINED) {
                zme_w = p_drv_para->out_wnd.width
                          + (zme_w - p_drv_para->out_wnd.width) / 2; // 2 is index
                zme_h = zme_w * (u32pixr1_out) * ar_h / ar_w / ALG_RATIO_PIX_RATIO1;
            }
        }
        win_attr_generate_zwe(p_out_para, zme_h, zme_w);
        win_attr_generate_out_para_with_zoom(p_drv_para, p_out_para);
    }

    return HI_SUCCESS;
}


static hi_void win_attr_aspect_ratio_process(win_attr_algratio_para *p_asp_drv_para,
    win_attr_algratio_outpara *p_asp_cfg)
{

    win_attr_algratio_para  st_asp_cal_para = *p_asp_drv_para;
    hi_s64 sr_w, sr_h;
    hi_s64 in_w, in_h;
    hi_s64 out_x, out_y;

    out_x = p_asp_drv_para->out_wnd.x;
    out_y = p_asp_drv_para->out_wnd.y;

    p_asp_drv_para->out_wnd.y = 0;
    p_asp_drv_para->out_wnd.x = 0;


    sr_w = st_asp_cal_para.aspect_width;
    sr_h = st_asp_cal_para.aspect_height;

    if (p_asp_drv_para->src_rect.height == 0 || p_asp_drv_para->src_rect.width == 0) {
        WIN_ERROR("Denominator may be zero !\n");
        return;
    }

    in_h = (p_asp_drv_para->in_wnd.height * sr_h) / p_asp_drv_para->src_rect.height;
    in_w = (p_asp_drv_para->in_wnd.width * sr_w) / p_asp_drv_para->src_rect.width;

    /* cal src crop rect aspratio! */
    st_asp_cal_para.aspect_width = in_w ;
    st_asp_cal_para.aspect_height = in_h;

    if (win_attr_alg_ratio_process(p_asp_drv_para, p_asp_cfg) != HI_SUCCESS) {
        WIN_ERROR("win_attr_alg_ratio_process failed!\n");
    }

    if (p_asp_cfg->en_crop) {
        /* cal inRect */
        p_asp_drv_para->in_wnd.x =
            (p_asp_cfg->crop_wnd.x * p_asp_drv_para->src_rect.width / p_asp_cfg->zme_w);
        p_asp_drv_para->in_wnd.width = (p_asp_cfg->crop_wnd.width * p_asp_drv_para->src_rect.width /
                                           p_asp_cfg->zme_w);
        p_asp_drv_para->in_wnd.y =
            (p_asp_cfg->crop_wnd.y * p_asp_drv_para->src_rect.height / p_asp_cfg->zme_h);
        p_asp_drv_para->in_wnd.height = (p_asp_cfg->crop_wnd.height * p_asp_drv_para->src_rect.height /
                                            p_asp_cfg->zme_h);

    }

    p_asp_cfg->out_wnd.x += out_x;
    p_asp_cfg->out_wnd.y += out_y;

}

static hi_bool win_attr_check_special_resolution(win_attr_algratio_para drv_para)
{
    if ((drv_para.e_asp_mode == HI_DRV_ASP_RAT_MODE_FULL
         || drv_para.e_asp_mode == HI_DRV_ASP_RAT_MODE_MAX)
        && drv_para.out_wnd.width == 720 // 720 is resolution
        && drv_para.in_wnd.width == 704 // 704 is resolution
        &&  (drv_para.in_wnd.height == 576 // 576 is resolution
                   || drv_para.in_wnd.height == 480) // 480 is resolution
        && drv_para.in_wnd.height
        == drv_para.out_wnd.height)
        return HI_TRUE;
    else
        return HI_FALSE;
}

static hi_void win_attr_config_special_resolution(win_attr_algratio_outpara *out_para,
    win_attr_algratio_para drv_para)
{
    out_para->out_wnd.x = 8; // 8 is min x
    out_para->out_wnd.y = 0;
    out_para->out_wnd.width = drv_para.in_wnd.width;
    out_para->out_wnd.height = drv_para.in_wnd.height;
}

static hi_s32 win_attr_cal_physic_resolution(drv_win_attr_cfg *p_attr_cfg)
{
    hi_s64 r_width = p_attr_cfg->out_reference_screen.width;
    hi_s64 r_height = p_attr_cfg->out_reference_screen.height;
    if (!p_attr_cfg->is_phy_coordinate) {
        if (p_attr_cfg->out_reference_screen.width == 0
            || p_attr_cfg->out_reference_screen.height == 0) {
            WIN_ERROR("invalid reference screen!");
            return HI_FAILURE;
        }

        p_attr_cfg->win_out_rect.x =
            (p_attr_cfg->win_out_rect.x) * (p_attr_cfg->out_physics_screen.width) / r_width;
        p_attr_cfg->win_out_rect.y =
            (p_attr_cfg->win_out_rect.y) * (p_attr_cfg->out_physics_screen.height) / r_height;
        p_attr_cfg->win_out_rect.width =
            (p_attr_cfg->win_out_rect.width) * (p_attr_cfg->out_physics_screen.width) / r_width;
        p_attr_cfg->win_out_rect.height =
            (p_attr_cfg->win_out_rect.height) * (p_attr_cfg->out_physics_screen.height) / r_height;
    }
    return HI_SUCCESS;
}

static hi_s32 win_attr_cfg_para_check(drv_win_attr_cfg *cfg)
{
#if 0
    if (cfg->src_width == 0 || cfg->src_height == 0) {
        WIN_ERROR("invalid src width or height,width:%d height:%d\n", cfg->src_width, cfg->src_height);
        return HI_FAILURE;
    }

    if (cfg->src_crop_rect.width == 0 || cfg->src_crop_rect.height == 0) {
        if (cfg->src_crop_rect.x != 0 || cfg->src_crop_rect.y != 0
            || cfg->src_crop_rect.width != 0 || cfg->src_crop_rect.height != 0) {
            WIN_ERROR("invalid src_crop_rect!\n");
            return HI_FAILURE;
        }
    }

    if (cfg->out_physics_screen.width == 0 || cfg->out_physics_screen.height == 0) {
        WIN_ERROR("zero physics screen!\n");
        return HI_FAILURE;
    }

    if (!cfg->out_rect_is_physics_resolution
        && (cfg->out_reference_screen.width == 0 || cfg->out_reference_screen.height == 0)) {
        WIN_ERROR("zero reference screen!\n");
        return HI_FAILURE;
    }

    if (cfg->win_out_rect.width == 0 || cfg->win_out_rect.height == 0) {
        WIN_ERROR("zero win out rect!\n");
        return HI_FAILURE;
    }
#endif
    return HI_SUCCESS;
}

static hi_s32 win_attr_check_para(drv_win_attr_cfg *attr_cfg, drv_win_attr_out_config *out_cfg)
{
    hi_s32 ret;
    if (attr_cfg == HI_NULL || out_cfg == HI_NULL) {
        return HI_FAILURE;
    }

    ret = win_attr_cal_physic_resolution(attr_cfg);

    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    ret = win_attr_cfg_para_check(attr_cfg);

    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return ret;
}

hi_s32 drv_win_attrprocess_update(hi_handle hi_attr, drv_win_attr_cfg *attr_cfg, drv_win_attr_out_config *out_cfg)
{
    unsigned long flags;
#ifndef LOCAL_TEST
    win_attr_node *win_node = HI_NULL;
#endif
    win_attr_algratio_para  drv_para = {0};
    win_attr_algratio_outpara  out_para = {0};
    hi_drv_disp_offset  offset_tmp = {0};
    hi_s32 ret;

    ret = win_attr_check_para(attr_cfg, out_cfg);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

#ifndef LOCAL_TEST
    osal_spin_lock_irqsave(&g_win_attr_context.win_attr_spin_lock, &flags);
    win_node = win_attr_get_node(&(g_win_attr_context.win_attr_node_list), hi_attr);
    if (win_node == HI_NULL) {
        osal_spin_unlock_irqrestore(&g_win_attr_context.win_attr_spin_lock, &flags);
        WIN_ERROR("can not find win attr context, hi_attr is %d\n", hi_attr);
        return HI_FAILURE;
    }
    if (!osal_memncmp(attr_cfg, sizeof(drv_win_attr_cfg), &win_node->attr_cfg, sizeof(drv_win_attr_cfg)) &&
        win_node->calculated) {
        *out_cfg = win_node->out_cfg;
        osal_spin_unlock_irqrestore(&g_win_attr_context.win_attr_spin_lock, &flags);
        return HI_SUCCESS;
    }
#endif

    win_config_in_rect(attr_cfg, &out_cfg->src_crop_rect);
    win_attr_init_drv_para(*attr_cfg, &drv_para, out_cfg);

    if (win_attr_check_special_resolution(drv_para)) {
        win_attr_config_special_resolution(&out_para, drv_para);
    } else {
        if (attr_cfg->src_active_format_flag) {
            win_attr_afd_process(attr_cfg->src_afd_type, &drv_para, &out_para);
        } else {
            win_attr_aspect_ratio_process(&drv_para, &out_para);
        }
    }

    out_cfg->src_crop_rect = drv_para.in_wnd;
    out_cfg->win_out_rect   = attr_cfg->win_out_rect;
    out_cfg->win_video_rect  = out_para.out_wnd;

    /* since we support picture moved out of screen, so we should give a revise to the window ,
        *both inrect and outrect. */
    ret = win_attr_revise_outofscreenwin_outrect(out_cfg, &attr_cfg->out_physics_screen, &offset_tmp);
    if (ret != HI_SUCCESS) {
#ifndef LOCAL_TEST
        osal_spin_unlock_irqrestore(&g_win_attr_context.win_attr_spin_lock, &flags);
#endif
        return ret;
    }

#ifndef LOCAL_TEST
    win_node->attr_cfg = *attr_cfg;
    win_node->out_cfg = *out_cfg;
    win_node->calculated = HI_TRUE;
    osal_spin_unlock_irqrestore(&g_win_attr_context.win_attr_spin_lock, &flags);
#endif
    return HI_SUCCESS;
}

#ifdef LOCAL_TEST
hi_s32 drv_win_attrprocess_init(void)
{
    return HI_SUCCESS;
}
hi_s32 drv_win_attrprocess_deinit(void)
{
    return HI_SUCCESS;
}

hi_handle drv_win_attrprocess_create(void)
{
    return HI_SUCCESS;
}
hi_s32 drv_win_attrprocess_destory(hi_handle h_attr)
{
    return HI_SUCCESS;
}
#endif
