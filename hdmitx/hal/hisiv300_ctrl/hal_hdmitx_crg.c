/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: source file of driver hdmitx crg
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#include <linux/hisilicon/securec.h>
#ifdef HI_HDMI_TEE_SUPPORT
#include <linux/hisilicon/hi_drv_mbx.h>
#endif
#include "hi_osal.h"
#include "drv_hdmitx.h"
#include "hal_hdmitx_io.h"
#include "hal_hdmitx_crg.h"
#include "hal_hdmitx_ctrl_reg.h"

#define PPLL0_CRG_ADDR              0x00A00590
#define PPLL1_CRG_ADDR              0x00A00594

#ifdef HI_HDMI_TEE_SUPPORT
#define PERI_CRG_PLL0               0x00A00078
#define PERI_CRG_PLL1               0x00A00084
#endif
/*
 * Enable crg clk
 * The following information should be considered.
 * 0~5 Bits of Ctr0 is the same as Ctrl1's;
 * PERI_CRG354: hdmi2.0 ctrl reg
 * [12]hdmitx20_pxld4_cken
 * PERI_CRG355: hdmi2.1 ctrl reg
 * [20]hdmitx21_pxld4_cken
 * [19]hdmitx21_pxl_cken
 * [7]hdmitx21_dsc_cken
 * PERI_CRG354: PERI_CRG355
 * [5]ppll_tmds_cken
 * [2]hdmitx_ctrl_as_cken
 * [1]hdmitx_ctrl_os_cken
 * [0]hdmitx_ctrl_osc_24m_cken
 */
#define hdmitx_ctrl0_pxld4_cken(x) (((x) & 0x1) << 12)
#define HDMITX_CTRL0_PXLD4_CKEN_M  (0x1 << 12)

#define hdmitx_ctrl1_pxld4_cken(x) (((x) & 0x1) << 20)
#define HDMITX_CTRL1_PXLD4_CKEN_M  (0x1 << 20)
#define hdmitx_ctrl1_pxl_cken(x) (((x) & 0x1) << 19)
#define HDMITX_CTRL1_PXL_CKEN_M  (0x1 << 19)
#define hdmitx_ctrl1_dsc_cken(x) (((x) & 0x1) << 7)
#define HDMITX_CTRL1_DSC_CKEN_M  (0x1 << 7)

#define hdmitx_ctrl_ppll_tmds_cken(x) (((x) & 0x1) << 5)
#define HDMITX_CTRL_PPLL_TMDS_CKEN_M  (0x1 << 5)
#define hdmitx_ctrl_as_cken(x) (((x) & 0x1) << 2)
#define HDMITX_CTRL_AS_CKEN_M  (0x1 << 2)
#define hdmitx_ctrl_os_cken(x) (((x) & 0x1) << 1)
#define HDMITX_CTRL_OS_CKEN_M  (0x1 << 1)
#define hdmitx_ctrl_osc_24m_cken(x) (((x) & 0x1) << 0)
#define HDMITX_CTRL_OSC_24M_CKEN_M  (0x1 << 0)

/*
 * Enable crg clk
 * The following information should be considered.
 * PERI_CRG356: phy20 reg
 * PERI_CRG357: phy21 reg
 * [2]ac_ctrl_modclk_cken
 * [1]hdmitx_phy_modclk_cken
 * [0]hdmitx_phy_tmds_cken
 */
#define hdmitx_phy_ac_ctrl_modclk_cken(x) (((x) & 0x1) << 2)
#define HDMITX_PHY_AC_CTRL_MODCLK_CKEN_M  (0x1 << 2)
#define hdmitx_phy_modclk_cken(x) (((x) & 0x1) << 1)
#define HDMITX_PHY_MODCLK_CKEN_M  (0x1 << 1)
#define hdmitx_phy_tmds_cken(x) (((x) & 0x1) << 0)
#define HDMITX_PHY_TMDS_CKEN_M  (0x1 << 0)

#define PHY20_RST_BASE 0x0107f010
#define PHY21_RST_BASE 0x0103f010
#define phy_rst(x)     (((x) & 0x1) << 0)
#define PHY_RST_MASK   (0x1 << 0)

#define hdmitx20_pxld4_clk_div(x)   (((x) & 0x1f) << 7)
#define HDMITX20_PXLD4_CLK_DIV_MASK (0x1f << 7)

#define hdmitx21_pxld4_clk_div(x)   (((x) & 0x1f) << 9)
#define HDMITX21_PXLD4_CLK_DIV_MASK (0x1f << 9)

#define hdmitx21_ppll_out(x)   (((x) & 0x3) << 9)
#define HDMITX21_PPLL_OUT_MASK (0x3 << 9)

#define PPLL_LOCK_SHIFT 28

