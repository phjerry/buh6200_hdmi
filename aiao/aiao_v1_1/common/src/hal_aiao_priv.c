/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao hardware driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include "hi_osal.h"
#include "hi_aiao_log.h"

#include "hi_reg_common.h"
#include "hi_drv_sys.h"

#include "hal_aiao_priv.h"
#include "hi_audsp_aoe.h"
#include "audio_util.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

static volatile hi_reg_crg      *g_reg_aiao_crg = HI_NULL;

static volatile S_AIAO_COM_REGS_TYPE *g_aiao_com_reg = HI_NULL;
static volatile S_AIAO_RX_REGS_TYPE *g_aiao_rx_reg[AIAO_MAX_PORT_NUMBER];
static volatile S_AIAO_TX_REGS_TYPE *g_aiao_tx_reg[AIAO_MAX_PORT_NUMBER];
static volatile S_AIAO_TXSPDIF_REGS_TYPE *g_aiao_tx_spdif_reg[AIAO_MAX_PORT_NUMBER];
static volatile S_AIAO_SPDIFER_REGS_TYPE *g_aiao_spdifer_reg[AIAO_MAX_PORT_NUMBER];

/* global */
static hi_void aiao_low_set_rx_crg_clk_en(hi_u32 chn_id, hi_s32 en);
static hi_void aiao_low_set_tx_crg_clk_en(hi_u32 chn_id, hi_s32 en);

static hi_void aiao_low_set_spdif_master_clk_en(hi_u32 chn_id, hi_s32 en);

static hi_void aiao_low_set_rx_i2s_slave_clk(hi_u32 chn_id, aiao_if_attr *if_attr_slave);
static hi_void aiao_low_set_tx_i2s_slave_clk(hi_u32 chn_id, aiao_if_attr *if_attr_slave);

static hi_void aiao_low_set_rx_i2s_master_clk(hi_u32 chn_id, aiao_if_attr *if_attr_master);
static hi_void aiao_low_set_tx_i2s_master_clk(hi_u32 chn_id, aiao_if_attr *if_attr_master);

static hi_void aiao_low_set_spdif_master_clk(hi_u32 chn_id, aiao_if_attr *if_attr_master);
static hi_void aiao_low_set_i2s_dulicate_clk(hi_u32 chn_id, hi_s32 dir, aiao_if_attr *if_attr_duplicate);

/* tx/rx i2s interface */
static hi_void aiao_low_set_i2s_source_select(hi_u32 chn_id, hi_s32 dir, aiao_i2s_source i2s_sel);
static hi_void aiao_low_set_i2s_data_select(hi_u32 chn_id, hi_s32 dir, aiao_i2s_sd org_sd,
                                            aiao_i2s_sd src_sd);
static hi_void aiao_low_set_i2s_bit_depth(hi_u32 chn_id, hi_s32 dir, aiao_bit_depth bit_depth);
static hi_void aiao_low_set_i2s_mode(hi_u32 chn_id, hi_s32 dir, aiao_i2s_mode i2s_mode);
static hi_void aiao_low_set_track_mode(hi_u32 chn_id, hi_s32 dir, aiao_track_mode track_mode);
static hi_void aiao_low_set_multislot_mode(hi_u32 chn_id, hi_s32 dir, hi_s32 en);
static hi_void aiao_low_set_i2s_ch_num(hi_u32 chn_id, hi_s32 dir, hi_s32 is_multislot, aiao_i2s_chnum i2s_chnum);
static hi_void aiao_low_set_pcm_sync_delay(hi_u32 chn_id, hi_s32 dir, hi_s32 delay_cycles);
static hi_void aiao_low_spdif_set_bit_depth(hi_u32 chn_id, aiao_bit_depth bit_depth);
static hi_void aiao_low_spdif_set_track_mode(hi_u32 chn_id, aiao_track_mode track_mode);
static hi_void aiao_low_spdif_set_ch_num(hi_u32 chn_id, aiao_i2s_chnum i2s_chnum);

/* tx/rx I2S DSP */
static hi_void aiao_low_set_start(hi_u32 chn_id, hi_s32 dir, hi_s32 en);
static hi_void aiao_low_set_mute(hi_u32 chn_id, hi_s32 dir, hi_s32 en);
static hi_void aiao_low_set_mute_fade(hi_u32 chn_id, hi_s32 dir, hi_s32 en);
static hi_void aiao_low_set_fade_in_rate(hi_u32 chn_id, hi_s32 dir, aiao_fade_rate fade_rate);
static hi_void aiao_low_set_fade_out_rate(hi_u32 chn_id, hi_s32 dir, aiao_fade_rate fade_rate);
static hi_void aiao_low_set_volume_db(hi_u32 chn_id, hi_s32 dir, hi_s32 vol_db);
static hi_void aiao_low_set_bypass(hi_u32 chn_id, hi_s32 dir, hi_s32 en);
static hi_s32 aiao_low_get_stop_done_status(hi_u32 chn_id, hi_s32 dir);

/* tx SPDIF DSP */
static hi_void aiao_low_spdif_set_start(hi_u32 chn_id, hi_s32 en);
static hi_void aiao_low_spdif_set_mute(hi_u32 chn_id, hi_s32 en);
static hi_void aiao_low_spdif_set_mute_fade(hi_u32 chn_id, hi_s32 en);
static hi_void aiao_low_spdif_set_fade_in_rate(hi_u32 chn_id, aiao_fade_rate fade_rate);
static hi_void aiao_low_spdif_set_fade_out_rate(hi_u32 chn_id, aiao_fade_rate fade_rate);
static hi_void aiao_low_spdif_set_volume_db(hi_u32 chn_id, hi_s32 vol_db);
static hi_void aiao_low_spdif_set_bypass(hi_u32 chn_id, hi_s32 en);
static hi_s32 aiao_low_spdif_get_stop_done_status(hi_u32 chn_id);

/* tx i2s/spdif buffer */
static hi_void aiao_txbuf_set_buf_addr_and_size(hi_u32 chn_id, hi_s32 tx_type, hi_u32 start_addr,
                                                hi_u32 size);
static hi_void aiao_txbuf_set_buf_wptr(hi_u32 chn_id, hi_s32 tx_type, hi_u32 wptr);
static hi_void aiao_txbuf_set_buf_rptr(hi_u32 chn_id, hi_s32 tx_type, hi_u32 rptr);
static hi_void aiao_txbuf_get_buf_wptr(hi_u32 chn_id, hi_s32 tx_type, hi_u32 *wptr);
static hi_void aiao_txbuf_get_buf_rptr(hi_u32 chn_id, hi_s32 tx_type, hi_u32 *rptr);
static hi_void aiao_txbuf_set_buf_trans_size(hi_u32 chn_id, hi_s32 tx_type, hi_u32 period_size);
static hi_void aiao_txbuf_set_buf_alempty_size(hi_u32 chn_id, hi_s32 tx_type, hi_u32 alempty_size);
static hi_void aiao_txbuf_get_debug_bclk_cnt(hi_u32 chn_id, hi_s32 tx_type, hi_u32 *bclk_cnt);
static hi_void aiao_txbuf_get_debug_fclk_cnt(hi_u32 chn_id, hi_s32 tx_type, hi_u32 *fclk_cnt);
static hi_void aiao_txbuf_get_buf_wptr_addr(hi_u32 chn_id, hi_s32 tx_type, hi_u32 **wptr_addr);
static hi_void aiao_txbuf_get_buf_rptr_addr(hi_u32 chn_id, hi_s32 tx_type, hi_u32 **rptr_addr);

/* rx buffer */
static hi_void aiao_rxbuf_set_buf_addr_and_size(hi_u32 chn_id, hi_u32 start_addr, hi_u32 size);
static hi_void aiao_rxbuf_set_buf_wptr(hi_u32 chn_id, hi_u32 wptr);
static hi_void aiao_rxbuf_set_buf_rptr(hi_u32 chn_id, hi_u32 rptr);
static hi_void aiao_rxbuf_get_buf_wptr(hi_u32 chn_id, hi_u32 *wptr);
static hi_void aiao_rxbuf_get_buf_rptr(hi_u32 chn_id, hi_u32 *rptr);
static hi_void aiao_rxbuf_set_buf_trans_size(hi_u32 chn_id, hi_u32 period_size);
static hi_void aiao_rxbuf_set_buf_alfull_size(hi_u32 chn_id, hi_u32 alfull_size);
static hi_void aiao_rxbuf_get_debug_bclk_cnt(hi_u32 chn_id, hi_u32 *bclk_cnt);
static hi_void aiao_rxbuf_get_debug_fclk_cnt(hi_u32 chn_id, hi_u32 *fclk_cnt);
static hi_void aiao_rxbuf_get_buf_wptr_addr(hi_u32 chn_id, hi_u32 **wptr_addr);
static hi_void aiao_rxbuf_get_buf_rptr_addr(hi_u32 chn_id, hi_u32 **rptr_addr);

/* tx/rx i2s/spdif interface */
hi_void aiao_hw_set_i2s_source_select(aiao_port_id port_id, aiao_i2s_source i2s_sel);
hi_void aiao_hw_set_i2s_data_select(aiao_port_id port_id, aiao_i2s_sd org_sd, aiao_i2s_sd src_sd);
hi_void aiao_hw_set_i2s_bit_depth(aiao_port_id port_id, aiao_bit_depth bit_depth);
hi_void aiao_hw_set_i2s_mode(aiao_port_id port_id, aiao_i2s_mode i2s_mode);
hi_void aiao_hw_set_track_mode(aiao_port_id port_id, aiao_track_mode track_mode);
hi_void aiao_hw_set_multislot_mode(aiao_port_id port_id, hi_s32 en);
hi_void aiao_hw_set_i2s_ch_num(aiao_port_id port_id, hi_s32 is_multislot, aiao_i2s_chnum i2s_chnum);
hi_void aiao_hw_set_pcm_sync_delay(aiao_port_id port_id, hi_s32 delay_cycles);

hi_void aiao_spdif_hal_set_mode(aiao_port_id port_id, aiao_spdif_mode spdif_mode);
hi_void aiao_spdif_hal_set_enable(aiao_port_id port_id, hi_s32 en);
hi_void aiao_spdif_hal_set_unknow(aiao_port_id port_id);
hi_void aiao_spdif_hal_set_bit_width(aiao_port_id port_id, aiao_bit_depth bit_depth);
hi_void aiao_spdif_hal_set_samplerate(aiao_port_id port_id, aiao_sample_rate sample_rate);

static const hi_u16 g_bclk_div_tab[16] = {
    1, 3, 2, 4, 6, 8, 12, 16, 24, 32, 48, 64, 8, 8, 8, 8
};

static const hi_u16 g_fs_div_tab[6] = {
    16,
    32,
    48,
    64,
    128,
    256,
};

/* aiao_replace */
#define AIAO_0128FS_TAB_IDX 0
#define AIAO_0256FS_TAB_IDX 1
#define AIAO_0384FS_TAB_IDX 2
#define AIAO_0512FS_TAB_IDX 3
#define AIAO_1024FS_TAB_IDX 4
#define AIAO_FS_TAB_IDX_NUM 5

#define AIAO_008_KHZ_SUB_IDX 0
#define AIAO_011_KHZ_SUB_IDX 1
#define AIAO_012_KHZ_SUB_IDX 2
#define AIAO_016_KHZ_SUB_IDX 3
#define AIAO_022_KHZ_SUB_IDX 4
#define AIAO_024_KHZ_SUB_IDX 5
#define AIAO_032_KHZ_SUB_IDX 6
#define AIAO_044_KHZ_SUB_IDX 7
#define AIAO_048_KHZ_SUB_IDX 8
#define AIAO_088_KHZ_SUB_IDX 9
#define AIAO_096_KHZ_SUB_IDX 10
#define AIAO_176_KHZ_SUB_IDX 11
#define AIAO_192_KHZ_SUB_IDX 12
#define AIAO_FS_SUB_IDX_NUM 13

/*
 * this file is machine generated, DO NOT EDIT!
 */
#if defined(AIAO_PLL_600MHZ)
static hi_u32 g_mclk_pll_tab[5][13] = {
    /* 128*FS mclk */
    {   0x00037ec8, /* 8000 */ 0x0004d120, /* 11025 */ 0x00053e2d, /* 12000 */ 0x0006fd91, /* 16000 */
        0x0009a240, /* 22050 */ 0x000a7c5a, /* 24000 */ 0x000dfb23, /* 32000 */ 0x00134480, /* 44100 */
        0x0014f8b5, /* 48000 */ 0x00268900, /* 88200 */ 0x0029f16b, /* 96000 */ 0x004d1201, /* 176400 */
        0x0053e2d6, /* 192000 */
    },
    /* 256*FS mclk */
    {   0x0006fd91, /* 8000 */ 0x0009a240, /* 11025 */ 0x000a7c5a, /* 12000 */ 0x000dfb23, /* 16000 */
        0x00134480, /* 22050 */ 0x0014f8b5, /* 24000 */ 0x001bf647, /* 32000 */ 0x00268900, /* 44100 */
        0x0029f16b, /* 48000 */ 0x004d1201, /* 88200 */ 0x0053e2d6, /* 96000 */ 0x009a2403, /* 176400 */
        0x00a7c5ac, /* 192000 */
    },
    /* 384*FS mclk */
    {   0x000a7c5a, /* 8000 */ 0x000e7360, /* 11025 */ 0x000fba88, /* 12000 */ 0x0014f8b5, /* 16000 */
        0x001ce6c0, /* 22050 */ 0x001f7510, /* 24000 */ 0x0029f16b, /* 32000 */ 0x0039cd81, /* 44100 */
        0x003eea20, /* 48000 */ 0x00739b02, /* 88200 */ 0x007dd441, /* 96000 */ 0x00e73605, /* 176400 */
        0x00fba882, /* 192000 */
    },
    /* 512*FS mclk */
    {   0x000dfb23, /* 8000 */ 0x00134480, /* 11025 */ 0x0014f8b5, /* 12000 */ 0x001bf647, /* 16000 */
        0x00268900, /* 22050 */ 0x0029f16b, /* 24000 */ 0x0037ec8e, /* 32000 */ 0x004d1201, /* 44100 */
        0x0053e2d6, /* 48000 */ 0x009a2403, /* 88200 */ 0x00a7c5ac, /* 96000 */ 0x01344806, /* 176400 */
        0x014f8b58, /* 192000 */
    },
    /* 1024*FS mclk */
    {   0x001bf647, /* 8000 */ 0x00268900, /* 11025 */ 0x0029f16b, /* 12000 */ 0x0037ec8e, /* 16000 */
        0x004d1201, /* 22050 */ 0x0053e2d6, /* 24000 */ 0x006fd91d, /* 32000 */ 0x009a2403, /* 44100 */
        0x00a7c5ac, /* 48000 */ 0x01344806, /* 88200 */ 0x014f8b58, /* 96000 */ 0x0268900c, /* 176400 */
        0x029f16b0, /* 192000 */
    },
};

#elif defined(AIAO_PLL_492MHZ)
static hi_u32 g_mclk_pll_tab[5][13] = {
    /* crg: mclk0*pow(2, CRG_POW)/ ARM_BPLL_FREQ */
    /* 128*FS mclk */
    {   0x00044444, /* 8000 */ 0x0005e147, /* 11025 */ 0x00066666, /* 12000 */ 0x00088888, /* 16000 */
        0x000bc28f, /* 22050 */ 0x000ccccc, /* 24000 */ 0x00111111, /* 32000 */ 0x0017851e, /* 44100 */
        0x00199999, /* 48000 */ 0x002f0a3d, /* 88200 */ 0x00333333, /* 96000 */ 0x005e147b, /* 176400 */
        0x00666666, /* 192000 */
    },
    /* 256*FS mclk */
    {   0x00088888, /* 8000 */ 0x000bc28f, /* 11025 */ 0x000ccccc, /* 12000 */ 0x00111111, /* 16000 */
        0x0017851e, /* 22050 */ 0x00199999, /* 24000 */ 0x00222222, /* 32000 */ 0x002f0a3d, /* 44100 */
        0x00333333, /* 48000 */ 0x005e147b, /* 88200 */ 0x00666666, /* 96000 */ 0x00bc28f6, /* 176400 */
        0x00cccccd, /* 192000 */
    },
    /* 384*FS mclk */
    {   0x000ccccc, /* 8000 */ 0x0011a3d7, /* 11025 */ 0x00133333, /* 12000 */ 0x00199999, /* 16000 */
        0x002347ae, /* 22050 */ 0x00266666, /* 24000 */ 0x00333333, /* 32000 */ 0x00468f5c, /* 44100 */
        0x004ccccd, /* 48000 */ 0x008d1eb8, /* 88200 */ 0x0099999a, /* 96000 */ 0x011a3d70, /* 176400 */
        0x01333334, /* 192000 */
    },
    /* 512*FS mclk */
    {   0x00111111, /* 8000 */ 0x0017851e, /* 11025 */ 0x00199999, /* 12000 */ 0x00222222, /* 16000 */
        0x002f0a3d, /* 22050 */ 0x00333333, /* 24000 */ 0x00444444, /* 32000 */ 0x005e147b, /* 44100 */
        0x00666666, /* 48000 */ 0x00bc28f6, /* 88200 */ 0x00cccccd, /* 96000 */ 0x017851ec, /* 176400 */
        0x0199999a, /* 192000 */
    },
    /* 1024*FS mclk */
    {   0x00222222, /* 8000 */ 0x002f0a3d, /* 11025 */ 0x00333333, /* 12000 */ 0x00444444, /* 16000 */
        0x005e147b, /* 22050 */ 0x00666666, /* 24000 */ 0x00888889, /* 32000 */ 0x00bc28f6, /* 44100 */
        0x00cccccd, /* 48000 */ 0x017851ec, /* 88200 */ 0x0199999a, /* 96000 */ 0x02f0a3d8, /* 176400 */
        0x03333334, /* 192000 */
    },
};

