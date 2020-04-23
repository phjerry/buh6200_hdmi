/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_REG_COMPOSER_H__
#define __HAL_VDP_REG_COMPOSER_H__

#include "hi_reg_vdp.h"

hi_void vdp_vid_setdmcomposeren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 composer_en);
hi_void vdp_vid_setcomposerblen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bl_en);
hi_void vdp_vid_setdmstben(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 stb_en);
hi_void vdp_vid_setcomposervdrbitdepth(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vdr_bit_depth);
hi_void vdp_vid_setcomposerstbmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 stb_mode);
hi_void vdp_vid_setcomposerblnumpivotsy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bl_num_pivots);
hi_void vdp_vid_setcomposerblbitdepth(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bl_bit_depth);
hi_void vdp_vid_setcomposerblpolypivotvaluey(vdp_regs_type *vdp_reg, hi_u32 offset,
    hi_u32 bl_pivot_value[9]); /* 9 is number */
hi_void vdp_vid_setcomposerblpolyordery(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bl_order[8]); /* 8 is number */
hi_void vdp_vid_setcomposerblpolycoefy(vdp_regs_type *vdp_reg, hi_u32 offset,
    hi_s32 bl_coef[8][3]); /* 8, 3 is number */
hi_void vdp_vid_setcomposerblnumpivotsv(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bl_num_pivots_v);
hi_void vdp_vid_setcomposerblnumpivotsu(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bl_num_pivots_u);
hi_void vdp_vid_setcomposermappingidcv(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mapping_idc_v);
hi_void vdp_vid_setcomposermappingidcu(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mapping_idc_u);
hi_void vdp_vid_setcomposerblpolypivotvalueu(vdp_regs_type *vdp_reg, hi_u32 offset,
    hi_u32 bl_pivot_value[5]); /* 5 is number */
hi_void vdp_vid_setcomposerblpolyorderu(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bl_order[4]); /* 4 is number */
hi_void vdp_vid_setcomposerblpolycoefu(vdp_regs_type *vdp_reg, hi_u32 offset,
    hi_s32 bl_coef[4][3]); /* 4, 3 is number */
hi_void vdp_vid_setcomposerblmmrorderu(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bl_mmr_order_u);
hi_void vdp_vid_setcomposerblmmrcoefu(vdp_regs_type *vdp_reg, hi_u32 offset,
    hi_s32 bl_mmr_coef_u[22]); /* 22 is number */
hi_void vdp_vid_setcomposerblpolypivotvaluev(vdp_regs_type *vdp_reg, hi_u32 offset,
    hi_u32 bl_pivot_value[5]); /* 5 is number */
hi_void vdp_vid_setcomposerblpolyorderv(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bl_order[4]); /* 4 is number */
hi_void vdp_vid_setcomposerblpolycoefv(vdp_regs_type *vdp_reg, hi_u32 offset,
    hi_s32 bl_coef[4][3]); /* 4, 3 is number */
hi_void vdp_vid_setcomposerblmmrorderv(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bl_mmr_order_v);
hi_void vdp_vid_setcomposerblmmrcoefv(vdp_regs_type *vdp_reg, hi_u32 offset,
    hi_s32 bl_mmr_coef_v[22]); /* 22 is number */
hi_void vdp_vid_setcomposerelzmeen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 el_zme_en);
hi_void vdp_vid_setcomposerelen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 el_en);
hi_void vdp_vid_setcomposerelbitdepth(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 el_bit_depth);
hi_void vdp_vid_setcomposerelnldqoffsety(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 el_nldq_offset_y);
hi_void vdp_vid_setcomposerelnldqcoefy(vdp_regs_type *vdp_reg, hi_u32 offset,
    hi_s32 el_nldq_coef_y[3]); /* 3 is number */
hi_void vdp_vid_setcomposerelnldqoffsetu(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 el_nldq_offset_u);
hi_void vdp_vid_setcomposerelnldqcoefu(vdp_regs_type *vdp_reg, hi_u32 offset,
    hi_s32 el_nldq_coef_u[3]); /* 3 is number */
hi_void vdp_vid_setcomposerelnldqoffsetv(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 el_nldq_offset_v);
hi_void vdp_vid_setcomposerelnldqcoefv(vdp_regs_type *vdp_reg, hi_u32 offset,
    hi_s32 el_nldq_coef_v[3]); /* 3 is number */
hi_void vdp_vid_setcomposerupsamplemax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 upsample_max);
hi_void vdp_vid_setcomposerupsamplemin(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 upsample_min);

#endif
