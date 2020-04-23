/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: source file of hdmitx dphy hal layer
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#include "hal_hdmitx_phy.h"
#include "hal_hdmitx_dphy_reg.h"
#include "hal_hdmitx_io.h"
#ifdef HI_HDMI_TEE_SUPPORT
#include <linux/hisilicon/hi_drv_mbx.h>
#endif
#include "hi_osal.h"
#include "drv_hdmitx.h"

/* aphy cs define */
#define APHY_CS_0       0x1
#define APHY_CS_1       0x2
#define APHY_CS_2       0x4
#define APHY_CS_3       0x8
#define APHY_CS_012     0x7
#define APHY_CS_0123    0xf
#define APHY_CS_8       0x100
#define APHY_CS_9       0x200

#define APHY_CS_MAX     10

#define APB_RESET       (0x1 << 5)

#define RATE_3G         1
#define RATE_6G         2
#define RATE_8G         3
#define RATE_10G        4
#define RATE_12G        5

#define MAX_FFE_LEVEL   4

/* tpll register */
#define REG_TPLL_1          0x0
#define reg_pll_postdiv2(x) (((x) & (0x7)) << 28)
#define REG_PLL_POSTDIV2_M  (0x7 << 28)
#define reg_pll_postdiv1(x) (((x) & (0x7)) << 24)
#define REG_PLL_POSTDIV1_M  (0x7 << 24)
#define reg_pll_frac(x)     ((x) & 0xffffff << 0)
#define REG_PLL_FRAC_M      (0xffffff << 0)

#define REG_TPLL_2          0x4
#define reg_pll_en(x)       (((x) & 0x1) << 20)
#define REG_PLL_EN_M        (0x1 << 20)
#define reg_pll_refdiv(x)   (((x) & 0x3f) << 12)
#define REG_PLL_REFDIV_M    (0x3f << 12)
#define reg_pll_fbdiv(x)    (((x) & 0xfff) << 0)
#define REG_PLL_FBDIV_M     (0xfff << 0)

#define aphy_refclksel(x)   (((x) & 0x7) << 4)
#define APHY_REFCLKSEL_M    (0x7 << 4)
#define aphy_refclkdiv(x)   (((x) & 0x7) << 0)
#define APHY_REFCLKDIV_M    (0x7 << 0)

#define aphy_data_channel_en(x) (((x) & 0x1) << 1)
#define APHY_DATA_CHANNEL_EN_M  (0x1 << 1)

#define PIXEL_CLK_37500     37500
#define PIXEL_CLK_75000     75000
#define PIXEL_CLK_150000    150000
#define PIXEL_CLK_300000    300000
#define PIXEL_CLK_600000    600000

enum aphy_addr_offset {
    APHY_OFFSET_0 = 0x0,
    APHY_OFFSET_1,
    APHY_OFFSET_2,
    APHY_OFFSET_3,
    APHY_OFFSET_4,
    APHY_OFFSET_5,
    APHY_OFFSET_6,
    APHY_OFFSET_7,
    APHY_OFFSET_8,
    APHY_OFFSET_9,
    APHY_OFFSET_A,
    APHY_OFFSET_B,
    APHY_OFFSET_C,
    APHY_OFFSET_D,
    APHY_OFFSET_E,
    APHY_OFFSET_F,
    APHY_OFFSET_MAX,
};

struct tpll_div_params {
    hi_u32 postdiv2;
    hi_u32 postdiv1;
    hi_u32 fbdiv;
    hi_u32 refdiv;
};

struct fcg_div_params {
    hi_u32 tmds_cnt_val;
    hi_u32 cnt1_target;
    hi_u8 fcg_divm;
    hi_u8 fcg_divp;
    hi_u8 ki;
    hi_u8 lock_th;
    hi_u8 fcg_postdiv;
};

struct dphy_spec_en {
    hi_bool drv_post2_en;
    hi_bool drv_post1_en;
    hi_bool drv_pre_en;
};

struct dphy_spec_params {
    hi_u8 drv_post2;
    hi_u8 drv_post1;
    hi_u8 drv_main;
    hi_u8 drv_pre;
};

struct aphy_spec_params {
    hi_u8 offset_0;
    hi_u8 offset_1;
    hi_u8 offset_2;
    hi_u8 offset_3;
    hi_u8 offset_4;
    hi_u8 offset_8;
    hi_u8 offset_9;
    hi_u8 offset_a;
    hi_u8 offset_b;
};

struct spec_params {
    struct dphy_spec_en en;
    struct dphy_spec_params dphy;
    struct aphy_spec_params aphy;
};

struct tmds_spec_params {
    hi_u32 min_tmds_clk;
    hi_u32 max_tmds_clk;
    struct spec_params data;
    struct spec_params clock;
};

struct frl_spec_params {
    hi_u32 rate;
    struct dphy_spec_en en;
    struct aphy_spec_params aphy;
    struct dphy_spec_params ffe[MAX_FFE_LEVEL];
};

static struct tmds_spec_params g_tmds_spec[] = {
    {   /* min_tmds_clk, max_tmds_clk */
        25000, 100000,
        /* data */
        {
            /* post2_en, post1_en, pre_en */
            { HI_FALSE, HI_FALSE, HI_FALSE },
            /* post2, post1, main, pre */
            { 0x00, 0x00, 0x13, 0x00 },
            /* offset */
            { 0x7f, 0x7e, 0x3e, 0xb6, 0x07, 0x48,  0x80, 0x00, 0x70 },
        },
        /* clock */
        {
            /* post2_en, post1_en,pre_en */
            { HI_FALSE, HI_FALSE, HI_FALSE },
            /* post2, post1, main, pre */
            { 0x00, 0x00, 0x13, 0x00 },
            /* offset */
            { 0x7f, 0x3e, 0x3e,  0x06, 0x00, 0x48,  0x80, 0x00, 0x70 },
        },
    },

    {   /* min_tmds_clk, max_tmds_clk */
        100000, 165000,
        /* data */
        {
            /* post2_en, post1_en, pre_en */
            { HI_FALSE, HI_FALSE, HI_FALSE },
            /* post2, post1, main, pre */
            { 0x00, 0x00, 0x13, 0x00 },
            /* offset */
            { 0x7f, 0x7e, 0x3e, 0xb6, 0x07, 0x48, 0x80, 0x00, 0x70 },
        },
        /* clock */
        {
            /* post2_en, post1_en, pre_en */
            { HI_FALSE, HI_FALSE, HI_FALSE },
            /* post2, post1, main, pre */
            { 0x00, 0x00, 0x13, 0x00 },
            /* offset */
            { 0x7f, 0x3e, 0x3e,  0x06, 0x00, 0x48,  0x80, 0x00, 0x70 },
        },
    },

    {   /* min_tmds_clk, max_tmds_clk */
        165000, 340000,
        /* data */
        {
            /* post2_en, post1_en, pre_en */
            { HI_FALSE, HI_TRUE, HI_TRUE },
            /* post2, post1, main, pre */
            { 0x00, 0x05, 0x1a, 0x00 },
            /* offset */
            { 0x7f, 0x7e, 0x3e, 0xb7, 0x00, 0x6c,  0x80, 0x00, 0x60 },
        },
        /* clock */
        {
            /* post2_en, post1_en, pre_en */
            { HI_FALSE, HI_TRUE, HI_TRUE },
            /* post2, post1, main, pre */
            { 0x00, 0x00, 0x13, 0x00 },
            /* offset */
            { 0x7f, 0x7e, 0x3e, 0x06, 0x00, 0x48, 0x80, 0x00, 0x60 },
        },
    },

    {   /* min_tmds_clk, max_tmds_clk */
        340000, 600000,
        /* data */
        {
            /* post2_en, post1_en, pre_en */
            { HI_FALSE, HI_TRUE, HI_TRUE },
            /* post2, post1, main, pre */
            { 0x00, 0x00, 0x27, 0x05 },
            /* offset */
            { 0x7f, 0x7f, 0x3e, 0x06, 0x00, 0x74, 0x81, 0x00, 0x50 },
        },
        /* clock */
        {
            /* post2_en, post1_en, pre_en */
            { HI_FALSE, HI_TRUE, HI_TRUE },
            /* post2, post1, main, pre */
            { 0x00, 0x00, 0x12, 0x00 },
            /* offset */
            { 0x7f, 0x7f, 0x3e, 0x06, 0x00, 0x48, 0x81, 0x00, 0x50 },
        },
    },
};

static struct frl_spec_params g_frl_spec[] = {
    {
        RATE_3G,
        /* post2_en, post1_en, pre_en */
        { HI_FALSE, HI_TRUE, HI_TRUE},
        /* offset */
        { 0x7f, 0x7f, 0x3e, 0x06, 0x10,  0x74, 0x80, 0x00,  0x40 },
        {   /* post2, post1, main, pre */
            { 0x00, 0x00, 0x26,  0x08 }, /* ffe0 */
            { 0x00, 0x00, 0x26,  0x08 }, /* ffe1 */
            { 0x00, 0x00, 0x26,  0x08 }, /* ffe2 */
            { 0x00, 0x00, 0x26,  0x08 }, /* ffe3 */
        },
    },

    {
        RATE_6G,
        /* post2_en, post1_en, pre_en */
        { HI_FALSE, HI_TRUE, HI_TRUE },
        /* offset */
        { 0x7f, 0x7f, 0x3e, 0x06, 0x10, 0x74, 0x80, 0x00, 0x40 },
        {   /* post2, post1, main, pre */
            { 0x00, 0x00, 0x26, 0x0a }, /* ffe0 */
            { 0x00, 0x00, 0x26, 0x0a }, /* ffe1 */
            { 0x00, 0x00, 0x26, 0x0a }, /* ffe2 */
            { 0x00, 0x00, 0x26, 0x0a }, /* ffe3 */
        },
    },

    {
        RATE_8G,
        /* post2_en, post1_en, pre_en */
        { HI_FALSE, HI_TRUE, HI_TRUE},
        /* offset */
        { 0x7f, 0x7f, 0x3e, 0x06, 0x10, 0x74, 0x80, 0x00, 0x40 },
        {   /* post2, post1, main, pre */
            { 0x00, 0x04, 0x24, 0x0c }, /* ffe0 */
            { 0x00, 0x04, 0x24, 0x0c }, /* ffe1 */
            { 0x00, 0x04, 0x24, 0x0c }, /* ffe2 */
            { 0x00, 0x04, 0x24, 0x0c }, /* ffe3 */
        },
    },

    {
        RATE_10G,
        /* post2_en, post1_en, pre_en */
        { HI_FALSE, HI_TRUE, HI_TRUE },
        /* offset */
        { 0x7f, 0x7f, 0x3e, 0x06, 0x10, 0x74, 0x80, 0x00, 0x40 },
        {   /* post2, post1, main, pre */
            { 0x00, 0x08, 0x25, 0x16 }, /* ffe0 */
            { 0x00, 0x08, 0x25, 0x16 }, /* ffe1 */
            { 0x00, 0x08, 0x25, 0x16 }, /* ffe2 */
            { 0x00, 0x08, 0x25, 0x16 }, /* ffe3 */
        },
    },

    {
        RATE_12G,
        /* post2_en, post1_en, pre_en */
        { HI_FALSE, HI_TRUE, HI_TRUE },
        /* offset */
        { 0x7f, 0x7f, 0x3e, 0x06, 0x10, 0x74, 0x80, 0x00, 0x40 },
        {   /* post2, post1, main, pre */
            { 0x00, 0x0a, 0x28, 0x2f}, /* ffe0 */
            { 0x00, 0x0a, 0x28, 0x2f}, /* ffe1 */
            { 0x00, 0x0a, 0x28, 0x2f}, /* ffe2 */
            { 0x00, 0x0a, 0x28, 0x2f}, /* ffe3 */
        },
    }
};

#define PHY_TMDS_OUTEN          0x14

#define PHY0_CRG_ADDR           0x00A00590
#define PHY1_CRG_ADDR           0x00A00594

#ifdef HI_HDMI_TEE_SUPPORT
#define TPLL0_CRG_ADDR          0x00A000C0
#define TPLL1_CRG_ADDR          0x00A000CC
#endif

