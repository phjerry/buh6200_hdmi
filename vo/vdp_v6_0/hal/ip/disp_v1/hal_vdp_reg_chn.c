/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_comm.h"
#include "hal_vdp_reg_chn.h"

hi_void vdp_disp_setufoffineen(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 ufofflineen)
{
    u_dhd0_ctrl dhd0_ctrl;

    dhd0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET));
    dhd0_ctrl.bits.uf_offline_en = ufofflineen;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET), dhd0_ctrl.u32);
}

hi_void vdp_disp_setdispmode(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 dispmode)
{
    u_dhd0_ctrl dhd0_ctrl;

    dhd0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET));
    dhd0_ctrl.bits.disp_mode = dispmode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET), dhd0_ctrl.u32);
}

hi_void vdp_disp_sethdmimode(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 hdmi_md)
{
    u_dhd0_ctrl dhd0_ctrl;

    dhd0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET));
    dhd0_ctrl.bits.hdmi_mode = hdmi_md;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET), dhd0_ctrl.u32);
}

hi_void vdp_disp_setfpintersync(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 fpintersync)
{
    u_dhd0_ctrl dhd0_ctrl;

    dhd0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET));
    dhd0_ctrl.bits.fp_inter_sync = fpintersync;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET), dhd0_ctrl.u32);
}

hi_void vdp_disp_setfpgalmten(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 fpgalmten)
{
    u_dhd0_ctrl dhd0_ctrl;

    dhd0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET));
    dhd0_ctrl.bits.fpga_lmt_en = fpgalmten;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET), dhd0_ctrl.u32);
}

hi_void vdp_disp_setfpgalmtwidth(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 fpgalmtwidth)
{
    u_dhd0_ctrl dhd0_ctrl;

    dhd0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET));
    dhd0_ctrl.bits.fpga_lmt_width = fpgalmtwidth;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET), dhd0_ctrl.u32);
}

hi_void vdp_disp_setmirroren(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 mirroren)
{
    u_dhd0_ctrl dhd0_ctrl;

    dhd0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET));
    dhd0_ctrl.bits.mirror_en = mirroren;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET), dhd0_ctrl.u32);
}

hi_void vdp_disp_sethdmi420enable(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 enable)
{
    u_dhd0_ctrl1 dhd0_ctrl1;

    dhd0_ctrl1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_ctrl1.u32) + chn_id * CHN_OFFSET));
    dhd0_ctrl1.bits.hdmi420_en = enable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_ctrl1.u32) + chn_id * CHN_OFFSET), dhd0_ctrl1.u32);
}

hi_void vdp_disp_sethdmi420csel(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 csel)
{
    u_dhd0_ctrl1 dhd0_ctrl1;

    dhd0_ctrl1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_ctrl1.u32) + chn_id * CHN_OFFSET));
    dhd0_ctrl1.bits.hdmi420c_sel = csel;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_ctrl1.u32) + chn_id * CHN_OFFSET), dhd0_ctrl1.u32);

    return;
}

hi_void vdp_disp_setregup(vdp_regs_type *vdp_reg, hi_u32 chn_id)
{
    u_dhd0_ctrl dhd0_ctrl;
    u_chn0_upd chn0_upd;

    dhd0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET));
    chn0_upd.u32 = vdp_regread((uintptr_t)(&(vdp_reg->chn0_upd.u32) + chn_id * CHN_OFFSET));
    dhd0_ctrl.bits.regup = 0x1;
    chn0_upd.bits.regup = 0x1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET), dhd0_ctrl.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->chn0_upd.u32) + chn_id * CHN_OFFSET), chn0_upd.u32);

    return;
}

hi_void vdp_disp_setintfenable(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 btrue)
{
    u_dhd0_ctrl dhd0_ctrl;

    dhd0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET));
    dhd0_ctrl.bits.intf_en = btrue;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET), dhd0_ctrl.u32);


    return;
}

hi_bool vdp_disp_getintfenable(vdp_regs_type *vdp_reg, hi_u32 chn_id)
{
    u_dhd0_ctrl dhd0_ctrl;

    dhd0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET));
    return dhd0_ctrl.bits.intf_en;
}


hi_void vdp_disp_setintmask(vdp_regs_type *vdp_reg, hi_u32 masktypeen)
{
    u_vointmsk vointmsk;

    /* dispaly interrupt mask enable */
    vointmsk.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vointmsk.u32)));
    vointmsk.u32 = vointmsk.u32 | masktypeen;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vointmsk.u32)), vointmsk.u32);

    return;
}

hi_void vdp_disp_setintdisable(vdp_regs_type *vdp_reg, hi_u32 masktypeen)
{
    u_vointmsk vointmsk;

    /* dispaly interrupt mask enable */
    vointmsk.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vointmsk.u32)));
    vointmsk.u32 = vointmsk.u32 & (~masktypeen);
    vdp_regwrite((uintptr_t)(&(vdp_reg->vointmsk.u32)), vointmsk.u32);

    return;
}

hi_void vdp_disp_bfmclksel(vdp_regs_type *vdp_reg, hi_u32 num)
{
    u_vodebug vodebug;
    /* dispaly interrupt mask enable */
    vodebug.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vodebug.u32)));
    vodebug.bits.bfm_clk_sel = num;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vodebug.u32)), vodebug.u32);
    return;
}

