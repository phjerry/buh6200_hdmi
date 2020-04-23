/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: mipi tx module
* Author: sdk
* Create: 2019-11-20
*/
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <asm/delay.h>
#include <asm/barrier.h>

#include "linux/hisilicon/securec.h"
#include "hi_reg_common.h"
#include "drv_mipi_tx_ioctl.h"
#include "mipi_tx_hal.h"
#include "mipi_tx_reg.h"
#include "drv_mipi_tx_define.h"
#include "hi_drv_sys.h"
#include "hi_osal.h"
#include "hi_drv_osal.h"
#include "hi_type.h"
#include "hi_errno.h"

/****************************************************************************
 * macro definition                                                         *
 ****************************************************************************/
#define MIPI_TX_REGS_ADDR0   0x016E0000
#define MIPI_TX_REGS_ADDR1   0x016E0400
#define MIPI_TX_REGS_SIZE    0x400

#define MIPI_TX_IRQ_PORT0    (223 + 32)
#define MIPI_TX_IRQ_PORT1    (224 + 32)
#define MIPI_TX_IRQ_NAME0    "int_mipitx_port0"
#define MIPI_TX_IRQ_NAME1    "int_mipitx_port1"

#ifndef HI_FPGA
#define MIPI_TX_IO_CONFIG    0x1F001088
#define MIPI_TX_GPIO0_0      0x12140010
#define MIPI_TX_GPIO0_1      0x12140400
#define MIPI_TX_GPIO6_0      0x12146004
#define MIPI_TX_GPIO6_1      0x12146400
#endif

#define MIPI_TX_REF_CLK      27  /* MHZ */

#define TLPX                60
#define TCLK_PREPARE        60
#define TCLK_ZERO           260
#define TCLK_TRAIL          80
#define TPRE_DELAY          100
#define THS_PREPARE         80
#define THS_ZERO            180
#define THS_TRAIL           80

#define TCLK_POST           95

/* phy addr */
#define PHY_TESTEN_ADDR      0x10000
#define PHY_TESTEN_DATA      0x00000
#define PLL_SET0             0x63
#define PLL_SET1             0x64
#define PLL_SET2             0x65

#define DATA0_TPRE_DELAY     0x28
#define DATA1_TPRE_DELAY     0x38
#define DATA2_TPRE_DELAY     0x48
#define DATA3_TPRE_DELAY     0x58

#define CLK_TLPX             0x10
#define CLK_TCLK_PREPARE     0x11
#define CLK_TCLK_ZERO        0x12
#define CLK_TCLK_TRAIL       0x13

#define DATA0_TLPX           0x20
#define DATA0_THS_PREPARE    0x21
#define DATA0_THS_ZERO       0x22
#define DATA0_THS_TRAIL      0x23
#define DATA1_TLPX           0x30
#define DATA1_THS_PREPARE    0x31
#define DATA1_THS_ZERO       0x32
#define DATA1_THS_TRAIL      0x33
#define DATA2_TLPX           0x40
#define DATA2_THS_PREPARE    0x41
#define DATA2_THS_ZERO       0x42
#define DATA2_THS_TRAIL      0x43
#define DATA3_TLPX           0x50
#define DATA3_THS_PREPARE    0x51
#define DATA3_THS_ZERO       0x52
#define DATA3_THS_TRAIL      0x53

#define CLK_POST_DELAY_SET   0x18

#define MODE_CMD   0x1
#define MODE_VIDEO 0x0

#define CTR_ID0 0
#define CTR_ID1 1

#define PHY_REF_CLK1   19.2  /* MHZ */
#define round_div_8000(value)    (((value) + 4000) / 8000) /* round quotient(div 8000) of value */
#define roundup_div_8000(value)  (((value) + 7999) / 8000) /* round up quotient(div 8000) of value */

#define set_phy_tpre_delay(index, data_tpre_delay) do {        \
        set_phy_reg(index, DATA0_TPRE_DELAY, data_tpre_delay); \
        set_phy_reg(index, DATA1_TPRE_DELAY, data_tpre_delay); \
        set_phy_reg(index, DATA2_TPRE_DELAY, data_tpre_delay); \
        set_phy_reg(index, DATA3_TPRE_DELAY, data_tpre_delay); \
    } while (0)

#define set_phy_data_cfg0(index, data_tlpx, data_ths_prepare, data_ths_zero, data_ths_trail) do { \
        set_phy_reg(index, DATA0_TLPX, data_tlpx);                                                \
        set_phy_reg(index, DATA0_THS_PREPARE, data_ths_prepare);                                  \
        set_phy_reg(index, DATA0_THS_ZERO, data_ths_zero);                                        \
        set_phy_reg(index, DATA0_THS_TRAIL, data_ths_trail);                                      \
        set_phy_reg(index, DATA1_TLPX, data_tlpx);                                                \
        set_phy_reg(index, DATA1_THS_PREPARE, data_ths_prepare);                                  \
        set_phy_reg(index, DATA1_THS_ZERO, data_ths_zero);                                        \
        set_phy_reg(index, DATA1_THS_TRAIL, data_ths_trail);                                      \
    } while (0)