static hi_u32 phy_pow(hi_u32 base, hi_u32 exp)
{
    hi_u8 i;
    hi_u32 val = 1;

    for (i = 0; i < exp; i++) {
        val = val * base;
    }

    return val;
}

static hi_void stb_writeb(struct hdmitx_phy *hdmi_phy, hi_u32 cs, enum aphy_addr_offset offset, hi_u8 data)
{
    /* CS reset */
    hdmi_clr(hdmi_phy->regs, REG_PHY_CSEN, REG_STB_CS_EN_M);
    /* WR  en */
    hdmi_clr(hdmi_phy->regs, REG_PHY_WR, REG_STB_WEN_M);
    /* WDATA set */
    hdmi_write_bits(hdmi_phy->regs, REG_PHY_WDATA, REG_STB_WDATA_M, data);
    /* ADDR set */
    hdmi_write_bits(hdmi_phy->regs, REG_PHY_ADDR, REG_STB_ADDR_M, offset);
    /* WR set */
    hdmi_write_bits(hdmi_phy->regs, REG_PHY_WR, REG_STB_WEN_M, 0x1);
    /* CS set */
    hdmi_write_bits(hdmi_phy->regs, REG_PHY_CSEN, REG_STB_CS_EN_M, cs);
    /* delay time 1 */
    osal_udelay(1);
    /* CS reset */
    hdmi_clr(hdmi_phy->regs, REG_PHY_CSEN, REG_STB_CS_EN_M);
    /* WR reset */
    hdmi_clr(hdmi_phy->regs, REG_PHY_WR, REG_STB_WEN_M);
}

static hi_void stb_readb(struct hdmitx_phy *hdmi_phy, hi_u32 cs, enum aphy_addr_offset offset, hi_u8 *data)
{
    /* cs reset */
    hdmi_clr(hdmi_phy->regs, REG_PHY_CSEN, REG_STB_CS_EN_M);
    /* wr reset */
    hdmi_clr(hdmi_phy->regs, REG_PHY_WR, REG_STB_WEN_M);
    hdmi_write_bits(hdmi_phy->regs, REG_PHY_ADDR, REG_STB_ADDR_M, offset);
    osal_udelay(1);
    /* cs set */
    hdmi_write_bits(hdmi_phy->regs, REG_PHY_CSEN, REG_STB_CS_EN_M, cs);
    osal_udelay(1);
    /* rdata read */
    *data = hdmi_readl(hdmi_phy->regs, REG_PHY_RDATA);
    /* cs reset */
    hdmi_clr(hdmi_phy->regs, REG_PHY_CSEN, REG_STB_CS_EN_M);
}

static hi_void stb_set_bits(struct hdmitx_phy *hdmi_phy, hi_u32 cs,
    enum aphy_addr_offset aphy_offset, hi_u8 mask, hi_u8 value)
{
    hi_u8 i;
    hi_u32 cs_tmp = 0;
    hi_u8 tmp = 0;

    for (i = 0; i < APHY_CS_MAX; i++) {
        if ((cs >> i) & 0x1) {
            cs_tmp = 0x1 << i;
            break;
        }
    }

    if (i == APHY_CS_MAX) {
        HDMI_ERR("cs is zero!");
        return;
    }

    stb_readb(hdmi_phy, cs_tmp, aphy_offset, &tmp);
    tmp = (tmp & ~mask) | (value & mask);
    stb_writeb(hdmi_phy, cs, aphy_offset, tmp);
}

static struct tmds_spec_params* get_tmds_spec_params(hi_u32 tmds_clk, hi_u8 pcb_len)
{
    hi_u32 i;
    hi_u32 len;
    struct tmds_spec_params *tmp = HI_NULL;

    if (pcb_len == PCB_LEN_1) {
        tmp = &g_tmds_spec[0];
        len = ARRAY_SIZE(g_tmds_spec);
    } else if (pcb_len == PCB_LEN_2) {
        tmp = &g_tmds_spec[0];
        len = ARRAY_SIZE(g_tmds_spec);
    } else if (pcb_len == PCB_LEN_3) {
        tmp = &g_tmds_spec[0];
        len = ARRAY_SIZE(g_tmds_spec);
    } else if (pcb_len == PCB_LEN_4) {
        tmp = &g_tmds_spec[0];
        len = ARRAY_SIZE(g_tmds_spec);
    } else if (pcb_len == PCB_LEN_5) {
        tmp = &g_tmds_spec[0];
        len = ARRAY_SIZE(g_tmds_spec);
    } else {
        tmp = &g_tmds_spec[0];
        len = ARRAY_SIZE(g_tmds_spec);
    }

    for (i = 0; i < len; i++) {
        if (tmds_clk >= tmp[i].min_tmds_clk && tmds_clk < tmp[i].max_tmds_clk) {
            return &tmp[i];
        }
    }

    HDMI_ERR("tmds clock exceed max tmds clock!\n");
    return &tmp[0];
}

static struct frl_spec_params* get_frl_spec_params(hi_u32 rate, hi_u8 pcb_len)
{
    hi_u32 i = 0;
    hi_u32 len = 0;
    struct frl_spec_params *tmp = HI_NULL;

    if (pcb_len == PCB_LEN_1) {
        tmp = &g_frl_spec[0];
        len = ARRAY_SIZE(g_frl_spec);
    } else if (pcb_len == PCB_LEN_2) {
        tmp = &g_frl_spec[0];
        len = ARRAY_SIZE(g_frl_spec);
    } else if (pcb_len == PCB_LEN_3) {
        tmp = &g_frl_spec[0];
        len = ARRAY_SIZE(g_frl_spec);
    } else if (pcb_len == PCB_LEN_4) {
        tmp = &g_frl_spec[0];
        len = ARRAY_SIZE(g_frl_spec);
    } else if (pcb_len == PCB_LEN_5) {
        tmp = &g_frl_spec[0];
        len = ARRAY_SIZE(g_frl_spec);
    } else {
        tmp = &g_frl_spec[0];
        len = ARRAY_SIZE(g_frl_spec);
    }

    for (i = 0; i < len; i++) {
        if (rate == tmp[i].rate) {
            return &tmp[i];
        }
    }

    HDMI_ERR("frl rate exceed max frl rate!\n");
    return &tmp[0];
}

static hi_u32 get_ref_clk_div(hi_u32 pixel_clk)
{
    hi_u32 ref_clk_div = 0;

    if (pixel_clk < PIXEL_CLK_37500) {
        ref_clk_div = 0x0;
    } else if (pixel_clk >= PIXEL_CLK_37500 && pixel_clk < PIXEL_CLK_75000) {
        ref_clk_div = 0x1;
    } else if (pixel_clk >= PIXEL_CLK_75000 && pixel_clk < PIXEL_CLK_150000) {
        ref_clk_div = 0x2;
    } else if (pixel_clk >= PIXEL_CLK_150000 && pixel_clk < PIXEL_CLK_300000) {
        ref_clk_div = 0x3;
    } else if (pixel_clk >= PIXEL_CLK_300000 && pixel_clk < PIXEL_CLK_600000) {
        ref_clk_div = 0x4;
    }

    return ref_clk_div;
}

static hi_u32 get_tmds_clk_div(hi_u32 tmds_clk)
{
    hi_u32 tmds_clk_div = 0;

    if (tmds_clk < PIXEL_CLK_37500) {
        tmds_clk_div = 0x4;
    } else if (tmds_clk >= PIXEL_CLK_37500 && tmds_clk < PIXEL_CLK_75000) {
        tmds_clk_div = 0x3;
    } else if (tmds_clk >= PIXEL_CLK_75000 && tmds_clk < PIXEL_CLK_150000) {
        tmds_clk_div = 0x2;
    } else if (tmds_clk >= PIXEL_CLK_150000 && tmds_clk < PIXEL_CLK_300000) {
        tmds_clk_div = 0x1;
    } else if (tmds_clk >= PIXEL_CLK_300000 &&  tmds_clk < PIXEL_CLK_600000) {
        tmds_clk_div = 0x0;
    }

    return tmds_clk_div;
}

static hi_u32 get_tmds_fcon_param(struct hdmitx_phy *hdmi_phy, hi_u32 tmds_clk)
{
    hi_u32 pll_vco_freq;
    hi_u32 tmds_divsel;
    hi_u32 fccntr0;
    hi_u32 i_ref_cnt;

    fccntr0 = hdmi_read_bits(hdmi_phy->regs, REG_FCCNTR0, REG_I_REF_CNT_LEN_M);
    HDMI_DBG(">>fccntr0 = %d<<\n", fccntr0);
    tmds_divsel = get_tmds_clk_div(tmds_clk);
    pll_vco_freq = tmds_clk * 5 * phy_pow(2, tmds_divsel);
    i_ref_cnt = (pll_vco_freq * 2 * fccntr0) / (1000 * 20 * 3);

    return i_ref_cnt;
}

static hi_u32 get_frl_fcon_param(struct hdmitx_phy *hdmi_phy, hi_u32 frl_rate)
{
    hi_u32 i_ref_cnt = 0;
    hi_u32 i_ref_cnt_length;

    i_ref_cnt_length = hdmi_read_bits(hdmi_phy->regs, REG_FCCNTR0, REG_I_REF_CNT_LEN_M);
    HDMI_DBG(">>i_ref_cnt_length = %d<<\n", i_ref_cnt_length);

    switch (frl_rate) {
        case FRL_RATE_3L_3G:
        case FRL_RATE_3L_6G:
        case FRL_RATE_4L_6G:
        case FRL_RATE_4L_12G:
            i_ref_cnt = i_ref_cnt_length * 100;
            break;
        case FRL_RATE_4L_8G:
            i_ref_cnt = (i_ref_cnt_length * 2000) / 30;
            break;
        case FRL_RATE_4L_10G:
            i_ref_cnt = (i_ref_cnt_length * 2500) / 30;
            break;
        default:
            break;
    }

    return i_ref_cnt;
}

static hi_u32 get_tmds_frac_param(hi_u32 tmds_clk, hi_u32 pixel_clk,
    hi_u32 color_depth, hi_u32 *m_value, hi_u32 *n_value)
{
    hi_u32 m, n;
    hi_u32 mn_value;
    hi_u8 ref_clk_div;
    hi_u8 tmds_divsel;
    hi_u32 k;

    tmds_divsel = get_tmds_clk_div(tmds_clk);
    ref_clk_div = get_ref_clk_div(pixel_clk);

    /**
     * pll_refclk = pixel_clk / phy_pow(2, ref_clk_div);
     * pll_vco_clk = tmds_clk * 5 * phy_pow(2, tmds_divsel);
     * mn_value = pll_vco_clk / pll_refclk
     *  = tmds_clk * 5 * phy_pow(2, tmds_divsel) *
     *    phy_pow(2, ref_clk_div) /pixel_clk
     *  = 5 * phy_pow(2, tmds_divsel) *
     *    phy_pow(2, ref_clk_div) * (depth_color);
     */
    switch (color_depth) {
        case DEEP_COLOR_24BIT:
            mn_value = 5 * phy_pow(2, tmds_divsel) * phy_pow(2, ref_clk_div) * 1;
            break;
        case DEEP_COLOR_30BIT:
            mn_value = 5 * phy_pow(2, tmds_divsel) * phy_pow(2, ref_clk_div) * 5 / 4;
            break;
        case DEEP_COLOR_36BIT:
            mn_value = 5 * phy_pow(2, tmds_divsel) * phy_pow(2, ref_clk_div) * 3 / 2;
            break;
        default:
            mn_value = 5 * phy_pow(2, tmds_divsel) * phy_pow(2, ref_clk_div) * 1;
            break;
    }

    n = mn_value % 10;

    if (n == 0) {
        k = 2;
        n = 10;
    } else {
        k = 1;
    }

    m = mn_value / 10 - k;

    *m_value = m;
    *n_value = n;

    return mn_value;
}

static hi_void get_frl_frac_param(hi_u32 frl_rate, hi_u32 *m_value, hi_u32 *n_value, hi_u32 *x_value)
{
    switch (frl_rate) {
        case FRL_RATE_3L_3G:
        case FRL_RATE_3L_6G:
        case FRL_RATE_4L_6G:
        case FRL_RATE_4L_12G:
            *m_value = 0xb;
            *n_value = 0x5;
            *x_value = 0x0;
            break;
        case FRL_RATE_4L_8G:
            *m_value = 0x7;
            *n_value = 0x3;
            *x_value = 0x555555;
            break;
        case FRL_RATE_4L_10G:
            *m_value = 0x9;
            *n_value = 0x4;
            *x_value = 0x2aaaaa;
            break;
        default:
            break;
    }
}

