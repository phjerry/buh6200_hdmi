/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver frl frlg module source file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#include <linux/hisilicon/securec.h>

#include "drv_hdmitx.h"
#include "hal_hdmitx_io.h"
#include "drv_hdmitx_modes.h"
#include "drv_hdmitx_connector.h"
#include "drv_hdmitx_ddc.h"
#include "hal_hdmitx_ctrl.h"
#include "hal_hdmitx_frl.h"
#include "hal_hdmitx_link_training_reg.h"
#include "hal_hdmitx_frl_reg.h"
#include "hal_hdmitx_phy.h"

/* Link Training State */
#define LTS_L 0
#define LTS_1 1
#define LTS_2 2
#define LTS_3 3
#define LTS_4 4
#define LTS_P 5

#define reg_ffe_n(ln)        (REG_SOURCE_FFE_UP0 + 0x4 * ((ln) / 2))
#define ffe_mask_n(ln)       (0xf << (0x4 * ((ln) / 2)))
#define ffe_value_n(ln, val) (((val) & 0xf) << (0x4 * ((ln) / 2)))

enum frl_data_source {
    FRL_GAP_PACKET,
    FRL_VIDEO,
    FRL_TRAINING_PATTERN
};

#define LTS1_EDID_FAIL     0
#define LTS1_PASS          1
#define LTS2_PASS          2
#define LTS2_TIMEOUT       3
#define LTS3_TIMEOUT       4
#define LTS3_REQ_NEW_RATE  5
#define LTS3_PASS          6
#define LTS4_LOWER_RATE    7
#define LTS4_NO_LOWER_RATE 8
#define LTSP_RETRAIN       9
#define LTSP_START         10
#define LTSP_TIMEOUT       11
#define LTSL               12

static const char *const g_frl_msg[] = {
    "LTS1 edid check fail",
    "LTS1 PASS",
    "LTS2 PASS",
    "LTS2 timeout",
    "LTS3 timeout",
    "LTS3 req new rate",
    "LTS3 pass",
    "LTS4 lower rate",
    "LTS4 no lower rate(fail)",
    "LTSP re-train",
    "LTSP start",
    "LTSP timeout",
    "LTSL",
};

struct frl_16to18_data {
    hi_u32 ram_addr;
    hi_u32 wr_data;
    hi_u8 sel_7b8b_9b10b;
};

