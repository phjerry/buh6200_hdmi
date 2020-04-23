/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_ip_crg.h"
#include "vdp_chip_define.h"
#include "hal_vdp_ip_dispchn.h"
#include "hal_vdp_comm.h"
#include "hi_osal.h"

#ifdef HI_TEE_SUPPORT
#include "linux/hisilicon/hi_drv_mbx.h"
#endif

#ifndef __DISP_PLATFORM_BOOT__
#include <asm/io.h>
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

// #define VDP_DEBUG VDP_PRINT
#define VDP_DEBUG(fmt...)

#define HPLL0_LINK         0 /* hpll 0 must link to hdmi 2.0 , default dhd1 */
#define HPLL1_LINK         1 /* hpll 1 must link to hdmi 2.1 , default dhd0 */
#define PLL_CLK_UNIT       6000
#define HPLL_REF_CLK       24000
#define HPLL_CLK_DIV_1     1
#define HPLL_CLK_DIV_2     2
#define HPLL_CLK_DIV_4     4
#define HPLL_CLK_DIV_8     8
#define HPLL_CLK_DIV_16    16
#define PIX_8_MULT_MODE    8
#define VDP_RESET_SLEEP_MS 40
#define HLL_POWER_DOWN_BIT (1 << 23)

typedef enum {
    VO_HD_CLK_DIV_4 = 0x0,
    VO_HD_CLK_DIV_8 = 0x1,
    VO_HD_CLK_DIV_1 = 0x2,
} hpll_vo_hd_clk_div;

typedef enum {
    VO_HD_HDMI_CLK_DIV_1 = 0x0,
    VO_HD_HDMI_CLK_DIV_2 = 0x1,
} hpll_vo_hd_hdmi_clk_sel;

typedef struct {
    hi_u32 pix_clk_start;
    hi_u32 pix_clk_end;
    hi_u32 intf_clk_div;
    hi_u32 hpll_clk_div;
} crg_pix_div_info;

hi_void vdp_ip_crg_attach_intf(vdp_dispchn_chn chn_id, vdp_disp_intf intf)
{
    U_PERI_CRG345 PERI_CRG345;
    U_PERI_CRG346 PERI_CRG346;
    U_PERI_CRG358 PERI_CRG358;
    PERI_CRG345.u32 = g_vdp_crg_reg->PERI_CRG345.u32;
    PERI_CRG346.u32 = g_vdp_crg_reg->PERI_CRG346.u32;
    PERI_CRG358.u32 = g_vdp_crg_reg->PERI_CRG358.u32;

    if (chn_id == VDP_DISPCHN_CHN_DHD0) {
        if (intf == VDP_DISP_INTF_HDMI) { /* set 2.1 */
            PERI_CRG345.bits.vdp_hdmi_clk_sel = 0;
            PERI_CRG345.bits.vo_hd_clk_sel = HPLL1_LINK;
        } else if (intf == VDP_DISP_INTF_HDMI1) { /* set 2.0 */
            PERI_CRG345.bits.vdp_hdmi1_clk_sel = 0;
            PERI_CRG345.bits.vo_hd_clk_sel = HPLL0_LINK;
        } else if (intf == VDP_DISP_INTF_MIPI) {
            PERI_CRG345.bits.vdp_mipi_clk_sel = 0;
            PERI_CRG345.bits.vo_hd_clk_sel = HPLL1_LINK;
        } else if (intf == VDP_DISP_INTF_CVBS) {
            PERI_CRG358.bits.vdac_ch0_clk_sel = 0;
            PERI_CRG358.bits.vdac_ch1_clk_sel = 0;
            PERI_CRG358.bits.vdac_ch2_clk_sel = 0;
            PERI_CRG358.bits.vdac_ch3_clk_sel = 0;
            PERI_CRG345.bits.vo_hd_clk_sel = HPLL1_LINK;
        } else if (intf == VDP_DISP_INTF_VBO) {
            PERI_CRG345.bits.vo_hd_clk_sel = HPLL0_LINK;
        }
    } else if (chn_id == VDP_DISPCHN_CHN_DHD1) {
        if (intf == VDP_DISP_INTF_HDMI) { /* set 2.1 */
            PERI_CRG345.bits.vdp_hdmi_clk_sel = 1;
            PERI_CRG346.bits.vo_hd1_clk_sel = HPLL1_LINK;
        } else if (intf == VDP_DISP_INTF_HDMI1) { /* set 2.0 */
            PERI_CRG345.bits.vdp_hdmi1_clk_sel = 1;
            PERI_CRG346.bits.vo_hd1_clk_sel = HPLL0_LINK;
        } else if (intf == VDP_DISP_INTF_MIPI) {
            PERI_CRG345.bits.vdp_mipi_clk_sel = 1;
            PERI_CRG346.bits.vo_hd1_clk_sel = HPLL0_LINK;
        } else if (intf == VDP_DISP_INTF_CVBS) {
            PERI_CRG358.bits.vdac_ch0_clk_sel = 1;
            PERI_CRG358.bits.vdac_ch1_clk_sel = 1;
            PERI_CRG358.bits.vdac_ch2_clk_sel = 1;
            PERI_CRG358.bits.vdac_ch3_clk_sel = 1;
            PERI_CRG346.bits.vo_hd1_clk_sel = HPLL0_LINK;
        }
    }
    g_vdp_crg_reg->PERI_CRG345.u32 = PERI_CRG345.u32;
    g_vdp_crg_reg->PERI_CRG346.u32 = PERI_CRG346.u32;
    g_vdp_crg_reg->PERI_CRG358.u32 = PERI_CRG358.u32;

    return;
}

