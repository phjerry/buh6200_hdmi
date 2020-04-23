/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_reg_composer.h"
#include "hal_vdp_comm.h"

hi_void vdp_vid_setdmcomposeren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 composer_en)
{
    u_composer_ctrl composer_ctrl;

    composer_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->composer_ctrl.u32) + offset));
    composer_ctrl.bits.composer_en = composer_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->composer_ctrl.u32) + offset), composer_ctrl.u32);

    return;
}

hi_void vdp_vid_setcomposerblen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bl_en)
{
    u_poly_ctrl poly_ctrl;

    poly_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->poly_ctrl.u32) + offset));
    poly_ctrl.bits.bl_en = bl_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->poly_ctrl.u32)), poly_ctrl.u32);

    return;
}

hi_void vdp_vid_setdmstben(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 stb_en)
{
    u_composer_ctrl composer_ctrl;

    composer_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->composer_ctrl.u32) + offset));
    composer_ctrl.bits.stb_en = stb_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->composer_ctrl.u32) + offset), composer_ctrl.u32);

    return;
}

hi_void vdp_vid_setcomposervdrbitdepth(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vdr_bit_depth)
{
    u_composer_ctrl composer_ctrl;

    composer_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->composer_ctrl.u32) + offset));
    composer_ctrl.bits.vdr_bit_depth = vdr_bit_depth;
    vdp_regwrite((uintptr_t)(&(vdp_reg->composer_ctrl.u32) + offset), composer_ctrl.u32);

    return;
}

hi_void vdp_vid_setcomposerstbmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 stb_mode)
{
    u_composer_ctrl composer_ctrl;

    composer_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->composer_ctrl.u32) + offset));
    composer_ctrl.bits.stb_mode = stb_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->composer_ctrl.u32) + offset), composer_ctrl.u32);

    return;
}

hi_void vdp_vid_setcomposerblnumpivotsy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bl_num_pivots)
{
    u_poly_ctrl poly_ctrl;

    poly_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->poly_ctrl.u32) + offset));
    poly_ctrl.bits.bl_num_pivots = bl_num_pivots;
    vdp_regwrite((uintptr_t)(&(vdp_reg->poly_ctrl.u32) + offset), poly_ctrl.u32);

    return;
}

hi_void vdp_vid_setcomposerblbitdepth(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bl_bit_depth)
{
    u_poly_ctrl poly_ctrl;
    u_mmr_ctrl mmr_ctrl;

    poly_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->poly_ctrl.u32) + offset));
    poly_ctrl.bits.bl_bit_depth = bl_bit_depth;
    vdp_regwrite((uintptr_t)(&(vdp_reg->poly_ctrl.u32) + offset), poly_ctrl.u32);

    mmr_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->mmr_ctrl.u32) + offset));
    mmr_ctrl.bits.bl_bit_depth = bl_bit_depth;
    vdp_regwrite((uintptr_t)(&(vdp_reg->mmr_ctrl.u32) + offset), mmr_ctrl.u32);

    return;
}