#define FRL_9B10B_TABLE_SIZE 512
#define FRL_7B8B_TABLE_SIZE  128
/* frl 9b10b coding table */
static const hi_u32 g_table_9b_10b[FRL_9B10B_TABLE_SIZE] = {
    0x34b2d2, 0x8b1d32, 0x52ab52, 0xc18f92, 0x62a752, 0xa19792, 0x61a792, 0xe087d2,
    0x32b352, 0x919b92, 0x51ab92, 0xd08bd2, 0x31b392, 0xb093d2, 0x722372, 0xd10bb2,
    0x8d1cb2, 0x899d92, 0x49ad92, 0xc88dd2, 0x29b592, 0xa895d2, 0x64a6d2, 0xe87a10,
    0x19b992, 0x9899d2, 0x58a9d2, 0xd87610, 0x38b1d2, 0xb86e10, 0x785e10, 0xd20b72,
    0x8c9cd2, 0x859e92, 0x45ae92, 0xc40ef4, 0x25b692, 0xa416f4, 0x6426f4, 0xe47910,
    0x15ba92, 0x941af4, 0x542af4, 0xd47510, 0x3432f4, 0xb46d10, 0x745d10, 0xbfd00,
    0x2e3472, 0x8c1cf4, 0x4c2cf4, 0xcc7310, 0x2c34f4, 0xac6b10, 0x6c5b10, 0x13fb00,
    0x1c38f4, 0x9c6710, 0x5c5710, 0x23f700, 0x3c4f10, 0x43ef00, 0x83df00, 0x70a3d2,
    0x4e2c72, 0x961a72, 0x562a72, 0xc20f74, 0x363272, 0xa21774, 0x622774, 0xe27890,
    0x532b32, 0x921b74, 0x522b74, 0xd27490, 0x323374, 0xb26c90, 0x725c90, 0xdfc80,
    0xca0d72, 0x8a1d74, 0x4a2d74, 0xca7290, 0x2a3574, 0xaa6a90, 0x6a5a90, 0xea3a80,
    0x1a3974, 0x9a6690, 0x5a5690, 0xda3680, 0x3a4e90, 0xba2e80, 0x7a1e80, 0x6a2572,
    0xc60e72, 0x861e74, 0x462e74, 0xc67190, 0x263674, 0xa66990, 0x665990, 0xe63980,
    0x163a74, 0x966590, 0x565590, 0xd63580, 0x364d90, 0xb62d80, 0x761d80, 0xa61672,
    0xe3c74,  0x8e6390, 0x4e5390, 0xce3380, 0x2e4b90, 0xae2b80, 0x6e1b80, 0x11fb82,
    0x1e4790, 0x9e2780, 0x5e1780, 0x21f782, 0x3e0f80, 0x41ef82, 0x81df82, 0x9a1972,
    0x4d2cb2, 0x871e32, 0x472e32, 0xc10fb4, 0x273632, 0xa117b4, 0x6127b4, 0xe17850,
    0x173a32, 0x911bb4, 0x512bb4, 0xd17450, 0x3133b4, 0xb16c50, 0x715c50, 0xefc40,
    0xc90db2, 0x891db4, 0x492db4, 0xc97250, 0x2935b4, 0xa96a50, 0x695a50, 0xe93a40,
    0x1939b4, 0x996650, 0x595650, 0xd93640, 0x394e50, 0xb92e40, 0x791e40, 0x6925b2,
    0xc50eb2, 0x851eb4, 0x452eb4, 0xc57150, 0x2536b4, 0xa56950, 0x655950, 0xe53940,
    0x153ab4, 0x956550, 0x555550, 0xd53540, 0x354d50, 0xb52d40, 0x751d40, 0xa516b2,
    0xd3cb4,  0x8d6350, 0x4d5350, 0xcd3340, 0x2d4b50, 0xad2b40, 0x6d1b40, 0x12fb42,
    0x1d4750, 0x9d2740, 0x5d1740, 0x22f742, 0x3d0f40, 0x42ef42, 0x82df42, 0x9919b2,
    0xc30f32, 0x831f34, 0x432f34, 0xc370d0, 0x233734, 0xa368d0, 0x6358d0, 0xe338c0,
    0x133b34, 0x9364d0, 0x5354d0, 0xd334c0, 0x334cd0, 0xb32cc0, 0x731cc0, 0xa31732,
    0xb3d34,  0x8b62d0, 0x4b52d0, 0xcb32c0, 0x2b4ad0, 0xab2ac0, 0x6b1ac0, 0x14fac2,
    0x1b46d0, 0x9b26c0, 0x5b16c0, 0x24f6c2, 0x3b0ec0, 0x44eec2, 0x84dec2, 0x931b32,
    0x2b3532, 0x8761d0, 0x4751d0, 0xc731c0, 0x2749d0, 0xa729c0, 0x6719c0, 0x18f9c2,
    0x1745d0, 0x9725c0, 0x5715c0, 0x28f5c2, 0x370dc0, 0x48edc2, 0x88ddc2, 0x333332,
    0x1cb8d2, 0x8f23c0, 0x4f13c0, 0x30f3c2, 0x2f0bc0, 0x50ebc2, 0x90dbc2, 0x10fbc4,
    0x1f07c0, 0x60e7c2, 0xa0d7c2, 0x20f7c4, 0xc0cfc2, 0x40efc4, 0x80dfc4, 0xa915b2,
    0x2ab552, 0x869e52, 0x46ae52, 0xc08fd4, 0x26b652, 0xa097d4, 0x60a7d4, 0xe0f830,
    0x16ba52, 0x909bd4, 0x50abd4, 0xd0f430, 0x30b3d4, 0xb0ec30, 0x70dc30, 0xf7c20,
    0x8a9d52, 0x889dd4, 0x48add4, 0xc8f230, 0x28b5d4, 0xa8ea30, 0x68da30, 0xe8ba20,
    0x18b9d4, 0x98e630, 0x58d630, 0xd8b620, 0x38ce30, 0xb8ae20, 0x789e20, 0x68a5d2,
    0xc48ed2, 0x849ed4, 0x44aed4, 0xc4f130, 0x24b6d4, 0xa4e930, 0x64d930, 0xe4b920,
    0x14bad4, 0x94e530, 0x54d530, 0xd4b520, 0x34cd30, 0xb4ad20, 0x749d20, 0xa496d2,
    0xcbcd4,  0x8ce330, 0x4cd330, 0xccb320, 0x2ccb30, 0xacab20, 0x6c9b20, 0x137b22,
    0x1cc730, 0x9ca720, 0x5c9720, 0x237722, 0x3c8f20, 0x436f22, 0x835f22, 0x949ad2,
    0xc28f52, 0x829f54, 0x42af54, 0xc2f0b0, 0x22b754, 0xa2e8b0, 0x62d8b0, 0xe2b8a0,
    0x12bb54, 0x92e4b0, 0x52d4b0, 0xd2b4a0, 0x32ccb0, 0xb2aca0, 0x729ca0, 0xe20772,
    0xabd54,  0x8ae2b0, 0x4ad2b0, 0xcab2a0, 0x2acab0, 0xaaaaa0, 0x6a9aa0, 0x157aa2,
    0x1ac6b0, 0x9aa6a0, 0x5a96a0, 0x2576a2, 0x3a8ea0, 0x456ea2, 0x855ea2, 0x929b52,
    0xa29752, 0x86e1b0, 0x46d1b0, 0xc6b1a0, 0x26c9b0, 0xa6a9a0, 0x6699a0, 0x1979a2,
    0x16c5b0, 0x96a5a0, 0x5695a0, 0x2975a2, 0x368da0, 0x496da2, 0x895da2, 0x97da4,
    0x1e3872, 0x8ea3a0, 0x4e93a0, 0x3173a2, 0x2e8ba0, 0x516ba2, 0x915ba2, 0x117ba4,
    0x1e87a0, 0x6167a2, 0xa157a2, 0x2177a4, 0xc14fa2, 0x416fa4, 0x815fa4, 0xaa1572,
    0x54aad2, 0x819f94, 0x41af94, 0xc1f070, 0x21b794, 0xa1e870, 0x61d870, 0xe1b860,
    0x11bb94, 0x91e470, 0x51d470, 0xd1b460, 0x31cc70, 0xb1ac60, 0x719c60, 0xe107b2,
    0x9bd94,  0x89e270, 0x49d270, 0xc9b260, 0x29ca70, 0xa9aa60, 0x699a60, 0x167a62,
    0x19c670, 0x99a660, 0x599660, 0x267662, 0x398e60, 0x466e62, 0x865e62, 0x3931b2,
    0x951ab2, 0x85e170, 0x45d170, 0xc5b160, 0x25c970, 0xa5a960, 0x659960, 0x1a7962,
    0x15c570, 0x95a560, 0x559560, 0x2a7562, 0x358d60, 0x4a6d62, 0x8a5d62, 0xa7d64,
    0x1d38b2, 0x8da360, 0x4d9360, 0x327362, 0x2d8b60, 0x526b62, 0x925b62, 0x127b64,
    0x1d8760, 0x626762, 0xa25762, 0x227764, 0xc24f62, 0x426f64, 0x825f64, 0x7123b2,
    0x1ab952, 0x839f12, 0x43af12, 0xc3b0e0, 0x23b712, 0xa3a8e0, 0x6398e0, 0x1c78e2,
    0x13bb12, 0x93a4e0, 0x5394e0, 0x2c74e2, 0x338ce0, 0x4c6ce2, 0x8c5ce2, 0xc7ce4,
    0x1b3932, 0x8ba2e0, 0x4b92e0, 0x3472e2, 0x2b8ae0, 0x546ae2, 0x945ae2, 0x147ae4,
    0x1b86e0, 0x6466e2, 0xa456e2, 0x2476e4, 0xc44ee2, 0x446ee4, 0x845ee4, 0xb21372,
    0x8e1c72, 0x87a1e0, 0x4791e0, 0x3871e2, 0x2789e0, 0x5869e2, 0x9859e2, 0x1879e4,
    0x1785e0, 0x6865e2, 0xa855e2, 0x2875e4, 0xc84de2, 0x486de4, 0x885de4, 0x3532b2,
    0xfbc10,  0x7063e2, 0xb053e2, 0x3073e4, 0xd04be2, 0x506be4, 0x905be4, 0x4aad52,
    0xe047e2, 0x6067e4, 0xa057e4, 0x4cacd2, 0x3a3172, 0x2cb4d2, 0x2d34b2, 0x5a2972,
};