hi_void vdp_disp_openintf(vdp_regs_type *vdp_reg, hi_u32 chn_id, vdp_sync_info stsyncinfo)
{
    u_dhd0_ctrl dhd0_ctrl;
    u_dhd0_vsync1 dhd0_vsync1;
    u_dhd0_vsync2 dhd0_vsync2;
    u_dhd0_vplus1 dhd0_vplus1;
    u_dhd0_vplus2 dhd0_vplus2;
    u_dhd0_pwr dhd0_pwr;
    u_dhd0_hsync1 dhd0_hsync1;
    u_dhd0_hsync2 dhd0_hsync2;

    dhd0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET));
    dhd0_ctrl.bits.iop = stsyncinfo.iop;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET), dhd0_ctrl.u32);

    dhd0_hsync1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_hsync1.u32) + chn_id * CHN_OFFSET));
    dhd0_hsync2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_hsync2.u32) + chn_id * CHN_OFFSET));
    dhd0_hsync1.bits.hact = stsyncinfo.hact - 1;
    dhd0_hsync1.bits.hbb = stsyncinfo.hbb - 1;
    dhd0_hsync2.bits.hfb = stsyncinfo.hfb - 1;
    dhd0_hsync2.bits.hmid = (stsyncinfo.hmid == 0) ? 0 : stsyncinfo.hmid - 1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_hsync1.u32) + chn_id * CHN_OFFSET), dhd0_hsync1.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_hsync2.u32) + chn_id * CHN_OFFSET), dhd0_hsync2.u32);

    /* config vhd interface veritical timming */
    dhd0_vsync1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_vsync1.u32) + chn_id * CHN_OFFSET));
    dhd0_vsync2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_vsync2.u32) + chn_id * CHN_OFFSET));
    dhd0_vsync1.bits.vact = stsyncinfo.vact - 1;
    dhd0_vsync1.bits.vbb = stsyncinfo.vbb - 1;
    dhd0_vsync2.bits.vfb = stsyncinfo.vfb - 1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_vsync1.u32) + chn_id * CHN_OFFSET), dhd0_vsync1.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_vsync2.u32) + chn_id * CHN_OFFSET), dhd0_vsync2.u32);

    /* config vhd interface veritical bottom timming,no use in progressive mode */
    dhd0_vplus1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_vplus1.u32) + chn_id * CHN_OFFSET));
    dhd0_vplus2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_vplus2.u32) + chn_id * CHN_OFFSET));
    dhd0_vplus1.bits.bvact = stsyncinfo.bvact - 1;
    dhd0_vplus1.bits.bvbb = stsyncinfo.bvbb - 1;
    dhd0_vplus2.bits.bvfb = stsyncinfo.bvfb - 1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_vplus1.u32) + chn_id * CHN_OFFSET), dhd0_vplus1.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_vplus2.u32) + chn_id * CHN_OFFSET), dhd0_vplus2.u32);

    /* config vhd interface veritical bottom timming */
    dhd0_pwr.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_pwr.u32) + chn_id * CHN_OFFSET));
    dhd0_pwr.bits.hpw = stsyncinfo.hpw - 1;
    dhd0_pwr.bits.vpw = stsyncinfo.vpw - 1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_pwr.u32) + chn_id * CHN_OFFSET), dhd0_pwr.u32);
}

hi_void vdp_disp_setchksumen(vdp_regs_type *vdp_reg, hi_u32 chksumen)
{
    u_dhd_top_ctrl dhd_top_ctrl;

    dhd_top_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd_top_ctrl.u32)));
    dhd_top_ctrl.bits.chk_sum_en = chksumen;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd_top_ctrl.u32)), dhd_top_ctrl.u32);
}

hi_void vdp_disp_setsyncinv(vdp_regs_type *vdp_reg, hi_u32 chn_id, vdp_disp_syncinv *eninv)
{
    u_intf_hdmi_sync_inv intf_hdmi_sync_inv;

    intf_hdmi_sync_inv.u32 = vdp_regread((uintptr_t)(&(vdp_reg->intf_hdmi_sync_inv.u32) + chn_id * INTF_OFFSET));
    intf_hdmi_sync_inv.bits.f_inv = eninv->fd_inv;
    intf_hdmi_sync_inv.bits.vs_inv = eninv->vs_inv;
    intf_hdmi_sync_inv.bits.hs_inv = eninv->hs_inv;
    intf_hdmi_sync_inv.bits.dv_inv = eninv->dv_inv;
    vdp_regwrite((uintptr_t)(&(vdp_reg->intf_hdmi_sync_inv.u32) + chn_id * INTF_OFFSET), intf_hdmi_sync_inv.u32);
}

hi_void vdp_disp_setdateclip0len(vdp_regs_type *vdp_reg, hi_bool enable)
{
    u_date_clip0_l date_clip0_l;

    date_clip0_l.u32 = vdp_regread((uintptr_t)(&(vdp_reg->date_clip0_l.u32)));
    date_clip0_l.bits.clip_en = enable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->date_clip0_l.u32)), date_clip0_l.u32);
}

hi_void vdp_disp_setdatecoeff0(vdp_regs_type *vdp_reg, hi_u32 date)
{
    u_date_coeff0 date_coeff0;

    date_coeff0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->date_coeff0.u32)));
    date_coeff0.u32 = date;
    vdp_regwrite((uintptr_t)(&(vdp_reg->date_coeff0.u32)), date_coeff0.u32);
}

hi_void vdp_disp_setdacenable(vdp_regs_type *vdp_reg, hi_bool enable)
{
    u_vo_dac_ctrl vo_dac_ctrl;

    vo_dac_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vo_dac_ctrl.u32)));
    vo_dac_ctrl.bits.envbg = enable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vo_dac_ctrl.u32)), vo_dac_ctrl.u32);
}

hi_void vdp_disp_setdac0ctrl(vdp_regs_type *vdp_reg, hi_bool enable, hi_u32 dacgc)
{
    u_vo_dac0_ctrl vo_dac0_ctrl;

    vo_dac0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vo_dac0_ctrl.u32)));
    vo_dac0_ctrl.bits.dac_en = enable;
    vo_dac0_ctrl.bits.dacgc = dacgc;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vo_dac0_ctrl.u32)), vo_dac0_ctrl.u32);
}

hi_void vdp_disp_setdac1ctrl(vdp_regs_type *vdp_reg, hi_bool enable, hi_u32 dacgc)
{
    u_vo_dac1_ctrl vo_dac1_ctrl;

    vo_dac1_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vo_dac1_ctrl.u32)));
    vo_dac1_ctrl.bits.dac_en = enable;
    vo_dac1_ctrl.bits.dacgc = dacgc;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vo_dac1_ctrl.u32)), vo_dac1_ctrl.u32);
}

hi_void vdp_disp_setdac2ctrl(vdp_regs_type *vdp_reg, hi_bool enable, hi_u32 dacgc)
{
    u_vo_dac2_ctrl vo_dac2_ctrl;

    vo_dac2_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vo_dac2_ctrl.u32)));
    vo_dac2_ctrl.bits.dac_en = enable;
    vo_dac2_ctrl.bits.dacgc = dacgc;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vo_dac2_ctrl.u32)), vo_dac2_ctrl.u32);
}

hi_void vdp_disp_setdac3ctrl(vdp_regs_type *vdp_reg, hi_bool enable, hi_u32 dacgc)
{
    u_vo_dac3_ctrl vo_dac3_ctrl;

    vo_dac3_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vo_dac3_ctrl.u32)));
    vo_dac3_ctrl.bits.dac_en = enable;
    vo_dac3_ctrl.bits.dacgc = dacgc;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vo_dac3_ctrl.u32)), vo_dac3_ctrl.u32);
}

