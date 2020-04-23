/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: function define
* Author: image
* Create: 2019-04-12
 */

#include "hi_drv_disp.h"
#include "hi_drv_win.h"

#include "osal_list.h"
#include "drv_xdp_osal.h"
#include "drv_window_priv.h"
#include "drv_win_mng.h"

#include "vdp_base_define.h"
#include "hal_layer_intf.h"

#include "win_layer_mapping_policy.h"
#define V0                0
#define V1                1
#define V3                3
#define WINDOW_MAX_NUMBER 17

typedef struct {
    hi_s32 x_start;
    hi_u32 origin_pos;
} alg_info;

typedef struct {
    /* input param */
    hi_u16 win_zorder;
    hi_u16 layer_num;
    hi_u32 layer_zorder;

    /* output param. */
    hi_drv_disp_zorder match_opt;
} win_zorder_info;

typedef struct {
    hi_u16 x_start;
    hi_u16 x_end;
    hi_u16 y_start;
    hi_u16 y_end;

    /* the zorder is an input param. */
    hi_u16 zorder;
    hi_u32 video_layer;
} window_info;

typedef struct {
    /* video_layer and region_no are output params. */
    hi_u16 video_layer;
    hi_u16 region_no;
    hi_u32 overlap_cnt;
} window_mapping_result;

typedef struct {
    window_info input_win_coordinate_info[WINDOW_MAX_NUMBER];
    win_zorder_info input_win_zorder_info[WINDOW_MAX_NUMBER];
    window_mapping_result win_mapping_result[WINDOW_MAX_NUMBER];
} win_map_to_layer_info;

hi_s32 window_layer_mapping(window_info *win_coordinate,
                            window_mapping_result *win_mapping_result,
                            hi_u32 win_num);
hi_s32 layer_zorder_match(win_zorder_info *win_zorder_infor, hi_u32 win_num);

window_mapping_result g_win_mapping_result_tmp[WINDOW_MAX_NUMBER] = { 0 };
window_info g_win_info[WINDOW_MAX_NUMBER] = { 0 };

hi_void win_get_all_enabled_windows(win_descriptor **win_array, hi_u32 *win_num, win_descriptor *win)
{
    hi_u32 enabled_win_cnt = 0;
    hi_u32 win_cnt = 0;
    win_descriptor *tmp_win;
    hi_handle *all_win;

    all_win = winmanage_get_win_handle(win->win_user_attr.disp_chn);

    for (win_cnt = 0; win_cnt < WINDOW_MAX_NUMBER; win_cnt++) {
        if (all_win[win_cnt] == 0) {
            continue;
        }

        tmp_win = winmanage_get_window(all_win[win_cnt]);
        if (tmp_win == HI_NULL) {
            continue;
        }

        if (tmp_win->win_play_ctrl.win_enable == HI_TRUE) {
            win_array[enabled_win_cnt++] = tmp_win;
        }
    }

    *win_num = enabled_win_cnt;
    return;
}