/* frl 7b8b coding table */
static const hi_u32 g_table_7b_8b[FRL_7B8B_TABLE_SIZE] = {
    0x946b2, 0x8c732, 0x4cb32, 0xc8372, 0x2cd32, 0xa8572, 0x68972, 0xe1e10,
    0x926d2, 0x98672, 0x58a72, 0xd1d10, 0x38c72, 0xb1b10, 0x71710, 0xff00,
    0x8a752, 0x88774, 0x48b74, 0xc9c90, 0x28d74, 0xa9a90, 0x69690, 0xe8e80,
    0x18e74, 0x99990, 0x59590, 0xd8d80, 0x39390, 0xb8b80, 0x78780, 0x16e92,
    0x54ab2, 0x847b4, 0x44bb4, 0xc5c50, 0x24db4, 0xa5a50, 0x65650, 0xe4e40,
    0x14eb4, 0x95950, 0x55550, 0xd4d40, 0x35350, 0xb4b40, 0x74740, 0x1ae52,
    0xc43b2, 0x8d8d0, 0x4d4d0, 0xcccc0, 0x2d2d0, 0xacac0, 0x6c6c0, 0x13ec2,
    0x1d1d0, 0x9c9c0, 0x5c5c0, 0x23dc2, 0x3c3c0, 0x43bc2, 0x837c2, 0x34cb2,
    0x52ad2, 0x827d4, 0x42bd4, 0xc3c30, 0x22dd4, 0xa3a30, 0x63630, 0xe2e20,
    0x12ed4, 0x93930, 0x53530, 0xd2d20, 0x33330, 0xb2b20, 0x72720, 0x1ce32,
    0xc23d2, 0x8b8b0, 0x4b4b0, 0xcaca0, 0x2b2b0, 0xaaaa0, 0x6a6a0, 0x15ea2,
    0x1b1b0, 0x9a9a0, 0x5a5a0, 0x25da2, 0x3a3a0, 0x45ba2, 0x857a2, 0x629d2,
    0x26d92, 0x87870, 0x47470, 0xc6c60, 0x27270, 0xa6a60, 0x66660, 0x19e62,
    0x17170, 0x96960, 0x56560, 0x29d62, 0x36360, 0x49b62, 0x89762, 0x32cd2,
    0x4ab52, 0x8e8e0, 0x4e4e0, 0x31ce2, 0x2e2e0, 0x51ae2, 0x916e2, 0x2ad52,
    0x1e1e0, 0x619e2, 0xa15e2, 0x86792, 0xc13e2, 0xa45b2, 0x649b2, 0xa25d2,
};

void frl_log_msg(hi_u32 msg_macro)
{
    if (msg_macro < ARRAY_SIZE(g_frl_msg)) {
        HDMI_ERR("%s", g_frl_msg[msg_macro]);
    }
}

static void frl_pfifo_up_threshold_set(struct frl *frl, hi_u32 val)
{
    hdmi_clrset(frl->base_addr, REG_PFIFO_LINE_THRESHOLD,
                REG_PFIFO_UP_THRESHOLD_M, reg_pfifo_up_threshold(val));
}

static void frl_pfifo_down_threshold_set(struct frl *frl, hi_u32 val)
{
    hdmi_clrset(frl->base_addr, REG_PFIFO_LINE_THRESHOLD,
                REG_PFIFO_DOWN_THRESHOLD_M, reg_pfifo_down_threshold(val));
}

static void frl_training_set_ffe(struct frl *frl, hi_u8 lnx, hi_u8 level)
{
    hi_u32 offset, mask, value;

    offset = reg_ffe_n(lnx);
    mask = ffe_mask_n(lnx);
    value = ffe_value_n(lnx, level);
    hdmi_clrset(frl->base_addr, offset, mask, value);
}

static void frl_config_data_source(struct frl *frl, enum frl_data_source source)
{
    int work_en = 0;
    int chan_sel = 0;

    switch (source) {
        case FRL_GAP_PACKET:
            work_en = 0;
            chan_sel = 1;
            break;
        case FRL_VIDEO:
            work_en = 1;
            chan_sel = 1;
            break;
        case FRL_TRAINING_PATTERN:
            chan_sel = 0;
            work_en = 0;
            break;
        default:
            break;
    }

    /**
     * Maybe it need to fixed depends on the hw behavior, in some cases,
     * the two register should be config in reverse order.
     */
    hdmi_clrset(frl->base_addr, REG_FRL_CHAN_SEL, REG_FRL_CHAN_SEL_M,
                reg_frl_chan_sel_f(chan_sel));
    hdmi_clrset(frl->base_addr, REG_WORK_EN, REG_WORK_EN_M, reg_work_en_f(work_en));
}

static void frl_training_pattern_reset(struct frl *frl)
{
    hdmi_clrset(frl->base_addr, REG_FRL_TRN_LTP0,
                (REG_LANE0_LTP_REQ_M | REG_LANE1_LTP_REQ_M), 0);
    hdmi_clrset(frl->base_addr, REG_FRL_TRN_LTP1,
                (REG_LANE2_LTP_REQ_M | REG_LANE3_LTP_REQ_M), 0);
    frl_config_data_source(frl, FRL_TRAINING_PATTERN);
}

static void frl_config_phy_oe(struct frl *frl, hi_bool enable)
{
    if (!frl || (!frl->hdmi)) {
        return;
    }

    if (enable) {
        hdmi_phy_on(frl->hdmi);
    } else {
        hdmi_phy_off(frl->hdmi);
    }
}

static void frl_config_phy_param(struct frl *frl)
{
#ifndef HI_FPGA_SUPPORT
    struct phy_frl phy_frl;
    struct hisilicon_hdmi *hdmi = frl->hdmi;

    if (frl->scdc.frl_rate) {
        phy_frl.frl_rate = frl->scdc.frl_rate;
        phy_frl.pcb_len = PCB_LEN_1;
        phy_frl.tpll_enable = hdmi->ctrl->tpll_enable;
        hal_phy_configure_frl(hdmi->phy, &phy_frl);
    }
#endif
}

static void sink_set_frl_rate_ffe_levels(struct frl *frl)
{
    hi_s32 ret;
    hi_u8 config_value;

    hdmi_clrset(frl->base_addr, REG_FRL_TRN_RATE, REG_FRL_TRN_RATE_M,
        reg_frl_trn_rate_f(frl->scdc.frl_rate));
    /* write scdc 0x31[7:4] FFE_Levels,[3:0] FRL_Rate */
    config_value = ((frl->scdc.frl_rate & TRAIN_FRL_RATE_MASK) |
                    ((frl->config.src_ffe_levels << 4) & TRAIN_FFE_LEVELS_MASK)); /* ffe level must left shift 4bits. */
    ret = hi_hdmi_scdc_writeb(frl->ddc, SCDC_SINK_CFG2, config_value);
    if (ret < 0) {
        frl->stat.event = TRAIN_EVENT_DDC_ERR;
        frl->scdc.frl_rate = 0;
        HDMI_ERR("FRL_rate=%d,FFE_levels=%d, scdc fail\n",
                 frl->scdc.frl_rate, frl->config.src_ffe_levels);
    }

    frl->scdc.ffe_levels = frl->config.src_ffe_levels;
}