#define set_phy_data_cfg1(index, data_tlpx, data_ths_prepare, data_ths_zero, data_ths_trail) do { \
        set_phy_reg(index, DATA2_TLPX, data_tlpx);                                                \
        set_phy_reg(index, DATA2_THS_PREPARE, data_ths_prepare);                                  \
        set_phy_reg(index, DATA2_THS_ZERO, data_ths_zero);                                        \
        set_phy_reg(index, DATA2_THS_TRAIL, data_ths_trail);                                      \
        set_phy_reg(index, DATA3_TLPX, data_tlpx);                                                \
        set_phy_reg(index, DATA3_THS_PREPARE, data_ths_prepare);                                  \
        set_phy_reg(index, DATA3_THS_ZERO, data_ths_zero);                                        \
        set_phy_reg(index, DATA3_THS_TRAIL, data_ths_trail);                                      \
    } while (0)

#define set_phy_clk_cfg(index, clk_tlpx, clk_tclk_prepare, clk_tclk_zero, clk_tclk_trail) do { \
        set_phy_reg(index, CLK_TLPX, clk_tlpx);                                                \
        set_phy_reg(index, CLK_TCLK_PREPARE, clk_tclk_prepare);                                \
        set_phy_reg(index, CLK_TCLK_ZERO, clk_tclk_zero);                                      \
        set_phy_reg(index, CLK_TCLK_TRAIL, clk_tclk_trail);                                    \
    } while (0)

#define set_phy_pll_cfg(index, pll_set0, pll_set1, pll_set2) do { \
        set_phy_reg(index, PLL_SET0, pll_set0);                   \
        set_phy_reg(index, PLL_SET1, pll_set1);                   \
        set_phy_reg(index, PLL_SET2, pll_set2);                   \
    } while (0)

volatile mipi_tx_regs_type *g_mipi_tx_regs[MAX_CTRL_NUM] = {HI_NULL}; /* why must set to volatile */

hi_u32 g_mipi_irq_satus[MAX_CTRL_NUM] = {0}; /* 0: not initialized, 1:initialized */

mipi_tx_dev_phy g_mipi_tx_phy_ctx[MAX_CTRL_NUM]; /* need check extern */

static hi_void get_mipi_index(mipi_dev_type dev_type, hi_s32 *start_index, hi_s32 *end_index)
{
    if (dev_type == MIPI_DEV_TYPE_CTR0_CTR1) {
        *start_index = 0;
        *end_index = 1;
    } else if (dev_type == MIPI_DEV_TYPE_CTR1) {
        *start_index = 1;
        *end_index = 1;
    } else {
        *start_index = 0;
        *end_index = 0;
    }
}
static void mipi_tx_isb(void)
{
    isb();
}

static void mipi_tx_dsb(void)
{
#ifdef CONFIG_64BIT
    dsb(sy);
#else
    dsb();
#endif
}

void mipi_tx_dmb(void)
{
#ifdef CONFIG_64BIT
    dmb(sy);
#else
    dmb();
#endif
}

hi_void set_phy_reg(hi_u32 index, hi_u32 addr, hi_u8 value)
{
    mipi_tx_isb();
    mipi_tx_dsb();
    mipi_tx_dmb();
    g_mipi_tx_regs[index]->phy_tst_ctrl1.u32 = (PHY_TESTEN_ADDR + addr);
    mipi_tx_isb();
    mipi_tx_dsb();
    mipi_tx_dmb();
    g_mipi_tx_regs[index]->phy_tst_ctrl0.u32 = 0x2;
    mipi_tx_isb();
    mipi_tx_dsb();
    mipi_tx_dmb();
    g_mipi_tx_regs[index]->phy_tst_ctrl0.u32 = 0x0;
    mipi_tx_isb();
    mipi_tx_dsb();
    mipi_tx_dmb();
    g_mipi_tx_regs[index]->phy_tst_ctrl1.u32 = (PHY_TESTEN_DATA + value);
    mipi_tx_isb();
    mipi_tx_dsb();
    mipi_tx_dmb();
    g_mipi_tx_regs[index]->phy_tst_ctrl0.u32 = 0x2;
    mipi_tx_isb();
    mipi_tx_dsb();
    mipi_tx_dmb();
    g_mipi_tx_regs[index]->phy_tst_ctrl0.u32 = 0x0;
    mipi_tx_isb();
    mipi_tx_dsb();
    mipi_tx_dmb();
}

static hi_void get_phy_reg(hi_u32 index, hi_u32 addr, hi_u8 *value)
{
    g_mipi_tx_regs[index]->phy_tst_ctrl1.u32 = (PHY_TESTEN_ADDR + addr);
    g_mipi_tx_regs[index]->phy_tst_ctrl0.u32 = 0x2;
    g_mipi_tx_regs[index]->phy_tst_ctrl0.u32 = 0x0;
    *value = g_mipi_tx_regs[index]->phy_tst_ctrl1.bits.phy_testdout;
}

hi_void read_phy_reg(hi_u32 index, hi_u32 len)
{
    int i = 0;
    int j = 0;
    hi_u8 value[16]; /* 16 data per row */

    osal_printk("PHY-%d\n", index);

    len = (len % 16 == 0) ? len : (len + 16 - (len % 16)); /* 16 data per row */
    for (i = 0; i < len; i++) {
        j = (j % 16 == 0) ? 0 : j; /* 16 data per row */
        get_phy_reg(index, i, &value[j++]);
        if (j == 16) { /* 16 data per row */
            osal_printk("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
                value[0], value[1], value[2], value[3], value[4], value[5], /* data 0, 1, 2, 3, 4, 5 */
                value[6], value[7], value[8], value[9], value[10], value[11], /* data 6, 7, 8, 9, 10, 11 */
                value[12], value[13], value[14], value[15]); /* data 12, 13, 14, 15 */
        }
    }
}