#define PPLL_1_OFFSET    0
#define ppll_postdiv1(x) (((x) & 0x7) << 24)
#define PPLL_POSTDIV1_M  (0x7 << 24)
#define ppll_postdiv2(x) (((x) & 0x7) << 28)
#define PPLL_POSTDIV2_M  (0x7 << 28)

#define PPLL_2_OFFSET  0x4
#define ppll_en(x) (((x) & 0x1) << 20)
#define PPLL_EN_M  (0x1 << 20)
#define ppll_refdiv(x) (((x) & 0x3f) << 12)
#define PPLL_REFDIV_M  (0x3f << 12)
#define ppll_fbdiv(x)  (((x) & 0xfff) << 0)
#define PPLL_FBDIV_M   (0xfff << 0)

#define TPLL_REF_CLK_CKSEL_MSAK (0x1 << 11)

#define PLL_LOCK_BASE_REG   0xa00860
#define PPLL0_TX20_LOCK_STA_MASK  (0x1 << 10)
#define PPLL1_TX21_LOCK_STA_MASK  (0x1 << 9)

struct hdmi_crg_reg_config {
    int dsc_clk_sel;
    int vidpath_clk_sel;
    int vidpath_dout_clk_sel;
    int clk_tmds_source_sel;
    int audio_clk_gen_sel;
    int audpath_acr_clk_sel;
    int clk_frl_source_sel;
};

struct hdmi_crg_ppll_attr {
    hi_u32 fbdiv;
    hi_u32 refdiv;
    hi_u32 postdiv1;
    hi_u32 postdiv2;
    hi_u32 ppll_out_sel;
};

struct hdmi_crg_reg_config g_hdmi_crg_default_configs[] = {
    /*
     *  TMDS mode(0~600M):
     *  1. Video path
     */
    {
        .dsc_clk_sel = 0,
        .vidpath_clk_sel = 0,
        .vidpath_dout_clk_sel = 0,
        .clk_tmds_source_sel = 0,
        .audio_clk_gen_sel = 0,
        .audpath_acr_clk_sel = 1,
        .clk_frl_source_sel = 0
    },
    /*
     * frl nodsc mode(0~2.4G):
     * 1. videopath clock is from hpll 1d4
     * 2.   (1) phy ref clock is from hpll 1d4
     *      (2) hdmi top clock is from phy(tmds_1d4 which matchs ref 1d4)
     * 3. frl top clock from phy frl
     * 4. audio default i2s and acr clock from frl.
     * 5. phy clock is configured in the following phy_config.
     * note1:
     *   when pixel rate 0~600m and 600m~2.4g.
     *   when final pixel format is 420(original is alway 444).
     * the 1d4 divider is different.
     * note2:
     *       we can choose ppll as the default clock source instead of phy.
     *       then, the clk_tmds_source_sel should be 1.
     *       and ppll should be configured as the 1d4 tmds clock which
     *       matches the 1d4 pixel clock.it also needs to consider the
     *       600~2.4g and 420.
     */
    {
        .dsc_clk_sel = 0,
        .vidpath_clk_sel = 0,
        .vidpath_dout_clk_sel = 1,
        .clk_tmds_source_sel = 0,
        .audio_clk_gen_sel = 0,
        .audpath_acr_clk_sel = 0,
        .clk_frl_source_sel = 0
    },
    /*
     * frl dsc mode(600M~2.4G, 2.4G+):
     * 1. videopath clock is from ppll or phy default
     * 2.   (1) phy ref clock is from 1d8
     *      (2) hdmi top clock is the same with videopath.
     * 3. frl top clock from phy frl
     * 4. audio default i2s and acr clock from frl.
     * 5. phy clock is configured in the following phy_config.
     * note1:
     *  The ppll out or phy out should match the dsc compressed pixel
     *  clock. No need to consider 10bit.
     *  if the ppll is the source, then we have to config hdmi
     *  FREQDIV and the ppll divider to get a match compress rate.
     *  if the phy is the source, only need to config the phy tmds
     *  divider.
     */
    {
        .dsc_clk_sel = 0,
        .vidpath_clk_sel = 1,
        .vidpath_dout_clk_sel = 1,
        .clk_tmds_source_sel = 1,
        .audio_clk_gen_sel = 0,
        .audpath_acr_clk_sel = 0,
        .clk_frl_source_sel = 0
    }
};

static hi_bool get_ppll_lock_status(struct hdmi_crg *crg)
{
    hi_bool ret = HI_FALSE;
    struct hisilicon_hdmi *hdmi = crg->parent;
    void __iomem *crg_reg = HI_NULL;
    u32 crg_addr;

    crg_addr = PLL_LOCK_BASE_REG;

    crg_reg = osal_ioremap_nocache(crg_addr, 4); /* 4 Bytes */
    if (IS_ERR(crg_reg)) {
        HDMI_ERR("Failed to get memory resource\n");
        return ret;
    }

    if (hdmi->id == 0) {
        ret = !!(hdmi_readl(crg_reg, 0) & PPLL1_TX21_LOCK_STA_MASK);
    } else {
        ret = !!(hdmi_readl(crg_reg, 0) & PPLL0_TX20_LOCK_STA_MASK);
    }

    osal_iounmap(crg_reg);

    return ret;
}