static void sink_cleanup_frl_rate(struct frl *frl)
{
    hi_s32 ret;
    /* write scdc 0x31[7:4] FFE_Levels(0),[3:0] FRL_Rate(0) */
    ret = hi_hdmi_scdc_writeb(frl->ddc, SCDC_SINK_CFG2, 0);
    frl->scdc.frl_rate = 0;
    frl->scdc.ffe_levels = 0;
    if (ret < 0) {
        frl->stat.event = TRAIN_EVENT_DDC_ERR;
        HDMI_ERR("FRL_rate=%d,FFE_levels=%d, scdc fail\n",
                 frl->scdc.frl_rate, frl->scdc.ffe_levels);
    }
}

static void sink_get_flt_no_timeout(struct frl *frl)
{
    hi_s32 ret;
    hi_u8 value;

    /* read scdc addr offset 0x35 */
    ret = hi_hdmi_scdc_readb(frl->ddc, SCDC_SOU_TEST_CFG, &value);
    /* scdc ddc fail */
    if (ret < 0) {
        frl->stat.event = TRAIN_EVENT_DDC_ERR;
        /* err_log */
        return;
    }
    frl->scdc.flt_no_timeout = !!(value & TRAIN_FLT_NO_TIMEOUT_MASK);
}

static void sink_get_flt_status(struct frl *frl)
{
    hi_s32 ret;
    hi_u8 value;

    /* read scdc 0x10 */
    ret = hi_hdmi_scdc_readb(frl->ddc, SCDC_UPDATE_FLAG1, &value);
    /* scdc fail */
    if (ret < 0) {
        frl->stat.event = TRAIN_EVENT_DDC_ERR;
        HDMI_ERR("flt_start read scdc fail!\n");
    }
    /* frl->scdc.flt_start */
    frl->scdc.flt_start = !!(value & TRAIN_FLT_START_MASK);
    /* frl->scdc.flt_update */
    frl->scdc.flt_update = !!(value & TRAIN_FLT_UPDATE_MASK);

    if (frl->scdc.flt_start && frl->scdc.flt_update)
        HDMI_ERR("Sink conflict status:flt_start,flt_update=%d,%d.\n",
                 frl->scdc.flt_start, frl->scdc.flt_update);
}

hi_s32 sink_get_flt_sinkversion(struct frl *frl)
{
    hi_s32 ret;
    hi_u8 value;

    /* read scdc 0x10 */
    ret = hi_hdmi_scdc_readb(frl->ddc, SCDC_SINK_VERSION, &value);
    /* scdc fail */
    if (ret < 0) {
        frl->stat.event = TRAIN_EVENT_DDC_ERR;
        HDMI_ERR("flt_start read SCDC_SINK_VERSION fail!\n");
    }
    /* frl->scdc.flt_start */
    frl->config.sink_version = value;
	return value;
}

static hi_bool sink_get_flt_ready(struct frl *frl)
{
    hi_s32 ret;
    hi_u8 value;

    /* get flt ready flag  offset 0x40 */
    ret = hi_hdmi_scdc_readb(frl->ddc, SCDC_STATUS_FLAG1, &value);
    /* scdc ddc fail */
    if (ret < 0) {
        frl->stat.event = TRAIN_EVENT_DDC_ERR;
        HDMI_ERR("ddc error!\n");
        return false;
    }
    frl->scdc.flt_ready = !!(value & TRAIN_FLT_READY_MASK);

    return true;
}

static void frl_set_ln_total(struct frl *frl)
{
    if (frl->scdc.frl_rate == FRL_RATE_3G3L ||
        (frl->scdc.frl_rate == FRL_RATE_6G3L)) {
        frl->stat.work_3lane = true;
    } else {
        frl->stat.work_3lane = false;
    }
}

static hi_bool frl_is_change_rate(struct frl *frl)
{
    if (frl->scdc.ln_1_0_ltp_req == 0xff &&
        (frl->scdc.ln_3_2_ltp_req & 0x0f) == 0x0f &&
        frl->stat.work_3lane) {
        return true;
    } else if (frl->scdc.ln_1_0_ltp_req == 0xff &&
             frl->scdc.ln_3_2_ltp_req == 0xff &&
             (!frl->stat.work_3lane)) {
        return true;
    } else {
        return false;
    }
}

static hi_bool frl_is_train_pass(struct frl *frl)
{
    if (frl->scdc.ln_1_0_ltp_req == 0x0 &&
        (frl->scdc.ln_3_2_ltp_req & 0x0f) == 0x0 &&
        frl->stat.work_3lane) {
        return true;
    } else if (frl->scdc.ln_1_0_ltp_req == 0x0 &&
             frl->scdc.ln_3_2_ltp_req == 0x0 &&
             (!frl->stat.work_3lane)) {
        return true;
    } else {
        return false;
    }
}