static hi_void cal_phy_pll0(hi_u32 phy_rate, hi_u8 pll_set2, hi_u8 *pll_set0)
{
    hi_u32 temp1 = (pll_set2 % 8); /* 8 is param, ref to manual */
    hi_u32 temp2 = 1;
    while (temp1 != 0) {
        temp2 *= 2; /* 2 is param of formula, ref to manual */
        temp1--;
    }
    (*pll_set0) = ((phy_rate + MIPI_TX_REF_CLK - 1) / MIPI_TX_REF_CLK) * temp2 % 256; /* 256 ref to manual */
}

static hi_void cal_phy_pll2(hi_u32 phy_rate, hi_u8 *pll_set2)
{
    hi_u32 temp1 = 1;
    hi_u32 temp2 = 1;

    *pll_set2 = 0x0;

    if (phy_rate < 1375) { /* fvco_ck rang from 1375Mhz to 2750Mhz */
        temp1 = (1375 + phy_rate - 1) / phy_rate; /* 1375Mhz */
        while (2 * temp2 <= temp1) { /* LOG 2 */
            temp2 *= 2; /* LOG 2 */
            (*pll_set2)++;
        }
    }
}

hi_s32 mipi_tx_drv_set_phy_cfg(hi_s32 index, const mipi_tx_dev_cfg *dev_cfg)
{
    hi_u8 pll_set0;
    hi_u8 pll_set1;
    hi_u8 pll_set2;

    hi_u8 data_tpre_delay;
    hi_u8 clk_tlpx;
    hi_u8 clk_tclk_prepare;
    hi_u8 clk_tclk_zero;
    hi_u8 clk_tclk_trail;
    hi_u8 data_tlpx;
    hi_u8 data_ths_prepare;
    hi_u8 data_ths_zero;
    hi_u8 data_ths_trail;
    hi_u8 clk_post_delay_set;
    hi_u32 phy_rate = dev_cfg->phy_data_rate;

    if (dev_cfg == HI_NULL) {
        hi_err_mipi("dev_cfg is null.\n");
        return HI_FAILURE;
    }

    /* pll_set: pll_set1/pll_set2/pll_set0 */
    pll_set1 = ((phy_rate + MIPI_TX_REF_CLK - 1) / MIPI_TX_REF_CLK) >> 0x8;
    cal_phy_pll2(phy_rate, &pll_set2);
    cal_phy_pll0(phy_rate, pll_set2, &pll_set0);
    set_phy_pll_cfg(index, pll_set0, pll_set1, pll_set2);

    /* DATA0~3 TPRE-DELAY */
    data_tpre_delay = round_div_8000(phy_rate * TPRE_DELAY) - 1;
    set_phy_tpre_delay(index, data_tpre_delay);

    /* CLK CFG: CLK_TLPX/CLK_TCLK_PREPARE/CLK_TCLK_ZERO/CLK_TCLK_TRAIL */
    clk_tlpx = roundup_div_8000(phy_rate * TLPX) - 1;
    clk_tclk_prepare = roundup_div_8000(phy_rate * TCLK_PREPARE) - 1;
    clk_tclk_zero = roundup_div_8000(phy_rate * TCLK_ZERO) - 1;
    clk_tclk_trail = roundup_div_8000(phy_rate * THS_TRAIL) - 1;
    set_phy_clk_cfg(index, clk_tlpx, clk_tclk_prepare, clk_tclk_zero, clk_tclk_trail);

    /* DATA CFG: DATA_TLPX/DATA_THS_PREPARE/DATA_THS_ZERO/DATA_THS_TRAIL */
    data_tlpx = roundup_div_8000(phy_rate * TLPX) - 1;
    data_ths_prepare = roundup_div_8000(phy_rate * THS_PREPARE) - 1;
    data_ths_zero = roundup_div_8000(phy_rate * THS_ZERO) - 1;
    data_ths_trail = roundup_div_8000(phy_rate * THS_TRAIL) - 1;
    set_phy_data_cfg0(index, data_tlpx, data_ths_prepare, data_ths_zero, data_ths_trail);
    set_phy_data_cfg1(index, data_tlpx, data_ths_prepare, data_ths_zero, data_ths_trail);

    clk_post_delay_set = roundup_div_8000(TCLK_POST * phy_rate) + data_ths_trail + 4; /* 4 is param, ref to manual */
    set_phy_reg(index, CLK_POST_DELAY_SET, clk_post_delay_set);

    /* data lp2hs,hs2lp time */
    g_mipi_tx_regs[index]->phy_tmr_cfg.u32 = ((data_ths_trail + 1) << 16) + (data_tpre_delay /* shift left 16 */
                                             + data_tlpx + data_ths_prepare + data_ths_zero);
    /* clk lp2hs,hs2lp time */
    g_mipi_tx_regs[index]->phy_tmr_lpclk_cfg.u32 = ((clk_post_delay_set + data_ths_trail + 1) << 16) /* shift left 16 */
                                                   + (clk_tlpx + clk_tclk_prepare + clk_tclk_zero + 3); /* 3 is param */
    /* phy enable */
    g_mipi_tx_regs[index]->phy_rstz.u32 = 0xf;
    msleep(1);
    g_mipi_tx_regs[index]->lpclk_ctrl.u32 = 0x1;

    return HI_SUCCESS;
}

