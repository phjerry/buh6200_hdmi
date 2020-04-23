/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_reg_vid.h"
#include "hal_vdp_comm.h"

const hi_u32 g_vdp_vid_reg[] = {
    (0x00000 / 4),  // vid1 - vid1  0x00000 4 reg offset
    (0x01000 / 4),  // vid2 - vid1  0x01000 4 reg offset
    (0x02000 / 4),  // vid3 - vid1  0x02000 4 reg offset
    (0x2F000 / 4)   // vid0 - vid1  0x2F000 4 reg offset
};

hi_void vdp_vid_setvmxppcmode(vdp_regs_type *vdp_reg, hi_u32 data, xdp_vmx_ppc_mode envmxppcmode)
{
    u_v0_ctrl v0_ctrl;

    v0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v0_ctrl.u32)));
    v0_ctrl.bits.vmx_ppc_mode = envmxppcmode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v0_ctrl.u32)), v0_ctrl.u32);

    return;
}

hi_void vdp_vid_set8kcleparasrc4kenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32enable)
{
    u_v0_ctrl v0_ctrl;

    v0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v0_ctrl.u32)));
    v0_ctrl.bits.sr_8k_cle_para_src = u32enable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v0_ctrl.u32)), v0_ctrl.u32);

    return;
}

hi_void vdp_vid_set8kclmparasrc4kenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32enable)
{
    u_v0_ctrl v0_ctrl;

    v0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v0_ctrl.u32)));
    v0_ctrl.bits.sr_8k_clm_para_src = u32enable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v0_ctrl.u32)), v0_ctrl.u32);

    return;
}

hi_void vdp_vid_set8ksrpwrupingenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32enable)
{
    u_v0_ctrl v0_ctrl;

    v0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v0_ctrl.u32)));
    v0_ctrl.bits.sr_8k_pwrup_ing = u32enable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v0_ctrl.u32)), v0_ctrl.u32);

    return;
}

hi_void vdp_vid_set8ksrpwrdownenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32enable)
{
    u_v0_ctrl v0_ctrl;

    v0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v0_ctrl.u32)));
    v0_ctrl.bits.sr_8k_pwrdn_bypass = u32enable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v0_ctrl.u32)), v0_ctrl.u32);

    return;
}

hi_void vdp_vid_set4ksrpwrupingenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32enable)
{
    u_v0_ctrl v0_ctrl;

    v0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v0_ctrl.u32)));
    v0_ctrl.bits.sr_4k_pwrup_ing = u32enable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v0_ctrl.u32)), v0_ctrl.u32);

    return;
}

hi_void vdp_vid_set4ksrpwrdownenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32enable)
{
    u_v0_ctrl v0_ctrl;

    v0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v0_ctrl.u32)));
    v0_ctrl.bits.sr_4k_pwrdn_bypass = u32enable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v0_ctrl.u32)), v0_ctrl.u32);

    return;
}

hi_void vdp_vid_sethdrpwrdownenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32enable)
{
    u_v0_ctrl v0_ctrl;

    v0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v0_ctrl.u32)));
    v0_ctrl.bits.hdr_pwrdn_bypass = u32enable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v0_ctrl.u32)), v0_ctrl.u32);

    return;
}

hi_void vdp_vid_set8ksrtwochnwth(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32leftwth, hi_u32 u32rightwth)
{
    u_v0_sr_8k60_split v0_sr_8k60_split;

    v0_sr_8k60_split.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v0_sr_8k60_split.u32)));
    v0_sr_8k60_split.bits.sr_8k60_split_wth_lft = u32leftwth - 1;
    v0_sr_8k60_split.bits.sr_8k60_split_wth_rgt = u32rightwth - 1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v0_sr_8k60_split.u32)), v0_sr_8k60_split.u32);

    return;
}

// v0 layer link drv
hi_void vdp_vid_set8ksrmuxdisable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32benable)
{
    u_v0_ctrl v0_ctrl;

    v0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v0_ctrl.u32)));
    v0_ctrl.bits.sr_8k_mux = u32benable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v0_ctrl.u32)), v0_ctrl.u32);

    return;
}

hi_void vdp_vid_set4ksrmuxdisable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32benable)
{
    u_v0_ctrl v0_ctrl;

    v0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v0_ctrl.u32)));
    v0_ctrl.bits.sr_4k_mux = u32benable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v0_ctrl.u32)), v0_ctrl.u32);

    return;
}