static void sink_get_lnx_ltp_req(struct frl *frl)
{
    hi_u8 ln0_req;
    hi_u8 ln1_req;
    hi_u8 ln2_req;
    hi_u8 ln3_req;

    hi_s32 ret1;
    hi_s32 ret2;

    ret1 = hi_hdmi_scdc_readb(frl->ddc, SCDC_STATUS_FLAG2, &frl->scdc.ln_1_0_ltp_req);
    ret2 = hi_hdmi_scdc_readb(frl->ddc, SCDC_STATUS_FLAG3, &frl->scdc.ln_3_2_ltp_req);
    if ((ret1 < 0) || (ret2 < 0)) {
        frl->stat.event = TRAIN_EVENT_DDC_ERR;
        HDMI_ERR("lnx_req scdc read fail,ret1=%d,ret2=%d\n", ret1, ret2);
        return;
    }

    ln0_req = frl->scdc.ln_1_0_ltp_req & TRAIN_LN0_LTP_REQ_MASK;
    ln1_req = (frl->scdc.ln_1_0_ltp_req & TRAIN_LN1_LTP_REQ_MASK) >> 4; /* ln1_req must be right shift 4bit. */
    ln2_req = frl->scdc.ln_3_2_ltp_req & TRAIN_LN2_LTP_REQ_MASK;
    ln3_req = (frl->scdc.ln_3_2_ltp_req & TRAIN_LN3_LTP_REQ_MASK) >> 4; /* ln3_req must be right shift 4bit. */

    HDMI_INFO("sink lnx_req[0-3]=0x%02x,0x%02x,0x%02x,0x%02x,work_3lane=%u\n",
              ln0_req, ln1_req, ln2_req, ln3_req, frl->stat.work_3lane);

    if (ln0_req >= 1 && ln0_req <= 8) { /* ltp is in the 1~8 range. */
        hdmi_clrset(frl->base_addr, REG_FRL_TRN_LTP0, REG_LANE0_LTP_REQ_M, reg_lane0_ltp_req(ln0_req));
    } else if (ln0_req == 0xe) {
        frl->stat.ffe_levels[0] = (frl->stat.ffe_levels[0] + 1) % 4; /* lane0 ffe level can't over 3(4 - 1). */
        frl_training_set_ffe(frl, 0, frl->stat.ffe_levels[0]); /* lane0 ffe level set. */
    }

    if (ln1_req >= 1 && ln1_req <= 8) { /* ltp is in the 1~8 range. */
        hdmi_clrset(frl->base_addr, REG_FRL_TRN_LTP0, REG_LANE1_LTP_REQ_M, reg_lane1_ltp_req(ln1_req));
    } else if (ln1_req == 0xe) {
        frl->stat.ffe_levels[1] = (frl->stat.ffe_levels[1] + 1) % 4; /* lane1 ffe level can't over 3(4 - 1). */
        frl_training_set_ffe(frl, 1, frl->stat.ffe_levels[1]); /* lane1 ffe level set. */
    }

    if (ln2_req >= 1 && ln2_req <= 8) { /* ltp is in the 1~8 range. */
        hdmi_clrset(frl->base_addr, REG_FRL_TRN_LTP1, REG_LANE2_LTP_REQ_M, reg_lane2_ltp_req(ln2_req));
    } else if (ln2_req == 0xe) {
        frl->stat.ffe_levels[2] = (frl->stat.ffe_levels[2] + 1) % 4; /* lane2 ffe level can't over 3(4 - 1). */
        frl_training_set_ffe(frl, 2, frl->stat.ffe_levels[2]); /* lane2 ffe level set. */
    }

    if (frl->stat.work_3lane) {
        hdmi_clrset(frl->base_addr, REG_FRL_TRN_LTP1, REG_LANE3_LTP_REQ_M, reg_lane3_ltp_req(0));
        frl->stat.ffe_levels[3] = 0; /* lane3 ffe level must be set 0, when work mode is 3 lane. */
        frl_training_set_ffe(frl, 3, frl->stat.ffe_levels[3]); /* lane3 ffe level set. */
    } else if (ln3_req >= 1 && ln3_req <= 8) { /* ltp is in the 1~8 range. */
        hdmi_clrset(frl->base_addr, REG_FRL_TRN_LTP1, REG_LANE3_LTP_REQ_M, reg_lane3_ltp_req(ln3_req));
    } else if (ln3_req == 0xe) {
        frl->stat.ffe_levels[3] = (frl->stat.ffe_levels[3] + 1) % 4; /* lane3 ffe level can't over 3(4 - 1). */
        frl_training_set_ffe(frl, 3, frl->stat.ffe_levels[3]);
    }
}

void sink_clear_flt_start(struct frl *frl)
{
    hi_s32 ret;

    /* write scdc 0x10 source clears FLT_start by writing 1 */
    ret = hi_hdmi_scdc_writeb(frl->ddc, SCDC_UPDATE_FLAG1, TRAIN_FLT_START_MASK);
    /* scdc fail */
    if (ret < 0) {
        frl->stat.event = TRAIN_EVENT_DDC_ERR;
        HDMI_ERR("ddc error!\n");
        return;
    }
    frl->scdc.flt_start = false;
}

static void sink_clear_flt_update(struct frl *frl)
{
    hi_s32 ret;

    /* write scdc 0x10 source clears FLT_update by writing 1 */
    ret = hi_hdmi_scdc_writeb(frl->ddc, SCDC_UPDATE_FLAG1, TRAIN_FLT_UPDATE_MASK);
    /* scdc fail */
    if (ret < 0) {
        frl->stat.event = TRAIN_EVENT_DDC_ERR;
        HDMI_ERR("crear FLT_update scdc fail!\n");
        return;
    }
    frl->scdc.flt_update = false;
}

static void sink_get_flt_update(struct frl *frl)
{
    hi_s32 ret;
    hi_u8 value;

    /* read scdc 0x10 */
    ret = hi_hdmi_scdc_readb(frl->ddc, SCDC_UPDATE_FLAG1, &value);
    /* scdc ddc fail */
    if (ret < 0) {
        frl->stat.event = TRAIN_EVENT_DDC_ERR;
        HDMI_ERR("flt update read scdc fail!\n");
    }
    frl->scdc.flt_update = !!(value & TRAIN_FLT_UPDATE_MASK);
}

static void frl_hw_init(struct frl *frl)
{
    /* select CPU frl training mode */
    hdmi_clrset(frl->base_addr, REG_FRL_TRN_MODE, REG_FRL_TRN_MODE_M,
                reg_frl_trn_mode_f(0));
    /* pattern reset */
    frl_training_pattern_reset(frl);
}

static hi_bool frl_edid_check(struct frl *frl)
{
    if (!frl->config.scdc_present) {
        HDMI_ERR("scdc present=%d\n", frl->config.scdc_present);
        return false;
    }
    if (!frl->config.sink_version) {
        HDMI_ERR("scdc version=%d\n", frl->config.sink_version);
        return false;
    }
    if (!frl->config.frl_max_rate) {
        HDMI_ERR("frl_max_rate=%d\n", frl->config.frl_max_rate);
        return false;
    }
    HDMI_ERR("ffe=%d,max_rate=%d,min_rate=%d,sink_version=%d,scdc_present=%d \n", frl->config.src_ffe_levels,
              frl->config.frl_max_rate,
              frl->config.frl_min_rate,
              frl->config.sink_version,
              frl->config.scdc_present);
    return true;
}

static void frl_training_clear_ffe(struct frl *frl)
{
    frl_training_set_ffe(frl, 0, 0); /* Clear lane0 ffe level. */
    frl_training_set_ffe(frl, 1, 0); /* Clear lane1 ffe level. */
    frl_training_set_ffe(frl, 2, 0); /* Clear lane2 ffe level. */
    frl_training_set_ffe(frl, 3, 0); /* Clear lane3 ffe level. */
}

static void frl_ram_config_enable(struct frl *frl, hi_bool enable)
{
    hdmi_clrset(frl->base_addr, REG_LM_SRC_EN, REG_RAM_CONFIG_EN_M,
                reg_ram_config_en(enable));
}

static void frl_write_16to18_data(struct frl *frl,
                                  struct frl_16to18_data *data)
{
    hi_u32 value = 0;

    hdmi_writel(frl->base_addr, REG_LM_COMMAND_EN, reg_command_en(0));
    osal_udelay(1);

    /* step1: reg 0x320c, coding table write data */
    hdmi_writel(frl->base_addr, REG_LM_IN_AC0_WDATA, data->wr_data);
    osal_udelay(1);

    /*
     * step2: reg 0x3200,  bit[31] = 1,9b RAM; bit[31] = 0,7B RAM;
     * bit[24:16] = RAM addr;bit[15:8] = channel number,read used;bit[7:4]=0xa
     */
    value |= reg_command(0);
    value |= reg_in_ac0_number(0);
    value |= reg_protect_number(0xa);
    value |= reg_in_ac0_addr((data->sel_7b8b_9b10b << 15) | data->ram_addr); /* shift left 15 */
    hdmi_writel(frl->base_addr, REG_LM_IN_AC0_CMD, value);
    osal_udelay(1);

