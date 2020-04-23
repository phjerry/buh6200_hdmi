/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_ip_dispchn.h"

#include "hal_vdp_reg_chn.h"
#include "hal_vdp_comm.h"
#include "hal_vdp_reg_disp.h"

#define VDP_MIX_PRIO_0  0
#define VDP_MIX_PRIO_1  1
#define VDP_MIX_PRIO_2  2

#define VDP_DEFAULT_LOAD_POS 1
#define VDP_START_POS_DIV_2 2

hi_void vdp_ip_dispchn_dhd0_mask_enalbe(hi_bool enable)
{
    if (enable == HI_FALSE) {
        vdp_dispchn_set_cbm_mix(g_vdp_reg, VDP_DISPCHN_CBM_MIX1, VDP_DISPCHN_PRIO_CBM1_VID3, 0);
        vdp_dispchn_set_cbm_mix(g_vdp_reg, VDP_DISPCHN_CBM_MIX1, VDP_DISPCHN_PRIO_CBM1_GFX3, 1);
    } else {
        vdp_dispchn_set_cbm_mix(g_vdp_reg, VDP_DISPCHN_CBM_MIX1, 0, 0);
        vdp_dispchn_set_cbm_mix(g_vdp_reg, VDP_DISPCHN_CBM_MIX1, 0, 1);
    }
}

hi_void vdp_ip_dispchn_date_coef(vdp_disp_intf intf, hi_drv_disp_fmt disp_fmt)
{
    hi_u32 value = 0;

    switch (disp_fmt) {
        case HI_DRV_DISP_FMT_PAL:
        case HI_DRV_DISP_FMT_PAL_B:
        case HI_DRV_DISP_FMT_PAL_B1:
        case HI_DRV_DISP_FMT_PAL_D:
        case HI_DRV_DISP_FMT_PAL_D1:
        case HI_DRV_DISP_FMT_PAL_G:
        case HI_DRV_DISP_FMT_PAL_H:
        case HI_DRV_DISP_FMT_PAL_K:
        case HI_DRV_DISP_FMT_PAL_I:
            value = 0x6a8452dc;
            break;
        case HI_DRV_DISP_FMT_PAL_N:
            value = 0x6a8852dc;
            break;
        case HI_DRV_DISP_FMT_PAL_NC:
            value = 0x6a9052dc;
            break;
        case HI_DRV_DISP_FMT_SECAM_SIN:
            value = 0x6aa052dc;
            break;
        case HI_DRV_DISP_FMT_SECAM_COS:
            value = 0x6ae052dc;
            break;
        case HI_DRV_DISP_FMT_PAL_M:
            value = 0x449050dc;
            break;
        case HI_DRV_DISP_FMT_PAL_60:
            value = 0x44a050dc;
            break;
        case HI_DRV_DISP_FMT_NTSC:
            value = 0x048450dc;
            break;
        case HI_DRV_DISP_FMT_NTSC_J:
            value = 0x248450dc;
            break;
        case HI_DRV_DISP_FMT_NTSC_443:
            value = 0x04a450dc;
            break;
        default:
            value = 0x628412dc;
            break;
    }

    vdp_disp_setdatecoeff0(g_vdp_reg, value);
    vdp_disp_setdatecoeff21(g_vdp_reg, intf);
}

hi_void vdp_ip_dispchn_sync_inv(vdp_disp_intf intf, vdp_disp_syncinv *syncinv)
{
    if (intf == VDP_DISP_INTF_CVBS) {
        vdp_disp_setsyncinv(g_vdp_reg, 6, syncinv); /* 6 is cvbs offset */
    }
}

hi_void vdp_ip_dispchn_attach_intf(vdp_dispchn_chn chn_id, vdp_disp_intf intf)
{
    vdp_disp_setintfmuxsel(g_vdp_reg, chn_id, intf);
}
hi_void vdp_ip_dispchn_set_cbm_mix(hi_u32 mixv_layer, hi_u32 vid_layer, hi_u32 pro)
{
    vdp_dispchn_set_cbm_mix(g_vdp_reg, mixv_layer, vid_layer, pro);
}