#elif defined(AIAO_PLL_995MHZ)
static hi_u32 g_mclk_pll_tab[5][13] = {
    /* crg: mclk0*pow(2, CRG_POW)/ ARM_BPLL_FREQ */
    /* 128*FS mclk */
    {   0x00021b64, /* 8000 */ 0x0002e759, /* 11025 */ 0x00032916, /* 12000 */ 0x000436c8, /* 16000 */
        0x0005ceb2, /* 22050 */ 0x0006522c, /* 24000 */ 0x00086d90, /* 32000 */ 0x000b9d64, /* 44100 */
        0x000ca458, /* 48000 */ 0x00173ac9, /* 88200 */ 0x001948b1, /* 96000 */ 0x002e7592, /* 176400 */
        0x00329162, /* 192000 */
    },
    /* 256*FS mclk */
    {   0x000436c8, /* 8000 */ 0x0005ceb2, /* 11025 */ 0x0006522c, /* 12000 */ 0x00086d90, /* 16000 */
        0x000b9d64, /* 22050 */ 0x000ca458, /* 24000 */ 0x0010db20, /* 32000 */ 0x00173ac9, /* 44100 */
        0x001948b1, /* 48000 */ 0x002e7592, /* 88200 */ 0x00329162, /* 96000 */ 0x005ceb24, /* 176400 */
        0x006522c4, /* 192000 */
    },
    /* 384*FS mclk */
    {   0x0006522c, /* 8000 */ 0x0008b60b, /* 11025 */ 0x00097b42, /* 12000 */ 0x000ca458, /* 16000 */
        0x00116c16, /* 22050 */ 0x0012f684, /* 24000 */ 0x001948b1, /* 32000 */ 0x0022d82d, /* 44100 */
        0x0025ed09, /* 48000 */ 0x0045b05b, /* 88200 */ 0x004bda13, /* 96000 */ 0x008b60b6, /* 176400 */
        0x0097b426, /* 192000 */
    },
    /* 512*FS mclk */
    {   0x00086d90, /* 8000 */ 0x000b9d64, /* 11025 */ 0x000ca458, /* 12000 */ 0x0010db20, /* 16000 */
        0x00173ac9, /* 22050 */ 0x001948b1, /* 24000 */ 0x0021b641, /* 32000 */ 0x002e7592, /* 44100 */
        0x00329162, /* 48000 */ 0x005ceb24, /* 88200 */ 0x006522c4, /* 96000 */ 0x00b9d648, /* 176400 */
        0x00ca4588, /* 192000 */
    },
    /* 1024*FS mclk */
    {   0x0010db20, /* 8000 */ 0x00173ac9, /* 11025 */ 0x001948b1, /* 12000 */ 0x0021b641, /* 16000 */
        0x002e7592, /* 22050 */ 0x00329162, /* 24000 */ 0x00436c82, /* 32000 */ 0x005ceb24, /* 44100 */
        0x006522c4, /* 48000 */ 0x00b9d648, /* 88200 */ 0x00ca4588, /* 96000 */ 0x0173ac90, /* 176400 */
        0x01948b10, /* 192000 */
    },
};

#elif defined(AIAO_PLL_750MHZ)
static hi_u32 g_mclk_pll_tab[5][13] = {
    /* crg: mclk0*pow(2, CRG_POW)/ ARM_BPLL_FREQ */
    /* 128*FS mclk */
    {   0x0002cc3d, /* 8000 */ 0x0003db11, /* 11025 */ 0x0004325c, /* 12000 */ 0x0005987b, /* 16000 */
        0x0007b623, /* 22050 */ 0x000864b8, /* 24000 */ 0x000b30f6, /* 32000 */ 0x000f6c46, /* 44100 */
        0x0010c971, /* 48000 */ 0x001ed88d, /* 88200 */ 0x002192e2, /* 96000 */ 0x003db11a, /* 176400 */
        0x004325c5, /* 192000 */
    },
    /* 256*FS mclk */
    {   0x0005987b, /* 8000 */ 0x0007b623, /* 11025 */ 0x000864b8, /* 12000 */ 0x000b30f6, /* 16000 */
        0x000f6c46, /* 22050 */ 0x0010c971, /* 24000 */ 0x001661ec, /* 32000 */ 0x001ed88d, /* 44100 */
        0x002192e2, /* 48000 */ 0x003db11a, /* 88200 */ 0x004325c5, /* 96000 */ 0x007b6234, /* 176400 */
        0x00864b8a, /* 192000 */
    },
    /* 384*FS mclk */
    {   0x000864b8, /* 8000 */ 0x000b9134, /* 11025 */ 0x000c9715, /* 12000 */ 0x0010c971, /* 16000 */
        0x00172269, /* 22050 */ 0x00192e2a, /* 24000 */ 0x002192e2, /* 32000 */ 0x002e44d3, /* 44100 */
        0x00325c54, /* 48000 */ 0x005c89a7, /* 88200 */ 0x0064b8a8, /* 96000 */ 0x00b9134e, /* 176400 */
        0x00c97150, /* 192000 */
    },
    /* 512*FS mclk */
    {   0x000b30f6, /* 8000 */ 0x000f6c46, /* 11025 */ 0x0010c971, /* 12000 */ 0x001661ec, /* 16000 */
        0x001ed88d, /* 22050 */ 0x002192e2, /* 24000 */ 0x002cc3d8, /* 32000 */ 0x003db11a, /* 44100 */
        0x004325c5, /* 48000 */ 0x007b6234, /* 88200 */ 0x00864b8a, /* 96000 */ 0x00f6c468, /* 176400 */
        0x010c9714, /* 192000 */
    },
    /* 1024*FS mclk */
    {   0x001661ec, /* 8000 */ 0x001ed88d, /* 11025 */ 0x002192e2, /* 12000 */ 0x002cc3d8, /* 16000 */
        0x003db11a, /* 22050 */ 0x004325c5, /* 24000 */ 0x005987b1, /* 32000 */ 0x007b6234, /* 44100 */
        0x00864b8a, /* 48000 */ 0x00f6c468, /* 88200 */ 0x010c9714, /* 96000 */ 0x01ed88d0, /* 176400 */
        0x02192e28, /* 192000 */
    },
};

#elif defined(AIAO_PLL_307MHZ)
static int g_mclk_pll_tab[5][13] = {
    /* crg: mclk0*pow(2, CRG_POW)/ ARM_BPLL_FREQ */
    /* 128*FS mclk */
    {   0x0006d3a0, /* 8000 */ 0x00096872, /* 11025 */ 0x000a3d70, /* 12000 */ 0x000da740, /* 16000 */
        0x0012d0e5, /* 22050 */ 0x00147ae1, /* 24000 */ 0x001b4e81, /* 32000 */ 0x0025a1ca, /* 44100 */
        0x0028f5c2, /* 48000 */ 0x004b4395, /* 88200 */ 0x0051eb85, /* 96000 */ 0x0096872b, /* 176400 */
        0x00a3d70a, /* 192000 */
    },
    /* 256*FS mclk */
    {   0x000da740, /* 8000 */ 0x0012d0e5, /* 11025 */ 0x00147ae1, /* 12000 */ 0x001b4e81, /* 16000 */
        0x0025a1ca, /* 22050 */ 0x0028f5c2, /* 24000 */ 0x00369d03, /* 32000 */ 0x004b4395, /* 44100 */
        0x0051eb85, /* 48000 */ 0x0096872b, /* 88200 */ 0x00a3d70a, /* 96000 */ 0x012d0e56, /* 176400 */
        0x0147ae14, /* 192000 */
    },
    /* 384*FS mclk */
    {   0x00147ae1, /* 8000 */ 0x001c3958, /* 11025 */ 0x001eb851, /* 12000 */ 0x0028f5c2, /* 16000 */
        0x003872b0, /* 22050 */ 0x003d70a3, /* 24000 */ 0x0051eb85, /* 32000 */ 0x0070e560, /* 44100 */
        0x007ae147, /* 48000 */ 0x00e1cac1, /* 88200 */ 0x00f5c28f, /* 96000 */ 0x01c39582, /* 176400 */
        0x01eb851e, /* 192000 */
    },
    /* 512*FS mclk */
    {   0x001b4e81, /* 8000 */ 0x0025a1ca, /* 11025 */ 0x0028f5c2, /* 12000 */ 0x00369d03, /* 16000 */
        0x004b4395, /* 22050 */ 0x0051eb85, /* 24000 */ 0x006d3a07, /* 32000 */ 0x0096872b, /* 44100 */
        0x00a3d70a, /* 48000 */ 0x012d0e56, /* 88200 */ 0x0147ae14, /* 96000 */ 0x025a1cac, /* 176400 */
        0x028f5c28, /* 192000 */
    },
    /* 1024*FS mclk */
    {   0x00369d03, /* 8000 */ 0x004b4395, /* 11025 */ 0x0051eb85, /* 12000 */ 0x006d3a07, /* 16000 */
        0x0096872b, /* 22050 */ 0x00a3d70a, /* 24000 */ 0x00da740e, /* 32000 */ 0x012d0e56, /* 44100 */
        0x0147ae14, /* 48000 */ 0x025a1cac, /* 88200 */ 0x028f5c28, /* 96000 */ 0x04b43958, /* 176400 */
        0x051eb850, /* 192000 */
    },
};
#elif defined(AIAO_PLL_100MHZ)
static hi_u32 g_mclk_pll_tab[5][13] = {
    {   0x0014f8b5, /* 8000 */ 0x001ce6c0, /* 11025 */ 0x001f7510, /* 12000 */ 0x0029f16b, /* 16000 */
        0x0039cd81, /* 22050 */ 0x003eea20, /* 24000 */ 0x0053e2d6, /* 32000 */ 0x00739b02, /* 44100 */
        0x007dd441, /* 48000 */ 0x00e73605, /* 88200 */ 0x00fba882, /* 96000 */ 0x01ce6c0a, /* 176400 */
        0x01f75104, /* 192000 */
    },
    {   0x0029f16b, /* 8000 */ 0x0039cd81, /* 11025 */ 0x003eea20, /* 12000 */ 0x0053e2d6, /* 16000 */
        0x00739b02, /* 22050 */ 0x007dd441, /* 24000 */ 0x00a7c5ac, /* 32000 */ 0x00e73605, /* 44100 */
        0x00fba882, /* 48000 */ 0x01ce6c0a, /* 88200 */ 0x01f75104, /* 96000 */ 0x039cd814, /* 176400 */
        0x03eea208, /* 192000 */
    },
    {   0x003eea20, /* 8000 */ 0x0056b441, /* 11025 */ 0x005e5f31, /* 12000 */ 0x007dd441, /* 16000 */
        0x00ad6883, /* 22050 */ 0x00bcbe62, /* 24000 */ 0x00fba882, /* 32000 */ 0x015ad106, /* 44100 */
        0x01797cc4, /* 48000 */ 0x02b5a20c, /* 88200 */ 0x02f2f988, /* 96000 */ 0x056b4418, /* 176400 */
        0x05e5f310, /* 192000 */
    },
    {   0x0053e2d6, /* 8000 */ 0x00739b02, /* 11025 */ 0x007dd441, /* 12000 */ 0x00a7c5ac, /* 16000 */
        0x00e73605, /* 22050 */ 0x00fba882, /* 24000 */ 0x014f8b58, /* 32000 */ 0x01ce6c0a, /* 44100 */
        0x01f75104, /* 48000 */ 0x039cd814, /* 88200 */ 0x03eea208, /* 96000 */ 0x0739b028, /* 176400 */
        0x07dd4410, /* 192000 */
    },
    {   0x00a7c5ac, /* 8000 */ 0x00e73605, /* 11025 */ 0x00fba882, /* 12000 */ 0x014f8b58, /* 16000 */
        0x01ce6c0a, /* 22050 */ 0x01f75104, /* 24000 */ 0x029f16b0, /* 32000 */ 0x039cd814, /* 44100 */
        0x03eea208, /* 48000 */ 0x0739b028, /* 88200 */ 0x07dd4410, /* 96000 */ 0x0e736050, /* 176400 */
        0x0fba8820, /* 192000 */
    },
};
#elif defined(AIAO_PLL_860MHZ)
static hi_u32 g_mclk_pll_tab[5][13] = {
    /* crg: mclk * FS * pow(2, CRG_POW) / ARM_BPLL_FREQ */
    /* 128*FS mclk */
    {   0x00027027, /* 8000 */ 0x00035c28, /* 11025 */ 0x0003a83a, /* 12000 */ 0x0004e04e, /* 16000 */
        0x0006b851, /* 22050 */ 0x00075075, /* 24000 */ 0x0009c09c, /* 32000 */ 0x000d70a3, /* 44100 */
        0x000ea0ea, /* 48000 */ 0x001ae147, /* 88200 */ 0x001d41d4, /* 96000 */ 0x0035c28f, /* 176400 */
        0x003a83a8, /* 192000 */
    },
    /* 256*FS mclk */
    {   0x0004e04e, /* 8000 */ 0x0006b851, /* 11025 */ 0x00075075, /* 12000 */ 0x0009c09c, /* 16000 */
        0x000d70a3, /* 22050 */ 0x000ea0ea, /* 24000 */ 0x00138138, /* 32000 */ 0x001ae147, /* 44100 */
        0x001d41d4, /* 48000 */ 0x0035c28f, /* 88200 */ 0x003a83a8, /* 96000 */ 0x006b851e, /* 176400 */
        0x00750750, /* 192000 */
    },
    /* 384*FS mclk */
    {   0x00075075, /* 8000 */ 0x000a147a, /* 11025 */ 0x000af8af, /* 12000 */ 0x000ea0ea, /* 16000 */
        0x001428f5, /* 22050 */ 0x0015f15f, /* 24000 */ 0x001d41d4, /* 32000 */ 0x002851eb, /* 44100 */
        0x002be2be, /* 48000 */ 0x0050a3d7, /* 88200 */ 0x0057c57c, /* 96000 */ 0x00a147ae, /* 176400 */
        0x00af8af9, /* 192000 */
    },
    /* 512*FS mclk */
    {   0x0009c09c, /* 8000 */ 0x000d70a3, /* 11025 */ 0x000ea0ea, /* 12000 */ 0x00138138, /* 16000 */
        0x001ae147, /* 22050 */ 0x001d41d4, /* 24000 */ 0x00270270, /* 32000 */ 0x0035c28f, /* 44100 */
        0x003a83a8, /* 48000 */ 0x006b851e, /* 88200 */ 0x00750750, /* 96000 */ 0x00d70a3d, /* 176400 */
        0x00ea0ea1, /* 192000 */
    },
    /* 1024*FS mclk */
    {   0x00138138, /* 8000 */ 0x001ae147, /* 11025 */ 0x001d41d4, /* 12000 */ 0x00270270, /* 16000 */
        0x0035c28f, /* 22050 */ 0x003a83a8, /* 24000 */ 0x004e04e0, /* 32000 */ 0x006b851e, /* 44100 */
        0x00750750, /* 48000 */ 0x00d70a3d, /* 88200 */ 0x00ea0ea1, /* 96000 */ 0x01ae147a, /* 176400 */
        0x01d41d42, /* 192000 */
    },
};

#elif defined(AIAO_PLL_297MHZ)
static hi_u32 g_mclk_pll_tab[5][13] = {
    /* crg: mclk0*pow(2, CRG_POW)/ ARM_BPLL_FREQ */
    /* 128*FS mclk */
    {   0x00070fa5, /* 8000 */ 0x0009bb29, /* 11025 */ 0x000a9778, /* 12000 */ 0x000e1f4a, /* 16000 */
        0x00137653, /* 22050 */ 0x00152ef0, /* 24000 */ 0x001c3e95, /* 32000 */ 0x0026eca6, /* 44100 */
        0x002a5de0, /* 48000 */ 0x004dd94c, /* 88200 */ 0x0054bbc1, /* 96000 */ 0x009bb299, /* 176400 */
        0x00a97782, /* 192000 */
    },
    /* 256*FS mclk */
    {   0x000e1f4a, /* 8000 */ 0x00137653, /* 11025 */ 0x00152ef0, /* 12000 */ 0x001c3e95, /* 16000 */
        0x0026eca6, /* 22050 */ 0x002a5de0, /* 24000 */ 0x00387d2b, /* 32000 */ 0x004dd94c, /* 44100 */
        0x0054bbc1, /* 48000 */ 0x009bb299, /* 88200 */ 0x00a97782, /* 96000 */ 0x01376532, /* 176400 */
        0x0152ef04, /* 192000 */
    },
    /* 384*FS mclk */
    {   0x00152ef0, /* 8000 */ 0x001d317c, /* 11025 */ 0x001fc668, /* 12000 */ 0x002a5de0, /* 16000 */
        0x003a62f9, /* 22050 */ 0x003f8cd0, /* 24000 */ 0x0054bbc1, /* 32000 */ 0x0074c5f3, /* 44100 */
        0x007f19a1, /* 48000 */ 0x00e98be6, /* 88200 */ 0x00fe3343, /* 96000 */ 0x01d317cc, /* 176400 */
        0x01fc6686, /* 192000 */
    },
    /* 512*FS mclk */
    {   0x001c3e95, /* 8000 */ 0x0026eca6, /* 11025 */ 0x002a5de0, /* 12000 */ 0x00387d2b, /* 16000 */
        0x004dd94c, /* 22050 */ 0x0054bbc1, /* 24000 */ 0x0070fa56, /* 32000 */ 0x009bb299, /* 44100 */
        0x00a97782, /* 48000 */ 0x01376532, /* 88200 */ 0x0152ef04, /* 96000 */ 0x026eca64, /* 176400 */
        0x02a5de08, /* 192000 */
    },
    /* 1024*FS mclk */
    {   0x00387d2b, /* 8000 */ 0x004dd94c, /* 11025 */ 0x0054bbc1, /* 12000 */ 0x0070fa56, /* 16000 */
        0x009bb299, /* 22050 */ 0x00a97782, /* 24000 */ 0x00e1f4ad, /* 32000 */ 0x01376532, /* 44100 */
        0x0152ef04, /* 48000 */ 0x026eca64, /* 88200 */ 0x02a5de08, /* 96000 */ 0x04dd94c8, /* 176400 */
        0x054bbc10, /* 192000 */
    },
};

#elif defined(AIAO_PLL_663MHZ)
static hi_u32 g_mclk_pll_tab[5][13] = {
    /* crg: mclk * FS * pow(2, CRG_POW) / ARM_BPLL_FREQ */
    /* 128*FS mclk */
    {   0x00032916, /* 8000 */ 0x00045b05, /* 11025 */ 0x0004bda1, /* 12000 */ 0x0006522c, /* 16000 */
        0x0008b60b, /* 22050 */ 0x00097b42, /* 24000 */ 0x000ca458, /* 32000 */ 0x00116c16, /* 44100 */
        0x0012f684, /* 48000 */ 0x0022d82d, /* 88200 */ 0x0025ed09, /* 96000 */ 0x0045b05b, /* 176400 */
        0x004bda13, /* 192000 */
    },
    /* 256*FS mclk */
    {   0x0006522c, /* 8000 */ 0x0008b60b, /* 11025 */ 0x00097b42, /* 12000 */ 0x000ca458, /* 16000 */
        0x00116c16, /* 22050 */ 0x0012f684, /* 24000 */ 0x001948b1, /* 32000 */ 0x0022d82d, /* 44100 */
        0x0025ed09, /* 48000 */ 0x0045b05b, /* 88200 */ 0x004bda13, /* 96000 */ 0x008b60b6, /* 176400 */
        0x0097b426, /* 192000 */
    },
    /* 384*FS mclk */
    {   0x00097b42, /* 8000 */ 0x000d1111, /* 11025 */ 0x000e38e3, /* 12000 */ 0x0012f684, /* 16000 */
        0x001a2222, /* 22050 */ 0x001c71c7, /* 24000 */ 0x0025ed09, /* 32000 */ 0x00344444, /* 44100 */
        0x0038e38e, /* 48000 */ 0x00688888, /* 88200 */ 0x0071c71c, /* 96000 */ 0x00d11111, /* 176400 */
        0x00e38e39, /* 192000 */
    },
    /* 512*FS mclk */
    {   0x000ca458, /* 8000 */ 0x00116c16, /* 11025 */ 0x0012f684, /* 12000 */ 0x001948b1, /* 16000 */
        0x0022d82d, /* 22050 */ 0x0025ed09, /* 24000 */ 0x00329162, /* 32000 */ 0x0045b05b, /* 44100 */
        0x004bda13, /* 48000 */ 0x008b60b6, /* 88200 */ 0x0097b426, /* 96000 */ 0x0116c16c, /* 176400 */
        0x012f684c, /* 192000 */
    },
    /* 1024*FS mclk */
    {   0x001948b1, /* 8000 */ 0x0022d82d, /* 11025 */ 0x0025ed09, /* 12000 */ 0x00329162, /* 16000 */
        0x0045b05b, /* 22050 */ 0x004bda13, /* 24000 */ 0x006522c4, /* 32000 */ 0x008b60b6, /* 44100 */
        0x0097b426, /* 48000 */ 0x0116c16c, /* 88200 */ 0x012f684c, /* 96000 */ 0x022d82d8, /* 176400 */
        0x025ed098, /* 192000 */
    },
};

