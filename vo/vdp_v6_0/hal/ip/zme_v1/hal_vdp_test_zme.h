/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_TEST_ZME_H__
#define __HAL_VDP_TEST_ZME_H__

#include "hal_vdp_ip_zme.h"
#include "hal_vdp_ip_coef.h"

typedef struct {
    void *p_coef;
    void *p_coef_new;
    vdp_coef_data_type coef_data_type;
    hi_u32 length;
    vdp_rm_coef_mode coef_data_mode;
    hi_s32 coef_max;
    hi_s32 coef_min;
} vdp_coef_gen_cfg;


hi_void vdp_zme_test_8kzme(hi_u32 offset, vdp_zme_image_info *info);


#endif

