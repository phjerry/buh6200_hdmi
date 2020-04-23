/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_comm.h"
#include "hal_vdp_ip_para.h"
#include "hal_vdp_ip_nxg.h"

#include "hal_vdp_reg_nxg.h"

hi_s32 vdp_nxg_set_mode(hi_u32 layer, vdp_nxg_info *nxg_cfg)
{
    hi_u32 offset = 0;
    vdp_nxg_info tmp_cfg;
    vdp_nxg_info *cfg = &tmp_cfg;

    memset(cfg, 0, sizeof(vdp_nxg_info));

    cfg->enable = HI_TRUE;
    cfg->debug_en = HI_TRUE;
    cfg->fmt = 0;
    cfg->frame_type = HI_TEE_VWM_NEXGUARD_SETTING_HDR10;

    vdp_nxg_setnxgen(g_vdp_reg, offset, cfg->enable);
    vdp_nxg_setnxgdebugen(g_vdp_reg, offset, cfg->debug_en);
    vdp_nxg_setnxgframerate(g_vdp_reg, offset, cfg->frame_rate);
    vdp_nxg_setnxgkeyin(g_vdp_reg, offset, cfg->keyin);
    vdp_nxg_setnxgyuvnrgbin(g_vdp_reg, offset, cfg->fmt);
    vdp_nxg_setnxgframetype(g_vdp_reg, offset, cfg->frame_type);
    vdp_nxg_setnxgpayload24or56(g_vdp_reg, offset, cfg->payload);
    vdp_nxg_setnxgsetting0(g_vdp_reg, offset, cfg->setting[0]);
    vdp_nxg_setnxgsetting1(g_vdp_reg, offset, cfg->setting[1]);
    vdp_nxg_setnxgsetting2(g_vdp_reg, offset, cfg->setting[2]); /* 2 is number */
    vdp_nxg_setnxgsetting3(g_vdp_reg, offset, cfg->setting[3]); /* 3 is number */
    vdp_nxg_setnxgsetting4(g_vdp_reg, offset, cfg->setting[4]); /* 4 is number */
    vdp_nxg_setnxgsetting5(g_vdp_reg, offset, cfg->setting[5]); /* 5 is number */
    vdp_nxg_setnxgvideoyfpos(g_vdp_reg, offset, cfg->video_yfpos);
    vdp_nxg_setnxgvideoxfpos(g_vdp_reg, offset, cfg->video_xfpos);
    vdp_nxg_setnxgvideoylpos(g_vdp_reg, offset, cfg->video_ylpos);
    vdp_nxg_setnxgvideoxlpos(g_vdp_reg, offset, cfg->video_xlpos);

    return HI_SUCCESS;
}

