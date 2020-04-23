/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: sharpen reg layer header file
 * Author: pq
 * Create: 2019-11-11
 */


#ifndef _PQ_HAL_SHARPEN_REGSET_H_
#define _PQ_HAL_SHARPEN_REGSET_H_

#include "hi_register_vdp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* *******************************SHARPEN PARA REG*************************************** */
hi_void pq_reg_sharp_setparaupv0chn(vdp_regs_type *vdp_reg, hi_u32 u32chnnum);
hi_void pq_reg_sharp_para_addr_v0_chn14(vdp_regs_type *vdp_reg, hi_u32 data);

hi_void pq_reg_sharp_set_en(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sharp_en);
hi_void pq_reg_sharp_set_demo_en(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 demo_en);
hi_void pq_reg_sharp_set_demo_mode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 demo_mode);
hi_void pq_reg_sharp_set_demo_pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 demo_pos);
hi_void pq_reg_sharp_set_peak_gain(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 peak_gain);
hi_void pq_reg_sharp_set_peak_ratio(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 peak_ratio);
hi_void pq_reg_sharp_set_lti_ratio(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lti_ratio);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