typedef struct {
    hi_u32 hpll_clk_min;
    hi_u32 hpll_clk_max;
    hi_u32 posdiv1;
    hi_u32 posdiv2;
    hi_u32 refdiv;
} hpll_posdiv_table;

/* 800M < hpll * posdiv1 * posdiv2 < 3200M, posdiv1 > posdiv2 */
static hpll_posdiv_table g_div_table[] = {
    {10000000,  40000000,  6, 6, 1},
    {40000000,  60000000,  6, 4, 1},
    {60000000,  80000000,  4, 4, 1},
    {80000000,  250000000, 4, 2, 1},
    {250000000, 500000000, 2, 2, 1},
    {500000000, 800000000, 2, 1, 1},
};

#define CLK_CALC_RADIO 1000
hi_void vdp_crg_hpll_get_reg(hi_u32 hpll_clk, hi_u32 *ctrl0, hi_u32 *ctrl1)
{
    hi_u32 int_part;
    hi_u32 minor_part;
    hi_u64 tmp_minor_part;
    hi_u32 i = 0;
    hi_u32 posdiv1 = 1;
    hi_u32 posdiv2 = 1;
    hi_u32 refdiv = 1;
    hi_u32 pll_clk_unit;

    for (i = 0; i < sizeof(g_div_table) / sizeof(g_div_table[0]); i++) {
        if ((hpll_clk * CLK_CALC_RADIO > g_div_table[i].hpll_clk_min) &&
            (hpll_clk * CLK_CALC_RADIO <= g_div_table[i].hpll_clk_max)) {
            posdiv1 = g_div_table[i].posdiv1;
            posdiv2 = g_div_table[i].posdiv2;
            refdiv = g_div_table[i].refdiv;
            break;
        }
    }

    if (i == sizeof(g_div_table) / sizeof(g_div_table[0])) {
        VDP_PRINT("hpll warning clk = %d not in table!\n", hpll_clk);
    }

    pll_clk_unit = HPLL_REF_CLK / (posdiv1 * posdiv2 * refdiv);
    int_part = hpll_clk / pll_clk_unit;
    int_part |= 0xBF00000 | (refdiv << 12); /* 12 is a offset */
    minor_part = hpll_clk % pll_clk_unit;
    tmp_minor_part = ((hi_u64)minor_part) * (1 << 24); /* 24 is a offset */
    minor_part = (hi_u32)osal_div_u64(tmp_minor_part, pll_clk_unit);
    minor_part |= (posdiv1 << 24) | (posdiv2 << 28); /* 24 is a offset 28 is a offset */

    *ctrl0 = minor_part;
    *ctrl1 = int_part;

    return;
}