static void hdmi_crg_hw_config(struct hdmi_crg *crg,
                               struct hdmi_crg_reg_config *reg_config, int encoder_mode)
{
    int value;

    hdmi_clrset(crg->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_DSC_CLK_SEL_M,
                reg_dsc_clk_sel(reg_config->dsc_clk_sel));

    hdmi_clrset(crg->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_VIDPATH_CLK_SEL_M,
                reg_vidpath_clk_sel(reg_config->vidpath_clk_sel));

    hdmi_clrset(crg->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_VIDPATH_DOUT_CLK_SEL_M,
                reg_vidpath_dout_clk_sel(reg_config->vidpath_dout_clk_sel));

    hdmi_clrset(crg->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_CLK_TMDS_SOURCE_SEL_M,
                reg_clk_tmds_source_sel(reg_config->clk_tmds_source_sel));

    hdmi_clrset(crg->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_AUD_CLK_GEN_SEL_M,
                reg_aud_clk_gen_sel(reg_config->audio_clk_gen_sel));

    hdmi_clrset(crg->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_AUDPATH_ACR_CLK_SEL_M,
                reg_audpath_acr_clk_sel(reg_config->audpath_acr_clk_sel));

    hdmi_clrset(crg->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_CLK_FRL_SOURCE_SEL_M,
                reg_clk_frl_source_sel(reg_config->clk_frl_source_sel));

    /*
     * config audio_i2s_en and pxl_div_en should be enabled in
     * audiopath
     * Enable the phy crg clock gates here, including:
     * hdmitx_phy1_modclk_cken: for dphy 24M work clock.
     * hdmitx_phy1_tmds_cken: for output tmds clock.
     * ac_ctrl1_modclk_cken: not used but we still enable it, remove after
     * chipback.
     */
    value = hdmi_readl(crg->phy_crg_regs, 0);
    value |= 0x7;
    hdmi_writel(crg->phy_crg_regs, 0, value);

    /* set tpll refer clock sel */
    if (encoder_mode == 0) {
        hdmi_clr(crg->phy_crg_regs, 0, TPLL_REF_CLK_CKSEL_MSAK);
    } else {
        hdmi_set(crg->phy_crg_regs, 0, TPLL_REF_CLK_CKSEL_MSAK);
    }
}

/* Note : ppll_out_sel postdiv(0) fout1 1 fout2 2 */
static void cal_frl_dsc_ppll_setting(int pixel_clk, bool yuv_420, struct hdmi_crg_ppll_attr *ppll_attr)
{
    if ((pixel_clk > 600000) && (pixel_clk <= 2000000)) { /* 600000Hz~2000000Hz */
        ppll_attr->refdiv = 1;
        ppll_attr->fbdiv = 48;
        ppll_attr->postdiv1 = 3;
        ppll_attr->postdiv2 = 2;
    } else if ((pixel_clk > 2000000) && (pixel_clk <= 2400000)) { /* 2000000Hz~2400000Hz */
        ppll_attr->refdiv = 1;
        ppll_attr->fbdiv = 16;
        ppll_attr->postdiv1 = 2;
        ppll_attr->postdiv2 = 1;
    } else if ((pixel_clk > 2400000) && (pixel_clk <= 4800000)) { /* 2400000Hz~4800000Hz */
        ppll_attr->refdiv = 1;
        ppll_attr->fbdiv = 32;
        ppll_attr->postdiv1 = 2;
        ppll_attr->postdiv2 = 1;
    } else {
        HDMI_ERR("have no more than 48G clock! default to 600M~2G\n");
        ppll_attr->refdiv = 1;
        ppll_attr->fbdiv = 48;
        ppll_attr->postdiv1 = 3;
        ppll_attr->postdiv2 = 2;
    }

    if (yuv_420) {
        ppll_attr->ppll_out_sel = 1;    /* fout1 */
    } else {
        ppll_attr->ppll_out_sel = 0;    /* postdiv */
    }
}

