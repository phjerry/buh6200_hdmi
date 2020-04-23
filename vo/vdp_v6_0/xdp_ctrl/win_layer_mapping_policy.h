/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: ion  adapter code
* Author: image
* Create: 2019-03-28
 */

#ifndef WIN_LAYER_MAPPING_H
#define WIN_LAYER_MAPPING_H
#include "hi_type.h"
#include "hi_drv_disp.h"

typedef struct {
    hi_handle win_handle;
    hi_rect win_coordinate;
    hi_u16 win_zorder;
    hi_u32 layer_id;
    hi_u32 layer_current_zorder;
} win_coordinate_info;

typedef struct {
    hi_handle win_handle;
    hi_u16 layer_id;
    hi_u16 region_id;
    hi_drv_disp_zorder match_operation;
} win_layer_mappingresult;

hi_s32 window_redistribute_proccess(win_descriptor *tmp_win);

#endif