static hi_u32 get_frl_vco_freq(hi_u32 rate)
{
    hi_u32 vco_freq;

    switch (rate) {
        case FRL_RATE_3L_3G:
        case FRL_RATE_3L_6G:
        case FRL_RATE_4L_6G:
        case FRL_RATE_4L_12G:
            vco_freq = 3000000;
            break;
        case FRL_RATE_4L_8G:
            vco_freq = 2000000;
            break;
        case FRL_RATE_4L_10G:
            vco_freq = 2500000;
            break;
        default:
            vco_freq = 3000000;
            break;
    }

    return vco_freq;
}

static hi_u32 get_disable_dsc_tmds_cnt_val(struct fcg_div_params *phy_fcg,
    const struct phy_frl_tmds_clk *frl_tmds_clk)
{
    hi_u32 cnt1_target = 0x400;
    hi_u32 fcg_div;
    hi_u32 tmds_cnt_val;
    hi_u32 ref_pixel_clk;
    hi_u32 vco_clk;
    hi_u32 org_dsc_target;
    hi_u32 ratio;

    if (frl_tmds_clk->pixel_clk == 0) {
        HDMI_ERR("pixel clock is zero!\n");
        return 0;
    }

    vco_clk = get_frl_vco_freq(frl_tmds_clk->rate);
    ref_pixel_clk = frl_tmds_clk->pixel_clk / 4;
    org_dsc_target = frl_tmds_clk->pixel_clk / 4;
    ratio = org_dsc_target / ref_pixel_clk;

    /** tmds_cnt_val = (dsc_target / ref_pixel_clk) * cnt1_target;
     * dsc_target = org_dsc_target * n(color_depth: 24bit, n = 1;
     * color_depth: 30bit, n = 5 /4 ; color_depth: 36bit, n = 3 / 2;)
     *
     * fcg_div = vco_clk / tmds_clk - 1;
     * tmds_clk = ref_pixel_clk * n(color_depth: 24bit, n = 1;
     * color_depth: 30bit, n = 5 /4 ; color_depth: 36bit, n = 3 / 2;)
     */
    switch (frl_tmds_clk->color_depth) {
        case DEEP_COLOR_24BIT:
            fcg_div = vco_clk / ref_pixel_clk - 1;
            tmds_cnt_val = ratio * cnt1_target;
            break;
        case DEEP_COLOR_30BIT:
            fcg_div = vco_clk * 4 / (ref_pixel_clk * 5) - 1;
            tmds_cnt_val = ratio * 5 * cnt1_target / 4;
            break;
        case DEEP_COLOR_36BIT:
            fcg_div = vco_clk * 2 / (ref_pixel_clk * 3) - 1;
            tmds_cnt_val = ratio * 3 * cnt1_target / 2;
            break;
        default:
            fcg_div = vco_clk / ref_pixel_clk - 1;
            tmds_cnt_val = ratio * cnt1_target;
            break;
    }

    phy_fcg->cnt1_target = cnt1_target;
    phy_fcg->tmds_cnt_val = tmds_cnt_val;

    return fcg_div;
}

static hi_u32 get_enable_dsc_tmds_cnt_val(struct fcg_div_params *phy_fcg,
    const struct phy_frl_tmds_clk *frl_tmds_clk)
{
    hi_u32 cnt1_target;
    hi_u32 fcg_div;
    hi_u32 tmds_cnt_val ;
    hi_u32 dsc_target;
    hi_u32 i = 1;
    hi_u32 vco_clk;
    hi_u32 hctotal;
    hi_u32 htotal;
    hi_u32 n;
    hi_u32 pixel_clk;

    if (frl_tmds_clk->htotal == 0 ||
        frl_tmds_clk->pixel_clk == 0) {
        HDMI_ERR("pixel clk or htotal is zero!\n");
        return 0;
    }

    vco_clk = get_frl_vco_freq(frl_tmds_clk->rate);
    hctotal = frl_tmds_clk->hctotal;
    htotal = frl_tmds_clk->htotal;

    if (frl_tmds_clk->yuv_420 == HI_TRUE) {
        pixel_clk = frl_tmds_clk->pixel_clk * 2;
    } else {
        pixel_clk = frl_tmds_clk->pixel_clk;
    }

    dsc_target = (hi_u32)osal_div_u64((hi_u64)pixel_clk * hctotal, htotal * 4);

    n = (frl_tmds_clk->yuv_420 == HI_TRUE) ? 4 : 2;
    /** ref_pixel_clk = (frl_tmds_clk->yuv_420 == HI_TRUE) ?
     *      pixel_clk / 16 :
     *      pixel_clk / 8;
     * tmds_cnt_val = (dsc_target / ref_pixel_clk) * cnt1_target
     * tmds_cnt_val / cnt1_target = dsc_target / ref_pixel_clk
     * tmds_cnt_val / cnt1_target = hctotal * n / htotal;(yuv_420
     * is HI_TRUE, n = 4; yuv_420 is HI_FALSE, n = 2;)
     * tmds_cnt_val = hctotal * n;
     * cnt1_target = htotal;
     */
    cnt1_target = htotal;
    tmds_cnt_val = n * hctotal;

    fcg_div = vco_clk / dsc_target - 1;

    while (tmds_cnt_val < 1024 || cnt1_target < 1024) {
        tmds_cnt_val = tmds_cnt_val * i;
        cnt1_target = cnt1_target * i;
        i++;
    }

    phy_fcg->cnt1_target = cnt1_target;
    phy_fcg->tmds_cnt_val = tmds_cnt_val;

    return fcg_div;
}

static hi_u32 get_fcg_post_div(hi_u32 fcg_div, struct fcg_div_params *phy_fcg)
{
    hi_u32 fcg_post_div;
    hi_u32 fcg_div_tmp;

    if (fcg_div <= 112) {
        fcg_post_div = 0x0;
        fcg_div_tmp = fcg_div;
    } else if (fcg_div > 112 && fcg_div <= 224) {
        fcg_post_div = 0x1;
        fcg_div_tmp = fcg_div / 2;
    } else if (fcg_div > 224 && fcg_div <= 448) {
        fcg_post_div = 0x2;
        fcg_div_tmp = fcg_div / 4;
    } else {
        fcg_post_div = 0x3;
        fcg_div_tmp = fcg_div / 8;
    }

    phy_fcg->fcg_postdiv = fcg_post_div;
    return fcg_div_tmp;
}

static hi_void get_fcg_div_params(hi_u32 fcg_div, struct fcg_div_params *phy_fcg)
{
    hi_u8 fcg_divm = 0;
    hi_u8 fcg_divp = 0;
    hi_u8 fcg_div_tmp;

    fcg_div_tmp = get_fcg_post_div(fcg_div, phy_fcg);
    if (fcg_div_tmp <= 4 && fcg_div_tmp > 0) {
        fcg_divm = 4;
        fcg_divp = 0;
    } else if (fcg_div_tmp <= 12 && fcg_div_tmp > 4) {
        fcg_divm = fcg_div_tmp;
        fcg_divp = 0;
    } else if (fcg_div_tmp <= 24 && fcg_div_tmp > 12) {
        fcg_divp = 1;
        if ((fcg_div_tmp >> 1) <= 6) {
            fcg_divm = 6;
        } else {
            fcg_divm = fcg_div_tmp >> 1;
        }
    } else if (fcg_div_tmp > 24 && fcg_div_tmp <= 48) {
        fcg_divp = 2;
        if ((fcg_div_tmp >> 2) <= 6) {
            fcg_divm = 6;
        } else {
            fcg_divm = fcg_div_tmp >> 2;
        }
    } else if (fcg_div_tmp > 48 && fcg_div_tmp <= 96) {
        fcg_divp = 3;
        if ((fcg_div_tmp >> 3) <= 6) {
            fcg_divm = 6;
        } else {
            fcg_divm = fcg_div_tmp >> 3;
        }
    } else if (fcg_div_tmp > 96 && fcg_div_tmp <= 112) {
        fcg_divm = 12;
        fcg_divp = 3;
    }

    phy_fcg->fcg_divm = fcg_divm;
    phy_fcg->fcg_divp = fcg_divp;
}

static hi_u8 get_fcg_ki(hi_u32 cnt1_target)
{
    hi_u8 ki = 0;

    if (cnt1_target < 1024) {
        ki = 0xC;
    } else if (cnt1_target >= 1024 && cnt1_target < 1024 * 2) {
        ki = 0xB;
    } else if (cnt1_target >= 1024 * 2 && cnt1_target < 1024 * 4) {
        ki = 0xA;
    } else if (cnt1_target >= 1024 * 4 && cnt1_target < 1024 * 8) {
        ki = 0x9;
    } else if (cnt1_target >= 1024 * 8 && cnt1_target < 1024 * 16) {
        ki = 0x8;
    } else {
        ki = 0x7;
    }

    return ki;
}

static hi_void get_disable_dsc_fcg_params(struct fcg_div_params *phy_fcg,
    const struct phy_frl_tmds_clk *frl_tmds_clk)
{
    hi_u8 ki;
    hi_u32 fcg_div;

    /* get tmds cnt val && fcg div */
    fcg_div = get_disable_dsc_tmds_cnt_val(phy_fcg, frl_tmds_clk);
    /* get divm divp param */
    get_fcg_div_params(fcg_div, phy_fcg);
    /* get ki && lock value */
    ki = get_fcg_ki(phy_fcg->cnt1_target);

    phy_fcg->ki = ki;
    phy_fcg->lock_th = ki - 0x5;
}

static hi_void get_enable_dsc_fcg_params(struct fcg_div_params *phy_fcg,
    const struct phy_frl_tmds_clk *frl_tmds_clk)
{
    hi_u32 fcg_div;
    hi_u8 ki;

    if (frl_tmds_clk->pixel_clk == 0) {
        HDMI_ERR("pixel clock is zero!\n");
        return;
    }
    /* get tmds cnt val  &  fcg div */
    fcg_div = get_enable_dsc_tmds_cnt_val(phy_fcg, frl_tmds_clk);
    /* get divm divp param */
    get_fcg_div_params(fcg_div, phy_fcg);
    /* get ki  &&  lock value */
    ki = get_fcg_ki(phy_fcg->cnt1_target);

    phy_fcg->ki = ki;
    phy_fcg->lock_th = ki - 0x5;
}

static hi_void get_tmds_ssc_params(const struct phy_ssc *ssc, hi_u16 *mod_n, hi_u16 *mod_d)
{
    hi_u32 m = 0;
    hi_u32 n = 0;
    hi_u32 mn;
    hi_u32 mod_dn;
    hi_u32 ref_clk_div;
    hi_u32 pll_ref_clk;
    hi_u32 tmp_mod_n;
    hi_u32 tmp_mod_d;
    hi_u32 rem;

    if (ssc->ssc_freq == 0) {
        HDMI_ERR("Ssc Freq is zero\n");
        return;
    }

    mn = get_tmds_frac_param(ssc->tmds_clk, ssc->pixel_clk, ssc->color_depth, &m, &n);

    ref_clk_div = get_ref_clk_div(ssc->pixel_clk);
    pll_ref_clk = ssc->pixel_clk / phy_pow(2, ref_clk_div);

    /* pll_ref_clk * 10000 / (ssc->ssc_freq * 4) */
    mod_dn = pll_ref_clk * 2500 / ssc->ssc_freq;
    if (mod_dn % 10000 >= 5000) {
        tmp_mod_n = mod_dn / 10000 + 1;
    } else {
        tmp_mod_n = mod_dn / 10000;
    }

    /** Relative deviation = (MOD_D(0x80A0[15:0])*
     *  MOD_N(0x809C[31:16])) / (65536*(((M+1)*10)+(N+X)))
     */
    if (tmp_mod_n == 0) {
        HDMI_ERR("tmp_mod_n is zero\n");
        return;
    }

    tmp_mod_d = (hi_u32)osal_div_u64((hi_u64)phy_pow(2, 24) * mn * ssc->ssc_amp, tmp_mod_n);
    rem = tmp_mod_d % 100000;
    if (rem >= 50000) {
        tmp_mod_d = tmp_mod_d / 100000 + 1;
    } else {
        tmp_mod_d = tmp_mod_d / 100000;
    }

    *mod_n = tmp_mod_n;
    *mod_d = tmp_mod_d;

    HDMI_DBG("mod_n = %x, mod_d = %x\n", tmp_mod_n, tmp_mod_d);
}

