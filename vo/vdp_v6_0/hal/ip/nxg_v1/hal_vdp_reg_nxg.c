/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_reg_nxg.h"
#include "hal_vdp_comm.h"

hi_void vdp_nxg_setnxgen(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 nxg_en)
{
    u_v1_nxg_ctrl nxg_ctrl;

    nxg_ctrl.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_ctrl.u32) + offset));
    nxg_ctrl.bits.nxg_en = nxg_en;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_ctrl.u32) + offset), nxg_ctrl.u32);

    return ;
}

hi_void vdp_nxg_setnxgsubmarken(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 submark_en)
{
    u_v1_nxg_ctrl nxg_ctrl;

    nxg_ctrl.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_ctrl.u32) + offset));
    nxg_ctrl.bits.nxg_submark_en = submark_en;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_ctrl.u32) + offset), nxg_ctrl.u32);

    return ;
}

hi_void vdp_nxg_setnxgdebugen(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 debug_en)
{
    u_v1_nxg_ctrl nxg_ctrl;

    nxg_ctrl.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_ctrl.u32) + offset));
    nxg_ctrl.bits.ng_debugenablein = debug_en;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_ctrl.u32) + offset), nxg_ctrl.u32);

    return ;
}

hi_void vdp_nxg_setnxgcken(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 ck_en)
{
    u_v1_nxg_ctrl nxg_ctrl;

    nxg_ctrl.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_ctrl.u32) + offset));
    nxg_ctrl.bits.nxg_core_ck_en = ck_en;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_ctrl.u32) + offset), nxg_ctrl.u32);

    return ;
}

hi_void vdp_nxg_setnxgservicein(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 service_en)
{
    u_v1_nxg_ctrl nxg_ctrl;

    nxg_ctrl.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_ctrl.u32) + offset));
    nxg_ctrl.bits.ng_enableservicein = service_en;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_ctrl.u32) + offset), nxg_ctrl.u32);

    return ;
}

hi_void vdp_nxg_setnxgyuvnrgbin(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 yuvnrgbin)
{
    u_v1_nxg_ctrl nxg_ctrl;

    nxg_ctrl.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_ctrl.u32) + offset));
    nxg_ctrl.bits.ng_yuvnrgbin = yuvnrgbin;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_ctrl.u32) + offset), nxg_ctrl.u32);

    return ;
}

hi_void vdp_nxg_setnxgframerate(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 frame_rate)
{
    u_v1_nxg_ctrl nxg_ctrl;

    nxg_ctrl.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_ctrl.u32) + offset));
    nxg_ctrl.bits.ng_frameratein = frame_rate;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_ctrl.u32) + offset), nxg_ctrl.u32);

    return ;
}

hi_void vdp_nxg_setnxgframetype(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 frame_type)
{
    u_v1_nxg_ctrl nxg_ctrl;

    nxg_ctrl.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_ctrl.u32) + offset));
    nxg_ctrl.bits.ng_frametypein = frame_type;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_ctrl.u32) + offset), nxg_ctrl.u32);

    return ;
}

hi_void vdp_nxg_setnxgkeyin(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 keyin)
{
    u_v1_nxg_keyin nxg_keyin;

    nxg_keyin.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_keyin.u32) + offset));
    nxg_keyin.bits.ng_keyin = keyin;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_keyin.u32) + offset), nxg_keyin.u32);

    return ;
}

hi_void vdp_nxg_setnxgpayload24or56(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 payload24or56)
{
    u_v1_nxg_payload0 nxg_payload0;

    nxg_payload0.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_payload0.u32) + offset));
    nxg_payload0.bits.ng_paylaod24_56n = payload24or56;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_payload0.u32) + offset), nxg_payload0.u32);

    return ;
}

hi_void vdp_nxg_setnxgoperatorid(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 operatorid)
{
    u_v1_nxg_payload0 nxg_payload0;

    nxg_payload0.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_payload0.u32) + offset));
    nxg_payload0.bits.operator_id = operatorid;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_payload0.u32) + offset), nxg_payload0.u32);

    return ;
}

hi_void vdp_nxg_setnxgtimestamp(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 timestamp)
{
    u_v1_nxg_payload0 nxg_payload0;

    nxg_payload0.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_payload0.u32) + offset));
    nxg_payload0.bits.timestamp = timestamp;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_payload0.u32) + offset), nxg_payload0.u32);

    return ;
}