hi_void vdp_disp_setdatecoeff21(vdp_regs_type *vdp_reg, vdp_disp_intf intf)
{
    u_date_coeff21 date_coeff21;

    date_coeff21.u32 = vdp_regread((uintptr_t)(&(vdp_reg->date_coeff21.u32)));

    switch (intf) {
        case VDP_DISP_INTF_CVBS:
            date_coeff21.bits.dac0_in_sel = 0x1;
            date_coeff21.bits.dac1_in_sel = 0x1;
            date_coeff21.bits.dac2_in_sel = 0x1;
            date_coeff21.bits.dac3_in_sel = 0x1;
            break;
        case VDP_DISP_INTF_HDMI:
        case VDP_DISP_INTF_HDMI0:
        case VDP_DISP_INTF_HDMI1:
            date_coeff21.bits.dac0_in_sel = 0x0;
            date_coeff21.bits.dac1_in_sel = 0x0;
            date_coeff21.bits.dac2_in_sel = 0x0;
            date_coeff21.bits.dac3_in_sel = 0x0;
            break;
        default:
            break;
    }

    vdp_regwrite((uintptr_t)(&(vdp_reg->date_coeff21.u32)), date_coeff21.u32);
}

hi_void vdp_disp_setintfmuxdac(vdp_regs_type *vdp_reg, vdp_disp_intf intf)
{
    u_vomux_dac vomux_dac;

    vomux_dac.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vomux_dac.u32)));

    switch (intf) {
        case VDP_DISP_INTF_CVBS:
            vomux_dac.bits.dac0_sel = 0x0;
            vomux_dac.bits.dac1_sel = 0x0;
            vomux_dac.bits.dac2_sel = 0x0;
            vomux_dac.bits.dac3_sel = 0x0;
            break;
        default:
            break;
    }

    vdp_regwrite((uintptr_t)(&(vdp_reg->vomux_dac.u32)), vomux_dac.u32);
}

hi_void vdp_disp_setintfmuxsel(vdp_regs_type *vdp_reg, hi_u32 chn_id, vdp_disp_intf intf)
{
    u_vo_mux vo_mux;

    vo_mux.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vo_mux.u32)));

    switch (intf) {
        case VDP_DISP_INTF_LCD:
            vo_mux.bits.digital_sel = 2; // 2 is reg value
            vo_mux.bits.lcd_sel = chn_id;
            break;
        case VDP_DISP_INTF_BT1120:
            vo_mux.bits.bt_sel = chn_id;
            vo_mux.bits.digital_sel = 0;
            break;
        case VDP_DISP_INTF_BT656:
            vo_mux.bits.bt_sel = chn_id;
            vo_mux.bits.digital_sel = 1;
            break;
        case VDP_DISP_INTF_MIPI:
            vo_mux.bits.mipi_sel = chn_id;
            break;
        case VDP_DISP_INTF_HDMI:
            vo_mux.bits.hdmi0_sel = chn_id;
            break;
        case VDP_DISP_INTF_HDMI1:
            vo_mux.bits.hdmi1_sel = chn_id;
            break;
        case VDP_DISP_INTF_VGA:
            vo_mux.bits.vga_sel = chn_id;
            break;
        case VDP_DISP_INTF_CVBS:
            vo_mux.bits.cvbs_sel = chn_id;
            break;
        case VDP_DISP_INTF_VBO:
            vo_mux.bits.vbo_sel = chn_id;
            break;
        default:
            VDP_PRINT("Error! VDP_DISP_SetIntfMuxSel Wrong Select\n");
            return;
    }

    vdp_regwrite((uintptr_t)(&(vdp_reg->vo_mux.u32)), vo_mux.u32);

    return;
}

hi_u32 vdp_disp_getintsta(vdp_regs_type *vdp_reg, hi_u32 intmask)
{
    u_vointsta vointsta;

    /* read interrupt status */
    vointsta.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vointsta.u32)));

    return (vointsta.u32 & intmask);
}

hi_u32 vdp_disp_getdhdstate(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 *pu32vback_blank, hi_u32 *pu32count_vcnt)
{
    u_dhd0_state dhd0_state;

    /* read interrupt status */
    dhd0_state.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_state.u32) + data * CHN_OFFSET));

    *pu32vback_blank = dhd0_state.bits.vback_blank;

    *pu32count_vcnt = dhd0_state.bits.count_vcnt;

    return (dhd0_state.u32);
}

hi_void vdp_disp_clearintsta(vdp_regs_type *vdp_reg, hi_u32 intmask)
{
    /* clear interrupt status */
    vdp_regwrite((uintptr_t) & (vdp_reg->vomskintsta.u32), intmask);
}

hi_void vdp_disp_setvtthdmode(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 uthdnum, hi_u32 mode)
{
    u_dhd0_vtthd5 dhd0_vtthd5;
    u_dhd0_vtthd3 dhd0_vtthd3;
    u_dhd0_vtthd dhd0_vtthd;

    if (uthdnum == 1) {
        dhd0_vtthd.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_vtthd.u32) + chn_id * CHN_OFFSET));
        dhd0_vtthd.bits.thd1_mode = mode;  // frame or field
        vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_vtthd.u32) + chn_id * CHN_OFFSET), dhd0_vtthd.u32);
    } else if (uthdnum == 2)  { // 2 is reg value
        dhd0_vtthd.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_vtthd.u32) + chn_id * CHN_OFFSET));
        dhd0_vtthd.bits.thd2_mode = mode;  // frame or field
        vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_vtthd.u32) + chn_id * CHN_OFFSET), dhd0_vtthd.u32);
    } else if (uthdnum == 3) { // 3 is reg value
        dhd0_vtthd3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_vtthd3.u32) + chn_id * CHN_OFFSET));
        dhd0_vtthd3.bits.thd3_mode = mode;  // frame or field
        vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_vtthd3.u32) + chn_id * CHN_OFFSET), dhd0_vtthd3.u32);
    } else if (uthdnum == 4) {  // 4 is reg value
        dhd0_vtthd3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_vtthd3.u32) + chn_id * CHN_OFFSET));
        dhd0_vtthd3.bits.thd4_mode = mode;  /* frame or field */
        vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_vtthd3.u32) + chn_id * CHN_OFFSET), dhd0_vtthd3.u32);
    } else if (uthdnum == 5) { // 5 is reg value
        dhd0_vtthd5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_vtthd5.u32) + chn_id * CHN_OFFSET));
        dhd0_vtthd5.bits.thd5_mode = mode;  /* frame or field */
        vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_vtthd5.u32) + chn_id * CHN_OFFSET), dhd0_vtthd5.u32);
    } else if (uthdnum == 6) { // 6 is reg value
        dhd0_vtthd5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_vtthd5.u32) + chn_id * CHN_OFFSET));
        dhd0_vtthd5.bits.thd6_mode = mode;  /* frame or field */
        vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_vtthd5.u32) + chn_id * CHN_OFFSET), dhd0_vtthd5.u32);
    }
}

