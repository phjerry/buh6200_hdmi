/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_comm.h"
#include "hal_vdp_reg_disp.h"

hi_void vdp_dsp_setchn0mux0(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_chn0_drpc chn0_drpc;

    chn0_drpc.u32 = vdp_regread((uintptr_t)(&(vdp_reg->chn0_drpc.u32)));
    chn0_drpc.bits.chn0_mux0 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->chn0_drpc.u32)), chn0_drpc.u32);

    return;
}

hi_void vdp_dsp_setchn0mux1(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_chn0_drpc chn0_drpc;

    chn0_drpc.u32 = vdp_regread((uintptr_t)(&(vdp_reg->chn0_drpc.u32)));
    chn0_drpc.bits.chn0_mux1 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->chn0_drpc.u32)), chn0_drpc.u32);

    return;
}

hi_void vdp_dsp_setchn0mux2(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_chn0_drpc chn0_drpc;

    chn0_drpc.u32 = vdp_regread((uintptr_t)(&(vdp_reg->chn0_drpc.u32)));
    chn0_drpc.bits.chn0_mux2 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->chn0_drpc.u32)), chn0_drpc.u32);

    return;
}

hi_void vdp_dsp_setchn0splitenable(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_chn0_split_combi0 chn0_split_combi0;

    chn0_split_combi0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->chn0_split_combi0.u32)));
    chn0_split_combi0.bits.enable = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->chn0_split_combi0.u32)), chn0_split_combi0.u32);

    return;
}

hi_void vdp_dsp_setchn0splitoverlap(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_chn0_split_combi0 chn0_split_combi0;

    chn0_split_combi0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->chn0_split_combi0.u32)));
    chn0_split_combi0.bits.overlap = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->chn0_split_combi0.u32)), chn0_split_combi0.u32);

    return;
}

static hi_void vdp_dispchn_set_cbm_mix0(vdp_regs_type *vdp_reg, hi_u32 layer_id, hi_u32 prio)
{
    u_cbm_mix1 cbm_mix1;

    switch (prio) {
        case 0: // 0 is reg value
            cbm_mix1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->cbm_mix1.u32)));
            cbm_mix1.bits.mixer_prio0 = layer_id;
            vdp_regwrite((uintptr_t)(&(vdp_reg->cbm_mix1.u32)), cbm_mix1.u32);
            break;
        case 1: // 1 is reg value
            cbm_mix1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->cbm_mix1.u32)));
            cbm_mix1.bits.mixer_prio1 = layer_id;
            vdp_regwrite((uintptr_t)(&(vdp_reg->cbm_mix1.u32)), cbm_mix1.u32);
            break;
        case 2: // 2 is reg value
            cbm_mix1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->cbm_mix1.u32)));
            cbm_mix1.bits.mixer_prio2 = layer_id;
            vdp_regwrite((uintptr_t)(&(vdp_reg->cbm_mix1.u32)), cbm_mix1.u32);
            break;
        default:
            VDP_PRINT("Error, Vou_SetCbmMixerPrio() Set mixer  select wrong layer ID\n");
            return;
    }

    return;
}

static hi_void vdp_dispchn_set_cbm_mixg0(vdp_regs_type *vdp_reg, hi_u32 layer_id, hi_u32 prio)
{
    u_mixg0_mix mixg0_mix;
    switch (prio) {
        case 0: // 0 is reg value
            mixg0_mix.u32 = vdp_regread((uintptr_t)(&(vdp_reg->mixg0_mix.u32)));
            mixg0_mix.bits.mixer_prio0 = layer_id;
            vdp_regwrite((uintptr_t)(&(vdp_reg->mixg0_mix.u32)), mixg0_mix.u32);
            break;
        case 1: // 1 is reg value
            mixg0_mix.u32 = vdp_regread((uintptr_t)(&(vdp_reg->mixg0_mix.u32)));
            mixg0_mix.bits.mixer_prio1 = layer_id;
            vdp_regwrite((uintptr_t)(&(vdp_reg->mixg0_mix.u32)), mixg0_mix.u32);
            break;
        case 2: // 2 is reg value
            mixg0_mix.u32 = vdp_regread((uintptr_t)(&(vdp_reg->mixg0_mix.u32)));
            mixg0_mix.bits.mixer_prio2 = layer_id;
            vdp_regwrite((uintptr_t)(&(vdp_reg->mixg0_mix.u32)), mixg0_mix.u32);
            break;
        default:
            VDP_PRINT("Error, Vou_SetCbmMixerPrio() Set mixer  select wrong layer ID\n");
            return;
    }

    return;
}

