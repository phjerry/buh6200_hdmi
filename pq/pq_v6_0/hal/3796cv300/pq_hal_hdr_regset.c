/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hdr regset api
 * Author: pq
 * Create: 2016-01-1
 */

#include <linux/string.h>
#include "hi_type.h"
#include "pq_mng_hdr.h"
#include "pq_hal_comm.h"
#include "pq_hal_hdr.h"
#include "pq_hal_hdr_regset.h"
#include "pq_hal_hdrv2_regset_comm.h"

hi_void pq_para_setparaaddrvhdchn07(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn07 para_addr_vhd_chn07;

    para_addr_vhd_chn07.bits.para_addr_vhd_chn07 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_vhd_chn07.u32)), para_addr_vhd_chn07.u32);

    return;
}

hi_void pq_para_setparaaddrvhdchn08(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn08 para_addr_vhd_chn08;

    para_addr_vhd_chn08.bits.para_addr_vhd_chn08 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_vhd_chn08.u32)), para_addr_vhd_chn08.u32);

    return;
}

hi_void pq_para_setparaaddrvhdchn19(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn19 para_addr_vhd_chn19;

    para_addr_vhd_chn19.bits.para_addr_vhd_chn19 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_vhd_chn19.u32)), para_addr_vhd_chn19.u32);

    return;
}

hi_void pq_para_setparaaddrv0chn01(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn01 para_addr_v0_chn01;

    para_addr_v0_chn01.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn01.u32)));
    para_addr_v0_chn01.bits.para_addr_v0_chn01 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_v0_chn01.u32)), para_addr_v0_chn01.u32);

    return;
}

hi_void pq_para_setparaaddrv0chn02(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn02 para_addr_v0_chn02;

    para_addr_v0_chn02.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn02.u32)));
    para_addr_v0_chn02.bits.para_addr_v0_chn02 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_v0_chn02.u32)), para_addr_v0_chn02.u32);

    return;
}

hi_void pq_para_setparaaddrv0chn03(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn03 para_addr_v0_chn03;

    para_addr_v0_chn03.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn03.u32)));
    para_addr_v0_chn03.bits.para_addr_v0_chn03 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_v0_chn03.u32)), para_addr_v0_chn03.u32);

    return;
}

hi_void pq_para_setparaaddrv0chn04(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn04 para_addr_v0_chn04;

    para_addr_v0_chn04.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn04.u32)));
    para_addr_v0_chn04.bits.para_addr_v0_chn04 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_v0_chn04.u32)), para_addr_v0_chn04.u32);

    return;
}

hi_void pq_para_setparaaddrv0chn05(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn05 para_addr_v0_chn05;

    para_addr_v0_chn05.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn05.u32)));
    para_addr_v0_chn05.bits.para_addr_v0_chn05 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_v0_chn05.u32)), para_addr_v0_chn05.u32);

    return;
}

hi_void pq_para_setparaaddrv0chn06(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn06 para_addr_v0_chn06;

    para_addr_v0_chn06.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn06.u32)));
    para_addr_v0_chn06.bits.para_addr_v0_chn06 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_v0_chn06.u32)), para_addr_v0_chn06.u32);

    return;
}

hi_void pq_para_setparaaddrv0chn07(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn07 para_addr_v0_chn07;

    para_addr_v0_chn07.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn07.u32)));
    para_addr_v0_chn07.bits.para_addr_v0_chn07 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_v0_chn07.u32)), para_addr_v0_chn07.u32);

    return;
}

hi_void pq_para_setparaaddrv0chn08(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn08 para_addr_v0_chn08;

    para_addr_v0_chn08.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn08.u32)));
    para_addr_v0_chn08.bits.para_addr_v0_chn08 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_v0_chn08.u32)), para_addr_v0_chn08.u32);

    return;
}

hi_void pq_para_setparaaddrv0chn09(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn09 para_addr_v0_chn09;

    para_addr_v0_chn09.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn09.u32)));
    para_addr_v0_chn09.bits.para_addr_v0_chn09 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_v0_chn09.u32)), para_addr_v0_chn09.u32);

    return;
}

hi_s32 pq_cfg_distribute_addr(pq_hdr_coef_addr *vdp_coef_buf_addr)
{
    vdp_regs_type *vdp_reg = HI_NULL;

    vdp_reg = pq_hal_get_vdp_set_reg();
    PQ_CHECK_NULL_PTR_RE_FAIL(vdp_reg);

    pq_para_setparaaddrvhdchn07(vdp_reg, vdp_coef_buf_addr->phy_addr[PQ_COEF_BUF_OM_DEGMM]);
    pq_para_setparaaddrvhdchn08(vdp_reg, vdp_coef_buf_addr->phy_addr[PQ_COEF_BUF_OM_GMM]);
    pq_para_setparaaddrvhdchn19(vdp_reg, vdp_coef_buf_addr->phy_addr[PQ_COEF_BUF_OM_CM]);
    pq_para_setparaaddrv0chn01(vdp_reg, vdp_coef_buf_addr->phy_addr[PQ_COEF_BUF_V0_HDR0]);
    pq_para_setparaaddrv0chn02(vdp_reg, vdp_coef_buf_addr->phy_addr[PQ_COEF_BUF_V0_HDR1]);
    pq_para_setparaaddrv0chn03(vdp_reg, vdp_coef_buf_addr->phy_addr[PQ_COEF_BUF_V0_HDR2]);
    pq_para_setparaaddrv0chn04(vdp_reg, vdp_coef_buf_addr->phy_addr[PQ_COEF_BUF_V0_HDR3]);
    pq_para_setparaaddrv0chn05(vdp_reg, vdp_coef_buf_addr->phy_addr[PQ_COEF_BUF_V0_HDR4]);
    pq_para_setparaaddrv0chn06(vdp_reg, vdp_coef_buf_addr->phy_addr[PQ_COEF_BUF_V0_HDR5]);
    pq_para_setparaaddrv0chn07(vdp_reg, vdp_coef_buf_addr->phy_addr[PQ_COEF_BUF_V0_HDR6]);
    pq_para_setparaaddrv0chn08(vdp_reg, vdp_coef_buf_addr->phy_addr[PQ_COEF_BUF_V0_HDR7]);
    pq_para_setparaaddrv0chn09(vdp_reg, vdp_coef_buf_addr->phy_addr[PQ_COEF_BUF_V0_HDR8]);

    return HI_SUCCESS;
}

/********************************************************************************************/

hi_void pq_vhdr_cm_setvhdrcmckgten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_cm_ck_gt_en)
{
    u_vhdr_cm_ctrl vhdr_cm_ctrl;

    vhdr_cm_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_ctrl.u32) + offset));
    vhdr_cm_ctrl.bits.vhdr_cm_ck_gt_en = vhdr_cm_ck_gt_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_ctrl.u32) + offset), vhdr_cm_ctrl.u32);

    return;
}

hi_void pq_vhdr_cm_setvhdrcmen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_cm_en)
{
    u_vhdr_cm_ctrl vhdr_cm_ctrl;

    vhdr_cm_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_ctrl.u32) + offset));
    vhdr_cm_ctrl.bits.vhdr_cm_en = vhdr_cm_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_ctrl.u32) + offset), vhdr_cm_ctrl.u32);

    return;
}

hi_void pq_vhdr_cm_setvhdrcmclipcmax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_cm_clip_c_max)
{
    u_vhdr_cm_clip vhdr_cm_clip;

    vhdr_cm_clip.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_clip.u32) + offset));
    vhdr_cm_clip.bits.vhdr_cm_clip_c_max = vhdr_cm_clip_c_max;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_clip.u32) + offset), vhdr_cm_clip.u32);

    return;
}

hi_void pq_vhdr_cm_setvhdrcmclipcmin(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_cm_clip_c_min)
{
    u_vhdr_cm_clip vhdr_cm_clip;

    vhdr_cm_clip.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_clip.u32) + offset));
    vhdr_cm_clip.bits.vhdr_cm_clip_c_min = vhdr_cm_clip_c_min;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_clip.u32) + offset), vhdr_cm_clip.u32);

    return;
}

hi_void pq_vhdr_cm_setvhdrcmscale2p(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vhdr_cm_scale2p vhdr_cm_scale2p;

    vhdr_cm_scale2p.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_scale2p.u32) + offset));
    vhdr_cm_scale2p.bits.vhdr_cm_scale2p = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_scale2p.u32) + offset), vhdr_cm_scale2p.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx4step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x4_step)
{
    u_vhdr_cm_step1 vhdr_cm_step1;

    vhdr_cm_step1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_step1.u32) + offset));
    vhdr_cm_step1.bits.v_cm_x4_step = v_cm_x4_step;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_step1.u32) + offset), vhdr_cm_step1.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx3step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x3_step)
{
    u_vhdr_cm_step1 vhdr_cm_step1;

    vhdr_cm_step1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_step1.u32) + offset));
    vhdr_cm_step1.bits.v_cm_x3_step = v_cm_x3_step;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_step1.u32) + offset), vhdr_cm_step1.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx2step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x2_step)
{
    u_vhdr_cm_step1 vhdr_cm_step1;

    vhdr_cm_step1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_step1.u32) + offset));
    vhdr_cm_step1.bits.v_cm_x2_step = v_cm_x2_step;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_step1.u32) + offset), vhdr_cm_step1.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx1step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x1_step)
{
    u_vhdr_cm_step1 vhdr_cm_step1;

    vhdr_cm_step1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_step1.u32) + offset));
    vhdr_cm_step1.bits.v_cm_x1_step = v_cm_x1_step;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_step1.u32) + offset), vhdr_cm_step1.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx8step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x8_step)
{
    u_vhdr_cm_step2 vhdr_cm_step2;

    vhdr_cm_step2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_step2.u32) + offset));
    vhdr_cm_step2.bits.v_cm_x8_step = v_cm_x8_step;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_step2.u32) + offset), vhdr_cm_step2.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx7step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x7_step)
{
    u_vhdr_cm_step2 vhdr_cm_step2;

    vhdr_cm_step2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_step2.u32) + offset));
    vhdr_cm_step2.bits.v_cm_x7_step = v_cm_x7_step;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_step2.u32) + offset), vhdr_cm_step2.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx6step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x6_step)
{
    u_vhdr_cm_step2 vhdr_cm_step2;

    vhdr_cm_step2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_step2.u32) + offset));
    vhdr_cm_step2.bits.v_cm_x6_step = v_cm_x6_step;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_step2.u32) + offset), vhdr_cm_step2.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx5step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x5_step)
{
    u_vhdr_cm_step2 vhdr_cm_step2;

    vhdr_cm_step2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_step2.u32) + offset));
    vhdr_cm_step2.bits.v_cm_x5_step = v_cm_x5_step;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_step2.u32) + offset), vhdr_cm_step2.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx1pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x1_pos)
{
    u_vhdr_cm_pos1 vhdr_cm_pos1;

    vhdr_cm_pos1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_pos1.u32) + offset));
    vhdr_cm_pos1.bits.v_cm_x1_pos = v_cm_x1_pos;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_pos1.u32) + offset), vhdr_cm_pos1.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx2pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x2_pos)
{
    u_vhdr_cm_pos2 vhdr_cm_pos2;

    vhdr_cm_pos2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_pos2.u32) + offset));
    vhdr_cm_pos2.bits.v_cm_x2_pos = v_cm_x2_pos;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_pos2.u32) + offset), vhdr_cm_pos2.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx3pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x3_pos)
{
    u_vhdr_cm_pos3 vhdr_cm_pos3;

    vhdr_cm_pos3.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_pos3.u32) + offset));
    vhdr_cm_pos3.bits.v_cm_x3_pos = v_cm_x3_pos;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_pos3.u32) + offset), vhdr_cm_pos3.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx4pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x4_pos)
{
    u_vhdr_cm_pos4 vhdr_cm_pos4;

    vhdr_cm_pos4.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_pos4.u32) + offset));
    vhdr_cm_pos4.bits.v_cm_x4_pos = v_cm_x4_pos;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_pos4.u32) + offset), vhdr_cm_pos4.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx5pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x5_pos)
{
    u_vhdr_cm_pos5 vhdr_cm_pos5;

    vhdr_cm_pos5.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_pos5.u32) + offset));
    vhdr_cm_pos5.bits.v_cm_x5_pos = v_cm_x5_pos;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_pos5.u32) + offset), vhdr_cm_pos5.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx6pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x6_pos)
{
    u_vhdr_cm_pos6 vhdr_cm_pos6;

    vhdr_cm_pos6.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_pos6.u32) + offset));
    vhdr_cm_pos6.bits.v_cm_x6_pos = v_cm_x6_pos;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_pos6.u32) + offset), vhdr_cm_pos6.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx7pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x7_pos)
{
    u_vhdr_cm_pos7 vhdr_cm_pos7;

    vhdr_cm_pos7.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_pos7.u32) + offset));
    vhdr_cm_pos7.bits.v_cm_x7_pos = v_cm_x7_pos;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_pos7.u32) + offset), vhdr_cm_pos7.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx8pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x8_pos)
{
    u_vhdr_cm_pos8 vhdr_cm_pos8;

    vhdr_cm_pos8.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_pos8.u32) + offset));
    vhdr_cm_pos8.bits.v_cm_x8_pos = v_cm_x8_pos;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_pos8.u32) + offset), vhdr_cm_pos8.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx4num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x4_num)
{
    u_vhdr_cm_num1 vhdr_cm_num1;

    vhdr_cm_num1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_num1.u32) + offset));
    vhdr_cm_num1.bits.v_cm_x4_num = v_cm_x4_num;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_num1.u32) + offset), vhdr_cm_num1.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx3num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x3_num)
{
    u_vhdr_cm_num1 vhdr_cm_num1;

    vhdr_cm_num1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_num1.u32) + offset));
    vhdr_cm_num1.bits.v_cm_x3_num = v_cm_x3_num;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_num1.u32) + offset), vhdr_cm_num1.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx2num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x2_num)
{
    u_vhdr_cm_num1 vhdr_cm_num1;

    vhdr_cm_num1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_num1.u32) + offset));
    vhdr_cm_num1.bits.v_cm_x2_num = v_cm_x2_num;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_num1.u32) + offset), vhdr_cm_num1.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx1num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x1_num)
{
    u_vhdr_cm_num1 vhdr_cm_num1;

    vhdr_cm_num1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_num1.u32) + offset));
    vhdr_cm_num1.bits.v_cm_x1_num = v_cm_x1_num;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_num1.u32) + offset), vhdr_cm_num1.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx8num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x8_num)
{
    u_vhdr_cm_num2 vhdr_cm_num2;

    vhdr_cm_num2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_num2.u32) + offset));
    vhdr_cm_num2.bits.v_cm_x8_num = v_cm_x8_num;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_num2.u32) + offset), vhdr_cm_num2.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx7num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x7_num)
{
    u_vhdr_cm_num2 vhdr_cm_num2;

    vhdr_cm_num2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_num2.u32) + offset));
    vhdr_cm_num2.bits.v_cm_x7_num = v_cm_x7_num;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_num2.u32) + offset), vhdr_cm_num2.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx6num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x6_num)
{
    u_vhdr_cm_num2 vhdr_cm_num2;

    vhdr_cm_num2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_num2.u32) + offset));
    vhdr_cm_num2.bits.v_cm_x6_num = v_cm_x6_num;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_num2.u32) + offset), vhdr_cm_num2.u32);

    return;
}