hi_void vdp_disp_setvtthd(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 uthdnum, hi_u32 vtthd)
{
    u_dhd0_vtthd dhd0_vtthd;
    u_dhd0_vtthd3 dhd0_vtthd3;
    u_dhd0_vtthd5 dhd0_vtthd5;

    if (uthdnum == 1) {
        dhd0_vtthd.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_vtthd.u32) + chn_id * CHN_OFFSET));
        dhd0_vtthd.bits.vtmgthd1 = vtthd;
        vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_vtthd.u32) + chn_id * CHN_OFFSET), dhd0_vtthd.u32);
    } else if (uthdnum == 2) { // 2 is reg value
        dhd0_vtthd.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_vtthd.u32) + chn_id * CHN_OFFSET));
        dhd0_vtthd.bits.vtmgthd2 = vtthd;
        vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_vtthd.u32) + chn_id * CHN_OFFSET), dhd0_vtthd.u32);
    } else if (uthdnum == 3) { // 3 is reg value
        dhd0_vtthd3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_vtthd3.u32) + chn_id * CHN_OFFSET));
        dhd0_vtthd3.bits.vtmgthd3 = vtthd;
        vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_vtthd3.u32) + chn_id * CHN_OFFSET), dhd0_vtthd3.u32);
    } else if (uthdnum == 4) { // 4 is reg value
        dhd0_vtthd3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_vtthd3.u32) + chn_id * CHN_OFFSET));
        dhd0_vtthd3.bits.vtmgthd4 = vtthd;
        vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_vtthd3.u32) + chn_id * CHN_OFFSET), dhd0_vtthd3.u32);
    } else if (uthdnum == 5) { // 5 is reg value
        dhd0_vtthd5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_vtthd5.u32) + chn_id * CHN_OFFSET));
        dhd0_vtthd5.bits.vtmgthd5 = vtthd;
        vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_vtthd5.u32) + chn_id * CHN_OFFSET), dhd0_vtthd5.u32);
    } else if (uthdnum == 6) { // 6 is reg value
        dhd0_vtthd5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_vtthd5.u32) + chn_id * CHN_OFFSET));
        dhd0_vtthd5.bits.vtmgthd6 = vtthd;
        vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_vtthd5.u32) + chn_id * CHN_OFFSET), dhd0_vtthd5.u32);
    }
}

hi_void vdp_disp_setintfhdmiinv(vdp_regs_type *vdp_reg, hi_u32 chn_id, vdp_disp_syncinv eninv)
{
    u_intf_hdmi_sync_inv intf_hdmi_sync_inv;

    intf_hdmi_sync_inv.u32 = vdp_regread((uintptr_t)(&(vdp_reg->intf_hdmi_sync_inv.u32)));
    intf_hdmi_sync_inv.bits.vs_inv = eninv.vs_inv;
    intf_hdmi_sync_inv.bits.hs_inv = eninv.hs_inv;
    intf_hdmi_sync_inv.bits.dv_inv = eninv.dv_inv;
    vdp_regwrite((uintptr_t)(&(vdp_reg->intf_hdmi_sync_inv.u32)), intf_hdmi_sync_inv.u32);
}

hi_void vdp_disp_setdhdinv(vdp_regs_type *vdp_reg, hi_u32 chn_id, vdp_disp_syncinv eninv)
{
    u_dhd0_ctrl dhd0_ctrl;

    dhd0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET));
    dhd0_ctrl.bits.intf_idv = eninv.dv_inv;
    dhd0_ctrl.bits.intf_ihs = eninv.hs_inv;
    dhd0_ctrl.bits.intf_ivs = eninv.vs_inv;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET), dhd0_ctrl.u32);
}

hi_void vdp_disp_setcbarenable(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 btrue)
{
    u_dhd0_ctrl dhd0_ctrl;

    dhd0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET));
    dhd0_ctrl.bits.cbar_en = btrue;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET), dhd0_ctrl.u32);
}

hi_void vdp_disp_setcbarsel(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 _cbar_sel)
{
    u_dhd0_ctrl dhd0_ctrl;

    dhd0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET));
    dhd0_ctrl.bits.cbar_sel = _cbar_sel;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET), dhd0_ctrl.u32);
}

hi_void vdp_disp_setcbarmode(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 _cbar_mode)
{
    u_dhd0_ctrl dhd0_ctrl;

    dhd0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET));
    dhd0_ctrl.bits.cbar_mode = _cbar_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET), dhd0_ctrl.u32);
}

hi_void vdp_disp_setreqstartlinepos(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 reqstartlinepos)
{
    u_dhd0_start_pos1 dhd0_start_pos1;

    dhd0_start_pos1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_start_pos1.u32) + chn_id * CHN_OFFSET));
    dhd0_start_pos1.bits.req_start_line_pos = reqstartlinepos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_start_pos1.u32) + chn_id * CHN_OFFSET), dhd0_start_pos1.u32);
    return;
}

hi_void vdp_disp_setreqstartpos(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 reqstartpos)
{
    u_dhd0_start_pos1 dhd0_start_pos1;

    dhd0_start_pos1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_start_pos1.u32) + chn_id * CHN_OFFSET));
    dhd0_start_pos1.bits.req_start_pos = reqstartpos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_start_pos1.u32) + chn_id * CHN_OFFSET), dhd0_start_pos1.u32);
    return;
}

hi_void vdp_disp_setprestartlinepos(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 prestartlinepos)
{
    u_dhd0_start_pos2 dhd0_start_pos2;

    dhd0_start_pos2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_start_pos2.u32) + chn_id * CHN_OFFSET));
    dhd0_start_pos2.bits.pre_start_line_pos = prestartlinepos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_start_pos2.u32) + chn_id * CHN_OFFSET), dhd0_start_pos2.u32);
    return;
}

hi_void vdp_disp_setprestartpos(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 prestartpos)
{
    u_dhd0_start_pos2 dhd0_start_pos2;

    dhd0_start_pos2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_start_pos2.u32) + chn_id * CHN_OFFSET));
    dhd0_start_pos2.bits.pre_start_pos = prestartpos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_start_pos2.u32) + chn_id * CHN_OFFSET), dhd0_start_pos2.u32);
    return;
}

