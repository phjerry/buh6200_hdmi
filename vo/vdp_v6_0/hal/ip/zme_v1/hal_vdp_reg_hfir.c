/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_reg_hfir.h"
#include "hal_vdp_comm.h"

hi_void vdp_vid_setifirmode(vdp_regs_type *vdp_reg, hi_u32 data, vdp_ifirmode enmode)
{
    u_v3_hfir_ctrl v3_hfir_ctrl;

    if (data >= VID_MAX) {
        return ;
    }

    v3_hfir_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_hfir_ctrl.u32)));
    v3_hfir_ctrl.bits.hfir_mode = enmode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_hfir_ctrl.u32)), v3_hfir_ctrl.u32);

    return ;
}

hi_void vdp_vid_setifirmiden(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32en)
{
    u_v3_hfir_ctrl v3_hfir_ctrl;

    if (data >= VID_MAX) {
        return ;
    }

    v3_hfir_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_hfir_ctrl.u32)));
    v3_hfir_ctrl.bits.mid_en = u32en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_hfir_ctrl.u32)), v3_hfir_ctrl.u32);
    return ;
}

hi_void vdp_vid_setifirckgten(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32en)
{
    u_v3_hfir_ctrl v3_hfir_ctrl;

    if (data >= VID_MAX) {
        return ;
    }

    v3_hfir_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_hfir_ctrl.u32)));
    v3_hfir_ctrl.bits.ck_gt_en = u32en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_hfir_ctrl.u32)), v3_hfir_ctrl.u32);
    return ;
}

hi_void vdp_vid_setifircoef(vdp_regs_type *vdp_reg, hi_u32 data, hi_s32 *coef)
{
    u_v3_ifircoef01 v3_ifircoef01;
    u_v3_ifircoef23 v3_ifircoef23;
    u_v3_ifircoef45 v3_ifircoef45;
    u_v3_ifircoef67 v3_ifircoef67;

    if (data >= VID_MAX) {
        return ;
    }

    v3_ifircoef01.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_ifircoef01.u32)));
    v3_ifircoef23.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_ifircoef23.u32)));
    v3_ifircoef45.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_ifircoef45.u32)));
    v3_ifircoef67.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_ifircoef67.u32)));

    v3_ifircoef01.bits.coef0 = coef[0];
    v3_ifircoef01.bits.coef1 = coef[1];
    v3_ifircoef23.bits.coef2 = coef[2]; /* 2 is number */
    v3_ifircoef23.bits.coef3 = coef[3]; /* 3 is number */
    v3_ifircoef45.bits.coef4 = coef[4]; /* 4 is number */
    v3_ifircoef45.bits.coef5 = coef[5]; /* 5 is number */
    v3_ifircoef67.bits.coef6 = coef[6]; /* 6 is number */
    v3_ifircoef67.bits.coef7 = coef[7]; /* 7 is number */

    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_ifircoef01.u32)), v3_ifircoef01.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_ifircoef23.u32)), v3_ifircoef23.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_ifircoef45.u32)), v3_ifircoef45.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_ifircoef67.u32)), v3_ifircoef67.u32);

    return ;
}