hi_void vdp_vid_setcomposerblpolypivotvaluey(vdp_regs_type *vdp_reg,
    hi_u32 offset, hi_u32 bl_pivot_value[9]) /* 9 is number */
{
    u_bl_pivot_value_0 bl_pivot_value_0;
    u_bl_pivot_value_1 bl_pivot_value_1;
    u_bl_pivot_value_2 bl_pivot_value_2;
    u_bl_pivot_value_3 bl_pivot_value_3;
    u_bl_pivot_value_4 bl_pivot_value_4;
    u_bl_pivot_value_5 bl_pivot_value_5;
    u_bl_pivot_value_6 bl_pivot_value_6;
    u_bl_pivot_value_7 bl_pivot_value_7;
    u_bl_pivot_value_8 bl_pivot_value_8;

    bl_pivot_value_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_pivot_value_0.u32) + offset));
    bl_pivot_value_1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_pivot_value_1.u32) + offset));
    bl_pivot_value_2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_pivot_value_2.u32) + offset));
    bl_pivot_value_3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_pivot_value_3.u32) + offset));
    bl_pivot_value_4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_pivot_value_4.u32) + offset));
    bl_pivot_value_5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_pivot_value_5.u32) + offset));
    bl_pivot_value_6.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_pivot_value_6.u32) + offset));
    bl_pivot_value_7.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_pivot_value_7.u32) + offset));
    bl_pivot_value_8.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_pivot_value_8.u32) + offset));
    bl_pivot_value_0.bits.bl_0_pivot_value = bl_pivot_value[0];
    bl_pivot_value_1.bits.bl_1_pivot_value = bl_pivot_value[1];
    bl_pivot_value_2.bits.bl_2_pivot_value = bl_pivot_value[2];  // 2 is reg index
    bl_pivot_value_3.bits.bl_3_pivot_value = bl_pivot_value[3];  // 3 is reg index
    bl_pivot_value_4.bits.bl_4_pivot_value = bl_pivot_value[4];  // 4 is reg index
    bl_pivot_value_5.bits.bl_5_pivot_value = bl_pivot_value[5];  // 5 is reg index
    bl_pivot_value_6.bits.bl_6_pivot_value = bl_pivot_value[6];  // 6 is reg index
    bl_pivot_value_7.bits.bl_7_pivot_value = bl_pivot_value[7];  // 7 is reg index
    bl_pivot_value_8.bits.bl_8_pivot_value = bl_pivot_value[8];  // 8 is reg index
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_pivot_value_0.u32) + offset), bl_pivot_value_0.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_pivot_value_1.u32) + offset), bl_pivot_value_1.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_pivot_value_2.u32) + offset), bl_pivot_value_2.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_pivot_value_3.u32) + offset), bl_pivot_value_3.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_pivot_value_4.u32) + offset), bl_pivot_value_4.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_pivot_value_5.u32) + offset), bl_pivot_value_5.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_pivot_value_6.u32) + offset), bl_pivot_value_6.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_pivot_value_7.u32) + offset), bl_pivot_value_7.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_pivot_value_8.u32) + offset), bl_pivot_value_8.u32);

    return;
}

hi_void vdp_vid_setcomposerblpolyordery(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bl_order[8]) // 8 is reg index
{
    u_bl_poly_order bl_poly_order;

    bl_poly_order.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_poly_order.u32) + offset));
    bl_poly_order.bits.bl_0_order = bl_order[0];
    bl_poly_order.bits.bl_1_order = bl_order[1];
    bl_poly_order.bits.bl_2_order = bl_order[2];  // 2 is reg index
    bl_poly_order.bits.bl_3_order = bl_order[3];  // 3 is reg index
    bl_poly_order.bits.bl_4_order = bl_order[4];  // 4 is reg index
    bl_poly_order.bits.bl_5_order = bl_order[5];  // 5 is reg index
    bl_poly_order.bits.bl_6_order = bl_order[6];  // 6 is reg index
    bl_poly_order.bits.bl_7_order = bl_order[7];  // 7 is reg index
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_poly_order.u32) + offset), bl_poly_order.u32);

    return;
}

hi_void vdp_vid_setcomposerblpolycoefy(vdp_regs_type *vdp_reg, hi_u32 offset,
    hi_s32 bl_coef[8][3]) /* 8, 3 is number */
{
    u_bl_coef_00 bl_coef_00;
    hi_u32 i, j;

    for (i = 0; i < 8; i++) { /* 8 is number */
        for (j = 0; j < 3; j++) { /* 3 is number */
            bl_coef_00.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_coef_00.u32) + offset));
            bl_coef_00.bits.bl_00_coef = bl_coef[i][j];
            vdp_regwrite((uintptr_t)(&(vdp_reg->bl_coef_00.u32) + offset), bl_coef_00.u32);
            offset += 1;
        }
    }

    return;
}

hi_void vdp_vid_setcomposerblnumpivotsv(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bl_num_pivots_v)
{
    u_mmr_ctrl mmr_ctrl;

    mmr_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->mmr_ctrl.u32) + offset));
    mmr_ctrl.bits.bl_num_pivots_v = bl_num_pivots_v;
    vdp_regwrite((uintptr_t)(&(vdp_reg->mmr_ctrl.u32) + offset), mmr_ctrl.u32);

    return;
}

hi_void vdp_vid_setcomposerblnumpivotsu(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bl_num_pivots_u)
{
    u_mmr_ctrl mmr_ctrl;

    mmr_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->mmr_ctrl.u32) + offset));
    mmr_ctrl.bits.bl_num_pivots_u = bl_num_pivots_u;
    vdp_regwrite((uintptr_t)(&(vdp_reg->mmr_ctrl.u32) + offset), mmr_ctrl.u32);

    return;
}

