/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_REG_NXG_H__
#define __HAL_VDP_REG_NXG_H__

#include "hi_reg_vdp.h"

hi_void vdp_nxg_setnxgen            (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 nxg_en);
hi_void vdp_nxg_setnxgsubmarken     (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 submark_en);
hi_void vdp_nxg_setnxgdebugen       (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 debug_en);
hi_void vdp_nxg_setnxgcken          (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 ck_en);
hi_void vdp_nxg_setnxgservicein     (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 service_en);
hi_void vdp_nxg_setnxgyuvnrgbin     (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 yuvnrgbin);
hi_void vdp_nxg_setnxgframerate     (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 frame_rate);
hi_void vdp_nxg_setnxgframetype     (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 frame_type);
hi_void vdp_nxg_setnxgkeyin         (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 keyin);
hi_void vdp_nxg_setnxgpayload24or56 (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 payload24or56);
hi_void vdp_nxg_setnxgoperatorid    (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 operatorid);
hi_void vdp_nxg_setnxgtimestamp     (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 timestamp);
hi_void vdp_nxg_setnxgsubscriberid  (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 subscriberid);

hi_void vdp_nxg_setnxgsetting0      (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 setting_0);
hi_void vdp_nxg_setnxgsetting1      (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 setting_1);
hi_void vdp_nxg_setnxgsetting2      (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 setting_2);
hi_void vdp_nxg_setnxgsetting3      (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 setting_3);
hi_void vdp_nxg_setnxgsetting4      (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 setting_4);
hi_void vdp_nxg_setnxgsetting5      (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 setting_5);

hi_void vdp_nxg_setnxgvideoyfpos    (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 video_yfpos);
hi_void vdp_nxg_setnxgvideoxfpos    (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 video_xfpos);
hi_void vdp_nxg_setnxgvideoylpos    (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 video_ylpos);
hi_void vdp_nxg_setnxgvideoxlpos    (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 video_xlpos);

hi_void vdp_nxg_setnxghblank        (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 hblank);
hi_void vdp_nxg_setnxgvblank        (vdp_regs_type *g_vdp_reg, hi_u32 offset, hi_u32 vblank);

#endif//__HAL_VDP_REG_NXG_H__