hi_void vdp_ip_dispchn_set_intf(vdp_dispchn_chn chn, vdp_disp_intf intf)
{
    if (intf == VDP_DISP_INTF_CVBS) {
        vdp_disp_setintfmuxdac(g_vdp_reg, VDP_DISP_INTF_CVBS);
        vdp_disp_setdacenable(g_vdp_reg, HI_TRUE);
        vdp_disp_setdac0ctrl(g_vdp_reg, HI_TRUE, 0x3f);
        vdp_disp_setdac1ctrl(g_vdp_reg, HI_TRUE, 0x3f);
        vdp_disp_setdac2ctrl(g_vdp_reg, HI_TRUE, 0x3f);
        vdp_disp_setdac3ctrl(g_vdp_reg, HI_TRUE, 0x3f);
    } else if (intf == VDP_DISP_INTF_HDMI) {
        vdp_disp_setintfmuxdac(g_vdp_reg, VDP_DISP_INTF_HDMI);
        vdp_disp_setdacenable(g_vdp_reg, HI_FALSE);
        vdp_disp_setdac0ctrl(g_vdp_reg, HI_FALSE, 0);
        vdp_disp_setdac1ctrl(g_vdp_reg, HI_FALSE, 0);
        vdp_disp_setdac2ctrl(g_vdp_reg, HI_FALSE, 0);
        vdp_disp_setdac3ctrl(g_vdp_reg, HI_FALSE, 0);
    }
}

hi_void vdp_ip_dispchn_init(hi_void)
{
    if (vdp_ip_dispchn_get_enable(VDP_DISPCHN_CHN_DHD0) == HI_TRUE) {
        return;
    }

    vdp_disp_setchksumen(g_vdp_reg, HI_TRUE);

    /* mux */
    vdp_disp_setintfmuxsel(g_vdp_reg, VDP_DISPCHN_CHN_DHD0, VDP_DISP_INTF_HDMI1); /* default dhd0 to 2.0 */
    vdp_disp_setintfmuxsel(g_vdp_reg, VDP_DISPCHN_CHN_DHD1, VDP_DISP_INTF_HDMI); /* default dhd0 to 2.1 */

    /* cbm - mix0 */
    vdp_dispchn_reset_cbm_mix(g_vdp_reg, VDP_DISPCHN_CBM_MIX0);
    vdp_dispchn_set_cbm_mix(g_vdp_reg, VDP_DISPCHN_CBM_MIX0, VDP_DISPCHN_PRIO_CBM0_VP0, VDP_MIX_PRIO_0);
    vdp_dispchn_set_cbm_mix(g_vdp_reg, VDP_DISPCHN_CBM_MIX0, VDP_DISPCHN_PRIO_CBM0_GP0, VDP_MIX_PRIO_1);

    /* cbm - mixv0 */
    vdp_dispchn_reset_cbm_mix(g_vdp_reg, VDP_DISPCHN_CBM_MIXV0);
    vdp_dispchn_set_cbm_mix(g_vdp_reg, VDP_DISPCHN_CBM_MIXV0, VDP_DISPCHN_MIX0_VID0, VDP_MIX_PRIO_0);
    vdp_dispchn_set_cbm_mix(g_vdp_reg, VDP_DISPCHN_CBM_MIXV0, VDP_DISPCHN_MIX0_VID1, VDP_MIX_PRIO_1);

    /* cbm - mixg0 */
    vdp_dispchn_reset_cbm_mix(g_vdp_reg, VDP_DISPCHN_CBM_MIXG0);
    vdp_dispchn_set_cbm_mix(g_vdp_reg, VDP_DISPCHN_CBM_MIXG0, VDP_DISPCHN_MIX0_GFX0, VDP_MIX_PRIO_0);
    vdp_dispchn_set_cbm_mix(g_vdp_reg, VDP_DISPCHN_CBM_MIXG0, VDP_DISPCHN_MIX0_GFX1, VDP_MIX_PRIO_1);
    vdp_dispchn_set_cbm_mix(g_vdp_reg, VDP_DISPCHN_CBM_MIXG0, VDP_DISPCHN_MIX0_GFX2, VDP_MIX_PRIO_2);

    /* cbm - mix1 */
    vdp_dispchn_reset_cbm_mix(g_vdp_reg, VDP_DISPCHN_CBM_MIX1);
    vdp_dispchn_set_cbm_mix(g_vdp_reg, VDP_DISPCHN_CBM_MIX1, VDP_DISPCHN_PRIO_CBM1_VID3, VDP_MIX_PRIO_0);
    vdp_dispchn_set_cbm_mix(g_vdp_reg, VDP_DISPCHN_CBM_MIX1, VDP_DISPCHN_PRIO_CBM1_GFX3, VDP_MIX_PRIO_1);

    vdp_dsp_setchn0mux1(g_vdp_reg, 1);
    vdp_dsp_setchn0mux2(g_vdp_reg, 1);

    return;
}