#elif defined(AIAO_PLL_933MHZ)
static hi_u32 g_mclk_pll_tab[5][13] = {
    /* crg: mclk * FS * pow(2, CRG_POW) / ARM_BPLL_FREQ */
    /* 128*FS mclk */
    {   0x00023ee0, /* 8000 */ 0x00031840, /* 11025 */ 0x00035e50, /* 12000 */ 0x00047dc1, /* 16000 */
        0x00063081, /* 22050 */ 0x0006bca1, /* 24000 */ 0x0008fb82, /* 32000 */ 0x000c6102, /* 44100 */
        0x000d7943, /* 48000 */ 0x0018c205, /* 88200 */ 0x001af286, /* 96000 */ 0x0031840a, /* 176400 */
        0x0035e50d, /* 192000 */
    },
    /* 256*FS mclk */
    {   0x00047dc1, /* 8000 */ 0x00063081, /* 11025 */ 0x0006bca1, /* 12000 */ 0x0008fb82, /* 16000 */
        0x000c6102, /* 22050 */ 0x000d7943, /* 24000 */ 0x0011f704, /* 32000 */ 0x0018c205, /* 44100 */
        0x001af286, /* 48000 */ 0x0031840a, /* 88200 */ 0x0035e50d, /* 96000 */ 0x00630815, /* 176400 */
        0x006bca1b, /* 192000 */
    },
    /* 384*FS mclk */
    {   0x0006bca1, /* 8000 */ 0x000948c2, /* 11025 */ 0x000a1af2, /* 12000 */ 0x000d7943, /* 16000 */
        0x00129184, /* 22050 */ 0x001435e5, /* 24000 */ 0x001af286, /* 32000 */ 0x00252308, /* 44100 */
        0x00286bca, /* 48000 */ 0x004a4610, /* 88200 */ 0x0050d794, /* 96000 */ 0x00948c20, /* 176400 */
        0x00a1af28, /* 192000 */
    },
    /* 512*FS mclk */
    {   0x0008fb82, /* 8000 */ 0x000c6102, /* 11025 */ 0x000d7943, /* 12000 */ 0x0011f704, /* 16000 */
        0x0018c205, /* 22050 */ 0x001af286, /* 24000 */ 0x0023ee09, /* 32000 */0x0031840a, /* 44100 */
        0x0035e50d, /* 48000 */ 0x00630815, /* 88200 */ 0x006bca1b, /* 96000 */ 0x00c6102b, /* 176400 */
        0x00d79436, /* 192000 */
    },
    /* 1024*FS mclk */
    {   0x0011f704, /* 8000 */ 0x0018c205, /* 11025 */ 0x001af286, /* 12000 */ 0x0023ee09, /* 16000 */
        0x0031840a, /* 22050 */ 0x0035e50d, /* 24000 */ 0x0047dc12, /* 32000 */ 0x00630815, /* 44100 */
        0x006bca1b, /* 48000 */ 0x00c6102b, /* 88200 */ 0x00d79436, /* 96000 */ 0x018c2056, /* 176400 */
        0x01af286c, /* 192000 */
    },
};

#elif defined(AIAO_PLL_909MHZ)
static hi_u32 g_mclk_pll_tab[5][13] = {
    /* the crg: mclk * FS * pow(2, CRG_POW) / ARM_BPLL_FREQ */
    /* 128*FS mclk */
    {   0x00024e6a, /* 8000 */ 0x00032daa, /* 11025 */ 0x0003759f, /* 12000 */ 0x00049cd4, /* 16000 */
        0x00065b54, /* 22050 */ 0x0006eb3e, /* 24000 */ 0x000939a8, /* 32000 */ 0x000cb6a8, /* 44100 */
        0x000dd67c, /* 48000 */ 0x00196d51, /* 88200 */ 0x001bacf9, /* 96000 */ 0x0032daa3, /* 176400 */
        0x003759f2, /* 192000 */
    },
    /* 256*FS mclk */
    {   0x00049cd4, /* 8000 */ 0x00065b54, /* 11025 */ 0x0006eb3e, /* 12000 */ 0x000939a8, /* 16000 */
        0x000cb6a8, /* 22050 */ 0x000dd67c, /* 24000 */ 0x00127350, /* 32000 */ 0x00196d51, /* 44100 */
        0x001bacf9, /* 48000 */ 0x0032daa3, /* 88200 */ 0x003759f2, /* 96000 */ 0x0065b546, /* 176400 */
        0x006eb3e4, /* 192000 */
    },
    /* 384*FS mclk */
    {   0x0006eb3e, /* 8000 */ 0x000988fe, /* 11025 */ 0x000a60dd, /* 12000 */ 0x000dd67c, /* 16000 */
        0x001311fd, /* 22050 */ 0x0014c1ba, /* 24000 */ 0x001bacf9, /* 32000 */ 0x002623fa, /* 44100 */
        0x00298375, /* 48000 */ 0x004c47f5, /* 88200 */ 0x005306eb, /* 96000 */ 0x00988fea, /* 176400 */
        0x00a60dd6, /* 192000 */
    },
    /* 512*FS mclk */
    {   0x000939a8, /* 8000 */ 0x000cb6a8, /* 11025 */ 0x000dd67c, /* 12000 */ 0x00127350, /* 16000 */
        0x00196d51, /* 22050 */ 0x001bacf9, /* 24000 */ 0x0024e6a1, /* 32000 */ 0x0032daa3, /* 44100 */
        0x003759f2, /* 48000 */ 0x0065b546, /* 88200 */ 0x006eb3e4, /* 96000 */ 0x00cb6a8d, /* 176400 */
        0x00dd67c9, /* 192000 */
    },
    /* 1024*FS mclk */
    {   0x00127350, /* 8000 */ 0x00196d51, /* 11025 */ 0x001bacf9, /* 12000 */ 0x0024e6a1, /* 16000 */
        0x0032daa3, /* 22050 */ 0x003759f2, /* 24000 */ 0x0049cd43, /* 32000 */ 0x0065b546, /* 44100 */
        0x006eb3e4, /* 48000 */ 0x00cb6a8d, /* 88200 */ 0x00dd67c9, /* 96000 */ 0x0196d51a, /* 176400 */
        0x01bacf92, /* 192000 */
    },
};

#elif defined(AIAO_PLL_1081MHZ)
static hi_u32 g_mclk_pll_tab[5][13] = {
    /* crg: mclk * FS * pow(2, CRG_POW) / ARM_BPLL_FREQ */
    /* 128*FS mclk */
    {   0x0001f07c, /* 8000 */ 0x0002ac37, /* 11025 */ 0x0002e8ba, /* 12000 */ 0x0003e0f8, /* 16000 */
        0x0005586f, /* 22050 */ 0x0005d174, /* 24000 */ 0x0007c1f0, /* 32000 */ 0x000ab0df, /* 44100 */
        0x000ba2e8, /* 48000 */ 0x001561be, /* 88200 */ 0x001745d1, /* 96000 */ 0x002ac37d, /* 176400 */
        0x002e8ba3, /* 192000 */
    },
    /* 256*FS mclk */
    {   0x0003e0f8, /* 8000 */ 0x0005586f, /* 11025 */ 0x0005d174, /* 12000 */ 0x0007c1f0, /* 16000 */
        0x000ab0df, /* 22050 */ 0x000ba2e8, /* 24000 */ 0x000f83e1, /* 32000 */ 0x001561be, /* 44100 */
        0x001745d1, /* 48000 */ 0x002ac37d, /* 88200 */ 0x002e8ba3, /* 96000 */ 0x005586fb, /* 176400 */
        0x005d1746, /* 192000 */
    },
    /* 384*FS mclk */
    {   0x0005d174, /* 8000 */ 0x000804a7, /* 11025 */ 0x0008ba2e, /* 12000 */ 0x000ba2e8, /* 16000 */
        0x0010094f, /* 22050 */ 0x0011745d, /* 24000 */ 0x001745d1, /* 32000 */ 0x0020129e, /* 44100 */
        0x0022e8ba, /* 48000 */ 0x0040253c, /* 88200 */ 0x0045d174, /* 96000 */ 0x00804a79, /* 176400 */
        0x008ba2e9, /* 192000 */
    },
    /* 512*FS mclk */
    {   0x0007c1f0, /* 8000 */ 0x000ab0df, /* 11025 */ 0x000ba2e8, /* 12000 */ 0x000f83e1, /* 16000 */
        0x001561be, /* 22050 */ 0x001745d1, /* 24000 */ 0x001f07c2, /* 32000 */ 0x002ac37d, /* 44100 */
        0x002e8ba3, /* 48000 */ 0x005586fb, /* 88200 */ 0x005d1746, /* 96000 */ 0x00ab0df7, /* 176400 */
        0x00ba2e8c, /* 192000 */
    },
    /* 1024*FS mclk */
    {   0x000f83e1, /* 8000 */ 0x001561be, /* 11025 */ 0x001745d1, /* 12000 */ 0x001f07c2, /* 16000 */
        0x002ac37d, /* 22050 */ 0x002e8ba3, /* 24000 */ 0x003e0f84, /* 32000 */ 0x005586fb, /* 44100 */
        0x005d1746, /* 48000 */ 0x00ab0df7, /* 88200 */ 0x00ba2e8c, /* 96000 */ 0x01561bee, /* 176400 */
        0x01745d18, /* 192000 */
    },
};

#elif defined(AIAO_PLL_798MHZ)
static hi_u32 g_mclk_pll_tab[5][13] = {
    /* crg: mclk * FS * pow(2, CRG_POW) / ARM_BPLL_FREQ */
    /* 128*FS mclk */
    {   0x0002a02a, /* 8000 */ 0x00039e53, /* 11025 */ 0x0003f03f, /* 12000 */ 0x00054054, /* 16000 */
        0x00073ca7, /* 22050 */ 0x0007e07e, /* 24000 */ 0x000a80a8, /* 32000 */ 0x000e794e, /* 44100 */
        0x000fc0fc, /* 48000 */ 0x001cf29c, /* 88200 */ 0x001f81f8, /* 96000 */ 0x0039e538, /* 176400 */
        0x003f03f0, /* 192000 */
    },
    /* 256*FS mclk */
    {   0x00054054, /* 8000 */ 0x00073ca7, /* 11025 */ 0x0007e07e, /* 12000 */ 0x000a80a8, /* 16000 */
        0x000e794e, /* 22050 */ 0x000fc0fc, /* 24000 */ 0x00150150, /* 32000 */ 0x001cf29c, /* 44100 */
        0x001f81f8, /* 48000 */ 0x0039e538, /* 88200 */ 0x003f03f0, /* 96000 */ 0x0073ca70, /* 176400 */
        0x007e07e0, /* 192000 */
    },
    /* 384*FS mclk */
    {   0x0007e07e, /* 8000 */ 0x000adafa, /* 11025 */ 0x000bd0bd, /* 12000 */ 0x000fc0fc, /* 16000 */
        0x0015b5f5, /* 22050 */ 0x0017a17a, /* 24000 */ 0x001f81f8, /* 32000 */ 0x002b6bea, /* 44100 */
        0x002f42f4, /* 48000 */ 0x0056d7d4, /* 88200 */ 0x005e85e8, /* 96000 */ 0x00adafa8, /* 176400 */
        0x00bd0bd1, /* 192000 */
    },
    /* 512*FS mclk */
    {   0x000a80a8, /* 8000 */ 0x000e794e, /* 11025 */ 0x000fc0fc, /* 12000 */ 0x00150150, /* 16000 */
        0x001cf29c, /* 22050 */ 0x001f81f8, /* 24000 */ 0x002a02a0, /* 32000 */ 0x0039e538, /* 44100 */
        0x003f03f0, /* 48000 */ 0x0073ca70, /* 88200 */ 0x007e07e0, /* 96000 */ 0x00e794e0, /* 176400 */
        0x00fc0fc1, /* 192000 */
    },
    /* 1024*FS mclk */
    {   0x00150150, /* 8000 */ 0x001cf29c, /* 11025 */ 0x001f81f8, /* 12000 */ 0x002a02a0, /* 16000 */
        0x0039e538, /* 22050 */ 0x003f03f0, /* 24000 */ 0x00540540, /* 32000 */ 0x0073ca70, /* 44100 */
        0x007e07e0, /* 48000 */ 0x00e794e0, /* 88200 */ 0x00fc0fc1, /* 96000 */ 0x01cf29c0, /* 176400 */
        0x01f81f82, /* 192000 */
    },
};
#elif defined(AIAO_PLL_12MHZ)
static hi_u32 g_mclk_pll_tab[5][13] = {
    /* crg: mclk * FS * pow(2, CRG_POW) / ARM_BPLL_FREQ */
    /* 128*FS mclk */
    {
        0x00aaaaab, /* 8000 */ 0x00eb3333, /* 11025 */ 0x01000000, /* 12000 */ 0x01555556, /* 16000 */
        0x01d66666, /* 22050 */ 0x02000000, /* 24000 */ 0x02aaaaac, /* 32000 */ 0x03accccc, /* 44100 */
        0x04000000, /* 48000 */ 0x07599998, /* 88200 */ 0x08000000, /* 96000 */ 0x0eb33330, /* 176400 */
        0x10000000, /* 192000 */
    },
    /* 256*FS mclk */
    {
        0x01555556, /* 8000 */ 0x01d66666, /* 11025 */ 0x02000000, /* 12000 */ 0x02aaaaac, /* 16000 */
        0x03accccc, /* 22050 */ 0x04000000, /* 24000 */ 0x05555558, /* 32000 */ 0x07599998, /* 44100 */
        0x08000000, /* 48000 */ 0x0eb33330, /* 88200 */ 0x10000000, /* 96000 */ 0x1d666660, /* 176400 */
        0x20000000, /* 192000 */
    },
    /* 384*FS mclk */
    {
        0x02000000, /* 8000 */ 0x02c19998, /* 11025 */ 0x03000000, /* 12000 */ 0x04000000, /* 16000 */
        0x05833330, /* 22050 */ 0x06000000, /* 24000 */ 0x08000000, /* 32000 */ 0x0b066660, /* 44100 */
        0x0c000000, /* 48000 */ 0x160cccc0, /* 88200 */ 0x18000000, /* 96000 */ 0x2c199980, /* 176400 */
        0x30000000, /* 192000 */ },
    /* 512*FS mclk */
    {
        0x02aaaaac, /* 8000 */ 0x03accccc, /* 11025 */ 0x04000000, /* 12000 */ 0x05555558, /* 16000 */
        0x07599998, /* 22050 */ 0x08000000, /* 24000 */ 0x0aaaaab0, /* 32000 */ 0x0eb33330, /* 44100 */
        0x10000000, /* 48000 */ 0x1d666660, /* 88200 */ 0x20000000, /* 96000 */ 0x3accccc0, /* 176400 */
        0x40000000, /* 192000 */ },
    /* 1024*FS mclk */
    {
        0x05555558, /* 8000 */ 0x07599998, /* 11025 */ 0x08000000, /* 12000 */ 0x0aaaaab0, /* 16000 */
        0x0eb33330, /* 22050 */ 0x10000000, /* 24000 */ 0x15555560, /* 32000 */ 0x1d666660, /* 44100 */
        0x20000000, /* 48000 */ 0x3accccc0, /* 88200 */ 0x40000000, /* 96000 */ 0x75999980, /* 176400 */
        0x80000000, /* 192000 */ },

};

#elif defined(AIAO_PLL_786MHZ)
static hi_u32 g_mclk_pll_tab[AIAO_FS_TAB_IDX_NUM][AIAO_FS_SUB_IDX_NUM] = {
    /* 128*FS mclk */
    {
        0x0002aaaa, /* 08000 */ 0x0003accc, /* 11025 */ 0x00040000, /* 12000 */ 0x00055555, /* 16000 */
        0x00075999, /* 22050 */ 0x00080000, /* 24000 */ 0x000aaaaa, /* 32000 */ 0x000eb333, /* 44100 */
        0x00100000, /* 48000 */ 0x001d6666, /* 88200 */ 0x00200000, /* 96000 */ 0x003acccc, /* 176400 */
        0x00400000, /* 192000 */
    },

    /* 256*FS mclk */
    {
        0x00055555, /* 08000 */ 0x00075999, /* 11025 */ 0x00080000, /* 12000 */ 0x000aaaaa, /* 16000 */
        0x000eb333, /* 22050 */ 0x00100000, /* 24000 */ 0x00155555, /* 32000 */ 0x001d6666, /* 44100 */
        0x00200000, /* 48000 */ 0x003acccc, /* 88200 */ 0x00400000, /* 96000 */ 0x00759999, /* 176400 */
        0x00800000, /* 192000 */
    },

    /* 384*FS mclk */
    {
        0x00080000, /* 08000 */ 0x000b0666, /* 11025 */ 0x000c0000, /* 12000 */ 0x00100000, /* 16000 */
        0x00160ccc, /* 22050 */ 0x00180000, /* 24000 */ 0x00200000, /* 32000 */ 0x002c1999, /* 44100 */
        0x00300000, /* 48000 */ 0x00583333, /* 88200 */ 0x00600000, /* 96000 */ 0x00b06666, /* 176400 */
        0x00c00000, /* 192000 */
    },

    /* 512*FS mclk */
    {
        0x000aaaaa, /* 08000 */ 0x000eb333, /* 11025 */ 0x00100000, /* 12000 */ 0x00155555, /* 16000 */
        0x001d6666, /* 22050 */ 0x00200000, /* 24000 */ 0x002aaaaa, /* 32000 */ 0x003acccc, /* 44100 */
        0x00400000, /* 48000 */ 0x00759999, /* 88200 */ 0x00800000, /* 96000 */ 0x00eb3333, /* 176400 */
        0x01000000, /* 192000 */
    },

    /* 1024*FS mclk */
    {
        0x00155555, /* 08000 */ 0x001d6666, /* 11025 */ 0x00200000, /* 12000 */ 0x002aaaaa, /* 16000 */
        0x003acccc, /* 22050 */ 0x00400000, /* 24000 */ 0x00555555, /* 32000 */ 0x00759999, /* 44100 */
        0x00800000, /* 48000 */ 0x00eb3333, /* 88200 */ 0x01000000, /* 96000 */ 0x01d66666, /* 176400 */
        0x02000000, /* 192000 */
    },
};
#else
#error "YOU MUST DEFINE AIAO PLL SOURCE!"
#endif