/* check whether the layout is valid. */
hi_void win_get_location_info(win_descriptor **win_array, window_info *win_info, hi_u32 win_num)
{
    hi_u32 i = 0;
    hi_disp_display_info disp_info = { 0 };
    hi_u32 width = 0;
    hi_u32 height = 0;

    (hi_void) hi_drv_disp_get_display_info(win_array[i]->win_user_attr.disp_chn, &disp_info);

    for (i = 0; i < win_num; i++) {
        if ((win_array[i]->win_user_attr.out_rect.width == 0) ||
            (win_array[i]->win_user_attr.out_rect.height == 0)) {
            width = disp_info.virtaul_screen.width;
            height = disp_info.virtaul_screen.height;

        } else if ((win_array[i]->win_user_attr.out_rect.x > disp_info.virtaul_screen.width) ||
                   (win_array[i]->win_user_attr.out_rect.y > disp_info.virtaul_screen.height)) {
            width = 0;
            height = 0;

        } else if (((win_array[i]->win_user_attr.out_rect.x + win_array[i]->win_user_attr.out_rect.width) < 0) ||
                   ((win_array[i]->win_user_attr.out_rect.y + win_array[i]->win_user_attr.out_rect.height) < 0)) {
            width = 0;
            height = 0;
        } else {
            if (disp_info.virtaul_screen.width >=
                (win_array[i]->win_user_attr.out_rect.x +
                 win_array[i]->win_user_attr.out_rect.width)) {
                width = win_array[i]->win_user_attr.out_rect.width;
            } else {
                width = disp_info.virtaul_screen.width - win_array[i]->win_user_attr.out_rect.x;
            }

            if (disp_info.virtaul_screen.height >=
                (win_array[i]->win_user_attr.out_rect.y +
                 win_array[i]->win_user_attr.out_rect.height)) {
                height = win_array[i]->win_user_attr.out_rect.height;
            } else {
                height = disp_info.virtaul_screen.height - win_array[i]->win_user_attr.out_rect.y;
            }
        }

        win_info[i].x_start = win_array[i]->win_user_attr.out_rect.x;
        win_info[i].x_end = win_array[i]->win_user_attr.out_rect.x + width;
        win_info[i].y_start = win_array[i]->win_user_attr.out_rect.y;
        win_info[i].y_end = win_array[i]->win_user_attr.out_rect.y + height;

        /* get the window zorder */
        win_info[i].zorder = win_array[i]->expected_win_attr.win_zorder;
        win_info[i].video_layer = win_array[i]->expect_win_layermapping.layer_id;
    }

    return;
}

static hi_bool check_window_overlap(window_info win_base, window_info win_in)
{
    /* windows overlapping judgement */
    if ((win_in.y_end <= win_base.y_start) || (win_in.y_start >= win_base.y_end)
        || (win_in.x_end <= win_base.x_start) || (win_in.x_start >= win_base.x_end)) {
        return HI_FALSE;
    } else {
        return HI_TRUE;
    }
}

static hi_void calc_window_overlap_num(window_info *win_coordinate, hi_u32 *calc_win_num, hi_u32 win_num)
{
    hi_u32 x;
    hi_u32 y;

    for (x = 0; x < win_num; x++) {
        for (y = 0; y < win_num; y++) {
            if (x != y) {
                calc_win_num[x] += check_window_overlap(win_coordinate[x], win_coordinate[y]);
            }
        }
    }
}

static hi_void register_window_overlap(window_mapping_result *win_mapping_result,
                                       hi_u32 *calc_win_num,
                                       hi_u32 win_num)
{
    hi_u32 i = 0;

    for (i = 0; i < win_num; i++) {
        win_mapping_result[i].overlap_cnt = calc_win_num[i];
    }

    return;
}

static hi_void get_max_window_overlap_pos(hi_u32 *calc_win_num, hi_u32 win_num, hi_u32 *max_overlap_value,
                                          hi_u32 *max_overlap_win)
{
    hi_u32 x;

    *max_overlap_value = calc_win_num[0];
    *max_overlap_win = 0;
    for (x = 1; x < win_num; x++) {
        if (calc_win_num[x] > *max_overlap_value) {
            *max_overlap_value = calc_win_num[x];
            *max_overlap_win = x;
        }
    }
}

static hi_void get_v0_window_area_pos(window_info *win_coordinate, hi_u32 win_num, hi_u32 *max_area_win)
{
    hi_u32 i = 0;

    *max_area_win = 0;
    for (i = 0; i < win_num; i++) {
        if (win_coordinate[i].video_layer == V0) {
            *max_area_win = i;
            return;
        }
    }
    return;
}

