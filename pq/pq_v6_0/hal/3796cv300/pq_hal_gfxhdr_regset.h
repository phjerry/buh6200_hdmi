/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_errno.h"
#include "drv_pq_comm.h"

#ifndef __PQ_HAL_GFXHDR_REGSET_H__
#define __PQ_HAL_GFXHDR_REGSET_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_void pq_hdr_setregup(vdp_regs_type *vdp_reg);
hi_void pq_para_setparaaddrdispchn09(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_para_setparaaddrdispchn10(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_para_setparaaddrdispchn11(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_para_setparaaddrdispchn12(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_para_setparahaddrdispchn09(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_para_setparahaddrdispchn10(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_para_setparahaddrdispchn11(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_para_setparahaddrdispchn12(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_para_setparaupdispchn(vdp_regs_type *vdp_reg, hi_u32 u32chnnum);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