static hi_void get_tmds_tpll_div_params(struct tpll_div_params *tpll_params,
    hi_u32 pixel_clk, hi_bool yuv_420, hi_u32 ctrl_id)
{
    hi_u32 pixel_protocol;

    /** YUV444:pixel input pixel foutpostdiv output
     * YUV420: pixel input 1/2pixel foutpostdiv output
     */
    pixel_protocol  = yuv_420 ? (2 * pixel_clk) : pixel_clk;
    if (yuv_420) {
        if (pixel_protocol >= 50000 && pixel_protocol < PIXEL_CLK_150000) {
            tpll_params->refdiv = 0x1;
            tpll_params->postdiv1 = 0x6;
            tpll_params->postdiv2 = 0x6;
            tpll_params->fbdiv = (ctrl_id == 1) ? 0x12 : 0x90;
        } else if (pixel_protocol >= PIXEL_CLK_150000 && pixel_protocol < PIXEL_CLK_300000) {
            tpll_params->refdiv = (ctrl_id == 1) ? 0x2 : 0x1;
            tpll_params->postdiv1 = 0x4;
            tpll_params->postdiv2 = 0x4;
            tpll_params->fbdiv = (ctrl_id == 1) ? 0x10 : 0x40;
        } else if (pixel_protocol >= PIXEL_CLK_300000 && pixel_protocol < PIXEL_CLK_600000) {
            tpll_params->refdiv = (ctrl_id == 1) ? 0x4 : 0x1;
            tpll_params->postdiv1 = 0x3;
            tpll_params->postdiv2 = 0x3;
            tpll_params->fbdiv = (ctrl_id == 1) ? 0x12 : 0x24;
        } else if (pixel_protocol >= PIXEL_CLK_600000 && pixel_protocol <= 1188000) {
            tpll_params->refdiv = (ctrl_id == 1) ? 0x8 : 0x1;
            tpll_params->postdiv1 = 0x2;
            tpll_params->postdiv2 = 0x2;
            tpll_params->fbdiv = 0x10;
        } else {
            HDMI_ERR("[YUV420]Pixel clock is exceed max pixel clock(1188M)!\n");
        }
    } else {
        if (pixel_protocol >= 50000 && pixel_protocol < PIXEL_CLK_150000) {
            tpll_params->refdiv = 0x1;
            tpll_params->postdiv1 = 0x4;
            tpll_params->postdiv2 = 0x4;
            tpll_params->fbdiv = (ctrl_id == 1) ? 0x10 : 0x80;
        } else if (pixel_protocol >= PIXEL_CLK_150000 && pixel_protocol < PIXEL_CLK_300000) {
            tpll_params->refdiv = (ctrl_id == 1) ? 0x2 : 0x1;
            tpll_params->postdiv1 = 0x3;
            tpll_params->postdiv2 = 0x3;
            tpll_params->fbdiv = (ctrl_id == 1) ? 0x12 : 0x48;
        } else if (pixel_protocol >= PIXEL_CLK_300000 &&  pixel_protocol < PIXEL_CLK_600000) {
            tpll_params->refdiv = (ctrl_id == 1) ? 0x4 : 0x1;
            tpll_params->postdiv1 = 0x2;
            tpll_params->postdiv2 = 0x2;
            tpll_params->fbdiv = (ctrl_id == 1) ? 0x12 : 0x20;
        } else {
            HDMI_DBG("Pixel clock is exceed max pixel clock(600M)!\n");
        }
    }
}

static hi_u8 get_ref_clk_sel(hi_u32 pixel_clk, hi_u32 mode, hi_bool tpll_enable)
{
    hi_u8 ref_clk_sel;

    /* tpll bypass need check with current phy work mode */
    ref_clk_sel = (mode == PHY_SET_MODE_TMDS) ?  0x0 : 0x3;
    /* tpll enable */
    if (tpll_enable) {
        ref_clk_sel = 0x5;
    }

    /* tpll must bypass, HW limited */
    if (pixel_clk < 100000 &&  mode == PHY_SET_MODE_TMDS) {
        ref_clk_sel = 0x0;
    }

    return ref_clk_sel;
}

static hi_void get_frl_tpll_div_params(struct tpll_div_params *tpll_params)
{
    /* 24M input Fout_post_div output 24M */
    tpll_params->refdiv = 0x1;
    tpll_params->fbdiv = 0x24;
    tpll_params->postdiv1 = 0x6;
    tpll_params->postdiv2 = 0x6;
}

static hi_void phy_hw_bus_reset(struct hdmitx_phy *hdmi_phy)
{
    hi_void *crg_reg = HI_NULL;
    hi_u32 crg_addr;

    if (hdmi_phy->phy_id == PHY_ID_0) {
        crg_addr = PHY0_CRG_ADDR;
    } else {
        crg_addr = PHY1_CRG_ADDR;
    }

    crg_reg = osal_ioremap_nocache(crg_addr, 4);
    if (IS_ERR(crg_reg)) {
        HDMI_ERR("Failed to get memory resource\n");
        return;
    }

    hdmi_set(crg_reg, 0, APB_RESET);
    osal_udelay(1);
    hdmi_clr(crg_reg, 0, APB_RESET);

    osal_iounmap(crg_reg);
}

static hi_void phy_hw_reset(struct hdmitx_phy *hdmi_phy)
{
    /* step 1: APB reset */
    phy_hw_bus_reset(hdmi_phy);

    /* step 2: Dphy reset */
    hdmi_set(hdmi_phy->regs, REG_SWRESET, REG_GLOBAL_RESET_M);
    osal_udelay(1);
    hdmi_clr(hdmi_phy->regs, REG_SWRESET, REG_GLOBAL_RESET_M);

    /* step 3: Aphy reset  */
    hdmi_clr(hdmi_phy->regs, REG_RESETN, REG_STB_RESETN_M);
    osal_udelay(1);
    hdmi_set(hdmi_phy->regs, REG_RESETN, REG_STB_RESETN_M);
}

static hi_void phy_hw_init(struct hdmitx_phy *hdmi_phy, hi_u32 pixel_clk, hi_u32 rate, hi_bool tpll_enable)
{
    hi_u8 ref_clk_sel;
    hi_u32 mode = hdmi_phy->mode;

    ref_clk_sel = get_ref_clk_sel(pixel_clk, mode, tpll_enable);
    /** step1: Dphy clock internal set;
     *  1).dac clock gate enable;
     *  2).select mod clk;
     *  3).select 24MHz clock as reference clock;
     */
    hdmi_set(hdmi_phy->regs, REG_SWRESET, REG_DAC_CLOCK_GAT_M);
    hdmi_set(hdmi_phy->regs, REG_CLKSET, REG_P_MODCLK_SEL_M);
    hdmi_set(hdmi_phy->regs, REG_REFCLKSEL, REG_I_REF_CLK_SEL_M);

    /* step2:Aphy access mode set */
    hdmi_write_bits(hdmi_phy->regs, REG_STBOPT, REG_STB_DELAY0_M, 0x2);
    hdmi_write_bits(hdmi_phy->regs, REG_STBOPT, REG_STB_DELAY1_M, 0x2);
    hdmi_write_bits(hdmi_phy->regs, REG_STBOPT, REG_STB_DELAY2_M, 0x2);

    /* Aphy PLL setting
     * step 3: aphy internel block enable
     */
    stb_writeb(hdmi_phy, APHY_CS_8, APHY_OFFSET_0, 0xff);
    stb_writeb(hdmi_phy, APHY_CS_8, APHY_OFFSET_1, 0x31);

    /* step 4: Aphy ref clock select.
     * Note: we must make the source clock is stable before we run to here
     */
    if (hdmi_phy->mode == PHY_SET_MODE_TMDS) {
        stb_writeb(hdmi_phy, APHY_CS_8, APHY_OFFSET_2,
            aphy_refclksel(ref_clk_sel) | aphy_refclkdiv(get_ref_clk_div(pixel_clk)));
    } else {
        stb_writeb(hdmi_phy, APHY_CS_8, APHY_OFFSET_2,
            aphy_refclksel(ref_clk_sel) | aphy_refclkdiv(0x00));
    }
    /**
     *  step 5: Aphy pll loop parameter setting,
     *  should be provided by hw test guys
     */
    stb_writeb(hdmi_phy, APHY_CS_8, APHY_OFFSET_3, 0x1);
    stb_writeb(hdmi_phy, APHY_CS_8, APHY_OFFSET_4, 0x82);
    stb_writeb(hdmi_phy, APHY_CS_8, APHY_OFFSET_5, 0x6D);
    stb_writeb(hdmi_phy, APHY_CS_8, APHY_OFFSET_6, 0x1D);
    stb_writeb(hdmi_phy, APHY_CS_8, APHY_OFFSET_7, 0x12);
    stb_writeb(hdmi_phy, APHY_CS_8, APHY_OFFSET_8, 0x84);
    stb_writeb(hdmi_phy, APHY_CS_8, APHY_OFFSET_9, 0x80);
    stb_writeb(hdmi_phy, APHY_CS_8, APHY_OFFSET_A, 0x3);
    stb_writeb(hdmi_phy, APHY_CS_8, APHY_OFFSET_C, 0x14);
    /**
     *  step 6: fractional block should be enable
     *  if it 's FRL 4L10G and 4L8G
     */
    if (hdmi_phy->mode == PHY_SET_MODE_FRL && (rate == FRL_RATE_4L_8G || rate == FRL_RATE_4L_10G)) {
        stb_writeb(hdmi_phy, APHY_CS_8, APHY_OFFSET_D, 0xfd);
    } else {
        stb_writeb(hdmi_phy, APHY_CS_8, APHY_OFFSET_D, 0xf0);
    }
    /**
     *  step 7: Aphy pll
     */
    stb_writeb(hdmi_phy, APHY_CS_8, APHY_OFFSET_E, 0x11);
    stb_writeb(hdmi_phy, APHY_CS_9, APHY_OFFSET_0, 0x07);
    /**
     *  step 8: Aphy pll tmds divider setting
     */
    if (hdmi_phy->mode == PHY_SET_MODE_TMDS) {
        stb_writeb(hdmi_phy, APHY_CS_9, APHY_OFFSET_7, (PHY_TMDS_OUTEN << 3) | get_tmds_clk_div(rate));
    } else {
        stb_writeb(hdmi_phy, APHY_CS_9, APHY_OFFSET_7, 0x20);
    }

    /** step 9: Aphy frl clock output enable
     */
    if (hdmi_phy->mode == PHY_SET_MODE_FRL) {
        stb_writeb(hdmi_phy, APHY_CS_9, APHY_OFFSET_E, 0x0c);
    }
    /**
     *  step 10: Turn off the PHY to wait until VSYNC outputs data
     *  after OE is turned on.
     */
    hdmi_write_bits(hdmi_phy->regs, REG_HDMI_OE_CFG, REG_CFG_OE_SYNC_EN_M, HI_FALSE);
}


