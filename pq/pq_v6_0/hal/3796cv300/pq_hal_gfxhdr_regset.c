/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "pq_hal_comm.h"
#include "drv_pq_ext.h"
#include "pq_hal_gfxhdr_regset.h"
#include "pq_hal_hdrv2_regset_comm.h"

#define PQ_PARAUP_UP_OFFSET 96
#define GFX_OFFSET          (0x800 / 4)

hi_void pq_para_setparaupdispchn(vdp_regs_type *vdp_reg, hi_u32 u32chnnum)
{
    u_para_up_disp para_up_disp;

    para_up_disp.u32 = (1 << (u32chnnum - PQ_PARAUP_UP_OFFSET));
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_up_disp.u32)), para_up_disp.u32);

    return;
}

hi_void pq_para_setparaaddrdispchn09(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_disp_chn09 para_addr_disp_chn09;

    para_addr_disp_chn09.bits.para_addr_disp_chn09 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_disp_chn09.u32)), para_addr_disp_chn09.u32);

    return;
}

hi_void pq_para_setparaaddrdispchn10(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_disp_chn10 para_addr_disp_chn10;

    para_addr_disp_chn10.bits.para_addr_disp_chn10 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_disp_chn10.u32)), para_addr_disp_chn10.u32);

    return;
}

hi_void pq_para_setparaaddrdispchn11(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_disp_chn11 para_addr_disp_chn11;

    para_addr_disp_chn11.bits.para_addr_disp_chn11 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_disp_chn11.u32)), para_addr_disp_chn11.u32);

    return;
}

hi_void pq_para_setparaaddrdispchn12(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_disp_chn12 para_addr_disp_chn12;

    para_addr_disp_chn12.bits.para_addr_disp_chn12 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_disp_chn12.u32)), para_addr_disp_chn12.u32);

    return;
}

hi_void pq_para_setparahaddrdispchn09(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_disp_chn09 para_haddr_disp_chn09;

    para_haddr_disp_chn09.bits.para_haddr_disp_chn09 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_haddr_disp_chn09.u32)), para_haddr_disp_chn09.u32);

    return;
}

hi_void pq_para_setparahaddrdispchn10(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_disp_chn10 para_haddr_disp_chn10;

    para_haddr_disp_chn10.bits.para_haddr_disp_chn10 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_haddr_disp_chn10.u32)), para_haddr_disp_chn10.u32);

    return;
}

hi_void pq_para_setparahaddrdispchn11(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_disp_chn11 para_haddr_disp_chn11;

    para_haddr_disp_chn11.bits.para_haddr_disp_chn11 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_haddr_disp_chn11.u32)), para_haddr_disp_chn11.u32);

    return;
}

hi_void pq_para_setparahaddrdispchn12(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_disp_chn12 para_haddr_disp_chn12;

    para_haddr_disp_chn12.bits.para_haddr_disp_chn12 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_haddr_disp_chn12.u32)), para_haddr_disp_chn12.u32);

    return;
}

hi_void pq_hdr_setregup(vdp_regs_type *vdp_reg)
{
    u_db_hdr_upd db_hdr_upd;
    u_gp0_upd gp0_upd;
    hi_u32 data = 0;

    db_hdr_upd.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_hdr_upd.u32)));
    db_hdr_upd.bits.regup = 1;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_hdr_upd.u32)), db_hdr_upd.u32);

    gp0_upd.bits.regup = 1;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->gp0_upd.u32) + data * GFX_OFFSET), gp0_upd.u32);

    return;
}