hi_u32 vdp_crg_get_hpll_sel(vdp_dispchn_chn chn_id, hi_drv_disp_intf *intf_attr)
{
    hi_u32 hpll_sel = HPLL1_LINK;
#ifdef VDP_SUPPORT_PANEL
    return HPLL0_LINK;
#endif
    if (chn_id == VDP_DISPCHN_CHN_DHD0) {
        hpll_sel = HPLL1_LINK;
        if ((intf_attr->intf_type == HI_DRV_DISP_INTF_TYPE_HDMI)
            && (intf_attr->un_intf.hdmi == HI_DRV_HDMI_ID_1)) { /* sel hdmi 2.0 */
            hpll_sel = HPLL0_LINK;
        }
    } else if (chn_id == VDP_DISPCHN_CHN_DHD1) {
        hpll_sel = HPLL0_LINK;
        if ((intf_attr->intf_type == HI_DRV_DISP_INTF_TYPE_HDMI)
            && (intf_attr->un_intf.hdmi == HI_DRV_HDMI_ID_0)) { /* sel hdmi 2.1 */
            hpll_sel = HPLL1_LINK;
        }
    }
    return hpll_sel;
}
#ifdef HI_TEE_SUPPORT
#define  ADDR_PERI_CRG_PLL12              0x00A00030
#define  ADDR_PERI_CRG_PLL13              0x00A00034
#define  ADDR_PERI_CRG_PLL15              0x00A0003c
#define  ADDR_PERI_CRG_PLL16              0x00A00040
#endif
hi_void vdp_crg_hpll_set_reg(vdp_dispchn_chn chn_id, hi_drv_disp_intf *intf_attr,
                             hi_u32 ctrl0, hi_u32 ctrl1)
{
    hi_u32 hpll_sel = vdp_crg_get_hpll_sel(chn_id, intf_attr);
    if (hpll_sel == HPLL0_LINK) {
        U_PERI_CRG_PLL12 PERI_CRG_PLL12;
        U_PERI_CRG_PLL13 PERI_CRG_PLL13;
        PERI_CRG_PLL12.u32 = g_vdp_crg_reg->PERI_CRG_PLL12.u32;
        PERI_CRG_PLL13.u32 = g_vdp_crg_reg->PERI_CRG_PLL13.u32;
        /* if hpll clk not change , not need to power down hpll ,set new clk */
        if ((PERI_CRG_PLL12.u32 == ctrl0) && (PERI_CRG_PLL13.u32 == ctrl1)) {
            return;
        }
        /* hpll clk power down */
        PERI_CRG_PLL12.u32 = ctrl0;
        PERI_CRG_PLL13.u32 = ctrl1 & (~HLL_POWER_DOWN_BIT); /* 23 is a offset */

#ifdef HI_TEE_SUPPORT
        hi_drv_mbx_write_hrf_reg(ADDR_PERI_CRG_PLL12, PERI_CRG_PLL12.u32);
        hi_drv_mbx_write_hrf_reg(ADDR_PERI_CRG_PLL13, PERI_CRG_PLL13.u32);
#else
        g_vdp_crg_reg->PERI_CRG_PLL12.u32 = PERI_CRG_PLL12.u32;
        g_vdp_crg_reg->PERI_CRG_PLL13.u32 = PERI_CRG_PLL13.u32;
#endif
        osal_msleep_uninterruptible(1);
        /* set new hpll clk */
        PERI_CRG_PLL12.u32 = g_vdp_crg_reg->PERI_CRG_PLL12.u32;
        PERI_CRG_PLL13.u32 = g_vdp_crg_reg->PERI_CRG_PLL13.u32;

        PERI_CRG_PLL12.u32 = ctrl0;
        PERI_CRG_PLL13.u32 = ctrl1;

#ifdef HI_TEE_SUPPORT
        hi_drv_mbx_write_hrf_reg(ADDR_PERI_CRG_PLL12, PERI_CRG_PLL12.u32);
        hi_drv_mbx_write_hrf_reg(ADDR_PERI_CRG_PLL13, PERI_CRG_PLL13.u32);
#else
        g_vdp_crg_reg->PERI_CRG_PLL12.u32 = PERI_CRG_PLL12.u32;
        g_vdp_crg_reg->PERI_CRG_PLL13.u32 = PERI_CRG_PLL13.u32;
#endif

    } else {
        U_PERI_CRG_PLL15 PERI_CRG_PLL15;
        U_PERI_CRG_PLL16 PERI_CRG_PLL16;
        PERI_CRG_PLL15.u32 = g_vdp_crg_reg->PERI_CRG_PLL15.u32;
        PERI_CRG_PLL16.u32 = g_vdp_crg_reg->PERI_CRG_PLL16.u32;
        /* if hpll clk not change , not need to power down hpll ,set new clk */
        if ((PERI_CRG_PLL15.u32 == ctrl0) && (PERI_CRG_PLL16.u32 == ctrl1)) {
            return;
        }
        /* hpll clk power down */
        PERI_CRG_PLL15.u32 = ctrl0;
        PERI_CRG_PLL16.u32 = ctrl1 & (~HLL_POWER_DOWN_BIT); /* 23 is a offset */

#ifdef HI_TEE_SUPPORT
        hi_drv_mbx_write_hrf_reg(ADDR_PERI_CRG_PLL15, PERI_CRG_PLL15.u32);
        hi_drv_mbx_write_hrf_reg(ADDR_PERI_CRG_PLL16, PERI_CRG_PLL16.u32);
#else
        g_vdp_crg_reg->PERI_CRG_PLL15.u32 = PERI_CRG_PLL15.u32;
        g_vdp_crg_reg->PERI_CRG_PLL16.u32 = PERI_CRG_PLL16.u32;
#endif

        osal_msleep_uninterruptible(1);
        /* set new hpll clk */
        PERI_CRG_PLL15.u32 = g_vdp_crg_reg->PERI_CRG_PLL15.u32;
        PERI_CRG_PLL16.u32 = g_vdp_crg_reg->PERI_CRG_PLL16.u32;

        PERI_CRG_PLL15.u32 = ctrl0;
        PERI_CRG_PLL16.u32 = ctrl1;
#ifdef HI_TEE_SUPPORT
        hi_drv_mbx_write_hrf_reg(ADDR_PERI_CRG_PLL15, PERI_CRG_PLL15.u32);
        hi_drv_mbx_write_hrf_reg(ADDR_PERI_CRG_PLL16, PERI_CRG_PLL16.u32);
#else
        g_vdp_crg_reg->PERI_CRG_PLL15.u32 = PERI_CRG_PLL15.u32;
        g_vdp_crg_reg->PERI_CRG_PLL16.u32 = PERI_CRG_PLL16.u32;
#endif

    }

    VDP_DEBUG("-debug->set  hpll %d ctrl0 %x ctrl1 %x intfen %d\n", hpll_sel, ctrl0, ctrl1,
    vdp_ip_dispchn_get_enable(chn_id));
    return;
}