static hi_void phy_hw_tmds_clk_set(struct hdmitx_phy *hdmi_phy, const struct phy_tmds *tmds)
{
    hi_u32 p_div_h20;
    hi_bool hdmi2_en = HI_FALSE;
    hi_u32 m, n;

    get_tmds_frac_param(tmds->tmds_clk, tmds->pixel_clk, tmds->color_depth, &m, &n);
    p_div_h20 = get_tmds_clk_div(tmds->tmds_clk);
    hdmi2_en = (tmds->tmds_clk > 340000) ? HI_TRUE : HI_FALSE;
    hdmi_write_bits(hdmi_phy->regs, REG_FCDSTEPSET, REG_P_DIVN_H20_M, p_div_h20);
    hdmi_clr(hdmi_phy->regs, REG_FCDSTEPSET, REG_FCDSTEPSET_UNUSED_M);
    hdmi_clr(hdmi_phy->regs, REG_FCDSTEPSET, REG_UP_SAMPLER_RATIO_SEL_M);

    hdmi_write_bits(hdmi_phy->regs, REG_FDIVMANUAL, REG_I_MANUAL_EN_M, 0x0e);
    hdmi_write_bits(hdmi_phy->regs, REG_FDIVMANUAL, REG_I_MDIV_M, m);
    hdmi_write_bits(hdmi_phy->regs, REG_FDIVSTAT1, REG_I_FDIV_IN_N_M, n);
    hdmi_write_bits(hdmi_phy->regs, REG_FDIVSTAT1, REG_I_FDIV_IN_X_M, 0x0);
    hdmi_write_bits(hdmi_phy->regs, REG_HDMI_MODE, REG_REG_HDMI_MODE_EN_M, hdmi2_en);
    /* FDIV init */ /* set n,x to aphy */
    hdmi_clr(hdmi_phy->regs, REG_FDIVSET0, REG_INIT_M);
    hdmi_clr(hdmi_phy->regs, REG_FDIVSET0, REG_EN_CTRL_M);
    hdmi_clr(hdmi_phy->regs, REG_FDIVSET0, REG_EN_MOD_M);
    hdmi_clr(hdmi_phy->regs, REG_FDIVSET0, REG_EN_SDM_M);

    hdmi_set(hdmi_phy->regs, REG_FDIVSET0, REG_INIT_M);
    /* delay 1us */
    osal_udelay(1);
    hdmi_clr(hdmi_phy->regs, REG_FDIVSET0, REG_INIT_M);
}

static hi_bool phy_hw_wait_fcon_done(struct hdmitx_phy *hdmi_phy)
{
    hi_u8 fcon_result[4] = {0};
    hi_u32 count = 0;
    hi_bool ret;

    while (count <= 3) {
        fcon_result[0] = hdmi_read_bits(hdmi_phy->regs, REG_FCSTAT, REG_CLK_OK_M);
        fcon_result[1] = hdmi_read_bits(hdmi_phy->regs, REG_FCSTAT, REG_BUSY_M);
        fcon_result[2] = hdmi_read_bits(hdmi_phy->regs, REG_FCSTAT, REG_DONE_M);
        fcon_result[3] = hdmi_read_bits(hdmi_phy->regs, REG_FCSTAT, REG_ERROR_M);
        if (fcon_result[1] == 0) {
            break;
        }

        osal_msleep(1); /* need confirm */
        count++;
    }

    if (fcon_result[3] == 0 && fcon_result[2] == 1 &&
        fcon_result[1] == 0 && fcon_result[0] == 1) {
        HDMI_DBG("set fcon is done!\n");
        ret = HI_TRUE;
    } else if (fcon_result[3] == 1) {
        HDMI_DBG("set fcon error!\n");
        ret = HI_FALSE;
    } else {
        HDMI_DBG("set fcon failed!\n");
        ret = HI_FALSE;
    }

    return ret;
}

static int phy_hw_fcon_param_set(struct hdmitx_phy *hdmi_phy, hi_u32 rate)
{
    int ret;

    /* step1: */
    hdmi_write_bits(hdmi_phy->regs, REG_CTSET0, REG_I_ENABLE_M, HI_TRUE);
    hdmi_clr(hdmi_phy->regs, REG_CTSET0, REG_I_RUN_M);
    hdmi_clr(hdmi_phy->regs, REG_CTSET0, REG_CTSET0_UNUSED_M);

    /* step 2: */
    hdmi_write_bits(hdmi_phy->regs, REG_FCCNTR0, REG_I_VCO_ST_WAIT_LEN_M, 0x50);
    hdmi_write_bits(hdmi_phy->regs, REG_FCCNTR0, REG_I_VCO_END_WAIT_LEN_M, 0x50);
    hdmi_write_bits(hdmi_phy->regs, REG_FCCNTR0, REG_I_REF_CNT_LEN_M, 0x80);
    hdmi_write_bits(hdmi_phy->regs, REG_FCOPT, REG_I_CT_SEL_M, HI_TRUE);
    hdmi_write_bits(hdmi_phy->regs, REG_FCOPT, REG_I_CLKDET_SEL_M, HI_TRUE);
    hdmi_write_bits(hdmi_phy->regs, REG_FCOPT, REG_I_CT_MODE_M, 0x2);
    hdmi_clr(hdmi_phy->regs, REG_FCOPT, REG_FCOPT_UNUSED_1_M);
    hdmi_write_bits(hdmi_phy->regs, REG_FCOPT, REG_I_CT_EN_M, 0x1);
    hdmi_clr(hdmi_phy->regs, REG_FCOPT, REG_FCOPT_UNUSED_2_M);
    hdmi_clr(hdmi_phy->regs, REG_FCOPT, REG_I_CT_IDX_SEL_M);
    hdmi_clr(hdmi_phy->regs, REG_FCOPT, REG_FCOPT_UNUSED_M);
    if (hdmi_phy->mode == PHY_SET_MODE_TMDS) {
        hdmi_write_bits(hdmi_phy->regs, REG_FCCNTR1, REG_I_REF_CNT_M, get_tmds_fcon_param(hdmi_phy, rate));
    } else {
        hdmi_write_bits(hdmi_phy->regs, REG_FCCNTR1, REG_I_REF_CNT_M, get_frl_fcon_param(hdmi_phy, rate));
    }
    hdmi_write_bits(hdmi_phy->regs, REG_FCCONTINSET1, REG_P_CONTIN_UPD_RATE_M, 0x960);
    hdmi_clr(hdmi_phy->regs, REG_FCCONTINSET1, REG_P_CONTIN_UPD_TIME_M);
    hdmi_write_bits(hdmi_phy->regs, REG_FCCONTINSET2, REG_P_CONTIN_UPD_TH_DN_M, 0x5);
    hdmi_clr(hdmi_phy->regs, REG_FCCONTINSET2, REG_FCCONTINSET2_UNUSED_M);
    hdmi_write_bits(hdmi_phy->regs, REG_FCCONTINSET2, REG_P_CONTIN_UPD_TH_UP_M, 0x3ff);
    hdmi_write_bits(hdmi_phy->regs, REG_FCCONTINSET0, REG_P_CONTIN_UPD_EN_M, 0x1);
    hdmi_clr(hdmi_phy->regs, REG_FCCONTINSET0, REG_P_CONTIN_UPD_OPT_M);
    hdmi_write_bits(hdmi_phy->regs, REG_FCCONTINSET0, REG_P_CONTIN_UPD_POL_M, 0x1);
    hdmi_clr(hdmi_phy->regs, REG_FCCONTINSET0, REG_FCCONTINSET0_UNUSED_M);
    hdmi_write_bits(hdmi_phy->regs, REG_FCCONTINSET0, REG_P_CONTIN_UPD_STEP_M, 0x2);
    hdmi_write_bits(hdmi_phy->regs, REG_CTSET0, REG_I_ENABLE_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_CTSET0, REG_I_RUN_M, 0x1);
    hdmi_clr(hdmi_phy->regs, REG_CTSET0, REG_CTSET0_UNUSED_M);
    if (phy_hw_wait_fcon_done(hdmi_phy)) {
        ret = 0;
    } else {
        ret = -1;
    }

    return ret;
}

static hi_void phy_hw_tmds_aphy_spec_set(struct hdmitx_phy *hdmi_phy, struct tmds_spec_params *tmds)
{
    struct aphy_spec_params *data = &tmds->data.aphy;
    struct aphy_spec_params *clk = &tmds->clock.aphy;

    /* data drv set  */
    stb_writeb(hdmi_phy, APHY_CS_012, APHY_OFFSET_0, data->offset_0);
    stb_writeb(hdmi_phy, APHY_CS_012, APHY_OFFSET_1, data->offset_1);
    stb_writeb(hdmi_phy, APHY_CS_012, APHY_OFFSET_2, data->offset_2);
    stb_writeb(hdmi_phy, APHY_CS_012, APHY_OFFSET_3, data->offset_3);
    stb_writeb(hdmi_phy, APHY_CS_012, APHY_OFFSET_4, data->offset_4);
    stb_writeb(hdmi_phy, APHY_CS_012, APHY_OFFSET_8, data->offset_8);
    stb_writeb(hdmi_phy, APHY_CS_012, APHY_OFFSET_9, data->offset_9);
    stb_writeb(hdmi_phy, APHY_CS_012, APHY_OFFSET_A, data->offset_a);
    stb_writeb(hdmi_phy, APHY_CS_012, APHY_OFFSET_B, data->offset_b);
    /* clk drv set */
    stb_writeb(hdmi_phy, APHY_CS_3, APHY_OFFSET_0, clk->offset_0);
    stb_writeb(hdmi_phy, APHY_CS_3, APHY_OFFSET_1, clk->offset_1);
    stb_writeb(hdmi_phy, APHY_CS_3, APHY_OFFSET_2, clk->offset_2);
    stb_writeb(hdmi_phy, APHY_CS_3, APHY_OFFSET_3, clk->offset_3);
    stb_writeb(hdmi_phy, APHY_CS_3, APHY_OFFSET_4, clk->offset_4);
    stb_writeb(hdmi_phy, APHY_CS_3, APHY_OFFSET_8, clk->offset_8);
    stb_writeb(hdmi_phy, APHY_CS_3, APHY_OFFSET_9, clk->offset_9);
    stb_writeb(hdmi_phy, APHY_CS_3, APHY_OFFSET_A, clk->offset_a);
    stb_writeb(hdmi_phy, APHY_CS_3, APHY_OFFSET_B, clk->offset_b);
}

static hi_void phy_hw_tmds_dphy_spec_set(struct hdmitx_phy *hdmi_phy, struct tmds_spec_params *tmds)
{
    struct dphy_spec_params *data = &tmds->data.dphy;
    struct dphy_spec_params *clk = &tmds->clock.dphy;
    struct dphy_spec_en *data_en = &tmds->data.en;
    struct dphy_spec_en *clk_en = &tmds->clock.en;

    /* select dphy drv set mode */
    hdmi_write_bits(hdmi_phy->regs, REG_TMDS_DRV_CFG_CH0, REG_CFG_HDMI_FFE_SEL_M, 0x1);
    /* dphy data drv set */
    hdmi_write_bits(hdmi_phy->regs, REG_TMDS_DRV_CFG_CH0, REG_CFG_DRV_POST2_CH0_M, data->drv_post2);
    hdmi_write_bits(hdmi_phy->regs, REG_TMDS_DRV_CFG_CH0, REG_CFG_DRV_POST1_CH0_M, data->drv_post1);
    hdmi_write_bits(hdmi_phy->regs, REG_TMDS_DRV_CFG_CH0, REG_CFG_DRV_M_CH0_M, data->drv_main);
    hdmi_write_bits(hdmi_phy->regs, REG_TMDS_DRV_CFG_CH0, REG_CFG_DRV_PRE_CH0_M, data->drv_pre);

    hdmi_write_bits(hdmi_phy->regs, REG_TMDS_DRV_CFG_CH1, REG_CFG_DRV_POST2_CH1_M, data->drv_post2);
    hdmi_write_bits(hdmi_phy->regs, REG_TMDS_DRV_CFG_CH1, REG_CFG_DRV_POST1_CH1_M, data->drv_post1);
    hdmi_write_bits(hdmi_phy->regs, REG_TMDS_DRV_CFG_CH1, REG_CFG_DRV_M_CH1_M, data->drv_main);
    hdmi_write_bits(hdmi_phy->regs, REG_TMDS_DRV_CFG_CH1, REG_CFG_DRV_PRE_CH1_M, data->drv_pre);

    hdmi_write_bits(hdmi_phy->regs, REG_TMDS_DRV_CFG_CH2, REG_CFG_DRV_POST2_CH2_M, data->drv_post2);
    hdmi_write_bits(hdmi_phy->regs, REG_TMDS_DRV_CFG_CH2, REG_CFG_DRV_POST1_CH2_M, data->drv_post1);
    hdmi_write_bits(hdmi_phy->regs, REG_TMDS_DRV_CFG_CH2, REG_CFG_DRV_M_CH2_M, data->drv_main);
    hdmi_write_bits(hdmi_phy->regs, REG_TMDS_DRV_CFG_CH2, REG_CFG_DRV_PRE_CH2_M, data->drv_pre);

    /* dphy clk drv set */
    hdmi_write_bits(hdmi_phy->regs, REG_TMDS_DRV_CFG_CH3, REG_CFG_DRV_POST2_CH3_M, clk->drv_post2);
    hdmi_write_bits(hdmi_phy->regs, REG_TMDS_DRV_CFG_CH3, REG_CFG_DRV_POST1_CH3_M, clk->drv_post1);
    hdmi_write_bits(hdmi_phy->regs, REG_TMDS_DRV_CFG_CH3, REG_CFG_DRV_M_CH3_M, clk->drv_main);
    hdmi_write_bits(hdmi_phy->regs, REG_TMDS_DRV_CFG_CH3, REG_CFG_DRV_PRE_CH3_M, clk->drv_pre);

    /* dphy data drv enable */
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C2_PRE_EN_M, data_en->drv_pre_en);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C2_POST1_EN_M, data_en->drv_post1_en);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C2_POST2_EN_M, data_en->drv_post2_en);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C1_PRE_EN_M, data_en->drv_pre_en);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C1_POST1_EN_M, data_en->drv_post1_en);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C1_POST2_EN_M, data_en->drv_post2_en);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C0_PRE_EN_M, data_en->drv_pre_en);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C0_POST1_EN_M, data_en->drv_post1_en);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C0_POST2_EN_M, data_en->drv_post2_en);

    /* dphy clock drv enable */
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C3_PRE_EN_M, clk_en->drv_pre_en);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C3_POST1_EN_M, clk_en->drv_post1_en);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C3_POST2_EN_M, clk_en->drv_post2_en);
}

