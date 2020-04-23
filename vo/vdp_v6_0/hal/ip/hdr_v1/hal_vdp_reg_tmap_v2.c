/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_reg_tmap_v2.h"
#include "hal_vdp_comm.h"

hi_void vdp_tmap_v2_settmrshiftrounden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tm_rshift_round_en)
{
    u_stb_tonemap_ctrl stb_tonemap_ctrl;

    stb_tonemap_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->stb_tonemap_ctrl.u32) + offset));
    stb_tonemap_ctrl.bits.tm_rshift_round_en = tm_rshift_round_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->stb_tonemap_ctrl.u32) + offset), stb_tonemap_ctrl.u32);

    return ;
}

hi_void vdp_tmap_v2_setstbtonemapen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 stb_tonemap_en)
{
    u_stb_tonemap_ctrl stb_tonemap_ctrl;

    stb_tonemap_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->stb_tonemap_ctrl.u32) + offset));
    stb_tonemap_ctrl.bits.stb_tonemap_en = stb_tonemap_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->stb_tonemap_ctrl.u32) + offset), stb_tonemap_ctrl.u32);

    return ;
}

hi_void vdp_tmap_v2_settmapsclutrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_sclut_rd_en)
{
    u_stb_tonemap_para_ren stb_tonemap_para_ren;

    stb_tonemap_para_ren.u32 = vdp_regread((uintptr_t)(&(vdp_reg->stb_tonemap_para_ren.u32) + offset));
    stb_tonemap_para_ren.bits.tmap_sclut_rd_en = tmap_sclut_rd_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->stb_tonemap_para_ren.u32) + offset), stb_tonemap_para_ren.u32);

    return ;
}

hi_void vdp_tmap_v2_settmapsslutrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_sslut_rd_en)
{
    u_stb_tonemap_para_ren stb_tonemap_para_ren;

    stb_tonemap_para_ren.u32 = vdp_regread((uintptr_t)(&(vdp_reg->stb_tonemap_para_ren.u32) + offset));
    stb_tonemap_para_ren.bits.tmap_sslut_rd_en = tmap_sslut_rd_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->stb_tonemap_para_ren.u32) + offset), stb_tonemap_para_ren.u32);

    return ;
}

hi_void vdp_tmap_v2_settmaptslutrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_tslut_rd_en)
{
    u_stb_tonemap_para_ren stb_tonemap_para_ren;

    stb_tonemap_para_ren.u32 = vdp_regread((uintptr_t)(&(vdp_reg->stb_tonemap_para_ren.u32) + offset));
    stb_tonemap_para_ren.bits.tmap_tslut_rd_en = tmap_tslut_rd_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->stb_tonemap_para_ren.u32) + offset), stb_tonemap_para_ren.u32);

    return ;
}

hi_void vdp_tmap_v2_settmapsilutrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_silut_rd_en)
{
    u_stb_tonemap_para_ren stb_tonemap_para_ren;

    stb_tonemap_para_ren.u32 = vdp_regread((uintptr_t)(&(vdp_reg->stb_tonemap_para_ren.u32) + offset));
    stb_tonemap_para_ren.bits.tmap_silut_rd_en = tmap_silut_rd_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->stb_tonemap_para_ren.u32) + offset), stb_tonemap_para_ren.u32);

    return ;
}

hi_void vdp_tmap_v2_settmaptilutrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_tilut_rd_en)
{
    u_stb_tonemap_para_ren stb_tonemap_para_ren;

    stb_tonemap_para_ren.u32 = vdp_regread((uintptr_t)(&(vdp_reg->stb_tonemap_para_ren.u32) + offset));
    stb_tonemap_para_ren.bits.tmap_tilut_rd_en = tmap_tilut_rd_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->stb_tonemap_para_ren.u32) + offset), stb_tonemap_para_ren.u32);

    return ;
}

hi_void vdp_tmap_v2_settmapparardata(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_para_rdata)
{
    u_stb_tonemap_para_rdata stb_tonemap_para_rdata;

    stb_tonemap_para_rdata.u32 = vdp_regread((uintptr_t)(&(vdp_reg->stb_tonemap_para_rdata.u32) + offset));
    stb_tonemap_para_rdata.bits.tmap_para_rdata = tmap_para_rdata;
    vdp_regwrite((uintptr_t)(&(vdp_reg->stb_tonemap_para_rdata.u32) + offset), stb_tonemap_para_rdata.u32);

    return ;
}

hi_void vdp_tmap_v2_settmc1expan(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tm_c1_expan)
{
    u_stb_tonemap_smc stb_tonemap_smc;

    stb_tonemap_smc.u32 = vdp_regread((uintptr_t)(&(vdp_reg->stb_tonemap_smc.u32) + offset));
    stb_tonemap_smc.bits.tm_c1_expan = tm_c1_expan;
    vdp_regwrite((uintptr_t)(&(vdp_reg->stb_tonemap_smc.u32) + offset), stb_tonemap_smc.u32);

    return ;
}

hi_void vdp_tmap_v2_settmsmcenable(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tm_smc_enable)
{
    u_stb_tonemap_smc stb_tonemap_smc;

    stb_tonemap_smc.u32 = vdp_regread((uintptr_t)(&(vdp_reg->stb_tonemap_smc.u32) + offset));
    stb_tonemap_smc.bits.tm_smc_enable = tm_smc_enable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->stb_tonemap_smc.u32) + offset), stb_tonemap_smc.u32);

    return ;
}

hi_void vdp_tmap_v2_setimtmrshiftbit(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imtm_rshift_bit)
{
    u_stb_tonemap_odw_proc stb_tonemap_odw_proc;

    stb_tonemap_odw_proc.u32 = vdp_regread((uintptr_t)(&(vdp_reg->stb_tonemap_odw_proc.u32) + offset));
    stb_tonemap_odw_proc.bits.imtm_rshift_bit = imtm_rshift_bit;
    vdp_regwrite((uintptr_t)(&(vdp_reg->stb_tonemap_odw_proc.u32) + offset), stb_tonemap_odw_proc.u32);

    return ;
}

hi_void vdp_tmap_v2_setimtmrshiften(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imtm_rshift_en)
{
    u_stb_tonemap_odw_proc stb_tonemap_odw_proc;

    stb_tonemap_odw_proc.u32 = vdp_regread((uintptr_t)(&(vdp_reg->stb_tonemap_odw_proc.u32) + offset));
    stb_tonemap_odw_proc.bits.imtm_rshift_en = imtm_rshift_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->stb_tonemap_odw_proc.u32) + offset), stb_tonemap_odw_proc.u32);

    return ;
}

hi_void vdp_tmap_v2_setimtms2uen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imtm_s2u_en)
{
    u_stb_tonemap_odw_proc stb_tonemap_odw_proc;

    stb_tonemap_odw_proc.u32 = vdp_regread((uintptr_t)(&(vdp_reg->stb_tonemap_odw_proc.u32) + offset));
    stb_tonemap_odw_proc.bits.imtm_s2u_en = imtm_s2u_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->stb_tonemap_odw_proc.u32) + offset), stb_tonemap_odw_proc.u32);

    return ;
}