static hi_void set_output_format(hi_u32 index, const mipi_tx_dev_cfg *dev_cfg)
{
    hi_s32 color_coding = 0x5;

    if (dev_cfg->mipi_mode == HI_DRV_MIPI_MODE_SINGLE_DSC) {
        color_coding = 0xf;
    } else if (dev_cfg->output_format == MIPI_RGB_16_BIT) {
        color_coding = 0x0;
    } else if (dev_cfg->output_format == MIPI_RGB_18_BIT) {
        color_coding = 0x3;
    } else if (dev_cfg->output_format == MIPI_RGB_24_BIT) {
        color_coding = 0x5;
    } else {
        hi_err_mipi("output_format(%x) err\n", dev_cfg->output_format);
    }

    g_mipi_tx_regs[index]->color_coding.u32 = color_coding;
}

static hi_void set_video_mode_cfg(hi_u32 index, const mipi_tx_dev_cfg *dev_cfg)
{
    hi_s32 video_mode;

    if (dev_cfg->video_mode == NON_BURST_MODE_SYNC_PULSES) {
        video_mode = 0;
    } else if (dev_cfg->video_mode == NON_BURST_MODE_SYNC_EVENTS) {
        video_mode = 1;
    } else {
        video_mode = 2; /* reg value of vid_mode_type, 2 is burst mode */
    }

    if (dev_cfg->output_mode == OUTPUT_MODE_CSI || dev_cfg->output_mode == OUTPUT_MODE_DSI_CMD) {
        video_mode = 2; /* reg value of vid_mode_type, 2 is burst mode */
    }

    g_mipi_tx_regs[index]->vid_mode_cfg.u32 = 0x3f00 + video_mode;
}

static hi_void set_timing_config(hi_u32 index, const mipi_tx_dev_cfg *dev_cfg)
{
    hi_u32 hsa_time;
    hi_u32 hbp_time;
    hi_u32 hline_time;
    hi_u32 phy_rate = dev_cfg->phy_data_rate;
    hi_u32 temp = 125; /* 1000 / 8 = 125 , 1000 pixel_clk form KHZ to MHZ, ref to excel */

    if (dev_cfg->pixel_clk == 0) {
        hi_err_mipi("pixel_clk is 0, illegal.\n");
        return;
    }

    hsa_time = phy_rate * dev_cfg->sync_info.vid_hsa_pixels * temp / dev_cfg->pixel_clk;
    hbp_time = phy_rate * dev_cfg->sync_info.vid_hbp_pixels * temp / dev_cfg->pixel_clk;
    hline_time = phy_rate * dev_cfg->sync_info.vid_hline_pixels * temp / dev_cfg->pixel_clk;

    g_mipi_tx_regs[index]->vid_hsa_time.u32 = hsa_time;
    g_mipi_tx_regs[index]->vid_hbp_time.u32 = hbp_time;
    g_mipi_tx_regs[index]->vid_hline_time.u32 = hline_time;

    g_mipi_tx_regs[index]->vid_vsa_lines.u32 = dev_cfg->sync_info.vid_vsa_lines;
    g_mipi_tx_regs[index]->vid_vbp_lines.u32 = dev_cfg->sync_info.vid_vbp_lines;
    g_mipi_tx_regs[index]->vid_vfp_lines.u32 = dev_cfg->sync_info.vid_vfp_lines;
    g_mipi_tx_regs[index]->vid_vactive_lines.u32 = dev_cfg->sync_info.vid_active_lines;

#ifdef MIPI_TX_DEBUG
    hi_err_mipi("VID_HSA_TIME(0x48): 0x%x\n", hsa_time);
    hi_err_mipi("VID_HBP_TIME(0x4c): 0x%x\n", hbp_time);
    hi_err_mipi("VID_HLINE_TIME(0x50): 0x%x\n", hline_time);
    hi_err_mipi("VID_VSA_LINES(0x54): 0x%x\n", dev_cfg->sync_info.vid_vsa_lines);
    hi_err_mipi("VID_VBP_LINES(0x58): 0x%x\n", dev_cfg->sync_info.vid_vbp_lines);
    hi_err_mipi("VID_VFP_LINES(0x5c): 0x%x\n", dev_cfg->sync_info.vid_vfp_lines);
    hi_err_mipi("VID_VACTIVE_LINES(0x60): 0x%x\n", dev_cfg->sync_info.vid_active_lines);
#endif
}

static hi_void set_lane_config(hi_u32 index, const hi_s16 lane_id[MAX_LANE_NUM], hi_u32 lane_id_len)
{
    hi_s32 i;
    hi_s32 lane_num;
    mipi_tx_lane_id lane_id_reg;

    lane_num = 0;
    if (lane_id_len != MAX_LANE_NUM) {
        hi_err_mipi("lane id length is invalid.\n");
    }
    for (i = 0; i < lane_id_len; i++) {
        if (lane_id[i] != -1) {
            lane_num++;
        }
    }

    g_mipi_tx_regs[index]->phy_if_cfg.u32 = lane_num - 1;

    lane_id_reg.u32 = g_mipi_tx_regs[index]->lane_id.u32;
    if (lane_id[0] >= 0) {
        lane_id_reg.bits.lane0_id = lane_id[0];
    }
    if (lane_id[1] >= 0) { /* lane 1 */
        lane_id_reg.bits.lane1_id = lane_id[1]; /* lane 1 */
    }
    if (lane_id[2] >= 0) { /* lane 2 */
        lane_id_reg.bits.lane2_id = lane_id[2]; /* lane 2 */
    }
    if (lane_id[3] >= 0) { /* lane 3 */
        lane_id_reg.bits.lane3_id = lane_id[3]; /* lane 3 */
    }
    g_mipi_tx_regs[index]->lane_id.u32 = lane_id_reg.u32;

#ifdef MIPI_TX_DEBUG
    hi_err_mipi("LANE_ID(0x250): 0x%x\n", lane_id_reg.u32);
#endif
}