hi_void vdp_vid_setcomposermappingidcv(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mapping_idc_v)
{
    u_mmr_ctrl mmr_ctrl;

    mmr_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->mmr_ctrl.u32) + offset));
    mmr_ctrl.bits.mapping_idc_v = mapping_idc_v;
    vdp_regwrite((uintptr_t)(&(vdp_reg->mmr_ctrl.u32) + offset), mmr_ctrl.u32);

    return;
}

hi_void vdp_vid_setcomposermappingidcu(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mapping_idc_u)
{
    u_mmr_ctrl mmr_ctrl;

    mmr_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->mmr_ctrl.u32) + offset));
    mmr_ctrl.bits.mapping_idc_u = mapping_idc_u;
    vdp_regwrite((uintptr_t)(&(vdp_reg->mmr_ctrl.u32) + offset), mmr_ctrl.u32);

    return;
}

hi_void vdp_vid_setcomposerblpolypivotvalueu(vdp_regs_type *vdp_reg, hi_u32 offset,
    hi_u32 bl_pivot_value[5]) /* 5 is number */
{
    u_bl_pivot_value_u0 bl_pivot_value_u0;
    u_bl_pivot_value_u1 bl_pivot_value_u1;
    u_bl_pivot_value_u2 bl_pivot_value_u2;
    u_bl_pivot_value_u3 bl_pivot_value_u3;
    u_bl_pivot_value_u4 bl_pivot_value_u4;

    bl_pivot_value_u0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_pivot_value_u0.u32) + offset));
    bl_pivot_value_u1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_pivot_value_u1.u32) + offset));
    bl_pivot_value_u2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_pivot_value_u2.u32) + offset));
    bl_pivot_value_u3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_pivot_value_u3.u32) + offset));
    bl_pivot_value_u4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_pivot_value_u4.u32) + offset));
    bl_pivot_value_u0.bits.bl_0_pivot_value_u = bl_pivot_value[0];
    bl_pivot_value_u1.bits.bl_1_pivot_value_u = bl_pivot_value[1];  // 1 is reg index
    bl_pivot_value_u2.bits.bl_2_pivot_value_u = bl_pivot_value[2];  // 2 is reg index
    bl_pivot_value_u3.bits.bl_3_pivot_value_u = bl_pivot_value[3];  // 3 is reg index
    bl_pivot_value_u4.bits.bl_4_pivot_value_u = bl_pivot_value[4];  // 4 is reg index
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_pivot_value_u0.u32) + offset), bl_pivot_value_u0.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_pivot_value_u1.u32) + offset), bl_pivot_value_u1.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_pivot_value_u2.u32) + offset), bl_pivot_value_u2.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_pivot_value_u3.u32) + offset), bl_pivot_value_u3.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_pivot_value_u4.u32) + offset), bl_pivot_value_u4.u32);

    return;
}

hi_void vdp_vid_setcomposerblpolyorderu(vdp_regs_type *vdp_reg, hi_u32 offset,
    hi_u32 bl_order[4]) /* 4 is number */
{
    u_bl_poly_order_u bl_poly_order_u;

    bl_poly_order_u.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_poly_order_u.u32) + offset));
    bl_poly_order_u.bits.bl_0_order_u = bl_order[0];
    bl_poly_order_u.bits.bl_1_order_u = bl_order[1];
    bl_poly_order_u.bits.bl_2_order_u = bl_order[2]; // 2 is reg index
    bl_poly_order_u.bits.bl_3_order_u = bl_order[3]; // 3 is reg index
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_poly_order_u.u32) + offset), bl_poly_order_u.u32);

    return;
}

hi_void vdp_vid_setcomposerblpolycoefu(vdp_regs_type *vdp_reg, hi_u32 offset,
    hi_s32 bl_coef[4][3]) /* 4, 3 is number */
{
    u_bl_coef_u00 bl_coef_u00;
    hi_u32 i, j;

    for (i = 0; i < 8; i++) { /* 8 is number */
        for (j = 0; j < 3; j++) { /* 3 is number */
            bl_coef_u00.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_coef_u00.u32) + offset));
            bl_coef_u00.bits.bl_00_coef_u = bl_coef[0][0];
            vdp_regwrite((uintptr_t)(&(vdp_reg->bl_coef_u00.u32) + offset), bl_coef_u00.u32);
            offset += 1;
        }
    }

    return;
}

hi_void vdp_vid_setcomposerblmmrorderu(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bl_mmr_order_u)
{
    u_bl_u_mmr_order bl_u_mmr_order;

    bl_u_mmr_order.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_u_mmr_order.u32) + offset));
    bl_u_mmr_order.bits.bl_mmr_order_u = bl_mmr_order_u;
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_u_mmr_order.u32) + offset), bl_u_mmr_order.u32);

    return;
}