hi_void vdp_disp_setrmbusy(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 rmbusy)
{
    u_dhd0_busy_mode dhd0_busy_mode;

    dhd0_busy_mode.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_busy_mode.u32) + chn_id * CHN_OFFSET));
    dhd0_busy_mode.bits.rm_busy = rmbusy;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_busy_mode.u32) + chn_id * CHN_OFFSET), dhd0_busy_mode.u32);
    return;
}

hi_void vdp_disp_setparabusymode(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 parabusymode)
{
    u_dhd0_busy_mode dhd0_busy_mode;

    dhd0_busy_mode.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_busy_mode.u32) + chn_id * CHN_OFFSET));
    dhd0_busy_mode.bits.para_busy_mode = parabusymode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_busy_mode.u32) + chn_id * CHN_OFFSET), dhd0_busy_mode.u32);
    return;
}

hi_void vdp_disp_setprotointerenable(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 enable)
{
    u_dhd0_ctrl dhd0_ctrl;

    dhd0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET));
    dhd0_ctrl.bits.p2i_en = enable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_ctrl.u32) + chn_id * CHN_OFFSET), dhd0_ctrl.u32);
}

hi_void vdp_disp_setreqpos(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 startpos)
{
    u_dhd0_start_pos1 dhd0_start_pos1;

    dhd0_start_pos1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_start_pos1.u32) + data * CHN_OFFSET));
    dhd0_start_pos1.bits.req_start_pos = startpos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_start_pos1.u32) + data * CHN_OFFSET), dhd0_start_pos1.u32);
}

hi_void vdp_disp_setstartpos(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 startpos)
{
    u_dhd0_start_pos dhd0_start_pos;

    dhd0_start_pos.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_start_pos.u32) + data * CHN_OFFSET));
    dhd0_start_pos.bits.start_pos = startpos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_start_pos.u32) + data * CHN_OFFSET), dhd0_start_pos.u32);
}

hi_void vdp_disp_setfistartpos(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 startpos)
{
    u_dhd0_start_pos dhd0_start_pos;

    dhd0_start_pos.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_start_pos.u32) + data * CHN_OFFSET));
    dhd0_start_pos.bits.fi_start_pos = startpos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_start_pos.u32) + data * CHN_OFFSET), dhd0_start_pos.u32);
}

hi_void vdp_disp_settimingstartpos(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 startpos)
{
    u_dhd0_start_pos dhd0_start_pos;

    dhd0_start_pos.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_start_pos.u32) + data * CHN_OFFSET));
    dhd0_start_pos.bits.timing_start_pos = startpos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_start_pos.u32) + data * CHN_OFFSET), dhd0_start_pos.u32);
}

hi_void vdp_disp_setreqstartpos1(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 startpos1)
{
    u_dhd0_parathd dhd0_parathd;

    dhd0_parathd.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_parathd.u32) + data * CHN_OFFSET));
    dhd0_parathd.bits.req_start_pos1 = startpos1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_parathd.u32) + data * CHN_OFFSET), dhd0_parathd.u32);
}

hi_void vdp_disp_setregupsameparaload(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 mode)
{
    u_dhd0_ctrl1 dhd0_ctrl1;

    dhd0_ctrl1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_ctrl1.u32) + data * CHN_OFFSET));
    dhd0_ctrl1.bits.regup_mode = mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_ctrl1.u32) + data * CHN_OFFSET), dhd0_ctrl1.u32);
}

hi_void vdp_disp_setparaloadpos(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 startpos)
{
    u_dhd0_parathd dhd0_parathd;

    dhd0_parathd.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_parathd.u32) + data * CHN_OFFSET));
    dhd0_parathd.bits.para_thd = startpos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_parathd.u32) + data * CHN_OFFSET), dhd0_parathd.u32);
}

hi_void vdp_disp_settconprethd(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 prethd)
{
    u_dhd0_precharge_thd dhd0_precharge_thd;

    dhd0_precharge_thd.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_precharge_thd.u32) + data * CHN_OFFSET));
    dhd0_precharge_thd.bits.tcon_precharge_thd = prethd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_precharge_thd.u32) + data * CHN_OFFSET), dhd0_precharge_thd.u32);
}

hi_void vdp_disp_setvsynctemode(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 vsynctemode)
{
    u_dhd0_precharge_thd dhd0_precharge_thd;

    dhd0_precharge_thd.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_precharge_thd.u32) + data * CHN_OFFSET));
    dhd0_precharge_thd.bits.vsync_te_mode = vsynctemode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_precharge_thd.u32) + data * CHN_OFFSET), dhd0_precharge_thd.u32);
}