/* hi_fone_v310_b02 use AIAO_PLL_860MHZ */
static hi_u32 g_mclk_pll_tab_mv300[5][13] = {
    /* crg: mclk * FS * pow(2, CRG_POW) / ARM_BPLL_FREQ */
    /* 128*FS mclk */
    {   0x00027027, /* 8000 */ 0x00035c28, /* 11025 */ 0x0003a83a, /* 12000 */ 0x0004e04e, /* 16000 */
        0x0006b851, /* 22050 */ 0x00075075, /* 24000 */ 0x0009c09c, /* 32000 */ 0x000d70a3, /* 44100 */
        0x000ea0ea, /* 48000 */ 0x001ae147, /* 88200 */ 0x001d41d4, /* 96000 */ 0x0035c28f, /* 176400 */
        0x003a83a8, /* 192000 */
    },
    /* 256*FS mclk */
    {   0x0004e04e, /* 8000 */ 0x0006b851, /* 11025 */ 0x00075075, /* 12000 */ 0x0009c09c, /* 16000 */
        0x000d70a3, /* 22050 */ 0x000ea0ea, /* 24000 */ 0x00138138, /* 32000 */ 0x001ae147, /* 44100 */
        0x001d41d4, /* 48000 */ 0x0035c28f, /* 88200 */ 0x003a83a8, /* 96000 */ 0x006b851e, /* 176400 */
        0x00750750, /* 192000 */
    },
    /* 384*FS mclk */
    {   0x00075075, /* 8000 */ 0x000a147a, /* 11025 */ 0x000af8af, /* 12000 */ 0x000ea0ea, /* 16000 */
        0x001428f5, /* 22050 */ 0x0015f15f, /* 24000 */ 0x001d41d4, /* 32000 */ 0x002851eb, /* 44100 */
        0x002be2be, /* 48000 */ 0x0050a3d7, /* 88200 */ 0x0057c57c, /* 96000 */ 0x00a147ae, /* 176400 */
        0x00af8af9, /* 192000 */
    },
    /* 512*FS mclk */
    {   0x0009c09c, /* 8000 */ 0x000d70a3, /* 11025 */ 0x000ea0ea, /* 12000 */ 0x00138138, /* 16000 */
        0x001ae147, /* 22050 */ 0x001d41d4, /* 24000 */ 0x00270270, /* 32000 */ 0x0035c28f, /* 44100 */
        0x003a83a8, /* 48000 */ 0x006b851e, /* 88200 */ 0x00750750, /* 96000 */ 0x00d70a3d, /* 176400 */
        0x00ea0ea1, /* 192000 */
    },
    /* 1024*FS mclk */
    {   0x00138138, /* 8000 */ 0x001ae147, /* 11025 */ 0x001d41d4, /* 12000 */ 0x00270270, /* 16000 */
        0x0035c28f, /* 22050 */ 0x003a83a8, /* 24000 */ 0x004e04e0, /* 32000 */ 0x006b851e, /* 44100 */
        0x00750750, /* 48000 */ 0x00d70a3d, /* 88200 */ 0x00ea0ea1, /* 96000 */ 0x01ae147a, /* 176400 */
        0x01d41d42, /* 192000 */
    },
};

/* 98MV200 VP9 use AIAO_PLL_1081MHZ */
static hi_u32 g_mclk_pll_tab_1081[5][13] = {
    /* crg: mclk * FS * pow(2, CRG_POW) / ARM_BPLL_FREQ */
    /* 128*FS mclk */
    {   0x0001f07c, /* 8000 */ 0x0002ac37, /* 11025 */ 0x0002e8ba, /* 12000 */ 0x0003e0f8, /* 16000 */
        0x0005586f, /* 22050 */ 0x0005d174, /* 24000 */ 0x0007c1f0, /* 32000 */ 0x000ab0df, /* 44100 */
        0x000ba2e8, /* 48000 */ 0x001561be, /* 88200 */ 0x001745d1, /* 96000 */ 0x002ac37d, /* 176400 */
        0x002e8ba3, /* 192000 */
    },
    /* 256*FS mclk */
    {   0x0003e0f8, /* 8000 */ 0x0005586f, /* 11025 */ 0x0005d174, /* 12000 */ 0x0007c1f0, /* 16000 */
        0x000ab0df, /* 22050 */ 0x000ba2e8, /* 24000 */ 0x000f83e1, /* 32000 */ 0x001561be, /* 44100 */
        0x001745d1, /* 48000 */ 0x002ac37d, /* 88200 */ 0x002e8ba3, /* 96000 */ 0x005586fb, /* 176400 */
        0x005d1746, /* 192000 */
    },
    /* 384*FS mclk */
    {   0x0005d174, /* 8000 */ 0x000804a7, /* 11025 */ 0x0008ba2e, /* 12000 */ 0x000ba2e8, /* 16000 */
        0x0010094f, /* 22050 */ 0x0011745d, /* 24000 */ 0x001745d1, /* 32000 */ 0x0020129e, /* 44100 */
        0x0022e8ba, /* 48000 */ 0x0040253c, /* 88200 */ 0x0045d174, /* 96000 */ 0x00804a79, /* 176400 */
        0x008ba2e9, /* 192000 */
    },
    /* 512*FS mclk */
    {   0x0007c1f0, /* 8000 */ 0x000ab0df, /* 11025 */ 0x000ba2e8, /* 12000 */ 0x000f83e1, /* 16000 */
        0x001561be, /* 22050 */ 0x001745d1, /* 24000 */ 0x001f07c2, /* 32000 */ 0x002ac37d, /* 44100 */
        0x002e8ba3, /* 48000 */ 0x005586fb, /* 88200 */ 0x005d1746, /* 96000 */ 0x00ab0df7, /* 176400 */
        0x00ba2e8c, /* 192000 */
    },
    /* 1024*FS mclk */
    {   0x000f83e1, /* 8000 */ 0x001561be, /* 11025 */ 0x001745d1, /* 12000 */ 0x001f07c2, /* 16000 */
        0x002ac37d, /* 22050 */ 0x002e8ba3, /* 24000 */ 0x003e0f84, /* 32000 */ 0x005586fb, /* 44100 */
        0x005d1746, /* 48000 */ 0x00ab0df7, /* 88200 */ 0x00ba2e8c, /* 96000 */ 0x01561bee, /* 176400 */
        0x01745d18, /* 192000 */
    },
};

static hi_u32 get_mclk_crg(hi_u32 sample_rate, hi_u32 bclk_div, hi_u32 fclk_div)
{
    hi_u32 *mclk_tab = HI_NULL;

    hi_u32(*mclk_pll_tab)[13];

    hi_u32 mclk_div = bclk_div * fclk_div;

    if (autil_check_sys_mode() == HI_TRUE) {
        mclk_pll_tab = g_mclk_pll_tab_1081;
    } else if (autil_is_chip_mv300() == HI_TRUE) {
        mclk_pll_tab = g_mclk_pll_tab_mv300;
    } else {
        mclk_pll_tab = g_mclk_pll_tab;
    }

    switch (mclk_div) {
        case 128:
            mclk_tab = mclk_pll_tab[AIAO_0128FS_TAB_IDX];
            break;
        case 256:
            mclk_tab = mclk_pll_tab[AIAO_0256FS_TAB_IDX];
            break;
        case 384:
            mclk_tab = mclk_pll_tab[AIAO_0384FS_TAB_IDX];
            break;
        case 512:
            mclk_tab = mclk_pll_tab[AIAO_0512FS_TAB_IDX];
            break;
        case 1024:
            mclk_tab = mclk_pll_tab[AIAO_1024FS_TAB_IDX];
            break;
        default:
            mclk_tab = mclk_pll_tab[AIAO_0256FS_TAB_IDX]; /* defaulse 256 * FS */
    }

    switch (sample_rate) {
        case 8000:
            return mclk_tab[AIAO_008_KHZ_SUB_IDX];
        case 11025:
            return mclk_tab[AIAO_011_KHZ_SUB_IDX];
        case 12000:
            return mclk_tab[AIAO_012_KHZ_SUB_IDX];
        case 16000:
            return mclk_tab[AIAO_016_KHZ_SUB_IDX];
        case 22050:
            return mclk_tab[AIAO_022_KHZ_SUB_IDX];
        case 24000:
            return mclk_tab[AIAO_024_KHZ_SUB_IDX];
        case 32000:
            return mclk_tab[AIAO_032_KHZ_SUB_IDX];
        case 44100:
            return mclk_tab[AIAO_044_KHZ_SUB_IDX];
        case 48000:
            return mclk_tab[AIAO_048_KHZ_SUB_IDX];
        case 88200:
            return mclk_tab[AIAO_088_KHZ_SUB_IDX];
        case 96000:
            return mclk_tab[AIAO_096_KHZ_SUB_IDX];
        case 176400:
            return mclk_tab[AIAO_176_KHZ_SUB_IDX];
        case 192000:
            return mclk_tab[AIAO_192_KHZ_SUB_IDX];
        default:
            return mclk_tab[AIAO_048_KHZ_SUB_IDX];
    }
}

static hi_u32 get_fslk_div(hi_u32 fs_div)
{
    hi_u32 n;

    for (n = 0; n < sizeof(g_fs_div_tab) / sizeof(g_fs_div_tab[0]); n++) {
        if (((hi_u32)g_fs_div_tab[n]) == fs_div) {
            break;
        }
    }

    return n;
}

static hi_u32 get_bclk_div(hi_u32 xclk_div)
{
    hi_u32 n;

    for (n = 0; n < 16; n++) {
        if (((hi_u32)g_bclk_div_tab[n]) == xclk_div) {
            break;
        }
    }

    return n;
}

#define AIAO_REG_SIZE 0x10000

static hi_void io_address_map(hi_void)
{
    hi_void *reg_aiao_vir_addr = HI_NULL;
    hi_s32 ch;

    g_reg_aiao_crg = hi_drv_sys_get_crg_reg_ptr();
    if (g_reg_aiao_crg == HI_NULL) {
        HI_ERR_AIAO("call hi_drv_sys_get_crg_reg_ptr failed\n");
        return;
    }

    reg_aiao_vir_addr = osal_ioremap_nocache(AIAO_CBB_REGBASE, AIAO_REG_SIZE);
    if (reg_aiao_vir_addr == HI_NULL) {
        HI_ERR_AIAO("osal_ioremap_nocache AIAO_CBB_REGBASE failed\n");
        return;
    }

    g_aiao_com_reg = (S_AIAO_COM_REGS_TYPE *)reg_aiao_vir_addr;

    for (ch = 0; ch < AIAO_MAX_RX_PORT_NUMBER; ch++) {
        g_aiao_rx_reg[ch] = (S_AIAO_RX_REGS_TYPE *)((reg_aiao_vir_addr + AIAO_RX_OFFSET) + AIAO_RX_REG_BANDSIZE * ch);
    }

    for (ch = 0; ch < AIAO_MAX_TX_PORT_NUMBER; ch++) {
        g_aiao_tx_reg[ch] = (S_AIAO_TX_REGS_TYPE *)((reg_aiao_vir_addr + AIAO_TX_OFFSET) + AIAO_TX_REG_BANDSIZE * ch);
    }

    for (ch = 0; ch < AIAO_MAX_TXSPDIF_PORT_NUMBER; ch++) {
        g_aiao_tx_spdif_reg[ch] =
            (S_AIAO_TXSPDIF_REGS_TYPE *)((reg_aiao_vir_addr + AIAO_TXSDPIF_OFFSET) + AIAO_TXSPDIF_REG_BANDSIZE * ch);
    }

    for (ch = 0; ch < AIAO_MAX_TXSPDIF_PORT_NUMBER; ch++) {
        g_aiao_spdifer_reg[ch] =
            (S_AIAO_SPDIFER_REGS_TYPE *)((reg_aiao_vir_addr + AIAO_SDPIFER_OFFSET) + AIAO_SPDIFER_REG_BANDSIZE * ch);
    }
}

static hi_void io_address_unmap(hi_void)
{
    hi_s32 ch;

    if (g_aiao_com_reg != HI_NULL) {
        osal_iounmap((hi_void *)g_aiao_com_reg);
        g_aiao_com_reg = HI_NULL;
    }

    for (ch = 0; ch < AIAO_MAX_RX_PORT_NUMBER; ch++) {
        g_aiao_rx_reg[ch] = HI_NULL;
    }

    for (ch = 0; ch < AIAO_MAX_TX_PORT_NUMBER; ch++) {
        g_aiao_tx_reg[ch] = HI_NULL;
    }

    for (ch = 0; ch < AIAO_MAX_TXSPDIF_PORT_NUMBER; ch++) {
        g_aiao_tx_spdif_reg[ch] = HI_NULL;
    }

    for (ch = 0; ch < AIAO_MAX_TXSPDIF_PORT_NUMBER; ch++) {
        g_aiao_spdifer_reg[ch] = HI_NULL;
    }
}

#ifdef HI_FPGA_SUPPORT
hi_s32 aiao_hw_power_on(hi_void)
{
    hi_u32 timeout = 0;

    /* reset aiao crg */
    g_reg_aiao_crg->PERI_CRG456.bits.aiao_srst_req = 1;

    while (timeout < AIAO_CLK_TIMEOUT) {
        if (g_aiao_com_reg->AIAO_STATUS.bits.srst_rdy == 0) {
            break;
        }
        timeout++;
    }

    if (timeout == AIAO_CLK_TIMEOUT) {
        HI_FATAL_AIAO("aiao_srst_req reset failed\n");
        return HI_FAILURE;
    }

    mb();

    /* enable aiao working clock */
    g_reg_aiao_crg->PERI_CRG456.bits.aiao_cken = 1;
    osal_udelay(1);

    /* enable aiao crg */
    g_reg_aiao_crg->PERI_CRG456.bits.aiao_srst_req = 0;

    timeout = 0;
    while (timeout < AIAO_CLK_TIMEOUT) {
        if (g_aiao_com_reg->AIAO_STATUS.bits.srst_rdy == 1) {
            break;
        }
        timeout++;
    }

    if (timeout == AIAO_CLK_TIMEOUT) {
        HI_FATAL_AIAO("aiao_srst_req enable failed\n");
        return HI_FAILURE;
    }

    mb();

    /* check aiao ready */
    if (!g_aiao_com_reg->HW_CAPABILITY) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void aiao_hw_power_off(hi_void)
{
    hi_u32 timeout = 0;

    g_reg_aiao_crg->PERI_CRG456.bits.aiao_srst_req = 1; /* reset aiao crg */

    while (timeout < AIAO_CLK_TIMEOUT) {
        if (g_aiao_com_reg->AIAO_STATUS.bits.srst_rdy == 0) {
            break;
        }
        timeout++;
    }

    if (timeout == AIAO_CLK_TIMEOUT) {
        HI_FATAL_AO("aiao_srst_req failed!\n");
        return;
    }

    mb();

    g_reg_aiao_crg->PERI_CRG456.bits.aiao_cken = 0; /* disable aiao working clock */
    osal_udelay(1);
}
#else
hi_s32 aiao_hw_power_on(hi_void)
{
    hi_u32 timeout = 0;
    U_PERI_CRG456 u_tmp_val;

    u_tmp_val.u32 = g_reg_aiao_crg->PERI_CRG456.u32;

    if (u_tmp_val.bits.aiao_cken == 1) {
        HI_LOG_WARN("aiao is already powered on!\n");
        return HI_SUCCESS;
    }

    u_tmp_val.bits.aiao_srst_req = 1; /* reset */
    g_reg_aiao_crg->PERI_CRG456.u32 = u_tmp_val.u32;

    while (timeout < 10) {
        if (g_aiao_com_reg->AIAO_STATUS.bits.srst_rdy == 0) {
            break;
        }
        timeout++;
    }

    if (timeout == 10) {
        HI_FATAL_AO("aiao_srst_req failed!\n");
        return HI_FAILURE;
    }

    mb();

    u_tmp_val.u32 = g_reg_aiao_crg->PERI_CRG456.u32;
    u_tmp_val.bits.aiao_cken = 1; /* enable aiao working clock */
    g_reg_aiao_crg->PERI_CRG456.u32 = u_tmp_val.u32;
    osal_udelay(1);

    u_tmp_val.u32 = g_reg_aiao_crg->PERI_CRG456.u32;
    u_tmp_val.bits.aiao_srst_req = 0; /* enable aiao */
    g_reg_aiao_crg->PERI_CRG456.u32 = u_tmp_val.u32;

    while (timeout < 10) {
        if (g_aiao_com_reg->AIAO_STATUS.bits.srst_rdy == 1) {
            break;
        }
        timeout++;
    }

    if (timeout == 10) {
        HI_FATAL_AO("aiao_srst_req failed!\n");
        return HI_FAILURE;
    }

    mb();
    if (!g_aiao_com_reg->HW_CAPABILITY) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void aiao_hw_power_off(hi_void)
{
    hi_u32 timeout = 0;
    U_PERI_CRG456 u_tmp_val;

    u_tmp_val.u32 = g_reg_aiao_crg->PERI_CRG456.u32;
    u_tmp_val.bits.aiao_srst_req = 1; /* disable aiao */
    g_reg_aiao_crg->PERI_CRG456.u32 = u_tmp_val.u32;
    while (timeout < 10) {
        if (g_aiao_com_reg->AIAO_STATUS.bits.srst_rdy == 0) {
            break;
        }
        timeout++;
    }

    if (timeout == 10) {
        HI_FATAL_AO("aiao_srst_req failed!\n");
        return;
    }

    mb();

    u_tmp_val.u32 = g_reg_aiao_crg->PERI_CRG456.u32;
    u_tmp_val.bits.aiao_cken = 0; /* disable aiao working clock */
    g_reg_aiao_crg->PERI_CRG456.u32 = u_tmp_val.u32;
    osal_udelay(1);
}
#endif

hi_void aiao_hw_set_outstanding(hi_void)
{
    g_aiao_com_reg->OUT_STANDING.bits.vhb_outst_num = 2;
}

hi_s32 aiao_hw_init(hi_void)
{
    hi_u32 ret;
    io_address_map();

    ret = aiao_hw_power_on();
    if (ret != HI_SUCCESS) {
        return ret;
    }

    aiao_hw_set_outstanding();

    return HI_SUCCESS;
}

hi_void aiao_hw_de_init(hi_void)
{
    aiao_hw_power_off();
    io_address_unmap();
}

#if defined(HI_AIAO_VERIFICATION_SUPPORT)
hi_void aiao_hw_get_hw_capability(hi_u32 *capability)
{
    *capability = g_aiao_com_reg->HW_CAPABILITY;
}

hi_void aiao_hw_dbg_rw_reg(aiao_dbg_reg *reg)
{
    volatile hi_u32 *addr = HI_NULL;

    if (reg->reg_addr_base == HI_REG_CRG_BASE_ADDR) {
        if (reg->reg_addr_off_set == AIAO_SYSCRG_REGOFFSET) {
            if (reg->is_read) {
                reg->reg_value = g_reg_aiao_crg->PERI_CRG456.u32;
            } else {
                g_reg_aiao_crg->PERI_CRG456.u32 = reg->reg_value;
            }
        } else {
            HI_ERR_AIAO(" err reg_addr_off_set(0x%x) \n", reg->reg_addr_off_set);
        }
    } else if (reg->reg_addr_base == AIAO_CBB_REGBASE) {
        addr = (hi_u32 *)(((hi_void *)g_aiao_com_reg) + reg->reg_addr_off_set);
        if (reg->is_read) {
            reg->reg_value = *addr;
        } else {
            *addr = reg->reg_value;
        }
    } else {
        HI_ERR_AIAO(" err reg_addr_base(0x%x) \n", reg->reg_addr_base);
        return;
    }
}

hi_void aiao_hw_get_hw_version(hi_u32 *version)
{
    *version = g_aiao_com_reg->HW_VERSION;
}
#endif

hi_void aiao_hw_set_top_int(hi_u32 multibit)
{
    g_aiao_com_reg->AIAO_INT_ENA.u32 = multibit;
}

hi_u32 aiao_hw_get_top_int_raw_status(hi_void)
{
    return g_aiao_com_reg->AIAO_INT_RAW.u32;
}

hi_u32 aiao_hw_get_top_int_status(hi_void)
{
    return g_aiao_com_reg->AIAO_INT_STATUS.u32;
}

hi_void aiao_hw_set_int(aiao_port_id port_id, hi_u32 multibit)
{
    hi_u32 chn_id = PORT2CHID(port_id);

    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
            g_aiao_rx_reg[chn_id]->RX_INT_ENA.u32 = multibit;
            break;
        case AIAO_MODE_TXI2S:
            g_aiao_tx_reg[chn_id]->TX_INT_ENA.u32 = multibit;
            break;
        case AIAO_MODE_TXSPDIF:
            g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_INT_ENA.u32 = multibit;
            break;
        default:
            break;
    }
}

hi_void aiao_hw_clr_int(aiao_port_id port_id, hi_u32 multibit)
{
    hi_u32 chn_id = PORT2CHID(port_id);

    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
            g_aiao_rx_reg[chn_id]->RX_INT_CLR.u32 = multibit;
            break;
        case AIAO_MODE_TXI2S:
            g_aiao_tx_reg[chn_id]->TX_INT_CLR.u32 = multibit;
            break;
        case AIAO_MODE_TXSPDIF:
            g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_INT_CLR.u32 = multibit;
            break;
        default:
            break;
    }
}