static void cal_frl_nodsc_ppll_setting(int pixel_clk, bool yuv_420,
                                       int color_depth, struct hdmi_crg_ppll_attr *ppll_attr)
{
    int pixel_pll_clk;

    if (pixel_clk > 4800000) { /* 4800000Hz */
        HDMI_ERR("Not supported pixel rate higher than 48G, \
            default to 600M\n");
        pixel_pll_clk = 600000; /* 600000Hz */
    }

    if (pixel_clk <= 600000) { /* 600000Hz */
        pixel_pll_clk = pixel_clk;
    } else if (pixel_clk > 600000 && pixel_clk <= 2400000) { /* 600000Hz~2400000Hz */
        pixel_pll_clk = pixel_clk / 4;
    } else {
        pixel_pll_clk = pixel_clk / 8;
    }

    /*
     *  formula:
     *  vco = fbdiv * pixel_pll_clk/refdiv
     *  vco = foutpostdiv * postdiv1 * postdiv2
     *  800 <= vco <= 3200
     *  96cv300 clock design(frl + nodsc):
     *  pixel_clk: video format pixel clk defined by vesa/cea
     *  pixel_pll_clk: ppll input refclk
     *  pixel_pll_clk = pixel_clk * 1/8 (pixel_clk 0~600M)
     *  pixel_pll_clk = pixel_clk * 1/4 * 1/8 (pixel_clk 600M~2400M)
     *  target_clk:  ppll output clk for hdmi
     *  target_clk = pixel_clk * 1/4 * (YUV420 ? 1/2 : 1) * (color_depth / 8)
     *  Base on the above formula:
     *  if 0 < pixel <= 600m
     *  target_clk = fout2
     *  if 600m < pixel <= 2400m
     *  if (yuv420)
     *      target_clk = fout1
     *  else
     *      target_clk = foutpostdiv
     */
    if ((pixel_pll_clk >= 25000) && (pixel_pll_clk <= 59000)) { /* 25000Hz~59000Hz */
        /*
         *  fbdiv * 25 > 800
         *  fbdiv * 59 * 1.5(12bit) <=3200
         */
        ppll_attr->refdiv = 0x1;
        ppll_attr->postdiv1 = 0x6;
        ppll_attr->postdiv2 = 0x6;
        ppll_attr->fbdiv = 0x24 * color_depth;
    } else if ((pixel_pll_clk > 59000) && (pixel_pll_clk <= 133000)) { /* 59000Hz~133000Hz */
        ppll_attr->refdiv = 0x1;
        ppll_attr->postdiv1 = 0x4;
        ppll_attr->postdiv2 = 0x4;
        ppll_attr->fbdiv = 0x10 * color_depth;
    } else if ((pixel_pll_clk > 113000) && (pixel_pll_clk <= 266000)) { /* 133000Hz~266000Hz */
        ppll_attr->refdiv = 0x1;
        ppll_attr->postdiv1 = 0x4;
        ppll_attr->postdiv2 = 0x2;
        ppll_attr->fbdiv = 0x8 * color_depth;
    } else if ((pixel_pll_clk > 226000) && (pixel_pll_clk <= 533000)) { /* 266000Hz~533000Hz */
        ppll_attr->refdiv = 0x1;
        ppll_attr->postdiv1 = 0x4;
        ppll_attr->postdiv2 = 0x1;
        ppll_attr->fbdiv = 0x4 * color_depth;
    } else {
        ppll_attr->refdiv = 0x1;
        ppll_attr->postdiv1 = 0x2;
        ppll_attr->fbdiv = 0x2 * color_depth;
        if ((pixel_clk <= 600000) && (pixel_clk >= 594000) && yuv_420) { /* 594000Hz~600000Hz */
            ppll_attr->postdiv2 = 0x2;
        } else {
            ppll_attr->postdiv2 = 0x1;
        }
    }

    if (pixel_clk <= 600000) { /* 600000Hz */
        ppll_attr->ppll_out_sel = 2;
    } else if (yuv_420) {
        ppll_attr->ppll_out_sel = 1;
    } else {
        ppll_attr->ppll_out_sel = 0;
    }
}

static hi_void ppll_tmds_cksel_set(struct hdmi_crg *crg, hi_u32 ppll_out_sel)
{
    struct hisilicon_hdmi *hdmi = HI_NULL;
    void __iomem *crg_reg = HI_NULL;
    u32 crg_addr;

    if (crg == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return;
    }

    hdmi = crg->parent;

    if (hdmi->id == 1) {
        crg_addr = PPLL0_CRG_ADDR;
    } else {
        crg_addr = PPLL1_CRG_ADDR;
    }

    crg_reg = osal_ioremap_nocache(crg_addr, 4); /* 4 Bytes */
    if (IS_ERR(crg_reg)) {
        HDMI_ERR("Failed to get memory resource\n");
        return;
    }

    hdmi_clrset(crg_reg, 0, HDMITX21_PPLL_OUT_MASK, hdmitx21_ppll_out(ppll_out_sel));

    osal_iounmap(crg_reg);
}