hi_void pq_vhdr_cm_setvcmx5num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x5_num)
{
    u_vhdr_cm_num2 vhdr_cm_num2;

    vhdr_cm_num2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vhdr_cm_num2.u32) + offset));
    vhdr_cm_num2.bits.v_cm_x5_num = v_cm_x5_num;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->vhdr_cm_num2.u32) + offset), vhdr_cm_num2.u32);

    return;
}
static hi_void pq_set_hdr_cm_mode(vdp_regs_type *vdp_reg, hi_u32 offset, pq_hdrv1_cm_cfg *cm_cfg)
{
    pq_vhdr_cm_setvhdrcmckgten(vdp_reg, offset, HI_TRUE);
    pq_vhdr_cm_setvhdrcmen(vdp_reg, offset, cm_cfg->enable);
    pq_vhdr_cm_setvhdrcmclipcmax(vdp_reg, offset, cm_cfg->clip_c_max);
    pq_vhdr_cm_setvhdrcmclipcmin(vdp_reg, offset, cm_cfg->clip_c_min);
    pq_vhdr_cm_setvhdrcmscale2p(vdp_reg, offset, cm_cfg->scale2p);
    pq_vhdr_cm_setvcmx4step(vdp_reg, offset, cm_cfg->step[3]);  /* 3 is reg index */
    pq_vhdr_cm_setvcmx3step(vdp_reg, offset, cm_cfg->step[2]);  /* 2 is reg index */
    pq_vhdr_cm_setvcmx2step(vdp_reg, offset, cm_cfg->step[1]);
    pq_vhdr_cm_setvcmx1step(vdp_reg, offset, cm_cfg->step[0]);
    pq_vhdr_cm_setvcmx8step(vdp_reg, offset, cm_cfg->step[7]);  /* 7 is reg index */
    pq_vhdr_cm_setvcmx7step(vdp_reg, offset, cm_cfg->step[6]);  /* 6 is reg index */
    pq_vhdr_cm_setvcmx6step(vdp_reg, offset, cm_cfg->step[5]);  /* 5 is reg index */
    pq_vhdr_cm_setvcmx5step(vdp_reg, offset, cm_cfg->step[4]);  /* 4 is reg index */
    pq_vhdr_cm_setvcmx1pos(vdp_reg, offset, cm_cfg->pos[1]);
    pq_vhdr_cm_setvcmx2pos(vdp_reg, offset, cm_cfg->pos[1]);
    pq_vhdr_cm_setvcmx3pos(vdp_reg, offset, cm_cfg->pos[2]);  /* 2 is reg index */
    pq_vhdr_cm_setvcmx4pos(vdp_reg, offset, cm_cfg->pos[3]);  /* 3 is reg index */
    pq_vhdr_cm_setvcmx5pos(vdp_reg, offset, cm_cfg->pos[4]);  /* 4 is reg index */
    pq_vhdr_cm_setvcmx6pos(vdp_reg, offset, cm_cfg->pos[5]);  /* 5 is reg index */
    pq_vhdr_cm_setvcmx7pos(vdp_reg, offset, cm_cfg->pos[6]);  /* 6 is reg index */
    pq_vhdr_cm_setvcmx8pos(vdp_reg, offset, cm_cfg->pos[7]);  /* 7 is reg index */
    pq_vhdr_cm_setvcmx4num(vdp_reg, offset, cm_cfg->num[3]);  /* 3 is reg index */
    pq_vhdr_cm_setvcmx3num(vdp_reg, offset, cm_cfg->num[2]);  /* 2 is reg index */
    pq_vhdr_cm_setvcmx2num(vdp_reg, offset, cm_cfg->num[1]);
    pq_vhdr_cm_setvcmx1num(vdp_reg, offset, cm_cfg->num[0]);
    pq_vhdr_cm_setvcmx8num(vdp_reg, offset, cm_cfg->num[7]);  /* 7 is reg index */
    pq_vhdr_cm_setvcmx7num(vdp_reg, offset, cm_cfg->num[6]);  /* 6 is reg index */
    pq_vhdr_cm_setvcmx6num(vdp_reg, offset, cm_cfg->num[5]);  /* 5 is reg index */
    pq_vhdr_cm_setvcmx5num(vdp_reg, offset, cm_cfg->num[4]);  /* 4 is reg index */

    return;
}
/*****************************HDR V2*******************************************/
hi_void pq_hdr_setvcvmipt2lmsen(vdp_regs_type *vdp_reg, hi_u32 v_ipt2lms_en)
{
    u_db_cvm1_ipt2lms_ctrl db_cvm1_ipt2lms_ctrl;

    db_cvm1_ipt2lms_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_ctrl.u32)));
    db_cvm1_ipt2lms_ctrl.bits.cvm1_ipt2lms_en = v_ipt2lms_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_ctrl.u32)), db_cvm1_ipt2lms_ctrl.u32);

    return;
}

hi_void pq_hdr_setvcvmlms2rgben(vdp_regs_type *vdp_reg, hi_u32 v_lms2rgb_en)
{
    u_db_cvm1_lms2rgb_ctrl db_cvm1_lms2rgb_ctrl;

    db_cvm1_lms2rgb_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_ctrl.u32)));
    db_cvm1_lms2rgb_ctrl.bits.cvm1_lms2rgb_en = v_lms2rgb_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_ctrl.u32)), db_cvm1_lms2rgb_ctrl.u32);

    return;
}
hi_void pq_hdr_setvcvmrgb2yuven(vdp_regs_type *vdp_reg, hi_u32 v_rgb2yuv_en)
{
    u_db_cvm1_rgb2yuv_ctrl db_cvm1_rgb2yuv_ctrl;

    db_cvm1_rgb2yuv_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_ctrl.u32)));
    db_cvm1_rgb2yuv_ctrl.bits.cvm1_rgb2yuv_en = v_rgb2yuv_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_ctrl.u32)), db_cvm1_rgb2yuv_ctrl.u32);

    return;
}

hi_void pq_hdr_setvcvmomaprangeover(vdp_regs_type *vdp_reg, hi_u32 v_omap_range_over)
{
    u_db_cvm1_out_rangeoverone db_cvm1_out_rangeoverone;

    db_cvm1_out_rangeoverone.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_out_rangeoverone.u32)));
    db_cvm1_out_rangeoverone.bits.v_omap_range_over = v_omap_range_over;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_out_rangeoverone.u32)), db_cvm1_out_rangeoverone.u32);

    return;
}
hi_void pq_hdr_setcvmoutbits(vdp_regs_type *vdp_reg, hi_u32 cvm_out_bits)
{
    u_db_cvm_ctrl db_cvm_ctrl;

    db_cvm_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)));
    db_cvm_ctrl.bits.cvm_out_bits = cvm_out_bits;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)), db_cvm_ctrl.u32);

    return;
}
hi_void pq_hdr_setcvmoutcolor(vdp_regs_type *vdp_reg, hi_u32 cvm_out_color)
{
    u_db_cvm_ctrl db_cvm_ctrl;

    db_cvm_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)));
    db_cvm_ctrl.bits.cvm_out_color = cvm_out_color;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)), db_cvm_ctrl.u32);

    return;
}

hi_void pq_hdr_setvcvmomapiptoff(vdp_regs_type *vdp_reg, hi_s32 omap_iptoff[3])  /* 3 is reg index */
{
    u_db_cvm1_omap_iptoff0 db_cvm1_omap_iptoff0;
    u_db_cvm1_omap_iptoff1 db_cvm1_omap_iptoff1;
    u_db_cvm1_omap_iptoff2 db_cvm1_omap_iptoff2;

    db_cvm1_omap_iptoff0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_omap_iptoff0.u32)));
    db_cvm1_omap_iptoff1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_omap_iptoff1.u32)));
    db_cvm1_omap_iptoff2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_omap_iptoff2.u32)));
    db_cvm1_omap_iptoff0.bits.cvm_omap_ipt_0_off = omap_iptoff[0];
    db_cvm1_omap_iptoff1.bits.cvm_omap_ipt_1_off = omap_iptoff[1];
    db_cvm1_omap_iptoff2.bits.cvm_omap_ipt_2_off = omap_iptoff[2];  /* 2 is reg index */
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_omap_iptoff0.u32)), db_cvm1_omap_iptoff0.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_omap_iptoff1.u32)), db_cvm1_omap_iptoff1.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_omap_iptoff2.u32)), db_cvm1_omap_iptoff2.u32);
}

hi_void pq_hdr_setvcvmomapiptscale(vdp_regs_type *vdp_reg, hi_u32 scale)
{
    u_db_cvm1_omap_iptscale db_cvm1_omap_iptscale;

    db_cvm1_omap_iptscale.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_omap_iptscale.u32)));
    db_cvm1_omap_iptscale.bits.cvm_omap_ipt_scale = scale;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_omap_iptscale.u32)), db_cvm1_omap_iptscale.u32);
}

hi_void pq_hdr_setvdmomaplms2rgbmin(vdp_regs_type *vdp_reg, hi_u32 v_omap_lms2rgbmin)
{
    u_db_cvm1_lms2rgb_min db_cvm1_lms2rgb_min;

    db_cvm1_lms2rgb_min.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_min.u32)));
    db_cvm1_lms2rgb_min.bits.v_omap_lms2rgbmin = v_omap_lms2rgbmin;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_min.u32)), db_cvm1_lms2rgb_min.u32);

    return;
}

hi_void pq_hdr_setvdmomaplms2rgbmax(vdp_regs_type *vdp_reg, hi_u32 v_omap_lms2rgbmax)
{
    u_db_cvm1_lms2rgb_max db_cvm1_lms2rgb_max;

    db_cvm1_lms2rgb_max.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_max.u32)));
    db_cvm1_lms2rgb_max.bits.v_omap_lms2rgbmax = v_omap_lms2rgbmax;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_max.u32)), db_cvm1_lms2rgb_max.u32);

    return;
}

hi_void pq_hdr_setvdmomapipt2lmsmin(vdp_regs_type *vdp_reg, hi_u32 v_omap_ipt2lmsmin)
{
    u_db_cvm1_ipt2lms_min db_cvm1_ipt2lms_min;

    db_cvm1_ipt2lms_min.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_min.u32)));
    db_cvm1_ipt2lms_min.bits.v_omap_ipt2lmsmin = v_omap_ipt2lmsmin;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_min.u32)), db_cvm1_ipt2lms_min.u32);

    return;
}

