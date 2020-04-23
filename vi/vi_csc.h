/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: vi csc
 * Author: sdk
 * Create: 2019-12-14
 */

#ifndef __VI_CSC_H__
#define __VI_CSC_H__

#include "vi_comm.h"
#include "vi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 vi_csc_get_csc_info(vi_instance *vi_instance_p, hi_drv_pq_csc_info *csc_info_p);
hi_s32 vi_csc_get_csc_para(hi_drv_pq_csc_info csc_info, hi_drv_pq_csc_coef *pq_csc_coef_p);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