hi_u32 aiao_hw_get_int_status_raw(aiao_port_id port_id)
{
    hi_u32 chn_id = PORT2CHID(port_id);

    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
            return g_aiao_rx_reg[chn_id]->RX_INT_RAW.u32;
        case AIAO_MODE_TXI2S:
            return g_aiao_tx_reg[chn_id]->TX_INT_RAW.u32;
        case AIAO_MODE_TXSPDIF:
            return g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_INT_RAW.u32;
        default:
            break;
    }

    return 0;
}

hi_u32 aiao_hw_get_int_status(aiao_port_id port_id)
{
    hi_u32 chn_id = PORT2CHID(port_id);

    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
            return g_aiao_rx_reg[chn_id]->RX_INT_STATUS.u32;
        case AIAO_MODE_TXI2S:
            return g_aiao_tx_reg[chn_id]->TX_INT_STATUS.u32;
        case AIAO_MODE_TXSPDIF:
            return g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_INT_STATUS.u32;
        default:
            break;
    }

    return 0;
}

hi_void aiao_hw_set_buf_period_size(aiao_port_id port_id, hi_u32 period_size)
{
    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
            aiao_rxbuf_set_buf_trans_size(PORT2CHID(port_id), period_size);
            break;
        case AIAO_MODE_TXI2S:
        case AIAO_MODE_TXSPDIF:
            aiao_txbuf_set_buf_trans_size(PORT2CHID(port_id), PORT2MODE(port_id), period_size);
            break;
        default:
            break;
    }
}

hi_s32 aiao_hw_set_start(aiao_port_id port_id, hi_s32 en)
{
    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
        case AIAO_MODE_TXI2S:
            aiao_low_set_start(PORT2CHID(port_id), PORT2DIR(port_id), en);
            break;
        case AIAO_MODE_TXSPDIF:
            aiao_low_spdif_set_start(PORT2CHID(port_id), en);
            aiao_spdif_hal_set_unknow(port_id);
            aiao_spdif_hal_set_enable(port_id, en);
            break;
        default:
            break;
    }

    if (!en) {
        volatile hi_s32 loop = 0;

#if defined(HW_CHN_PTR_BUG)
        aiao_hw_set_buf_period_size(port_id, 1);
        switch (PORT2MODE(port_id)) {
            case AIAO_MODE_RXI2S:
                aiao_hw_set_buf_wptr(port_id, 0);
                break;
            case AIAO_MODE_TXI2S:
            case AIAO_MODE_TXSPDIF:
                aiao_hw_set_buf_rptr(port_id, 0);
                break;
            default:
                break;
        }
        osal_udelay(500);
#endif

        for (loop = 0; loop < 100; loop++) {
            osal_udelay(10);
            if (aiao_hw_get_stop_done_status(port_id)) {
                return HI_SUCCESS;
            }
        }

        HI_ERR_AIAO("stop port(%d) time out!\n", port_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 aiao_hw_get_stop_done_status(aiao_port_id port_id)
{
    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
        case AIAO_MODE_TXI2S:
            return aiao_low_get_stop_done_status(PORT2CHID(port_id), PORT2DIR(port_id));
        case AIAO_MODE_TXSPDIF:
            return aiao_low_spdif_get_stop_done_status(PORT2CHID(port_id));
        default:
            return 1;
    }
}

hi_void aiao_hw_set_bypass(aiao_port_id port_id, hi_s32 en)
{
    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
        case AIAO_MODE_TXI2S:
            aiao_low_set_bypass(PORT2CHID(port_id), PORT2DIR(port_id), en);
            break;

        case AIAO_MODE_TXSPDIF:
            /* aiao_check bit_depth at spdif bypass */
            aiao_low_spdif_set_bypass(PORT2CHID(port_id), en);
            break;

        default:
            break;
    }
}

hi_void aiao_hw_set_mode(aiao_port_id port_id, hi_s32 en)
{
    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
        case AIAO_MODE_TXI2S:
            break;

        case AIAO_MODE_TXSPDIF:
            aiao_spdif_hal_set_mode(port_id, en ? AIAO_SPDIF_MODE_COMPRESSED : AIAO_SPDIF_MODE_PCM);
            break;

        default:
            break;
    }
}

hi_void aiao_hw_set_volume_db(aiao_port_id port_id, hi_s32 vol_db)
{
    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
        case AIAO_MODE_TXI2S:
            aiao_low_set_volume_db(PORT2CHID(port_id), PORT2DIR(port_id), vol_db);
            break;
        case AIAO_MODE_TXSPDIF:
            aiao_low_spdif_set_volume_db(PORT2CHID(port_id), vol_db);
            break;
        default:
            break;
    }
}

hi_void aiao_hw_set_fade_out_rate(aiao_port_id port_id, aiao_fade_rate fade_rate)
{
    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
        case AIAO_MODE_TXI2S:
            aiao_low_set_fade_out_rate(PORT2CHID(port_id), PORT2DIR(port_id), fade_rate);
            break;
        case AIAO_MODE_TXSPDIF:
            aiao_low_spdif_set_fade_out_rate(PORT2CHID(port_id), fade_rate);
            break;
        default:
            break;
    }
}

hi_void aiao_hw_set_fade_in_rate(aiao_port_id port_id, aiao_fade_rate fade_rate)
{
    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
        case AIAO_MODE_TXI2S:
            aiao_low_set_fade_in_rate(PORT2CHID(port_id), PORT2DIR(port_id), fade_rate);
            break;
        case AIAO_MODE_TXSPDIF:
            aiao_low_spdif_set_fade_in_rate(PORT2CHID(port_id), fade_rate);
            break;
        default:
            break;
    }
}

hi_void aiao_hw_set_mute_fade(aiao_port_id port_id, hi_s32 en)
{
    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
        case AIAO_MODE_TXI2S:
            aiao_low_set_mute_fade(PORT2CHID(port_id), PORT2DIR(port_id), en);
            break;
        case AIAO_MODE_TXSPDIF:
            aiao_low_spdif_set_mute_fade(PORT2CHID(port_id), en);
            break;
        default:
            break;
    }
}

hi_void aiao_hw_set_mute(aiao_port_id port_id, hi_s32 en)
{
    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
        case AIAO_MODE_TXI2S:
            aiao_low_set_mute(PORT2CHID(port_id), PORT2DIR(port_id), en);
            break;
        case AIAO_MODE_TXSPDIF:
            aiao_low_spdif_set_mute(PORT2CHID(port_id), en);
            break;
        default:
            break;
    }
}

hi_void aiao_hw_set_buf(aiao_port_id port_id, aiao_buf_info *buf)
{
    hi_u32 chn_id = PORT2CHID(port_id);

    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
            aiao_rxbuf_set_buf_addr_and_size(chn_id, buf->buff_saddr, buf->buff_size);
            aiao_rxbuf_set_buf_wptr(chn_id, buf->buff_wptr);
            aiao_rxbuf_set_buf_rptr(chn_id, buf->buff_rptr);
#ifdef AIAO_RWPTR_SWBUG
            if (buf->buff_rptr == buf->buff_wptr) {
                hi_u32 buff_wptr;
                aiao_rxbuf_get_buf_wptr(chn_id, &buff_wptr);
                aiao_rxbuf_set_buf_rptr(chn_id, buff_wptr);
            }
#endif
            aiao_rxbuf_set_buf_alfull_size(PORT2CHID(port_id), buf->threshold_size);
            aiao_rxbuf_set_buf_trans_size(PORT2CHID(port_id), buf->period_buf_size);
            break;
        case AIAO_MODE_TXI2S:
            aiao_txbuf_set_buf_addr_and_size(chn_id, AIAO_MODE_TXI2S, buf->buff_saddr, buf->buff_size);
            aiao_txbuf_set_buf_wptr(chn_id, AIAO_MODE_TXI2S, buf->buff_wptr);
            aiao_txbuf_set_buf_rptr(chn_id, AIAO_MODE_TXI2S, buf->buff_rptr);
#ifdef AIAO_RWPTR_SWBUG
            if (buf->buff_rptr == buf->buff_wptr) {
                hi_u32 buff_rptr;
                aiao_txbuf_get_buf_rptr(chn_id, AIAO_MODE_TXI2S, &buff_rptr);
                aiao_txbuf_set_buf_wptr(chn_id, AIAO_MODE_TXI2S, buff_rptr);
            }
#endif
            aiao_txbuf_set_buf_alempty_size(PORT2CHID(port_id), PORT2MODE(port_id), buf->threshold_size);
            aiao_txbuf_set_buf_trans_size(PORT2CHID(port_id), PORT2MODE(port_id), buf->period_buf_size);
            break;
        case AIAO_MODE_TXSPDIF:
            aiao_txbuf_set_buf_addr_and_size(chn_id, AIAO_MODE_TXSPDIF, buf->buff_saddr, buf->buff_size);
            aiao_txbuf_set_buf_wptr(chn_id, AIAO_MODE_TXSPDIF, buf->buff_wptr);
            aiao_txbuf_set_buf_rptr(chn_id, AIAO_MODE_TXSPDIF, buf->buff_rptr);
#ifdef AIAO_RWPTR_SWBUG
            if (buf->buff_rptr == buf->buff_wptr) {
                hi_u32 buff_rptr;
                aiao_txbuf_get_buf_rptr(chn_id, AIAO_MODE_TXSPDIF, &buff_rptr);
                aiao_txbuf_set_buf_wptr(chn_id, AIAO_MODE_TXSPDIF, buff_rptr);
            }
#endif
            aiao_txbuf_set_buf_alempty_size(PORT2CHID(port_id), PORT2MODE(port_id), buf->threshold_size);
            aiao_txbuf_set_buf_trans_size(PORT2CHID(port_id), PORT2MODE(port_id), buf->period_buf_size);
            break;
        default:
            break;
    }
}

hi_void aiao_hw_set_buf_threshold_size(aiao_port_id port_id, hi_u32 threshold_size)
{
    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
            aiao_rxbuf_set_buf_alfull_size(PORT2CHID(port_id), threshold_size);
            break;
        case AIAO_MODE_TXI2S:
        case AIAO_MODE_TXSPDIF:
            aiao_txbuf_set_buf_alempty_size(PORT2CHID(port_id), PORT2MODE(port_id), threshold_size);
            break;
        default:
            break;
    }
}

hi_void aiao_hw_get_rptr_and_wptr_reg_addr(aiao_port_id port_id, hi_u32 **wptr_reg, hi_u32 **rptr_reg)
{
    hi_u32 chn_id = PORT2CHID(port_id);

    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
            aiao_rxbuf_get_buf_wptr_addr(chn_id, wptr_reg);
            aiao_rxbuf_get_buf_rptr_addr(chn_id, rptr_reg);
            break;
        case AIAO_MODE_TXI2S:
        case AIAO_MODE_TXSPDIF:
            aiao_txbuf_get_buf_wptr_addr(chn_id, PORT2MODE(port_id), wptr_reg);
            aiao_txbuf_get_buf_rptr_addr(chn_id, PORT2MODE(port_id), rptr_reg);
            break;
        default:
            break;
    }
}

hi_void aiao_hw_get_rptr_and_wptr_reg_phy_addr(aiao_port_id port_id,
    hi_u64 *pt_wptr_reg, hi_u64 *pt_rptr_reg)
{
    hi_u32 chn_id = PORT2CHID(port_id);
    hi_u64 offset;
    hi_u32 *wptr = HI_NULL;
    hi_u32 *rptr = HI_NULL;

    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
            aiao_rxbuf_get_buf_wptr_addr(chn_id, &wptr);
            aiao_rxbuf_get_buf_rptr_addr(chn_id, &rptr);
            offset = (hi_u64)((hi_u8 *)wptr - (hi_u8 *)g_aiao_com_reg);
            *pt_wptr_reg = (hi_u64)AIAO_CBB_REGBASE + offset;
            offset = (hi_u64)((hi_u8 *)rptr - (hi_u8 *)g_aiao_com_reg);
            *pt_rptr_reg = (hi_u64)AIAO_CBB_REGBASE + offset;
            break;
        case AIAO_MODE_TXI2S:
        case AIAO_MODE_TXSPDIF:
            aiao_txbuf_get_buf_wptr_addr(chn_id, PORT2MODE(port_id), &wptr);
            aiao_txbuf_get_buf_rptr_addr(chn_id, PORT2MODE(port_id), &rptr);
            offset = (hi_u64)((hi_u8 *)wptr - (hi_u8 *)g_aiao_com_reg);
            *pt_wptr_reg = (hi_u64)AIAO_CBB_REGBASE + offset;
            offset = (hi_u64)((hi_u8 *)rptr - (hi_u8 *)g_aiao_com_reg);
            *pt_rptr_reg = (hi_u64)AIAO_CBB_REGBASE + offset;
            break;
        default:
            break;
    }
}

hi_void aiao_hw_get_dbg_ws_cnt(aiao_port_id port_id, hi_u32 *ws_cnt)
{
    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
            aiao_rxbuf_get_debug_fclk_cnt(PORT2CHID(port_id), ws_cnt);
            break;
        case AIAO_MODE_TXI2S:
        case AIAO_MODE_TXSPDIF:
            aiao_txbuf_get_debug_fclk_cnt(PORT2CHID(port_id), PORT2MODE(port_id), ws_cnt);
            break;
        default:
            break;
    }
}

hi_void aiao_hw_get_dbg_bclk_cnt(aiao_port_id port_id, hi_u32 *bclk_cnt)
{
    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
            aiao_rxbuf_get_debug_bclk_cnt(PORT2CHID(port_id), bclk_cnt);
            break;
        case AIAO_MODE_TXI2S:
        case AIAO_MODE_TXSPDIF:
            aiao_txbuf_get_debug_bclk_cnt(PORT2CHID(port_id), PORT2MODE(port_id), bclk_cnt);
            break;
        default:
            break;
    }
}

hi_void aiao_hw_set_buf_addr_and_size(aiao_port_id port_id, hi_u32 start_addr, hi_u32 size)
{
    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
            aiao_rxbuf_set_buf_addr_and_size(PORT2CHID(port_id), start_addr, size);
            break;
        case AIAO_MODE_TXI2S:
        case AIAO_MODE_TXSPDIF:
            aiao_txbuf_set_buf_addr_and_size(PORT2CHID(port_id), PORT2MODE(port_id), start_addr, size);
            break;
        default:
            break;
    }
}

hi_void aiao_hw_set_buf_rptr(aiao_port_id port_id, hi_u32 rptr)
{
    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
            aiao_rxbuf_set_buf_rptr(PORT2CHID(port_id), rptr);
            break;
        case AIAO_MODE_TXI2S:
        case AIAO_MODE_TXSPDIF:
            aiao_txbuf_set_buf_rptr(PORT2CHID(port_id), PORT2MODE(port_id), rptr);
            break;
        default:
            break;
    }
}

hi_void aiao_hw_set_buf_wptr(aiao_port_id port_id, hi_u32 wptr)
{
    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
            aiao_rxbuf_set_buf_wptr(PORT2CHID(port_id), wptr);
            break;
        case AIAO_MODE_TXI2S:
        case AIAO_MODE_TXSPDIF:
            aiao_txbuf_set_buf_wptr(PORT2CHID(port_id), PORT2MODE(port_id), wptr);
            break;
        default:
            break;
    }
}

hi_void aiao_hw_get_bufu32_rptr(aiao_port_id port_id, hi_u32 *rptr)
{
    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
            aiao_rxbuf_get_buf_rptr(PORT2CHID(port_id), rptr);
            break;
        case AIAO_MODE_TXI2S:
        case AIAO_MODE_TXSPDIF:
            aiao_txbuf_get_buf_rptr(PORT2CHID(port_id), PORT2MODE(port_id), rptr);
            break;
        default:
            break;
    }
}

hi_void aiao_hw_get_bufu32_wptr(aiao_port_id port_id, hi_u32 *wptr)
{
    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
            aiao_rxbuf_get_buf_wptr(PORT2CHID(port_id), wptr);
            break;
        case AIAO_MODE_TXI2S:
        case AIAO_MODE_TXSPDIF:
            aiao_txbuf_get_buf_wptr(PORT2CHID(port_id), PORT2MODE(port_id), wptr);
            break;
        default:
            break;
    }
}