static hi_void phy_hw_frl_aphy_spec_set(struct hdmitx_phy *hdmi_phy, struct frl_spec_params *frl)
{
    struct aphy_spec_params *data = &frl->aphy;

    /* data drv set  */
    stb_writeb(hdmi_phy, APHY_CS_0123, APHY_OFFSET_0, data->offset_0);
    stb_writeb(hdmi_phy, APHY_CS_0123, APHY_OFFSET_1, data->offset_1);
    stb_writeb(hdmi_phy, APHY_CS_0123, APHY_OFFSET_2, data->offset_2);
    stb_writeb(hdmi_phy, APHY_CS_0123, APHY_OFFSET_3, data->offset_3);
    stb_writeb(hdmi_phy, APHY_CS_0123, APHY_OFFSET_4, data->offset_4);
    stb_writeb(hdmi_phy, APHY_CS_0123, APHY_OFFSET_8, data->offset_8);
    stb_writeb(hdmi_phy, APHY_CS_0123, APHY_OFFSET_9, data->offset_9);
    stb_writeb(hdmi_phy, APHY_CS_0123, APHY_OFFSET_A, data->offset_a);
    stb_writeb(hdmi_phy, APHY_CS_0123, APHY_OFFSET_B, data->offset_b);
}

static hi_void phy_hw_frl_dphy_spec_set(struct hdmitx_phy *hdmi_phy, struct frl_spec_params *frl)
{
    struct dphy_spec_en *en = &frl->en;
    struct dphy_spec_params *ffe = &frl->ffe[0];

    /* select dphy drv set mode */
    hdmi_write_bits(hdmi_phy->regs, REG_TMDS_DRV_CFG_CH0, REG_CFG_HDMI_FFE_SEL_M, 0x0);

    /* dphy ffe0~3 drv set */
    hdmi_write_bits(hdmi_phy->regs, REG_FFE0_CFG, REG_CFG_FFE0_DRV_POST2_M, ffe[0].drv_post2);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE0_CFG, REG_CFG_FFE0_DRV_POST1_M, ffe[0].drv_post1);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE0_CFG, REG_CFG_FFE0_DRV_M_M, ffe[0].drv_main);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE0_CFG, REG_CFG_FFE0_DRV_PRE_M, ffe[0].drv_pre);

    hdmi_write_bits(hdmi_phy->regs, REG_FFE1_CFG, REG_CFG_FFE1_DRV_POST2_M, ffe[1].drv_post2);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE1_CFG, REG_CFG_FFE1_DRV_POST1_M, ffe[1].drv_post1);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE1_CFG, REG_CFG_FFE1_DRV_M_M, ffe[1].drv_main);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE1_CFG, REG_CFG_FFE1_DRV_PRE_M, ffe[1].drv_pre);

    hdmi_write_bits(hdmi_phy->regs, REG_FFE2_CFG, REG_CFG_FFE2_DRV_POST2_M, ffe[2].drv_post2);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE2_CFG, REG_CFG_FFE2_DRV_POST1_M, ffe[2].drv_post1);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE2_CFG, REG_CFG_FFE2_DRV_M_M, ffe[2].drv_main);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE2_CFG, REG_CFG_FFE2_DRV_PRE_M, ffe[2].drv_pre);

    hdmi_write_bits(hdmi_phy->regs, REG_FFE3_CFG, REG_CFG_FFE3_DRV_POST2_M, ffe[3].drv_post2);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE3_CFG, REG_CFG_FFE3_DRV_POST1_M, ffe[3].drv_post1);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE3_CFG, REG_CFG_FFE3_DRV_M_M, ffe[3].drv_main);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE3_CFG, REG_CFG_FFE3_DRV_PRE_M, ffe[3].drv_pre);

    /* dphy data drv enable */
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C3_PRE_EN_M, en->drv_pre_en);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C3_POST1_EN_M, en->drv_post1_en);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C3_POST2_EN_M, en->drv_post2_en);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C2_PRE_EN_M, en->drv_pre_en);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C2_POST1_EN_M, en->drv_post1_en);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C2_POST2_EN_M, en->drv_post2_en);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C1_PRE_EN_M, en->drv_pre_en);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C1_POST1_EN_M, en->drv_post1_en);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C1_POST2_EN_M, en->drv_post2_en);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C0_PRE_EN_M, en->drv_pre_en);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C0_POST1_EN_M, en->drv_post1_en);
    hdmi_write_bits(hdmi_phy->regs, REG_FFE_EN_CFG, REG_CFG_C0_POST2_EN_M, en->drv_post2_en);
}

static hi_void phy_hw_tpll_params_set(struct hdmitx_phy *hdmi_phy, struct tpll_div_params *tpll_params)
{
#ifdef HI_HDMI_TEE_SUPPORT
    hi_u32 tmp;
    hi_u32 tmp_addr;

    if (hdmi_phy->phy_id == 0) {
        tmp_addr = TPLL0_CRG_ADDR;
    } else {
        tmp_addr = TPLL1_CRG_ADDR;
    }

    /* set postdiv1 and  postdiv2 */
    tmp = hdmi_readl(hdmi_phy->tpll_regs, REG_TPLL_1);
    tmp &= ~(REG_PLL_POSTDIV1_M | REG_PLL_POSTDIV2_M);
    tmp |= reg_pll_postdiv1(tpll_params->postdiv1) |
        reg_pll_postdiv2(tpll_params->postdiv2);
    hi_drv_mbx_write_hrf_reg(tmp_addr + REG_TPLL_1, tmp);
    /* set refdiv and  fbdiv */
    tmp = hdmi_readl(hdmi_phy->tpll_regs, REG_TPLL_2);
    tmp &= ~(REG_PLL_REFDIV_M | REG_PLL_FBDIV_M);
    tmp |= reg_pll_refdiv(tpll_params->refdiv) |
        reg_pll_fbdiv(tpll_params->fbdiv);
    hi_drv_mbx_write_hrf_reg(tmp_addr + REG_TPLL_2, tmp);
    /* TPPL disable/enable */
    tmp = hdmi_readl(hdmi_phy->tpll_regs, REG_TPLL_2);
    tmp &= ~REG_PLL_EN_M;
    hi_drv_mbx_write_hrf_reg(tmp_addr + REG_TPLL_2, tmp);
    tmp = hdmi_readl(hdmi_phy->tpll_regs, REG_TPLL_2);
    tmp |= REG_PLL_EN_M;
    hi_drv_mbx_write_hrf_reg(tmp_addr + REG_TPLL_2, tmp);
#else
    /* set postdiv1 and  postdiv2 */
    hdmi_write_bits(hdmi_phy->tpll_regs, REG_TPLL_1, REG_PLL_POSTDIV1_M, tpll_params->postdiv1);
    hdmi_write_bits(hdmi_phy->tpll_regs, REG_TPLL_1, REG_PLL_POSTDIV2_M, tpll_params->postdiv2);

    /* set refdiv and  fbdiv */
    hdmi_write_bits(hdmi_phy->tpll_regs, REG_TPLL_2, REG_PLL_REFDIV_M, tpll_params->refdiv);
    hdmi_write_bits(hdmi_phy->tpll_regs, REG_TPLL_2, REG_PLL_FBDIV_M, tpll_params->fbdiv);
    /* TPPL disable/enable */
    hdmi_clr(hdmi_phy->tpll_regs, REG_TPLL_2, REG_PLL_EN_M);
    hdmi_set(hdmi_phy->tpll_regs, REG_TPLL_2, REG_PLL_EN_M);
#endif
}


static hi_void phy_hw_data_channel_enable(struct hdmitx_phy *hdmi_phy, hi_u32 rate)
{
    /* hs fifo enable hs link external */
    hdmi_write_bits(hdmi_phy->regs, REG_HSSET, REG_P_HSSET_M, 0x3);
    /* transmitter fifo and up sampler enable */
    hdmi_write_bits(hdmi_phy->regs, REG_TXFIFOSET0, REG_P_PR_EN_H20_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_TXFIFOSET0, REG_P_ENABLE_H20_M, 0x1);
    hdmi_clr(hdmi_phy->regs, REG_TXFIFOSET0, REG_TXFIFOSET0_UNUSED_M);

    if (hdmi_phy->mode == PHY_SET_MODE_TMDS) {
        /* hs input data selection */
        hdmi_clr(hdmi_phy->regs, REG_TXDATAOUTSEL, REG_P_CH_OUT_SEL_M);
        stb_set_bits(hdmi_phy, APHY_CS_0123, APHY_OFFSET_0, APHY_DATA_CHANNEL_EN_M, aphy_data_channel_en(0));
        stb_set_bits(hdmi_phy, APHY_CS_0123, APHY_OFFSET_0, APHY_DATA_CHANNEL_EN_M, aphy_data_channel_en(1));
    } else {
        hdmi_write_bits(hdmi_phy->regs, REG_TXDATAOUTSEL, REG_P_CH_OUT_SEL_M, 0x1);
        switch (rate) {
            case FRL_RATE_3L_3G:
            case FRL_RATE_3L_6G:
                stb_set_bits(hdmi_phy, APHY_CS_0123, APHY_OFFSET_0, APHY_DATA_CHANNEL_EN_M, aphy_data_channel_en(0));
                stb_set_bits(hdmi_phy, APHY_CS_012, APHY_OFFSET_0, APHY_DATA_CHANNEL_EN_M, aphy_data_channel_en(1));
                break;
            case FRL_RATE_4L_6G:
            case FRL_RATE_4L_8G:
            case FRL_RATE_4L_10G:
            case FRL_RATE_4L_12G:
                stb_set_bits(hdmi_phy, APHY_CS_0123, APHY_OFFSET_0, APHY_DATA_CHANNEL_EN_M, aphy_data_channel_en(0));
                stb_set_bits(hdmi_phy, APHY_CS_0123, APHY_OFFSET_0, APHY_DATA_CHANNEL_EN_M, aphy_data_channel_en(1));
                break;
            default:
                break;
        }
    }
}

static hi_void phy_hw_dcc_qec_init(struct hdmitx_phy *hdmi_phy)
{
    /* defaut value */
}

