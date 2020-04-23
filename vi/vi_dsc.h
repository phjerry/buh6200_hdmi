/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: vi dsc
 * Author: sdk
 * Create: 2019-12-14
 */

#ifndef __VI_DSC_H__
#define __VI_DSC_H__

#include "vi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 vi_dsc_get_buf_num(vi_instance *vi_instance_p, hi_u32 *min_buf_num_p, hi_u32 *max_buf_num_p);
hi_s32 vi_dsc_get_in_attr(vi_instance *vi_instance_p);
hi_s32 vi_dsc_get_ctrl_attr(vi_instance *vi_instance_p);
hi_s32 vi_dsc_get_out_attr(vi_instance *vi_instance_p);
hi_s32 vi_dsc_updata_vi_attr(vi_instance *vi_instance_p);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