/* crg */
hi_void aiao_hw_set_master_clk_en(aiao_port_id port_id, aiao_if_attr *if_attr_master, hi_s32 en)
{
    hi_u32 chn_id = 0;
    hi_u32 crg_id = 0;

    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
            crg_id = RX_CRG2ID(if_attr_master->crg_source);
            aiao_low_set_rx_crg_clk_en(crg_id, en);
            break;
        case AIAO_MODE_TXI2S:
            crg_id = TX_CRG2ID(if_attr_master->crg_source);
            aiao_low_set_tx_crg_clk_en(crg_id, en);
            break;
        case AIAO_MODE_TXSPDIF:
            chn_id = SPDIF_TX_PORT2CHID(port_id);
            aiao_low_set_spdif_master_clk_en(chn_id, en);
            break;
        default:
            break;
    }
}

hi_void aiao_hw_set_i2s_slave_clk(aiao_port_id port_id, aiao_if_attr *if_attr_slave)
{
    hi_u32 chn_id = PORT2CHID(port_id);
    hi_u32 crg_id = 0;

    aiao_low_set_i2s_mode(chn_id, PORT2DIR(port_id), if_attr_slave->i2s_mode);

    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
            crg_id = RX_CRG2ID(if_attr_slave->crg_source);
            aiao_low_set_rx_i2s_slave_clk(crg_id, if_attr_slave);
            aiao_low_set_rx_crg_clk_en(crg_id, 1);
            break;
        case AIAO_MODE_TXI2S:
            crg_id = TX_CRG2ID(if_attr_slave->crg_source);
            aiao_low_set_tx_i2s_slave_clk(crg_id, if_attr_slave);
            aiao_low_set_tx_crg_clk_en(crg_id, 1);
            break;
        default:
            break;
    }
}

#ifdef HI_SND_HDMI_I2S_SPDIF_MUX
hi_void aiao_hw_set_op_type(aiao_port_id port_id, aiao_op_type op_type)
{
    hi_u32 chn_id = PORT2CHID(port_id);

    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
        case AIAO_MODE_TXI2S:
            g_aiao_tx_reg[chn_id]->TX_IF_ATTRI.bits.tx_spd_i2s_sel = op_type;
            break;
        case AIAO_MODE_TXSPDIF:
            g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_IF_ATTRI.bits.tx_spd_i2s_sel = op_type;
            break;
        default:
            break;
    }
}
#endif

hi_void aiao_hw_set_i2s_master_clk(aiao_port_id port_id, aiao_if_attr *if_attr_master)
{
    hi_u32 chn_id = PORT2CHID(port_id);
    hi_u32 crg_id = 0;

    aiao_low_set_i2s_mode(chn_id, PORT2DIR(port_id), if_attr_master->i2s_mode);

    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
            crg_id = RX_CRG2ID(if_attr_master->crg_source);
            aiao_low_set_rx_i2s_master_clk(crg_id, if_attr_master);
            aiao_low_set_rx_crg_clk_en(crg_id, 1);
            break;
        case AIAO_MODE_TXI2S:
            crg_id = TX_CRG2ID(if_attr_master->crg_source);
            aiao_low_set_tx_i2s_master_clk(crg_id, if_attr_master);
            aiao_low_set_tx_crg_clk_en(crg_id, 1);
            break;
        case AIAO_MODE_TXSPDIF:
            chn_id = SPDIF_TX_PORT2CHID(port_id);
            aiao_low_set_spdif_master_clk(chn_id, if_attr_master);
            aiao_spdif_hal_set_samplerate(port_id, if_attr_master->rate);
            break;
        default:
            break;
    }
}

static hi_void aiao_hw_set_i2s_dulicate_clk(aiao_port_id port_id, aiao_if_attr *if_attr_duplicate)
{
    hi_u32 chn_id = 0;
    hi_u32 crg_id = 0;

    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
            chn_id = RX_PORT2CHID(port_id);
            crg_id = RX_CRG2ID(if_attr_duplicate->crg_source);
            aiao_low_set_i2s_dulicate_clk(chn_id, PORT2DIR(port_id), if_attr_duplicate);
            aiao_low_set_rx_crg_clk_en(crg_id, 1);
            break;
        case AIAO_MODE_TXI2S:
            chn_id = TX_PORT2CHID(port_id);
            crg_id = TX_CRG2ID(if_attr_duplicate->crg_source);
            aiao_low_set_i2s_dulicate_clk(chn_id, PORT2DIR(port_id), if_attr_duplicate);
            aiao_low_set_tx_crg_clk_en(crg_id, 1);
            break;
        default:
            break;
    }
}

hi_void aiao_hw_set_if_attr(aiao_port_id port_id, aiao_if_attr *if_attr)
{
    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
        case AIAO_MODE_TXI2S:
            if (if_attr->i2s_mode == AIAO_MODE_PCM) {
                aiao_hw_set_pcm_sync_delay(port_id, if_attr->pcm_delay_cycles);
            } else {
                aiao_hw_set_i2s_data_select(port_id, AIAO_I2S_SD0, if_attr->sd0);
                aiao_hw_set_i2s_data_select(port_id, AIAO_I2S_SD1, if_attr->sd1);
                aiao_hw_set_i2s_data_select(port_id, AIAO_I2S_SD2, if_attr->sd2);
                aiao_hw_set_i2s_data_select(port_id, AIAO_I2S_SD3, if_attr->sd3);
            }
            break;
        case AIAO_MODE_TXSPDIF:
            aiao_hw_set_spdif_port_en(port_id, 1);
            break;

        default:
            break;
    }

    aiao_hw_set_multislot_mode(port_id, if_attr->multislot);
    aiao_hw_set_i2s_ch_num(port_id, if_attr->multislot, if_attr->ch_num);
    aiao_hw_set_i2s_bit_depth(port_id, if_attr->bit_depth);
    aiao_hw_set_i2s_source_select(port_id, if_attr->source);
    switch (if_attr->crg_mode) {
        case AIAO_CRG_MODE_MASTER:
            aiao_hw_set_i2s_master_clk(port_id, if_attr);
            aiao_hw_set_i2s_dulicate_clk(port_id, if_attr);
            break;
        case AIAO_CRG_MODE_SLAVE:
            aiao_hw_set_i2s_slave_clk(port_id, if_attr);
            aiao_hw_set_i2s_dulicate_clk(port_id, if_attr);
            break;
        case AIAO_CRG_MODE_DUPLICATE:
            aiao_hw_set_i2s_dulicate_clk(port_id, if_attr);
            break;
        default:
            break;
    }
}

hi_void aiao_hw_set_spdif_port_en(aiao_port_id port_id, hi_s32 en)
{
    switch (PORT2CHID(port_id)) {
        case 0:
            g_aiao_com_reg->SPDIF_TX_MUX.bits.spdif_tx_0_port_en = en;
            break;

        case 1:
            g_aiao_com_reg->SPDIF_TX_MUX.bits.spdif_tx_1_port_en = en;
            break;
        case 2:
            g_aiao_com_reg->SPDIF_TX_MUX.bits.spdif_tx_2_port_en = en;
            break;

        case 3:
            g_aiao_com_reg->SPDIF_TX_MUX.bits.spdif_tx_3_port_en = en;
            break;

        default:
            break;
    }
}

hi_void aiao_hw_set_spdif_port_select(aiao_port_id port_id, aiao_spdifport_source src_chn_id)
{
    switch (PORT2CHID(port_id)) {
        case 0:
            g_aiao_com_reg->SPDIF_TX_MUX.bits.spdif_tx_0_port_sel = src_chn_id;
            break;

        case 1:
            g_aiao_com_reg->SPDIF_TX_MUX.bits.spdif_tx_1_port_sel = src_chn_id;
            break;
        case 2:
            g_aiao_com_reg->SPDIF_TX_MUX.bits.spdif_tx_2_port_sel = src_chn_id;
            break;

        case 3:
            g_aiao_com_reg->SPDIF_TX_MUX.bits.spdif_tx_3_port_sel = src_chn_id;
            break;

        default:
            break;
    }
}

hi_void aiao_hw_set_i2s_source_select(aiao_port_id port_id, aiao_i2s_source i2s_sel)
{
    hi_u32 chn_id = PORT2CHID(port_id);

    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
        case AIAO_MODE_TXI2S:
            aiao_low_set_i2s_source_select(chn_id, PORT2DIR(port_id), i2s_sel);
            break;
        default:
            break;
    }
}

hi_void aiao_hw_set_i2s_data_select(aiao_port_id port_id, aiao_i2s_sd org_sd, aiao_i2s_sd src_sd)
{
    hi_u32 chn_id = PORT2CHID(port_id);

    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
        case AIAO_MODE_TXI2S:
            aiao_low_set_i2s_data_select(chn_id, PORT2DIR(port_id), org_sd, src_sd);
            break;
        default:
            break;
    }
}

hi_void aiao_hw_set_i2s_bit_depth(aiao_port_id port_id, aiao_bit_depth bit_depth)
{
    hi_u32 chn_id = PORT2CHID(port_id);

    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
        case AIAO_MODE_TXI2S:
            aiao_low_set_i2s_bit_depth(chn_id, PORT2DIR(port_id), bit_depth);
            break;
        case AIAO_MODE_TXSPDIF:
            aiao_low_spdif_set_bit_depth(chn_id, bit_depth);
            aiao_spdif_hal_set_bit_width(port_id, bit_depth);
            break;
        default:
            break;
    }
}

hi_void aiao_hw_set_i2s_mode(aiao_port_id port_id, aiao_i2s_mode i2s_mode)
{
    hi_u32 chn_id = PORT2CHID(port_id);

    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
        case AIAO_MODE_TXI2S:
            aiao_low_set_i2s_mode(chn_id, PORT2DIR(port_id), i2s_mode);
            break;
        default:
            break;
    }
}

hi_void aiao_hw_set_track_mode(aiao_port_id port_id, aiao_track_mode track_mode)
{
    hi_u32 chn_id = PORT2CHID(port_id);

    osal_udelay(10);
    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
        case AIAO_MODE_TXI2S:
            aiao_low_set_track_mode(chn_id, PORT2DIR(port_id), track_mode);
            break;
        case AIAO_MODE_TXSPDIF:
            aiao_low_spdif_set_track_mode(chn_id, track_mode);
            break;
        default:
            break;
    }
    osal_udelay(10);
}

hi_void aiao_hw_set_multislot_mode(aiao_port_id port_id, hi_s32 en)
{
    hi_u32 chn_id = PORT2CHID(port_id);

    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
        case AIAO_MODE_TXI2S:
            aiao_low_set_multislot_mode(chn_id, PORT2DIR(port_id), en);
            break;
        default:
            break;
    }
}

hi_void aiao_hw_set_i2s_ch_num(aiao_port_id port_id, hi_s32 is_multislot, aiao_i2s_chnum i2s_chnum)
{
    hi_u32 chn_id = PORT2CHID(port_id);

    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
        case AIAO_MODE_TXI2S:
            aiao_low_set_i2s_ch_num(chn_id, PORT2DIR(port_id), is_multislot, i2s_chnum);
            break;
        case AIAO_MODE_TXSPDIF:
            aiao_low_spdif_set_ch_num(chn_id, i2s_chnum);
            break;

        default:
            break;
    }
}

hi_void aiao_hw_set_pcm_sync_delay(aiao_port_id port_id, hi_s32 delay_cycles)
{
    hi_u32 chn_id = PORT2CHID(port_id);

    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
        case AIAO_MODE_TXI2S:
            aiao_low_set_pcm_sync_delay(chn_id, PORT2DIR(port_id), delay_cycles);
            break;
        default:
            break;
    }
}

#ifdef HI_AIAO_TIMER_SUPPORT
static hi_void aiao_low_set_timer_config(hi_u32 chn_id, hi_u32 config)
{
    g_aiao_com_reg->TIMER_CONFIG[chn_id].TIMER_CONFIG.bits.timer_config = config;
}

static hi_void aiao_low_set_timer_master_clk(hi_u32 chn_id, const aiao_if_timer_attr *if_attr)
{
    hi_u32 mclk, bclk, fclk;
    hi_u32 ben;
    hi_u32 u32cfg1;

    mclk = get_mclk_crg((hi_u32)(if_attr->rate), if_attr->bclk_div, if_attr->fclk_div);
    bclk = get_bclk_div(if_attr->bclk_div);
    fclk = get_fslk_div(if_attr->fclk_div);
    ben = 1;
    u32cfg1 = bclk + (fclk << 4) + (ben << 8);

    g_aiao_com_reg->TIMER_CRG[chn_id].TIMER_CRG_CFG0.bits.aiao_mclk_div = mclk;
    g_aiao_com_reg->TIMER_CRG[chn_id].TIMER_CRG_CFG1.u32 = u32cfg1;
}

hi_void aiao_timer_set_attr(aiao_timer_id timer_id, const aiao_if_timer_attr *if_attr, hi_u32 config)
{
    hi_u32 chn_id = TIMER2CHID(timer_id);

    aiao_low_set_timer_config(chn_id, config);
    aiao_low_set_timer_master_clk(chn_id, if_attr);
}

hi_void aiao_timer_set_enable(aiao_timer_id timer_id, hi_s32 en)
{
    hi_u32 chn_id = TIMER2CHID(timer_id);

    switch (chn_id) {
        case 0:
            g_aiao_com_reg->AIAO_INT_ENA.bits.timer_ch0_int_ena = en;
            break;
        case 1:
            g_aiao_com_reg->AIAO_INT_ENA.bits.timer_ch1_int_ena = en;
            break;
        default:
            break;
    }
}

hi_void aiao_timer_clear_timer(aiao_timer_id timer_id)
{
    hi_u32 chn_id = TIMER2CHID(timer_id);

    switch (chn_id) {
        case 0:
            g_aiao_com_reg->TIMER_CLEAR.TIMER_INT_CLEAR.bits.timer_ch0_clr = 1;
            break;
        case 1:
            g_aiao_com_reg->TIMER_CLEAR.TIMER_INT_CLEAR.bits.timer_ch1_clr = 1;
            break;
        default:
            break;
    }
}
#endif

static hi_void aiao_low_set_rx_i2s_slave_clk(hi_u32 crg_id, aiao_if_attr *if_attr_slave)
{
    hi_u32 mclk;

    mclk = get_mclk_crg((hi_u32)(if_attr_slave->rate), if_attr_slave->bclk_div, if_attr_slave->fclk_div);

    if (if_attr_slave->rise_edge == AIAO_MODE_EDGE_RISE) {
        g_aiao_com_reg->I2S_RX_CRG[crg_id].I2S_CRG_CFG0.bits.aiao_mclk_div = mclk;
        g_aiao_com_reg->I2S_RX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclkin_pctrl = 1;
        g_aiao_com_reg->I2S_RX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclk_oen = 1;
        g_aiao_com_reg->I2S_RX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclk_sel = 1;
        if (if_attr_slave->i2s_mode == AIAO_MODE_I2S) {
            g_aiao_com_reg->I2S_RX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclkin_pctrl = 0;
        }
    } else {
        g_aiao_com_reg->I2S_RX_CRG[crg_id].I2S_CRG_CFG0.bits.aiao_mclk_div = mclk;
        g_aiao_com_reg->I2S_RX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclkin_pctrl = 0;
        g_aiao_com_reg->I2S_RX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclk_oen = 1;
        g_aiao_com_reg->I2S_RX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclk_sel = 1;
    }
}

static hi_void aiao_low_set_tx_i2s_slave_clk(hi_u32 crg_id, aiao_if_attr *if_attr_slave)
{
    hi_u32 mclk;

    mclk = get_mclk_crg((hi_u32)(if_attr_slave->rate), if_attr_slave->bclk_div, if_attr_slave->fclk_div);

    if (if_attr_slave->rise_edge == AIAO_MODE_EDGE_RISE) {
        g_aiao_com_reg->I2S_TX_CRG[crg_id].I2S_CRG_CFG0.bits.aiao_mclk_div = mclk;
        g_aiao_com_reg->I2S_TX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclkin_pctrl = 1;
        g_aiao_com_reg->I2S_TX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclk_oen = 1;
        g_aiao_com_reg->I2S_TX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclk_sel = 1;
        if (if_attr_slave->i2s_mode == AIAO_MODE_I2S) {
            g_aiao_com_reg->I2S_TX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclkin_pctrl = 0;
        }
    } else {
        g_aiao_com_reg->I2S_TX_CRG[crg_id].I2S_CRG_CFG0.bits.aiao_mclk_div = mclk;
        g_aiao_com_reg->I2S_TX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclkin_pctrl = 0;
        g_aiao_com_reg->I2S_TX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclk_oen = 1;
        g_aiao_com_reg->I2S_TX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclk_sel = 1;
    }
}

static hi_void aiao_low_set_rx_crg_clk_en(hi_u32 crg_id, hi_s32 en)
{
    g_aiao_com_reg->I2S_RX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_cken = en;
}

static hi_void aiao_low_set_tx_crg_clk_en(hi_u32 crg_id, hi_s32 en)
{
    g_aiao_com_reg->I2S_TX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_cken = en;
}

static hi_void aiao_low_set_rx_i2s_master_clk(hi_u32 crg_id, aiao_if_attr *if_attr_master)
{
    hi_u32 mclk, bclk, fclk;

    mclk = get_mclk_crg((hi_u32)(if_attr_master->rate), if_attr_master->bclk_div, if_attr_master->fclk_div);
    bclk = get_bclk_div(if_attr_master->bclk_div);
    fclk = get_fslk_div(if_attr_master->fclk_div);

    g_aiao_com_reg->I2S_RX_CRG[crg_id].I2S_CRG_CFG0.bits.aiao_mclk_div = mclk;
    g_aiao_com_reg->I2S_RX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclk_div = bclk;
    g_aiao_com_reg->I2S_RX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_fsclk_div = fclk;
    g_aiao_com_reg->I2S_RX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclk_oen = 0;
    g_aiao_com_reg->I2S_RX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclk_sel = 0;

    if (if_attr_master->rise_edge == AIAO_MODE_EDGE_RISE) {
        g_aiao_com_reg->I2S_RX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclkout_pctrl = 1;
    } else {
        g_aiao_com_reg->I2S_RX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclkout_pctrl = 0;
    }

    if (if_attr_master->i2s_mode == AIAO_MODE_I2S) {
        g_aiao_com_reg->I2S_RX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclkout_pctrl = 0;
    }
}

static hi_void aiao_low_set_tx_i2s_master_clk(hi_u32 crg_id, aiao_if_attr *if_attr_master)
{
    hi_u32 mclk, bclk, fclk;

    mclk = get_mclk_crg((hi_u32)(if_attr_master->rate), if_attr_master->bclk_div, if_attr_master->fclk_div);
    bclk = get_bclk_div(if_attr_master->bclk_div);
    fclk = get_fslk_div(if_attr_master->fclk_div);

    g_aiao_com_reg->I2S_TX_CRG[crg_id].I2S_CRG_CFG0.bits.aiao_mclk_div = mclk;
    g_aiao_com_reg->I2S_TX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclk_div = bclk;
    g_aiao_com_reg->I2S_TX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_fsclk_div = fclk;
    g_aiao_com_reg->I2S_TX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclk_oen = 0;
    g_aiao_com_reg->I2S_TX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclk_sel = 0;

    if (if_attr_master->rise_edge == AIAO_MODE_EDGE_RISE) {
        g_aiao_com_reg->I2S_TX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclkout_pctrl = 1;
    } else {
        g_aiao_com_reg->I2S_TX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclkout_pctrl = 0;
    }

    if (if_attr_master->i2s_mode == AIAO_MODE_I2S) {
        g_aiao_com_reg->I2S_TX_CRG[crg_id].I2S_CRG_CFG1.bits.aiao_bclkout_pctrl = 0;
    }
}