hi_void vdp_disp_setintfmirroren(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 mirroren)
{
    u_dhd0_ctrl1 dhd0_ctrl1;

    dhd0_ctrl1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_ctrl1.u32) + data * CHN_OFFSET));
    dhd0_ctrl1.bits.mir_en = mirroren;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_ctrl1.u32) + data * CHN_OFFSET), dhd0_ctrl1.u32);
}
hi_void vdp_disp_setintfmultichnmode(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 multichn_mode)
{
    u_dhd0_ctrl1 dhd0_ctrl1;

    dhd0_ctrl1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_ctrl1.u32) + data * CHN_OFFSET));
    dhd0_ctrl1.bits.multichn_en = multichn_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_ctrl1.u32) + data * CHN_OFFSET), dhd0_ctrl1.u32);
}
hi_void vdp_disp_setsplitmode(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 splitmode)
{
    u_dhd0_ctrl1 dhd0_ctrl1;

    dhd0_ctrl1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_ctrl1.u32) + data * CHN_OFFSET));

    if (splitmode == VDP_SPLIT_MODE_4P_1SPLIT) {
        dhd0_ctrl1.bits.multichn_en = 2; // 2 is reg value
        dhd0_ctrl1.bits.multichn_split_mode = 2; // 2 is reg value
    } else if (splitmode == VDP_SPLIT_MODE_4P_4SPLIT) {
        dhd0_ctrl1.bits.multichn_en = 2; // 2 is reg value
        dhd0_ctrl1.bits.multichn_split_mode = 3; // 3 is reg value
    } else if (splitmode == VDP_SPLIT_MODE_4P_2SPLIT) {
        dhd0_ctrl1.bits.multichn_en = 2; // 2 is reg value
        dhd0_ctrl1.bits.multichn_split_mode = 1;
    } else if (splitmode == VDP_SPLIT_MODE_2P_1SPLIT) {
        dhd0_ctrl1.bits.multichn_en = 1;
        dhd0_ctrl1.bits.multichn_split_mode = 0;
    } else if (splitmode == VDP_SPLIT_MODE_2P_2SPLIT) {
        dhd0_ctrl1.bits.multichn_en = 1;
        dhd0_ctrl1.bits.multichn_split_mode = 1;
    } else if (splitmode == VDP_SPLIT_MODE_2P_ODDEVEN) {
        dhd0_ctrl1.bits.multichn_en = 1;
        dhd0_ctrl1.bits.multichn_split_mode = 0;
    } else if (splitmode == VDP_SPLIT_MODE_1P_1SPLIT) {
        dhd0_ctrl1.bits.multichn_en = 0;
        dhd0_ctrl1.bits.multichn_split_mode = 0;
    } else if (splitmode == VDP_SPLIT_MODE_8P_1SPLIT) {
        dhd0_ctrl1.bits.multichn_en = 3; // 3 is reg value
        dhd0_ctrl1.bits.multichn_split_mode = 0;
    } else if (splitmode == VDP_SPLIT_MODE_8P_2SPLIT) {
        dhd0_ctrl1.bits.multichn_en = 3; // 3 is reg value
        dhd0_ctrl1.bits.multichn_split_mode = 4; // 4 is reg value
    } else if (splitmode == VDP_SPLIT_MODE_8P_4SPLIT) {
        dhd0_ctrl1.bits.multichn_en = 3; // 3 is reg value
        dhd0_ctrl1.bits.multichn_split_mode = 5; // 5 is reg value
    } else if (splitmode == VDP_SPLIT_MODE_8P_8SPLIT) {
        dhd0_ctrl1.bits.multichn_en = 3; // 3 is reg value
        dhd0_ctrl1.bits.multichn_split_mode = 6; // 6 is reg value
    }

    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_ctrl1.u32) + data * CHN_OFFSET), dhd0_ctrl1.u32);
}

hi_void vdp_disp_setdhd1splitmode(vdp_regs_type *vdp_reg, hi_u32 splitmode)
{
    u_dhd1_ctrl1 dhd1_ctrl1;

    dhd1_ctrl1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd1_ctrl1.u32)));

    if (splitmode == VDP_SPLIT_MODE_4P_1SPLIT) {
        dhd1_ctrl1.bits.multichn_en = 2; // 2 is reg value
        dhd1_ctrl1.bits.multichn_split_mode = 2; // 2 is reg value
    } else if (splitmode == VDP_SPLIT_MODE_4P_4SPLIT) {
        dhd1_ctrl1.bits.multichn_en = 2; // 2 is reg value
        dhd1_ctrl1.bits.multichn_split_mode = 3; // 3 is reg value
    } else if (splitmode == VDP_SPLIT_MODE_4P_2SPLIT) {
        dhd1_ctrl1.bits.multichn_en = 2; // 2 is reg value
        dhd1_ctrl1.bits.multichn_split_mode = 1;
    } else if (splitmode == VDP_SPLIT_MODE_2P_1SPLIT) {
        dhd1_ctrl1.bits.multichn_en = 1;
        dhd1_ctrl1.bits.multichn_split_mode = 0;
    } else if (splitmode == VDP_SPLIT_MODE_2P_2SPLIT) {
        dhd1_ctrl1.bits.multichn_en = 1;
        dhd1_ctrl1.bits.multichn_split_mode = 1;
    } else if (splitmode == VDP_SPLIT_MODE_2P_ODDEVEN) {
        dhd1_ctrl1.bits.multichn_en = 1;
        dhd1_ctrl1.bits.multichn_split_mode = 0;
    } else if (splitmode == VDP_SPLIT_MODE_1P_1SPLIT) {
        dhd1_ctrl1.bits.multichn_en = 0;
        dhd1_ctrl1.bits.multichn_split_mode = 0;
    }
    if (splitmode == VDP_SPLIT_MODE_4P_2SPLIT) {
        dhd1_ctrl1.bits.multichn_en = 2; // 2 is reg value
        dhd1_ctrl1.bits.multichn_split_mode = 1;
    }

    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd1_ctrl1.u32)), dhd1_ctrl1.u32);
}

hi_void vdp_disp_setchnenable(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 en)
{
    u_chn0_ctrl chn0_ctrl;

    /* set vou dhd channel chn correct enable */
    chn0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->chn0_ctrl.u32) + chn_id * CHN_OFFSET));
    chn0_ctrl.bits.surface_en = en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->chn0_ctrl.u32) + chn_id * CHN_OFFSET), chn0_ctrl.u32);
}

hi_void vdp_disp_setupdmode(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 data)
{
    u_chn0_ctrl chn0_ctrl;

    /* set vou dhd channel mirror correct enable */
    chn0_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->chn0_ctrl.u32) + chn_id * CHN_OFFSET));
    chn0_ctrl.bits.rgup_mode = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->chn0_ctrl.u32) + chn_id * CHN_OFFSET), chn0_ctrl.u32);
}

hi_void vdp_disp_setsttmmubypass(vdp_regs_type *vdp_reg, hi_u32 sttbypass)
{
    u_wbc_smmu_bypass wbc_smmu_bypass;

    wbc_smmu_bypass.u32 = vdp_regread((uintptr_t)(&(vdp_reg->wbc_smmu_bypass.u32)));
    wbc_smmu_bypass.bits.stt_bypass = sttbypass;
    vdp_regwrite((uintptr_t)(&(vdp_reg->wbc_smmu_bypass.u32)), wbc_smmu_bypass.u32);
}

hi_void vdp_disp_setablsttaddrhigh(vdp_regs_type *vdp_reg, hi_u32 addr)
{
    u_wbc_stt_addr_h wbc_stt_addr_h;

    wbc_stt_addr_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->wbc_stt_addr_h.u32)));
    wbc_stt_addr_h.bits.wbc_stt_addr_h = addr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->wbc_stt_addr_h.u32)), wbc_stt_addr_h.u32);
}

hi_void vdp_disp_setablsttaddrlow(vdp_regs_type *vdp_reg, hi_u32 addr)
{
    u_wbc_stt_addr_l wbc_stt_addr_l;

    wbc_stt_addr_l.u32 = vdp_regread((uintptr_t)(&(vdp_reg->wbc_stt_addr_l.u32)));
    wbc_stt_addr_l.bits.wbc_stt_addr_l = addr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->wbc_stt_addr_l.u32)), wbc_stt_addr_l.u32);
}