    /* step3: reg 0x321c=1 enable command */
    hdmi_writel(frl->base_addr, REG_LM_COMMAND_EN, reg_command_en(1));
    osal_udelay(1);

    /* step4: reg 0x321c=0 clear command */
    hdmi_writel(frl->base_addr, REG_LM_COMMAND_EN, reg_command_en(0));
    hdmi_clrset(frl->base_addr, REG_LM_IN_AC0_CMD, REG_PROTECT_NUMBER_M,
                reg_protect_number(0x5));
    osal_udelay(1);
}

static void frl_config_16to18_table(struct frl *frl)
{
    hi_u32 i = 0;
    struct frl_16to18_data config_data;

    /* step1:0x3210=0x1 */
    frl_ram_config_enable(frl, true);

    /* step2:7B8B coding table write operation */
    config_data.sel_7b8b_9b10b = 0;
    for (i = 0; i < ARRAY_SIZE(g_table_7b_8b); i++) {
        config_data.ram_addr = i;
        config_data.wr_data = g_table_7b_8b[i];
        frl_write_16to18_data(frl, &config_data);
    }

    /* step3:9B10B coding table write operation */
    config_data.sel_7b8b_9b10b = 1;
    for (i = 0; i < ARRAY_SIZE(g_table_9b_10b); i++) {
        config_data.ram_addr = i;
        config_data.wr_data = g_table_9b_10b[i];
        frl_write_16to18_data(frl, &config_data);
    }

    /**
	 * step4:when Soure write operation is done,
	 * Source clear operation enable register 0x3210
	 */
    osal_udelay(1);
    frl_ram_config_enable(frl, false);
}

static void frl_training_prepare(struct frl *frl)
{
    if (!frl) {
        return;
    }

    frl_config_data_source(frl, FRL_TRAINING_PATTERN);
}

static void frl_stat_init_defalts(struct frl_stat *stat)
{
    stat->ready_timeout = false;
    stat->tflt_timeout = false;
    stat->ltsp_timeout = false;
    stat->ltsp_poll = false;
    stat->phy_output = false;
    stat->video_transifer = false;
    stat->frl_start = false;
    stat->ffe_levels[0] = 0; /* lane0 ffe level init. */
    stat->ffe_levels[1] = 0; /* lane1 ffe level init. */
    stat->ffe_levels[2] = 0; /* lane2 ffe level init. */
    stat->ffe_levels[3] = 0; /* lane3 ffe level init. */
    stat->frl_state = LTS_L;
    stat->event = TRAIN_EVENT_SUCCESS;
}

static void frl_scdc_init_defaults(struct frl_scdc *scdc)
{
    scdc->frl_rate = 0;
    scdc->ffe_levels = 0;
    scdc->flt_no_timeout = false;
    scdc->frl_max = 0;
    scdc->flt_update = false;
    scdc->flt_start = false;
    scdc->flt_ready = false;
    scdc->ln_1_0_ltp_req = 0;
    scdc->ln_3_2_ltp_req = 0;
}

static void frl_config_init_defaults(struct frl_config *config)
{
    config->max_rate_proir = false;
    config->ready_timeout = TRAIN_READY_TIMEOUT;
    config->tflt_margin = TRAIN_TFLT_MARGIN;
    config->update_flag_magin = TRAIN_UPDATE_TIMEOUT_MAGIN;
    config->ltsp_poll_interval = TRAIN_LTSP_POLL_INTERVAL;
    config->frl_max_rate = FRL_RATE_12G4L;
    config->frl_min_rate = FRL_RATE_3G3L;
    config->src_ffe_levels = FRL_FFE_LEVELS_MAX;
    config->sink_version = 1;
    config->scdc_present = true;
}

static void frl_training_state_machine_reset(struct frl *frl)
{
    if (!frl) {
        return;
    }

    frl_stat_init_defalts(&frl->stat);
    frl_scdc_init_defaults(&frl->scdc);
    frl->stat.event = TRAIN_EVENT_SUCCESS;
    frl->stat.frl_state = LTS_1;
}

static void frl_event_notify(struct frl *frl)
{
    hi_char *event = "TRAIN_FAIL";

    if (frl == NULL) {
        HDMI_ERR("null pointer.\n");
        return;
    }

    hdmi_sysfs_event(frl->hdmi, event, sizeof(event));
}

static void frl_training_state_machine_stop(struct frl *frl)
{
    if (!frl) {
        return;
    }

    frl_stat_init_defalts(&frl->stat);
    frl_scdc_init_defaults(&frl->scdc);
    frl->stat.event = TRAIN_EVENT_DISABLE;
    frl->stat.frl_state = LTS_L;
}

void frl_config_crg_phy(struct frl *frl)
{
    struct hisilicon_hdmi *hdmi = NULL;
    struct hdmi_controller *ctrl = NULL;
    struct hdmi_hw_config *cur_config = NULL;

    if (frl == HI_NULL) {
        HDMI_ERR("frl poniter is null!\n");
        return;
    }

    hdmi = frl->hdmi;
    if (hdmi == HI_NULL) {
        HDMI_ERR("hdmi poniter is null!\n");
        return;
    }

    ctrl = hdmi->ctrl;
    if (ctrl == HI_NULL) {
        HDMI_ERR("ctrl poniter is null!\n");
        return;
    }

    cur_config = &ctrl->cur_hw_config;

    if (frl->scdc.frl_rate < frl->config.frl_min_rate &&
        frl->scdc.frl_rate >= frl->config.dsc_frl_min_rate) {
        cur_config->dsc_enable = true;
    }
    /* We must make sure the order set as follows. */
    frl_config_phy_param(frl);
    hdmi_crg_set(hdmi);

    if (!ctrl->ppll_enable) {
        hdmi_phy_fcg_set(hdmi);
    }

    /* Deepcolor need disable when dsc enable in the hdmi2.1 spec. */
    if (cur_config->dsc_enable) {
        hal_ctrl_disable_deepcolor_for_dsc(ctrl);
    }

    hdmi_soft_reset(hdmi, HI_FALSE);
    frl_config_16to18_table(frl);
}

static hi_void frl_training_lts1_process(struct frl *frl)
{

    if (!frl_edid_check(frl)) {
        frl->stat.event = TRAIN_EVENT_SINK_NO_SCDC;
        frl->stat.frl_state = LTS_L;
        frl_log_msg(LTS1_EDID_FAIL);
    } else {
        frl_log_msg(LTS1_PASS);
        frl->stat.frl_state = LTS_2;
        osal_timer_set(&frl->timer, frl->config.ready_timeout);
    }


}