static hi_void set_clkmgr_cfg(hi_u32 index, const mipi_tx_dev_cfg *dev_cfg)
{
    hi_u32 phy_rate = dev_cfg->phy_data_rate;
    /* is clkmgr_cfg.u32 = 0x103; ->50ns per bit */
    if ((phy_rate / 160) < 2) { /* tx escape div parm 160, 2 div */
        g_mipi_tx_regs[index]->clkmgr_cfg.u32 = 0x102;
        return;
    }

    g_mipi_tx_regs[index]->clkmgr_cfg.u32 = 0x100 + (phy_rate + 159) / 160; /* +159, div 160, roundup */
}

hi_s32 mipi_tx_drv_set_controller_cfg(hi_s32 index, const mipi_tx_dev_cfg *dev_cfg)
{
    if (dev_cfg == HI_NULL) {
        hi_err_mipi("dev_cfg is null.\n");
    }

    /* disable input */
    g_mipi_tx_regs[index]->operation_mode.u32 = 0x0;

    /* vcid: Currently, only one peripheral is available. vcid set to zero */
    g_mipi_tx_regs[index]->vcid.u32 = 0x0;

    /* output format */
    set_output_format(index, dev_cfg);

    /* txescclk,timeout */
    set_clkmgr_cfg(index, dev_cfg);

    /* cmd transmission mode */
    if (dev_cfg->output_mode == OUTPUT_MODE_DSI_CMD) {
        g_mipi_tx_regs[index]->cmd_mode_cfg.u32 = 0x0; /* set all to high-speed */
    } else {
        g_mipi_tx_regs[index]->cmd_mode_cfg.u32 = 0xffffff00; /* set all to low-speed */
    }

    /* crc,ecc,bta,eotp_rx enable */
    g_mipi_tx_regs[index]->pckhdl_cfg.u32 = 0x1e;
    /* gen_vcid_rx */
    g_mipi_tx_regs[index]->gen_vcid.u32 = 0x0;

    /* mode config */
    g_mipi_tx_regs[index]->mode_cfg.u32 = 0x0; /* attention: set to video mode default now */

    /* video mode cfg */
    set_video_mode_cfg(index, dev_cfg);
    if (dev_cfg->output_mode == OUTPUT_MODE_DSI_VIDEO) {
        g_mipi_tx_regs[index]->vid_pkt_size.u32 = dev_cfg->sync_info.vid_pkt_size;
    } else {
        g_mipi_tx_regs[index]->edpi_cmd_size.u32 = dev_cfg->sync_info.edpi_cmd_size;
    }

    /* num_chunks/null_size */
    g_mipi_tx_regs[index]->vid_num_chunks.u32 = 0x0;
    g_mipi_tx_regs[index]->vid_null_size.u32 = 0x0;

    /* timing config */
    set_timing_config(index, dev_cfg);

    /* invact,outvact time */
    g_mipi_tx_regs[index]->lp_cmd_tim.u32 = 0x0;

    /* lp_wr_to_cnt */
    g_mipi_tx_regs[index]->lp_wr_to_cnt.u32 = 0x0;
    /* bta_to_cnt */
    g_mipi_tx_regs[index]->bta_to_cnt.u32 = 0x0;

    /* lanes */
    set_lane_config(index, dev_cfg->lane_id, MAX_LANE_NUM);

    /* phy_txrequlpsclk */
    g_mipi_tx_regs[index]->phy_ulps_ctrl.u32 = 0x0;

    /* pwr_up unreset */
    g_mipi_tx_regs[index]->pwr_up.u32 = 0x0;
    g_mipi_tx_regs[index]->pwr_up.u32 = 0x1;
    return HI_SUCCESS;
}

#ifdef MIPI_TX_DEBUG
static hi_void print_cmd_info(mipi_cmd_info *cmd, hi_u8 *in_params, mipi_tx_gen_hdr gen_hdr)
{
    int i;

    hi_err_mipi("\n=====Set Cmd=======\n");
    hi_err_mipi("cmd->id: 0x%x\n", cmd->id);
    hi_err_mipi("cmd->cmd_type: 0x%x\n", cmd->data_type);
    hi_err_mipi("cmd->in_param_size: 0x%x\n", cmd->in_param_size);
    hi_err_mipi("in_params: \n");
    for (i = 0; i < cmd->in_param_size; i++) {
        hi_err_mipi("0x%x\n", in_params[i]);
    }
    hi_err_mipi("\n");
    hi_err_mipi("GEN_HDR(0x6C): 0x%x\n", gen_hdr.u32);
}
#endif