hi_void vdp_crg_hpll_clk_set_div(vdp_dispchn_chn chn_id, hi_drv_disp_intf *intf_attr,
                                 hi_u32 hpll_div)
{
    /* 1 hpll->vdp_clk_div(n)->->vo_hd_clk_div(1/4/8)->vo_hd_hdmi_clk_sel(1/2) */
    hi_u32 vdp_clk_div = 0;
    hi_u32 vo_hd_clk_div = VO_HD_CLK_DIV_1;
    hi_u32 vo_hd_hdmi_clk_sel = VO_HD_HDMI_CLK_DIV_1;
    hi_u32 hpll_sel = vdp_crg_get_hpll_sel(chn_id, intf_attr);
    /* 2 ppc-> vdp_hd_clk_div(2/4)-> vdp_ini_cksel(mux) */
    U_PERI_CRG345 PERI_CRG345;
    U_PERI_CRG346 PERI_CRG346;
    PERI_CRG345.u32 = g_vdp_crg_reg->PERI_CRG345.u32;
    PERI_CRG346.u32 = g_vdp_crg_reg->PERI_CRG346.u32;

    if ((hpll_div == HPLL_CLK_DIV_4) &&
        (intf_attr->intf_type == HI_DRV_DISP_INTF_TYPE_CVBS)) {
        vo_hd_clk_div = VO_HD_CLK_DIV_4;
    } else if (hpll_div == HPLL_CLK_DIV_2) {
        vdp_clk_div = 1; /* 1 is number 2 div */
    } else if (hpll_div == HPLL_CLK_DIV_4) {
        vdp_clk_div = 3; /* 3 is number 4 div */
    } else if (hpll_div == HPLL_CLK_DIV_8) {
        vdp_clk_div = 7; /* 7 is number 8 div */
    }

    if (chn_id == VDP_DISPCHN_CHN_DHD0) {
        PERI_CRG345.bits.vo_hd_clk_div = vo_hd_clk_div;
        PERI_CRG345.bits.vo_hd_hdmi_clk_sel = vo_hd_hdmi_clk_sel;
    } else if (chn_id == VDP_DISPCHN_CHN_DHD1) {
        PERI_CRG346.bits.vo_hd1_clk_div = vo_hd_clk_div;
        PERI_CRG346.bits.vo_hd1_hdmi_clk_sel = vo_hd_hdmi_clk_sel;
    }

    if (hpll_sel == HPLL1_LINK) {
        PERI_CRG346.bits.vdp1_clk_div = vdp_clk_div;
    } else {
        PERI_CRG346.bits.vdp0_clk_div = vdp_clk_div;
    }

    g_vdp_crg_reg->PERI_CRG345.u32 = PERI_CRG345.u32;
    g_vdp_crg_reg->PERI_CRG346.u32 = PERI_CRG346.u32;

    VDP_DEBUG("vo_hd_clk_div %x vo_hd_hdmi_clk_sel %x vdp_clk_div  %x\n",
        vo_hd_clk_div, vo_hd_hdmi_clk_sel, vdp_clk_div);
    VDP_DEBUG("-debug-> 345 %x 346 %x\n", PERI_CRG345.u32, PERI_CRG346.u32);
    return;
}