static hi_void frl_training_lts2_process(struct frl *frl)
{
    sink_get_flt_ready(frl);
    if (frl->scdc.flt_ready) {
        sink_get_flt_no_timeout(frl);
        frl->scdc.frl_rate = frl->config.max_rate_proir ? frl->config.frl_max_rate : frl->config.frl_min_rate;
        frl->scdc.ffe_levels = 0;
        frl_training_clear_ffe(frl);
        /* To here,driver strratgy should confirm oe is down */
        frl_config_crg_phy(frl);
        frl_config_phy_oe(frl, true);
        sink_set_frl_rate_ffe_levels(frl);
        frl_set_ln_total(frl);
        osal_timer_set(&frl->timer, frl->config.tflt_margin + TRAIN_TFLT);
        HDMI_ERR("LTS2 rate=%u,start LTS3 timer\n", frl->scdc.frl_rate);
        frl_log_msg(LTS2_PASS);
        frl->stat.frl_state = LTS_3;
    } else if (frl->stat.ready_timeout) { /* timeout in timer */
        frl->stat.event = TRAIN_EVENT_READY_TIMEOUT;
        frl->stat.frl_state = LTS_L;
        frl_log_msg(LTS2_TIMEOUT);
    } else {
        osal_msleep(1); /* need sleep 1ms. */
    }
}

static hi_void frl_training_lts3_process(struct frl *frl)
{
    if (frl->stat.tflt_timeout) {
        frl_log_msg(LTS3_TIMEOUT);
        /* timeout in timer */
        frl->stat.event = TRAIN_EVENT_TFLT_TIMEOUT;
        frl->stat.frl_state = LTS_L;
    }
    sink_get_flt_update(frl);
	HDMI_ERR("222frl->scdc.flt_update=%d \n", frl->scdc.flt_update);
    if (frl->scdc.flt_update) {
        sink_get_lnx_ltp_req(frl);
        if (frl_is_change_rate(frl)) {
            frl->stat.frl_state = LTS_4;
            frl_log_msg(LTS3_REQ_NEW_RATE);
        } else if (frl_is_train_pass(frl)) {
            osal_timer_set(&frl->timer, frl->config.update_flag_magin + TRAIN_TFLT);
            frl->stat.frl_state = LTS_P;
            frl_config_data_source(frl, FRL_GAP_PACKET);
            frl_log_msg(LTS3_PASS);
        }
        sink_clear_flt_update(frl);
    } else {
        osal_msleep(1); /* need sleep 1ms. */
    }
}

static hi_void frl_training_lts4_process(struct frl *frl)
{
    frl_training_pattern_reset(frl);
    frl_training_clear_ffe(frl);
    if (frl->scdc.frl_rate > frl->config.dsc_frl_min_rate) {
        osal_timer_set(&frl->timer, frl->config.tflt_margin + TRAIN_TFLT);
        frl->scdc.frl_rate -= 1;
        frl_config_phy_oe(frl, false);
        frl_config_crg_phy(frl);
        frl_config_phy_oe(frl, true);
        sink_set_frl_rate_ffe_levels(frl);
        HDMI_ERR("LTS4 change rate=%u\n", frl->scdc.frl_rate);
        frl_set_ln_total(frl);
        frl_log_msg(LTS4_LOWER_RATE);
        frl->stat.frl_state = LTS_3;
    } else {
        frl_log_msg(LTS4_NO_LOWER_RATE);
        frl->stat.event = TRAIN_EVENT_LAST_RATE;
        frl->stat.frl_state = LTS_L;
    }
}

static hi_void frl_training_ltsp_process(struct frl *frl, hi_bool *machine_continue)
{
    sink_get_flt_status(frl);
    if (frl->scdc.flt_update) {
        frl_training_pattern_reset(frl);
        osal_timer_set(&frl->timer, frl->config.tflt_margin + TRAIN_TFLT);
        frl->stat.frl_state = LTS_3;
        sink_clear_flt_update(frl);
        frl_log_msg(LTSP_RETRAIN);
    } else if (frl->scdc.flt_start) {
        /*
         * hitxv300_frl_worken_set(frl,true,false) should be called
         * by hdmi_driver module.
         */
        *machine_continue = HI_FALSE;
        hdmi_frl_work_en(frl->hdmi);
        frl_log_msg(LTSP_START);
        frl->stat.event = TRAIN_EVENT_SUCCESS;
    } else if (frl->stat.ltsp_timeout) {
        frl->stat.event = TRAIN_EVENT_LTSP_TIMEOUT;
        frl->stat.frl_state = LTS_L;
        frl_log_msg(LTSP_TIMEOUT);
    } else {
        osal_msleep(1); /* need sleep 1ms. */
    }
}

static hi_void frl_training_ltsl_process(struct frl *frl, hi_bool *machine_continue)
{
    frl_log_msg(LTSL);
    sink_cleanup_frl_rate(frl);
    frl_training_pattern_reset(frl);
    *machine_continue = HI_FALSE;
    frl_event_notify(frl);
}

static hi_bool frl_training_state_machine_step(struct frl *frl)
{
    hi_bool continue_machine = HI_TRUE;
	HDMI_ERR("frl_training_state_machine_ste,LTS step=%d\n",frl->stat.frl_state);
    switch (frl->stat.frl_state) {
        case LTS_1:
            frl_training_lts1_process(frl);
            break;
        case LTS_2:
            frl_training_lts2_process(frl);
            break;
        case LTS_3:
            frl_training_lts3_process(frl);
            break;
        case LTS_4:
            frl_training_lts4_process(frl);
            break;
        case LTS_P:
            frl_training_ltsp_process(frl, &continue_machine);
            break;

        case LTS_L:
        default:
            frl_training_ltsl_process(frl, &continue_machine);
            break;
    }

    if (frl->stat.event == TRAIN_EVENT_DDC_ERR) {
        frl->stat.frl_state = LTS_L;
        continue_machine = HI_FALSE;
    }

    if (!continue_machine) {
        osal_timer_del(&frl->timer);
    }

    return continue_machine;
}

static void timer_callback(hi_ulong data)
{
    struct frl *frl = (struct frl *)data;

    if (frl == HI_NULL) {
        HDMI_ERR("null!\n");
        return;
    }

    if (frl->stat.frl_state == LTS_2) {
        frl->stat.ready_timeout = true;
    }

    if (frl->stat.frl_state == LTS_3) {
        frl->stat.tflt_timeout = true && (!frl->scdc.flt_no_timeout);
    }

    if (frl->stat.frl_state == LTS_P) {
        frl->stat.ltsp_timeout = true;
    }
}