static hi_void vdp_dispchn_set_cbm_mixv0(vdp_regs_type *vdp_reg, hi_u32 layer_id, hi_u32 prio)
{
    u_mixv0_mix mixv0_mix;
    switch (prio) {
        case 0: // 0 is reg value
            mixv0_mix.u32 = vdp_regread((uintptr_t)(&(vdp_reg->mixv0_mix.u32)));
            mixv0_mix.bits.mixer_prio0 = layer_id;
            vdp_regwrite((uintptr_t)(&(vdp_reg->mixv0_mix.u32)), mixv0_mix.u32);
            break;
        case 1: // 1 is reg value
            mixv0_mix.u32 = vdp_regread((uintptr_t)(&(vdp_reg->mixv0_mix.u32)));
            mixv0_mix.bits.mixer_prio1 = layer_id;
            vdp_regwrite((uintptr_t)(&(vdp_reg->mixv0_mix.u32)), mixv0_mix.u32);
            break;
        case 2: // 2 is reg value
            mixv0_mix.u32 = vdp_regread((uintptr_t)(&(vdp_reg->mixv0_mix.u32)));
            mixv0_mix.bits.mixer_prio2 = layer_id;
            vdp_regwrite((uintptr_t)(&(vdp_reg->mixv0_mix.u32)), mixv0_mix.u32);
            break;
        default:
            VDP_PRINT("Error, Vou_SetCbmMixerPrio() Set mixer  select wrong layer ID\n");
            return;
    }

    return;
}

static hi_void vdp_dispchn_set_cbm_mix2(vdp_regs_type *vdp_reg, hi_u32 layer_id, hi_u32 prio)
{
    u_cbm_mix2 cbm_mix2;

    switch (prio) {
        case 0: // 0 is reg value
            cbm_mix2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->cbm_mix2.u32)));
            cbm_mix2.bits.mixer_prio0 = layer_id;
            vdp_regwrite((uintptr_t)(&(vdp_reg->cbm_mix2.u32)), cbm_mix2.u32);
            break;
        case 1: // 1 is reg value
            cbm_mix2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->cbm_mix2.u32)));
            cbm_mix2.bits.mixer_prio1 = layer_id;
            vdp_regwrite((uintptr_t)(&(vdp_reg->cbm_mix2.u32)), cbm_mix2.u32);
            break;
        case 2: // 2 is reg value
            cbm_mix2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->cbm_mix2.u32)));
            cbm_mix2.bits.mixer_prio2 = layer_id;
            vdp_regwrite((uintptr_t)(&(vdp_reg->cbm_mix2.u32)), cbm_mix2.u32);
            break;
        case 3: // 3 is reg value
            cbm_mix2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->cbm_mix2.u32)));
            cbm_mix2.bits.mixer_prio3 = layer_id;
            vdp_regwrite((uintptr_t)(&(vdp_reg->cbm_mix2.u32)), cbm_mix2.u32);
            break;
        default:
            VDP_PRINT("Error, Vou_SetCbmMixerPrio() Set mixer  select wrong layer ID\n");
            return;
    }

    return;
}

hi_void vdp_dispchn_set_cbm_mix(vdp_regs_type *vdp_reg, vdp_dispchn_cbm_mix mixer_id, hi_u32 layer_id, hi_u32 prio)
{
    /* DHD0 */
    if (mixer_id == VDP_DISPCHN_CBM_MIX0) {
        vdp_dispchn_set_cbm_mix0(vdp_reg, layer_id, prio);
    } else if (mixer_id == VDP_DISPCHN_CBM_MIXG0) {
        vdp_dispchn_set_cbm_mixg0(vdp_reg, layer_id, prio);
    } else if (mixer_id == VDP_DISPCHN_CBM_MIXG1) {
        /* reserved */
    } else if (mixer_id == VDP_DISPCHN_CBM_MIXV0) {
        vdp_dispchn_set_cbm_mixv0(vdp_reg, layer_id, prio);
    } else if (mixer_id == VDP_DISPCHN_CBM_MIXV1) {
        /* reserved */
    } else if (mixer_id == VDP_DISPCHN_CBM_MIX1) {
        vdp_dispchn_set_cbm_mix2(vdp_reg, layer_id, prio);
    }
    return;
}