static hi_void ppll_config(struct hdmi_crg *crg, int fbdiv,
    int refdiv, int postdiv1, int postdiv2)
{
#ifdef HI_HDMI_TEE_SUPPORT
    hi_u32 tmp;
    hi_u32 tmp_addr;

    if (crg->parent->id == 0) {
        tmp_addr = PERI_CRG_PLL1;
    } else {
        tmp_addr = PERI_CRG_PLL0;
    }

    tmp = hdmi_readl(crg->ppll_crg_regs, PPLL_1_OFFSET);
    tmp &= ~(PPLL_POSTDIV1_M | PPLL_POSTDIV2_M);
    tmp |= ppll_postdiv1(postdiv1) | ppll_postdiv2(postdiv2);
    hi_drv_mbx_write_hrf_reg(tmp_addr + PPLL_1_OFFSET, tmp);

    tmp = hdmi_readl(crg->ppll_crg_regs, PPLL_2_OFFSET);
    tmp &= ~(PPLL_REFDIV_M | PPLL_FBDIV_M);
    tmp |= ppll_refdiv(refdiv) | ppll_fbdiv(fbdiv);
    hi_drv_mbx_write_hrf_reg(tmp_addr + PPLL_2_OFFSET, tmp);

    tmp = hdmi_readl(crg->ppll_crg_regs, PPLL_2_OFFSET);
    tmp &= ~PPLL_EN_M;
    hi_drv_mbx_write_hrf_reg(tmp_addr + PPLL_2_OFFSET, tmp);

    tmp = hdmi_readl(crg->ppll_crg_regs, PPLL_2_OFFSET);
    tmp |= PPLL_EN_M;
    hi_drv_mbx_write_hrf_reg(tmp_addr + PPLL_2_OFFSET, tmp);
#else
    hdmi_clrset(crg->ppll_crg_regs, PPLL_1_OFFSET,
                PPLL_POSTDIV1_M | PPLL_POSTDIV2_M,
                ppll_postdiv1(postdiv1) | ppll_postdiv2(postdiv2));
    hdmi_clrset(crg->ppll_crg_regs, PPLL_2_OFFSET,
                PPLL_REFDIV_M | PPLL_FBDIV_M,
                ppll_refdiv(refdiv) | ppll_fbdiv(fbdiv));
    hdmi_clr(crg->ppll_crg_regs, PPLL_2_OFFSET, PPLL_EN_M);
    hdmi_set(crg->ppll_crg_regs, PPLL_2_OFFSET, PPLL_EN_M);
#endif
}

/*
 *  ppll is used only for FRL mode.
 *  pixel_clk: clock for the video format, for example 720P60 -->74.25M
 *  pixel_pll_clk: hw clock to driver the pixel process
 *  (1) For pixel_clk 0~600M , pixel_pll_clk = pixel_clk
 *  (2) For pixel_clk 600M~2.4G, pixel_pll_clk = pixel_clk / 4
 *  (3) For pixel_clk >2.4G, pixel_pll_clk = pixel_clk / 8
 *  clk_dsc: the output of ppll to be used for frl+dsc mode.
 *  clk_tmds: the output of ppll to be used hdmi top in frl mode.
 *  For ppll: refclk = fout4(HPLL) = postdiv_clk(HPLL)/8 = pixel_pll_clk / 8
 *  FRL + DSC:
 *  we use dsc refdiv to process dsc compress, and the target clk from ppll is:
 *  444/422: clk_dsc = pixel_clk/4.
 *  420: clk_dsc = pixel_clk/8
 *  (1) pixel_clk 600M~2.4G
 *  444/422: clk_dsc = (4 * pixel_pll_clk) /4 = pixel_pll_clk = 8 * refclk
 *  420: clk_dsc = 4 * refclk
 *  (2) pixel_clk >2.4G
 *  444/422: clk_dsc = (8 * pixel_pll_clk) /4 = pixel_pll_clk = 16 * refclk
 *  420: clk_dsc = 8 * refclk
 *  From pll calculation:
 *  vco = (refclk/refdiv) * (fbdiv)
 *  postdiv_clk(ppll) = (refclk/refdiv)*fbdiv / postdiv1 / postdiv2
 *  fout1 = postdiv_clk(ppll) / 2
 *  fout2 = postdiv_clk(ppll) / 4
 *  So:
 *  vco = postdiv_clk(ppll) * postdiv1 * postdiv2
 *  and: 800 <= vco <= 3200
 *   postdiv1 and postdiv2 should be 1~7
 *  So finally:
 *  (1) 444/422: clk_dsc = postdiv(ppll)   postdiv(ppll)/refclk = 8 or 16
 *  (2) 420:  clk_dsc = fout1 postdiv(ppll)/refclk = 4 or 8
 *  apply the vco limit.
 *  pixel 600~2000: refdiv = 1; fbdiv = 48; postdiv1 = 3; postdiv2 = 2;
 *  pixel 2000~2400: refdiv = 1; fbdiv = 16; postdiv1 = 2; postdiv2 = 1;
 *  pixel 2400~4800: refdiv = 1; fbdiv = 16; postdiv1 = 2; postdiv2 = 1;
 *  FRL + NoDSC:
 *  (1) pixel_clk 0~600M
 *  (2) pixel_clk 600M~2.4G
 *  more complex, refer to the detail code.
 *  Note: for pixel_clk 0~600M only 594M~600M we support 420.
 */