hi_void vdp_vid_set4kzmemuxdisable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32benable)
{
    u_v0_ctrl v0_ctrl;

    v0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v0_ctrl.u32)));
    v0_ctrl.bits.zme_4kor8k_mux = u32benable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v0_ctrl.u32)), v0_ctrl.u32);

    return;
}

hi_void vdp_vid_settvdbhdrmuxenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32benable)
{
    u_v0_ctrl v0_ctrl;

    v0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v0_ctrl.u32)));
    v0_ctrl.bits.tv_db_hdr_mux = u32benable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v0_ctrl.u32)), v0_ctrl.u32);

    return;
}

hi_void vdp_vid_setdbhdrapprovemuxenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32benable)
{
    u_v0_ctrl v0_ctrl;

    v0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v0_ctrl.u32)));
    v0_ctrl.bits.hdr_approve_mux = u32benable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v0_ctrl.u32)), v0_ctrl.u32);

    return;
}

hi_void vdp_vid_setdbhdrmuxenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32benable)
{
    u_v1_ctrl v1_ctrl;

    v1_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v1_ctrl.u32)));
    v1_ctrl.bits.hdr_mux = u32benable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v1_ctrl.u32)), v1_ctrl.u32);

    return;
}

hi_void vdp_vid_sethdroutmaplinkctrl(vdp_regs_type *vdp_reg, xdp_hdr_outmap_link enhdroutmaplink)
{
    u_vp0_ctrl vp0_ctrl;

    vp0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vp0_ctrl.u32)));
    vp0_ctrl.bits.hdr_outmap_link = enhdroutmaplink;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vp0_ctrl.u32)), vp0_ctrl.u32);

    return;
}

hi_void vdp_vid_setdcilinkctrl(vdp_regs_type *vdp_reg, xdp_dci_link endcilink)
{
    u_v0_ctrl v0_ctrl;

    v0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v0_ctrl.u32)));
    v0_ctrl.bits.dci_link = endcilink;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v0_ctrl.u32)), v0_ctrl.u32);

    return;
}

hi_void vdp_vid_sethdrlinkctrl(vdp_regs_type *vdp_reg, xdp_hdr_link enhdrlink)
{
    u_v0_ctrl v0_ctrl;

    v0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v0_ctrl.u32)));
    v0_ctrl.bits.hdr_link = enhdrlink;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v0_ctrl.u32)), v0_ctrl.u32);

    return;
}

hi_void vdp_vid_set8kzmelinkctrl(vdp_regs_type *vdp_reg, xdp_8kzme_link en8kzmelink)
{
    u_v0_ctrl v0_ctrl;

    v0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v0_ctrl.u32)));
    v0_ctrl.bits.zme_8k120_link = en8kzmelink;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v0_ctrl.u32)), v0_ctrl.u32);

    return;
}

hi_void vdp_vid_set8ksrlinkctrl(vdp_regs_type *vdp_reg, xdp_8ksr_link en8ksrlink)
{
    u_v0_ctrl v0_ctrl;

    v0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v0_ctrl.u32)));
    v0_ctrl.bits.sr_8k60_link = en8ksrlink;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v0_ctrl.u32)), v0_ctrl.u32);

    return;
}

hi_void vdp_vid_setacmlinkctrl(vdp_regs_type *vdp_reg, xdp_acm_link enacmlink)
{
    u_v0_ctrl v0_ctrl;

    v0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v0_ctrl.u32)));
    v0_ctrl.bits.acm_link = enacmlink;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v0_ctrl.u32)), v0_ctrl.u32);

    return;
}

// vid layer drv
hi_void vdp_vid_setlayerenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32benable)
{
    u_v1_ctrl v1_ctrl;

    v1_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v1_ctrl.u32) + g_vdp_vid_reg[data]));
    v1_ctrl.bits.surface_en = u32benable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v1_ctrl.u32) + g_vdp_vid_reg[data]), v1_ctrl.u32);

    return;
}