/* use intf enable to check if need reset */
hi_bool vdp_crg_check_need_reset(hi_void)
{
    if (vdp_ip_dispchn_get_enable(VDP_DISPCHN_CHN_DHD0) == HI_TRUE) {
        return HI_FALSE;
    } else {
        return HI_TRUE;
    }
}

hi_void vdp_crg_clk0_init(hi_bool enable)
{
    U_PERI_CRG345 PERI_CRG345;
    PERI_CRG345.u32 = g_vdp_crg_reg->PERI_CRG345.u32;
    PERI_CRG345.bits.vo_hd_clk_sel = HPLL1_LINK;
    PERI_CRG345.bits.vo_hd_clk_div = 0x2; /* 0:div4 1:div8 2:div1 */
    PERI_CRG345.bits.vdp_ppc_cken = enable;
    PERI_CRG345.bits.vdp_ini_cksel = 0;      /* 0:div1 1:div2 */
    PERI_CRG345.bits.vo_hd_hdmi_clk_sel = 0; /* 0:div1 1:div2 */
    PERI_CRG345.bits.vdp_hd_clk_div = 0;     /* 0:div2 1:div4 */
    PERI_CRG345.bits.vo_hd_cken = enable;
    PERI_CRG345.bits.vo_sdate_cken = enable;
    PERI_CRG345.bits.vo_sd_cken = enable;
    PERI_CRG345.bits.vdp_apb_cken = enable;
    PERI_CRG345.bits.vdp_axi_cken = enable;
    PERI_CRG345.bits.vdp_hdmi_clk_sel = 0; /* 0:hd 1:hd1 */
    PERI_CRG345.bits.vdp_hdmi_cken = enable;
    PERI_CRG345.bits.vdp_mipi_clk_sel = 0; /* 0:hd 1:hd1 */
    PERI_CRG345.bits.vdp_mipi_cken = enable;
    PERI_CRG345.bits.vdp_bt_ch_sel = 0; /* 0:hd 1:hd1 */
    PERI_CRG345.bits.vdp_bt_cken = enable;
    PERI_CRG345.bits.vdp_bt_bp_cken = enable;
    PERI_CRG345.bits.vdp_out_pctrl = 1;     /* 0:Forward 1:Reverse */
    PERI_CRG345.bits.vdp_hdmi1_clk_sel = 1; /* 0:hd 1:hd1 */
    PERI_CRG345.bits.vdp_hdmi1_cken = enable;
    PERI_CRG345.bits.vdp_cksel = 0; /* 0:648M 1:600M 2:629M */
    PERI_CRG345.bits.vdp_mmu0_srst_req = 0;
    PERI_CRG345.bits.vdp_mmu1_srst_req = 0;
    PERI_CRG345.bits.vdp_mmu2_srst_req = 0;
    g_vdp_crg_reg->PERI_CRG345.u32 = PERI_CRG345.u32;
}