static hi_bool check_v0_window_max(window_info *win_coordinate, hi_u32 win_num, hi_u32 *v0_win)
{
    hi_u32 i = 0;
    hi_u32 v0_width;
    hi_u32 v0_height;
    hi_u32 tmp_width = 0;
    hi_u32 tmp_height = 0;

    v0_width = win_coordinate[*v0_win].x_end - win_coordinate[*v0_win].x_start;
    v0_height = win_coordinate[*v0_win].y_end - win_coordinate[*v0_win].y_start;

    for (i = 0; i < win_num; i++) {
        tmp_width = win_coordinate[i].x_end - win_coordinate[i].x_start;
        tmp_height = win_coordinate[i].y_end - win_coordinate[i].y_start;

        if ((v0_width * v0_height) < (tmp_width * tmp_height)) {
            return HI_FALSE;
        }
    }

    return HI_TRUE;
}

static hi_u32 get_window_area(window_info win_info)
{
    return ((hi_u32)(win_info.x_end - win_info.x_start)) * ((hi_u32)(win_info.y_end - win_info.y_start));
}

static hi_void get_max_window_area_pos(window_info *win_coordinate, hi_u32 win_num, hi_u32 *max_area_win)
{
    hi_u32 x;
    hi_u32 cur_area;
    hi_u32 max_area;
    hi_u32 max_zorder;
    hi_u32 i;
    hi_u32 j;
    hi_u32 zorder[WINDOW_MAX_NUMBER] = { 0 };
    hi_u32 win_index[WINDOW_MAX_NUMBER] = { 0 };

    *max_area_win = 0;
    max_area = get_window_area(win_coordinate[0]);
    for (x = 1; x < win_num; x++) {
        cur_area = get_window_area(win_coordinate[x]);
        if (cur_area > max_area) {
            max_area = cur_area;
            *max_area_win = x;
        }
    }

    j = 0;
    for (i = 0; i < win_num; i++) {
        if (get_window_area(win_coordinate[i]) == max_area) {
            /* If one of max-area window is on V0, take it as "max_area_win" and return */
            if (win_coordinate[i].video_layer == V0) {
                *max_area_win = i;

                return;
            }

            win_index[j] = i;
            zorder[j] = win_coordinate[i].zorder;
            j++;
        }
    }

    *max_area_win = win_index[0];
    max_zorder = zorder[0];

    for (i = 1; i < j; i++) {
        if (zorder[i] > max_zorder) {
            *max_area_win = win_index[i];
            max_zorder = zorder[i];
        }
    }

    return;
}

static hi_void cal_layer_mapping(window_info *win_coordinate, window_mapping_result *win_mapping_result,
                                 hi_u32 win_num, hi_u32 vo_win_pos)
{
    /* set windows Zorder except the one in V0 layer */
    hi_u32 x;
    hi_u32 y;
    hi_u32 arr_cnt;
    alg_info *win_order_arr = HI_NULL;
    alg_info tmp_alg_info;
    alg_info alg_info[WINDOW_MAX_NUMBER] = { { 0 } };

    win_order_arr = alg_info;
    y = 0;

    for (x = 0; x < win_num; x++) {
        if (x == vo_win_pos) {
            continue;
        }
        win_order_arr[y].x_start = win_coordinate[x].x_start;
        win_order_arr[y].origin_pos = x;
        y++;
    }

    /* Bubble sort in descending order */
    arr_cnt = (win_num - 1);
    for (x = 0; x < arr_cnt; x++) {
        for (y = x + 1; y < arr_cnt; y++) {
            if (win_order_arr[y].x_start < win_order_arr[x].x_start) {
                tmp_alg_info = win_order_arr[x];
                win_order_arr[x] = win_order_arr[y];
                win_order_arr[y] = tmp_alg_info;
            }
        }
    }

    /* set result */
    for (x = 0; x < arr_cnt; x++) {
        win_mapping_result[win_order_arr[x].origin_pos].region_no = x;
        win_mapping_result[win_order_arr[x].origin_pos].video_layer = V1;
    }

    win_mapping_result[vo_win_pos].region_no = 0;
    win_mapping_result[vo_win_pos].video_layer = V0;
    return;
}