static hi_s32 transfer_long_packet_data(hi_s32 index, hi_u16 size, const hi_u8 *in_params)
{
    hi_s32 i, j;
    mipi_tx_gen_pld_data gen_pld_data;

    if (in_params == HI_NULL) {
        hi_err_mipi("in_params is null.\n");
        return HI_ERR_MIPI_NULL_PTR;
    }

    gen_pld_data.u32 = g_mipi_tx_regs[index]->gen_pld_data.u32;

    for (i = 0; i <= size - 4;) { /* store 4 parameter at one time */
        gen_pld_data.bits.gen_pld_b1 = in_params[i++];
        gen_pld_data.bits.gen_pld_b2 = in_params[i++];
        gen_pld_data.bits.gen_pld_b3 = in_params[i++];
        gen_pld_data.bits.gen_pld_b4 = in_params[i++];
        g_mipi_tx_regs[index]->gen_pld_data.u32 = gen_pld_data.u32;
#ifdef MIPI_TX_DEBUG
        printk("GEN_PLD_DATA(%d): 0x%x\n", i, gen_pld_data.u32);
#endif
    }

    j = size % 4; /* store 4 parameter at one time */
    if (j != 0) {
        if (j >= 1) { /* remainder is 1 */
            gen_pld_data.bits.gen_pld_b1 = in_params[i++];
        }
        if (j >= 2) { /* remainder is 2 */
            gen_pld_data.bits.gen_pld_b2 = in_params[i++];
        }
        if (j >= 3) { /* remainder is 3 */
            gen_pld_data.bits.gen_pld_b3 = in_params[i++];
        }
        g_mipi_tx_regs[index]->gen_pld_data.u32 = gen_pld_data.u32;

#ifdef MIPI_TX_DEBUG
        printk("GEN_PLD_DATA(r=%d): 0x%x\n", j, gen_pld_data.u32);
#endif
    }
    return HI_SUCCESS;
}

hi_s32 mipi_tx_drv_set_cmd_info(mipi_cmd_info *cmd_info)
{
    mipi_tx_gen_hdr gen_hdr;
    hi_u8 *in_params = HI_NULL;
    if (cmd_info == HI_NULL) {
        hi_err_mipi("cmd_info is null.\n");
        return HI_FAILURE;
    }

    if (g_mipi_tx_regs[cmd_info->id] == HI_NULL) {
        hi_err_mipi("mipi_id %d not exist.\n", cmd_info->id);
        return HI_FAILURE;
    }

    gen_hdr.u32 = g_mipi_tx_regs[cmd_info->id]->gen_hdr.u32;

    if (cmd_info->in_param_size > 800) { /* the max buffer size is 800 */
        hi_err_mipi("cmd_size(%d) is error.\n", cmd_info->in_param_size);
        return HI_FAILURE;
    }

    if (cmd_info->in_param_size > 0) {
        in_params = kzalloc(cmd_info->in_param_size, GFP_KERNEL);
        if (in_params == HI_NULL) {
            hi_err_mipi("malloc cmd error.\n");
            return HI_FAILURE;
        }

        if (memcpy_s(in_params, cmd_info->in_param_size, cmd_info->in_params, cmd_info->in_param_size) != HI_SUCCESS) {
            hi_err_mipi("copy cmd info to kernel err.\n");
            kfree(in_params);
            return HI_FAILURE;
        }
    }

    if (cmd_info->in_param_size > 0x2) {
        if (transfer_long_packet_data(cmd_info->id, cmd_info->in_param_size, in_params) != HI_SUCCESS) {
            kfree(in_params);
            return HI_FAILURE;
        }
        gen_hdr.bits.gen_wc_lsbyte = cmd_info->in_param_size & 0xff;
        gen_hdr.bits.gen_wc_msbyte = (cmd_info->in_param_size & 0xff00) >> 8; /* 8 bit */
    } else {
        gen_hdr.bits.gen_wc_lsbyte = 0x0;
        gen_hdr.bits.gen_wc_msbyte = 0x0;
        if (cmd_info->in_param_size >= 1) { /* remainder is 1 */
            gen_hdr.bits.gen_wc_lsbyte = in_params[0];
        }
        if (cmd_info->in_param_size >= 2) { /* remainder is 2 */
            gen_hdr.bits.gen_wc_msbyte = in_params[1];
        }
    }

    gen_hdr.bits.gen_dt = cmd_info->data_type;
    g_mipi_tx_regs[cmd_info->id]->gen_hdr.u32 = gen_hdr.u32;
#ifdef MIPI_TX_DEBUG
    print_cmd_info(cmd_info, in_params, gen_hdr);
#endif
    if (in_params != HI_NULL) {
        kfree(in_params);
    }
    return HI_SUCCESS;
}

static hi_s32 set_output_mode(const mipi_attr_pair *attr_pair)
{
    /* only set to video or cmd mode */
    hi_u32 mode;
    hi_drv_mipi_id id = attr_pair->id;
    hi_mipi_output_mode temp_mode = attr_pair->attr.output_mode;

    if (temp_mode >= OUTPUT_MODE_MAX) {
        hi_err_mipi("invalid output_mode(%d)\n", temp_mode);
        return HI_ERR_MIPI_INVALID_PARA;
    }

    mode = (temp_mode == OUTPUT_MODE_DSI_CMD) ? MODE_CMD : MODE_VIDEO;

    g_mipi_tx_regs[id]->mode_cfg.u32 = mode; /* wether need to power on and power off the device */

    return HI_SUCCESS;
}