static hi_void phy_hw_ch0_dcc_qec_set(struct hdmitx_phy *hdmi_phy)
{
    /* channel 0 dcc setting */
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH0, REG_CFG_DCC3_OVR_CH0_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH0, REG_CFG_DCC3_VALUE_CH0_M, 0x28);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH0, REG_CFG_DCC2_OVR_CH0_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH0, REG_CFG_DCC2_VALUE_CH0_M, 0x28);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH0, REG_CFG_DCC1_OVR_CH0_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH0, REG_CFG_DCC1_VALUE_CH0_M, 0x28);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH0, REG_CFG_DCC0_OVR_CH0_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH0, REG_CFG_DCC0_VALUE_CH0_M, 0x28);
    /* channel 0 qec setting */
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH0, REG_CFG_QEC3_OVR_CH0_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH0, REG_CFG_QEC3_VALUE_CH0_M, 0x0);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH0, REG_CFG_QEC2_OVR_CH0_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH0, REG_CFG_QEC2_VALUE_CH0_M, 0x0);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH0, REG_CFG_QEC1_OVR_CH0_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH0, REG_CFG_QEC1_VALUE_CH0_M, 0x0);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH0, REG_CFG_QEC0_OVR_CH0_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH0, REG_CFG_QEC0_VALUE_CH0_M, 0x1f);
}

static hi_void phy_hw_ch1_dcc_qec_set(struct hdmitx_phy *hdmi_phy)
{
    /* channel 1 dcc setting */
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH1, REG_CFG_DCC3_OVR_CH1_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH1, REG_CFG_DCC3_VALUE_CH1_M, 0x28);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH1, REG_CFG_DCC2_OVR_CH1_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH1, REG_CFG_DCC2_VALUE_CH1_M, 0x28);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH1, REG_CFG_DCC1_OVR_CH1_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH1, REG_CFG_DCC1_VALUE_CH1_M, 0x28);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH1, REG_CFG_DCC0_OVR_CH1_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH1, REG_CFG_DCC0_VALUE_CH1_M, 0x28);
    /* channel 1 qec setting */
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH1, REG_CFG_QEC3_OVR_CH1_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH1, REG_CFG_QEC3_VALUE_CH1_M, 0x0);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH1, REG_CFG_QEC2_OVR_CH1_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH1, REG_CFG_QEC2_VALUE_CH1_M, 0x0);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH1, REG_CFG_QEC1_OVR_CH1_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH1, REG_CFG_QEC1_VALUE_CH1_M, 0x0);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH1, REG_CFG_QEC0_OVR_CH1_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH1, REG_CFG_QEC0_VALUE_CH1_M, 0x1f);
}

static hi_void phy_hw_ch2_dcc_qec_set(struct hdmitx_phy *hdmi_phy)
{
    /* channel 2 dcc setting */
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH2, REG_CFG_DCC3_OVR_CH2_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH2, REG_CFG_DCC3_VALUE_CH2_M, 0x28);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH2, REG_CFG_DCC2_OVR_CH2_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH2, REG_CFG_DCC2_VALUE_CH2_M, 0x28);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH2, REG_CFG_DCC1_OVR_CH2_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH2, REG_CFG_DCC1_VALUE_CH2_M, 0x28);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH2, REG_CFG_DCC0_OVR_CH2_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH2, REG_CFG_DCC0_VALUE_CH2_M, 0x28);
    /* channel 2 qec setting */
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH2, REG_CFG_QEC3_OVR_CH2_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH2, REG_CFG_QEC3_VALUE_CH2_M, 0x0);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH2, REG_CFG_QEC2_OVR_CH2_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH2, REG_CFG_QEC2_VALUE_CH2_M, 0x0);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH2, REG_CFG_QEC1_OVR_CH2_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH2, REG_CFG_QEC1_VALUE_CH2_M, 0x0);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH2, REG_CFG_QEC0_OVR_CH2_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH2, REG_CFG_QEC0_VALUE_CH2_M, 0x1f);
}

static hi_void phy_hw_ch3_dcc_qec_set(struct hdmitx_phy *hdmi_phy)
{
    /* channel 3 dcc setting */
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH3, REG_CFG_DCC3_OVR_CH3_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH3, REG_CFG_DCC3_VALUE_CH3_M, 0x28);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH3, REG_CFG_DCC2_OVR_CH3_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH3, REG_CFG_DCC2_VALUE_CH3_M, 0x28);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH3, REG_CFG_DCC1_OVR_CH3_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH3, REG_CFG_DCC1_VALUE_CH3_M, 0x28);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH3, REG_CFG_DCC0_OVR_CH3_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_DCC_OVERWRITE_CFG_CH3, REG_CFG_DCC0_VALUE_CH3_M, 0x28);
    /* channel 3 qec setting */
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH3, REG_CFG_QEC3_OVR_CH3_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH3, REG_CFG_QEC3_VALUE_CH3_M, 0x0);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH3, REG_CFG_QEC2_OVR_CH3_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH3, REG_CFG_QEC2_VALUE_CH3_M, 0x0);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH3, REG_CFG_QEC1_OVR_CH3_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH3, REG_CFG_QEC1_VALUE_CH3_M, 0x0);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH3, REG_CFG_QEC0_OVR_CH3_M, 0x1);
    hdmi_write_bits(hdmi_phy->regs, REG_QEC_OVERWRITE_CFG_CH3, REG_CFG_QEC0_VALUE_CH3_M, 0x1f);
}

static hi_void phy_hw_frl_clk_set(struct hdmitx_phy *hdmi_phy, hi_u32 rate)
{
    hi_u32 m = 0;
    hi_u32 n = 0;
    hi_u32 x = 0;

    get_frl_frac_param(rate, &m, &n, &x);
    hdmi_write_bits(hdmi_phy->regs, REG_FDIVMANUAL, REG_I_MANUAL_EN_M, 0xe);

    hdmi_write_bits(hdmi_phy->regs, REG_FDIVMANUAL, REG_I_MDIV_M, m);

    hdmi_write_bits(hdmi_phy->regs, REG_FDIVSTAT1, REG_I_FDIV_IN_N_M, n);
    hdmi_write_bits(hdmi_phy->regs, REG_FDIVSTAT1, REG_I_FDIV_IN_X_M, x);

    hdmi_clr(hdmi_phy->regs, REG_FDIVSET0, REG_INIT_M);
    hdmi_set(hdmi_phy->regs, REG_FDIVSET0, REG_INIT_M); /* need confirm */
    osal_udelay(1);
    hdmi_clr(hdmi_phy->regs, REG_FDIVSET0, REG_INIT_M);

    hdmi_clr(hdmi_phy->regs, REG_FDIVSET0, REG_EN_CTRL_M);
    hdmi_clr(hdmi_phy->regs, REG_FDIVSET0, REG_EN_MOD_M);

    if (rate == FRL_RATE_4L_10G || rate == FRL_RATE_4L_8G) {
        hdmi_set(hdmi_phy->regs, REG_FDIVSET0, REG_EN_SDM_M);
    }

    switch (rate) {
        case FRL_RATE_3L_3G:
            hdmi_write_bits(hdmi_phy->regs, REG_FCDSTEPSET, REG_P_DIVN_H20_M, 0x2);
            break;
        case FRL_RATE_3L_6G:
        case FRL_RATE_4L_6G:
            hdmi_write_bits(hdmi_phy->regs, REG_FCDSTEPSET, REG_P_DIVN_H20_M, 0x1);
            break;
        case FRL_RATE_4L_8G:
        case FRL_RATE_4L_10G:
        case FRL_RATE_4L_12G:
            hdmi_write_bits(hdmi_phy->regs, REG_FCDSTEPSET, REG_P_DIVN_H20_M, 0x0);
            break;
        default:
            HDMI_ERR("unknow frl rate!\n");
            break;
    }

    hdmi_clr(hdmi_phy->regs, REG_FCDSTEPSET,  REG_FCDSTEPSET_UNUSED_M | REG_UP_SAMPLER_RATIO_SEL_M); /* need confirm */
}

static hi_void phy_hw_fcg_params_set(struct hdmitx_phy *hdmi_phy, struct fcg_div_params *phy_fcg)
{
    hi_u32 dlf_lock;
    hi_u32 count = 0;

    stb_writeb(hdmi_phy, APHY_CS_8, APHY_OFFSET_B, 0x0);

    hdmi_clr(hdmi_phy->regs, REG_FCGSET, REG_P_FCG_EN_M);
    hdmi_clr(hdmi_phy->regs, REG_FCGSET, REG_P_FCG_DIF_EN_M);
    hdmi_clr(hdmi_phy->regs, REG_FCGSET, REG_P_FCG_DITHER_EN_M);
    hdmi_clr(hdmi_phy->regs, REG_FCGSET, REG_P_FCG_LOCK_EN_M);

    stb_writeb(hdmi_phy, APHY_CS_8, APHY_OFFSET_B,
        ((phy_fcg->fcg_divm & 0xf) << 4) | ((phy_fcg->fcg_divp & 0x3) << 2) | 3);
    stb_writeb(hdmi_phy, APHY_CS_8, APHY_OFFSET_A,
        ((phy_fcg->fcg_postdiv & 0x3) << 6) | 0x3);

    hdmi_write_bits(hdmi_phy->regs, REG_FCGCNT, REG_P_TMDS_CNT_VAL_M, phy_fcg->tmds_cnt_val);
    hdmi_write_bits(hdmi_phy->regs, REG_FCGCNT, REG_P_CNT1_TARGET_M, phy_fcg->cnt1_target);
    hdmi_write_bits(hdmi_phy->regs, REG_FCGPARAM, REG_P_LOCK_CNT_M, 0x3);
    hdmi_write_bits(hdmi_phy->regs, REG_FCGPARAM, REG_P_LOCK_TH_M, phy_fcg->lock_th);
    hdmi_write_bits(hdmi_phy->regs, REG_FCGPARAM, REG_P_KI_M, phy_fcg->ki);
    hdmi_write_bits(hdmi_phy->regs, REG_FCGPARAM, REG_P_LOCK_MODE_M, 0x0);

    hdmi_set(hdmi_phy->regs, REG_FCGSET, REG_P_FCG_EN_M);
    hdmi_set(hdmi_phy->regs, REG_FCGSET, REG_P_FCG_DIF_EN_M);
    hdmi_set(hdmi_phy->regs, REG_FCGSET, REG_P_FCG_DITHER_EN_M);
    hdmi_set(hdmi_phy->regs, REG_FCGSET, REG_P_FCG_LOCK_EN_M);

    do {
        dlf_lock = hdmi_read_bits(hdmi_phy->regs, REG_FCGSTATE, REG_O_DLF_LOCK_M);
        if (dlf_lock) {
            HDMI_DBG("fcg lock successful: lock(%d)\n", dlf_lock);
            break;
        }

        count++;
        osal_msleep(1);

        if (count >= 300) {
            HDMI_ERR("fcg lock failed: lock(%d)\n", dlf_lock);
            break;
        }
    } while (1);
}

static hi_void phy_hw_tmds_ssc_set(struct hdmitx_phy *hdmi_phy, hi_bool enable, hi_u16 mod_n, hi_u16 mod_d)
{
    if (enable) {
        stb_writeb(hdmi_phy, APHY_CS_8, APHY_OFFSET_D, 0xFD);
        /* MOD_N MOD_T */
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET3, REG_MOD_LEN_M, 0x0);
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET3, REG_MOD_T_M, 0x1);
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET3, REG_MOD_N_M, mod_n);
        /* MOD_D */
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET4, REG_MOD_D_M, mod_d);
        /* FDIV init */
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_INIT_M, 0x0);
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_EN_CTRL_M, 0x0);
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_EN_MOD_M, 0x0);
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_EN_SDM_M, 0x0);
        /* FDIV init */
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_INIT_M, 0x1);
        osal_udelay(1);
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_INIT_M, 0x0);
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_EN_CTRL_M, 0x0);
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_EN_MOD_M, 0x0);
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_EN_SDM_M, 0x0);
        osal_udelay(1); /* 150 nsec  */
        /* FDIV control */
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_INIT_M, 0x0);
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_EN_CTRL_M, 0x1);
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_EN_MOD_M, 0x0);
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_EN_SDM_M, 0x0);
        /* FDIV control */
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_INIT_M, 0x0);
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_EN_CTRL_M, 0x1);
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_EN_MOD_M, 0x1);
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_EN_SDM_M, 0x0);
        /* FDIV control */
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_INIT_M, 0x0);
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_EN_CTRL_M, 0x1);
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_EN_MOD_M, 0x1);
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_EN_SDM_M, 0x1);
    } else {
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_INIT_M, 0x1);
        osal_udelay(1);
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_INIT_M, 0x0);
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_EN_CTRL_M, 0x0);
        hdmi_write_bits(hdmi_phy->regs, REG_FDIVSET0, REG_EN_MOD_M, 0x0);
    }
}

