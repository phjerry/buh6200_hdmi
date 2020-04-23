/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: function define
* Author: image
* Create: 2019-04-12
 */
#ifndef __VDP_EXT_FUNC_H__
#define __VDP_EXT_FUNC_H__

#include "drv_pq_ext.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 vdp_ext_func_init(hi_void);
hi_s32 vdp_ext_pq_get_csc_coef(hi_drv_pq_csc_layer layer_id, hi_drv_pq_csc_info csc_info,
                               hi_drv_pq_csc_coef *csc_coef);
hi_s32 vdp_ext_pq_update_vdp_alg(hi_drv_pq_display_id display_id, hi_drv_pq_vdp_info *vdp_info);
hi_s32 vdp_ext_pq_get_vdp_zme_strategy(hi_drv_pq_zme_layer layer_id, hi_drv_pq_zme_strategy_in *zme_in,
                                       hi_drv_pq_zme_strategy_out *zme_out);
hi_void vdp_ext_pq_update_vdp_stt_info(hi_void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

