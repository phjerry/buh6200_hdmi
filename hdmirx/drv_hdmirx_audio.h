/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: Definition of audio functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2014/12/27
 */
#ifndef __DRV_HDMIRX_AUDIO_H__
#define __DRV_HDMIRX_AUDIO_H__

#include "hi_type.h"
#include "drv_hdmirx_packet.h"
#include "hal_hdmirx_comm.h"

#define AUDIO_CHANNEL_MASK_TABLE_LENGTH 32
#define AUDIO_FS_LIST_LENGTH            9
#define AUDIO_CHECK_STABLE_THR          10
#define AUDIO_CHST1_ENCODED             0x02    /* 0-PCM, 1-for other purposes */
#define CHST4_SAMPLE_F                  0xf     /* bit[3:0] */
#define CHN_STATUS_READY                0x200   /* bit[9] */
#define AUDIO_FS_CHANGE                 0x40000 /* bit[18] */
#define AUDIO_FIFO_UNDERRUN             0xC000  /* bit[15:14] */
#define AUDIO_FIFO_OVERRUN              0x3000  /* bit[13:12] */
#define AUDIO_AAC_MUTE                  1       /* bit[0] */

#define AUDIO_CHST4_FS_44      0x00 /* fs = 44.1 k_hz */
#define AUDIO_CHST4_FS_UNKNOWN 0x01
#define AUDIO_CHST4_FS_48      0x02 /* fs = 48 k_hz */
#define AUDIO_CHST4_FS_32      0x03 /* fs = 32 k_hz */
#define AUDIO_CHST4_FS_22      0x04 /* fs = 22.05 k_hz */
#define AUDIO_CHST4_FS_24      0x06 /* fs = 24 k_hz */
#define AUDIO_CHST4_FS_88      0x08 /* fs = 88.2 k_hz */
#define AUDIO_CHST4_FS_768     0x09 /* fs = 768 k_hz (HBR audio 4x192k_hz) */
#define AUDIO_CHST4_FS_96      0x0A /* fs = 96 k_hz */
#define AUDIO_CHST4_FS_176     0x0C /* fs = 176.4 k_hz */
#define AUDIO_CHST4_FS_192     0x0E /* fs = 192 k_hz */

#define HDMIRX_AUDIO_GET_CTX(port) (&(g_hdmirx_audio_ctx_v2[port]))

typedef enum hi_hmdirx_audio_bit_width {
    RX_AUDIO_BIT_UNKNOWN1 = 0,
    RX_AUDIO_BIT_BIT16,
    RX_AUDIO_BIT_BIT18,
    RX_AUDIO_BIT_BIT19 = 4,
    RX_AUDIO_BIT_BIT20,
    RX_AUDIO_BIT_BIT17,
    RX_AUDIO_BIT_UNKNOWN2 = 8,
    RX_AUDIO_BIT_BIT20_1,
    RX_AUDIO_BIT_BIT22,
    RX_AUDIO_BIT_BIT23 = 12,
    RX_AUDIO_BIT_BIT24,
    RX_AUDIO_BIT_BIT21,
    RX_AUDIO_BIT_BUTT
} hmdirx_audio_bit_width;

typedef enum hi_hdmirx_audio_channel_num {
    RX_AUDIO_UNKNOWN,
    RX_AUDIO_2CH,
    RX_AUDIO_3CH,
    RX_AUDIO_4CH,
    RX_AUDIO_5CH,
    RX_AUDIO_6CH,
    RX_AUDIO_7CH,
    RX_AUDIO_8CH,
    RX_AUDIO_CH_BUTT
} hdmirx_audio_channel_num;

typedef enum hi_hdmirx_audio_code_type {
    RX_AUDIO_CODE_UNKNOWN,
    RX_AUDIO_CODE_PCM,
    RX_AUDIO_CODE_AC3,
    RX_AUDIO_CODE_MPEG1,
    RX_AUDIO_CODE_MP3,
    RX_AUDIO_CODE_MPEG2,
    RX_AUDIO_CODE_AAC,
    RX_AUDIO_CODE_DTS,
    RX_AUDIO_CODE_ATRAC,
    RX_AUDIO_CODE_ONEBIT,
    RX_AUDIO_CODE_DOLBY,
    RX_AUDIO_CODE_DTSHD,
    RX_AUDIO_CODE_MAT,
    RX_AUDIO_CODE_DST,
    RX_AUDIO_CODE_WMA,
    RX_AUDIO_CODE_BUTT
} hdmirx_audio_code_type;

