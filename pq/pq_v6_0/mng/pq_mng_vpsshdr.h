/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __PQ_MNG_VPSSHDR_H__
#define __PQ_MNG_VPSSHDR_H__

#include "drv_pq_comm.h"
#include "pq_hal_vpsshdr.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VID0_HDR_OFFSET     (0x2f000 / 4)
#define VID1_HDR_OFFSET     (0x00000 / 4)

typedef struct {
    hi_bool      init;
    hi_bool      enable;
    hi_bool      demo_enable;
    pq_demo_mode demo_mode;
} alg_vpss_hdr;

hi_s32 pq_mng_register_vpss_hdr(pq_reg_type type);
hi_s32 pq_mng_unregister_vpss_hdr(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