hi_void vdp_nxg_setnxgsubscriberid(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 subscriberid)
{
    u_v1_nxg_payload1 nxg_payload1;

    nxg_payload1.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_payload1.u32) + offset));
    nxg_payload1.bits.subscriber_id = subscriberid;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_payload1.u32) + offset), nxg_payload1.u32);

    return ;
}

hi_void vdp_nxg_setnxgsetting0(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 setting_0)
{
    u_v1_nxg_setting0 nxg_setting0;

    nxg_setting0.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_setting0.u32) + offset));
    nxg_setting0.bits.setting0 = setting_0;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_setting0.u32) + offset), nxg_setting0.u32);

    return ;
}

hi_void vdp_nxg_setnxgsetting1(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 setting_1)
{
    u_v1_nxg_setting1 nxg_setting1;

    nxg_setting1.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_setting1.u32) + offset));
    nxg_setting1.bits.setting1 = setting_1;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_setting1.u32) + offset), nxg_setting1.u32);

    return ;
}

hi_void vdp_nxg_setnxgsetting2(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 setting_2)
{
    u_v1_nxg_setting2 nxg_setting2;

    nxg_setting2.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_setting2.u32) + offset));
    nxg_setting2.bits.setting2 = setting_2;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_setting2.u32) + offset), nxg_setting2.u32);

    return ;
}

hi_void vdp_nxg_setnxgsetting3(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 setting_3)
{
    u_v1_nxg_setting3 nxg_setting3;

    nxg_setting3.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_setting3.u32) + offset));
    nxg_setting3.bits.setting3 = setting_3;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_setting3.u32) + offset), nxg_setting3.u32);

    return ;
}

hi_void vdp_nxg_setnxgsetting4(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 setting_4)
{
    u_v1_nxg_setting4 nxg_setting4;

    nxg_setting4.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_setting4.u32) + offset));
    nxg_setting4.bits.setting4 = setting_4;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_setting4.u32) + offset), nxg_setting4.u32);

    return ;
}

hi_void vdp_nxg_setnxgsetting5(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 setting_5)
{
    u_v1_nxg_setting5 nxg_setting5;

    nxg_setting5.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_setting5.u32) + offset));
    nxg_setting5.bits.setting5 = setting_5;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_setting5.u32) + offset), nxg_setting5.u32);

    return ;
}

hi_void vdp_nxg_setnxgvideoyfpos(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 video_yfpos)
{
    u_v1_nxg_fpos nxg_fpos;

    nxg_fpos.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_fpos.u32) + offset));
    nxg_fpos.bits.nxg_yfpos = video_yfpos;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_fpos.u32) + offset), nxg_fpos.u32);

    return ;
}

hi_void vdp_nxg_setnxgvideoxfpos(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 video_xfpos)
{
    u_v1_nxg_fpos nxg_fpos;

    nxg_fpos.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_fpos.u32) + offset));
    nxg_fpos.bits.nxg_xfpos = video_xfpos;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_fpos.u32) + offset), nxg_fpos.u32);

    return ;
}

hi_void vdp_nxg_setnxgvideoylpos(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 video_ylpos)
{
    u_v1_nxg_lpos nxg_lpos;

    nxg_lpos.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_lpos.u32) + offset));
    nxg_lpos.bits.nxg_ylpos = video_ylpos - 1;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_lpos.u32) + offset), nxg_lpos.u32);

    return ;
}

hi_void vdp_nxg_setnxgvideoxlpos(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 video_xlpos)
{
    u_v1_nxg_lpos nxg_lpos;

    nxg_lpos.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_lpos.u32) + offset));
    nxg_lpos.bits.nxg_xlpos = video_xlpos - 1;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_lpos.u32) + offset), nxg_lpos.u32);

    return ;
}

hi_void vdp_nxg_setnxghblank(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 hblank)
{
    u_v1_nxg_blank nxg_blank;

    nxg_blank.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_blank.u32) + offset));
    nxg_blank.bits.nxg_hblank = hblank - 1;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_blank.u32) + offset), nxg_blank.u32);

    return ;
}


hi_void vdp_nxg_setnxgvblank(vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 vblank)
{
    u_v1_nxg_blank nxg_blank;

    nxg_blank.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_nxg_blank.u32) + offset));
    nxg_blank.bits.nxg_vblank = vblank - 1;
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->v1_nxg_blank.u32) + offset), nxg_blank.u32);

    return ;
}