static hi_u32 vdp_dispchn_get_startpos(vdp_dispchn_chn chn_id, vdp_dispchn_info *dispchn_info)
{
    hi_u32 start_pos;

    if (dispchn_info->disp_fmt >= HI_DRV_DISP_FMT_7680X4320_100 &&
        dispchn_info->disp_fmt <= HI_DRV_DISP_FMT_7680X4320_120) {
        start_pos = 60; /* 60 is reg value */
    } else if (dispchn_info->disp_fmt >= HI_DRV_DISP_FMT_7680X4320_23_976 &&
        dispchn_info->disp_fmt <= HI_DRV_DISP_FMT_7680X4320_60) {
        start_pos = 30; /* 30 is reg value */
    } else if ((dispchn_info->disp_fmt >= HI_DRV_DISP_FMT_4096X2160_100 &&
            dispchn_info->disp_fmt <= HI_DRV_DISP_FMT_4096X2160_120) ||
        (dispchn_info->disp_fmt >= HI_DRV_DISP_FMT_3840X2160_100 &&
            dispchn_info->disp_fmt <= HI_DRV_DISP_FMT_3840X2160_120)) {
        start_pos = 30; /* 30 is reg value */
    } else if ((dispchn_info->disp_fmt >= HI_DRV_DISP_FMT_3840X2160_23_976 &&
            dispchn_info->disp_fmt <= HI_DRV_DISP_FMT_3840X2160_60) ||
        (dispchn_info->disp_fmt >= HI_DRV_DISP_FMT_4096X2160_24 &&
            dispchn_info->disp_fmt <= HI_DRV_DISP_FMT_4096X2160_60)) {
        start_pos = 30; /* 30 is reg value */
    } else if (dispchn_info->disp_fmt >= HI_DRV_DISP_FMT_1080I_50 &&
        dispchn_info->disp_fmt <= HI_DRV_DISP_FMT_1080P_120) {
        start_pos = 11; /* 11 is reg value */
    } else if (dispchn_info->disp_fmt >= HI_DRV_DISP_FMT_720P_50 &&
        dispchn_info->disp_fmt <= HI_DRV_DISP_FMT_720P_60) {
        start_pos = 10; /* 10 is reg value */
    } else {
        start_pos = (dispchn_info->timing.vbb + dispchn_info->timing.vfb) / VDP_START_POS_DIV_2;
    }

    return start_pos;
}