hi_void pq_hdr_setvdmomapipt2lmsmax(vdp_regs_type *vdp_reg, hi_u32 v_omap_ipt2lmsmax)
{
    u_db_cvm1_ipt2lms_max db_cvm1_ipt2lms_max;

    db_cvm1_ipt2lms_max.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_max.u32)));
    db_cvm1_ipt2lms_max.bits.v_omap_ipt2lmsmax = v_omap_ipt2lmsmax;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_max.u32)), db_cvm1_ipt2lms_max.u32);

    return;
}

hi_void pq_hdr_setvcvmomaprgb2yuvmin(vdp_regs_type *vdp_reg, hi_u32 v_omap_ygb2yuvmin)
{
    u_db_cvm1_rgb2yuv_min db_cvm1_rgb2yuv_min;

    db_cvm1_rgb2yuv_min.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_min.u32)));
    db_cvm1_rgb2yuv_min.bits.v_omap_rgb2yuv_min = v_omap_ygb2yuvmin;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_min.u32)), db_cvm1_rgb2yuv_min.u32);

    return;
}

hi_void pq_hdr_setvcvmomaprgb2yuvmax(vdp_regs_type *vdp_reg, hi_u32 v_omap_ygb2yuvmax)
{
    u_db_cvm1_rgb2yuv_max db_cvm1_rgb2yuv_max;

    db_cvm1_rgb2yuv_max.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_max.u32)));
    db_cvm1_rgb2yuv_max.bits.v_omap_rgb2yuv_max = v_omap_ygb2yuvmax;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_max.u32)), db_cvm1_rgb2yuv_max.u32);

    return;
}

hi_void pq_hdr_setvcvmomapipt2lms(vdp_regs_type *vdp_reg, hi_s16 v_omap_ipt2lms[3][3])  /* 3 is reg index */
{
    u_db_cvm1_ipt2lms_coef0 db_cvm1_ipt2lms_coef0;
    u_db_cvm1_ipt2lms_coef1 db_cvm1_ipt2lms_coef1;
    u_db_cvm1_ipt2lms_coef2 db_cvm1_ipt2lms_coef2;
    u_db_cvm1_ipt2lms_coef3 db_cvm1_ipt2lms_coef3;
    u_db_cvm1_ipt2lms_coef4 db_cvm1_ipt2lms_coef4;
    u_db_cvm1_ipt2lms_coef5 db_cvm1_ipt2lms_coef5;
    u_db_cvm1_ipt2lms_coef6 db_cvm1_ipt2lms_coef6;
    u_db_cvm1_ipt2lms_coef7 db_cvm1_ipt2lms_coef7;
    u_db_cvm1_ipt2lms_coef8 db_cvm1_ipt2lms_coef8;

    db_cvm1_ipt2lms_coef0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef0.u32)));
    db_cvm1_ipt2lms_coef1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef1.u32)));
    db_cvm1_ipt2lms_coef2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef2.u32)));
    db_cvm1_ipt2lms_coef3.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef3.u32)));
    db_cvm1_ipt2lms_coef4.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef4.u32)));
    db_cvm1_ipt2lms_coef5.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef5.u32)));
    db_cvm1_ipt2lms_coef6.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef6.u32)));
    db_cvm1_ipt2lms_coef7.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef7.u32)));
    db_cvm1_ipt2lms_coef8.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef8.u32)));
    db_cvm1_ipt2lms_coef0.bits.v_omap_00_ipt2lms = v_omap_ipt2lms[0][0];
    db_cvm1_ipt2lms_coef1.bits.v_omap_01_ipt2lms = v_omap_ipt2lms[0][1];
    db_cvm1_ipt2lms_coef2.bits.v_omap_02_ipt2lms = v_omap_ipt2lms[0][2];  /* 2 is reg index */
    db_cvm1_ipt2lms_coef3.bits.v_omap_10_ipt2lms = v_omap_ipt2lms[1][0];
    db_cvm1_ipt2lms_coef4.bits.v_omap_11_ipt2lms = v_omap_ipt2lms[1][1];
    db_cvm1_ipt2lms_coef5.bits.v_omap_12_ipt2lms = v_omap_ipt2lms[1][2];  /* 2 is reg index */
    db_cvm1_ipt2lms_coef6.bits.v_omap_20_ipt2lms = v_omap_ipt2lms[2][0];  /* 2 is reg index */
    db_cvm1_ipt2lms_coef7.bits.v_omap_21_ipt2lms = v_omap_ipt2lms[2][1];  /* 2 is reg index */
    db_cvm1_ipt2lms_coef8.bits.v_omap_22_ipt2lms = v_omap_ipt2lms[2][2];  /* 2 is reg index */
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef0.u32)), db_cvm1_ipt2lms_coef0.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef1.u32)), db_cvm1_ipt2lms_coef1.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef2.u32)), db_cvm1_ipt2lms_coef2.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef3.u32)), db_cvm1_ipt2lms_coef3.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef4.u32)), db_cvm1_ipt2lms_coef4.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef5.u32)), db_cvm1_ipt2lms_coef5.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef6.u32)), db_cvm1_ipt2lms_coef6.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef7.u32)), db_cvm1_ipt2lms_coef7.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef8.u32)), db_cvm1_ipt2lms_coef8.u32);

    return;
}

hi_void pq_hdr_setvcvmomapipt2lmsscale2p(vdp_regs_type *vdp_reg, hi_u32 v_omap_ipt2lmsscale2p)
{
    u_db_cvm1_ipt2lms_scale2p db_cvm1_ipt2lms_scale2p;

    db_cvm1_ipt2lms_scale2p.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_scale2p.u32)));
    db_cvm1_ipt2lms_scale2p.bits.v_omap_ipt2lmsscale2p = v_omap_ipt2lmsscale2p;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_scale2p.u32)), db_cvm1_ipt2lms_scale2p.u32);

    return;
}

hi_void pq_hdr_setvcvmomaplms2rgb(vdp_regs_type *vdp_reg, hi_s16 v_omap_lms2rgb[3][3])  /* 3 is reg index */
{
    u_db_cvm1_lms2rgb_coef0 db_cvm1_lms2rgb_coef0;
    u_db_cvm1_lms2rgb_coef1 db_cvm1_lms2rgb_coef1;
    u_db_cvm1_lms2rgb_coef2 db_cvm1_lms2rgb_coef2;
    u_db_cvm1_lms2rgb_coef3 db_cvm1_lms2rgb_coef3;
    u_db_cvm1_lms2rgb_coef4 db_cvm1_lms2rgb_coef4;
    u_db_cvm1_lms2rgb_coef5 db_cvm1_lms2rgb_coef5;
    u_db_cvm1_lms2rgb_coef6 db_cvm1_lms2rgb_coef6;
    u_db_cvm1_lms2rgb_coef7 db_cvm1_lms2rgb_coef7;
    u_db_cvm1_lms2rgb_coef8 db_cvm1_lms2rgb_coef8;

    db_cvm1_lms2rgb_coef0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef0.u32)));
    db_cvm1_lms2rgb_coef1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef1.u32)));
    db_cvm1_lms2rgb_coef2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef2.u32)));
    db_cvm1_lms2rgb_coef3.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef3.u32)));
    db_cvm1_lms2rgb_coef4.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef4.u32)));
    db_cvm1_lms2rgb_coef5.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef5.u32)));
    db_cvm1_lms2rgb_coef6.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef6.u32)));
    db_cvm1_lms2rgb_coef7.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef7.u32)));
    db_cvm1_lms2rgb_coef8.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef8.u32)));
    db_cvm1_lms2rgb_coef0.bits.v_omap_00_lms2rgb = v_omap_lms2rgb[0][0];
    db_cvm1_lms2rgb_coef1.bits.v_omap_01_lms2rgb = v_omap_lms2rgb[0][1];
    db_cvm1_lms2rgb_coef2.bits.v_omap_02_lms2rgb = v_omap_lms2rgb[0][2];  /* 2 is reg index */
    db_cvm1_lms2rgb_coef3.bits.v_omap_10_lms2rgb = v_omap_lms2rgb[1][0];
    db_cvm1_lms2rgb_coef4.bits.v_omap_11_lms2rgb = v_omap_lms2rgb[1][1];
    db_cvm1_lms2rgb_coef5.bits.v_omap_12_lms2rgb = v_omap_lms2rgb[1][2];  /* 2 is reg index */
    db_cvm1_lms2rgb_coef6.bits.v_omap_20_lms2rgb = v_omap_lms2rgb[2][0];  /* 2 is reg index */
    db_cvm1_lms2rgb_coef7.bits.v_omap_21_lms2rgb = v_omap_lms2rgb[2][1];  /* 2 is reg index */
    db_cvm1_lms2rgb_coef8.bits.v_omap_22_lms2rgb = v_omap_lms2rgb[2][2];  /* 2 is reg index */
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef0.u32)), db_cvm1_lms2rgb_coef0.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef1.u32)), db_cvm1_lms2rgb_coef1.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef2.u32)), db_cvm1_lms2rgb_coef2.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef3.u32)), db_cvm1_lms2rgb_coef3.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef4.u32)), db_cvm1_lms2rgb_coef4.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef5.u32)), db_cvm1_lms2rgb_coef5.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef6.u32)), db_cvm1_lms2rgb_coef6.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef7.u32)), db_cvm1_lms2rgb_coef7.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef8.u32)), db_cvm1_lms2rgb_coef8.u32);

    return;
}

hi_void pq_hdr_setvcvmomaplms2rgbdemo(vdp_regs_type *vdp_reg, hi_s16 v_omap_lms2rgb[3][3])  /* 3 is reg index */
{
    u_db_cvm1_lms2rgb_coef0_demo db_cvm1_lms2rgb_coef0;
    u_db_cvm1_lms2rgb_coef1_demo db_cvm1_lms2rgb_coef1;
    u_db_cvm1_lms2rgb_coef2_demo db_cvm1_lms2rgb_coef2;
    u_db_cvm1_lms2rgb_coef3_demo db_cvm1_lms2rgb_coef3;
    u_db_cvm1_lms2rgb_coef4_demo db_cvm1_lms2rgb_coef4;
    u_db_cvm1_lms2rgb_coef5_demo db_cvm1_lms2rgb_coef5;
    u_db_cvm1_lms2rgb_coef6_demo db_cvm1_lms2rgb_coef6;
    u_db_cvm1_lms2rgb_coef7_demo db_cvm1_lms2rgb_coef7;
    u_db_cvm1_lms2rgb_coef8_demo db_cvm1_lms2rgb_coef8;

    db_cvm1_lms2rgb_coef0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef0.u32)));
    db_cvm1_lms2rgb_coef1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef1.u32)));
    db_cvm1_lms2rgb_coef2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef2.u32)));
    db_cvm1_lms2rgb_coef3.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef3.u32)));
    db_cvm1_lms2rgb_coef4.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef4.u32)));
    db_cvm1_lms2rgb_coef5.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef5.u32)));
    db_cvm1_lms2rgb_coef6.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef6.u32)));
    db_cvm1_lms2rgb_coef7.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef7.u32)));
    db_cvm1_lms2rgb_coef8.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef8.u32)));
    db_cvm1_lms2rgb_coef0.bits.v_omap_00_lms2rgb_demo = v_omap_lms2rgb[0][0];
    db_cvm1_lms2rgb_coef1.bits.v_omap_01_lms2rgb_demo = v_omap_lms2rgb[0][1];
    db_cvm1_lms2rgb_coef2.bits.v_omap_02_lms2rgb_demo = v_omap_lms2rgb[0][2];  /* 2 is reg index */
    db_cvm1_lms2rgb_coef3.bits.v_omap_10_lms2rgb_demo = v_omap_lms2rgb[1][0];
    db_cvm1_lms2rgb_coef4.bits.v_omap_11_lms2rgb_demo = v_omap_lms2rgb[1][1];
    db_cvm1_lms2rgb_coef5.bits.v_omap_12_lms2rgb_demo = v_omap_lms2rgb[1][2];  /* 2 is reg index */
    db_cvm1_lms2rgb_coef6.bits.v_omap_20_lms2rgb_demo = v_omap_lms2rgb[2][0];  /* 2 is reg index */
    db_cvm1_lms2rgb_coef7.bits.v_omap_21_lms2rgb_demo = v_omap_lms2rgb[2][1];  /* 2 is reg index */
    db_cvm1_lms2rgb_coef8.bits.v_omap_22_lms2rgb_demo = v_omap_lms2rgb[2][2];  /* 2 is reg index */
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef0.u32)), db_cvm1_lms2rgb_coef0.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef1.u32)), db_cvm1_lms2rgb_coef1.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef2.u32)), db_cvm1_lms2rgb_coef2.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef3.u32)), db_cvm1_lms2rgb_coef3.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef4.u32)), db_cvm1_lms2rgb_coef4.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef5.u32)), db_cvm1_lms2rgb_coef5.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef6.u32)), db_cvm1_lms2rgb_coef6.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef7.u32)), db_cvm1_lms2rgb_coef7.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef8.u32)), db_cvm1_lms2rgb_coef8.u32);

    return;
}

