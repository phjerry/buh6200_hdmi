/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: vpss hal
 * Author: zhangjunyu
 * Create: 2016/07/01
 */

#include "vpss_comm.h"
#include "hal_vpss_adapter.h"
#include "hal_vpss_ip_hzme.h"
#include "hal_vpss_ip_vzme.h"
#include "hal_vpss.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VPSS_REG_VC1_CTRL_ADDR    0x7200
#define VPSS_REG_VC1_ME_CTRL_ADDR 0xec00

hi_bool g_gb_tunl_en = HI_TRUE;
xdp_data_rmode rmode_adp(xdp_data_rmode rd_mode)
{
    xdp_data_rmode rd_mode_adp = XDP_RMODE_PROGRESSIVE;

    switch (rd_mode) {
        case XDP_RMODE_PROGRESSIVE:
            rd_mode_adp = XDP_RMODE_PROGRESSIVE;
            break;
        case XDP_RMODE_TOP:
            rd_mode_adp = XDP_RMODE_PRO_TOP;
            break;
        case XDP_RMODE_BOTTOM:
            rd_mode_adp = XDP_RMODE_PRO_BOTTOM;
            break;
        case XDP_RMODE_PRO_TOP:
            rd_mode_adp = XDP_RMODE_PRO_TOP;
            break;
        case XDP_RMODE_PRO_BOTTOM:
            rd_mode_adp = XDP_RMODE_PRO_BOTTOM;
            break;
        default:
            rd_mode_adp = XDP_RMODE_MAX;
            break;
    }

    return rd_mode_adp;
}

hi_s32 set_me_vc1_reg(vpss_reg_type *vpss_reg, hi_drv_vc1_range_info *vc1_info)
{
    hi_u32 addr_offset;
    addr_offset = (VPSS_REG_VC1_ME_CTRL_ADDR - VPSS_REG_VC1_CTRL_ADDR) / 4; /* 1,1,4:para */
    vpss_vc1_set_vc1_mapc(vpss_reg, addr_offset, vc1_info->range_mapuv);
    vpss_vc1_set_vc1_mapy(vpss_reg, addr_offset, vc1_info->range_mapy);
    vpss_vc1_set_vc1_mapcflg(vpss_reg, addr_offset, vc1_info->range_mapuv_flag);
    vpss_vc1_set_vc1_mapyflg(vpss_reg, addr_offset, vc1_info->range_mapy_flag);
    vpss_vc1_set_vc1_rangedfrm(vpss_reg, addr_offset, vc1_info->ranged_frm);
    vpss_vc1_set_vc1_profile(vpss_reg, addr_offset, vc1_info->vc1_profile);
    return HI_SUCCESS;
}