static hi_void phy_hw_oe_enable(struct hdmitx_phy *hdmi_phy, hi_bool enable)
{
    hdmi_write_bits(hdmi_phy->regs, REG_HDMI_OE_CFG, REG_CFG_HDMI_OE_CH3_M, enable);
    hdmi_write_bits(hdmi_phy->regs, REG_HDMI_OE_CFG, REG_CFG_HDMI_OE_CH2_M, enable);
    hdmi_write_bits(hdmi_phy->regs, REG_HDMI_OE_CFG, REG_CFG_HDMI_OE_CH1_M, enable);
    hdmi_write_bits(hdmi_phy->regs, REG_HDMI_OE_CFG, REG_CFG_HDMI_OE_CH0_M, enable);
    /**
     *  Turn on the PHY to wait until VSYNC stops outputting data after OE is turned off.
     */
    if (enable) {
        hdmi_write_bits(hdmi_phy->regs, REG_HDMI_OE_CFG, REG_CFG_OE_SYNC_EN_M, HI_TRUE);
    }
}

static hi_void phy_tmds_spec_set(struct hdmitx_phy *hdmi_phy, hi_u32 tmds_clk, hi_u8 pcb_len)
{
    struct tmds_spec_params *tmds = HI_NULL;

    tmds = get_tmds_spec_params(tmds_clk, pcb_len);
    phy_hw_tmds_aphy_spec_set(hdmi_phy, tmds);
    phy_hw_tmds_dphy_spec_set(hdmi_phy, tmds);
}

static hi_void phy_frl_spec_set(struct hdmitx_phy *hdmi_phy, hi_u32 frl_rate, hi_u8 pcb_len)
{
    hi_u32 rate;
    struct frl_spec_params *frl = HI_NULL;

    switch (frl_rate) {
        case FRL_RATE_3L_3G:
            rate = RATE_3G;
            break;
        case FRL_RATE_3L_6G:
        case FRL_RATE_4L_6G:
            rate = RATE_6G;
            break;
        case FRL_RATE_4L_8G:
            rate = RATE_8G;
            break;
        case FRL_RATE_4L_10G:
            rate = RATE_10G;
            break;
        case FRL_RATE_4L_12G:
            rate = RATE_12G;
            break;
        default:
            rate = RATE_3G;
            break;
    }

    frl = get_frl_spec_params(rate, pcb_len);
    phy_hw_frl_aphy_spec_set(hdmi_phy, frl);
    phy_hw_frl_dphy_spec_set(hdmi_phy, frl);
}

static hi_void phy_tmds_clock_set(struct hdmitx_phy *hdmi_phy, const struct phy_tmds *tmds)
{
    phy_hw_tmds_clk_set(hdmi_phy, tmds);
    phy_hw_fcon_param_set(hdmi_phy, tmds->tmds_clk);
}

static hi_void phy_frl_clock_set(struct hdmitx_phy *hdmi_phy, hi_u32 frl_rate)
{
    phy_hw_frl_clk_set(hdmi_phy, frl_rate);
    phy_hw_fcon_param_set(hdmi_phy, frl_rate);
}

static hi_void phy_dcc_qec_set(struct hdmitx_phy *hdmi_phy)
{
    phy_hw_dcc_qec_init(hdmi_phy);
    phy_hw_ch0_dcc_qec_set(hdmi_phy);
    phy_hw_ch1_dcc_qec_set(hdmi_phy);
    phy_hw_ch2_dcc_qec_set(hdmi_phy);
    phy_hw_ch3_dcc_qec_set(hdmi_phy);
}

static hi_void phy_tmds_ssc_set(struct hdmitx_phy *hdmi_phy, const struct phy_ssc *ssc)
{
    hi_u16 mod_n = 0;
    hi_u16 mod_d = 0;

    if (ssc->tmds_clk > 340000) {
        HDMI_ERR("more than 340M clock freq have no ssc!\n");
        return;
    }

    if (ssc->enable) {
        get_tmds_ssc_params(ssc, &mod_n, &mod_d);
        phy_hw_tmds_ssc_set(hdmi_phy, HI_TRUE, mod_n, mod_d);
    } else {
        phy_hw_tmds_ssc_set(hdmi_phy, HI_FALSE, 0, 0);
    }
}

static hi_void phy_tpll_set(struct hdmitx_phy *hdmi_phy, hi_u32 pixel_clk, hi_bool yuv_420)
{
    struct tpll_div_params params = {};
    hi_u32 id = hdmi_phy->ctrl_id;

    if (hdmi_phy->mode == PHY_SET_MODE_TMDS) {
        get_tmds_tpll_div_params(&params, pixel_clk, yuv_420, id);
    } else {
        get_frl_tpll_div_params(&params);
    }
    phy_hw_tpll_params_set(hdmi_phy, &params);
}

static hi_void phy_tmds_set(struct hdmitx_phy *hdmi_phy, const struct phy_tmds *tmds)
{
    hi_u32 tmds_clk;
    hi_u32 pixel_clk;
    struct phy_ssc ssc = {};
    hi_u8 pcb_len;

    pcb_len = tmds->pcb_len;
    tmds_clk = tmds->tmds_clk;
    pixel_clk = tmds->pixel_clk;
    hdmi_phy->mode = PHY_SET_MODE_TMDS;

    /* reset phy */
    phy_hw_reset(hdmi_phy);
    /* tpll set */
    phy_tpll_set(hdmi_phy, pixel_clk, tmds->yuv_420);
    /* SET PLL  */
    phy_hw_init(hdmi_phy, pixel_clk, tmds_clk, tmds->tpll_enable);
    /* set phy tmds clock */
    phy_tmds_clock_set(hdmi_phy, tmds);
    /* dcc qec */
    phy_dcc_qec_set(hdmi_phy);
    /* scc set */
    ssc.enable = HI_FALSE;
    phy_tmds_ssc_set(hdmi_phy, &ssc);
    /* set spec */
    phy_tmds_spec_set(hdmi_phy, tmds_clk, pcb_len);
    /* data  path enable */
    phy_hw_data_channel_enable(hdmi_phy, 0);
}

static hi_void phy_frl_set(struct hdmitx_phy *hdmi_phy, const struct phy_frl *frl)
{
    hi_u32 frl_rate;
    hi_u8 pcb_len;

    pcb_len = frl->pcb_len;
    frl_rate = frl->frl_rate;
    hdmi_phy->mode = PHY_SET_MODE_FRL;

    /* reset phy */
    phy_hw_reset(hdmi_phy);
    /* tpll set,  don't care pixel clock and yuv_420 frl mode */
    phy_tpll_set(hdmi_phy, 0, HI_FALSE);
    /* SET PLL */
    phy_hw_init(hdmi_phy, 0, frl_rate, frl->tpll_enable);
    /* set phy frl clock */
    phy_frl_clock_set(hdmi_phy, frl_rate);
    /* DCC QEC */
    phy_dcc_qec_set(hdmi_phy);
    /* set spec */
    phy_frl_spec_set(hdmi_phy, frl_rate, pcb_len);
    /* data  path enable */
    phy_hw_data_channel_enable(hdmi_phy, frl_rate);
}

static hi_void phy_frl_tmds_clock_set(struct hdmitx_phy *hdmi_phy,
    const struct phy_frl_tmds_clk *frl_tmds_clk)
{
    struct fcg_div_params fcg = {};

    /* dsc enable */
    if (frl_tmds_clk->dsc_enable) {
        get_enable_dsc_fcg_params(&fcg, frl_tmds_clk);
    /* dsc disable */
    } else {
        get_disable_dsc_fcg_params(&fcg, frl_tmds_clk);
    }

    /* set fcg param */
    phy_hw_fcg_params_set(hdmi_phy, &fcg);
}

static hi_void phy_ssc_set(struct hdmitx_phy *hdmi_phy, const struct phy_ssc *ssc)
{
    phy_tmds_ssc_set(hdmi_phy, ssc);
}

static hi_void phy_spec_set(struct hdmitx_phy *hdmi_phy, const struct phy_spec *spec)
{
}

struct hdmitx_phy *hal_phy_init(const struct hdmitx_phy_resuorce *res)
{
    struct hdmitx_phy *phy = HI_NULL;
    if (res == HI_NULL) {
        HDMI_ERR("null ptr!\n");
        return HI_NULL;
    }

    phy = osal_vmalloc(HI_ID_HDMITX, sizeof(struct hdmitx_phy));
    if (phy == HI_NULL) {
        HDMI_ERR("phy%d-osal_vmalloc failed!\n", res->phy_id);
        return HI_NULL;
    }
    phy->on = HI_FALSE;
    phy->mode = PHY_SET_MODE_TMDS;
    phy->regs = res->regs;
    phy->tpll_regs = res->tpll_regs;
    phy->phy_id = res->phy_id;
    phy->ctrl_id = res->ctrl_id;
    HDMI_DBG("phy%d init\n", phy->phy_id);

    return phy;
}

hi_void hal_phy_deinit(struct hdmitx_phy *phy)
{
    if (phy == HI_NULL) {
        return;
    }
    HDMI_DBG("phy%d deinit\n", phy->phy_id);
    osal_vfree(HI_ID_HDMITX, phy);
}

hi_void hal_phy_on(struct hdmitx_phy *phy)
{
    if (phy == HI_NULL) {
        HDMI_ERR("null ptr!\n");
        return;
    }

    if (phy->on) {
        return;
    }
    phy_hw_oe_enable(phy, HI_TRUE);
    phy->on = HI_TRUE;
    HDMI_DBG("phy%d on\n", phy->phy_id);
}

hi_void hal_phy_off(struct hdmitx_phy *phy)
{
    if (phy == HI_NULL) {
        HDMI_ERR("null ptr!\n");
        return;
    }

    if (!phy->on) {
        return;
    }
    phy_hw_oe_enable(phy, HI_FALSE);
    phy->on = HI_FALSE;
    HDMI_DBG("phy%d off\n", phy->phy_id);
}

hi_bool hal_phy_is_on(struct hdmitx_phy *phy)
{
    if (phy == HI_NULL) {
        HDMI_ERR("null ptr!\n");
        return HI_FALSE;
    }
    return phy->on;
}

hi_void hal_phy_configure_tmds(struct hdmitx_phy *phy, const struct phy_tmds *tmds)
{
    if ((phy == HI_NULL) || (tmds == HI_NULL)) {
        HDMI_ERR("null ptr!\n");
        return;
    }

    if (phy->on) {
        HDMI_DBG("phy%d is on\n", phy->phy_id);
    }
    phy_tmds_set(phy, tmds);
}

hi_void hal_phy_configure_frl(struct hdmitx_phy *phy, const struct phy_frl *frl)
{
    if ((phy == HI_NULL) || (frl == HI_NULL)) {
        HDMI_ERR("null ptr!\n");
        return;
    }

    if (phy->on) {
        HDMI_DBG("phy%d is on\n", phy->phy_id);
    }
    phy_frl_set(phy, frl);
}

hi_void hal_phy_configure_frl_tmds_clk(struct hdmitx_phy *phy,
    const struct phy_frl_tmds_clk *frl_tmds_clk)
{
    if ((phy == HI_NULL) || (frl_tmds_clk == HI_NULL)) {
        HDMI_ERR("null ptr!\n");
        return;
    }

    if (phy->on) {
        HDMI_DBG("phy%d is on\n", phy->phy_id);
    }
    phy_frl_tmds_clock_set(phy, frl_tmds_clk);
}

hi_void hal_phy_configure_ssc(struct hdmitx_phy *phy, const struct phy_ssc *ssc)
{
    if ((phy == HI_NULL) || (ssc == HI_NULL)) {
        HDMI_ERR("null ptr!\n");
        return;
    }

    if (phy->on) {
        HDMI_DBG("phy%d is on\n", phy->phy_id);
    }
    phy_ssc_set(phy, ssc);
}

hi_void hal_phy_configure_spec(struct hdmitx_phy *phy, const struct phy_spec *spec)
{
    if ((phy == HI_NULL) || (spec == HI_NULL)) {
        HDMI_ERR("null ptr!\n");
        return;
    }

    if (phy->on) {
        HDMI_DBG("phy%d is on\n", phy->phy_id);
    }
    phy_spec_set(phy, spec);
}