static hi_void aiao_low_set_i2s_dulicate_clk(hi_u32 chn_id, hi_s32 dir, aiao_if_attr *if_attr_duplicate)
{
    aiao_crg_source org_source = if_attr_duplicate->crg_source;

    if (dir == AIAO_DIR_RX) {
        switch (chn_id) {
            case 0:
                g_aiao_com_reg->AIAO_SWITCH_RX_BCLK.bits.inner_bclk_ws_sel_rx_00 = org_source;
                break;

            case 1:
                g_aiao_com_reg->AIAO_SWITCH_RX_BCLK.bits.inner_bclk_ws_sel_rx_01 = org_source;
                break;
            case 2:
                g_aiao_com_reg->AIAO_SWITCH_RX_BCLK.bits.inner_bclk_ws_sel_rx_02 = org_source;
                break;
            case 3:
                g_aiao_com_reg->AIAO_SWITCH_RX_BCLK.bits.inner_bclk_ws_sel_rx_03 = org_source;
                break;

            case 4:
                g_aiao_com_reg->AIAO_SWITCH_RX_BCLK.bits.inner_bclk_ws_sel_rx_04 = org_source;
                break;

            case 5:
                g_aiao_com_reg->AIAO_SWITCH_RX_BCLK.bits.inner_bclk_ws_sel_rx_05 = org_source;
                break;
            case 6:
                g_aiao_com_reg->AIAO_SWITCH_RX_BCLK.bits.inner_bclk_ws_sel_rx_06 = org_source;
                break;
            case 7:
                g_aiao_com_reg->AIAO_SWITCH_RX_BCLK.bits.inner_bclk_ws_sel_rx_07 = org_source;
                break;
            default:
                break;
        }
    } else if (dir == AIAO_DIR_TX) {
        switch (chn_id) {
            case 0:
                g_aiao_com_reg->AIAO_SWITCH_TX_BCLK.bits.inner_bclk_ws_sel_tx_00 = org_source;
                break;

            case 1:
                g_aiao_com_reg->AIAO_SWITCH_TX_BCLK.bits.inner_bclk_ws_sel_tx_01 = org_source;
                break;
            case 2:
                g_aiao_com_reg->AIAO_SWITCH_TX_BCLK.bits.inner_bclk_ws_sel_tx_02 = org_source;
                break;

            case 3:
                g_aiao_com_reg->AIAO_SWITCH_TX_BCLK.bits.inner_bclk_ws_sel_tx_03 = org_source;
                break;
            case 4:
                g_aiao_com_reg->AIAO_SWITCH_TX_BCLK.bits.inner_bclk_ws_sel_tx_04 = org_source;
                break;

            case 5:
                g_aiao_com_reg->AIAO_SWITCH_TX_BCLK.bits.inner_bclk_ws_sel_tx_05 = org_source;
                break;
            case 6:
                g_aiao_com_reg->AIAO_SWITCH_TX_BCLK.bits.inner_bclk_ws_sel_tx_06 = org_source;
                break;

            case 7:
                g_aiao_com_reg->AIAO_SWITCH_TX_BCLK.bits.inner_bclk_ws_sel_tx_07 = org_source;
                break;

            default:
                break;
        }
    }

    aiao_low_set_i2s_mode(chn_id, dir, if_attr_duplicate->i2s_mode);
}

static hi_void aiao_low_set_spdif_master_clk(hi_u32 chn_id, aiao_if_attr *if_attr_master)
{
    hi_u32 mclk, bclk, fclk;

    mclk = get_mclk_crg((hi_u32)(if_attr_master->rate), if_attr_master->bclk_div, if_attr_master->fclk_div);
    bclk = get_bclk_div(if_attr_master->bclk_div);
    fclk = get_fslk_div(if_attr_master->fclk_div);
    g_aiao_com_reg->SPDIF_CRG[chn_id].SPDIF_CRG_CFG0.bits.aiao_mclk_div_spdif = mclk;
    g_aiao_com_reg->SPDIF_CRG[chn_id].SPDIF_CRG_CFG1.bits.aiao_bclk_div_spdif = bclk;
    g_aiao_com_reg->SPDIF_CRG[chn_id].SPDIF_CRG_CFG1.bits.aiao_fsclk_div_spdif = fclk;
    g_aiao_com_reg->SPDIF_CRG[chn_id].SPDIF_CRG_CFG1.bits.aiao_cken_spdif = 1;
}

static hi_void aiao_low_set_spdif_master_clk_en(hi_u32 chn_id, hi_s32 en)
{
    if (en == 0) {
        g_aiao_com_reg->SPDIF_CRG[chn_id].SPDIF_CRG_CFG1.bits.aiao_srst_req_spdif = 0;
    } else {
        g_aiao_com_reg->SPDIF_CRG[chn_id].SPDIF_CRG_CFG1.bits.aiao_srst_req_spdif = 1;
    }

    g_aiao_com_reg->SPDIF_CRG[chn_id].SPDIF_CRG_CFG1.bits.aiao_cken_spdif = en;
}

static hi_void aiao_low_set_i2s_source_select(hi_u32 chn_id, hi_s32 dir, aiao_i2s_source i2s_sel)
{
    if (dir == AIAO_DIR_RX) {
        g_aiao_rx_reg[chn_id]->RX_IF_ATTRI.bits.rx_sd_source_sel = i2s_sel;
    } else if (dir == AIAO_DIR_TX) {
        g_aiao_tx_reg[chn_id]->TX_IF_ATTRI.bits.tx_sd_source_sel = i2s_sel;
    }
}

static hi_void aiao_low_set_i2s_data_select(hi_u32 chn_id, hi_s32 dir, aiao_i2s_sd org_sd, aiao_i2s_sd src_sd)
{
    if (dir == AIAO_DIR_RX) {
        switch (org_sd) {
            case AIAO_I2S_SD0:
                g_aiao_rx_reg[chn_id]->RX_IF_ATTRI.bits.rx_sd0_sel = src_sd;
                break;
            case AIAO_I2S_SD1:
                g_aiao_rx_reg[chn_id]->RX_IF_ATTRI.bits.rx_sd1_sel = src_sd;
                break;
            case AIAO_I2S_SD2:
                g_aiao_rx_reg[chn_id]->RX_IF_ATTRI.bits.rx_sd2_sel = src_sd;
                break;
            case AIAO_I2S_SD3:
                g_aiao_rx_reg[chn_id]->RX_IF_ATTRI.bits.rx_sd3_sel = src_sd;
                break;
        }
    } else if (dir == AIAO_DIR_TX) {
        switch (org_sd) {
            case AIAO_I2S_SD0:
                g_aiao_tx_reg[chn_id]->TX_IF_ATTRI.bits.tx_sd0_sel = src_sd;
                break;
            case AIAO_I2S_SD1:
                g_aiao_tx_reg[chn_id]->TX_IF_ATTRI.bits.tx_sd1_sel = src_sd;
                break;
            case AIAO_I2S_SD2:
                g_aiao_tx_reg[chn_id]->TX_IF_ATTRI.bits.tx_sd2_sel = src_sd;
                break;
            case AIAO_I2S_SD3:
                g_aiao_tx_reg[chn_id]->TX_IF_ATTRI.bits.tx_sd3_sel = src_sd;
                break;
        }
    }
}

static hi_void aiao_low_set_i2s_bit_depth(hi_u32 chn_id, hi_s32 dir, aiao_bit_depth bit_depth)
{
    if (dir == AIAO_DIR_RX) {
        switch (bit_depth) {
            case AIAO_BIT_DEPTH_8:
                g_aiao_rx_reg[chn_id]->RX_IF_ATTRI.bits.rx_i2s_precision = 0;
                break;
            case AIAO_BIT_DEPTH_16:
                g_aiao_rx_reg[chn_id]->RX_IF_ATTRI.bits.rx_i2s_precision = 1;
                break;
            case AIAO_BIT_DEPTH_24:
                g_aiao_rx_reg[chn_id]->RX_IF_ATTRI.bits.rx_i2s_precision = 2;
                break;
        }
    } else if (dir == AIAO_DIR_TX) {
        switch (bit_depth) {
            case AIAO_BIT_DEPTH_8:
                g_aiao_tx_reg[chn_id]->TX_IF_ATTRI.bits.tx_i2s_precision = 0;
                break;
            case AIAO_BIT_DEPTH_16:
                g_aiao_tx_reg[chn_id]->TX_IF_ATTRI.bits.tx_i2s_precision = 1;
                break;
            case AIAO_BIT_DEPTH_24:
                g_aiao_tx_reg[chn_id]->TX_IF_ATTRI.bits.tx_i2s_precision = 2;
                break;
        }
    }
}

static hi_void aiao_low_set_i2s_mode(hi_u32 chn_id, hi_s32 dir, aiao_i2s_mode i2s_mode)
{
    if (dir == AIAO_DIR_RX) {
        g_aiao_rx_reg[chn_id]->RX_IF_ATTRI.bits.rx_mode = i2s_mode;
    } else if (dir == AIAO_DIR_TX) {
        g_aiao_tx_reg[chn_id]->TX_IF_ATTRI.bits.tx_mode = i2s_mode;
    }
}

static hi_void aiao_low_set_pcm_sync_delay(hi_u32 chn_id, hi_s32 dir, hi_s32 delay_cycles)
{
    if (dir == AIAO_DIR_RX) {
        g_aiao_rx_reg[chn_id]->RX_IF_ATTRI.bits.rx_sd_offset = delay_cycles;
    } else if (dir == AIAO_DIR_TX) {
        g_aiao_tx_reg[chn_id]->TX_IF_ATTRI.bits.tx_sd_offset = delay_cycles;
    }
}

static hi_void aiao_low_set_track_mode(hi_u32 chn_id, hi_s32 dir, aiao_track_mode track_mode)
{
    if (dir == AIAO_DIR_RX) {
        g_aiao_rx_reg[chn_id]->RX_IF_ATTRI.bits.rx_trackmode = track_mode;
    } else if (dir == AIAO_DIR_TX) {
        g_aiao_tx_reg[chn_id]->TX_IF_ATTRI.bits.tx_trackmode = track_mode;
    }
}

static hi_void aiao_low_set_i2s_ch_num(hi_u32 chn_id, hi_s32 dir, hi_s32 is_multislot, aiao_i2s_chnum i2s_chnum)
{
    if (dir == AIAO_DIR_RX) {
        if (is_multislot) {
            switch (i2s_chnum) {
                case AIAO_I2S_CHNUM_2:
                    g_aiao_rx_reg[chn_id]->RX_IF_ATTRI.bits.rx_ch_num = 0;
                    break;
                case AIAO_I2S_CHNUM_4:
                    g_aiao_rx_reg[chn_id]->RX_IF_ATTRI.bits.rx_ch_num = 1;
                    break;
                case AIAO_I2S_CHNUM_8:
                    g_aiao_rx_reg[chn_id]->RX_IF_ATTRI.bits.rx_ch_num = 2;
                    break;
                case AIAO_I2S_CHNUM_16:
                    g_aiao_rx_reg[chn_id]->RX_IF_ATTRI.bits.rx_ch_num = 3;
                    break;
                default:
                    g_aiao_rx_reg[chn_id]->RX_IF_ATTRI.bits.rx_ch_num = 0;
            }
        } else {
            switch (i2s_chnum) {
                case AIAO_I2S_CHNUM_1:
                    g_aiao_rx_reg[chn_id]->RX_IF_ATTRI.bits.rx_ch_num = 0;
                    break;
                case AIAO_I2S_CHNUM_2:
                    g_aiao_rx_reg[chn_id]->RX_IF_ATTRI.bits.rx_ch_num = 1;
                    break;
                case AIAO_I2S_CHNUM_8:
                    g_aiao_rx_reg[chn_id]->RX_IF_ATTRI.bits.rx_ch_num = 3;
                    break;
                default:
                    g_aiao_rx_reg[chn_id]->RX_IF_ATTRI.bits.rx_ch_num = 1;
            }
        }
    } else if (dir == AIAO_DIR_TX) {
        switch (i2s_chnum) {
            case AIAO_I2S_CHNUM_1:
                g_aiao_tx_reg[chn_id]->TX_IF_ATTRI.bits.tx_ch_num = 0;
                break;
            case AIAO_I2S_CHNUM_2:
                g_aiao_tx_reg[chn_id]->TX_IF_ATTRI.bits.tx_ch_num = 1;
                break;
            case AIAO_I2S_CHNUM_8:
                g_aiao_tx_reg[chn_id]->TX_IF_ATTRI.bits.tx_ch_num = 3;
                break;
            default:
                g_aiao_tx_reg[chn_id]->TX_IF_ATTRI.bits.tx_ch_num = 1;
        }
    }
}

static hi_void aiao_low_set_multislot_mode(hi_u32 chn_id, hi_s32 dir, hi_s32 en)
{
    if (dir == AIAO_DIR_RX) {
        g_aiao_rx_reg[chn_id]->RX_IF_ATTRI.bits.rx_multislot_en = en;
    } else if (dir == AIAO_DIR_TX) {
    }
}

static hi_void aiao_low_set_mute(hi_u32 chn_id, hi_s32 dir, hi_s32 en)
{
    if (dir == AIAO_DIR_RX) {
    } else if (dir == AIAO_DIR_TX) {
        g_aiao_tx_reg[chn_id]->TX_DSP_CTRL.bits.mute_en = en;
    }
}

static hi_void aiao_low_set_mute_fade(hi_u32 chn_id, hi_s32 dir, hi_s32 en)
{
    if (dir == AIAO_DIR_RX) {
    } else if (dir == AIAO_DIR_TX) {
        g_aiao_tx_reg[chn_id]->TX_DSP_CTRL.bits.mute_fade_en = en;
    }
}

static hi_void aiao_low_set_fade_in_rate(hi_u32 chn_id, hi_s32 dir, aiao_fade_rate fade_rate)
{
    if (dir == AIAO_DIR_RX) {
    } else if (dir == AIAO_DIR_TX) {
        g_aiao_tx_reg[chn_id]->TX_DSP_CTRL.bits.fade_in_rate = fade_rate;
    }
}

static hi_void aiao_low_set_fade_out_rate(hi_u32 chn_id, hi_s32 dir, aiao_fade_rate fade_rate)
{
    if (dir == AIAO_DIR_RX) {
    } else if (dir == AIAO_DIR_TX) {
        g_aiao_tx_reg[chn_id]->TX_DSP_CTRL.bits.fade_out_rate = fade_rate;
    }
}

static hi_void aiao_low_set_volume_db(hi_u32 chn_id, hi_s32 dir, hi_s32 vol_db)
{
    if (vol_db > AIAO_VOL_MAX_DB) {
        vol_db = AIAO_VOL_MAX_DB;
    }

    if (vol_db < AIAO_VOL_MIN_DB) {
        vol_db = AIAO_VOL_MIN_DB;
    }

    if (dir == AIAO_DIR_RX) {
    } else if (dir == AIAO_DIR_TX) {
        g_aiao_tx_reg[chn_id]->TX_DSP_CTRL.bits.volume = vol_db;
    }
}

static hi_void aiao_low_set_bypass(hi_u32 chn_id, hi_s32 dir, hi_s32 en)
{
    if (dir == AIAO_DIR_RX) {
        g_aiao_rx_reg[chn_id]->RX_DSP_CTRL.bits.bypass_en = en;
    } else if (dir == AIAO_DIR_TX) {
        g_aiao_tx_reg[chn_id]->TX_DSP_CTRL.bits.bypass_en = en;
    }
}

static hi_s32 aiao_low_get_stop_done_status(hi_u32 chn_id, hi_s32 dir)
{
    if (dir == AIAO_DIR_RX) {
        return g_aiao_rx_reg[chn_id]->RX_DSP_CTRL.bits.rx_disable_done;
    } else if (dir == AIAO_DIR_TX) {
        return g_aiao_tx_reg[chn_id]->TX_DSP_CTRL.bits.tx_disable_done;
    }

    return 1;
}

static hi_void aiao_low_set_start(hi_u32 chn_id, hi_s32 dir, hi_s32 en)
{
    if (dir == AIAO_DIR_RX) {
        g_aiao_rx_reg[chn_id]->RX_DSP_CTRL.bits.rx_enable = en;
    } else if (dir == AIAO_DIR_TX) {
        g_aiao_tx_reg[chn_id]->TX_DSP_CTRL.bits.tx_enable = en;
    }
}

/* tx SPDIF interface */
static hi_void aiao_low_spdif_set_bit_depth(hi_u32 chn_id, aiao_bit_depth bit_depth)
{
    switch (bit_depth) {
        case AIAO_BIT_DEPTH_16:
            g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_IF_ATTRI.bits.tx_i2s_precision = 1;
            break;
        case AIAO_BIT_DEPTH_24:
            g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_IF_ATTRI.bits.tx_i2s_precision = 2;
            break;
        default:
            g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_IF_ATTRI.bits.tx_i2s_precision = 1;
            break;
    }
}

static hi_void aiao_low_spdif_set_track_mode(hi_u32 chn_id, aiao_track_mode track_mode)
{
    g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_IF_ATTRI.bits.tx_trackmode = track_mode;
}

static hi_void aiao_low_spdif_set_ch_num(hi_u32 chn_id, aiao_i2s_chnum i2s_chnum)
{
    switch (i2s_chnum) {
        case AIAO_I2S_CHNUM_1:
            g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_IF_ATTRI.bits.tx_ch_num = 0;
            break;
        case AIAO_I2S_CHNUM_2:
            g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_IF_ATTRI.bits.tx_ch_num = 1;
            break;
        default:
            g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_IF_ATTRI.bits.tx_ch_num = 1;
    }
}

/* tx SPDIF DSP */
static hi_void aiao_low_spdif_set_mute(hi_u32 chn_id, hi_s32 en)
{
    g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_DSP_CTRL.bits.mute_en = en;
}

static hi_void aiao_low_spdif_set_mute_fade(hi_u32 chn_id, hi_s32 en)
{
    g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_DSP_CTRL.bits.mute_fade_en = en;
}

static hi_void aiao_low_spdif_set_fade_in_rate(hi_u32 chn_id, aiao_fade_rate fade_rate)
{
    g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_DSP_CTRL.bits.fade_in_rate = fade_rate;
}

static hi_void aiao_low_spdif_set_fade_out_rate(hi_u32 chn_id, aiao_fade_rate fade_rate)
{
    g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_DSP_CTRL.bits.fade_out_rate = fade_rate;
}

