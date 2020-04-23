/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hdr regset api
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __PQ_MNG_HDR_H__
#define __PQ_MNG_HDR_H__

#include "drv_pq_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VID0_HDR_OFFSET     (0x2f000 / 4)
#define VID1_HDR_OFFSET     (0x00000 / 4)

#define COEF_SIZE_RESERVE         (4096 * 128 / 8)
#define COEF_SIZE_HDR0          8192
#define COEF_SIZE_HDR1          4096
#define COEF_SIZE_HDR2          10000
#define COEF_SIZE_HDR3          4096
#define COEF_SIZE_HDR4          4096
#define COEF_SIZE_HDR5          4096
#define COEF_SIZE_HDR6          8192
#define COEF_SIZE_HDR7          4096
#define COEF_SIZE_HDR8          40000
#define COEF_SIZE_OM_DEGMM      (256 * 128 / 8)
#define COEF_SIZE_OM_GMM        (256 * 128 / 8)
#define COEF_SIZE_OM_CM         (256 * 128 / 8)

#define VDP_HDR_COEF_SIZE (COEF_SIZE_RESERVE + COEF_SIZE_HDR0 + \
                           COEF_SIZE_HDR1 + COEF_SIZE_HDR2 + COEF_SIZE_HDR3 + \
                           COEF_SIZE_HDR4 + COEF_SIZE_HDR5 + \
                           COEF_SIZE_HDR6 + COEF_SIZE_HDR7 + \
                           COEF_SIZE_HDR8 + COEF_SIZE_OM_DEGMM + \
                           COEF_SIZE_OM_GMM + COEF_SIZE_OM_CM)

typedef enum {
    HI_PQ_DISP_TYPE_SDR = 0,
    HI_PQ_DISP_TYPE_HDR10  = 1,
    HI_PQ_DISP_TYPE_HLG    = 2,

    HI_PQ_DISP_TYPE_MAX
} pq_disp_hdr_type;

hi_s32 pq_mng_register_hdr(pq_reg_type type);

hi_s32 pq_mng_unregister_hdr(hi_void);

hi_s32 pq_mng_tool_set_hdr_tmap(hi_pq_hdr_tmap *hdr_tmap);

hi_s32 pq_mng_tool_get_hdr_tmap(hi_pq_hdr_tmap *hdr_tmap);

hi_s32 pq_mng_tool_set_hdrpara_mode(hi_pq_hdr_para_mode *para_mode);

hi_s32 pq_mng_tool_get_hdrpara_mode(hi_pq_hdr_para_mode *para_mode);

hi_s32 pq_mng_tool_set_hdr_smap(hi_pq_hdr_smap *hdr_smap);

hi_s32 pq_mng_tool_get_hdr_smap(hi_pq_hdr_smap *hdr_smap);

hi_s32 pq_mng_set_hdr_cfg(hi_drv_pq_xdr_layer_id layer_id, hi_drv_pq_xdr_frame_info *xdr_frame_info);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