hi_void vdp_vid_setcomposerblmmrcoefu(vdp_regs_type *vdp_reg, hi_u32 offset,
    hi_s32 bl_mmr_coef_u[22]) /* 22 is number */
{
    u_bl_mmr_coef_u0 bl_mmr_coef_u0;
    hi_u32 i, j;

    for (i = 0; i < 8; i++) { /* 8 is number */
        for (j = 0; j < 3; j++) { /* 3 is number */
            bl_mmr_coef_u0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_mmr_coef_u0.u32) + offset));
            bl_mmr_coef_u0.bits.bl_0_mmr_coef_u = bl_mmr_coef_u[0];
            vdp_regwrite((uintptr_t)(&(vdp_reg->bl_mmr_coef_u0.u32) + offset), bl_mmr_coef_u0.u32);
            offset += 1;
        }
    }

    return;
}

hi_void vdp_vid_setcomposerblpolypivotvaluev(vdp_regs_type *vdp_reg, hi_u32 offset,
    hi_u32 bl_pivot_value[5]) /* 5 is number */
{
    u_bl_pivot_value_v0 bl_pivot_value_v0;
    u_bl_pivot_value_v1 bl_pivot_value_v1;
    u_bl_pivot_value_v2 bl_pivot_value_v2;
    u_bl_pivot_value_v3 bl_pivot_value_v3;
    u_bl_pivot_value_v4 bl_pivot_value_v4;

    bl_pivot_value_v0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_pivot_value_v0.u32) + offset));
    bl_pivot_value_v1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_pivot_value_v1.u32) + offset));
    bl_pivot_value_v2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_pivot_value_v2.u32) + offset));
    bl_pivot_value_v3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_pivot_value_v3.u32) + offset));
    bl_pivot_value_v4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_pivot_value_v4.u32) + offset));
    bl_pivot_value_v0.bits.bl_0_pivot_value_v = bl_pivot_value[0];
    bl_pivot_value_v1.bits.bl_1_pivot_value_v = bl_pivot_value[1];
    bl_pivot_value_v2.bits.bl_2_pivot_value_v = bl_pivot_value[2];  // 2 is reg index
    bl_pivot_value_v3.bits.bl_3_pivot_value_v = bl_pivot_value[3];  // 3 is reg index
    bl_pivot_value_v4.bits.bl_4_pivot_value_v = bl_pivot_value[4];  // 4 is reg index
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_pivot_value_v0.u32) + offset), bl_pivot_value_v0.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_pivot_value_v1.u32) + offset), bl_pivot_value_v1.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_pivot_value_v2.u32) + offset), bl_pivot_value_v2.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_pivot_value_v3.u32) + offset), bl_pivot_value_v3.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_pivot_value_v4.u32) + offset), bl_pivot_value_v4.u32);

    return;
}

hi_void vdp_vid_setcomposerblpolyorderv(vdp_regs_type *vdp_reg, hi_u32 offset,
    hi_u32 bl_order[4]) /* 4 is number */
{
    u_bl_poly_order_v bl_poly_order_v;

    bl_poly_order_v.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_poly_order_v.u32) + offset));
    bl_poly_order_v.bits.bl_0_order_v = bl_order[0];
    bl_poly_order_v.bits.bl_1_order_v = bl_order[1];
    bl_poly_order_v.bits.bl_2_order_v = bl_order[2];  // 2 is reg index
    bl_poly_order_v.bits.bl_3_order_v = bl_order[3];  // 3 is reg index
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_poly_order_v.u32) + offset), bl_poly_order_v.u32);

    return;
}

hi_void vdp_vid_setcomposerblpolycoefv(vdp_regs_type *vdp_reg, hi_u32 offset,
    hi_s32 bl_coef[4][3]) /* 4, 3 is number */
{
    u_bl_coef_v00 bl_coef_v00;
    hi_u32 i, j;

    for (i = 0; i < 8; i++) { /* 8 is number */
        for (j = 0; j < 3; j++) { /* 3 is number */
            bl_coef_v00.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_coef_v00.u32) + offset));
            bl_coef_v00.bits.bl_00_coef_v = bl_coef[0][0];
            vdp_regwrite((uintptr_t)(&(vdp_reg->bl_coef_v00.u32) + offset), bl_coef_v00.u32);
            offset += 1;
        }
    }

    return;
}

