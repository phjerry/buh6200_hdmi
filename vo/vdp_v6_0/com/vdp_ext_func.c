/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: function define
* Author: image
* Create: 2019-04-12
 */

#include "hi_type.h"
#include "vdp_ext_func.h"
#include "hi_disp_type.h"
#include "hi_drv_module.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

static pq_export_func *g_pq_func = HI_NULL;
hi_s32 vdp_ext_func_init(hi_void)
{
    hi_s32 ret;
    ret = hi_drv_module_get_func(HI_ID_PQ, (hi_void **)&g_pq_func);
    if (ret != HI_SUCCESS) {
        hi_err_disp("get pq func faild\n");
        return ret;
    }

    if ((g_pq_func == HI_NULL) ||
        (g_pq_func->pq_get_vdp_csc_coef == HI_NULL) ||
        (g_pq_func->pq_update_vdp_alg == HI_NULL) ||
        (g_pq_func->pq_update_vdp_stt_info == HI_NULL) ||
        (g_pq_func->pq_get_vdp_zme_strategy == HI_NULL)) {
        hi_err_disp("get pq func NULL error\n");
        return ret;
    }
    return HI_SUCCESS;
}

hi_s32 vdp_ext_pq_get_csc_coef(hi_drv_pq_csc_layer layer_id, hi_drv_pq_csc_info csc_info,
                               hi_drv_pq_csc_coef *csc_coef)
{
    hi_s32 ret = g_pq_func->pq_get_vdp_csc_coef(layer_id, csc_info, csc_coef);
    if (ret != HI_SUCCESS) {
        hi_err_disp("get vdp_ext_pq_get_csc_coef faild\n");
        return ret;
    }
    return HI_SUCCESS;
}

hi_s32 vdp_ext_pq_update_vdp_alg(hi_drv_pq_display_id display_id, hi_drv_pq_vdp_info *vdp_info)
{
    hi_s32 ret = g_pq_func->pq_update_vdp_alg(display_id, vdp_info);
    if (ret != HI_SUCCESS) {
        hi_err_disp("pq_update_vdp_alg faild\n");
        return ret;
    }
    return HI_SUCCESS;
}

hi_s32 vdp_ext_pq_get_vdp_zme_strategy(hi_drv_pq_zme_layer layer_id, hi_drv_pq_zme_strategy_in *zme_in,
                                             hi_drv_pq_zme_strategy_out *zme_out)
{
    hi_s32 ret = g_pq_func->pq_get_vdp_zme_strategy(layer_id, zme_in, zme_out);
    if (ret != HI_SUCCESS) {
        hi_err_disp("get vdp_ext_pq_get_csc_coef faild\n");
        return ret;
    }
    return HI_SUCCESS;
}

hi_void vdp_ext_pq_update_vdp_stt_info(hi_void)
{
    g_pq_func->pq_update_vdp_stt_info();
    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