hi_void vdp_dispchn_set_cbm_bkg(vdp_dispchn_cbm_mix chn_id, vdp_bkg *vdp_bkg)
{
    u_cbm_bkg1 cbm_bkg1;
    u_cbm_bkg2 cbm_bkg2;
    u_cbm_bkg3 cbm_bkg3;
    u_mixg0_bkg mixg0_bkg;
    u_mixg0_bkalpha mixg0_bkalpha; /* g0 mixer link */
    u_mixv0_bkg mixv0_bkg;

    if (chn_id == VDP_DISPCHN_CBM_MIX0) { /* DHD0  mixer link */
        cbm_bkg1.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->cbm_bkg1.u32)));
        cbm_bkg1.bits.cbm_bkgy1 = vdp_bkg->y;
        cbm_bkg1.bits.cbm_bkgcb1 = vdp_bkg->u;
        cbm_bkg1.bits.cbm_bkgcr1 = vdp_bkg->v;
        vdp_regwrite((uintptr_t)(&(g_vdp_reg->cbm_bkg1.u32)), cbm_bkg1.u32);
    } else if (chn_id == VDP_DISPCHN_CBM_MIX1) {
        cbm_bkg2.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->cbm_bkg2.u32)));
        cbm_bkg2.bits.cbm_bkgy2 = vdp_bkg->y;
        cbm_bkg2.bits.cbm_bkgcb2 = vdp_bkg->u;
        cbm_bkg2.bits.cbm_bkgcr2 = vdp_bkg->v;
        vdp_regwrite((uintptr_t)(&(g_vdp_reg->cbm_bkg2.u32)), cbm_bkg2.u32);
    } else if (chn_id == VDP_DISPCHN_CBM_MIX2) { /* DHD1 mixer link */
        cbm_bkg3.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->cbm_bkg3.u32)));
        cbm_bkg3.bits.cbm_bkgy3 = vdp_bkg->y;
        cbm_bkg3.bits.cbm_bkgcb3 = vdp_bkg->u;
        cbm_bkg3.bits.cbm_bkgcr3 = vdp_bkg->v;
        vdp_regwrite((uintptr_t)(&(g_vdp_reg->cbm_bkg3.u32)), cbm_bkg3.u32);
    } else if (chn_id == VDP_DISPCHN_CBM_MIXG0) {
        mixg0_bkg.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->mixg0_bkg.u32)));
        mixg0_bkg.bits.mixer_bkgy = vdp_bkg->y;
        mixg0_bkg.bits.mixer_bkgcb = vdp_bkg->u;
        mixg0_bkg.bits.mixer_bkgcr = vdp_bkg->v;
        vdp_regwrite((uintptr_t)(&(g_vdp_reg->mixg0_bkg.u32)), mixg0_bkg.u32);
        mixg0_bkalpha.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->mixg0_bkalpha.u32)));
        mixg0_bkalpha.bits.mixer_alpha = vdp_bkg->a;
        vdp_regwrite((uintptr_t)(&(g_vdp_reg->mixg0_bkalpha.u32)), mixg0_bkalpha.u32);
    } else if (chn_id == VDP_DISPCHN_CBM_MIXV0) {
        mixv0_bkg.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->mixv0_bkg.u32)));
        mixv0_bkg.bits.mixer_bkgy = vdp_bkg->y;
        mixv0_bkg.bits.mixer_bkgcb = vdp_bkg->u;
        mixv0_bkg.bits.mixer_bkgcr = vdp_bkg->v;
        vdp_regwrite((uintptr_t)(&(g_vdp_reg->mixv0_bkg.u32)), mixv0_bkg.u32);
    } else {
        VDP_PRINT("Error! VDP_CBM_SetMixerBkg() Select Wrong mixer ID\n");
    }

    return;
}

hi_void vdp_dispchn_reset_cbm_mix(vdp_regs_type *vdp_reg, vdp_dispchn_cbm_mix mixer_id)
{
    u_cbm_mix1 cbm_mix1;
    u_cbm_mix2 cbm_mix2;
    u_mixg0_mix mixg0_mix;
    u_mixv0_mix mixv0_mix;

    /* DHD0 */
    if (mixer_id == VDP_DISPCHN_CBM_MIX0) {
        cbm_mix1.u32 = 0;
        vdp_regwrite((uintptr_t)(&(vdp_reg->cbm_mix1.u32)), cbm_mix1.u32);
    } else if (mixer_id == VDP_DISPCHN_CBM_MIXG0) {
        mixg0_mix.u32 = 0;
        vdp_regwrite((uintptr_t)(&(vdp_reg->mixg0_mix.u32)), mixg0_mix.u32);
    } else if (mixer_id == VDP_DISPCHN_CBM_MIXV0) {
        mixv0_mix.u32 = 0;
        vdp_regwrite((uintptr_t)(&(vdp_reg->mixv0_mix.u32)), mixv0_mix.u32);
    } else if (mixer_id == VDP_DISPCHN_CBM_MIXV1) {
        /* reserved */
    } else if (mixer_id == VDP_DISPCHN_CBM_MIX1) { /* DHD1 */
        cbm_mix2.u32 = 0;
        vdp_regwrite((uintptr_t)(&(vdp_reg->cbm_mix2.u32)), cbm_mix2.u32);
    } else {
        VDP_PRINT("Error, VDP_CBM_ResetMixerPrio() Set mixer  select wrong layer ID\n");
    }

    return;
}

hi_u32 vdp_dispchn_get_voint(vdp_regs_type *vdp_reg)
{
    return vdp_regread ((uintptr_t)(&(vdp_reg->vointsta.u32)));
}

hi_u32 vdp_dispchn_get_voint_masked(vdp_regs_type *vdp_reg)
{
    return vdp_regread ((uintptr_t)(&(vdp_reg->vomskintsta.u32)));
}

hi_void vdp_dispchn_clean_voint(vdp_regs_type *vdp_reg, hi_u32 clean_value)
{
    u_vomskintsta vomskintsta;

    /* Dispaly interrupt mask enable */
    vomskintsta.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vomskintsta.u32)));
    vomskintsta.u32 = vomskintsta.u32 | clean_value;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vomskintsta.u32)), vomskintsta.u32);

    return;
}