hi_void vdp_vid_setlayergalpha(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 alpha0)
{
    u_v1_ctrl v1_ctrl;
    u_v1_alpha v1_alpha;

    v1_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v1_ctrl.u32) + g_vdp_vid_reg[data]));
    v1_ctrl.bits.galpha = alpha0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v1_ctrl.u32) + g_vdp_vid_reg[data]), v1_ctrl.u32);

    v1_alpha.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v1_alpha.u32) + g_vdp_vid_reg[data]));
    v1_alpha.bits.vbk_alpha = alpha0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v1_alpha.u32) + g_vdp_vid_reg[data]), v1_alpha.u32);
    return;
}

hi_void vdp_vid_setnosecflag(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32enable)
{
    u_v1_ctrl v1_ctrl;

    v1_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v1_ctrl.u32) + g_vdp_vid_reg[data]));
    v1_ctrl.bits.nosec_flag = u32enable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v1_ctrl.u32) + g_vdp_vid_reg[data]), v1_ctrl.u32);

    return;
}

hi_void vdp_vid_setregup(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_v1_upd v1_upd;

    v1_upd.bits.regup = 0x1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v1_upd.u32) + g_vdp_vid_reg[data]), v1_upd.u32);

    return;
}

hi_bool vdp_vid_get_wager_mark_status(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_v1_upd v1_upd;

    v1_upd.u32 = vdp_regread ((uintptr_t)(&(vdp_reg->v1_upd.u32) +
        g_vdp_vid_reg[data]));

    return v1_upd.bits.regup;
}

/* vid lbox */
hi_void vdp_vid_setvideopos(vdp_regs_type *vdp_reg, hi_u32 data, vdp_rect rect)
{
    u_v1_vfpos v1_vfpos;
    u_v1_vlpos v1_vlpos;

    /* video position */
    v1_vfpos.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v1_vfpos.u32) + g_vdp_vid_reg[data]));
    v1_vfpos.bits.video_xfpos = rect.x;
    v1_vfpos.bits.video_yfpos = rect.y;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v1_vfpos.u32) + g_vdp_vid_reg[data]), v1_vfpos.u32);

    v1_vlpos.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v1_vlpos.u32) + g_vdp_vid_reg[data]));
    v1_vlpos.bits.video_xlpos = rect.x + rect.w - 1;
    v1_vlpos.bits.video_ylpos = rect.y + rect.h - 1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v1_vlpos.u32) + g_vdp_vid_reg[data]), v1_vlpos.u32);
    return;
}

hi_void vdp_vid_setdisppos(vdp_regs_type *vdp_reg, hi_u32 data, vdp_rect rect)
{
    u_v1_dfpos v1_dfpos;
    u_v1_dlpos v1_dlpos;

    /* video position */
    v1_dfpos.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v1_dfpos.u32) + g_vdp_vid_reg[data]));
    v1_dfpos.bits.disp_xfpos = rect.x;
    v1_dfpos.bits.disp_yfpos = rect.y;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v1_dfpos.u32) + g_vdp_vid_reg[data]), v1_dfpos.u32);

    v1_dlpos.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v1_dlpos.u32) + g_vdp_vid_reg[data]));
    v1_dlpos.bits.disp_xlpos = rect.x + rect.w - 1;
    v1_dlpos.bits.disp_ylpos = rect.y + rect.h - 1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v1_dlpos.u32) + g_vdp_vid_reg[data]), v1_dlpos.u32);
    return;
}

hi_void vdp_vid_setlayerbkg(vdp_regs_type *vdp_reg, hi_u32 data, vdp_bkg stbkg)
{
    u_v1_bk v1_bk;

    v1_bk.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v1_bk.u32) + g_vdp_vid_reg[data]));
    v1_bk.bits.vbk_y = stbkg.y;
    v1_bk.bits.vbk_cb = stbkg.u;
    v1_bk.bits.vbk_cr = stbkg.v;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v1_bk.u32) + g_vdp_vid_reg[data]), v1_bk.u32);

    return;
}

hi_void vdp_vid_setlboxmutebkg(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 mute_en, vdp_bkg stmutebkg)
{
    u_v1_mute_bk v1_mute_bk;

    v1_mute_bk.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v1_mute_bk.u32) + g_vdp_vid_reg[data]));
    v1_mute_bk.bits.mute_en = mute_en;
    v1_mute_bk.bits.mute_y = stmutebkg.y;
    v1_mute_bk.bits.mute_cb = stmutebkg.u;
    v1_mute_bk.bits.mute_cr = stmutebkg.v;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v1_mute_bk.u32) + g_vdp_vid_reg[data]), v1_mute_bk.u32);

    return;
}