hi_void vdp_crg_clk1_init(hi_bool enable)
{
    U_PERI_CRG346 PERI_CRG346;
    PERI_CRG346.u32 = g_vdp_crg_reg->PERI_CRG346.u32;
    PERI_CRG346.bits.vo_hd1_clk_sel = HPLL0_LINK;
    PERI_CRG346.bits.vo_hd1_clk_div = 0x2; /* 0:div4 1:div8 2:div1 */
    PERI_CRG346.bits.vdp_v0_srst_req = 0;
    PERI_CRG346.bits.vdp_core_srst_req = 0;
    PERI_CRG346.bits.vdp_disp_srst_req = 0;
    PERI_CRG346.bits.vo_hd1_hdmi_clk_sel = 0; /* hd1_com_ini 0:div1 1:div2 */
    PERI_CRG346.bits.vdp_srst_req = 0;
    PERI_CRG346.bits.vo_hd1_cken = enable;
    PERI_CRG346.bits.vdp0_srst_req = 0;
    PERI_CRG346.bits.vdp1_srst_req = 0;
    PERI_CRG346.bits.vdp0_clk_div = 0; /* vdp clk div(n+1) */
    PERI_CRG346.bits.vdp1_clk_div = 0; /* vdp clk div(n+1) */
    PERI_CRG346.bits.vdp_hdr_srst_req = 0;
    PERI_CRG346.bits.vdp_sr0_srst_req = 0;
    PERI_CRG346.bits.vdp_sr1_srst_req = 0;
    PERI_CRG346.bits.vdp_sr2_srst_req = 0;
    PERI_CRG346.bits.vdp_core_axi_cken = enable;
    PERI_CRG346.bits.vdp_v0_axi_cken = enable;
    PERI_CRG346.bits.vdp_hd_cken = enable;
    PERI_CRG346.bits.lvds_pwm_cken = enable;
    g_vdp_crg_reg->PERI_CRG346.u32 = PERI_CRG346.u32;
}

hi_void vdp_crg_dac_clk_init(hi_bool enable)
{
    U_PERI_CRG358 PERI_CRG358;
    U_PERI_CRG359 PERI_CRG359;
    PERI_CRG358.u32 = g_vdp_crg_reg->PERI_CRG358.u32;
    PERI_CRG359.u32 = g_vdp_crg_reg->PERI_CRG359.u32;
    PERI_CRG358.bits.vdac_ch0_cken = enable;
    PERI_CRG358.bits.vdac_ch1_cken = enable;
    PERI_CRG358.bits.vdac_ch2_cken = enable;
    PERI_CRG358.bits.vdac_ch3_cken = enable;
    PERI_CRG358.bits.vdac_ch0_clk_sel = 0; /* hd0 */
    PERI_CRG358.bits.vdac_ch1_clk_sel = 0; /* hd0 */
    PERI_CRG358.bits.vdac_ch2_clk_sel = 0; /* hd0 */
    PERI_CRG358.bits.vdac_ch3_clk_sel = 0; /* hd0 */
    PERI_CRG359.bits.vdac_chop_cken = 1;
    PERI_CRG359.bits.vdac_0_clk_pctrl = 0; /* 0:Forward 1:Reverse */
    PERI_CRG359.bits.vdac_1_clk_pctrl = 0; /* 0:Forward 1:Reverse */
    PERI_CRG359.bits.vdac_2_clk_pctrl = 0; /* 0:Forward 1:Reverse */
    PERI_CRG359.bits.vdac_3_clk_pctrl = 0; /* 0:Forward 1:Reverse */
    g_vdp_crg_reg->PERI_CRG358.u32 = PERI_CRG358.u32;
    g_vdp_crg_reg->PERI_CRG359.u32 = PERI_CRG359.u32;
}

#define PIX_CLK_600M  600000
#define PIX_CLK_1200M 1200000
#define PIX_CLK_2400M 2400000