hi_void vdp_ip_dispchn_set_timming(vdp_dispchn_chn chn_id, vdp_dispchn_info *dispchn_info)
{
    hi_u32 start_pos;

    vdp_disp_setdispmode(g_vdp_reg, chn_id, dispchn_info->disp_mode);

    vdp_disp_openintf(g_vdp_reg, chn_id, dispchn_info->timing);

    vdp_disp_setsplitmode(g_vdp_reg, chn_id, dispchn_info->split_mode);

    start_pos = vdp_dispchn_get_startpos(chn_id, dispchn_info);
    vdp_disp_setparaloadpos(g_vdp_reg, chn_id, VDP_DEFAULT_LOAD_POS);
    vdp_disp_setstartpos(g_vdp_reg, chn_id, start_pos);

    return;
}

hi_void vdp_ip_dispchn_set_vtthd(vdp_dispchn_chn chn_id, hi_u32 num, hi_u32 thd, hi_u32 mode)
{
    vdp_disp_setvtthd(g_vdp_reg, chn_id, num, thd);
    vdp_disp_setvtthdmode(g_vdp_reg, chn_id, num, mode);

    return;
}

hi_u32 vdp_ip_dispchn_get_int_state(hi_void)
{
    return vdp_dispchn_get_voint(g_vdp_reg);
}

hi_u32 vdp_ip_dispchn_get_int_mask_state(hi_void)
{
    return vdp_dispchn_get_voint_masked(g_vdp_reg);
}

hi_void vdp_ip_dispchn_set_mask(vdp_dispchn_voint_status status, vdp_intmask mask)
{
    if (status == VDP_DISPCHN_INT_ON) {
        vdp_disp_setintmask(g_vdp_reg, mask);
    } else if (status == VDP_DISPCHN_INT_OFF) {
        vdp_disp_setintdisable(g_vdp_reg, mask);
    } else if (status == VDP_DISPCHN_INT_CLEAN) {
        vdp_dispchn_clean_voint(g_vdp_reg, mask);
    }

    return;
}

hi_void vdp_ip_dispchn_set_colobar(vdp_dispchn_chn chn_id, hi_bool enable)
{
    vdp_disp_setcbarenable(g_vdp_reg, chn_id, enable);
}

hi_void vdp_ip_dispchn_set_enable(vdp_dispchn_chn chn_id, hi_bool enable)
{
    vdp_disp_setintfenable(g_vdp_reg, chn_id, enable);
    vdp_disp_setregup(g_vdp_reg, chn_id);
}

hi_bool vdp_ip_dispchn_get_enable(vdp_dispchn_chn chn_id)
{
    return vdp_disp_getintfenable(g_vdp_reg, chn_id);
}

hi_void vdp_ip_dispchn_set_cbm_bkg(vdp_dispchn_cbm_mix chn_id, vdp_bkg *vdp_bkg)
{
    vdp_dispchn_set_cbm_bkg(chn_id, vdp_bkg);

    return ;
}

hi_void vdp_ip_dispchn_get_state(vdp_dispchn_chn chn_id, hi_bool *btm, hi_u32 *vcnt, hi_u32 *int_cnt)
{
    vdp_disp_get_state(g_vdp_reg, chn_id, btm, vcnt, int_cnt);
}

hi_void vdp_ip_dispchn_get_gfx_state(hi_u32 *total_pixel, hi_u32 *zero_pixel)
{
    vdp_disp_get_gfx_state(g_vdp_reg, total_pixel, zero_pixel);
}


hi_void vdp_ip_dispchn_regup(vdp_dispchn_chn chn)
{
    vdp_disp_setregup(g_vdp_reg, chn);
}

hi_void vdp_ip_dispchn_get_checksum(vdp_dispchn_chn chn, hi_u32 *r, hi_u32 *g, hi_u32 *b)
{
    *r = vdp_regread((uintptr_t)(&(g_vdp_reg->dhd0_intf_chksum_y.u32) + chn * CHN_OFFSET));
    *g = vdp_regread((uintptr_t)(&(g_vdp_reg->dhd0_intf_chksum_u.u32) + chn * CHN_OFFSET));
    *b = vdp_regread((uintptr_t)(&(g_vdp_reg->dhd0_intf_chksum_v.u32) + chn * CHN_OFFSET));
}