hi_void vdp_vid_setupdmode(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32mode)
{
    u_v1_ctrl v1_ctrl;

    v1_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v1_ctrl.u32) + g_vdp_vid_reg[data]));
    v1_ctrl.bits.rgup_mode = u32mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v1_ctrl.u32) + g_vdp_vid_reg[data]), v1_ctrl.u32);

    return;
}

hi_void vdp_vid_setsttregup(vdp_regs_type *vdp_reg, hi_u32 u32regup)
{
    u_stt_rupd stt_rupd;

    stt_rupd.u32 = vdp_regread((uintptr_t)(&(vdp_reg->stt_rupd.u32)));
    stt_rupd.bits.stt_rupd = u32regup;
    vdp_regwrite((uintptr_t)(&(vdp_reg->stt_rupd.u32)), stt_rupd.u32);

    return;
}

hi_void vdp_vid_setsttupdmode(vdp_regs_type *vdp_reg, hi_u32 u32mode)
{
    u_stt_ctrl stt_ctrl;

    stt_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->stt_ctrl.u32)));
    stt_ctrl.bits.stt_up_mode = u32mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->stt_ctrl.u32)), stt_ctrl.u32);

    return;
}

hi_void vdp_vid_setsttupfieldmode(vdp_regs_type *vdp_reg, hi_u32 u32fieldmode)
{
    u_stt_ctrl stt_ctrl;

    stt_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->stt_ctrl.u32)));
    stt_ctrl.bits.stt_up_field = u32fieldmode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->stt_ctrl.u32)), stt_ctrl.u32);

    return;
}

hi_void vdp_vid_setstten(vdp_regs_type *vdp_reg, hi_u32 u32stten)
{
    u_stt_ctrl stt_ctrl;

    stt_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->stt_ctrl.u32)));
    stt_ctrl.bits.stt_en = u32stten;
    vdp_regwrite((uintptr_t)(&(vdp_reg->stt_ctrl.u32)), stt_ctrl.u32);

    return;
}

hi_void vdp_vid_setsttaddrlow(vdp_regs_type *vdp_reg, hi_u32 u32addr)
{
    u_stt_addr_l stt_addr_l;

    stt_addr_l.u32 = vdp_regread((uintptr_t)(&(vdp_reg->stt_addr_l.u32)));
    stt_addr_l.bits.stt_addr_l = u32addr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->stt_addr_l.u32)), stt_addr_l.u32);

    return;
}

hi_void vdp_vid_setsttaddrhigh(vdp_regs_type *vdp_reg, hi_u32 u32addr)
{
    u_stt_addr_h stt_addr_h;

    stt_addr_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->stt_addr_h.u32)));
    stt_addr_h.bits.stt_addr_h = u32addr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->stt_addr_h.u32)), stt_addr_h.u32);

    return;
}

hi_void vdp_vid_setsttmmubypass(vdp_regs_type *vdp_reg, hi_u32 u32mmubypass)
{
    u_stt_smmu_ctrl stt_smmu_ctrl;

    stt_smmu_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->stt_smmu_ctrl.u32)));
    stt_smmu_ctrl.bits.stt_bypass = u32mmubypass;
    vdp_regwrite((uintptr_t)(&(vdp_reg->stt_smmu_ctrl.u32)), stt_smmu_ctrl.u32);

    return;
}

hi_void vdp_vid_setlumhistenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32benable)
{
    u_hist4hdr hist4hdr;

    hist4hdr.u32 = vdp_regread((uintptr_t)(&(vdp_reg->hist4hdr.u32)));
    hist4hdr.bits.hist4hdr_en = u32benable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->hist4hdr.u32)), hist4hdr.u32);

    return;
}

hi_void vdp_vid_setvidpressctrlenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32enable)
{
    u_vid_debug_ctrl vid_debug_ctrl;

    vid_debug_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vid_debug_ctrl.u32) + data * VID_OFFSET));
    vid_debug_ctrl.bits.fdr_press_en = u32enable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vid_debug_ctrl.u32) + data * VID_OFFSET), vid_debug_ctrl.u32);

    return;
}

