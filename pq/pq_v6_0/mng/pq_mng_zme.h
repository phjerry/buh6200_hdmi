/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq zme api define
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __PQ_MNG_ZME_H__
#define __PQ_MNG_ZME_H__

#include <linux/string.h>

#include "hi_type.h"
#include "hi_reg_common.h"
#include "drv_pq_comm.h"
#include "pq_hal_zme.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define ALG_V_HZME_PRECISION 1048576
#define ALG_V_VZME_PRECISION 4096

#define ALG_ZME_H_MAX_RATIO 16
#define ALG_ZME_V_MAX_RATIO 16

#define COEF_SIZE_RESERVE              (4096 * 128 / 8)
#define COEF_SIZE_8K_HZME              (4096 * 128 / 8)
#define COEF_SIZE_8K_VZME              (4096 * 128 / 8)
#define COEF_SIZE_4K_SR0               (12000 * 128 / 8)
#define COEF_SIZE_8K_SR0               (12000 * 128 / 8)
#define COEF_SIZE_XDP_2DSCALE_H        (4096 * 128 / 8)
#define COEF_SIZE_XDP_2DSCALE_V        (4096 * 128 / 8)

#define VDP_ZME_COEF_SIZE  (COEF_SIZE_RESERVE + COEF_SIZE_8K_HZME + \
                            COEF_SIZE_8K_VZME + COEF_SIZE_4K_SR0 + COEF_SIZE_8K_SR0 + \
                            COEF_SIZE_XDP_2DSCALE_H + COEF_SIZE_XDP_2DSCALE_V)

typedef enum {
    ZME_COEF_8T17P_LH = 0,
    ZME_COEF_6T17P_LV,
    ZME_COEF_8T17P_CH,
    ZME_COEF_6T17P_CV,

    ZME_COEF_TYPE_MAX
} pq_zme_coef_type;

typedef struct {
    hi_bool zme_init_flag;

    hi_bool vdp_zme_fir_en;
    hi_bool vdp_zme_med_en;

    pq_tun_mode zme_tun_mode;
} alg_video_zme;

hi_s32 pq_mng_register_zme(pq_reg_type type);

hi_s32 pq_mng_unregister_zme(hi_void);

hi_s32 pq_mng_get_vdp_zme_strategy(hi_drv_pq_zme_layer layer_id, hi_drv_pq_zme_strategy_in *zme_in,
                                   hi_drv_pq_zme_strategy_out *zme_out);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