static hi_s32 set_video_mode(const mipi_attr_pair *attr_pair)
{
    hi_drv_mipi_id id = attr_pair->id;
    hi_mipi_video_mode temp_mode = attr_pair->attr.video_mode;
    hi_u32 reg_value = g_mipi_tx_regs[id]->vid_mode_cfg.u32;

    if (temp_mode >= VIDEO_MODE_MAX) {
        hi_err_mipi("invalid video_mode(%d)\n", temp_mode);
        return HI_ERR_MIPI_INVALID_PARA;
    }

    if (temp_mode == NON_BURST_MODE_SYNC_PULSES) {
        reg_value &= 0xfffffffc;
    } else if (temp_mode == NON_BURST_MODE_SYNC_EVENTS) {
        reg_value &= 0xfffffffd;
    } else {
        reg_value &= 0xfffffffe;
    }

    g_mipi_tx_regs[id]->vid_mode_cfg.u32 = reg_value;

    return HI_SUCCESS;
}

hi_s32 mipi_tx_drv_set_attr(const mipi_attr_pair *attr_pair)
{
    if (attr_pair == HI_NULL) {
        hi_err_mipi("attr_pair is null.\n");
        return HI_ERR_MIPI_NULL_PTR;
    }

    if (attr_pair->attr_type == OUTPUT_MODE) {
        return set_output_mode(attr_pair);
    }

    if (attr_pair->attr_type == VIDEO_MODE) {
        return set_video_mode(attr_pair);
    }

    hi_err_mipi("invalid attr_type(%d)\n", attr_pair->attr_type);
    return HI_ERR_MIPI_INVALID_PARA;
}

hi_void mipi_tx_drv_enable_input(hi_drv_mipi_id id, hi_mipi_output_mode output_mode)
{
    if (output_mode == OUTPUT_MODE_DSI_CMD) {
        g_mipi_tx_regs[id]->mode_cfg.u32 = 0x1;
    } else {
        g_mipi_tx_regs[id]->mode_cfg.u32 = 0x0;
    }
    /* wether need to do reset and unreset operation for dsc */
    /* enable input */
    g_mipi_tx_regs[id]->operation_mode.u32 = 0x80150000; /* need to add dsc, ref to dsc_soft_rst */

    /* try use 0x13f02 for test color bar */
    g_mipi_tx_regs[id]->vid_mode_cfg.u32 = 0x3f02; /* if use this finally */

    /* reset mipi_tx */
    g_mipi_tx_regs[id]->pwr_up.u32 = 0x0;
    udelay(1);
    g_mipi_tx_regs[id]->pwr_up.u32 = 0x1;
}

hi_void mipi_tx_drv_disable_input(hi_drv_mipi_id id, hi_mipi_output_mode output_mode)
{
    g_mipi_tx_regs[id]->mode_cfg.u32 = 0x1; /* cmd mode is configured by default,when disable input */

    /* enable input */
    g_mipi_tx_regs[id]->operation_mode.u32 = 0x10150000; /* reset dsc & disable input */
    /* reset mipi_tx */
    g_mipi_tx_regs[id]->pwr_up.u32 = 0x0;
    udelay(1);
    g_mipi_tx_regs[id]->pwr_up.u32 = 0x1;
}

hi_void mipi_tx_drv_get_vall_det(hi_s32 index, hi_u32 *vall_det)
{
    *vall_det = g_mipi_tx_regs[index]->vert_det.bits.vall_det;
}

hi_void mipi_tx_drv_get_vact_det(hi_s32 index, hi_u32 *vact_det)
{
    *vact_det = g_mipi_tx_regs[index]->vert_det.bits.vact_det;
}
hi_void mipi_tx_drv_get_hline_det(hi_s32 index, hi_u32 *hall_det)
{
    *hall_det = g_mipi_tx_regs[index]->hori0_det.bits.hline_det;
}

hi_void mipi_tx_drv_get_hact_det(hi_s32 index, hi_u32 *hact_det)
{
    *hact_det = g_mipi_tx_regs[index]->hori0_det.bits.hact_det;
}

static hi_s32 mipi_tx_isr(hi_s32 irq, hi_void *dev_id)
{
    hi_s32 index, int_stat;
    if (dev_id == HI_NULL) {
        hi_err_mipi("irq(%d) func err.\n", irq);
    }

    if (irq == MIPI_TX_IRQ_PORT0) {
        index = 0;
    } else if (irq == MIPI_TX_IRQ_PORT1) {
        index = 1;
    } else {
        hi_err_mipi("irq(%d) num err,do no thing.\n", irq);
        return IRQ_HANDLED;
    }

    int_stat = g_mipi_tx_regs[index]->int_st1.u32; /* interrupt status reg, readclear */

    mipi_tx_drv_get_vall_det (index, &g_mipi_tx_phy_ctx[index].vall_det);
    mipi_tx_drv_get_vact_det (index, &g_mipi_tx_phy_ctx[index].vact_det);
    mipi_tx_drv_get_hline_det(index, &g_mipi_tx_phy_ctx[index].hall_det);
    mipi_tx_drv_get_hact_det (index, &g_mipi_tx_phy_ctx[index].hact_det);

    return IRQ_HANDLED;
}

static hi_void mipi_tx_enable_int(hi_s32 index, hi_s32 enable)
{
    g_mipi_tx_regs[index]->int_msk1.bits.mask_vsstart = enable & 0x1;
}