typedef enum hi_hdmirx_audio_mclk {
    RX_CFG_128FS = 0,
    RX_CFG_256FS = 1,
    RX_CFG_384FS = 2,
    RX_CFG_512FS = 3,
} hdmirx_audio_mclk;

typedef struct hi_audio_fs_search {
    hi_u32 code;   /* corresponding audio status fs code */
    hi_u32 ref_fs; /* reference fs frequency in 100 hz units */
    hi_u32 min_fs; /* minimum fs frequency in 100 hz units */
    hi_u32 max_fs; /* maximum fs frequency in 100 hz units */
} audio_fs_search;

typedef enum hi_hdmix_audio_state {
    AUDIO_STATE_OFF,
    AUDIO_STATE_REQ,
    AUDIO_STATE_READY,
    AUDIO_STATE_ON,
    AUDIO_STATE_BUTT
} hdmix_audio_state;

typedef struct hi_hdmirx_audio_context {
    hdmix_audio_state audio_state;
    hi_u32 ca;               /* channel allocation field from audio info fram */
    hi_u32 fs;               /* sample frequency from the audio channel status */
    hi_bool status_received; /* true if channel status data is valid */

    hi_bool enc;             /* false for PCM, true for encoded streams */
    hi_bool protected;       /* true for audio with protected context and with ACP packet */
    hi_bool hbr_mode;        /* true for HBR audio mode */

    hi_bool start_req;       /* a flag to start audio processing */
    hi_bool soft_mute;       /* true if soft mute is enabled */
    hi_bool blayout1;        /* true for layout 1; false for layout 0 */
    hi_u32 measured_fs;      /* calculated sample frequency in channel status's format */
    hi_bool exceptions_en;   /* true if audio exceptions are on, false otherwise (shadow bit of 0x60.0xB5.0) */
    hi_bool audio_is_on;     /* static variable used in switch_audio() */
    hi_u32 channel_sta[5];   /* first 5 bytes of audio status channel */
    hi_u32 time_out;
    /* hdmirx_audio_channel_num en_channel_num; */
    hi_u32 fifo_err_cnt;
    hdmirx_audio_code_type audio_cod_type;
} hdmirx_audio_context;

hi_void hdmirxv2_audio_initial(hi_drv_hdmirx_port port);
hi_bool hdmirxv2_audio_is_request(hi_drv_hdmirx_port port);
hi_void hdmirxv2_audio_restart(hi_drv_hdmirx_port port);
hi_void hdmirxv2_audio_stop(hi_drv_hdmirx_port port);
hi_void hdmirxv2_audio_get_audio_info(hi_drv_hdmirx_port port, hdmirx_audio_context *audio_ctx);
hi_void hdmirxv2_audio_aac_done(hi_drv_hdmirx_port port);
hi_void hdmirxv2_audio_stream_type_changed(hi_drv_hdmirx_port port);
hi_void hdmirxv2_audio_on_channel_status_chg(hi_drv_hdmirx_port port);
hi_void hdmirxv2_audio_set_ca(hi_drv_hdmirx_port port, hi_u32 ca);
hi_void hdmirxv2_audio_update(hi_drv_hdmirx_port port);
hi_void hdmirxv2_audio_update_on_acp(hi_drv_hdmirx_port port, hdmirx_acp_type type);
hi_void hdmirxv2_audio_main_loop(hi_drv_hdmirx_port port);
hi_void hdmirxv2_audio_get_info(hi_drv_hdmirx_port port, hi_drv_ai_hdmirx_attr *audio_info);
hi_void hdmirxv2_audio_proc_read(hi_drv_hdmirx_port port, hi_void *s);
hdmix_audio_state hdmirxv2_audio_getstatus(hi_drv_hdmirx_port port);
hi_u32 hdmirxv2_audio_get_tmds_clk_10k(hi_drv_hdmirx_port port);
hi_void hdmirxv2_audio_change_state(hi_drv_hdmirx_port port, hdmix_audio_state new_state, hi_u32 delay);
hi_void hdmirxv2_audio_set_exceptions_en(hi_drv_hdmirx_port port, hi_bool en);
hi_void hdmirxv2_audio_set_audio_ready(hi_drv_hdmirx_port port, hi_bool en);
hi_bool hdmirxv2_audio_get_audio_ready(hi_drv_hdmirx_port port);
hi_void hdmirxv2_audio_enable(hi_drv_hdmirx_port port, hi_bool on_off);
hi_void hdmirxv2_audio_set_coding_type(hi_drv_hdmirx_port port, hi_u32 coding_type);
hdmirx_audio_code_type hdmirxv2_audio_get_coding_type(hi_drv_hdmirx_port port);

#endif