static void set_ppll_factor(struct hdmi_crg *crg, struct crg_config_data *cfg)
{
    struct hdmi_crg_ppll_attr ppll_attr = {0};
    int pixel_clk = cfg->pixel_rate;
    hi_bool yuv_420 = cfg->yuv_420;
    int color_depth = cfg->color_depth;
    hi_bool locked = HI_FALSE;
    int i = 0;

    if (cfg->dsc_enable) /* no need to process color depth for dsc */
        cal_frl_dsc_ppll_setting(pixel_clk, yuv_420, &ppll_attr);
    else
        cal_frl_nodsc_ppll_setting(pixel_clk, yuv_420, color_depth, &ppll_attr);

    /* config PPLL out: 0 postdiv, 1 fout1,  2 fout2 */
    ppll_tmds_cksel_set(crg, ppll_attr.ppll_out_sel);

    /* config PPLL */
    ppll_config(crg, ppll_attr.fbdiv, ppll_attr.refdiv, ppll_attr.postdiv1, ppll_attr.postdiv2);

    /* Fixme, wait the pll lock */
    do {
        locked = get_ppll_lock_status(crg);
        if (locked) {
            break;
        }

        osal_msleep(1); /* need sleep 1ms. */

        i++;
        if (i > 100) {  /* timeout is 100ms. */
            HDMI_ERR("lock ppll timeout\n");
            break;
        }
    } while (1);
}

static void set_hpll_1d4_divider(struct hdmi_crg *crg, int div)
{
    struct hisilicon_hdmi *hdmi = crg->parent;

    /* Two port's HW register is not same */
    if (hdmi->id == 0)
        hdmi_clrset(crg->ctrl_crg_regs, 0,
                    HDMITX21_PXLD4_CLK_DIV_MASK,
                    hdmitx21_pxld4_clk_div(div - 1));
    else
        hdmi_clrset(crg->ctrl_crg_regs, 0,
                    HDMITX20_PXLD4_CLK_DIV_MASK,
                    hdmitx20_pxld4_clk_div(div - 1));
}

static hi_void rest_phy(hi_u32 phy_id)
{
    hi_void __iomem *reg = HI_NULL;
    hi_u32 base_addr;

    if (phy_id == 0) {
        base_addr = PHY21_RST_BASE;
    }else {
        base_addr = PHY20_RST_BASE;
    }

    reg = osal_ioremap_nocache(base_addr, 4); /* register size is 4Bytes. */
    if (IS_ERR(reg)) {
        HDMI_ERR("Failed to get vir addr for base phy addr 0x%x\n", base_addr);
        return;
    }

    hdmi_clr(reg, 0, PHY_RST_MASK);
    osal_udelay(1);
    hdmi_set(reg, 0, PHY_RST_MASK);

    osal_iounmap(reg);

    return;
}

static int gcd(int a, int b)
{
    int r;

    if (b == 0) {
        HDMI_ERR("b is zero!\n");
        return b;
    }

    r = a % b;

    while (r != 0) {
        a = b;
        b = r;
        r = a % b;
    }

    return b;
}

static void set_dsc_divider(struct hdmi_crg *crg, struct crg_config_data *data)
{
    int dsc_div_up, dsc_div_down, div;

    div = gcd(data->htotal, data->hctotal);
    if (div == 0) {
        HDMI_ERR("div is zero!\n");
        return;
    }
    dsc_div_up = data->hctotal / div;
    dsc_div_down = data->htotal / div;

    hdmi_clrset(crg->hdmi_regs, REG_DSC_DIV_UPDATA,
                REG_DSC_DIV_UP_M, reg_dsc_div_up(dsc_div_up));
    hdmi_clrset(crg->hdmi_regs, REG_DSC_DIV_DOWNDATA,
                REG_DSC_DIV_DOWN_M, reg_dsc_div_down(dsc_div_down));
    hdmi_clrset(crg->hdmi_regs, REG_DSC_DIV_UPDATA,
                REG_DSC_DIV_DIGITAL_EN_M, reg_dsc_div_digital_en(1));
}

static void ext_crg_config_tmds(struct hdmi_crg *crg, struct crg_config_data *data)
{
    if (data->yuv_420) {
        set_hpll_1d4_divider(crg, 8);
    } else {
        set_hpll_1d4_divider(crg, 4);
    }
}