hi_void vdp_vid_setvidpressctrlmindrdynum(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32mindrdynum)
{
    u_vid_press_ctrl vid_press_ctrl;

    if (data >= VID_MAX) {
        VDP_PRINT("Error) Select Wrong Video Layer ID\n");
        return;
    }

    vid_press_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vid_press_ctrl.u32) + data * VID_OFFSET));
    vid_press_ctrl.bits.min_drdy_num = u32mindrdynum;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vid_press_ctrl.u32) + data * VID_OFFSET), vid_press_ctrl.u32);

    return;
}

hi_void vdp_vid_setvidpressctrlmindrdynumscope(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32mindrdynumscop)
{
    u_vid_press_ctrl vid_press_ctrl;

    if (data >= VID_MAX) {
        VDP_PRINT("Error) Select Wrong Video Layer ID\n");
        return;
    }

    vid_press_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vid_press_ctrl.u32) + data * VID_OFFSET));
    vid_press_ctrl.bits.scope_drdy_num = u32mindrdynumscop;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vid_press_ctrl.u32) + data * VID_OFFSET), vid_press_ctrl.u32);

    return;
}

hi_void vdp_vid_setvidpressctrlminnodrdynum(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32minnodrdynum)
{
    u_vid_press_ctrl vid_press_ctrl;

    if (data >= VID_MAX) {
        VDP_PRINT("Error) Select Wrong Video Layer ID\n");
        return;
    }

    vid_press_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vid_press_ctrl.u32) + data * VID_OFFSET));
    vid_press_ctrl.bits.min_nodrdy_num = u32minnodrdynum;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vid_press_ctrl.u32) + data * VID_OFFSET), vid_press_ctrl.u32);

    return;
}

hi_void vdp_vid_setvidpressctrlminnodrdynumscope(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 u32minnodrdynumscop)
{
    u_vid_press_ctrl vid_press_ctrl;

    if (data >= VID_MAX) {
        VDP_PRINT("Error) Select Wrong Video Layer ID\n");
        return;
    }

    vid_press_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vid_press_ctrl.u32) + data * VID_OFFSET));
    vid_press_ctrl.bits.scope_nodrdy_num = u32minnodrdynumscop;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vid_press_ctrl.u32) + data * VID_OFFSET), vid_press_ctrl.u32);

    return;
}

hi_void vdp_vp_setlayergalpha(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 alpha)
{
    u_vp0_ctrl vp0_ctrl;

    /* special for bk alpha = video alpha */
    u_vp0_alpha vp0_alpha;

    vp0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vp0_ctrl.u32) + data * VP_OFFSET));
    vp0_ctrl.bits.galpha = alpha;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vp0_ctrl.u32) + data * VP_OFFSET), vp0_ctrl.u32);

    vp0_alpha.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vp0_alpha.u32) + data * VP_OFFSET));
    vp0_alpha.bits.vbk_alpha = alpha;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vp0_alpha.u32) + data * VP_OFFSET), vp0_alpha.u32);

    return;
}

hi_void vdp_vp_setvideopos(vdp_regs_type *vdp_reg, hi_u32 data, vdp_rect rect)
{
    u_vp0_vfpos vp0_vfpos;
    u_vp0_vlpos vp0_vlpos;

    /* video position */
    vp0_vfpos.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vp0_vfpos.u32) + data * VP_OFFSET));
    vp0_vfpos.bits.video_xfpos = rect.x;
    vp0_vfpos.bits.video_yfpos = rect.y;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vp0_vfpos.u32) + data * VP_OFFSET), vp0_vfpos.u32);

    vp0_vlpos.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vp0_vlpos.u32) + data * VP_OFFSET));
    vp0_vlpos.bits.video_xlpos = rect.x + rect.w - 1;
    vp0_vlpos.bits.video_ylpos = rect.y + rect.h - 1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vp0_vlpos.u32) + data * VP_OFFSET), vp0_vlpos.u32);

    return;
}