static void work_queue_callback(struct work_struct *work)
{
    hi_bool continue_mach = true;
    struct frl *frl = container_of(work, struct frl, dl_work.work);

    if (frl == HI_NULL) {
        HDMI_ERR("null !\n");
        return;
    }

    if (frl->stat.frl_state == LTS_P) {
        sink_get_flt_update(frl);
        if (frl->stat.event == TRAIN_EVENT_DDC_ERR) {
            frl_event_notify(frl);
            return;
        }

        if (!frl->scdc.flt_update) {
            frl->stat.ltsp_poll = true;
            schedule_delayed_work(&frl->dl_work,
                                  osal_msecs_to_jiffies(frl->config.ltsp_poll_interval));
        } else {
            HDMI_ERR("flt_update=%d,re-train LTS3\n", frl->scdc.flt_update);
            osal_timer_set(&frl->timer, frl->config.tflt_margin + TRAIN_TFLT);
            frl->stat.frl_state = LTS_3;
            frl->stat.ltsp_poll = false;
            frl_training_pattern_reset(frl);
            sink_clear_flt_update(frl);
            do {
                continue_mach = frl_training_state_machine_step(frl);
            } while (continue_mach);
        }
    }
}

void hitxv300_frl_worken_set(struct frl *frl, hi_bool enable, hi_bool fast_mode)
{
    if (frl == HI_NULL) {
        return;
    }

    if (enable) {
        frl_config_data_source(frl, FRL_VIDEO);
        HDMI_ERR("true!\n");
    } else {
        frl_config_data_source(frl, FRL_GAP_PACKET);
        HDMI_ERR("false!\n");
    }
    if (enable && (!fast_mode)) {
        sink_clear_flt_start(frl);
        frl->stat.ltsp_poll = true;
        HDMI_ERR("poll update!\n");
        schedule_delayed_work(&frl->dl_work, msecs_to_jiffies(frl->config.ltsp_poll_interval));
    }
}

hi_s32 hitxv300_frl_config_set(struct frl *frl, struct frl_config *config)
{
    if (frl != HI_NULL && config != HI_NULL) {
        /* check */
        frl->config = *config;
        return 0;
    }
    return -EINVAL;
}

hi_s32 hitxv300_frl_config_get(struct frl *frl, struct frl_config *config)
{
    if (frl != HI_NULL && config != HI_NULL) {
        *config = frl->config;
        return 0;
    }
    return -EINVAL;
}

hi_s32 hitxv300_frl_scdc_get(struct frl *frl, struct frl_scdc *scdc)
{
    if (frl != HI_NULL && scdc != HI_NULL) {
        *scdc = frl->scdc;
        return 0;
    }
    return -EINVAL;
}

hi_s32 hitxv300_frl_stat_get(struct frl *frl, struct frl_stat *stat)
{
    if (frl != HI_NULL && stat != HI_NULL) {
        *stat = frl->stat;
        return 0;
    }
    return -EINVAL;
}

hi_s32 hitxv300_frl_start(struct frl *frl)
{
    hi_bool continue_mach = HI_FALSE;

    if ((frl == HI_NULL) ||
        (frl->hdmi == HI_NULL) ||
        (frl->base_addr == HI_NULL)) {
        return -EINVAL;
    }
    HDMI_ERR("hitxv300_frl_start!\n");
    frl_training_prepare(frl);
    frl_training_state_machine_reset(frl);

    do {
        continue_mach = frl_training_state_machine_step(frl);
    } while (continue_mach);

    return frl->stat.event;
}

void hitxv300_frl_stop(struct frl *frl)
{
    if (frl == HI_NULL) {
        return;
    }
    sink_cleanup_frl_rate(frl);
    frl_config_data_source(frl, FRL_TRAINING_PATTERN);
    frl_training_state_machine_stop(frl);
    HDMI_ERR("stop FRL!\n");
}

struct frl *hitxv300_frl_init(struct hdmi_controller *parent, void *reg_base)
{
    struct frl *frl = HI_NULL;

    if (parent == HI_NULL || reg_base == HI_NULL || parent->parent == HI_NULL) {
        HDMI_ERR("ptr is null.\n");
        return HI_NULL;
    }

    frl = osal_kmalloc(HI_ID_HDMITX, sizeof(struct frl), OSAL_GFP_KERNEL);
    if (frl == HI_NULL) {
        HDMI_ERR("osal_kmalloc fail.\n");
        return HI_NULL;
    }

    if (memset_s(frl, sizeof(struct frl), 0, sizeof(struct frl))) {
        HDMI_ERR("memset_s fail.\n");
        osal_kfree(HI_ID_HDMITX, frl);
        return HI_NULL;
    }

    frl->hdmi = parent->parent;
    frl->ddc = frl->hdmi->ddc;
    frl->base_addr = reg_base;

    osal_timer_init(&frl->timer);
    frl->timer.handler = timer_callback;
    frl->timer.data = (hi_ulong)frl;

    INIT_DELAYED_WORK(&frl->dl_work, work_queue_callback);

    frl_stat_init_defalts(&frl->stat);
    frl_scdc_init_defaults(&frl->scdc);
    frl_config_init_defaults(&frl->config);
    frl_hw_init(frl);

    return frl;
}

void hitxv300_frl_exit(struct frl *frl)
{
    if (frl != HI_NULL) {
        osal_timer_del(&frl->timer);
        cancel_delayed_work_sync(&frl->dl_work);
        osal_kfree(HI_ID_HDMITX, frl);
    }
}


hi_s32 hal_frl_debug(struct hisilicon_hdmi *hdmi, enum debug_ext_cmd_list cmd, struct hdmitx_debug_msg msg)
{
    hi_s32 ret = HI_SUCCESS;
    struct hdmi_controller *ctrl = HI_NULL;
    struct frl *frl_info = HI_NULL;
    hi_u32 sub_cmd = msg.sub_cmd;
    void *data = msg.data;

    if (hdmi == HI_NULL || data == HI_NULL) {
        HDMI_ERR("The poniter is NULL!\n");
        return HI_FAILURE;
    }

    ctrl = hdmi->ctrl;
    if (ctrl == HI_NULL) {
        HDMI_ERR("The poniter is NULL!\n");
        return HI_FAILURE;
    }

    frl_info = ctrl->frl;
    if (frl_info == HI_NULL) {
        HDMI_ERR("The poniter is NULL!\n");
        return HI_FAILURE;
    }

    switch (cmd) {
        case DEBUG_CMD_FIFO_THRESHOLD:
            if (sub_cmd == 0) {
                frl_pfifo_up_threshold_set(frl_info, *(hi_u16 *)data);
            } else if (sub_cmd == 1) {
                frl_pfifo_down_threshold_set(frl_info, *(hi_u16 *)data);
            } else {
                ret = HI_FAILURE;
            }
            break;
        default:
            HDMI_ERR("frl cmd invalid!\n");
    }

    return ret;
}