static void ext_crg_config_frl_dsc(struct hdmi_crg *crg, struct crg_config_data *cfg)
{
    int pixel_rate = cfg->pixel_rate;
    /*
     * In frl mode with dsc(600M~):
     * (1) video path clock (pixel clock) is from ppll postdiv or phy
     * (2) hdmi top clock is the same with video path clock.
     * (3) phy ref clock is from 1d8 clock.
     */
    if (pixel_rate <= 600000) { /* less than 340Mhz can be use DSC in the spec */
        HDMI_ERR("not supported pixel rate :%dM in nodsc mode\n",
                 pixel_rate);
        return;
    }

    if (cfg->prefer_ppll) {
        /* how about 10bit/420 */
        if (cfg->htotal > 2 * cfg->hctotal) {
            set_ppll_factor(crg, cfg);
            set_dsc_divider(crg, cfg);
        } else {
            HDMI_ERR("not supported dsc compress rate now\n");
        }
    }
}

static void ext_crg_config_frl_nodsc(struct hdmi_crg *crg,
                                     struct crg_config_data *cfg)
{
    int pixel_rate = cfg->pixel_rate;
    /*
     * In frl mode with nodsc(0~2.4G):
     * (1) video path clock (pixel clock) is from hpll 1d4(420
     * need to be divided at HPLL 1d4 divider.
     * (2) hdmi top clock is from phy tmds clock or ppll.
     * (with 1X,1.25X,1.5X pixel to tmds factor)
     * (3) phy ref clock is from Hpll 1d4 clock
     * (4) acr clock is from phy frl and default i2s path enabled.
     */
    if (pixel_rate <= 600000 && pixel_rate >= 0) {
        /* orignal pixel is 1d1 clock, so no divider base is 4 */
        if (cfg->yuv_420) {
            set_hpll_1d4_divider(crg, 8);
        } else {
            set_hpll_1d4_divider(crg, 4);
        }
    } else if (pixel_rate > 600000 && pixel_rate <= 2400000) {
        /* orignal pixel is 1d4 clock, so no divider */
        if (cfg->yuv_420) {
            set_hpll_1d4_divider(crg, 2);
        } else {
            set_hpll_1d4_divider(crg, 1);
        }
    } else {
        HDMI_ERR("not supported pixel rate :%dM in nodsc mode\n",
                 pixel_rate);
        return;
    }

    /*
     * ppll rate setting for hdmi top, if we choose phy tmds clock for hdmi
     * top, then it is done in in phy config.
     */
    if (cfg->prefer_ppll) {
        set_ppll_factor(crg, cfg);
    }
}

/*
 * Mode and basic info which will affect the clock setting, refer to the clock
 * architechture.
 * The following infos should be considered.
 * (1) frl/dsc mode.
 * (2) 420/other.
 * (3) dsc and dsc rate if frl is enabled.
 * (4) pixel clock rate based on 444.
 * (5) other prefer pll source:
 *      phy pll/ppll,  phy ref prefer tpll/pixel clock.
 */
void hdmi_crg_config(struct hdmi_crg *crg, struct crg_config_data *cfg)
{
    struct hdmi_crg_reg_config reg_config;

    if (!cfg || !crg) {
        HDMI_ERR("invalid input cfg\n");
        return;
    }

    /*
     * To make code easy, the following sequence is applied.
     * 1. CONFIG system pll(PPLL AND HPLL)
     * 2. CONFIG HDMI internal CRG(clock gates/mux)
     * Phy clock is not configured here, so after phy clock is
     * configured, we can reset the controller.
     */
    if (cfg->encoder_mode == 1) {  // FRL
        if (cfg->dsc_enable) {     /* 600M+  FRL + DSC */
            ext_crg_config_frl_dsc(crg, cfg);
            reg_config = g_hdmi_crg_default_configs[2];
            if (cfg->prefer_ppll) { /* vidpath and top clock from ppll */
                reg_config.dsc_clk_sel = 1;
            }
        } else { /* 0~2.4G FRL */
            ext_crg_config_frl_nodsc(crg, cfg);
            reg_config = g_hdmi_crg_default_configs[1];
            if (cfg->prefer_ppll) { /* hdmi top clock from ppll */
                reg_config.clk_tmds_source_sel = 1;
            }
        }
    } else if (cfg->encoder_mode == 0) { /* 0~600M TMDS */
        /*
         * In tmds mode:
         * (1) video path clock (pixel clock) is from hpll 1d4(420
         * need to be divided at HPLL 1d4 divider.
         * (2) hdmi top clock is from phy(with 1X,1.25X,1.5X pixel
         * clock) tmds clock.
         * (3) phy ref clock is from Hpll 1d1 clock, 420 is process
         * by hdmi internally crg with 420 mode automatically.
         * (4) acr clock is from phy tmds.
         */
        ext_crg_config_tmds(crg, cfg);
        reg_config = g_hdmi_crg_default_configs[0];
    } else {
        HDMI_ERR("unsupported encoder_mode, FRL/TMDS is supported\n");
        return;
    }

    hdmi_crg_hw_config(crg, &reg_config, cfg->encoder_mode);
}