hi_void vdp_vp_setdisppos(vdp_regs_type *vdp_reg, hi_u32 data, vdp_rect rect)
{
    u_vp0_dfpos vp0_dfpos;
    u_vp0_dlpos vp0_dlpos;

    /* video position */
    vp0_dfpos.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vp0_dfpos.u32) + data * VP_OFFSET));
    vp0_dfpos.bits.disp_xfpos = rect.x;
    vp0_dfpos.bits.disp_yfpos = rect.y;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vp0_dfpos.u32) + data * VP_OFFSET), vp0_dfpos.u32);

    vp0_dlpos.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vp0_dlpos.u32) + data * VP_OFFSET));
    vp0_dlpos.bits.disp_xlpos = rect.x + rect.w - 1;
    vp0_dlpos.bits.disp_ylpos = rect.y + rect.h - 1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vp0_dlpos.u32) + data * VP_OFFSET), vp0_dlpos.u32);
    return;
}

hi_void vdp_vp_setinreso(vdp_regs_type *vdp_reg, hi_u32 data, vdp_rect rect)
{
    u_vp0_ireso vp0_ireso;

    vp0_ireso.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vp0_ireso.u32) + data * VP_OFFSET));
    vp0_ireso.bits.iw = rect.w - 1;
    vp0_ireso.bits.ih = rect.h - 1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vp0_ireso.u32) + data * VP_OFFSET), vp0_ireso.u32);

    return;
}

hi_void vdp_vp_setregup(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_vp0_upd vp0_upd;

    /* vp layer register update */
    vp0_upd.bits.regup = 0x1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vp0_upd.u32) + data * VP_OFFSET), vp0_upd.u32);

    return;
}

hi_void vdp_vp_setlayerbkg(vdp_regs_type *vdp_reg, hi_u32 data, vdp_bkg stbkg)
{
    u_vp0_bk vp0_bk;

    if (data == VDP_LAYER_VP0) {
        vp0_bk.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vp0_bk.u32)));
        vp0_bk.bits.vbk_y = stbkg.y;
        vp0_bk.bits.vbk_cb = stbkg.u;
        vp0_bk.bits.vbk_cr = stbkg.v;
        vdp_regwrite((uintptr_t)(&(vdp_reg->vp0_bk.u32)), vp0_bk.u32);
    }

    return;
}


hi_void vdp_vp_updatelayerenable(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_vp0_ctrl vp0_ctrl;
    u_v1_ctrl v1_ctrl;
    u_v1_ctrl v0_ctrl;
    v1_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v1_ctrl.u32) + g_vdp_vid_reg[VDP_LAYER_VID1]));
    v0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v1_ctrl.u32) + g_vdp_vid_reg[VDP_LAYER_VID0]));

    vp0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vp0_ctrl.u32) + data * VP_OFFSET));
    vp0_ctrl.bits.surface_en = (v1_ctrl.bits.surface_en || v0_ctrl.bits.surface_en);
    vdp_regwrite((uintptr_t)(&(vdp_reg->vp0_ctrl.u32) + data * VP_OFFSET), vp0_ctrl.u32);

    return;
}

hi_void vdp_vp_setthreedimdofenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 benable)
{
    u_vp0_dof_ctrl vp0_dof_ctrl;

    vp0_dof_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vp0_dof_ctrl.u32)));
    vp0_dof_ctrl.bits.dof_en = benable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vp0_dof_ctrl.u32)), vp0_dof_ctrl.u32);
}

hi_void vdp_vp_setthreedimdofstep(vdp_regs_type *vdp_reg, hi_u32 data, hi_s32 s32lstep, hi_s32 s32rstep)
{
    u_vp0_dof_step vp0_dof_step;

    vp0_dof_step.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vp0_dof_step.u32)));
    vp0_dof_step.bits.right_step = s32rstep;
    vp0_dof_step.bits.left_step = s32lstep;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vp0_dof_step.u32)), vp0_dof_step.u32);
}

hi_void vdp_vp_setthreedimdofbkg(vdp_regs_type *vdp_reg, hi_u32 data, vdp_bkg stbkg)
{
    u_vp0_dof_bkg vp0_dof_bkg;

    vp0_dof_bkg.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vp0_dof_bkg.u32)));
    vp0_dof_bkg.bits.dof_bk_y = stbkg.y;
    vp0_dof_bkg.bits.dof_bk_cb = stbkg.u;
    vp0_dof_bkg.bits.dof_bk_cr = stbkg.v;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vp0_dof_bkg.u32)), vp0_dof_bkg.u32);

    return;
}


