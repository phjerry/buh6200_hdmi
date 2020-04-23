/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_ip_vid.h"
#include "hal_vdp_comm.h"
#include "hal_vdp_reg_vid.h"
#include "hal_vdp_reg_disp.h"

hi_void vdp_ip_vid_set_cfg(hi_u32 layer, vdp_vid_cfg *vid_cfg)
{
    vdp_vid_setnosecflag(g_vdp_reg, layer, !vid_cfg->secure_en);
    vdp_vid_setlayerenable(g_vdp_reg, layer, vid_cfg->enable);
    vdp_vp_updatelayerenable(g_vdp_reg, VDP_LAYER_VP0); /* update vp enabe */
    if (vid_cfg->enable == HI_FALSE) {
        return;
    }

    vdp_vid_setvideopos(g_vdp_reg, layer, vid_cfg->video_rect);
    vdp_vid_setdisppos(g_vdp_reg, layer, vid_cfg->disp_rect);

    vdp_vid_setlayerbkg(g_vdp_reg, layer, vid_cfg->bkg);
    vdp_vid_setlayergalpha(g_vdp_reg, layer, vid_cfg->alpha);

    return;
}

hi_void vdp_ip_vp_set_cfg(hi_u32 layer, vdp_vp_cfg *vp_cfg)
{
    vdp_vp_setinreso(g_vdp_reg, layer, vp_cfg->vp0_rect);
    vdp_vp_setvideopos(g_vdp_reg, layer, vp_cfg->vp0_rect);
    vdp_vp_setdisppos(g_vdp_reg, layer, vp_cfg->vp0_rect);

    /* vp set the mix v0 to bgcolor, because vp video always equal disp */
    vdp_dispchn_set_cbm_bkg(VDP_DISPCHN_CBM_MIXV0, &vp_cfg->bkg);
    vdp_vp_setlayergalpha(g_vdp_reg, layer, vp_cfg->alpha);

    return;
}

hi_void vdp_ip_vid_regup(hi_void)
{
    vdp_vid_setupdmode(g_vdp_reg, VDP_LAYER_VID0, HI_TRUE);
    vdp_vid_setupdmode(g_vdp_reg, VDP_LAYER_VID1, HI_TRUE);
    vdp_vid_setupdmode(g_vdp_reg, VDP_LAYER_VID3, HI_TRUE);

    vdp_vid_setregup(g_vdp_reg, VDP_LAYER_VID0);
    vdp_vid_setregup(g_vdp_reg, VDP_LAYER_VID1);
    vdp_vid_setregup(g_vdp_reg, VDP_LAYER_VID3);
    vdp_vp_setregup(g_vdp_reg, VDP_LAYER_VP0);
}

hi_bool vdp_ip_vid_get_mark_status(hi_u32 layer)
{
    return vdp_vid_get_wager_mark_status(g_vdp_reg, layer);
}