hi_void vdp_vid_setcomposerblmmrorderv(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bl_mmr_order_v)
{
    u_bl_v_mmr_order bl_v_mmr_order;

    bl_v_mmr_order.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_v_mmr_order.u32) + offset));
    bl_v_mmr_order.bits.bl_mmr_order_v = bl_mmr_order_v;
    vdp_regwrite((uintptr_t)(&(vdp_reg->bl_v_mmr_order.u32) + offset), bl_v_mmr_order.u32);

    return;
}

hi_void vdp_vid_setcomposerblmmrcoefv(vdp_regs_type *vdp_reg, hi_u32 offset,
    hi_s32 bl_mmr_coef_v[22]) /* 22 is number */
{
    u_bl_mmr_coef_v0 bl_mmr_coef_v0;
    hi_u32 i, j;

    for (i = 0; i < 8; i++) { /* 8 is number */
        for (j = 0; j < 3; j++) { /* 3 is number */
            bl_mmr_coef_v0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->bl_mmr_coef_v0.u32) + offset));
            bl_mmr_coef_v0.bits.bl_0_mmr_coef_v = bl_mmr_coef_v[0];
            vdp_regwrite((uintptr_t)(&(vdp_reg->bl_mmr_coef_v0.u32) + offset), bl_mmr_coef_v0.u32);
            offset += 1;
        }
    }

    return;
}

hi_void vdp_vid_setcomposerelzmeen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 el_zme_en)
{
    u_nldq_ctrl nldq_ctrl;

    nldq_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->nldq_ctrl.u32) + offset));
    nldq_ctrl.bits.el_zme_en = el_zme_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->nldq_ctrl.u32) + offset), nldq_ctrl.u32);

    return;
}

hi_void vdp_vid_setcomposerelen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 el_en)
{
    u_nldq_ctrl nldq_ctrl;

    nldq_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->nldq_ctrl.u32) + offset));
    nldq_ctrl.bits.el_en = el_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->nldq_ctrl.u32) + offset), nldq_ctrl.u32);

    return;
}

hi_void vdp_vid_setcomposerelbitdepth(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 el_bit_depth)
{
    u_nldq_ctrl nldq_ctrl;

    nldq_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->nldq_ctrl.u32) + offset));
    nldq_ctrl.bits.el_bit_depth = el_bit_depth;
    vdp_regwrite((uintptr_t)(&(vdp_reg->nldq_ctrl.u32) + offset), nldq_ctrl.u32);

    return;
}

hi_void vdp_vid_setcomposerelnldqoffsety(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_el_nldq_y_offset el_nldq_offset_y;

    el_nldq_offset_y.u32 = vdp_regread((uintptr_t)(&(vdp_reg->el_nldq_y_offset.u32) + offset));
    el_nldq_offset_y.bits.el_nldq_offset_y = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->el_nldq_y_offset.u32) + offset), el_nldq_offset_y.u32);

    return;
}

hi_void vdp_vid_setcomposerelnldqcoefy(vdp_regs_type *vdp_reg, hi_u32 offset,
    hi_s32 el_nldq_coef_y[3]) /* 3 is number */
{
    u_el_nldq_y0_coef el_nldq_coef_y0;
    u_el_nldq_y1_coef el_nldq_coef_y1;
    u_el_nldq_y2_coef el_nldq_coef_y2;

    el_nldq_coef_y0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->el_nldq_y0_coef.u32) + offset));
    el_nldq_coef_y1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->el_nldq_y1_coef.u32) + offset));
    el_nldq_coef_y2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->el_nldq_y2_coef.u32) + offset));
    el_nldq_coef_y0.bits.el_nldq_coef_0_y = el_nldq_coef_y[0];
    el_nldq_coef_y1.bits.el_nldq_coef_1_y = el_nldq_coef_y[1];
    el_nldq_coef_y2.bits.el_nldq_coef_2_y = el_nldq_coef_y[2]; // 2 is reg index
    vdp_regwrite((uintptr_t)(&(vdp_reg->el_nldq_y0_coef.u32) + offset), el_nldq_coef_y0.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->el_nldq_y1_coef.u32) + offset), el_nldq_coef_y1.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->el_nldq_y2_coef.u32) + offset), el_nldq_coef_y2.u32);

    return;
}