hi_void pq_hdr_setvcvmomaplms2rgbscale2p(vdp_regs_type *vdp_reg, hi_u32 v_omap_lms2rgbscale2p)
{
    u_db_cvm1_lms2rgb_scale2p db_cvm1_lms2rgb_scale2p;

    db_cvm1_lms2rgb_scale2p.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_scale2p.u32)));
    db_cvm1_lms2rgb_scale2p.bits.v_omap_lms2rgbscale2p = v_omap_lms2rgbscale2p;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_scale2p.u32)), db_cvm1_lms2rgb_scale2p.u32);

    return;
}

hi_void pq_hdr_setvcvmomaplms2rgbscale2pdemo(vdp_regs_type *vdp_reg, hi_u32 v_omap_lms2rgbscale2p)
{
    u_db_cvm1_lms2rgb_scale2p_demo db_cvm1_lms2rgb_scale2p;

    db_cvm1_lms2rgb_scale2p.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_scale2p.u32)));
    db_cvm1_lms2rgb_scale2p.bits.v_omap_lms2rgbscale2p_demo = v_omap_lms2rgbscale2p;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_scale2p.u32)), db_cvm1_lms2rgb_scale2p.u32);

    return;
}

hi_void pq_hdr_setvcvmomaprgb2yuv(vdp_regs_type *vdp_reg, hi_s16 v_omap_rgb2yuv[3][3])  /* 3 is reg index */
{
    u_db_cvm1_rgb2yuv_coef0 db_cvm1_rgb2yuv_coef0;
    u_db_cvm1_rgb2yuv_coef1 db_cvm1_rgb2yuv_coef1;
    u_db_cvm1_rgb2yuv_coef2 db_cvm1_rgb2yuv_coef2;
    u_db_cvm1_rgb2yuv_coef3 db_cvm1_rgb2yuv_coef3;
    u_db_cvm1_rgb2yuv_coef4 db_cvm1_rgb2yuv_coef4;
    u_db_cvm1_rgb2yuv_coef5 db_cvm1_rgb2yuv_coef5;
    u_db_cvm1_rgb2yuv_coef6 db_cvm1_rgb2yuv_coef6;
    u_db_cvm1_rgb2yuv_coef7 db_cvm1_rgb2yuv_coef7;
    u_db_cvm1_rgb2yuv_coef8 db_cvm1_rgb2yuv_coef8;

    db_cvm1_rgb2yuv_coef0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef0.u32)));
    db_cvm1_rgb2yuv_coef1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef1.u32)));
    db_cvm1_rgb2yuv_coef2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef2.u32)));
    db_cvm1_rgb2yuv_coef3.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef3.u32)));
    db_cvm1_rgb2yuv_coef4.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef4.u32)));
    db_cvm1_rgb2yuv_coef5.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef5.u32)));
    db_cvm1_rgb2yuv_coef6.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef6.u32)));
    db_cvm1_rgb2yuv_coef7.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef7.u32)));
    db_cvm1_rgb2yuv_coef8.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef8.u32)));
    db_cvm1_rgb2yuv_coef0.bits.v_omap_00_rgb2yuv = v_omap_rgb2yuv[0][0];
    db_cvm1_rgb2yuv_coef1.bits.v_omap_01_rgb2yuv = v_omap_rgb2yuv[0][1];
    db_cvm1_rgb2yuv_coef2.bits.v_omap_02_rgb2yuv = v_omap_rgb2yuv[0][2];  /* 2 is reg index */
    db_cvm1_rgb2yuv_coef3.bits.v_omap_10_rgb2yuv = v_omap_rgb2yuv[1][0];
    db_cvm1_rgb2yuv_coef4.bits.v_omap_11_rgb2yuv = v_omap_rgb2yuv[1][1];
    db_cvm1_rgb2yuv_coef5.bits.v_omap_12_rgb2yuv = v_omap_rgb2yuv[1][2];  /* 2 is reg index */
    db_cvm1_rgb2yuv_coef6.bits.v_omap_20_rgb2yuv = v_omap_rgb2yuv[2][0];  /* 2 is reg index */
    db_cvm1_rgb2yuv_coef7.bits.v_omap_21_rgb2yuv = v_omap_rgb2yuv[2][1];  /* 2 is reg index */
    db_cvm1_rgb2yuv_coef8.bits.v_omap_22_rgb2yuv = v_omap_rgb2yuv[2][2];  /* 2 is reg index */
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef0.u32)), db_cvm1_rgb2yuv_coef0.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef1.u32)), db_cvm1_rgb2yuv_coef1.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef2.u32)), db_cvm1_rgb2yuv_coef2.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef3.u32)), db_cvm1_rgb2yuv_coef3.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef4.u32)), db_cvm1_rgb2yuv_coef4.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef5.u32)), db_cvm1_rgb2yuv_coef5.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef6.u32)), db_cvm1_rgb2yuv_coef6.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef7.u32)), db_cvm1_rgb2yuv_coef7.u32);
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef8.u32)), db_cvm1_rgb2yuv_coef8.u32);

    return;
}

hi_void pq_hdr_setvcvmomaprgb2yuvscale2p(vdp_regs_type *vdp_reg, hi_u32 v_omap_rgb2yuvscale2p)
{
    u_db_cvm1_rgb2yuv_scale2p db_cvm1_rgb2yuv_scale2p;

    db_cvm1_rgb2yuv_scale2p.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_scale2p.u32)));
    db_cvm1_rgb2yuv_scale2p.bits.v_omap_rgb2yuvscale2p = v_omap_rgb2yuvscale2p;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_scale2p.u32)), db_cvm1_rgb2yuv_scale2p.u32);

    return;
}

hi_void pq_hdr_setvcvmomaprgb2yuvoutdc(vdp_regs_type *vdp_reg, hi_s32 *v_omap_rgb2yuv_out_dc)
{
    u_db_cvm1_rgb2yuv_out_dc0 db_cvm1_rgb2yuv_out_dc0;
    u_db_cvm1_rgb2yuv_out_dc1 db_cvm1_rgb2yuv_out_dc1;
    u_db_cvm1_rgb2yuv_out_dc2 db_cvm1_rgb2yuv_out_dc2;

    db_cvm1_rgb2yuv_out_dc0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_out_dc0.u32)));
    db_cvm1_rgb2yuv_out_dc0.bits.v_omap_rgb2yuv_0_out_dc = v_omap_rgb2yuv_out_dc[0];
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_out_dc0.u32)), db_cvm1_rgb2yuv_out_dc0.u32);

    db_cvm1_rgb2yuv_out_dc1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_out_dc1.u32)));
    db_cvm1_rgb2yuv_out_dc1.bits.v_omap_rgb2yuv_1_out_dc = v_omap_rgb2yuv_out_dc[1];
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_out_dc1.u32)), db_cvm1_rgb2yuv_out_dc1.u32);

    db_cvm1_rgb2yuv_out_dc2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_out_dc2.u32)));
    db_cvm1_rgb2yuv_out_dc2.bits.v_omap_rgb2yuv_2_out_dc = v_omap_rgb2yuv_out_dc[2];  /* 2 is reg index */
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_out_dc2.u32)), db_cvm1_rgb2yuv_out_dc2.u32);

    return;
}

hi_void pq_hdr_setvcvmomaprgb2yuvindc(vdp_regs_type *vdp_reg, hi_s32 *v_omap_rgb2yuv_in_dc)
{
    u_db_cvm1_rgb2yuv_in_dc0 db_cvm1_rgb2yuv_in_dc0;
    u_db_cvm1_rgb2yuv_in_dc1 db_cvm1_rgb2yuv_in_dc1;
    u_db_cvm1_rgb2yuv_in_dc2 db_cvm1_rgb2yuv_in_dc2;

    db_cvm1_rgb2yuv_in_dc0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_in_dc0.u32)));
    db_cvm1_rgb2yuv_in_dc0.bits.om_v3_rgb2yuv_dc_in_0 = v_omap_rgb2yuv_in_dc[0];
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_in_dc0.u32)), db_cvm1_rgb2yuv_in_dc0.u32);

    db_cvm1_rgb2yuv_in_dc1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_in_dc1.u32)));
    db_cvm1_rgb2yuv_in_dc1.bits.om_v3_rgb2yuv_dc_in_1 = v_omap_rgb2yuv_in_dc[1];
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_in_dc1.u32)), db_cvm1_rgb2yuv_in_dc1.u32);

    db_cvm1_rgb2yuv_in_dc2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_in_dc2.u32)));
    db_cvm1_rgb2yuv_in_dc2.bits.om_v3_rgb2yuv_dc_in_2 = v_omap_rgb2yuv_in_dc[2];  /* 2 is reg index */
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_in_dc2.u32)), db_cvm1_rgb2yuv_in_dc2.u32);

    return;
}

hi_void pq_hdr_setvcvmomapipt2lmsoutdc(vdp_regs_type *vdp_reg, hi_s32 *v_omap_ipt2lms_out_dc)
{
    u_db_cvm1_ipt2lms_out_dc00 db_cvm1_ipt2lms_out_dc00;
    u_db_cvm1_ipt2lms_out_dc01 db_cvm1_ipt2lms_out_dc01;
    u_db_cvm1_ipt2lms_out_dc02 db_cvm1_ipt2lms_out_dc02;

    db_cvm1_ipt2lms_out_dc00.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_out_dc00.u32)));
    db_cvm1_ipt2lms_out_dc00.bits.om_v3_ipt2lms_dc_out_0 = v_omap_ipt2lms_out_dc[0];
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_out_dc00.u32)), db_cvm1_ipt2lms_out_dc00.u32);

    db_cvm1_ipt2lms_out_dc01.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_out_dc01.u32)));
    db_cvm1_ipt2lms_out_dc01.bits.om_v3_ipt2lms_dc_out_1 = v_omap_ipt2lms_out_dc[1];
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_out_dc01.u32)), db_cvm1_ipt2lms_out_dc01.u32);

    db_cvm1_ipt2lms_out_dc02.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_out_dc02.u32)));
    db_cvm1_ipt2lms_out_dc02.bits.om_v3_ipt2lms_dc_out_2 = v_omap_ipt2lms_out_dc[2];  /* 2 is reg index */
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_out_dc02.u32)), db_cvm1_ipt2lms_out_dc02.u32);

    return;
}

hi_void pq_hdr_setvcvmomapipt2lmsindc(vdp_regs_type *vdp_reg, hi_s32 *v_omap_ipt2lms_in_dc)
{
    u_db_cvm1_ipt2lms_in_dc00 db_cvm1_ipt2lms_in_dc00;
    u_db_cvm1_ipt2lms_in_dc01 db_cvm1_ipt2lms_in_dc01;
    u_db_cvm1_ipt2lms_in_dc02 db_cvm1_ipt2lms_in_dc02;

    db_cvm1_ipt2lms_in_dc00.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_in_dc00.u32)));
    db_cvm1_ipt2lms_in_dc00.bits.om_v3_ipt2lms_dc_in_0 = v_omap_ipt2lms_in_dc[0];
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_in_dc00.u32)), db_cvm1_ipt2lms_in_dc00.u32);

    db_cvm1_ipt2lms_in_dc01.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_in_dc01.u32)));
    db_cvm1_ipt2lms_in_dc01.bits.om_v3_ipt2lms_dc_in_1 = v_omap_ipt2lms_in_dc[1];
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_in_dc01.u32)), db_cvm1_ipt2lms_in_dc01.u32);

    db_cvm1_ipt2lms_in_dc02.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_in_dc02.u32)));
    db_cvm1_ipt2lms_in_dc02.bits.om_v3_ipt2lms_dc_in_2 = v_omap_ipt2lms_in_dc[2];  /* 2 is reg index */
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_in_dc02.u32)), db_cvm1_ipt2lms_in_dc02.u32);

    return;
}

hi_void pq_hdr_setvcvmomaprangemin(vdp_regs_type *vdp_reg, hi_u32 v_omap_range_min)
{
    u_db_cvm1_out_rangemin db_cvm1_out_rangemin;

    db_cvm1_out_rangemin.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_out_rangemin.u32)));
    db_cvm1_out_rangemin.bits.v_omap_range_min = v_omap_range_min;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_out_rangemin.u32)), db_cvm1_out_rangemin.u32);

    return;
}
hi_void pq_hipp_dbhdr_setomen(vdp_regs_type *vdp_reg, hi_u32 om_en)
{
    u_db_cvm_ctrl db_cvm_ctrl;

    vdp_reg = pq_hal_get_vdp_set_reg();
    PQ_CHECK_NULL_PTR_RE_NULL(vdp_reg);

    db_cvm_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)));
    db_cvm_ctrl.bits.om_en = om_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)), db_cvm_ctrl.u32);

    return;
}