/*
 * Enable crg clk
 * The following information should be considered.
 * PERI_CRG354: hdmi2.0 ctrl reg
 * [12]hdmitx_ctrl0_as_cken
 * [5]hdmitx_ctrl0_os_cken
 * [2]hdmitx_ctrl0_osc_24m_cken
 * [1]hdmitx20_pxld4_cken
 * [0]ppll0_tmds_cken
 * PERI_CRG356: phy20 reg
 * [2]ac_ctrl0_modclk_cken
 * [1]hdmitx_phy0_modclk_cken
 * [0]hdmitx_phy0_tmds_cken
 * PERI_CRG355: hdmi2.1 ctrl reg
 * [20]hdmitx_ctrl1_as_cken
 * [19]hdmitx_ctrl1_os_cken
 * [7]hdmitx_ctrl1_osc_24m_cken
 * [5]ppll1_tmds_cken
 * [2]hdmitx21_dsc_cken
 * [1]hdmitx21_pxld4_cken
 * [0]hdmitx21_pxl_cken
 * PERI_CRG357: phy2.1 ctrl reg
 * [2]hdmitx_phy1_tmds_cken
 * [1]hdmitx_phy1_modclk_cken
 * [0]ac_ctrl1_modclk_cken
 */
void hdmi_crg_clk_enable(struct hdmi_crg *crg, hi_bool enable)
{
    struct hisilicon_hdmi *hdmi = crg->parent;
    hi_u32 ctrl_val;
    hi_u32 phy_val;
    hi_u32 phy_id;

    if ((crg == NULL) || (crg->parent == NULL)) {
        HDMI_ERR("crg pointer is %p, crg->parent pointer is %p\n", crg, crg->parent);
        return;
    }

    phy_id = hdmi->id;

    ctrl_val = HDMITX_CTRL_OSC_24M_CKEN_M |
               HDMITX_CTRL_OS_CKEN_M |
               HDMITX_CTRL_AS_CKEN_M |
               HDMITX_CTRL_PPLL_TMDS_CKEN_M;

    phy_val  = HDMITX_PHY_TMDS_CKEN_M |
               HDMITX_PHY_MODCLK_CKEN_M |
               HDMITX_PHY_AC_CTRL_MODCLK_CKEN_M;

    if (hdmi->id == 1) {
        ctrl_val |= HDMITX_CTRL0_PXLD4_CKEN_M;
    } else {
        ctrl_val |= HDMITX_CTRL1_DSC_CKEN_M |
                    HDMITX_CTRL1_PXL_CKEN_M |
                    HDMITX_CTRL1_PXLD4_CKEN_M;
    }

    if (enable == HI_TRUE) {
        hdmi_set(crg->ctrl_crg_regs, 0, ctrl_val);
        hdmi_set(crg->phy_crg_regs, 0, phy_val);
    } else {
        hdmi_clr(crg->ctrl_crg_regs, 0, ctrl_val);
        /* Need to reset aphy before disable phy clk */
        rest_phy(phy_id);
        hdmi_clr(crg->phy_crg_regs, 0, phy_val);
    }

    return;
}


struct hdmi_crg *hdmi_crg_init(struct hisilicon_hdmi *hdmi)
{
    struct hdmi_crg *crg = HI_NULL;

    if (hdmi == HI_NULL) {
        return HI_NULL;
    }

    crg = osal_kmalloc(HI_ID_HDMITX, sizeof(struct hdmi_crg), OSAL_GFP_KERNEL);
    if (crg == HI_NULL) {
        HDMI_ERR("osal_kmalloc fail.\n");
        return HI_NULL;
    }

    if (memset_s(crg, sizeof(struct hdmi_crg), 0, sizeof(struct hdmi_crg))) {
        HDMI_ERR("memset_s fail.\n");
        osal_kfree(HI_ID_HDMITX, crg);
        return HI_NULL;
    }
    crg->parent = hdmi;
    crg->hdmi_regs = hdmi->hdmi_regs;             /* for hdmi internal crg */
    crg->ctrl_crg_regs = hdmi->ctrl_crg_regs;     /* for HPLL out div */
    crg->phy_crg_regs = hdmi->phy_crg_regs;       /* for PPLL out select */
    crg->ppll_crg_regs = hdmi->ppll_crg_regs;     /* for PPLL control */
    crg->ppll_state_regs = hdmi->ppll_state_regs; /* for PPLL status */
    /* Fixme, in hi96cv300, hdmi0 is tx2.1 has got dsc ,while hdmi1 is tx2.0 & no dsc */
    crg->dsc_enable = (hdmi->id == 0) ? 1 : 0;

    return crg;
}

void hdmi_crg_deinit(struct hdmi_crg *crg)
{
    osal_kfree(HI_ID_HDMITX, crg);
}