hi_void vdp_disp_setclipenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 clipen)
{
    u_dhd0_clip_l dhd0_clip_l;

    dhd0_clip_l.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_clip_l.u32) + data * CHN_OFFSET));
    dhd0_clip_l.bits.intf_clip_en = clipen;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_clip_l.u32) + data * CHN_OFFSET), dhd0_clip_l.u32);
}

hi_void vdp_disp_setclipcoef(vdp_regs_type *vdp_reg, hi_u32 data, vdp_disp_clip stclipdata)
{
    u_dhd0_clip_l dhd0_clip_l;
    u_dhd0_clip_h dhd0_clip_h;

    dhd0_clip_l.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_clip_l.u32) + data * CHN_OFFSET));
    dhd0_clip_l.bits.intf_clip_cl2 = stclipdata.clip_low_y;
    dhd0_clip_l.bits.intf_clip_cl1 = stclipdata.clip_low_cb;
    dhd0_clip_l.bits.intf_clip_cl0 = stclipdata.clip_low_cr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_clip_l.u32) + data * CHN_OFFSET), dhd0_clip_l.u32);

    dhd0_clip_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_clip_h.u32) + data * CHN_OFFSET));
    dhd0_clip_h.bits.intf_clip_ch2 = stclipdata.clip_high_y;
    dhd0_clip_h.bits.intf_clip_ch1 = stclipdata.clip_high_cb;
    dhd0_clip_h.bits.intf_clip_ch0 = stclipdata.clip_high_cr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_clip_h.u32) + data * CHN_OFFSET), dhd0_clip_h.u32);
}

hi_void vdp_disp_setintfclkmux(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 clkmux)
{
    u_dhd0_clk_dv_ctrl dhd0_clk_dv_ctrl;

    dhd0_clk_dv_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_clk_dv_ctrl.u32) + data * CHN_OFFSET));
    dhd0_clk_dv_ctrl.bits.intf_clk_mux = clkmux;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_clk_dv_ctrl.u32) + data * CHN_OFFSET), dhd0_clk_dv_ctrl.u32);
}

hi_void vdp_disp_setintfdvmux(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 dvmux)
{
    u_dhd0_clk_dv_ctrl dhd0_clk_dv_ctrl;

    dhd0_clk_dv_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_clk_dv_ctrl.u32) + data * CHN_OFFSET));
    dhd0_clk_dv_ctrl.bits.intf_dv_mux = dvmux;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_clk_dv_ctrl.u32) + data * CHN_OFFSET), dhd0_clk_dv_ctrl.u32);
}

hi_void vdp_disp_setintfnoactivepos(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 intfnoactivepos)
{
    u_dhd0_clk_dv_ctrl dhd0_clk_dv_ctrl;

    dhd0_clk_dv_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_clk_dv_ctrl.u32) + data * CHN_OFFSET));
    dhd0_clk_dv_ctrl.bits.no_active_area_pos = intfnoactivepos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_clk_dv_ctrl.u32) + data * CHN_OFFSET), dhd0_clk_dv_ctrl.u32);
}

hi_void vdp_disp_setintfrgbfixmux(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 rgbfixmux)
{
    u_dhd0_rgb_fix_ctrl dhd0_rgb_fix_ctrl;

    dhd0_rgb_fix_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_rgb_fix_ctrl.u32) + data * CHN_OFFSET));
    dhd0_rgb_fix_ctrl.bits.rgb_fix_mux = rgbfixmux;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_rgb_fix_ctrl.u32) + data * CHN_OFFSET), dhd0_rgb_fix_ctrl.u32);
}

hi_void vdp_disp_setintffixr(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 fixr)
{
    u_dhd0_rgb_fix_ctrl dhd0_rgb_fix_ctrl;

    dhd0_rgb_fix_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_rgb_fix_ctrl.u32) + data * CHN_OFFSET));
    dhd0_rgb_fix_ctrl.bits.fix_r = fixr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_rgb_fix_ctrl.u32) + data * CHN_OFFSET), dhd0_rgb_fix_ctrl.u32);
}

hi_void vdp_disp_setintffixg(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 fixg)
{
    u_dhd0_rgb_fix_ctrl dhd0_rgb_fix_ctrl;

    dhd0_rgb_fix_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_rgb_fix_ctrl.u32) + data * CHN_OFFSET));
    dhd0_rgb_fix_ctrl.bits.fix_g = fixg;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_rgb_fix_ctrl.u32) + data * CHN_OFFSET), dhd0_rgb_fix_ctrl.u32);
}

hi_void vdp_disp_setintffixb(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 fixb)
{
    u_dhd0_rgb_fix_ctrl dhd0_rgb_fix_ctrl;

    dhd0_rgb_fix_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_rgb_fix_ctrl.u32) + data * CHN_OFFSET));
    dhd0_rgb_fix_ctrl.bits.fix_b = fixb;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_rgb_fix_ctrl.u32) + data * CHN_OFFSET), dhd0_rgb_fix_ctrl.u32);
}

hi_void vdp_disp_setintfvimeasureen(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 vimeasureen)
{
    u_dhd0_lockcfg dhd0_lockcfg;

    dhd0_lockcfg.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_lockcfg.u32) + data * CHN_OFFSET));
    dhd0_lockcfg.bits.vi_measure_en = vimeasureen;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_lockcfg.u32) + data * CHN_OFFSET), dhd0_lockcfg.u32);
}

hi_void vdp_disp_setintflockcnten(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 lockcnten)
{
    u_dhd0_lockcfg dhd0_lockcfg;

    dhd0_lockcfg.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_lockcfg.u32) + data * CHN_OFFSET));
    dhd0_lockcfg.bits.lock_cnt_en = lockcnten;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_lockcfg.u32) + data * CHN_OFFSET), dhd0_lockcfg.u32);
}

hi_void vdp_disp_setintfvdpmeasureen(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 vdpmeasureen)
{
    u_dhd0_lockcfg dhd0_lockcfg;

    dhd0_lockcfg.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_lockcfg.u32) + data * CHN_OFFSET));
    dhd0_lockcfg.bits.vdp_measure_en = vdpmeasureen;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_lockcfg.u32) + data * CHN_OFFSET), dhd0_lockcfg.u32);
}

hi_void vdp_disp_setvbienable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 vbienable)
{
    u_dhd0_vbi dhd0_vbi;

    dhd0_vbi.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_vbi.u32) + data * CHN_OFFSET));
    dhd0_vbi.bits.vbi_en = vbienable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_vbi.u32) + data * CHN_OFFSET), dhd0_vbi.u32);
}