hi_void me_cfchn_set(vpss_reg_type *vpss_regs, vpss_mc_info *mc_info,
                     vpss_me_cfg *me_cfg, hi_u32 scan_cnt,
                     vpss_mac_rchn_cfg *rchn_cfg, vpss_mac_wchn_cfg *wchn_cfg)
{
    hi_u32 big_wth;
    hi_u32 big_hgt;
    hi_u32 sml_wth;
    hi_u32 sml_hgt;
    vpss_hal_wbc_cfg *wbc_cfg;
    xdp_data_wth data_width[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 ystr[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 cstr[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u64 yaddr[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u64 caddr[XDP_MAX_SCAN_NUM] = { 0 };
    xdp_data_rmode rd_mode[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 x[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 y[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 wth[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 hgt[XDP_MAX_SCAN_NUM] = { 0 };
    hi_bool mirror[XDP_MAX_SCAN_NUM] = { HI_FALSE, HI_FALSE, HI_TRUE, HI_FALSE, HI_TRUE };
    hi_bool flip[XDP_MAX_SCAN_NUM] = { HI_FALSE, HI_FALSE, HI_TRUE, HI_FALSE, HI_TRUE };
    hi_bool tunl_en[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 i = 0;
    wbc_cfg = mc_info->wbc_cfg;
    big_wth = (me_cfg->meds_en == HI_FALSE) ? rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth :
              (rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth / 2 + 1) / 2 * 2; /* 2,1,2,2:para */
    big_hgt = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.hgt;
    sml_wth = (big_wth / 2 + 1) / 2 * 2; /* 2,1,2,2:para */
    sml_hgt = big_hgt / 2; /* 2:para */

    if (rchn_cfg[VPSS_MAC_RCHN_CF].flip_en == HI_TRUE) {
        flip[0] = HI_TRUE;
    }

    rd_mode[0] = rchn_cfg[VPSS_MAC_RCHN_CF].rd_mode;
    x[0] = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.x;
    y[0] = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.y;
    wth[0] = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth;
    hgt[0] = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.hgt;
    data_width[0] = rchn_cfg[VPSS_MAC_RCHN_CF].data_width;
    ystr[0] = rchn_cfg[VPSS_MAC_RCHN_CF].addr[VPSS_RCHN_ADDR_DATA].str_y;
    cstr[0] = rchn_cfg[VPSS_MAC_RCHN_CF].addr[VPSS_RCHN_ADDR_DATA].str_c;
    yaddr[0] = rchn_cfg[VPSS_MAC_RCHN_CF].addr[VPSS_RCHN_ADDR_DATA].addr_y;
    caddr[0] = rchn_cfg[VPSS_MAC_RCHN_CF].addr[VPSS_RCHN_ADDR_DATA].addr_u;

    for (i = 1; i < 2 + 1; i++) { /* 1,2,1:para */
        tunl_en[i] = g_gb_tunl_en;
        rd_mode[i] = rmode_adp(rchn_cfg[VPSS_MAC_RCHN_CF].rd_mode);
        wth[i] = sml_wth;
        hgt[i] = sml_hgt;
        data_width[i] = XDP_DATA_WTH_8;
        ystr[i] = wchn_cfg[VPSS_MAC_WCHN_NR_RFRHV].addr[VPSS_WCHN_ADDR_DATA].str_y;
        cstr[i] = wchn_cfg[VPSS_MAC_WCHN_NR_RFRHV].addr[VPSS_WCHN_ADDR_DATA].str_c;
        yaddr[i] = wbc_cfg->wbc_hvds_cfg.w_frame.hal_addr.phy_addr_y;
        caddr[i] = wbc_cfg->wbc_hvds_cfg.w_frame.hal_addr.phy_addr_c;
    }

    for (i = 3; i < 4 + 1; i++) { /* 3,4,1:para */
        tunl_en[i] = g_gb_tunl_en;
        rd_mode[i] = rmode_adp(rchn_cfg[VPSS_MAC_RCHN_CF].rd_mode);
        wth[i] = big_wth;
        hgt[i] = big_hgt;
        data_width[i] = XDP_DATA_WTH_8;
        ystr[i] = wchn_cfg[VPSS_MAC_WCHN_NR_RFRH].addr[VPSS_WCHN_ADDR_DATA].str_y;
        cstr[i] = wchn_cfg[VPSS_MAC_WCHN_NR_RFRH].addr[VPSS_WCHN_ADDR_DATA].str_c;
        yaddr[i] = wbc_cfg->wbc_hds_cfg.w_frame.hal_addr.phy_addr_y;
        caddr[i] = wbc_cfg->wbc_hds_cfg.w_frame.hal_addr.phy_addr_c;
    }

    if (me_cfg->me_en == 1) {
        // me cf
        rchn_cfg[VPSS_MAC_RCHN_ME_CF].en = HI_TRUE;
        rchn_cfg[VPSS_MAC_RCHN_ME_CF].tunl_en = tunl_en[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_CF].data_type = XDP_DATA_TYPE_SP_LINEAR;
        rchn_cfg[VPSS_MAC_RCHN_ME_CF].rd_mode = rd_mode[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_CF].data_fmt = XDP_PROC_FMT_SP_400;
        rchn_cfg[VPSS_MAC_RCHN_ME_CF].data_width = data_width[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_CF].in_rect.x = x[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_CF].in_rect.y = y[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_CF].in_rect.wth = wth[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_CF].in_rect.hgt = hgt[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_CF].mirror_en = mirror[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_CF].flip_en = flip[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_CF].addr[VPSS_RCHN_ADDR_DATA].str_y = ystr[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_CF].addr[VPSS_RCHN_ADDR_DATA].str_c = cstr[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_CF].addr[VPSS_RCHN_ADDR_DATA].addr_y = yaddr[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_CF].addr[VPSS_RCHN_ADDR_DATA].addr_u = caddr[scan_cnt];
        vpss_mac_set_rchn_cfg(vpss_regs, VPSS_MAC_RCHN_ME_CF, &rchn_cfg[VPSS_MAC_RCHN_ME_CF]);
    }
}

hi_void me_refchn_set(vpss_reg_type *vpss_regs, vpss_mc_info *mc_info,
                      vpss_me_cfg *me_cfg, hi_u32 scan_cnt,
                      vpss_mac_rchn_cfg *rchn_cfg, vpss_mac_wchn_cfg *wchn_cfg)
{
    hi_u32 i = 0;
    vpss_hal_wbc_cfg *wbc_cfg;
    xdp_data_wth data_width[XDP_MAX_SCAN_NUM];
    hi_u32 x[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 y[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 wth[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 hgt[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 ystr[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 cstr[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u64 yaddr[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u64 caddr[XDP_MAX_SCAN_NUM] = { 0 };
    xdp_data_rmode rd_mode[XDP_MAX_SCAN_NUM] = { 0 };
    hi_bool mirror[XDP_MAX_SCAN_NUM] = { HI_FALSE, HI_FALSE, HI_TRUE, HI_FALSE, HI_TRUE };
    hi_bool flip[XDP_MAX_SCAN_NUM] = { HI_FALSE, HI_FALSE, HI_TRUE, HI_FALSE, HI_TRUE };
    hi_bool tunl_en[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 big_wth;
    hi_u32 big_hgt;
    hi_u32 sml_wth;
    hi_u32 sml_hgt;
    wbc_cfg = mc_info->wbc_cfg;
    big_wth = (me_cfg->meds_en == HI_FALSE) ? rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth :
              (rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth / 2 + 1) / 2 * 2; /* 2,1,2,2:para */
    big_hgt = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.hgt;
    sml_wth = (big_wth / 2 + 1) / 2 * 2; /* 2,1,2,2:para */
    sml_hgt = big_hgt / 2; /* 2:para */
    rd_mode[0] = rmode_adp(rchn_cfg[VPSS_MAC_RCHN_CF].rd_mode);
    wth[0] = (me_cfg->me_version == XDP_ME_VER5) ? rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth : big_wth;
    hgt[0] = (me_cfg->me_version == XDP_ME_VER5) ? rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.hgt : big_hgt;
    data_width[0] = (me_cfg->me_version == XDP_ME_VER5) ? wchn_cfg[VPSS_MAC_WCHN_NR_RFR].data_width : XDP_DATA_WTH_8;
    ystr[0] = (me_cfg->me_version == XDP_ME_VER5) ? wchn_cfg[VPSS_MAC_WCHN_NR_RFR].addr[VPSS_WCHN_ADDR_DATA].str_y :
              wchn_cfg[VPSS_MAC_WCHN_NR_RFRH].addr[VPSS_WCHN_ADDR_DATA].str_y;
    cstr[0] = (me_cfg->me_version == XDP_ME_VER5) ? wchn_cfg[VPSS_MAC_WCHN_NR_RFR].addr[VPSS_WCHN_ADDR_DATA].str_c :
              wchn_cfg[VPSS_MAC_WCHN_NR_RFRH].addr[VPSS_WCHN_ADDR_DATA].str_c;

    if (mc_info->pro == 0) {
        yaddr[0] = (me_cfg->me_version == XDP_ME_VER5) ? wbc_cfg->wbc_nr_cfg.rp2_frame.hal_addr.phy_addr_y :
                   wbc_cfg->wbc_hds_cfg.rp2_frame.hal_addr.phy_addr_y;
        caddr[0] = (me_cfg->me_version == XDP_ME_VER5) ? wbc_cfg->wbc_nr_cfg.rp2_frame.hal_addr.phy_addr_c :
                   wbc_cfg->wbc_hds_cfg.rp2_frame.hal_addr.phy_addr_c;
    } else {
        yaddr[0] = (me_cfg->me_version == XDP_ME_VER5) ? wbc_cfg->wbc_nr_cfg.rp1_frame.hal_addr.phy_addr_y :
                   wbc_cfg->wbc_hds_cfg.rp2_frame.hal_addr.phy_addr_y;
        caddr[0] = (me_cfg->me_version == XDP_ME_VER5) ? wbc_cfg->wbc_nr_cfg.rp1_frame.hal_addr.phy_addr_c :
                   wbc_cfg->wbc_hds_cfg.rp2_frame.hal_addr.phy_addr_c;
    }

    for (i = 1; i < 2 + 1; i++) { /* 1,2,1:para */
        tunl_en[i] = HI_FALSE;
        rd_mode[i] = rmode_adp(rchn_cfg[VPSS_MAC_RCHN_CF].rd_mode);
        wth[i] = sml_wth;
        hgt[i] = sml_hgt;
        data_width[i] = XDP_DATA_WTH_8;
        ystr[i] = wchn_cfg[VPSS_MAC_WCHN_NR_RFRHV].addr[VPSS_WCHN_ADDR_DATA].str_y;
        cstr[i] = wchn_cfg[VPSS_MAC_WCHN_NR_RFRHV].addr[VPSS_WCHN_ADDR_DATA].str_c;
        yaddr[i] = wbc_cfg->wbc_hvds_cfg.rp2_frame.hal_addr.phy_addr_y;
        caddr[i] = wbc_cfg->wbc_hvds_cfg.rp2_frame.hal_addr.phy_addr_c;
    }

    for (i = 3; i < 4 + 1; i++) { /* 3,4,1:para */
        tunl_en[i] = HI_FALSE;
        rd_mode[i] = rmode_adp(rchn_cfg[VPSS_MAC_RCHN_CF].rd_mode);
        wth[i] = big_wth;
        hgt[i] = big_hgt;
        data_width[i] = XDP_DATA_WTH_8;
        ystr[i] = wchn_cfg[VPSS_MAC_WCHN_NR_RFRH].addr[VPSS_WCHN_ADDR_DATA].str_y;
        cstr[i] = wchn_cfg[VPSS_MAC_WCHN_NR_RFRH].addr[VPSS_WCHN_ADDR_DATA].str_c;
        yaddr[i] = wbc_cfg->wbc_hds_cfg.rp2_frame.hal_addr.phy_addr_y;
        caddr[i] = wbc_cfg->wbc_hds_cfg.rp2_frame.hal_addr.phy_addr_c;
    }

    if (me_cfg->me_en == 1) {
        // me ref
        rchn_cfg[VPSS_MAC_RCHN_ME_REF].en = HI_TRUE;
        rchn_cfg[VPSS_MAC_RCHN_ME_REF].tunl_en = tunl_en[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_REF].data_type = XDP_DATA_TYPE_SP_LINEAR;
        rchn_cfg[VPSS_MAC_RCHN_ME_REF].rd_mode = rd_mode[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_REF].data_fmt = XDP_PROC_FMT_SP_400;
        rchn_cfg[VPSS_MAC_RCHN_ME_REF].data_width = data_width[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_REF].in_rect.x = x[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_REF].in_rect.y = y[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_REF].in_rect.wth = wth[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_REF].in_rect.hgt = hgt[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_REF].mirror_en = mirror[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_REF].flip_en = flip[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_REF].addr[VPSS_RCHN_ADDR_DATA].str_y = ystr[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_REF].addr[VPSS_RCHN_ADDR_DATA].str_c = cstr[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_REF].addr[VPSS_RCHN_ADDR_DATA].addr_y = yaddr[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_REF].addr[VPSS_RCHN_ADDR_DATA].addr_u = caddr[scan_cnt];
        vpss_mac_set_rchn_cfg(vpss_regs, VPSS_MAC_RCHN_ME_REF, &rchn_cfg[VPSS_MAC_RCHN_ME_REF]);
    }
}

hi_void me_p1_mvchn_set(vpss_reg_type *vpss_regs, vpss_mc_info *mc_info,
                        vpss_me_cfg *me_cfg, hi_u32 scan_cnt,
                        vpss_mac_rchn_cfg *rchn_cfg, vpss_mac_wchn_cfg *wchn_cfg)
{
    hi_u32 wth[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 hgt[XDP_MAX_SCAN_NUM] = { 0 };
    hi_bool mirror[XDP_MAX_SCAN_NUM] = { HI_FALSE, HI_FALSE, HI_TRUE, HI_FALSE, HI_TRUE };
    hi_bool flip[XDP_MAX_SCAN_NUM] = { HI_FALSE, HI_FALSE, HI_TRUE, HI_FALSE, HI_TRUE };
    hi_bool tunl_en[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 i = 0;
    hi_u32 ystr[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u64 yaddr[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u64 caddr[XDP_MAX_SCAN_NUM] = { 0 };
    vpss_hal_stt_cfg *stt_cfg;
    hi_u32 big_blkmv_wth;
    hi_u32 big_blkmv_hgt;
    hi_u32 big_wth;
    hi_u32 big_hgt;
    hi_u32 interlace;
    stt_cfg = mc_info->stt_cfg;
    interlace = rchn_cfg[VPSS_MAC_RCHN_CF].rd_mode == XDP_RMODE_PROGRESSIVE ? 0 : 1;
    big_wth = (me_cfg->meds_en == HI_FALSE) ? rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth :
              (rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth / 2 + 1) / 2 * 2; /* 2,1,2,2:para */
    big_hgt = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.hgt;
    big_blkmv_wth = (big_wth + 7) / 8; /* 7,8:para */
    big_blkmv_hgt = ((big_hgt >> interlace) + 3) / 4; /* 3,4:para */

    for (i = 0; i < XDP_MAX_SCAN_NUM; i++) {
        wth[i] = big_blkmv_wth;
        hgt[i] = big_blkmv_hgt;
        ystr[i] = align_anybw_16btye(big_blkmv_wth, 64); /* 16,64:para */
        yaddr[i] = stt_cfg->me_cfg.r_mv_p1_chn.phy_addr;
        caddr[i] = stt_cfg->me_cfg.r_mv_p1_chn.phy_addr;
    }

    if (me_cfg->me_en == 1) {
        rchn_cfg[VPSS_MAC_RCHN_ME_P1MV].en = HI_TRUE;
        rchn_cfg[VPSS_MAC_RCHN_ME_P1MV].tunl_en = tunl_en[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_P1MV].in_rect.x = 0;
        rchn_cfg[VPSS_MAC_RCHN_ME_P1MV].in_rect.y = 0;
        rchn_cfg[VPSS_MAC_RCHN_ME_P1MV].mirror_en = mirror[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_P1MV].flip_en = flip[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_P1MV].in_rect.wth = wth[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_P1MV].in_rect.hgt = hgt[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_P1MV].addr[VPSS_RCHN_ADDR_DATA].str_y = ystr[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_P1MV].addr[VPSS_RCHN_ADDR_DATA].addr_y = yaddr[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_P1MV].addr[VPSS_RCHN_ADDR_DATA].addr_u = caddr[scan_cnt];
        vpss_mac_set_rchn_cfg(vpss_regs, VPSS_MAC_RCHN_ME_P1MV, &rchn_cfg[VPSS_MAC_RCHN_ME_P1MV]);
    }
}

hi_void me_prmvchn_set(vpss_reg_type *vpss_regs, vpss_mc_info *mc_info,
                       vpss_me_cfg *me_cfg, hi_u32 scan_cnt,
                       vpss_mac_rchn_cfg *rchn_cfg, vpss_mac_wchn_cfg *wchn_cfg)
{
    hi_u32 wth[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 hgt[XDP_MAX_SCAN_NUM] = { 0 };
    hi_bool mirror[XDP_MAX_SCAN_NUM] = { HI_FALSE, HI_FALSE, HI_TRUE, HI_FALSE, HI_TRUE };
    hi_bool flip[XDP_MAX_SCAN_NUM] = { HI_FALSE, HI_FALSE, HI_TRUE, HI_FALSE, HI_TRUE };
    hi_bool tunl_en[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 ystr[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u64 yaddr[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u64 caddr[XDP_MAX_SCAN_NUM] = { 0 };
    vpss_hal_stt_cfg *stt_cfg;
    hi_u32 big_blkmv_wth;
    hi_u32 big_blkmv_hgt;
    hi_u32 big_wth;
    hi_u32 big_hgt;
    hi_u32 interlace;
    hi_u32 sml_blkmv_wth;
    hi_u32 sml_blkmv_hgt;
    hi_u32 sml_wth;
    stt_cfg = mc_info->stt_cfg;
    interlace = rchn_cfg[VPSS_MAC_RCHN_CF].rd_mode == XDP_RMODE_PROGRESSIVE ? 0 : 1;
    big_wth = (me_cfg->meds_en == HI_FALSE) ? rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth :
              (rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth / 2 + 1) / 2 * 2; /* 2,1,2,2:para */
    big_hgt = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.hgt;
    sml_wth = (big_wth / 2 + 1) / 2 * 2; /* 2,1,2,2:para */
    big_blkmv_wth = (big_wth + 7) / 8; /* 7,8:para */
    big_blkmv_hgt = ((big_hgt >> interlace) + 3) / 4; /* 3,4:para */
    sml_blkmv_wth = (sml_wth + 7) / 8; /* 7,8:para */
    sml_blkmv_hgt = ((big_hgt >> interlace) / 2 + 3) / 4; /* 2,3,4:para */
    wth[0] = big_blkmv_wth;
    hgt[0] = big_blkmv_hgt;
    ystr[0] = align_anybw_16btye(big_blkmv_wth, 64); /* 0,16,64:para */
    yaddr[0] = stt_cfg->me_cfg.r_mv_p1_chn.phy_addr;
    caddr[0] = stt_cfg->me_cfg.r_mv_p1_chn.phy_addr;
    wth[1] = big_blkmv_wth;
    hgt[1] = big_blkmv_hgt;
    ystr[1] = align_anybw_16btye(big_blkmv_wth, 64); /* 1,16,64:para */
    yaddr[1] = stt_cfg->me_cfg.r_mv_p1_chn.phy_addr;
    caddr[1] = stt_cfg->me_cfg.r_mv_p1_chn.phy_addr;
    wth[2] = sml_blkmv_wth; /* 2:para */
    hgt[2] = sml_blkmv_hgt; /* 2:para */
    ystr[2] = align_anybw_16btye(big_blkmv_wth, 64) * 2; /* 2,16,64,2:para */
    yaddr[2] = stt_cfg->me_cfg.w_mv_chn.phy_addr; /* 2:para */
    caddr[2] = stt_cfg->me_cfg.w_mv_chn.phy_addr; /* 2:para */
    wth[3] = sml_blkmv_wth; /* 3:para */
    hgt[3] = sml_blkmv_hgt; /* 3:para */
    ystr[3] = align_anybw_16btye(big_blkmv_wth, 64) * 2; /* 3,16,64,2:para */
    yaddr[3] = stt_cfg->me_cfg.w_mv_chn.phy_addr; /* 3:para */
    caddr[3] = stt_cfg->me_cfg.w_mv_chn.phy_addr; /* 3:para */
    wth[4] = big_blkmv_wth; /* 4:para */
    hgt[4] = big_blkmv_hgt; /* 4:para */
    ystr[4] = align_anybw_16btye(big_blkmv_wth, 64); /* 4,16,64:para */
    yaddr[4] = stt_cfg->me_cfg.w_mv_chn.phy_addr; /* 4:para */
    caddr[4] = stt_cfg->me_cfg.w_mv_chn.phy_addr; /* 4:para */

    if (me_cfg->me_en == 1) {
        rchn_cfg[VPSS_MAC_RCHN_ME_PRMV].en = HI_TRUE;
        rchn_cfg[VPSS_MAC_RCHN_ME_PRMV].tunl_en = tunl_en[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_PRMV].in_rect.x = 0;
        rchn_cfg[VPSS_MAC_RCHN_ME_PRMV].in_rect.y = 0;
        rchn_cfg[VPSS_MAC_RCHN_ME_PRMV].mirror_en = mirror[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_PRMV].flip_en = flip[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_PRMV].in_rect.wth = wth[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_PRMV].in_rect.hgt = hgt[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_PRMV].addr[VPSS_RCHN_ADDR_DATA].str_y = ystr[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_PRMV].addr[VPSS_RCHN_ADDR_DATA].addr_y = yaddr[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_PRMV].addr[VPSS_RCHN_ADDR_DATA].addr_u = caddr[scan_cnt];
        vpss_mac_set_rchn_cfg(vpss_regs, VPSS_MAC_RCHN_ME_PRMV, &rchn_cfg[VPSS_MAC_RCHN_ME_PRMV]);
    }
}

hi_void me_p1_gmvchn_set(vpss_reg_type *vpss_regs, vpss_mc_info *mc_info,
                         vpss_me_cfg *me_cfg, hi_u32 scan_cnt,
                         vpss_mac_rchn_cfg *rchn_cfg, vpss_mac_wchn_cfg *wchn_cfg)
{
    hi_u32 wth[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 hgt[XDP_MAX_SCAN_NUM] = { 0 };
    hi_bool mirror[XDP_MAX_SCAN_NUM] = { HI_FALSE, HI_FALSE, HI_TRUE, HI_FALSE, HI_TRUE };
    hi_bool flip[XDP_MAX_SCAN_NUM] = { HI_FALSE, HI_FALSE, HI_TRUE, HI_FALSE, HI_TRUE };
    hi_bool tunl_en[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 i = 0;
    hi_u32 ystr[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u64 yaddr[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u64 caddr[XDP_MAX_SCAN_NUM] = { 0 };
    vpss_hal_stt_cfg *stt_cfg;
    hi_u32 gmv_wth;
    hi_u32 gmv_hgt;
    stt_cfg = mc_info->stt_cfg;
    gmv_wth = 8; /* 8:para */
    gmv_hgt = 15; /* 15:para */

    for (i = 0; i < XDP_MAX_SCAN_NUM; i++) {
        wth[i] = gmv_wth;
        hgt[i] = gmv_hgt;
        ystr[i] = align_anybw_16btye(gmv_wth, 64); /* 16,64:para */
        yaddr[i] = stt_cfg->me_cfg.rg_mv_p1_chn.phy_addr;
        caddr[i] = stt_cfg->me_cfg.rg_mv_p1_chn.phy_addr;
    }

    if (me_cfg->me_en == 1) {
        rchn_cfg[VPSS_MAC_RCHN_ME_P1GMV].en = HI_TRUE;
        rchn_cfg[VPSS_MAC_RCHN_ME_P1GMV].tunl_en = tunl_en[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_P1GMV].in_rect.x = 0;
        rchn_cfg[VPSS_MAC_RCHN_ME_P1GMV].in_rect.y = 0;
        rchn_cfg[VPSS_MAC_RCHN_ME_P1GMV].mirror_en = mirror[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_P1GMV].flip_en = flip[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_P1GMV].in_rect.wth = wth[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_P1GMV].in_rect.hgt = hgt[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_P1GMV].addr[VPSS_RCHN_ADDR_DATA].str_y = ystr[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_P1GMV].addr[VPSS_RCHN_ADDR_DATA].addr_y = yaddr[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_P1GMV].addr[VPSS_RCHN_ADDR_DATA].addr_u = caddr[scan_cnt];
        vpss_mac_set_rchn_cfg(vpss_regs, VPSS_MAC_RCHN_ME_P1GMV, &rchn_cfg[VPSS_MAC_RCHN_ME_P1GMV]);
    }
}

hi_void me_prgmvchn_set(vpss_reg_type *vpss_regs, vpss_mc_info *mc_info,
                        vpss_me_cfg *me_cfg, hi_u32 scan_cnt,
                        vpss_mac_rchn_cfg *rchn_cfg, vpss_mac_wchn_cfg *wchn_cfg)
{
    hi_u32 wth[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 hgt[XDP_MAX_SCAN_NUM] = { 0 };
    hi_bool mirror[XDP_MAX_SCAN_NUM] = { HI_FALSE, HI_FALSE, HI_TRUE, HI_FALSE, HI_TRUE };
    hi_bool flip[XDP_MAX_SCAN_NUM] = { HI_FALSE, HI_FALSE, HI_TRUE, HI_FALSE, HI_TRUE };
    hi_bool tunl_en[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 i = 0;
    hi_u32 ystr[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u64 yaddr[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u64 caddr[XDP_MAX_SCAN_NUM] = { 0 };
    vpss_hal_stt_cfg *stt_cfg;
    hi_u32 gmv_wth;
    hi_u32 gmv_hgt;
    stt_cfg = mc_info->stt_cfg;
    gmv_wth = 8; /* 8:para */
    gmv_hgt = 15; /* 15:para */

    for (i = 0; i < XDP_MAX_SCAN_NUM; i++) {
        wth[i] = gmv_wth;
        hgt[i] = gmv_hgt;
        ystr[i] = align_anybw_16btye(gmv_wth, 64); /* 16,64:para */

        if ((i == 0) || (i == 1)) {
            yaddr[i] = stt_cfg->me_cfg.rg_mv_p1_chn.phy_addr;
            caddr[i] = stt_cfg->me_cfg.rg_mv_p1_chn.phy_addr;
        } else {
            yaddr[i] = stt_cfg->me_cfg.wg_mv_chn.phy_addr;
            caddr[i] = stt_cfg->me_cfg.wg_mv_chn.phy_addr;
        }
    }

    if (me_cfg->me_en == 1) {
        rchn_cfg[VPSS_MAC_RCHN_ME_PRGMV].en = HI_TRUE;
        rchn_cfg[VPSS_MAC_RCHN_ME_PRGMV].tunl_en = tunl_en[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_PRGMV].in_rect.x = 0;
        rchn_cfg[VPSS_MAC_RCHN_ME_PRGMV].in_rect.y = 0;
        rchn_cfg[VPSS_MAC_RCHN_ME_PRGMV].mirror_en = mirror[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_PRGMV].flip_en = flip[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_PRGMV].in_rect.wth = wth[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_PRGMV].in_rect.hgt = hgt[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_PRGMV].addr[VPSS_RCHN_ADDR_DATA].str_y = ystr[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_PRGMV].addr[VPSS_RCHN_ADDR_DATA].addr_y = yaddr[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_PRGMV].addr[VPSS_RCHN_ADDR_DATA].addr_u = caddr[scan_cnt];
        vpss_mac_set_rchn_cfg(vpss_regs, VPSS_MAC_RCHN_ME_PRGMV, &rchn_cfg[VPSS_MAC_RCHN_ME_PRGMV]);
    }
}

hi_void me_p1_rgmvchn_set(vpss_reg_type *vpss_regs, vpss_mc_info *mc_info,
                          vpss_me_cfg *me_cfg, hi_u32 scan_cnt,
                          vpss_mac_rchn_cfg *rchn_cfg, vpss_mac_wchn_cfg *wchn_cfg)
{
    hi_u32 wth[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 hgt[XDP_MAX_SCAN_NUM] = { 0 };
    hi_bool mirror[XDP_MAX_SCAN_NUM] = { HI_FALSE, HI_FALSE, HI_TRUE, HI_FALSE, HI_TRUE };
    hi_bool flip[XDP_MAX_SCAN_NUM] = { HI_FALSE, HI_FALSE, HI_TRUE, HI_FALSE, HI_TRUE };
    hi_bool tunl_en[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 i = 0;
    hi_u32 ystr[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u64 yaddr[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u64 caddr[XDP_MAX_SCAN_NUM] = { 0 };
    vpss_hal_stt_cfg *stt_cfg;
    hi_u32 rgmv_wth;
    hi_u32 rgmv_hgt;
    hi_u32 interlace;
    hi_u32 big_wth;
    hi_u32 big_hgt;
    stt_cfg = mc_info->stt_cfg;
    interlace = rchn_cfg[VPSS_MAC_RCHN_CF].rd_mode == XDP_RMODE_PROGRESSIVE ? 0 : 1;
    big_wth = (me_cfg->meds_en == HI_FALSE) ? rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth :
              (rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth / 2 + 1) / 2 * 2; /* 2,1,2,2:para */
    big_hgt = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.hgt;
    rgmv_wth = (big_wth + 33) / 64; /* 33,64:para */
    rgmv_hgt = ((big_hgt >> interlace) + 9) / 16; /* 9,16:para */

    for (i = 0; i < XDP_MAX_SCAN_NUM; i++) {
        wth[i] = rgmv_wth;
        hgt[i] = rgmv_hgt;
        ystr[i] = align_anybw_16btye(rgmv_wth, 64); /* 16,64:para */

        if (i == 0) {
            yaddr[i] = stt_cfg->me_cfg.r_rgmv_p2_chn.phy_addr;
            caddr[i] = stt_cfg->me_cfg.r_rgmv_p2_chn.phy_addr;
        } else {
            yaddr[i] = stt_cfg->me_cfg.r_rgmv_p1_chn.phy_addr;
            caddr[i] = stt_cfg->me_cfg.r_rgmv_p1_chn.phy_addr;
        }
    }

    if (me_cfg->me_en == 1) {
        rchn_cfg[VPSS_MAC_RCHN_ME_P1RGMV].en = HI_TRUE;
        rchn_cfg[VPSS_MAC_RCHN_ME_P1RGMV].tunl_en = tunl_en[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_P1RGMV].in_rect.x = 0;
        rchn_cfg[VPSS_MAC_RCHN_ME_P1RGMV].in_rect.y = 0;
        rchn_cfg[VPSS_MAC_RCHN_ME_P1RGMV].mirror_en = mirror[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_P1RGMV].flip_en = flip[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_P1RGMV].in_rect.wth = wth[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_P1RGMV].in_rect.hgt = hgt[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_P1RGMV].addr[VPSS_RCHN_ADDR_DATA].str_y = ystr[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_P1RGMV].addr[VPSS_RCHN_ADDR_DATA].addr_y = yaddr[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_P1RGMV].addr[VPSS_RCHN_ADDR_DATA].addr_u = caddr[scan_cnt];
        vpss_mac_set_rchn_cfg(vpss_regs, VPSS_MAC_RCHN_ME_P1RGMV, &rchn_cfg[VPSS_MAC_RCHN_ME_P1RGMV]);
    }
}

hi_void me_cfrgmvchn_set(vpss_reg_type *vpss_regs, vpss_mc_info *mc_info,
                         vpss_me_cfg *me_cfg, hi_u32 scan_cnt,
                         vpss_mac_rchn_cfg *rchn_cfg, vpss_mac_wchn_cfg *wchn_cfg)
{
    hi_u32 wth[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 hgt[XDP_MAX_SCAN_NUM] = { 0 };
    hi_bool mirror[XDP_MAX_SCAN_NUM] = { HI_FALSE, HI_FALSE, HI_TRUE, HI_FALSE, HI_TRUE };
    hi_bool flip[XDP_MAX_SCAN_NUM] = { HI_FALSE, HI_FALSE, HI_TRUE, HI_FALSE, HI_TRUE };
    hi_bool tunl_en[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 i = 0;
    hi_u32 ystr[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u64 yaddr[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u64 caddr[XDP_MAX_SCAN_NUM] = { 0 };
    vpss_hal_stt_cfg *stt_cfg;
    hi_u32 rgmv_wth;
    hi_u32 rgmv_hgt;
    hi_u32 interlace;
    hi_u32 big_wth;
    hi_u32 big_hgt;
    stt_cfg = mc_info->stt_cfg;
    interlace = rchn_cfg[VPSS_MAC_RCHN_CF].rd_mode == XDP_RMODE_PROGRESSIVE ? 0 : 1;
    big_wth = (me_cfg->meds_en == HI_FALSE) ? rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth :
              (rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth / 2 + 1) / 2 * 2; /* 2,1,2,2:para */
    big_hgt = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.hgt;
    rgmv_wth = (big_wth + 33) / 64; /* 33,64:para */
    rgmv_hgt = ((big_hgt >> interlace) + 9) / 16; /* 9,16:para */

    for (i = 0; i < XDP_MAX_SCAN_NUM; i++) {
        wth[i] = rgmv_wth;
        hgt[i] = rgmv_hgt;
        ystr[i] = align_anybw_16btye(rgmv_wth, 64); /* 16,64:para */

        if (i != 0) {
            tunl_en[i] = g_gb_tunl_en;
        }

        if (i == 0) {
            yaddr[i] = stt_cfg->me_cfg.r_rgmv_p1_chn.phy_addr;
            caddr[i] = stt_cfg->me_cfg.r_rgmv_p1_chn.phy_addr;
        } else {
            yaddr[i] = stt_cfg->me_cfg.r_rgmv_cf_chn.phy_addr;
            caddr[i] = stt_cfg->me_cfg.r_rgmv_cf_chn.phy_addr;
        }
    }

    if (me_cfg->me_en == 1) {
        rchn_cfg[VPSS_MAC_RCHN_ME_CFRGMV].en = HI_TRUE;
        rchn_cfg[VPSS_MAC_RCHN_ME_CFRGMV].tunl_en = tunl_en[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_CFRGMV].in_rect.x = 0;
        rchn_cfg[VPSS_MAC_RCHN_ME_CFRGMV].in_rect.y = 0;
        rchn_cfg[VPSS_MAC_RCHN_ME_CFRGMV].mirror_en = mirror[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_CFRGMV].flip_en = flip[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_CFRGMV].in_rect.wth = wth[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_CFRGMV].in_rect.hgt = hgt[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_CFRGMV].addr[VPSS_RCHN_ADDR_DATA].str_y = ystr[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_CFRGMV].addr[VPSS_RCHN_ADDR_DATA].addr_y = yaddr[scan_cnt];
        rchn_cfg[VPSS_MAC_RCHN_ME_CFRGMV].addr[VPSS_RCHN_ADDR_DATA].addr_u = caddr[scan_cnt];
        vpss_mac_set_rchn_cfg(vpss_regs, VPSS_MAC_RCHN_ME_CFRGMV, &rchn_cfg[VPSS_MAC_RCHN_ME_CFRGMV]);
    }
}

hi_void me_cfmvchn_set(vpss_reg_type *vpss_regs, vpss_mc_info *mc_info,
                       vpss_me_cfg *me_cfg, hi_u32 scan_cnt,
                       vpss_mac_rchn_cfg *rchn_cfg, vpss_mac_wchn_cfg *wchn_cfg)
{
    hi_u32 wth[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 hgt[XDP_MAX_SCAN_NUM] = { 0 };
    hi_bool mirror[XDP_MAX_SCAN_NUM] = { HI_FALSE, HI_FALSE, HI_TRUE, HI_FALSE, HI_TRUE };
    hi_bool flip[XDP_MAX_SCAN_NUM] = { HI_FALSE, HI_FALSE, HI_TRUE, HI_FALSE, HI_TRUE };
    hi_u32 ystr[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u64 yaddr[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u64 caddr[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 interlace;
    hi_u32 big_wth;
    hi_u32 big_hgt;
    hi_u32 sml_wth;
    hi_u32 big_blkmv_wth;
    hi_u32 big_blkmv_hgt;
    hi_u32 sml_blkmv_wth;
    hi_u32 sml_blkmv_hgt;
    vpss_hal_stt_cfg *stt_cfg;
    stt_cfg = mc_info->stt_cfg;
    interlace = rchn_cfg[VPSS_MAC_RCHN_CF].rd_mode == XDP_RMODE_PROGRESSIVE ? 0 : 1;
    big_wth = (me_cfg->meds_en == HI_FALSE) ? rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth :
              (rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth / 2 + 1) / 2 * 2; /* 2,1,2,2:para */
    big_hgt = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.hgt;
    sml_wth = (big_wth / 2 + 1) / 2 * 2; /* 2,1,2,2:para */
    big_blkmv_wth = (big_wth + 7) / 8; /* 7,8:para */
    big_blkmv_hgt = ((big_hgt >> interlace) + 3) / 4; /* 3,4:para */
    sml_blkmv_wth = (sml_wth + 7) / 8; /* 7,8:para */
    sml_blkmv_hgt = ((big_hgt >> interlace) / 2 + 3) / 4; /* 2,3,4:para */
    wth[0] = big_blkmv_wth;
    hgt[0] = big_blkmv_hgt;
    ystr[0] = align_anybw_16btye(big_blkmv_wth, 64); /* 0,16,64:para */
    yaddr[0] = stt_cfg->me_cfg.w_me_for_nr_stt_chn.phy_addr;
    caddr[0] = stt_cfg->me_cfg.w_me_for_nr_stt_chn.phy_addr;
    wth[1] = sml_blkmv_wth;
    hgt[1] = sml_blkmv_hgt;
    ystr[1] = align_anybw_16btye(big_blkmv_wth, 64) * 2; /* 1,16,64,2:para */
    yaddr[1] = stt_cfg->me_cfg.w_mv_chn.phy_addr;
    caddr[1] = stt_cfg->me_cfg.w_mv_chn.phy_addr;
    wth[2] = sml_blkmv_wth; /* 2:para */
    hgt[2] = sml_blkmv_hgt; /* 2:para */
    ystr[2] = align_anybw_16btye(big_blkmv_wth, 64) * 2; /* 2,16,64,2:para */
    yaddr[2] = stt_cfg->me_cfg.w_mv_chn.phy_addr; /* 2:para */
    caddr[2] = stt_cfg->me_cfg.w_mv_chn.phy_addr; /* 2:para */
    wth[3] = big_blkmv_wth; /* 3:para */
    hgt[3] = big_blkmv_hgt; /* 3:para */
    ystr[3] = align_anybw_16btye(big_blkmv_wth, 64); /* 3,16,64:para */
    yaddr[3] = stt_cfg->me_cfg.w_mv_chn.phy_addr; /* 3:para */
    caddr[3] = stt_cfg->me_cfg.w_mv_chn.phy_addr; /* 3:para */
    wth[4] = big_blkmv_wth; /* 4:para */
    hgt[4] = big_blkmv_hgt; /* 4:para */
    ystr[4] = align_anybw_16btye(big_blkmv_wth, 64); /* 4,16,64:para */
    yaddr[4] = stt_cfg->me_cfg.w_mv_chn.phy_addr; /* 4:para */
    caddr[4] = stt_cfg->me_cfg.w_mv_chn.phy_addr; /* 4:para */

    if (me_cfg->me_en == 1) {
        wchn_cfg[VPSS_MAC_WCHN_ME_CFMV].en = HI_TRUE;
        wchn_cfg[VPSS_MAC_WCHN_ME_CFMV].out_rect.x = 0;
        wchn_cfg[VPSS_MAC_WCHN_ME_CFMV].out_rect.y = 0;
        wchn_cfg[VPSS_MAC_WCHN_ME_CFMV].mirror_en = mirror[scan_cnt];
        wchn_cfg[VPSS_MAC_WCHN_ME_CFMV].flip_en = flip[scan_cnt];
        wchn_cfg[VPSS_MAC_WCHN_ME_CFMV].out_rect.wth = wth[scan_cnt];
        wchn_cfg[VPSS_MAC_WCHN_ME_CFMV].out_rect.hgt = hgt[scan_cnt];
        wchn_cfg[VPSS_MAC_WCHN_ME_CFMV].addr[VPSS_WCHN_ADDR_DATA].str_y = ystr[scan_cnt];
        wchn_cfg[VPSS_MAC_WCHN_ME_CFMV].addr[VPSS_WCHN_ADDR_DATA].addr_y = yaddr[scan_cnt];
        wchn_cfg[VPSS_MAC_WCHN_ME_CFMV].addr[VPSS_WCHN_ADDR_DATA].addr_u = caddr[scan_cnt];
        vpss_mac_set_wchn_cfg(vpss_regs, VPSS_MAC_WCHN_ME_CFMV, &wchn_cfg[VPSS_MAC_WCHN_ME_CFMV]);
        wchn_cfg[VPSS_MAC_WCHN_ME_CFMV].vir_addr_y = stt_cfg->me_cfg.w_mv_chn.vir_addr;
        wchn_cfg[VPSS_MAC_WCHN_ME_CFMV].size = stt_cfg->me_cfg.w_mv_chn.size;
    }
}

hi_void me_cfgmvchn_set(vpss_reg_type *vpss_regs, vpss_mc_info *mc_info,
                        vpss_me_cfg *me_cfg, hi_u32 scan_cnt,
                        vpss_mac_rchn_cfg *rchn_cfg, vpss_mac_wchn_cfg *wchn_cfg)
{
    hi_u32 wth[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u32 hgt[XDP_MAX_SCAN_NUM] = { 0 };
    hi_bool mirror[XDP_MAX_SCAN_NUM] = { HI_FALSE, HI_FALSE, HI_TRUE, HI_FALSE, HI_TRUE };
    hi_bool flip[XDP_MAX_SCAN_NUM] = { HI_FALSE, HI_FALSE, HI_TRUE, HI_FALSE, HI_TRUE };
    hi_u32 i = 0;
    hi_u32 ystr[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u64 yaddr[XDP_MAX_SCAN_NUM] = { 0 };
    hi_u64 caddr[XDP_MAX_SCAN_NUM] = { 0 };
    vpss_hal_stt_cfg *stt_cfg;
    hi_u32 gmv_wth;
    hi_u32 gmv_hgt;
    stt_cfg = mc_info->stt_cfg;
    gmv_wth = 8; /* 8:para */
    gmv_hgt = 15; /* 15:para */

    // me cf gmv
    for (i = 0; i < XDP_MAX_SCAN_NUM; i++) {
        wth[i] = gmv_wth;
        hgt[i] = gmv_hgt;
        ystr[i] = align_anybw_16btye(gmv_wth, 64); /* 16,64:para */
        yaddr[i] = stt_cfg->me_cfg.wg_mv_chn.phy_addr;
        caddr[i] = stt_cfg->me_cfg.wg_mv_chn.phy_addr;
    }

    if (me_cfg->me_en == 1) {
        wchn_cfg[VPSS_MAC_WCHN_ME_CFGMV].en = HI_TRUE;
        wchn_cfg[VPSS_MAC_WCHN_ME_CFGMV].out_rect.x = 0;
        wchn_cfg[VPSS_MAC_WCHN_ME_CFGMV].out_rect.y = 0;
        wchn_cfg[VPSS_MAC_WCHN_ME_CFGMV].mirror_en = mirror[scan_cnt];
        wchn_cfg[VPSS_MAC_WCHN_ME_CFGMV].flip_en = flip[scan_cnt];
        wchn_cfg[VPSS_MAC_WCHN_ME_CFGMV].out_rect.wth = wth[scan_cnt];
        wchn_cfg[VPSS_MAC_WCHN_ME_CFGMV].out_rect.hgt = hgt[scan_cnt];
        wchn_cfg[VPSS_MAC_WCHN_ME_CFGMV].addr[VPSS_WCHN_ADDR_DATA].str_y = ystr[scan_cnt];
        wchn_cfg[VPSS_MAC_WCHN_ME_CFGMV].addr[VPSS_WCHN_ADDR_DATA].addr_y = yaddr[scan_cnt];
        wchn_cfg[VPSS_MAC_WCHN_ME_CFGMV].addr[VPSS_WCHN_ADDR_DATA].addr_u = caddr[scan_cnt];
        vpss_mac_set_wchn_cfg(vpss_regs, VPSS_MAC_WCHN_ME_CFGMV, &wchn_cfg[VPSS_MAC_WCHN_ME_CFGMV]);
        wchn_cfg[VPSS_MAC_WCHN_ME_CFGMV].vir_addr_y = stt_cfg->me_cfg.wg_mv_chn.vir_addr;
        wchn_cfg[VPSS_MAC_WCHN_ME_CFGMV].size = stt_cfg->me_cfg.wg_mv_chn.size;
    }
}

hi_void me_chn_set(vpss_reg_type *vpss_regs, vpss_mc_info *mc_info,
                   vpss_me_cfg *me_cfg, hi_u32 scan_cnt,
                   vpss_mac_rchn_cfg *rchn_cfg, vpss_mac_wchn_cfg *wchn_cfg)
{
    vpss_hal_stt_cfg *stt_cfg = mc_info->stt_cfg;
    hi_u64 phy_addr;

    if (me_cfg->me_en == 1) {
        me_cfchn_set(vpss_regs, mc_info, me_cfg, scan_cnt, rchn_cfg, wchn_cfg);
        me_refchn_set(vpss_regs, mc_info, me_cfg, scan_cnt, rchn_cfg, wchn_cfg);
        me_p1_mvchn_set(vpss_regs, mc_info, me_cfg, scan_cnt, rchn_cfg, wchn_cfg);
        me_prmvchn_set(vpss_regs, mc_info, me_cfg, scan_cnt, rchn_cfg, wchn_cfg);
        me_p1_gmvchn_set(vpss_regs, mc_info, me_cfg, scan_cnt, rchn_cfg, wchn_cfg);
        me_prgmvchn_set(vpss_regs, mc_info, me_cfg, scan_cnt, rchn_cfg, wchn_cfg);
        me_p1_rgmvchn_set(vpss_regs, mc_info, me_cfg, scan_cnt, rchn_cfg, wchn_cfg);
        me_cfrgmvchn_set(vpss_regs, mc_info, me_cfg, scan_cnt, rchn_cfg, wchn_cfg);
        me_cfmvchn_set(vpss_regs, mc_info, me_cfg, scan_cnt, rchn_cfg, wchn_cfg);
        me_cfgmvchn_set(vpss_regs, mc_info, me_cfg, scan_cnt, rchn_cfg, wchn_cfg);

        /* me stt cfg */
        if (scan_cnt == 0) {
            phy_addr = stt_cfg->global_cfg.w_me1_stt_chn.phy_addr;
            vpss_mac_set_me_stt_waddr_l(vpss_regs, 0, (hi_u32)phy_addr);
            vpss_mac_set_me_stt_waddr_h(vpss_regs, 0, (hi_u32)(phy_addr >> VPSS_REG_SHIFT));
        } else {
            phy_addr = stt_cfg->global_cfg.w_me2_stt_chn.phy_addr;
            vpss_mac_set_me_stt_waddr_l(vpss_regs, 0, (hi_u32)phy_addr);
            vpss_mac_set_me_stt_waddr_h(vpss_regs, 0, (hi_u32)(phy_addr >> VPSS_REG_SHIFT));
        }
    }
}

hi_void mc_set(vpss_reg_type *vpss_regs, hi_u64 node_phy_addr,
               vpss_mc_info *mc_info, vpss_mac_rchn_cfg *rchn_cfg, vpss_mac_wchn_cfg *wchn_cfg)
{
    hi_u32 i, j;
    vpss_hzme_cfg hzme_cfg = { 0 };
    vpss_hzme_pq hzme_pq_cfg = { 0 };
    vpss_vzme_cfg vzme_cfg = { 0 };
    vpss_vzme_pq vzme_pq_cfg = { 0 };
    hi_u32 me_scan_num = 0;
    hi_u32 scan_pnext_addr = 0;
    vpss_me_cfg me_cfg = { 0 };
    vpss_hzme_cfg *hzme_cfg_0 = HI_NULL;
    vpss_hzme_pq *hzme_pq_cfg1 = HI_NULL;
    hi_bool cccl_en = HI_FALSE;
    hi_bool vc1_en = HI_FALSE;
    hi_bool me_vc1_en = HI_FALSE;
    // cf_dn hds cfg
    hzme_cfg_0 = &hzme_cfg;
    hzme_pq_cfg1 = &hzme_pq_cfg;
    hzme_cfg_0->in_height = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.hgt;
    hzme_cfg_0->out_height = hzme_cfg_0->in_height;
    hzme_cfg_0->in_width = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth;
    /* 0,1,0,2,1,2,2,0:para */
    hzme_cfg_0->out_width = (mc_info->meds_en == 1) ? (hzme_cfg_0->in_width / 2 + 1) / 2 * 2 : hzme_cfg_0->in_width;
    hzme_cfg_0->out_fmt = rchn_cfg[VPSS_MAC_RCHN_CF].data_fmt;
    hzme_cfg_0->ck_gt_en = 1;
    hzme_cfg_0->lh_fir_en = (mc_info->meds_en == 1) ? 1 : 0;
    hzme_cfg_0->ch_fir_en = hzme_cfg_0->lh_fir_en;
    hzme_cfg_0->lh_fir_mode = 0; /* 1 todo */
    hzme_cfg_0->ch_fir_mode = 0; /* 1 todo */
    hzme_pq_cfg1->lh_stc2nd_en = 0;
    hzme_pq_cfg1->ch_stc2nd_en = 0;
    hzme_pq_cfg1->lh_med_en = 0;
    hzme_pq_cfg1->ch_med_en = 0;
    hzme_pq_cfg1->nonlinear_scl_en = 0;
    hzme_pq_cfg1->lhfir_offset = 0;
    hzme_pq_cfg1->chfir_offset = 0;
    vpss_func_set_hzme_mode(vpss_regs, XDP_ZME_ID_VPSS_WR0_HDS_HZME, VPSS_HZME_TYP, hzme_cfg_0, hzme_pq_cfg1);

    if (mc_info->en3_drs_version == XDP_3DRS_MC) {
        // rgmv hvds
        hzme_cfg.in_height = wchn_cfg[VPSS_MAC_WCHN_NR_RFRH].out_rect.hgt;
        hzme_cfg.out_height = hzme_cfg.in_height;
        hzme_cfg.in_width = wchn_cfg[VPSS_MAC_WCHN_NR_RFRH].out_rect.wth;
        hzme_cfg.out_width = (hzme_cfg.in_width / 2 + 1) / 2 * 2; /* 2,1,2,2:para */
        hzme_cfg.out_fmt = rchn_cfg[VPSS_MAC_RCHN_CF].data_fmt;
        hzme_cfg.ck_gt_en = 1;
        hzme_cfg.lh_fir_en = 1;
        hzme_cfg.ch_fir_en = 1;
        hzme_cfg.lh_fir_mode = 1;
        hzme_cfg.ch_fir_mode = 1;
        hzme_pq_cfg.lh_stc2nd_en = 0;
        hzme_pq_cfg.ch_stc2nd_en = 0;
        hzme_pq_cfg.lh_med_en = 0;
        hzme_pq_cfg.ch_med_en = 0;
        hzme_pq_cfg.nonlinear_scl_en = 0;
        hzme_pq_cfg.lhfir_offset = 0;
        hzme_pq_cfg.chfir_offset = 0;
        vpss_func_set_hzme_mode(vpss_regs, XDP_ZME_ID_VPSS_WR1_HVDS_HZME, VPSS_HZME_TYP, &hzme_cfg, &hzme_pq_cfg);
        vzme_cfg.in_height = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.hgt;
        vzme_cfg.out_height = vzme_cfg.in_height / 2; /* 2:para */
        vzme_cfg.in_width = hzme_cfg.out_width;
        vzme_cfg.out_width = vzme_cfg.in_width;
        vzme_cfg.out_fmt = rchn_cfg[VPSS_MAC_RCHN_CF].data_fmt;
        vzme_cfg.ck_gt_en = 1;
        vzme_cfg.lv_fir_en = 1;
        vzme_cfg.cv_fir_en = 1;
        vzme_cfg.lv_fir_mode = 0; /* 1 todo */
        vzme_cfg.cv_fir_mode = 0; /* 1 todo */
        vzme_pq_cfg.lv_stc2nd_en = 0;
        vzme_pq_cfg.cv_stc2nd_en = 0;
        vzme_pq_cfg.lv_med_en = 0;
        vzme_pq_cfg.cv_med_en = 0;
        vzme_pq_cfg.lvfir_offset = 0;
        vzme_pq_cfg.cvfir_offset = 0;
        vpss_func_set_vzme_mode(vpss_regs, XDP_ZME_ID_VPSS_WR1_HVDS_VZME, VPSS_VZME_TYP, &vzme_cfg, &vzme_pq_cfg);
        wchn_cfg[VPSS_MAC_WCHN_NR_RFRHV].en = HI_TRUE;
        wchn_cfg[VPSS_MAC_WCHN_NR_RFRHV].data_fmt = XDP_PROC_FMT_SP_400;
        wchn_cfg[VPSS_MAC_WCHN_NR_RFRHV].data_width = XDP_DATA_WTH_8;
        wchn_cfg[VPSS_MAC_WCHN_NR_RFRHV].out_rect.wth = vzme_cfg.out_width;
        wchn_cfg[VPSS_MAC_WCHN_NR_RFRHV].out_rect.hgt = vzme_cfg.out_height >> (mc_info->pro == 0);
        wchn_cfg[VPSS_MAC_WCHN_NR_RFRHV].addr[VPSS_WCHN_ADDR_DATA].str_y = align_8bit_16_btye(vzme_cfg.out_width);
        wchn_cfg[VPSS_MAC_WCHN_NR_RFRHV].addr[VPSS_WCHN_ADDR_DATA].str_c = align_8bit_16_btye(vzme_cfg.out_width);
        vpss_mac_set_wchn_cfg(vpss_regs, VPSS_MAC_WCHN_NR_RFRHV, &(wchn_cfg[VPSS_MAC_WCHN_NR_RFRHV]));
        /* me cf hds */
        hzme_cfg.in_height = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.hgt;
        hzme_cfg.out_height = hzme_cfg.in_height;
        hzme_cfg.in_width = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth;
        /* 1,2,1,2,2:para */
        hzme_cfg.out_width = (mc_info->meds_en == 1) ? (hzme_cfg.in_width / 2 + 1) / 2 * 2 : hzme_cfg.in_width;
        hzme_cfg.out_fmt = rchn_cfg[VPSS_MAC_RCHN_CF].data_fmt;
        hzme_cfg.ck_gt_en = 1;
        hzme_cfg.lh_fir_en = (mc_info->meds_en == 1) ? 1 : 0;
        hzme_cfg.ch_fir_en = hzme_cfg.lh_fir_en;
        hzme_cfg.lh_fir_mode = 0; /* 1 todo */
        hzme_cfg.ch_fir_mode = 0; /* 1 todo */
        hzme_pq_cfg.lh_stc2nd_en = 0;
        hzme_pq_cfg.ch_stc2nd_en = 0;
        hzme_pq_cfg.lh_med_en = 0;
        hzme_pq_cfg.ch_med_en = 0;
        hzme_pq_cfg.nonlinear_scl_en = 0;
        hzme_pq_cfg.lhfir_offset = 0;
        hzme_pq_cfg.chfir_offset = 0;
        vpss_func_set_hzme_mode(vpss_regs, XDP_ZME_ID_VPSS_ME_CF_HDS_HZME, VPSS_HZME_TYP, &hzme_cfg, &hzme_pq_cfg);

        /* me ref hds */
        if (mc_info->me_version == XDP_ME_VER5) {
            vpss_func_set_hzme_mode(vpss_regs, XDP_ZME_ID_VPSS_ME_P2_HDS_HZME, VPSS_HZME_TYP, &hzme_cfg, &hzme_pq_cfg);
        }

        /* me */
        me_scan_num = mc_info->me_scan_num;
        vpss_scan_set_scan_start_addr(vpss_regs, 0, ((hi_u32)node_phy_addr + SCAN_OFFSET) >> 20, /* 0,32,20:para */
                                      ((hi_u32)node_phy_addr + SCAN_OFFSET) & 0xfffff);
        cccl_en = vpss_sys_get_cccl_en(vpss_regs);
        vc1_en = vpss_sys_get_vc1_en(vpss_regs);

        for (i = 0; i < me_scan_num; i++) {
            for (j = VPSS_MAC_RCHN_ME_CF; j < VPSS_MAC_RCHN_MAX; j++) {
                vpss_mac_init_rchn_cfg(&(rchn_cfg[j]));
            }

            for (j = VPSS_MAC_WCHN_ME_CFMV; j < VPSS_MAC_WCHN_MAX; j++) {
                vpss_mac_init_wchn_cfg(&(wchn_cfg[j]));
            }

            me_cfg.me_en = 1;
            me_cfg.meds_en = (mc_info->meds_en == 1) ? HI_TRUE : HI_FALSE;
            me_cfg.me_version = mc_info->me_version;
            me_cfg.up_smp = (i == 3) ? 1 : 0; /* 3,1,0:para */
            me_cfg.dn_smp = (i == 1) ? 1 : 0;
            me_cfg.layer = (i == 1 || i == 2) ? 1 : 0;  // 0: bigimg; 1: smlimg /* 1,2,1,0,0,1:para */
            me_cfg.me_scan = (i == 2 || i == 4) ? 1 : 0; /* 2,4,1,0:para */
            me_cfg.scan_no = i;
            me_cfg.me_cf_online_en = (i == 0) ? (cccl_en == HI_TRUE && me_cfg.me_version == XDP_ME_VER3) : HI_FALSE;
            vpss_scan_set_me_en ((hi_void *)((hi_u64) vpss_regs + VPSS_SCAN_REG_OFF_USE * i), 0, me_cfg.me_en);
            /* must cfg to default para */
            vpss_scan_set_me_vid_default_cfg ((hi_void *)((hi_u64) vpss_regs + VPSS_SCAN_REG_OFF_USE * i), 0);
            vpss_scan_set_me_scan ((hi_void *)((hi_u64) vpss_regs + VPSS_SCAN_REG_OFF_USE * i), 0, me_cfg.me_scan);
            vpss_scan_set_me_mv_upsmp_en((hi_void *)((hi_u64)vpss_regs + VPSS_SCAN_REG_OFF_USE * i), 0,
                                         me_cfg.up_smp);
            vpss_scan_set_me_mv_dnsmp_en((hi_void *)((hi_u64)vpss_regs + VPSS_SCAN_REG_OFF_USE * i), 0,
                                         me_cfg.dn_smp);
            vpss_scan_set_me_layer ((hi_void *)((hi_u64) vpss_regs + VPSS_SCAN_REG_OFF_USE * i), 0, me_cfg.layer);
            vpss_scan_set_me_cf_online_en((hi_void *)((hi_u64)vpss_regs + VPSS_SCAN_REG_OFF_USE * i), 0,
                                          me_cfg.me_cf_online_en);
            me_chn_set((hi_void *)((hi_u64)vpss_regs + VPSS_SCAN_REG_OFF_USE * i), mc_info, &me_cfg, i, rchn_cfg,
                       wchn_cfg);
            me_vc1_en = (vc1_en == HI_TRUE) && (i == 0);
            vpss_scan_set_me_vc1_en ((hi_void *)((hi_u64) vpss_regs + VPSS_SCAN_REG_OFF_USE * i), 0, me_vc1_en);
            set_me_vc1_reg(vpss_regs, (hi_drv_vc1_range_info *)mc_info->vc1_info);

            if (i < me_scan_num - 1) {
                scan_pnext_addr = (hi_u32)node_phy_addr + SCAN_OFFSET + VPSS_SCAN_REG_OFF_USE * (i + 1);
                vpss_scan_set_scan_pnext_addr((hi_void *)((hi_u64)vpss_regs + VPSS_SCAN_REG_OFF_USE * i), 0,
                                              scan_pnext_addr >> 20, scan_pnext_addr & 0xfffff); /* 20,0:para */
            } else {
                vpss_scan_set_scan_pnext_addr ((hi_void *)((hi_u64) vpss_regs + VPSS_SCAN_REG_OFF_USE * i), 0, 0, 0);
            }
        }
    }
}

hi_void mc_set_scan_addr(vpss_reg_type *vpss_regs, hi_u64 node_phy_addr)
{
    hi_u32 i = 0;
    hi_u32 me_scan_num = 5;
    hi_u32 scan_pnext_addr = 0x0;
    vpss_scan_set_scan_start_addr(vpss_regs, 0, ((hi_u32)node_phy_addr + SCAN_OFFSET) >> 20, /* 20:reg bit */
                                  ((hi_u32)node_phy_addr + SCAN_OFFSET) & 0xfffff);

    for (i = 0; i < me_scan_num; i++) {
        /* 4:32bit */
        vpss_scan_set_me_en ((hi_void *)((hi_u64) vpss_regs + VPSS_SCAN_REG_OFF_USE * i), 0, HI_FALSE);
        /* must cfg to default para; 4:32bit */
        vpss_scan_set_me_vid_default_cfg ((hi_void *)((hi_u64) vpss_regs + VPSS_SCAN_REG_OFF_USE * i), 0);

        if (i < me_scan_num - 1) {
            scan_pnext_addr = (hi_u32)node_phy_addr + SCAN_OFFSET + VPSS_SCAN_REG_OFF_USE * (i + 1);
            vpss_scan_set_scan_pnext_addr((hi_void *)((hi_u64)vpss_regs + VPSS_SCAN_REG_OFF_USE * i), 0,
                                          scan_pnext_addr >> 20, scan_pnext_addr & 0xfffff); /* 20,0:para */
        } else {
            vpss_scan_set_scan_pnext_addr ((hi_void *)((hi_u64) vpss_regs + VPSS_SCAN_REG_OFF_USE * i), 0, 0, 0);
        }
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