hi_s32 mipi_tx_register_irq(mipi_dev_type dev_type)
{
#ifdef HI_MIPI_CHIP_DEBUG
    hi_s32 ret;
    hi_s32 i, star_index, end_index;
    get_mipi_index(dev_type, &star_index, &end_index);

    for (i = star_index; i <= end_index; i++) {
        if (g_mipi_irq_satus[i] == 1) {
            hi_err_mipi("mipi_tx(%d) irq already initialized.\n", i);
            continue;
        }

        mipi_tx_enable_int(i, 0x1);
        if (i == CTR_ID0) {
            ret = hi_drv_osal_request_irq(MIPI_TX_IRQ_PORT0, (irq_handler_t)mipi_tx_isr, IRQF_SHARED,
                                          MIPI_TX_IRQ_NAME0, mipi_tx_isr);
        } else {
            ret = hi_drv_osal_request_irq(MIPI_TX_IRQ_PORT1, (irq_handler_t)mipi_tx_isr, IRQF_SHARED,
                                          MIPI_TX_IRQ_NAME1, mipi_tx_isr);
        }

        if (ret < 0) {
            hi_err_mipi("mipi_tx(%d) register irq error.\n", i);
            return HI_FAILURE;
        }

        g_mipi_irq_satus[i] = 1;
    }
#endif
    return HI_SUCCESS;
}

hi_void mipi_tx_unregister_irq(mipi_dev_type dev_type)
{
    hi_s32 i, star_index, end_index;
    get_mipi_index(dev_type, &star_index, &end_index);

    for (i = star_index; i <= end_index; i++) {
        if (g_mipi_irq_satus[i] == 0) {
            hi_warn_mipi("mipi_tx(%d) irq do not need free.\n", i);
            continue;
        }

        if (i == CTR_ID0) {
            hi_drv_osal_free_irq(MIPI_TX_IRQ_PORT0, MIPI_TX_IRQ_NAME0, mipi_tx_isr);
        } else {
            hi_drv_osal_free_irq(MIPI_TX_IRQ_PORT1, MIPI_TX_IRQ_NAME1, mipi_tx_isr);
        }

        mipi_tx_enable_int(i, 0x0);
    }
}

static hi_s32 mipi_tx_drv_reg_init(mipi_dev_type dev_type)
{
    hi_s32 i, star_index, end_index;
    hi_ulong base_regs_addr = MIPI_TX_REGS_ADDR0;

    get_mipi_index(dev_type, &star_index, &end_index);

    for (i = star_index; i <= end_index; i++) {
        base_regs_addr = (i == 0) ? MIPI_TX_REGS_ADDR0 : MIPI_TX_REGS_ADDR1;
        g_mipi_tx_regs[i] = (volatile mipi_tx_regs_type *)ioremap_nocache(base_regs_addr, MIPI_TX_REGS_SIZE);
        if (g_mipi_tx_regs[i] == HI_NULL) {
            hi_err_mipi("remap mipi_tx reg %x fail\n", i);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_void mipi_tx_drv_reg_exit(hi_void)
{
    hi_s32 i;
    for (i = 0; i < MAX_CTRL_NUM; i++) {
        if (g_mipi_tx_regs[i] != HI_NULL) {
            iounmap((hi_void *)g_mipi_tx_regs[i]);
            g_mipi_tx_regs[i] = HI_NULL;
        }
    }
}

static hi_s32 mipi_tx_drv_hw_init(mipi_dev_type dev_type)
{
    /* wether need to add dsc configure */
    volatile U_PERI_CRG457 temp_value1;
    volatile U_PERI_CRG458 temp_value2;
    volatile hi_reg_crg *mipi_tx_crg_addr = HI_NULL;
    hi_s32 star_index, end_index;

    get_mipi_index(dev_type, &star_index, &end_index);

    mipi_tx_crg_addr = hi_drv_sys_get_crg_reg_ptr();
    if (mipi_tx_crg_addr == HI_NULL) {
        hi_err_mipi("mipi_tx_crg_addr is null\n");
        return HI_ERR_MIPI_NULL_PTR;
    }

    temp_value1.u32 = mipi_tx_crg_addr->PERI_CRG457.u32;
    temp_value2.u32 = mipi_tx_crg_addr->PERI_CRG458.u32;

    if (star_index == 0x0) {
        /* mipi_tx gate clk enable */
        temp_value1.bits.mipitx_cken_mipitx_port0 = 0x1;
        /* unreset */
        temp_value1.bits.mipitx_srst_req_mipitx_port0 = 0x0;
    }

    if (end_index == 0x1) {
        /* mipi_tx gate clk enable */
        temp_value2.bits.mipitx_cken_mipitx_port1 = 0x1;
        /* unreset */
        temp_value2.bits.mipitx_srst_req_mipitx_port1 = 0x0;
    }

    /* ref clk 27MHz ,only mipi0 has ref clk reg */
    temp_value1.bits.mipitx_ref_cksel = 0x2;

    mipi_tx_crg_addr->PERI_CRG457.u32 = temp_value1.u32;
    mipi_tx_crg_addr->PERI_CRG458.u32 = temp_value2.u32;

    return HI_SUCCESS;
}

hi_s32 mipi_tx_drv_init(mipi_dev_type dev_type)
{
    hi_s32 ret;

    ret = mipi_tx_drv_reg_init(dev_type);
    if (ret != HI_SUCCESS) {
        hi_err_mipi("mipi_tx_drv_reg_init fail.\n");
        goto out0;
    }

    ret = mipi_tx_drv_hw_init(dev_type);
    if (ret != HI_SUCCESS) {
        hi_err_mipi("mipi_tx_drv_hw_init fail.\n");
        goto out1;
    }

    return HI_SUCCESS;

out1:
    mipi_tx_drv_reg_exit();
out0:
    return HI_FAILURE;
}

hi_void mipi_tx_drv_exit(hi_void)
{
    mipi_tx_drv_reg_exit();
}
