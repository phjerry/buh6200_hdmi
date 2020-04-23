/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */


#ifndef __VDP_DRV_IP_MDATA_H__
#define __VDP_DRV_IP_MDATA_H__

#include "vdp_chip_define.h"

typedef struct {
    hi_bool enable;
    hi_bool avi_enable;
    hi_bool smd_enable;
    hi_u64  addr;
    hi_u32  burst_num;
    hi_u32  data_len;
} vdp_mdata_cfg;

hi_void vdp_ip_mdata_init(hi_void);
hi_void vdp_ip_mdata_deinit(hi_void);
hi_void vdp_ip_mdata_set_cfg(vdp_mdata_cfg *cfg);

#endif