static hi_s32 judge_whether_zorder_match(window_mapping_result *winMappingResult,
                                         hi_u32 num_of_window,
                                         window_info *win_coordinate)
{
    hi_u32 i = 0;
    hi_u32 v0_zorder = 0;
    hi_bool blarger = HI_FALSE;
    hi_bool bless = HI_FALSE;

    for (i = 0; i < num_of_window; i++) {
        if (winMappingResult[i].video_layer == V0) {
            v0_zorder = win_coordinate[i].zorder;
            break;
        }
    }

    for (i = 0; i < num_of_window; i++) {
        if (winMappingResult[i].overlap_cnt != 0) {
            if (win_coordinate[i].zorder > v0_zorder) {
                blarger = HI_TRUE;
            }
            else if (win_coordinate[i].zorder < v0_zorder) {
                bless = HI_TRUE;
            }
        }
    }

    if ((blarger) && (bless)) {
        WIN_ERROR("zorders do not match!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 get_max_zorder_by_overlap_window(window_info *win_coordinate,
                                        hi_u32 num_of_window,
                                        hi_u32 find_overlap_window,
                                        hi_u32 *get_max_zorder_window,
                                        window_mapping_result *win_mapping_result_tmp)
{
    hi_u32 i;
    hi_s32 ret;
    hi_s32 ret1;
    *get_max_zorder_window = 0;

    for (i = 0; i < num_of_window; i++) {
        if ((find_overlap_window != i) &&
            check_window_overlap(win_coordinate[find_overlap_window], win_coordinate[i])) {
            if (get_window_area(win_coordinate[find_overlap_window]) > get_window_area(win_coordinate[i])) {
                *get_max_zorder_window = find_overlap_window;
            }
            else if (get_window_area(win_coordinate[find_overlap_window]) < get_window_area(win_coordinate[i])) {
                *get_max_zorder_window = i;
            }
            else {
                *get_max_zorder_window = (win_coordinate[find_overlap_window].zorder > win_coordinate[i].zorder ?
                                          find_overlap_window : i);
            }
            break;
        }
    }

    cal_layer_mapping(win_coordinate, win_mapping_result_tmp, num_of_window, find_overlap_window);
    ret = judge_whether_zorder_match(win_mapping_result_tmp, num_of_window, win_coordinate);

    cal_layer_mapping(win_coordinate, win_mapping_result_tmp, num_of_window, i);
    ret1 = judge_whether_zorder_match(win_mapping_result_tmp, num_of_window, win_coordinate);

    if (ret && ret1) {
        WIN_ERROR("get max zorder window failed\n");
        return HI_FAILURE;
    } else if (!ret && ret1) {
        *get_max_zorder_window = find_overlap_window;
    } else if (ret && !ret1) {
        *get_max_zorder_window = i;
    }

    return HI_SUCCESS;
}

hi_void get_no_overlap_result(window_info *win_tmp_coordinate, window_mapping_result *win_mapping_result,
                              hi_u32 win_num)
{
    hi_u32 vo_win_pos;

    get_v0_window_area_pos(win_tmp_coordinate, win_num, &vo_win_pos);

    if (check_v0_window_max(win_tmp_coordinate, win_num, &vo_win_pos) == HI_FALSE) {
        get_max_window_area_pos(win_tmp_coordinate, win_num, &vo_win_pos);
    }

    cal_layer_mapping(win_tmp_coordinate, win_mapping_result, win_num, vo_win_pos);
    return;
}

hi_void win_coordinate_init(window_info *win_tmp_coordinate, hi_u32 first_max_overlap_window,
                            hi_u32 *calc_win_num_array)
{
    win_tmp_coordinate[first_max_overlap_window].x_start = 0;
    win_tmp_coordinate[first_max_overlap_window].x_end = 0;
    win_tmp_coordinate[first_max_overlap_window].y_start = 0;
    win_tmp_coordinate[first_max_overlap_window].y_end = 0;
    memset((hi_void *)calc_win_num_array, 0, WINDOW_MAX_NUMBER * sizeof(hi_u32));
    return;
}

#define MAX_OVELAP_WIN_NUM 2
hi_s32 window_layer_mapping(window_info *win_coordinate, window_mapping_result *win_mapping_result, hi_u32 win_num)
{
    hi_u32 first_max_overlap_value;
    hi_u32 second_max_overlap_value;
    hi_u32 first_max_overlap_window;
    hi_u32 second_max_overlap_window;
    hi_u32 vo_win_pos;
    hi_u32 calc_window_num_array[WINDOW_MAX_NUMBER] = { 0 };
    hi_s32 ret = HI_SUCCESS;

    if (win_num == 1) {
        win_mapping_result[0].video_layer = V0;
        win_mapping_result[0].region_no = 0;
        return HI_SUCCESS;
    }
    memset((void *)g_win_mapping_result_tmp, 0, sizeof(window_mapping_result) * WINDOW_MAX_NUMBER);
    memcpy((hi_void *)g_win_info, (hi_void *)win_coordinate, win_num * sizeof(window_info));
    memset((hi_void *)calc_window_num_array, 0, WINDOW_MAX_NUMBER * sizeof(hi_u32));

    /* calculate the overlap count for every window. */
    calc_window_overlap_num(g_win_info, calc_window_num_array, win_num);
    /* register every window's overlap cnt to final win_mapping_result. */
    register_window_overlap(win_mapping_result, calc_window_num_array, win_num);

    get_max_window_overlap_pos(calc_window_num_array, win_num, &first_max_overlap_value, &first_max_overlap_window);

    /* no overlap */
    if (first_max_overlap_value == 0) {
        get_no_overlap_result(g_win_info, win_mapping_result, win_num);

    } else {
        /* remapping all windows  expect the one with the biggest overlap count */
        win_coordinate_init(g_win_info, first_max_overlap_window, calc_window_num_array);
        calc_window_overlap_num(g_win_info, calc_window_num_array, win_num);
        get_max_window_overlap_pos(calc_window_num_array, win_num, &second_max_overlap_value, &second_max_overlap_window);

        /* windows still  overlap after remapping */
        if (second_max_overlap_value > 0) {
            ret = HI_FAILURE;
            WIN_ERROR("Windows remmapping failed, overlap window:%d\n!", second_max_overlap_window);
            goto ALG_EXIT;
        }

        memcpy((hi_void *)g_win_info, (hi_void *)win_coordinate, win_num * sizeof(window_info));
        /* only one window overlap the others */
        if (first_max_overlap_value >= MAX_OVELAP_WIN_NUM) {
            vo_win_pos = first_max_overlap_window;
        } else {
            /* two windows is entirely overlapmapping with each other */
            /* remapping according to zorder of these two windows */
            memcpy((void *)g_win_mapping_result_tmp, (void *)win_mapping_result, win_num * sizeof(window_mapping_result));
            ret = get_max_zorder_by_overlap_window(g_win_info, win_num, first_max_overlap_window,
                                                   &vo_win_pos, g_win_mapping_result_tmp);
            if (ret != HI_SUCCESS) {
                goto ALG_EXIT;
            }
        }
        cal_layer_mapping(g_win_info, win_mapping_result, win_num, vo_win_pos);

        /* if some window overlaps, the zorder in the window  which located in v0,  should
                  be less than all the overlaps window ,or be larger than all the overlaps window. */
        ret = judge_whether_zorder_match(win_mapping_result, win_num, g_win_info);
    }

ALG_EXIT:
    return ret;
}

/* update the new layer and region number to window. */
hi_void win_update_mapping_info(win_descriptor *win, window_mapping_result *win_map_result, hi_drv_display disp_chn)
{

    win->expect_win_layermapping.layer_id = win_map_result->video_layer;
    win->expect_win_layermapping.layer_region_id = win_map_result->region_no;

    return;
}

hi_s32 layer_zorder_match(win_zorder_info *win_zorder_infor, hi_u32 win_num)
{
    hi_u32 i = 0;
    hi_u32 win_zorder = 0;
    hi_u32 k = 0;
    hi_bool blarger = HI_FALSE;
    hi_bool bless = HI_FALSE;

    for (i = 0; i < win_num; i++) {
        if ((win_zorder_infor[i].layer_num == V0)
            || (win_zorder_infor[i].layer_num == V3)) {
            win_zorder = win_zorder_infor[i].win_zorder;
            k = i;
        }
        win_zorder_infor[i].match_opt = HI_DRV_DISP_ZORDER_BUTT;
    }

    for (i = 0; i < win_num; i++) {
        if (win_zorder_infor[i].win_zorder > win_zorder) {
            bless = HI_TRUE;
        }
        else if (win_zorder_infor[i].win_zorder < win_zorder) {
            blarger = HI_TRUE;
        }
    }

    if ((blarger == HI_TRUE) && (bless == HI_TRUE)) {
        WIN_ERROR("Win zorder is not right, can't allocate layers.!\n");
        return HI_FAILURE;
    }

    if (blarger == HI_TRUE) {

        win_zorder_infor[k].match_opt = HI_DRV_DISP_ZORDER_MOVETOP;
    } else if (bless == HI_TRUE) {

        win_zorder_infor[k].match_opt = HI_DRV_DISP_ZORDER_MOVEBOTTOM;
    }

    return HI_SUCCESS;
}

static hi_void update_win_zorder_adjust_info(hi_drv_disp_zorder zorder_opt)
{
    hi_u32 i = 0;
    win_descriptor *win_tmp = HI_NULL;
    hi_handle *all_win = 0;

    all_win = winmanage_get_win_handle(HI_DRV_DISPLAY_0);

    for (i = 0; i < WINDOW_MAX_NUMBER; i++) {
        if (all_win[i] == 0) {
            continue;
        }

        win_tmp = winmanage_get_window(all_win[i]);

        if (win_tmp == HI_NULL) {
            continue;
        }

        if (win_tmp->expect_win_layermapping.layer_id == LAYER_ID_VID0) {

            if (zorder_opt == HI_DRV_DISP_ZORDER_MOVETOP) {
                win_tmp->expect_win_layermapping.layer_order = 1;
            } else if (zorder_opt == HI_DRV_DISP_ZORDER_MOVEBOTTOM)  {
                win_tmp->expect_win_layermapping.layer_order = 0;
            } else {
                win_tmp->expect_win_layermapping.layer_order = 0;
            }
        } else if (win_tmp->expect_win_layermapping.layer_id == LAYER_ID_VID1) {

            if (zorder_opt == HI_DRV_DISP_ZORDER_MOVETOP) {
                win_tmp->expect_win_layermapping.layer_order = 0;
            } else if (zorder_opt == HI_DRV_DISP_ZORDER_MOVEBOTTOM)  {
                win_tmp->expect_win_layermapping.layer_order = 1;
            } else {
                win_tmp->expect_win_layermapping.layer_order = 1;
            }
        }
    }

    return;
}

hi_void win_update_zorder_info(win_map_to_layer_info *win_to_layer_info, win_descriptor *win_array,
                               window_mapping_result *win_map_result,
                               hi_u32 *overlap_win_cnt, hi_u32 *updating_layer_index)
{
    win_to_layer_info->input_win_zorder_info[*overlap_win_cnt].layer_num =
    win_map_result->video_layer;
    win_to_layer_info->input_win_zorder_info[*overlap_win_cnt].win_zorder =
    win_array->expected_win_attr.win_zorder;

    if ((win_to_layer_info->input_win_zorder_info[*overlap_win_cnt].layer_num == LAYER_ID_VID0)
        || (win_to_layer_info->input_win_zorder_info[*overlap_win_cnt].layer_num == LAYER_ID_VID3)) {
        *updating_layer_index = *overlap_win_cnt;
    }

    (*overlap_win_cnt)++;

    return;
}

hi_void win_update_final_result(win_descriptor *win)
{
    hi_u32 i = 0;
    win_descriptor *tmp_win;
    hi_handle *all_win;

    all_win = winmanage_get_win_handle(win->win_user_attr.disp_chn);

    for (i = 0; i < WINDOW_MAX_NUMBER; i++) {
        if (all_win[i] == HI_NULL) {
            continue;
        }

        tmp_win = winmanage_get_window(all_win[i]);
        if ((tmp_win != HI_NULL) && (tmp_win->win_index == win->win_index)) {

            tmp_win->expect_win_layermapping =  win->expect_win_layermapping;
            break;
        }
    }
    return;
}

/* window adjust, move/zme/create/destroy may cause the layout
 * change. because the  adjust in thread-suituation may be half complete,
 * and then interruptted by isr, so the change should be located in isr func.
 */

hi_s32 window_redistribute_proccess(win_descriptor *tmp_win)
{
    win_descriptor *win_array[WINDOW_MAX_NUMBER] = { HI_NULL };
    hi_u32 win_num = 0;
    hi_u32 i = 0;
    hi_u32 overlap_win_cnt = 0;
    hi_u32 updating_layer_index = 0;
    hi_s32 ret;
    win_map_to_layer_info win_to_layer_info;

    if (tmp_win->win_user_attr.disp_chn == HI_DRV_DISPLAY_2) {
        tmp_win->expect_win_layermapping.layer_id = LAYER_ID_BUTT;
        tmp_win->expect_win_layermapping.layer_region_id = 0;
        return HI_SUCCESS;
    } else if (tmp_win->win_user_attr.disp_chn == HI_DRV_DISPLAY_1) {
        tmp_win->expect_win_layermapping.layer_id = LAYER_ID_VID3;
        tmp_win->expect_win_layermapping.layer_region_id = 0;
        return HI_SUCCESS;
    }

    /* first,get all the enabled window ptrs. */
    win_get_all_enabled_windows(win_array, &win_num, tmp_win);

    if (win_num == 0) {
        return HI_SUCCESS;
    } else if (win_num > WINDOW_MAX_NUMBER) {
        WIN_ERROR("Enabled window num is not correct.\n");
        return HI_FAILURE;
    }

    /* Second,  get the coordinate and locations value from windows. */
    win_get_location_info(win_array, win_to_layer_info.input_win_coordinate_info, win_num);

    /* third,  generate win-layer remapping. */
    ret = window_layer_mapping(win_to_layer_info.input_win_coordinate_info, win_to_layer_info.win_mapping_result, win_num);
    if (ret != HI_SUCCESS) {
        return HI_ERR_VO_OPERATION_DENIED;
    }

    for (i = 0; i < win_num; i++) {
        if (win_to_layer_info.win_mapping_result[i].overlap_cnt != 0) {
            win_update_zorder_info(&win_to_layer_info, win_array[i], &(win_to_layer_info.win_mapping_result[i]),
                                   &overlap_win_cnt, &updating_layer_index);
        }
    }

    /* check wether the window's order matches the layer's order. */
    ret = layer_zorder_match(win_to_layer_info.input_win_zorder_info, overlap_win_cnt);
    if (ret != HI_SUCCESS) {
        return HI_ERR_VO_OPERATION_DENIED;
    }

    /* update new window-layer mapping. */
    for (i = 0; i < win_num; i++) {
        win_update_mapping_info(win_array[i], &(win_to_layer_info.win_mapping_result[i]),
                                tmp_win->win_user_attr.disp_chn);
    }

    /* update all windows layer order. */
    update_win_zorder_adjust_info(win_to_layer_info.input_win_zorder_info[updating_layer_index].match_opt);

    return ret;
}