hi_void pq_hipp_dbhdr_setomu2sen(vdp_regs_type *vdp_reg, hi_u32 om_u2s_en)
{
    u_db_cvm_ctrl db_cvm_ctrl;

    db_cvm_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)));
    db_cvm_ctrl.bits.om_u2s_en = om_u2s_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)), db_cvm_ctrl.u32);

    return;
}
hi_void pq_hipp_dbhdr_setomlshiften(vdp_regs_type *vdp_reg, hi_u32 om_lshift_en)
{
    u_db_cvm_ctrl db_cvm_ctrl;

    db_cvm_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)));
    db_cvm_ctrl.bits.om_lshift_en = om_lshift_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)), db_cvm_ctrl.u32);

    return;
}
hi_void pq_hipp_dbhdr_setcvm1ipt2lmsen(vdp_regs_type *vdp_reg, hi_u32 cvm1_ipt2lms_en)
{
    u_db_cvm1_ipt2lms_ctrl db_cvm1_ipt2lms_ctrl;

    db_cvm1_ipt2lms_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_ctrl.u32)));
    db_cvm1_ipt2lms_ctrl.bits.cvm1_ipt2lms_en = cvm1_ipt2lms_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_ctrl.u32)), db_cvm1_ipt2lms_ctrl.u32);

    return;
}
hi_void pq_hipp_dbhdr_setomdegammaen(vdp_regs_type *vdp_reg, hi_u32 om_degamma_en)
{
    u_db_cvm_ctrl db_cvm_ctrl;

    db_cvm_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)));
    db_cvm_ctrl.bits.om_degamma_en = om_degamma_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)), db_cvm_ctrl.u32);

    return;
}
hi_void pq_hipp_dbhdr_setcvm1lms2rgben(vdp_regs_type *vdp_reg, hi_u32 cvm1_lms2rgb_en)
{
    u_db_cvm1_lms2rgb_ctrl db_cvm1_lms2rgb_ctrl;

    db_cvm1_lms2rgb_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_ctrl.u32)));
    db_cvm1_lms2rgb_ctrl.bits.cvm1_lms2rgb_en = cvm1_lms2rgb_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_ctrl.u32)), db_cvm1_lms2rgb_ctrl.u32);

    return;
}
hi_void pq_hipp_dbhdr_setomgammaen(vdp_regs_type *vdp_reg, hi_u32 om_gamma_en)
{
    u_db_cvm_ctrl db_cvm_ctrl;

    db_cvm_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)));
    db_cvm_ctrl.bits.om_gamma_en = om_gamma_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)), db_cvm_ctrl.u32);

    return;
}

hi_void pq_hipp_dbhdr_setvdenormen(vdp_regs_type *vdp_reg, hi_u32 v_denorm_en)
{
    u_db_cvm_ctrl db_cvm_ctrl;

    db_cvm_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)));
    db_cvm_ctrl.bits.v_denorm_en = v_denorm_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)), db_cvm_ctrl.u32);

    return;
}

hi_void pq_hipp_dbhdr_setomvcmen(vdp_regs_type *vdp_reg, hi_u32 om_vcvm_en)
{
    u_db_cvm_ctrl db_cvm_ctrl;

    db_cvm_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)));
    db_cvm_ctrl.bits.vcvm_en = om_vcvm_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)), db_cvm_ctrl.u32);

    return;
}

hi_void pq_hipp_dbhdr_setomcmen(vdp_regs_type *vdp_reg, hi_u32 om_cm_en)
{
    u_db_hipp_cm2_reg0 db_hipp_cm2_reg0;

    db_hipp_cm2_reg0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)));
    db_hipp_cm2_reg0.bits.om_cm_en = om_cm_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)), db_hipp_cm2_reg0.u32);

    return;
}

hi_void pq_hipp_dbhdr_setomcmckgten(vdp_regs_type *vdp_reg, hi_u32 om_cm_ck_gt_en)
{
    u_db_hipp_cm2_reg0 db_hipp_cm2_reg0;

    db_hipp_cm2_reg0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)));
    db_hipp_cm2_reg0.bits.om_cm_ck_gt_en = om_cm_ck_gt_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)), db_hipp_cm2_reg0.u32);

    return;
}

hi_void pq_hipp_dbhdr_setomcmbitdepthinmode(vdp_regs_type *vdp_reg, hi_u32 om_cm_bitdepth_in_mode)
{
    u_db_hipp_cm2_reg0 db_hipp_cm2_reg0;

    db_hipp_cm2_reg0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)));
    db_hipp_cm2_reg0.bits.om_cm_bitdepth_in_mode = om_cm_bitdepth_in_mode;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)), db_hipp_cm2_reg0.u32);

    return;
}

hi_void pq_hipp_dbhdr_setomcmbitdepthoutmode(vdp_regs_type *vdp_reg, hi_u32 om_cm_bitdepth_out_mode)
{
    u_db_hipp_cm2_reg0 db_hipp_cm2_reg0;

    db_hipp_cm2_reg0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)));
    db_hipp_cm2_reg0.bits.om_cm_bitdepth_out_mode = om_cm_bitdepth_out_mode;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)), db_hipp_cm2_reg0.u32);

    return;
}

hi_void pq_hipp_dbhdr_setomcmdemoen(vdp_regs_type *vdp_reg, hi_u32 om_cm_demo_en)
{
    u_db_hipp_cm2_reg0 db_hipp_cm2_reg0;

    db_hipp_cm2_reg0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)));
    db_hipp_cm2_reg0.bits.om_cm_demo_en = om_cm_demo_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)), db_hipp_cm2_reg0.u32);

    return;
}

hi_void pq_hipp_dbhdr_setomcmdemomode(vdp_regs_type *vdp_reg, hi_u32 om_cm_demo_mode)
{
    u_db_hipp_cm2_reg0 db_hipp_cm2_reg0;

    db_hipp_cm2_reg0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)));
    db_hipp_cm2_reg0.bits.om_cm_demo_mode = om_cm_demo_mode;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)), db_hipp_cm2_reg0.u32);

    return;
}

hi_void pq_hipp_dbhdr_setomcmdemopos(vdp_regs_type *vdp_reg, hi_u32 om_cm_demo_pos)
{
    u_db_hipp_cm2_reg0 db_hipp_cm2_reg0;

    db_hipp_cm2_reg0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)));
    db_hipp_cm2_reg0.bits.om_cm_demo_pos = om_cm_demo_pos;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)), db_hipp_cm2_reg0.u32);

    return;
}

hi_void pq_hipp_dbhdr_setcvm1rgb2yuven(vdp_regs_type *vdp_reg, hi_u32 cvm1_rgb2yuv_en)
{
    u_db_cvm1_rgb2yuv_ctrl db_cvm1_rgb2yuv_ctrl;

    db_cvm1_rgb2yuv_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_ctrl.u32)));
    db_cvm1_rgb2yuv_ctrl.bits.cvm1_rgb2yuv_en = cvm1_rgb2yuv_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_ctrl.u32)), db_cvm1_rgb2yuv_ctrl.u32);

    return;
}

hi_void pq_hdr_setvcvmoutoetf(vdp_regs_type *vdp_reg, hi_u32 cvm_out_oetf)
{
    u_db_cvm_ctrl db_cvm_ctrl;

    db_cvm_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)));
    db_cvm_ctrl.bits.cvm_out_oetf = cvm_out_oetf;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)), db_cvm_ctrl.u32);

    return;
}

/******************************TMAP V2******************************************/
hi_void pq_tmap_v2_settmrshiftrounden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tm_rshift_round_en)
{
    u_stb_tonemap_ctrl stb_tonemap_ctrl;

    stb_tonemap_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->stb_tonemap_ctrl.u32) + offset));
    stb_tonemap_ctrl.bits.tm_rshift_round_en = tm_rshift_round_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->stb_tonemap_ctrl.u32) + offset), stb_tonemap_ctrl.u32);

    return;
}

hi_void pq_tmap_v2_setstbtonemapen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 stb_tonemap_en)
{
    u_stb_tonemap_ctrl stb_tonemap_ctrl;

    stb_tonemap_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->stb_tonemap_ctrl.u32) + offset));
    stb_tonemap_ctrl.bits.stb_tonemap_en = stb_tonemap_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->stb_tonemap_ctrl.u32) + offset), stb_tonemap_ctrl.u32);

    return;
}

hi_void pq_tmap_v2_settmapsclutrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_sclut_rd_en)
{
    u_stb_tonemap_para_ren stb_tonemap_para_ren;

    stb_tonemap_para_ren.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->stb_tonemap_para_ren.u32) + offset));
    stb_tonemap_para_ren.bits.tmap_sclut_rd_en = tmap_sclut_rd_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->stb_tonemap_para_ren.u32) + offset), stb_tonemap_para_ren.u32);

    return;
}

hi_void pq_tmap_v2_settmapsslutrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_sslut_rd_en)
{
    u_stb_tonemap_para_ren stb_tonemap_para_ren;

    stb_tonemap_para_ren.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->stb_tonemap_para_ren.u32) + offset));
    stb_tonemap_para_ren.bits.tmap_sslut_rd_en = tmap_sslut_rd_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->stb_tonemap_para_ren.u32) + offset), stb_tonemap_para_ren.u32);

    return;
}

hi_void pq_tmap_v2_settmaptslutrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_tslut_rd_en)
{
    u_stb_tonemap_para_ren stb_tonemap_para_ren;

    stb_tonemap_para_ren.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->stb_tonemap_para_ren.u32) + offset));
    stb_tonemap_para_ren.bits.tmap_tslut_rd_en = tmap_tslut_rd_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->stb_tonemap_para_ren.u32) + offset), stb_tonemap_para_ren.u32);

    return;
}

hi_void pq_tmap_v2_settmapsilutrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_silut_rd_en)
{
    u_stb_tonemap_para_ren stb_tonemap_para_ren;

    stb_tonemap_para_ren.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->stb_tonemap_para_ren.u32) + offset));
    stb_tonemap_para_ren.bits.tmap_silut_rd_en = tmap_silut_rd_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->stb_tonemap_para_ren.u32) + offset), stb_tonemap_para_ren.u32);

    return;
}

hi_void pq_tmap_v2_settmaptilutrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_tilut_rd_en)
{
    u_stb_tonemap_para_ren stb_tonemap_para_ren;

    stb_tonemap_para_ren.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->stb_tonemap_para_ren.u32) + offset));
    stb_tonemap_para_ren.bits.tmap_tilut_rd_en = tmap_tilut_rd_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->stb_tonemap_para_ren.u32) + offset), stb_tonemap_para_ren.u32);

    return;
}

hi_void pq_tmap_v2_settmapparardata(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_para_rdata)
{
    u_stb_tonemap_para_rdata stb_tonemap_para_rdata;

    stb_tonemap_para_rdata.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->stb_tonemap_para_rdata.u32) + offset));
    stb_tonemap_para_rdata.bits.tmap_para_rdata = tmap_para_rdata;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->stb_tonemap_para_rdata.u32) + offset), stb_tonemap_para_rdata.u32);

    return;
}

hi_void pq_tmap_v2_settmc1expan(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tm_c1_expan)
{
    u_stb_tonemap_smc stb_tonemap_smc;

    stb_tonemap_smc.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->stb_tonemap_smc.u32) + offset));
    stb_tonemap_smc.bits.tm_c1_expan = tm_c1_expan;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->stb_tonemap_smc.u32) + offset), stb_tonemap_smc.u32);

    return;
}

hi_void pq_tmap_v2_settmsmcenable(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tm_smc_enable)
{
    u_stb_tonemap_smc stb_tonemap_smc;

    stb_tonemap_smc.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->stb_tonemap_smc.u32) + offset));
    stb_tonemap_smc.bits.tm_smc_enable = tm_smc_enable;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->stb_tonemap_smc.u32) + offset), stb_tonemap_smc.u32);

    return;
}

hi_void pq_tmap_v2_setimtmrshiftbit(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imtm_rshift_bit)
{
    u_stb_tonemap_odw_proc stb_tonemap_odw_proc;

    stb_tonemap_odw_proc.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->stb_tonemap_odw_proc.u32) + offset));
    stb_tonemap_odw_proc.bits.imtm_rshift_bit = imtm_rshift_bit;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->stb_tonemap_odw_proc.u32) + offset), stb_tonemap_odw_proc.u32);

    return;
}