static hi_void aiao_low_spdif_set_volume_db(hi_u32 chn_id, hi_s32 vol_db)
{
    if (vol_db > AIAO_VOL_MAX_DB) {
        vol_db = AIAO_VOL_MAX_DB;
    }

    if (vol_db < AIAO_VOL_MIN_DB) {
        vol_db = AIAO_VOL_MIN_DB;
    }

    g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_DSP_CTRL.bits.volume = vol_db;
}

static hi_void aiao_low_spdif_set_bypass(hi_u32 chn_id, hi_s32 en)
{
    g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_DSP_CTRL.bits.bypass_en = en;
}

static hi_s32 aiao_low_spdif_get_stop_done_status(hi_u32 chn_id)
{
    return g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_DSP_CTRL.bits.tx_disable_done;
}

static hi_void aiao_low_spdif_set_start(hi_u32 chn_id, hi_s32 en)
{
    g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_DSP_CTRL.bits.tx_enable = en;
}

/* tx buffer */
static hi_void aiao_txbuf_set_buf_addr_and_size(hi_u32 chn_id, hi_s32 tx_type, hi_u32 start_addr, hi_u32 buf_size)
{
    if (tx_type == AIAO_MODE_TXI2S) {
        g_aiao_tx_reg[chn_id]->TX_BUFF_SADDR = start_addr;
        g_aiao_tx_reg[chn_id]->TX_BUFF_SIZE.bits.tx_buff_size = buf_size;
    } else {
        g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_BUFF_SADDR = start_addr;
        g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_BUFF_SIZE.bits.tx_buff_size = buf_size;
    }
}

static hi_void aiao_txbuf_set_buf_wptr(hi_u32 chn_id, hi_s32 tx_type, hi_u32 wptr)
{
    if (tx_type == AIAO_MODE_TXI2S) {
        g_aiao_tx_reg[chn_id]->TX_BUFF_WPTR.bits.tx_buff_wptr = wptr;
    } else {
        g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_BUFF_WPTR.bits.tx_buff_wptr = wptr;
    }
}

static hi_void aiao_txbuf_set_buf_rptr(hi_u32 chn_id, hi_s32 tx_type, hi_u32 rptr)
{
    if (tx_type == AIAO_MODE_TXI2S) {
        g_aiao_tx_reg[chn_id]->TX_BUFF_RPTR.bits.tx_buff_rptr = rptr;
    } else {
        g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_BUFF_RPTR.bits.tx_buff_rptr = rptr;
    }
}

static hi_void aiao_txbuf_get_buf_wptr(hi_u32 chn_id, hi_s32 tx_type, hi_u32 *wptr)
{
    if (tx_type == AIAO_MODE_TXI2S) {
        *wptr = g_aiao_tx_reg[chn_id]->TX_BUFF_WPTR.bits.tx_buff_wptr;
    } else {
        *wptr = g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_BUFF_WPTR.bits.tx_buff_wptr;
    }
}

static hi_void aiao_txbuf_get_buf_rptr(hi_u32 chn_id, hi_s32 tx_type, hi_u32 *rptr)
{
    if (tx_type == AIAO_MODE_TXI2S) {
        *rptr = g_aiao_tx_reg[chn_id]->TX_BUFF_RPTR.bits.tx_buff_rptr;
    } else {
        *rptr = g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_BUFF_RPTR.bits.tx_buff_rptr;
    }
}

static hi_void aiao_txbuf_get_buf_rptr_addr(hi_u32 chn_id, hi_s32 tx_type, hi_u32 **rptr_addr)
{
    if (tx_type == AIAO_MODE_TXI2S) {
        *rptr_addr = (hi_u32 *)(&(g_aiao_tx_reg[chn_id]->TX_BUFF_RPTR.u32));
    } else {
        *rptr_addr = (hi_u32 *)(&(g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_BUFF_RPTR.u32));
    }
}

static hi_void aiao_txbuf_get_buf_wptr_addr(hi_u32 chn_id, hi_s32 tx_type, hi_u32 **wptr_addr)
{
    if (tx_type == AIAO_MODE_TXI2S) {
        *wptr_addr = (hi_u32 *)(&(g_aiao_tx_reg[chn_id]->TX_BUFF_WPTR.u32));
    } else {
        *wptr_addr = (hi_u32 *)(&(g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_BUFF_WPTR.u32));
    }
}

static hi_void aiao_txbuf_set_buf_trans_size(hi_u32 chn_id, hi_s32 tx_type, hi_u32 period_size)
{
    if (tx_type == AIAO_MODE_TXI2S) {
        g_aiao_tx_reg[chn_id]->TX_TRANS_SIZE.bits.tx_trans_size = period_size;
    } else {
        g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_TRANS_SIZE.bits.tx_trans_size = period_size;
    }
}

static hi_void aiao_txbuf_set_buf_alempty_size(hi_u32 chn_id, hi_s32 tx_type, hi_u32 alempty_size)
{
    if (tx_type == AIAO_MODE_TXI2S) {
        g_aiao_tx_reg[chn_id]->TX_BUFF_ALEMPTY_TH.bits.tx_buff_alempty_th = alempty_size;
    } else {
        g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_BUFF_ALEMPTY_TH.bits.tx_buff_alempty_th = alempty_size;
    }
}

static hi_void aiao_txbuf_get_debug_bclk_cnt(hi_u32 chn_id, hi_s32 tx_type, hi_u32 *dbg_bclk_cnt)
{
    if (tx_type == AIAO_MODE_TXI2S) {
        *dbg_bclk_cnt = g_aiao_tx_reg[chn_id]->TX_BCLK_CNT.bits.bclk_count;
    } else {
        *dbg_bclk_cnt = g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_BCLK_CNT.bits.bclk_count;
    }
}

static hi_void aiao_txbuf_get_debug_fclk_cnt(hi_u32 chn_id, hi_s32 tx_type, hi_u32 *dbg_fclk_cnt)
{
    if (tx_type == AIAO_MODE_TXI2S) {
        *dbg_fclk_cnt = g_aiao_tx_reg[chn_id]->TX_WS_CNT.bits.ws_count;
    } else {
        *dbg_fclk_cnt = g_aiao_tx_spdif_reg[chn_id]->SPDIFTX_WS_CNT.bits.ws_count;
    }
}

static hi_void aiao_rxbuf_set_buf_addr_and_size(hi_u32 chn_id, hi_u32 start_addr, hi_u32 buf_size)
{
    g_aiao_rx_reg[chn_id]->RX_BUFF_SADDR = start_addr;
    g_aiao_rx_reg[chn_id]->RX_BUFF_SIZE.bits.rx_buff_size = buf_size;
}

static hi_void aiao_rxbuf_set_buf_wptr(hi_u32 chn_id, hi_u32 wptr)
{
    g_aiao_rx_reg[chn_id]->RX_BUFF_WPTR.bits.rx_buff_wptr = wptr;
}

static hi_void aiao_rxbuf_set_buf_rptr(hi_u32 chn_id, hi_u32 rptr)
{
    g_aiao_rx_reg[chn_id]->RX_BUFF_RPTR.bits.rx_buff_rptr = rptr;
}

static hi_void aiao_rxbuf_get_buf_wptr(hi_u32 chn_id, hi_u32 *wptr)
{
    *wptr = g_aiao_rx_reg[chn_id]->RX_BUFF_WPTR.bits.rx_buff_wptr;
}

static hi_void aiao_rxbuf_get_buf_rptr(hi_u32 chn_id, hi_u32 *rptr)
{
    *rptr = g_aiao_rx_reg[chn_id]->RX_BUFF_RPTR.bits.rx_buff_rptr;
}

static hi_void aiao_rxbuf_get_buf_wptr_addr(hi_u32 chn_id, hi_u32 **wptr_addr)
{
    *wptr_addr = (hi_u32 *)(&(g_aiao_rx_reg[chn_id]->RX_BUFF_WPTR.u32));
}

static hi_void aiao_rxbuf_get_buf_rptr_addr(hi_u32 chn_id, hi_u32 **rptr_addr)
{
    *rptr_addr = (hi_u32 *)(&(g_aiao_rx_reg[chn_id]->RX_BUFF_RPTR.u32));
}

static hi_void aiao_rxbuf_set_buf_trans_size(hi_u32 chn_id, hi_u32 period_size)
{
    g_aiao_rx_reg[chn_id]->RX_TRANS_SIZE.bits.rx_trans_size = period_size;
}

static hi_void aiao_rxbuf_set_buf_alfull_size(hi_u32 chn_id, hi_u32 alfull_size)
{
    g_aiao_rx_reg[chn_id]->RX_BUFF_ALFULL_TH.bits.rx_buff_alfull_th = alfull_size;
}

static hi_void aiao_rxbuf_get_debug_bclk_cnt(hi_u32 chn_id, hi_u32 *dbg_bclk_cnt)
{
    *dbg_bclk_cnt = g_aiao_rx_reg[chn_id]->RX_BCLK_CNT.bits.bclk_count;
}

static hi_void aiao_rxbuf_get_debug_fclk_cnt(hi_u32 chn_id, hi_u32 *dbg_fclk_cnt)
{
    *dbg_fclk_cnt = g_aiao_rx_reg[chn_id]->RX_WS_CNT.bits.ws_count;
}

hi_void aiao_spdif_hal_set_mode(aiao_port_id port_id, aiao_spdif_mode spdif_mode)
{
    hi_u32 chn_id = PORT2CHID(port_id);

    if (spdif_mode == AIAO_SPDIF_MODE_COMPRESSED) {
        /* passthrough */
        g_aiao_spdifer_reg[chn_id]->SPDIF_CONFIG.u32 = 1;
        /* L:audio sample word used for other purpose */
        g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS1.bits.l_use_b = 1;
        /* R:audio sample word used for other purpose */
        g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS1.bits.r_use_b = 1;
    } else {
        /* PCM */
        g_aiao_spdifer_reg[chn_id]->SPDIF_CONFIG.u32 = 0;
        /* L:audio sample word is linear pcm sample */
        g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS1.bits.l_use_b = 0;
        /* R:audio sample word is linear pcm sample */
        g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS1.bits.r_use_b = 0;
    }
    return;
}

static hi_void set_spidf_category_code(aiao_port_id port_id, aiao_spdif_categorycode category_code)
{
    hi_u32 chn_id = PORT2CHID(port_id);
    g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS2.u32 &= ~0x7F7F;  /* clear bit0 ~ bit6 */
    switch (category_code) {
        case AIAO_SPDIF_CATEGORY_GENERAL:
            g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS2.u32 |= 0x0000;
            break;
        case AIAO_SPDIF_CATEGORY_BROADCAST_JP:
            g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS2.u32 |= 0x0404;
            break;
        case AIAO_SPDIF_CATEGORY_BROADCAST_USA:
            g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS2.u32 |= 0x6464;
            break;
        case AIAO_SPDIF_CATEGORY_BROADCAST_EU:
            g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS2.u32 |= 0x0C0C;
            break;
        case AIAO_SPDIF_CATEGORY_PCM_CODEC:
            g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS2.u32 |= 0x0202;
            break;
        case AIAO_SPDIF_CATEGORY_DIGITAL_SNDSAMPLER:
            g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS2.u32 |= 0x2222;
            break;
        case AIAO_SPDIF_CATEGORY_DIGITAL_MIXER:
            g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS2.u32 |= 0x1212;
            break;
        case AIAO_SPDIF_CATEGORY_DIGITAL_SNDPROCESSOR:
            g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS2.u32 |= 0x2A2A;
            break;
        case AIAO_SPDIF_CATEGORY_SRC:
            g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS2.u32 |= 0x1A1A;
            break;
        case AIAO_SPDIF_CATEGORY_MD:
            g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS2.u32 |= 0x4949;
            break;
        case AIAO_SPDIF_CATEGORY_DVD:
            g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS2.u32 |= 0x1919;
            break;
        case AIAO_SPDIF_CATEGORY_SYNTHESISER:
            g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS2.u32 |= 0x0505;
            break;
        case AIAO_SPDIF_CATEGORY_MIC:
            g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS2.u32 |= 0x0D0D;
            break;
        case AIAO_SPDIF_CATEGORY_DAT:
            g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS2.u32 |= 0x0303;
            break;
        case AIAO_SPDIF_CATEGORY_DCC:
            g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS2.u32 |= 0x4343;
            break;
        case AIAO_SPDIF_CATEGORY_VCR:
            g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS2.u32 |= 0x0B0B;
            break;
        default:
            break;
    }
    return;
}

static hi_void get_category_code_type(aiao_spdif_categorycode category_code, hi_bool *lbit_flag)
{
    switch (category_code) {
        case AIAO_SPDIF_CATEGORY_GENERAL:
        case AIAO_SPDIF_CATEGORY_PCM_CODEC:
        case AIAO_SPDIF_CATEGORY_DIGITAL_SNDSAMPLER:
        case AIAO_SPDIF_CATEGORY_DIGITAL_MIXER:
        case AIAO_SPDIF_CATEGORY_DIGITAL_SNDPROCESSOR:
        case AIAO_SPDIF_CATEGORY_SRC:
        case AIAO_SPDIF_CATEGORY_SYNTHESISER:
        case AIAO_SPDIF_CATEGORY_MIC:
        case AIAO_SPDIF_CATEGORY_DAT:
        case AIAO_SPDIF_CATEGORY_DCC:
        case AIAO_SPDIF_CATEGORY_VCR:
            *lbit_flag = HI_FALSE;
            break;
        case AIAO_SPDIF_CATEGORY_BROADCAST_JP:
        case AIAO_SPDIF_CATEGORY_BROADCAST_USA:
        case AIAO_SPDIF_CATEGORY_BROADCAST_EU:
        case AIAO_SPDIF_CATEGORY_MD:
        case AIAO_SPDIF_CATEGORY_DVD:
            *lbit_flag = HI_TRUE;
            break;
        default:
            break;
    }
    return;
}

hi_void aiao_hw_set_spdif_category_scms(aiao_port_id port_id, aiao_spdif_scms_mode scms_mode,
                                        aiao_spdif_categorycode category_code)
{
    hi_u32 chn_id = PORT2CHID(port_id);
    hi_bool lbit_flag = HI_FALSE;

    set_spidf_category_code(port_id, category_code);
    get_category_code_type(category_code, &lbit_flag);
    switch (scms_mode) {
        case AIAO_SPDIF_SCMS_COPYALLOW:
            g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS2.u32 &= 0xFFFF7F7F;
            g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS1.bits.l_use_c = 1;
            /* R: CP-bit set 1 */
            g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS1.bits.r_use_c = 1;
            break;
        case AIAO_SPDIF_SCMS_COPYONCE:
            if (lbit_flag == HI_FALSE) {
                /* L-bit: 1 */
                g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS2.u32 |= ~0xFFFF7F7F;
            } else {
                /* L-bit: 0 */
                g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS2.u32 &= 0xFFFF7F7F;
            }
            g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS1.bits.l_use_c = 0;
            /* R: CP-bit set 0 */
            g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS1.bits.r_use_c = 0;
            break;
        case AIAO_SPDIF_SCMS_COPYNOMORE:
        case AIAO_SPDIF_SCMS_COPYDEFY:
            if (lbit_flag == HI_FALSE) {
                /* L-bit: 0 */
                g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS2.u32 &= 0xFFFF7F7F;
            } else {
                /* L-bit:1 */
                g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS2.u32 |= ~0xFFFF7F7F;
            }
            g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS1.bits.l_use_c = 0;
            /* CP-bit set 0 */
            g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS1.bits.r_use_c = 0;
            break;
        default:

            break;
    }

    return;
}

hi_void aiao_spdif_hal_set_enable(aiao_port_id port_id, hi_s32 en)
{
    hi_u32 chn_id = PORT2CHID(port_id);

    g_aiao_spdifer_reg[chn_id]->SPDIF_CTRL.bits.spdif_en = en;

    return;
}

hi_void aiao_spdif_hal_set_unknow(aiao_port_id port_id)
{
    hi_u32 chn_id = PORT2CHID(port_id);

    g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS3.u32 = 0x2010;

    return;
}

hi_void aiao_spdif_hal_set_bit_width(aiao_port_id port_id, aiao_bit_depth bit_depth)
{
    hi_u32 tmp;
    hi_u32 max_sample_bits;
    hi_u32 cur_sample_bits;
    hi_u32 chn_id = PORT2CHID(port_id);

    switch (bit_depth) {
        case AIAO_BIT_DEPTH_16:
            cur_sample_bits = 0x1;
            max_sample_bits = 0;
            break;
        case AIAO_BIT_DEPTH_24:
            cur_sample_bits = 0x5;
            max_sample_bits = 1;
            break;
        default:
            cur_sample_bits = 0x1;
            max_sample_bits = 0;
            HI_WARN_AIAO("invaild bit_depth(%d) for spdif\n", bit_depth);
    }

    tmp = g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS5.u32;
    tmp &= (~0x1);
    tmp |= max_sample_bits;
    tmp &= (~(0x1 << 8));
    tmp |= ((max_sample_bits) << 8);
    tmp &= (0xfffffff1);
    tmp |= (cur_sample_bits << 1);
    tmp &= (0xfffff1ff);
    tmp |= ((cur_sample_bits) << 9);
    g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS5.u32 = tmp;
}

hi_void aiao_spdif_hal_set_samplerate(aiao_port_id port_id, aiao_sample_rate sample_rate)
{
    hi_u32 play_samplerate;
    hi_u32 org_samplerate;
    hi_u32 tmp;
    hi_u32 chn_id = PORT2CHID(port_id);

    switch (sample_rate) {
        case AIAO_SAMPLE_RATE_32K:
            play_samplerate = 0x3;
            org_samplerate = 0xc;
            break;
        case AIAO_SAMPLE_RATE_44K:
            play_samplerate = 0x0;
            org_samplerate = 0xf;
            break;
        case AIAO_SAMPLE_RATE_48K:
            play_samplerate = 0x2;
            org_samplerate = 0xd;
            break;
        case AIAO_SAMPLE_RATE_88K:
            play_samplerate = 0x8;
            org_samplerate = 0x7;
            break;
        case AIAO_SAMPLE_RATE_96K:
            play_samplerate = 0xa;
            org_samplerate = 0x5;
            break;
        case AIAO_SAMPLE_RATE_176K:
            play_samplerate = 0xc;
            org_samplerate = 0xf;
            break;
        case AIAO_SAMPLE_RATE_192K:
            play_samplerate = 0xe;
            org_samplerate = 0xd;
            break;
        default:
            play_samplerate = 0x2;
            org_samplerate = 0xd;
    }

    /* config SPDIF IP channel status reg for samplerate */
    tmp = g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS4.u32;
    tmp &= 0xf0f0;
    tmp |= play_samplerate;
    tmp |= (play_samplerate << 8);
    g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS4.u32 = tmp;

    tmp = g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS5.u32;
    tmp &= 0x0f0f;
    tmp |= (org_samplerate << 4);
    tmp |= (org_samplerate << 12);
    g_aiao_spdifer_reg[chn_id]->SPDIF_CH_STATUS5.u32 = tmp;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