hi_void vdp_vid_setcomposerelnldqoffsetu(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_el_nldq_u_offset el_nldq_offset_u;

    el_nldq_offset_u.u32 = vdp_regread((uintptr_t)(&(vdp_reg->el_nldq_u_offset.u32) + offset));
    el_nldq_offset_u.bits.el_nldq_offset_u = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->el_nldq_u_offset.u32) + offset), el_nldq_offset_u.u32);

    return;
}

hi_void vdp_vid_setcomposerelnldqcoefu(vdp_regs_type *vdp_reg, hi_u32 offset,
    hi_s32 el_nldq_coef_u[3]) /* 3 is number */
{
    u_el_nldq_u0_coef el_nldq_coef_u0;
    u_el_nldq_u1_coef el_nldq_coef_u1;
    u_el_nldq_u2_coef el_nldq_coef_u2;

    el_nldq_coef_u0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->el_nldq_u0_coef.u32) + offset));
    el_nldq_coef_u1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->el_nldq_u1_coef.u32) + offset));
    el_nldq_coef_u2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->el_nldq_u2_coef.u32) + offset));
    el_nldq_coef_u0.bits.el_nldq_coef_0_u = el_nldq_coef_u[0];
    el_nldq_coef_u1.bits.el_nldq_coef_1_u = el_nldq_coef_u[1];
    el_nldq_coef_u2.bits.el_nldq_coef_2_u = el_nldq_coef_u[2]; // 2 is reg index
    vdp_regwrite((uintptr_t)(&(vdp_reg->el_nldq_u0_coef.u32) + offset), el_nldq_coef_u0.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->el_nldq_u1_coef.u32) + offset), el_nldq_coef_u1.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->el_nldq_u2_coef.u32) + offset), el_nldq_coef_u2.u32);

    return;
}

hi_void vdp_vid_setcomposerelnldqoffsetv(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_el_nldq_v_offset el_nldq_offset_v;

    el_nldq_offset_v.u32 = vdp_regread((uintptr_t)(&(vdp_reg->el_nldq_v_offset.u32) + offset));
    el_nldq_offset_v.bits.el_nldq_offset_v = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->el_nldq_v_offset.u32) + offset), el_nldq_offset_v.u32);

    return;
}

hi_void vdp_vid_setcomposerelnldqcoefv(vdp_regs_type *vdp_reg, hi_u32 offset,
    hi_s32 el_nldq_coef_v[3]) /* 3 is number */
{
    u_el_nldq_v0_coef el_nldq_coef_v0;
    u_el_nldq_v1_coef el_nldq_coef_v1;
    u_el_nldq_v2_coef el_nldq_coef_v2;

    el_nldq_coef_v0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->el_nldq_v0_coef.u32) + offset));
    el_nldq_coef_v1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->el_nldq_v1_coef.u32) + offset));
    el_nldq_coef_v2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->el_nldq_v2_coef.u32) + offset));
    el_nldq_coef_v0.bits.el_nldq_coef_0_v = el_nldq_coef_v[0];
    el_nldq_coef_v1.bits.el_nldq_coef_1_v = el_nldq_coef_v[1];
    el_nldq_coef_v2.bits.el_nldq_coef_2_v = el_nldq_coef_v[2]; // 2 is reg index
    vdp_regwrite((uintptr_t)(&(vdp_reg->el_nldq_v0_coef.u32) + offset), el_nldq_coef_v0.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->el_nldq_v1_coef.u32) + offset), el_nldq_coef_v1.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->el_nldq_v2_coef.u32) + offset), el_nldq_coef_v2.u32);

    return;
}

hi_void vdp_vid_setcomposerupsamplemax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 upsample_max)
{
    u_composer_us_clip composer_us_clip;

    composer_us_clip.u32 = vdp_regread((uintptr_t)(&(vdp_reg->composer_us_clip.u32) + offset));
    composer_us_clip.bits.hdr_us_max = upsample_max;
    vdp_regwrite((uintptr_t)(&(vdp_reg->composer_us_clip.u32) + offset), composer_us_clip.u32);

    return;
}

hi_void vdp_vid_setcomposerupsamplemin(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 upsample_min)
{
    u_composer_us_clip composer_us_clip;

    composer_us_clip.u32 = vdp_regread((uintptr_t)(&(vdp_reg->composer_us_clip.u32) + offset));
    composer_us_clip.bits.hdr_us_min = upsample_min;
    vdp_regwrite((uintptr_t)(&(vdp_reg->composer_us_clip.u32) + offset), composer_us_clip.u32);

    return;
}