hi_void pq_tmap_v2_setimtmrshiften(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imtm_rshift_en)
{
    u_stb_tonemap_odw_proc stb_tonemap_odw_proc;

    stb_tonemap_odw_proc.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->stb_tonemap_odw_proc.u32) + offset));
    stb_tonemap_odw_proc.bits.imtm_rshift_en = imtm_rshift_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->stb_tonemap_odw_proc.u32) + offset), stb_tonemap_odw_proc.u32);

    return;
}

hi_void pq_tmap_v2_setimtms2uen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imtm_s2u_en)
{
    u_stb_tonemap_odw_proc stb_tonemap_odw_proc;

    stb_tonemap_odw_proc.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->stb_tonemap_odw_proc.u32) + offset));
    stb_tonemap_odw_proc.bits.imtm_s2u_en = imtm_s2u_en;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->stb_tonemap_odw_proc.u32) + offset), stb_tonemap_odw_proc.u32);

    return;
}

/************************************************************************/
static hi_void pq_set_hdr_imap_yuv2rgb(vdp_regs_type *vdp_reg, hi_u32 offset, hi_drv_pq_yuv2rgb_cfg *yuv2rgb)
{
    /* yuv2rgb */
    pq_hihdr_v2_setimyuv2rgbmode(vdp_reg, offset, yuv2rgb->mode);
    pq_hihdr_v2_setimv1y2ren(vdp_reg, offset, yuv2rgb->v1_enable);
    pq_hihdr_v2_setimv0y2ren(vdp_reg, offset, yuv2rgb->v0_enable);
    pq_hihdr_v2_setimyuv2rgben(vdp_reg, offset, yuv2rgb->enable);
    pq_hihdr_v2_setimapm33yuv2rgb00(vdp_reg, offset, yuv2rgb->coef[0][0]);
    pq_hihdr_v2_setimapm33yuv2rgb01(vdp_reg, offset, yuv2rgb->coef[0][1]);
    pq_hihdr_v2_setimapm33yuv2rgb02(vdp_reg, offset, yuv2rgb->coef[0][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33yuv2rgb10(vdp_reg, offset, yuv2rgb->coef[1][0]);
    pq_hihdr_v2_setimapm33yuv2rgb11(vdp_reg, offset, yuv2rgb->coef[1][1]);
    pq_hihdr_v2_setimapm33yuv2rgb12(vdp_reg, offset, yuv2rgb->coef[1][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33yuv2rgb20(vdp_reg, offset, yuv2rgb->coef[2][0]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33yuv2rgb21(vdp_reg, offset, yuv2rgb->coef[2][1]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33yuv2rgb22(vdp_reg, offset, yuv2rgb->coef[2][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33yuv2rgb200(vdp_reg, offset, yuv2rgb->coef2[0][0]);
    pq_hihdr_v2_setimapm33yuv2rgb201(vdp_reg, offset, yuv2rgb->coef2[0][1]);
    pq_hihdr_v2_setimapm33yuv2rgb202(vdp_reg, offset, yuv2rgb->coef2[0][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33yuv2rgb210(vdp_reg, offset, yuv2rgb->coef2[1][0]);
    pq_hihdr_v2_setimapm33yuv2rgb211(vdp_reg, offset, yuv2rgb->coef2[1][1]);
    pq_hihdr_v2_setimapm33yuv2rgb212(vdp_reg, offset, yuv2rgb->coef2[1][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33yuv2rgb220(vdp_reg, offset, yuv2rgb->coef2[2][0]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33yuv2rgb221(vdp_reg, offset, yuv2rgb->coef2[2][1]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33yuv2rgb222(vdp_reg, offset, yuv2rgb->coef2[2][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33yuv2rgbscale2p(vdp_reg, offset, yuv2rgb->scale2p);
    pq_hihdr_v2_setimv3yuv2rgbdcin0(vdp_reg, offset, yuv2rgb->dc_in[0]);
    pq_hihdr_v2_setimv3yuv2rgbdcin1(vdp_reg, offset, yuv2rgb->dc_in[1]);
    pq_hihdr_v2_setimv3yuv2rgbdcin2(vdp_reg, offset, yuv2rgb->dc_in[2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapv3yuv2rgboffinrgb0(vdp_reg, offset, yuv2rgb->offinrgb[0]);
    pq_hihdr_v2_setimapv3yuv2rgboffinrgb1(vdp_reg, offset, yuv2rgb->offinrgb[1]);
    pq_hihdr_v2_setimapv3yuv2rgboffinrgb2(vdp_reg, offset, yuv2rgb->offinrgb[2]);  /* 2 is reg index */
    pq_hihdr_v2_setimv3yuv2rgb2dcin0(vdp_reg, offset, yuv2rgb->dc_in2[0]);
    pq_hihdr_v2_setimv3yuv2rgb2dcin1(vdp_reg, offset, yuv2rgb->dc_in2[1]);
    pq_hihdr_v2_setimv3yuv2rgb2dcin2(vdp_reg, offset, yuv2rgb->dc_in2[2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapv3yuv2rgb2offinrgb0(vdp_reg, offset, yuv2rgb->offinrgb2[0]);
    pq_hihdr_v2_setimapv3yuv2rgb2offinrgb1(vdp_reg, offset, yuv2rgb->offinrgb2[0]);
    pq_hihdr_v2_setimapv3yuv2rgb2offinrgb2(vdp_reg, offset, yuv2rgb->offinrgb2[0]);
    pq_hihdr_v2_setimyuv2rgbclipmin(vdp_reg, offset, yuv2rgb->clip_min);
    pq_hihdr_v2_setimyuv2rgbclipmax(vdp_reg, offset, yuv2rgb->clip_max);
    pq_hihdr_v2_setimyuv2rgbthrr(vdp_reg, offset, yuv2rgb->thr_r);
    pq_hihdr_v2_setimyuv2rgbthrb(vdp_reg, offset, yuv2rgb->thr_b);
}

static hi_void pq_set_hdr_imap_rgb2lms(vdp_regs_type *vdp_reg, hi_u32 offset, hi_drv_pq_rgb2lms_cfg *rgb2lms)
{
    /* rgb2lms */
    pq_hihdr_v2_setimrgb2lmsen(vdp_reg, offset, rgb2lms->enable);
    pq_hihdr_v2_setimapm33rgb2lms00(vdp_reg, offset, rgb2lms->coef[0][0]);
    pq_hihdr_v2_setimapm33rgb2lms01(vdp_reg, offset, rgb2lms->coef[0][1]);
    pq_hihdr_v2_setimapm33rgb2lms02(vdp_reg, offset, rgb2lms->coef[0][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33rgb2lms10(vdp_reg, offset, rgb2lms->coef[1][0]);
    pq_hihdr_v2_setimapm33rgb2lms11(vdp_reg, offset, rgb2lms->coef[1][1]);
    pq_hihdr_v2_setimapm33rgb2lms12(vdp_reg, offset, rgb2lms->coef[1][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33rgb2lms20(vdp_reg, offset, rgb2lms->coef[2][0]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33rgb2lms21(vdp_reg, offset, rgb2lms->coef[2][1]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33rgb2lms22(vdp_reg, offset, rgb2lms->coef[2][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33rgb2lmsscale2p(vdp_reg, offset, rgb2lms->scale2p);
    pq_hihdr_v2_setimrgb2lmsclipmin(vdp_reg, offset, rgb2lms->clip_min);
    pq_hihdr_v2_setimrgb2lmsclipmax(vdp_reg, offset, rgb2lms->clip_max);
    pq_hihdr_v2_setimv3rgb2lmsdcin0(vdp_reg, offset, rgb2lms->dc_in[0]);
    pq_hihdr_v2_setimv3rgb2lmsdcin1(vdp_reg, offset, rgb2lms->dc_in[1]);
    pq_hihdr_v2_setimv3rgb2lmsdcin2(vdp_reg, offset, rgb2lms->dc_in[2]);  /* 2 is reg index */
}

static hi_void pq_set_hdr_imap_lms2ipt(vdp_regs_type *vdp_reg, hi_u32 offset, hi_drv_pq_lms2ipt_cfg *lms2ipt)
{
    /* lms2ipt */
    pq_hihdr_v2_setimlms2ipten(vdp_reg, offset, lms2ipt->enable);
    pq_hihdr_v2_setimapm33lms2ipt00(vdp_reg, offset, lms2ipt->coef[0][0]);
    pq_hihdr_v2_setimapm33lms2ipt01(vdp_reg, offset, lms2ipt->coef[0][1]);
    pq_hihdr_v2_setimapm33lms2ipt02(vdp_reg, offset, lms2ipt->coef[0][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33lms2ipt10(vdp_reg, offset, lms2ipt->coef[1][0]);
    pq_hihdr_v2_setimapm33lms2ipt11(vdp_reg, offset, lms2ipt->coef[1][1]);
    pq_hihdr_v2_setimapm33lms2ipt12(vdp_reg, offset, lms2ipt->coef[1][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33lms2ipt20(vdp_reg, offset, lms2ipt->coef[2][0]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33lms2ipt21(vdp_reg, offset, lms2ipt->coef[2][1]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33lms2ipt22(vdp_reg, offset, lms2ipt->coef[2][2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapm33lms2iptscale2p(vdp_reg, offset, lms2ipt->scale2p);
    pq_hihdr_v2_setimlms2iptclipminy(vdp_reg, offset, lms2ipt->clip_min_y);
    pq_hihdr_v2_setimlms2iptclipmaxy(vdp_reg, offset, lms2ipt->clip_max_y);
    pq_hihdr_v2_setimlms2iptclipminc(vdp_reg, offset, lms2ipt->clip_min_c);
    pq_hihdr_v2_setimlms2iptclipmaxc(vdp_reg, offset, lms2ipt->clip_max_c);
    pq_hihdr_v2_setimv3lms2iptdcout0(vdp_reg, offset, lms2ipt->dc_out[0]);
    pq_hihdr_v2_setimv3lms2iptdcout1(vdp_reg, offset, lms2ipt->dc_out[1]);
    pq_hihdr_v2_setimv3lms2iptdcout2(vdp_reg, offset, lms2ipt->dc_out[2]);  /* 2 is reg index */
    pq_hihdr_v2_setimv3lms2iptdcin0(vdp_reg, offset, lms2ipt->dc_in[0]);
    pq_hihdr_v2_setimv3lms2iptdcin1(vdp_reg, offset, lms2ipt->dc_in[1]);
    pq_hihdr_v2_setimv3lms2iptdcin2(vdp_reg, offset, lms2ipt->dc_in[2]);  /* 2 is reg index */
}

/*****************************************************************************/
static hi_void pq_set_hdr_imap_tmap_v1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_drv_pq_tmap_v1_cfg *tmap_v1)
{
    /* tmap_v1 */
    pq_hihdr_v2_setimtmpos(vdp_reg, offset, tmap_v1->tm_pos);
    pq_hihdr_v2_setimtmv1en(vdp_reg, offset, tmap_v1->enable);
    pq_hihdr_v2_setimtmscalemixalpha(vdp_reg, offset, tmap_v1->scale_mix_alpha);
    pq_hihdr_v2_setimtmmixalpha(vdp_reg, offset, tmap_v1->mix_alpha);
    pq_hihdr_v2_setimtmxstep3(vdp_reg, offset, tmap_v1->step[3]);  /* 3 is reg index */
    pq_hihdr_v2_setimtmxstep2(vdp_reg, offset, tmap_v1->step[2]);  /* 2 is reg index */
    pq_hihdr_v2_setimtmxstep1(vdp_reg, offset, tmap_v1->step[1]);
    pq_hihdr_v2_setimtmxstep0(vdp_reg, offset, tmap_v1->step[0]);
    pq_hihdr_v2_setimtmxstep7(vdp_reg, offset, tmap_v1->step[7]);  /* 7 is reg index */
    pq_hihdr_v2_setimtmxstep6(vdp_reg, offset, tmap_v1->step[6]);  /* 6 is reg index */
    pq_hihdr_v2_setimtmxstep5(vdp_reg, offset, tmap_v1->step[5]);  /* 5 is reg index */
    pq_hihdr_v2_setimtmxstep4(vdp_reg, offset, tmap_v1->step[4]);  /* 4 is reg index */
    pq_hihdr_v2_setimtmxpos0(vdp_reg, offset, tmap_v1->pos[0]);
    pq_hihdr_v2_setimtmxpos1(vdp_reg, offset, tmap_v1->pos[1]);
    pq_hihdr_v2_setimtmxpos2(vdp_reg, offset, tmap_v1->pos[2]);  /* 2 is reg index */
    pq_hihdr_v2_setimtmxpos3(vdp_reg, offset, tmap_v1->pos[3]);  /* 3 is reg index */
    pq_hihdr_v2_setimtmxpos4(vdp_reg, offset, tmap_v1->pos[4]);  /* 4 is reg index */
    pq_hihdr_v2_setimtmxpos5(vdp_reg, offset, tmap_v1->pos[5]);  /* 5 is reg index */
    pq_hihdr_v2_setimtmxpos6(vdp_reg, offset, tmap_v1->pos[6]);  /* 6 is reg index */
    pq_hihdr_v2_setimtmxpos7(vdp_reg, offset, tmap_v1->pos[7]);  /* 7 is reg index */
    pq_hihdr_v2_setimtmxnum3(vdp_reg, offset, tmap_v1->num[3]);  /* 3 is reg index */
    pq_hihdr_v2_setimtmxnum2(vdp_reg, offset, tmap_v1->num[2]);  /* 2 is reg index */
    pq_hihdr_v2_setimtmxnum1(vdp_reg, offset, tmap_v1->num[1]);
    pq_hihdr_v2_setimtmxnum0(vdp_reg, offset, tmap_v1->num[0]);
    pq_hihdr_v2_setimtmxnum7(vdp_reg, offset, tmap_v1->num[7]);  /* 7 is reg index */
    pq_hihdr_v2_setimtmxnum6(vdp_reg, offset, tmap_v1->num[6]);  /* 6 is reg index */
    pq_hihdr_v2_setimtmxnum5(vdp_reg, offset, tmap_v1->num[5]);  /* 5 is reg index */
    pq_hihdr_v2_setimtmxnum4(vdp_reg, offset, tmap_v1->num[4]);  /* 4 is reg index */
    pq_hihdr_v2_setimtmm3lumcal0(vdp_reg, offset, tmap_v1->lum_cal[0]);
    pq_hihdr_v2_setimtmm3lumcal1(vdp_reg, offset, tmap_v1->lum_cal[1]);
    pq_hihdr_v2_setimtmm3lumcal2(vdp_reg, offset, tmap_v1->lum_cal[2]);  /* 2 is reg index */
    pq_hihdr_v2_setimtmscalelumcal(vdp_reg, offset, tmap_v1->scale_lum_cal);
    pq_hihdr_v2_setimtmscalecoef(vdp_reg, offset, tmap_v1->scale_coef);
    pq_hihdr_v2_setimtmclipmin(vdp_reg, offset, tmap_v1->clip_min);
    pq_hihdr_v2_setimtmclipmax(vdp_reg, offset, tmap_v1->clip_max);
    pq_hihdr_v2_setimtmdcout0(vdp_reg, offset, tmap_v1->dc_out[0]);
    pq_hihdr_v2_setimtmdcout1(vdp_reg, offset, tmap_v1->dc_out[1]);
    pq_hihdr_v2_setimtmdcout2(vdp_reg, offset, tmap_v1->dc_out[2]);  /* 2 is reg index */
}
static hi_void pq_set_hdr_imap_cacm(vdp_regs_type *vdp_reg, hi_u32 offset, hi_drv_pq_cacm_cfg *cacm)
{
    /* CACM */
    pq_hihdr_v2_setimcmpos(vdp_reg, offset, cacm->cm_pos);
    pq_hihdr_v2_setimcmdemopos(vdp_reg, offset, cacm->demo_pos);
    pq_hihdr_v2_setimcmdemomode(vdp_reg, offset, cacm->demo_mode);
    pq_hihdr_v2_setimcmdemoen(vdp_reg, offset, cacm->demo_en);
    pq_hihdr_v2_setimcmbitdepthoutmode(vdp_reg, offset, cacm->bitdepth_out_mode);
    pq_hihdr_v2_setimcmbitdepthinmode(vdp_reg, offset, cacm->bitdepth_in_mode);
    pq_hihdr_v2_setimcmckgten(vdp_reg, offset, HI_TRUE);
    pq_hihdr_v2_setimcmen(vdp_reg, offset, cacm->enable);
}

static hi_void pq_set_hdr_imap_mode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_drv_pq_imap_cfg *imap_cfg)
{
    pq_hihdr_v2_setimrshiftrounden(vdp_reg, offset, imap_cfg->rshift_round_en);
    pq_hihdr_v2_setimrshiften(vdp_reg, offset, imap_cfg->rshift_en);
    pq_hihdr_v2_setimckgten(vdp_reg, offset, HI_TRUE);
    pq_hihdr_v2_setimiptinsel(vdp_reg, offset, imap_cfg->ipt_in_sel);
    pq_hihdr_v2_setimladjen(vdp_reg, offset, imap_cfg->ladj_en);
    pq_hihdr_v2_setimapincolor(vdp_reg, offset, imap_cfg->in_color);
    pq_hihdr_v2_setiminbits(vdp_reg, offset, imap_cfg->in_bits);
    pq_hihdr_v2_setimgammaen(vdp_reg, offset, imap_cfg->gamma_en);
    pq_hihdr_v2_setimdegammaen(vdp_reg, offset, imap_cfg->degamma_en);
    pq_hihdr_v2_setimnormen(vdp_reg, offset, imap_cfg->norm_en);
    pq_hihdr_v2_setimen(vdp_reg, offset, imap_cfg->enable);
    pq_hihdr_v2_setimapeotfparameotf(vdp_reg, offset, imap_cfg->eotfparam_eotf);
    pq_hihdr_v2_setimapeotfparamrangemin(vdp_reg, offset, imap_cfg->eotfparam_range_min);
    pq_hihdr_v2_setimapeotfparamrange(vdp_reg, offset, imap_cfg->eotfparam_range);
    pq_hihdr_v2_setimapeotfparamrangeinv(vdp_reg, offset, imap_cfg->eotfparam_range_inv);
    pq_hihdr_v2_setimapv3iptoff0(vdp_reg, offset, imap_cfg->v3ipt_off[0]);
    pq_hihdr_v2_setimapv3iptoff1(vdp_reg, offset, imap_cfg->v3ipt_off[1]);
    pq_hihdr_v2_setimapv3iptoff2(vdp_reg, offset, imap_cfg->v3ipt_off[2]);  /* 2 is reg index */
    pq_hihdr_v2_setimapiptscale(vdp_reg, offset, imap_cfg->ipt_scale);
    pq_hihdr_v2_setimdegammaclipmax(vdp_reg, offset, imap_cfg->degamma_clip_max);
    pq_hihdr_v2_setimdegammaclipmin(vdp_reg, offset, imap_cfg->degamma_clip_min);
    pq_hihdr_v2_setimapladjchromaweight(vdp_reg, offset, imap_cfg->ladj_chroma_weight);
    pq_hihdr_v2_setimdemolumamode(vdp_reg, offset, imap_cfg->demo_luma_mode);
    pq_hihdr_v2_setimdemolumaen(vdp_reg, offset, imap_cfg->demo_luma_en);
    pq_hihdr_v2_setimv1hdren(vdp_reg, offset, imap_cfg->v1_hdr_enable);
    pq_hihdr_v2_setimv0hdren(vdp_reg, offset, imap_cfg->v0_hdr_enable);

    pq_set_hdr_imap_yuv2rgb(vdp_reg, offset, &imap_cfg->yuv2rgb);
    pq_set_hdr_imap_rgb2lms(vdp_reg, offset, &imap_cfg->rgb2lms);
    pq_set_hdr_imap_lms2ipt(vdp_reg, offset, &imap_cfg->lms2ipt);
    pq_set_hdr_imap_tmap_v1(vdp_reg, offset, &imap_cfg->tmap_v1);
    pq_set_hdr_imap_cacm(vdp_reg, offset, &imap_cfg->cacm);

    return;
}

static hi_void pq_set_hdr_tmap_mode(vdp_regs_type *vdp_reg, hi_u32 offset, pq_tmap_cfg *tmap_cfg)
{
    pq_tmap_v2_settmrshiftrounden(vdp_reg, offset, tmap_cfg->rshift_round_en);
    pq_tmap_v2_setstbtonemapen(vdp_reg, offset, tmap_cfg->enable);
    pq_tmap_v2_settmapsclutrden(vdp_reg, offset, tmap_cfg->sclut_rd_en);
    pq_tmap_v2_settmapsslutrden(vdp_reg, offset, tmap_cfg->sslut_rd_en);
    pq_tmap_v2_settmaptslutrden(vdp_reg, offset, tmap_cfg->tslut_rd_en);
    pq_tmap_v2_settmapsilutrden(vdp_reg, offset, tmap_cfg->silut_rd_en);
    pq_tmap_v2_settmaptilutrden(vdp_reg, offset, tmap_cfg->tilut_rd_en);
    pq_tmap_v2_settmapparardata(vdp_reg, offset, tmap_cfg->para_rdata);
    pq_tmap_v2_settmc1expan(vdp_reg, offset, tmap_cfg->c1_expan);
    pq_tmap_v2_settmsmcenable(vdp_reg, offset, tmap_cfg->smc_enable);

    pq_tmap_v2_setimtmrshiftbit(vdp_reg, offset, tmap_cfg->rshift_bit);
    pq_tmap_v2_setimtmrshiften(vdp_reg, offset, tmap_cfg->rshift_en);
    pq_tmap_v2_setimtms2uen(vdp_reg, offset, tmap_cfg->s2u_en);

    return;
}

static hi_void pq_set_hdr_omap_mode(vdp_regs_type *vdp_reg, hi_u32 offset, pq_omap_cfg *omap_cfg)
{
    pq_hipp_dbhdr_setomcmckgten(vdp_reg, HI_TRUE);

    /* cvm */
    pq_hipp_dbhdr_setomen(vdp_reg, omap_cfg->enable);
    pq_hipp_dbhdr_setomu2sen(vdp_reg, omap_cfg->u2s_enable);
    pq_hipp_dbhdr_setomlshiften(vdp_reg, omap_cfg->lshift_en);
    pq_hipp_dbhdr_setomdegammaen(vdp_reg, omap_cfg->degamma_en);
    pq_hipp_dbhdr_setomgammaen(vdp_reg, omap_cfg->gamma_en);
    pq_hipp_dbhdr_setvdenormen(vdp_reg, omap_cfg->denorm_en);
    pq_hipp_dbhdr_setomvcmen(vdp_reg, omap_cfg->cvm_en);

    pq_hdr_setvcvmipt2lmsen(vdp_reg, omap_cfg->ipt2lms.enable);
    pq_hdr_setvcvmlms2rgben(vdp_reg, omap_cfg->lms2rgb.enable);
    pq_hdr_setvcvmrgb2yuven(vdp_reg, omap_cfg->rgb2yuv.enable);

    pq_hdr_setvcvmomaprangemin(vdp_reg, omap_cfg->range_min);
    pq_hdr_setvcvmomaprangeover(vdp_reg, omap_cfg->range_over);
    pq_hdr_setcvmoutbits(vdp_reg, omap_cfg->out_bits);
    pq_hdr_setcvmoutcolor(vdp_reg, omap_cfg->out_color);
    pq_hdr_setvcvmomapiptoff(vdp_reg, omap_cfg->ipt_off);
    pq_hdr_setvcvmomapiptscale(vdp_reg, omap_cfg->ipt_scale);
    pq_hdr_setvdmomaplms2rgbmin(vdp_reg, omap_cfg->lms2rgb.min);
    pq_hdr_setvdmomaplms2rgbmax(vdp_reg, omap_cfg->lms2rgb.max);
    pq_hdr_setvdmomapipt2lmsmin(vdp_reg, omap_cfg->ipt2lms.min);
    pq_hdr_setvdmomapipt2lmsmax(vdp_reg, omap_cfg->ipt2lms.max);
    pq_hdr_setvcvmomapipt2lms(vdp_reg, omap_cfg->ipt2lms.coef);
    pq_hdr_setvcvmomapipt2lmsscale2p(vdp_reg, omap_cfg->ipt2lms.scale2p);
    pq_hdr_setvcvmomapipt2lmsoutdc(vdp_reg, omap_cfg->ipt2lms.out_dc);
    pq_hdr_setvcvmomapipt2lmsindc(vdp_reg, omap_cfg->ipt2lms.in_dc);
    pq_hdr_setvcvmomaplms2rgb(vdp_reg, omap_cfg->lms2rgb.coef);
    pq_hdr_setvcvmomaplms2rgbscale2p(vdp_reg, omap_cfg->lms2rgb.scale2p);
    pq_hdr_setvcvmomaprgb2yuv(vdp_reg, omap_cfg->rgb2yuv.coef);
    pq_hdr_setvcvmomaprgb2yuvscale2p(vdp_reg, omap_cfg->rgb2yuv.scale2p);
    pq_hdr_setvcvmomaprgb2yuvoutdc(vdp_reg, omap_cfg->rgb2yuv.out_dc);
    pq_hdr_setvcvmomaprgb2yuvindc(vdp_reg, omap_cfg->rgb2yuv.in_dc);
    pq_hdr_setvcvmomaprgb2yuvmin(vdp_reg, omap_cfg->rgb2yuv.min);
    pq_hdr_setvcvmomaprgb2yuvmax(vdp_reg, omap_cfg->rgb2yuv.max);
    pq_hdr_setvcvmoutoetf(vdp_reg, omap_cfg->oetf);

    /* cm */
    pq_hipp_dbhdr_setomcmen(vdp_reg, omap_cfg->cm_cfg.enable);
    pq_hipp_dbhdr_setomcmbitdepthinmode(vdp_reg, omap_cfg->cm_cfg.bitdepth_in_mode);
    pq_hipp_dbhdr_setomcmbitdepthoutmode(vdp_reg, omap_cfg->cm_cfg.bitdepth_out_mode);
    pq_hipp_dbhdr_setomcmdemoen(vdp_reg, omap_cfg->cm_cfg.demo_en);
    pq_hipp_dbhdr_setomcmdemomode(vdp_reg, omap_cfg->cm_cfg.demo_mode);
    pq_hipp_dbhdr_setomcmdemopos(vdp_reg, omap_cfg->cm_cfg.demo_pos);

    return;
}

/*****************************************************************************************/
#define PQ_V0_PARAUP_OFFSET 64
hi_void pq_para_setparaupv0chn(vdp_regs_type *vdp_reg, hi_u32 u32chnnum)
{
    u_para_up_v0 para_up_v0;

    para_up_v0.u32 = (1 << (u32chnnum - PQ_V0_PARAUP_OFFSET));
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_up_v0.u32)), para_up_v0.u32);

    return;
}

hi_void pq_para_set_para_haddr_v0_chn02(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn02 para_haddr_v0_chn02;

    para_haddr_v0_chn02.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn02.u32)));
    para_haddr_v0_chn02.bits.para_haddr_v0_chn02 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_haddr_v0_chn02.u32)), para_haddr_v0_chn02.u32);

    return;
}

hi_void pq_para_set_para_addr_v0_chn02(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn02 para_addr_v0_chn02;

    para_addr_v0_chn02.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn02.u32)));
    para_addr_v0_chn02.bits.para_addr_v0_chn02 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_v0_chn02.u32)), para_addr_v0_chn02.u32);

    return;
}

hi_void pq_para_set_para_haddr_v0_chn03(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn03 para_haddr_v0_chn03;

    para_haddr_v0_chn03.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn03.u32)));
    para_haddr_v0_chn03.bits.para_haddr_v0_chn03 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_haddr_v0_chn03.u32)), para_haddr_v0_chn03.u32);

    return;
}

hi_void pq_para_set_para_addr_v0_chn03(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn03 para_addr_v0_chn03;

    para_addr_v0_chn03.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn03.u32)));
    para_addr_v0_chn03.bits.para_addr_v0_chn03 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_v0_chn03.u32)), para_addr_v0_chn03.u32);

    return;
}

hi_void pq_para_set_para_haddr_v0_chn04(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn04 para_haddr_v0_chn04;

    para_haddr_v0_chn04.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn04.u32)));
    para_haddr_v0_chn04.bits.para_haddr_v0_chn04 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_haddr_v0_chn04.u32)), para_haddr_v0_chn04.u32);

    return;
}

hi_void pq_para_set_para_addr_v0_chn04(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn04 para_addr_v0_chn04;

    para_addr_v0_chn04.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn04.u32)));
    para_addr_v0_chn04.bits.para_addr_v0_chn04 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_v0_chn04.u32)), para_addr_v0_chn04.u32);

    return;
}