/* intf_pix = pix_clk / intf_multi_pix hpll_pix = intf_pix * hpll_clk_div */
hi_void vdp_ip_crg_set_chn_clk(vdp_dispchn_chn chn_id, hi_drv_disp_intf *intf_attr,
                               vdp_disp_crg *crg_info)
{
    hi_u32 hpll_clk_div, intf_clk_div, hpll_clk, ctrl0, ctrl1;

    if (crg_info->multichn_en == VDP_DISPCHN_MULTICHN_8_PIXEL) {
        intf_clk_div = 8; /* 8  is a div */
    } else if (crg_info->multichn_en == VDP_DISPCHN_MULTICHN_4_PIXEL) {
        intf_clk_div = 4; /* 4  is a div */
    } else if (crg_info->multichn_en == VDP_DISPCHN_MULTICHN_2_PIXEL) {
        intf_clk_div = 2; /* 2  is a div */
    } else {
        intf_clk_div = 1;
    }

#ifdef VDP_SUPPORT_PANEL
    hpll_clk_div = HPLL_CLK_DIV_1;
#else
    if (intf_attr->intf_type == HI_DRV_DISP_INTF_TYPE_CVBS) {
        hpll_clk_div = HPLL_CLK_DIV_4; /* cvbs dac need 13.5M * 4 = 54M */
    } else if (intf_attr->intf_type == HI_DRV_DISP_INTF_TYPE_HDMI) {
        if (intf_attr->un_intf.hdmi == HI_DRV_HDMI_ID_1) { /* hdmi2.0 */
            hpll_clk_div = HPLL_CLK_DIV_1;
        } else { /* hdmi2.1 */
            if (crg_info->pixel_clk < PIX_CLK_600M) {
                hpll_clk_div = HPLL_CLK_DIV_8;
            } else if (crg_info->pixel_clk < PIX_CLK_1200M) {
                hpll_clk_div = HPLL_CLK_DIV_2; /* hdmi need 297M hpll out */
            } else if (crg_info->pixel_clk < PIX_CLK_2400M) {
                hpll_clk_div = HPLL_CLK_DIV_2;
            } else {
                hpll_clk_div = HPLL_CLK_DIV_1;
            }
        }
    } else if (intf_attr->intf_type == HI_DRV_DISP_INTF_TYPE_MIPI) {
        hpll_clk_div = HPLL_CLK_DIV_1;
    } else {
        hpll_clk_div = HPLL_CLK_DIV_1;
        VDP_PRINT("no support type %d now\n", intf_attr->intf_type);
    }
#endif

    /* 1. hpll clk = intf_clk * intf_clk_div */
    hpll_clk = crg_info->pixel_clk * hpll_clk_div / intf_clk_div;

    vdp_crg_hpll_get_reg(hpll_clk, &ctrl0, &ctrl1);
    vdp_crg_hpll_set_reg(chn_id, intf_attr, ctrl0, ctrl1);
    vdp_crg_hpll_clk_set_div(chn_id, intf_attr, hpll_clk_div);
    VDP_DEBUG("--->test set chn %d intftype %d intf_hdmiid %d fmt %d pix %d hpll %d\n",
              chn_id, intf_attr->intf_type, intf_attr->un_intf.hdmi, crg_info->disp_fmt,
              crg_info->pixel_clk, hpll_clk);
    return;
}

hi_void vdp_ip_crg_work_clk_enable(hi_bool enable)
{
    if (enable == HI_FALSE) {
        vdp_crg_clk0_init(HI_FALSE);
        vdp_crg_clk1_init(HI_FALSE);
        vdp_crg_dac_clk_init(HI_FALSE);
        return;
    }

    /* check need reset for init clk */
    if (vdp_crg_check_need_reset() == HI_TRUE) {
        vdp_crg_clk0_init(HI_TRUE);
        vdp_crg_clk1_init(HI_TRUE);
        vdp_crg_dac_clk_init(HI_TRUE);
    }

    return;
}

hi_void vdp_ip_crg_soft_reset(hi_void)
{
    U_PERI_CRG346 PERI_CRG346;
    PERI_CRG346.u32 = g_vdp_crg_reg->PERI_CRG346.u32;
    PERI_CRG346.bits.vdp_srst_req = 1;
    g_vdp_crg_reg->PERI_CRG346.u32 = PERI_CRG346.u32;
    osal_msleep_uninterruptible(VDP_RESET_SLEEP_MS);
    PERI_CRG346.u32 = g_vdp_crg_reg->PERI_CRG346.u32;
    PERI_CRG346.bits.vdp_srst_req = 0;
    g_vdp_crg_reg->PERI_CRG346.u32 = PERI_CRG346.u32;

    return;
}

hi_s32 vdp_ip_module_clk_enable(clock_module module, hi_bool enable)
{
    // :TODO:for crg update
    switch (module) {
        case CLOCK_MODULE_VID0:
            break;
        case CLOCK_MODULE_VID1:
            break;
        case CLOCK_MODULE_VID3:
            break;
        case CLOCK_MODULE_VP0:
            break;
        case CLOCK_MODULE_WBC0:
            break;
        default:
            VDP_PRINT("no support mode %d\n", module);
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