hi_void vdp_disp_setvbidata(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 vbiaddr, hi_u32 vbidata)
{
    u_dhd0_vbi dhd0_vbi;

    dhd0_vbi.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_vbi.u32) + data * CHN_OFFSET));
    dhd0_vbi.bits.vbi_addr = vbiaddr;
    dhd0_vbi.bits.vbi_data = vbidata;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_vbi.u32) + data * CHN_OFFSET), dhd0_vbi.u32);
}

hi_void vdp_disp_setdneedmode(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 mode)
{
    u_dhd0_precharge_thd1 dhd0_precharge_thd1;

    dhd0_precharge_thd1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_precharge_thd1.u32) + data * CHN_OFFSET));
    dhd0_precharge_thd1.bits.dneed_en_mode = mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_precharge_thd1.u32) + data * CHN_OFFSET), dhd0_precharge_thd1.u32);
}

hi_void vdp_disp_setintfvsyncblsel(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 intfvsyncblsel)
{
    u_dhd0_scan_bl dhd0_scan_bl;

    dhd0_scan_bl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_scan_bl.u32) + data * CHN_OFFSET));
    dhd0_scan_bl.bits.intf_vsync_bl_sel = intfvsyncblsel;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_scan_bl.u32) + data * CHN_OFFSET), dhd0_scan_bl.u32);
}

hi_void vdp_disp_setintfvsyncblinv(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 intfvsyncblinv)
{
    u_dhd0_scan_bl dhd0_scan_bl;

    dhd0_scan_bl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_scan_bl.u32) + data * CHN_OFFSET));
    dhd0_scan_bl.bits.intf_vsync_bl_inv = intfvsyncblinv;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_scan_bl.u32) + data * CHN_OFFSET), dhd0_scan_bl.u32);
}

hi_void vdp_disp_setintfllrrmode(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 llrmode)
{
    u_dhd0_scan_bl dhd0_scan_bl;

    dhd0_scan_bl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_scan_bl.u32) + data * CHN_OFFSET));
    dhd0_scan_bl.bits.llrr_mode = llrmode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_scan_bl.u32) + data * CHN_OFFSET), dhd0_scan_bl.u32);
}

hi_void vdp_disp_setintfpausemode(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 pausemode)
{
    u_dhd0_pause dhd0_pause;

    dhd0_pause.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_pause.u32) + data * CHN_OFFSET));
    dhd0_pause.bits.pause_mode = pausemode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_pause.u32) + data * CHN_OFFSET), dhd0_pause.u32);
}

hi_void vdp_disp_setintfafifopreufthd(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 afifopreufthd)
{
    u_dhd0_pre_uf_thd dhd0_pre_uf_thd;

    dhd0_pre_uf_thd.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_pre_uf_thd.u32) + data * CHN_OFFSET));
    dhd0_pre_uf_thd.bits.afifo_pre_uf_thd = afifopreufthd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_pre_uf_thd.u32) + data * CHN_OFFSET), dhd0_pre_uf_thd.u32);
}

hi_void vdp_disp_setintfvdacdethigh(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 vdacdethigh)
{
    u_dhd0_vdac_det0 dhd0_vdac_det0;

    dhd0_vdac_det0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_vdac_det0.u32) + data * CHN_OFFSET));
    dhd0_vdac_det0.bits.vdac_det_high = vdacdethigh;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_vdac_det0.u32) + data * CHN_OFFSET), dhd0_vdac_det0.u32);
}

hi_void vdp_disp_setintfdetline(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 detline)
{
    u_dhd0_vdac_det0 dhd0_vdac_det0;

    dhd0_vdac_det0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_vdac_det0.u32) + data * CHN_OFFSET));
    dhd0_vdac_det0.bits.det_line = detline;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_vdac_det0.u32) + data * CHN_OFFSET), dhd0_vdac_det0.u32);
}

hi_void vdp_disp_setintfdetpixelstat(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 detpixelstat)
{
    u_dhd0_vdac_det1 dhd0_vdac_det1;

    dhd0_vdac_det1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_vdac_det1.u32) + data * CHN_OFFSET));
    dhd0_vdac_det1.bits.det_pixel_stat = detpixelstat;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_vdac_det1.u32) + data * CHN_OFFSET), dhd0_vdac_det1.u32);
}

hi_void vdp_disp_setintfdetpixelwid(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 detpixelwid)
{
    u_dhd0_vdac_det1 dhd0_vdac_det1;

    dhd0_vdac_det1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_vdac_det1.u32) + data * CHN_OFFSET));
    dhd0_vdac_det1.bits.det_pixel_wid = detpixelwid;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_vdac_det1.u32) + data * CHN_OFFSET), dhd0_vdac_det1.u32);
}

hi_void vdp_disp_setintfvdacdeten(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 vdacdeten)
{
    u_dhd0_vdac_det1 dhd0_vdac_det1;

    dhd0_vdac_det1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_vdac_det1.u32) + data * CHN_OFFSET));
    dhd0_vdac_det1.bits.vdac_det_en = vdacdeten;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_vdac_det1.u32) + data * CHN_OFFSET), dhd0_vdac_det1.u32);
}

hi_void vdp_disp_settwodiv4pen(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 twodiv4pen)
{
    u_dhd0_ctrl1 dhd0_ctrl1;

    dhd0_ctrl1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_ctrl1.u32) + chn_id * CHN_OFFSET));
    dhd0_ctrl1.bits.twodiv_4p_en = twodiv4pen;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dhd0_ctrl1.u32) + chn_id * CHN_OFFSET), dhd0_ctrl1.u32);
}

hi_void vdp_disp_get_state(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_bool *btm, hi_u32 *vcnt, hi_u32 *int_cnt)
{
    u_dhd0_state dhd0_state;
    dhd0_state.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dhd0_state.u32) + chn_id * CHN_OFFSET));
    if (btm != HI_NULL) {
        *btm = dhd0_state.bits.bottom_field;
    }
    if (vcnt != HI_NULL) {
        *vcnt = dhd0_state.bits.count_vcnt;
    }
    if (int_cnt != HI_NULL) {
        *int_cnt = dhd0_state.bits.count_int;
    }

    return;
}

hi_void vdp_disp_get_gfx_state(vdp_regs_type *vdp_reg, hi_u32 *total_pixel, hi_u32 *zero_pixel)
{
    // :TODO: there is no g0 fmt must check
    return;
}