hi_void pq_para_set_para_haddr_v0_chn05(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn05 para_haddr_v0_chn05;

    para_haddr_v0_chn05.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn05.u32)));
    para_haddr_v0_chn05.bits.para_haddr_v0_chn05 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_haddr_v0_chn05.u32)), para_haddr_v0_chn05.u32);

    return;
}

hi_void pq_para_set_para_addr_v0_chn05(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn05 para_addr_v0_chn05;

    para_addr_v0_chn05.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn05.u32)));
    para_addr_v0_chn05.bits.para_addr_v0_chn05 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_v0_chn05.u32)), para_addr_v0_chn05.u32);

    return;
}

hi_void pq_para_set_para_haddr_v0_chn06(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn06 para_haddr_v0_chn06;

    para_haddr_v0_chn06.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn06.u32)));
    para_haddr_v0_chn06.bits.para_haddr_v0_chn06 = data;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn06.u32)), para_haddr_v0_chn06.u32);

    return;
}

hi_void pq_para_set_para_addr_v0_chn06(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn06 para_addr_v0_chn06;

    para_addr_v0_chn06.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn06.u32)));
    para_addr_v0_chn06.bits.para_addr_v0_chn06 = data;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn06.u32)), para_addr_v0_chn06.u32);

    return;
}

hi_void pq_para_set_para_haddr_v0_chn07(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn07 para_haddr_v0_chn07;

    para_haddr_v0_chn07.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn07.u32)));
    para_haddr_v0_chn07.bits.para_haddr_v0_chn07 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_haddr_v0_chn07.u32)), para_haddr_v0_chn07.u32);

    return;
}

hi_void pq_para_set_para_addr_v0_chn07(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn07 para_addr_v0_chn07;

    para_addr_v0_chn07.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn07.u32)));
    para_addr_v0_chn07.bits.para_addr_v0_chn07 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_v0_chn07.u32)), para_addr_v0_chn07.u32);

    return;
}

#define PQ_MAX_PARAUP_CHN 31
hi_void pq_para_setparaupvhdchn(vdp_regs_type *vdp_reg, hi_u32 u32chnnum)
{
    u_para_up_vhd para_up_vhd;
    if (u32chnnum <= PQ_MAX_PARAUP_CHN) {
        para_up_vhd.u32 = (1 << u32chnnum);
        pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_up_vhd.u32)), para_up_vhd.u32);
    }

    return;
}

hi_void pq_para_set_para_addr_vhd_chn07(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn07 para_addr_vhd_chn07;

    para_addr_vhd_chn07.bits.para_addr_vhd_chn07 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_vhd_chn07.u32)), para_addr_vhd_chn07.u32);

    return;
}

hi_void pq_para_set_para_haddr_vhd_chn07(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn07 para_haddr_vhd_chn07;

    para_haddr_vhd_chn07.bits.para_haddr_vhd_chn07 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn07.u32)), para_haddr_vhd_chn07.u32);

    return;
}

hi_void pq_para_set_para_addr_vhd_chn08(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn08 para_addr_vhd_chn08;

    para_addr_vhd_chn08.bits.para_addr_vhd_chn08 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_vhd_chn08.u32)), para_addr_vhd_chn08.u32);

    return;
}

hi_void pq_para_set_para_haddr_vhd_chn08(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn08 para_haddr_vhd_chn08;

    para_haddr_vhd_chn08.bits.para_haddr_vhd_chn08 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn08.u32)), para_haddr_vhd_chn08.u32);

    return;
}

hi_void pq_para_set_para_addr_vhd_chn19(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn19 para_addr_vhd_chn19;

    para_addr_vhd_chn19.bits.para_addr_vhd_chn19 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_addr_vhd_chn19.u32)), para_addr_vhd_chn19.u32);

    return;
}

hi_void pq_para_set_para_haddr_vhd_chn19(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn19 para_haddr_vhd_chn19;

    para_haddr_vhd_chn19.bits.para_haddr_vhd_chn19 = data;
    pq_new_regwrite ((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn19.u32)), para_haddr_vhd_chn19.u32);

    return;
}

static hi_void pq_hal_set_hdr_coef_addr(vdp_regs_type *vdp_reg, pq_hal_hdr_cfg *hdr_info)
{
    if (hdr_info->omap_cfg.degamma_en == HI_TRUE) {
        pq_para_set_para_addr_vhd_chn07(vdp_reg, hdr_info->coef_addr.omap_degamm_addr);
        pq_para_set_para_haddr_vhd_chn07(vdp_reg, hdr_info->coef_addr.omap_degamm_addr >> 32); /* 32 is number */
    }

    if (hdr_info->omap_cfg.gamma_en == HI_TRUE) {
        pq_para_set_para_addr_vhd_chn08(vdp_reg, hdr_info->coef_addr.omap_gamm_addr);
        pq_para_set_para_haddr_vhd_chn08(vdp_reg, hdr_info->coef_addr.omap_gamm_addr >> 32); /* 32 is number */
    }

    if (hdr_info->omap_cfg.cm_cfg.enable == HI_TRUE) {
        pq_para_set_para_addr_vhd_chn19(vdp_reg, hdr_info->coef_addr.omap_cm_addr);
        pq_para_set_para_haddr_vhd_chn19(vdp_reg, hdr_info->coef_addr.omap_cm_addr >> 32); /* 32 is number */
    }

    if (hdr_info->cm_cfg.enable == HI_TRUE) {
        pq_para_set_para_addr_v0_chn02(vdp_reg, hdr_info->coef_addr.hdrv1_cm);
        pq_para_set_para_haddr_v0_chn02(vdp_reg, hdr_info->coef_addr.hdrv1_cm >> 32); /* 32 is number */
    }

    if (hdr_info->imap_cfg.degamma_en == HI_TRUE) {
        pq_para_set_para_addr_v0_chn03(vdp_reg, hdr_info->coef_addr.imap_degamma_addr);
        pq_para_set_para_haddr_v0_chn03(vdp_reg, hdr_info->coef_addr.imap_degamma_addr >> 32); /* 32 is number */
    }

    if (hdr_info->imap_cfg.tmap_v1.enable == HI_TRUE) {
        pq_para_set_para_addr_v0_chn05(vdp_reg, hdr_info->coef_addr.imap_tmapv1_addr);
        pq_para_set_para_haddr_v0_chn05(vdp_reg, hdr_info->coef_addr.imap_tmapv1_addr >> 32); /* 32 is number */
    }

    if (hdr_info->imap_cfg.gamma_en == HI_TRUE) {
        pq_para_set_para_addr_v0_chn04(vdp_reg, hdr_info->coef_addr.imap_gamma_addr);
        pq_para_set_para_haddr_v0_chn04(vdp_reg, hdr_info->coef_addr.imap_gamma_addr >> 32); /* 32 is number */
    }

    if (hdr_info->imap_cfg.cacm.enable == HI_TRUE) {
        pq_para_set_para_addr_v0_chn06(vdp_reg, hdr_info->coef_addr.imap_cacm_addr);
        pq_para_set_para_haddr_v0_chn06(vdp_reg, hdr_info->coef_addr.imap_cacm_addr >> 32); /* 32 is number */
    }

    if (hdr_info->tmap_cfg.enable == HI_TRUE) {
        pq_para_set_para_addr_v0_chn07(vdp_reg, hdr_info->coef_addr.tmapv2_addr);
        pq_para_set_para_haddr_v0_chn07(vdp_reg, hdr_info->coef_addr.tmapv2_addr >> 32); /* 32 is number */
    }
}

#define GFX_OFFSET (0x800 / 4)
hi_void vdp_hdr_setregup(vdp_regs_type *vdp_reg)
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

hi_void pq_hal_set_hdr_cfg(hi_drv_pq_xdr_layer_id layer, pq_hal_hdr_cfg *hdr_info)
{
    hi_u32 offset;
    vdp_regs_type *vdp_reg = HI_NULL;

    if (layer == HI_DRV_PQ_XDR_LAYER_ID_0) {
        offset = VID0_HDR_OFFSET;
    } else {
        HI_ERR_PQ("No support layer: %d\n", layer);
        return;
    }

    vdp_reg = pq_hal_get_vdp_set_reg();
    PQ_CHECK_NULL_PTR_RE_NULL(vdp_reg);

    pq_hal_set_hdr_coef_addr(vdp_reg, hdr_info);

    pq_set_hdr_cm_mode(vdp_reg, offset, &hdr_info->cm_cfg);

    pq_set_hdr_imap_mode(vdp_reg, offset, &hdr_info->imap_cfg);

    pq_set_hdr_omap_mode(vdp_reg, offset, &hdr_info->omap_cfg);

    pq_set_hdr_tmap_mode(vdp_reg, offset, &hdr_info->tmap_cfg);

    pq_para_setparaupvhdchn(vdp_reg, PQ_COEF_BUF_OM_DEGMM);
    pq_para_setparaupvhdchn(vdp_reg, PQ_COEF_BUF_OM_GMM);
    pq_para_setparaupvhdchn(vdp_reg, PQ_COEF_BUF_OM_CM);

    pq_para_setparaupv0chn(vdp_reg, PQ_COEF_BUF_V0_HDR1);
    pq_para_setparaupv0chn(vdp_reg, PQ_COEF_BUF_V0_HDR2);
    pq_para_setparaupv0chn(vdp_reg, PQ_COEF_BUF_V0_HDR3);
    pq_para_setparaupv0chn(vdp_reg, PQ_COEF_BUF_V0_HDR4);
    pq_para_setparaupv0chn(vdp_reg, PQ_COEF_BUF_V0_HDR5);
    pq_para_setparaupv0chn(vdp_reg, PQ_COEF_BUF_V0_HDR6);

    return;
}

