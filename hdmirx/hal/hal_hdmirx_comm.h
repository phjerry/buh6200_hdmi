/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Definitions of common functions, enumeration, and structures
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-12-3
 */
#ifndef __HAL_HDMIRX_COMM_H__
#define __HAL_HDMIRX_COMM_H__

#include "hal_hdmirx_comm.h"
#include "hi_type.h"
#include "drv_hdmirx_struct.h"

#define BIT0   0x01
#define BIT1   0x02
#define BIT2   0x04
#define BIT3   0x08
#define BIT4   0x10
#define BIT5   0x20
#define BIT6   0x40
#define BIT7   0x80
#define BIT7_5 (BIT7 | BIT6 | BIT5)
#define BIT7_6 (BIT7 | BIT6)
#define BIT5_4 (BIT5 | BIT4)
#define BIT4_3 (BIT4 | BIT3)
#define BIT4_2 (BIT4 | BIT3 | BIT2)
#define BIT3_2 (BIT3 | BIT2)
#define BIT2_1 (BIT2 | BIT1)

#define BIT7_4 (BIT7 | BIT6 | BIT5 | BIT4)
#define BIT6_3 (BIT6 | BIT5 | BIT4 | BIT3)
#define BIT6_4 (BIT6 | BIT5 | BIT4)
#define BIT6_2 (BIT6 | BIT5 | BIT4 | BIT3 | BIT2)
#define BIT1_0 (BIT1 | BIT0)
#define BIT2_0 (BIT2 | BIT1 | BIT0)
#define BIT3_0 (BIT3 | BIT2 | BIT1 | BIT0)
#define BIT3_1 (BIT3 | BIT2 | BIT1)
#define BIT4_0 (BIT0 | BIT1 | BIT2 | BIT3 | BIT4)
#define BIT4_1 (BIT1 | BIT2 | BIT3 | BIT4)
#define BIT5_0 (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5)
#define BIT5_3 (BIT3 | BIT4 | BIT5)
#define BIT6_0 (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6)
#define BIT7_0 (BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)
#define BIT31_0 0xffffffff

#define INTR_CEA_UPDATE_GCP      0x1   /* mask bit 0 */
#define INTR_CEA_UPDATE_ACP      0x2   /* mask bit 1 */
#define INTR_CEA_UPDATE_ISRC1    0x4   /* mask bit 2 */
#define INTR_CEA_UPDATE_ISRC2    0x8   /* mask bit 3 */
#define INTR_CEA_UPDATE_GMP      0x10  /* mask bit 4 */
#define INTR_CEA_UPDATE_AUD_META 0x20  /* mask bit 5 */
#define INTR_CEA_UPDATE_VSI  0x40      /* mask bit 6 */
#define INTR_CEA_UPDATE_AVI  0x80      /* mask bit 7 */
#define INTR_CEA_UPDATE_SPD  0x100     /* mask bit 8 */
#define INTR_CEA_UPDATE_AIF  0x200     /* mask bit 9 */
#define INTR_CEA_UPDATE_MPEG 0x400     /* mask bit 10 */
#define INTR_CEA_UPDATE_HDR  0x800     /* mask bit 11 */
#define INTR_CEA_UPDATE_UNREC 0x1000   /* mask bit 12 */
#define INTR_CEA_UPDATE_TRASH 0x2000   /* mask bit 13 */
#define INTR_CEA_UPDATE_HFVSI_3D   0x4000  /* mask bit 14 */
#define INTR_CEA_UPDATE_HFVSI_HDR  0x8000  /* mask bit 15 */
#define INTR_CEA_HFVSI_3D_RETRANS  0x10000 /* mask bit 16 */
#define INTR_CEA_HFVSI_HDR_RETRANS 0x20000 /* mask bit 17 */
#define INTR_CEA_VSI_RETRANS       0x40000 /* mask bit 18 */

#define INTR_CEA_NO_GCP  0x1       /* mask bit 0 */
#define INTR_CEA_NO_ACP  0x2       /* mask bit 1 */
#define INTR_CEA_NO_ISRC1  0x4     /* mask bit 2 */
#define INTR_CEA_NO_ISRC2  0x8     /* mask bit 3 */
#define INTR_CEA_NO_GMP    0x10    /* mask bit 4 */
#define INTR_CEA_NO_AUD_META 0x20  /* mask bit 5 */
#define INTR_CEA_NO_VSI  0x40      /* mask bit 6 */
#define INTR_CEA_NO_AVI  0x80      /* mask bit 7 */
#define INTR_CEA_NO_SPD  0x100     /* mask bit 8 */
#define INTR_CEA_NO_AIF  0x200     /* mask bit 9 */
#define INTR_CEA_NO_MPEG 0x400     /* mask bit 10 */
#define INTR_CEA_NO_HDR  0x800     /* mask bit 11 */
#define INTR_CEA_NO_UNREC 0x1000   /* mask bit 12 */
#define INTR_CEA_NO_TRASH 0x2000   /* mask bit 13 */
#define INTR_CEA_NO_HFVSI_3D  0x4000   /* mask bit 14 */
#define INTR_CEA_NO_HFVSI_HDR 0x8000   /* mask bit 15 */
#define INTR_CEA_VSI_3D_CHANGE 0x10000 /* mask bit 16 */
#define INTR_CEA_SAMPLE_RATE_CHANGE 0x20000 /* mask bit 17 */
#define INTR_CEA_VS                 0x40000 /* mask bit 18 */

#define INTR_SERVING_PACKETS_MASK (INTR_CEA_UPDATE_MPEG | \
    INTR_CEA_UPDATE_AIF | INTR_CEA_UPDATE_SPD | INTR_CEA_UPDATE_AVI)

enum {
    HDMIRX_INTR1,
    HDMIRX_INTR2,
    HDMIRX_INTR3,
    HDMIRX_INTR4,
    HDMIRX_INTR5,
    HDMIRX_INTR_MAX
};

typedef enum hi_hdmirx_input_width {
    HDMIRX_INPUT_WIDTH_24,  /* color depth is 8 bit per channel, 24 bit per pixel */
    HDMIRX_INPUT_WIDTH_30,  /* color depth is 10 bit per channel, 30 bit per pixel */
    HDMIRX_INPUT_WIDTH_36,  /* color depth is 12 bit per channel, 36 bit per pixel */
    HDMIRX_INPUT_WIDTH_48,  /* color depth is 16 bit per channel, 48 bit per pixel */
    HDMIRX_INPUT_WIDTH_MAX
} hdmirx_input_width;

typedef enum hi_hdmirx_color_space {
    HDMIRX_COLOR_SPACE_RGB,
    HDMIRX_COLOR_SPACE_YCBCR422,
    HDMIRX_COLOR_SPACE_YCBCR444,
    HDMIRX_COLOR_SPACE_YCBCR420,
    HDMIRX_COLOR_SPACE_MAX
} hdmirx_color_space;

typedef enum hi_hdmirx_oversample {
    HDMIRX_OVERSAMPLE_NONE,
    HDMIRX_OVERSAMPLE_2X,
    HDMIRX_OVERSAMPLE_3X,
    HDMIRX_OVERSAMPLE_4X,
    HDMIRX_OVERSAMPLE_5X,
    HDMIRX_OVERSAMPLE_6X,
    HDMIRX_OVERSAMPLE_7X,
    HDMIRX_OVERSAMPLE_8X,
    HDMIRX_OVERSAMPLE_9X,
    HDMIRX_OVERSAMPLE_10X,
    HDMIRX_OVERSAMPLE_MAX
} hdmirx_oversample;

typedef enum hi_hdmirx_mute_type {
    HDMIRX_MUTE_TYPE_VS,
    HDMIRX_MUTE_TYPE_HS,
    HDMIRX_MUTE_TYPE_DE,
    HDMIRX_MUTE_TYPE_DATA,
    HDMIRX_MUTE_TYPE_MAX
}hdmirx_mute_type;

typedef enum hi_hdmirx_mute_mode {
    HDMIRX_MUTE_MODE_TIMELY,
    HDMIRX_MUTE_MODE_FRAME,
    HDMIRX_MUTE_MODE_MAX
}hdmirx_mute_mode;

typedef enum {
    HDMIRX_CLK_ZONE_TMDS14_25_42P5,
    HDMIRX_CLK_ZONE_TMDS14_42P5_75,
    HDMIRX_CLK_ZONE_TMDS14_75_150,
    HDMIRX_CLK_ZONE_TMDS14_150_250,
    HDMIRX_CLK_ZONE_TMDS14_250_340,
    HDMIRX_CLK_ZONE_TMDS20_85_100,
    HDMIRX_CLK_ZONE_TMDS20_100_150,
    HDMIRX_CLK_ZONE_FRL_3G,
    HDMIRX_CLK_ZONE_FRL_6G,
    HDMIRX_CLK_ZONE_FRL_8G,
    HDMIRX_CLK_ZONE_FRL_10G,
    HDMIRX_CLK_ZONE_FRL_12G,
    HDMIRX_CLK_ZONE_MAX
} hdmirx_clk_zone;

typedef enum hi_hdmirx_input_type {
    HDMIRX_INPUT_OFF,
    HDMIRX_INPUT_14,
    HDMIRX_INPUT_20,
    HDMIRX_INPUT_FRL3L3G,
    HDMIRX_INPUT_FRL3L6G,
    HDMIRX_INPUT_FRL4L6G,
    HDMIRX_INPUT_FRL4L8G,
    HDMIRX_INPUT_FRL4L10G,
    HDMIRX_INPUT_FRL4L12G,
    HDMIRX_INPUT_MAX
} hdmirx_input_type;

typedef enum hi_hdmirx_term_sel {
    HDMIRX_TERM_SEL_OFF,
    HDMIRX_TERM_SEL_HDMI,
    HDMIRX_TERM_SEL_MHL,
    HDMIRX_TERM_SEL_MAX
} hdmirx_term_sel;

typedef enum hi_hdmirx_damix_status {
    HDMIRX_DAMIX_STATUS_FAIL,
    HDMIRX_DAMIX_STATUS_DONE,
    HDMIRX_DAMIX_STATUS_MAX
} hdmirx_damix_status;

typedef enum hi_hdmirx_damix_type {
    HDMIRX_DAMIX_PLL,
    HDMIRX_DAMIX_CLK,
    HDMIRX_DAMIX_AUTO_EQ,
    HDMIRX_DAMIX_FIX_EQ,
    HDMIRX_DAMIX_DFE,
    HDMIRX_DAMIX_MAX
}hdmirx_damix_type;

typedef enum hi_hdmirx_damix_cfg {
    HDMIRX_DAMIX_CFG_MAX
}hdmirx_damix_cfg;

typedef enum hi_hdmirx_dphy_type {
    HDMIRX_DPHY_DOMAIN_SYNC,
    HDMIRX_DPHY_TMDS_CHAR_SYNC,
    HDMIRX_DPHY_FRL_CHAR_SYNC,
    HDMIRX_DPHY_FRL_LOCK,
    HDMIRX_DPHY_TMDS_CH_DESKEW,
    HDMIRX_DPHY_FRL_CH_DESKEW,
    HDMIRX_DPHY_FRL_LTP_DET,
    HDMIRX_DPHY_HACTIVE_STABLE,
    HDMIRX_DPHY_SCDT_DET,
    HDMIRX_DPHY_SCRAMBLE_STATUS,
    HDMIRX_DPHY_MAX
} hdmirx_dphy_type;

typedef enum hi_hdmirx_dfe_mode {
    HDMIRX_DFE_MODE_HARDWARE,
    HDMIRX_DFE_MODE_SOFTWARE,
    HDMIRX_DFE_MODE_MAX
} hdmirx_dfe_mode;

typedef enum {
    HDMIRX_PWD_INTR_DEPACK,
    HDMIRX_PWD_INTR_VIDEO,
    HDMIRX_PWD_INTR_AUDIO,
    HDMIRX_PWD_INTR_FRL,
    HDMIRX_PWD_INTR_HDCP2X,
    HDMIRX_PWD_INTR_HDCP1X,
    HDMIRX_PWD_INTR_ARC,
    HDMIRX_PWD_INTR_DEPACK_EMP,
    HDMIRX_PWD_INTR_TMDS_DECODER,
    HDMIRX_PWD_INTR_DPHY,
    HDMIRX_PWD_INTR_DAMIX,
    HDMIRX_PWD_INTR_RESERVE,
    HDMIRX_PWD_INTR_DET_CLK0_STAT_CHG,
    HDMIRX_PWD_INTR_DET_CLK1_STAT_CHG,
    HDMIRX_PWD_INTR_DET_CLK2_STAT_CHG,
    HDMIRX_PWD_INTR_DET_CLK3_STAT_CHG,
    HDMIRX_PWD_INTR_DET_PCLK_RATIO_STAT_CHG,
    HDMIRX_PWD_INTR_DET_FRL_RATE_CHG,
    HDMIRX_PWD_INTR_DET_CLK0_SEC,
    HDMIRX_PWD_INTR_MAX
} hdmirx_ctrl_pwd_intr_type;

typedef enum hi_hdmirx_interrupt_idx {
    HDMIRX_INT_CTS,
    HDMIRX_INT_UPDATE_AUD_PACKET,
    HDMIRX_INT_UPDATE_AUD,
    HDMIRX_INT_VRES_CHG,
    HDMIRX_INT_AUD_MUTE,
    HDMIRX_INT_HW_CTS_CHG,
    HDMIRX_INT_CTS_DROP_ERR,
    HDMIRX_INT_CTS_REUSE_ERR,
    HDMIRX_INT_UPDATE_SPD,
    HDMIRX_INT_UPDATE_MPEG,
    HDMIRX_INT_NEW_ACP,
    HDMIRX_INT_NEW_CP,
    HDMIRX_INT_NEW_AVI,
    HDMIRX_INT_AUDIO_FIFO,
    HDMIRX_INT_SCDT,
    HDMIRX_INT_CP_MUTE,
    HDMIRX_INT_NO_AVI,
    HDMIRX_INT_FIFO_OF,
    HDMIRX_INT_FIFO_UF,
    HDMIRX_INT_MAX
} hdmirx_ctrl_pwd_intr_idx;

typedef enum hi_hdmirx_infoframe {
    HDMIRX_AVI = 0x82,    /* AVI info_frame */
    HDMIRX_SPD = 0x83,    /* SPD info_frame */
    HDMIRX_AUDIO = 0x84,  /* audio info_frame */
    HDMIRX_MPEG = 0x85,   /* MPEG info_frame */
    HDMIRX_HDR10 = 0x87,  /* HDR10 info_frame */
    HDMIRX_ISRC1 = 0x05,  /* ISRC1 info_packet */
    HDMIRX_ISRC2 = 0x06,  /* ISRC2 info_packet */
    HDMIRX_ACP = 0x04,    /* ACP info_packet */
    HDMIRX_GC = 0x03,     /* general control info_packet */
    HDMIRX_GBD = 0x0A,    /* GBD info_packet */
    HDMIRX_VSIF = 0x81    /* VSIF info_frame */
} hdmirx_infoframe;

typedef enum {
    HDMIRX_AON_INT_CEC,
    HDMIRX_AON_INT_PWR,
    HDMIRX_AON_INT_P0_DET,
    HDMIRX_AON_INT_P1_DET,
    HDMIRX_AON_INT_P2_DET,
    HDMIRX_AON_INT_P3_DET,
    HDMIRX_AON_INT_MAX
} hdmirx_ctrl_aon_intr_type;

typedef enum {
    HDMIRX_PWD_SRST,
    HDMIRX_PWD_DPHY_CHNNEL_FIFO_SRST,
    HDMIRX_PWD_DPHY_DOMAIN_FIFO_SRST,
    HDMIRX_PWD_DAMIX_SRST,
    HDMIRX_PWD_DPHY_SRST,
    HDMIRX_PWD_TMDS_DECODER_SRST,
    HDMIRX_PWD_FRL_SRST,
    HDMIRX_PWD_FRL_MCU_SRST,
    HDMIRX_PWD_SCDC_SRST,
    HDMIRX_PWD_VID_FIFO_SRST,
    HDMIRX_PWD_VID_SRST,
    HDMIRX_PWD_AUD_FIFO_SRST,
    HDMIRX_PWD_AUD_SRST,
    HDMIRX_PWD_DEPACK_SRST,
    HDMIRX_PWD_HDCPD_SRST,
    HDMIRX_PWD_HDCP1X_CIPHER_SRST,
    HDMIRX_PWD_HDCP1X_SRST,
    HDMIRX_PWD_HDCP1X_KEY_SRST,
    HDMIRX_PWD_HDCP2X_SRST,
    HDMIRX_PWD_HDCP2X_AUTH_SRST,
    HDMIRX_PWD_HDCP2X_CYP_SRST,
    HDMIRX_PWD_HDCP2X_MCU_SRST,
    HDMIRX_PWD_SRST_MAX
} hdmirx_ctrl_pwd_reset_type;

typedef enum {
    HDMIRX_AON_ARST,
    HDMIRX_AON_DDC_ARST,
    HDMIRX_AON_CEC_ARST,
    HDMIRX_AON_DET_ARST,
    HDMIRX_AON_ARST_MAX
} hdmirx_ctrl_aon_reset_type;

typedef enum {
    HDMIRX_AUTORST_DPHY,
    HDMIRX_AUTORST_DPHY_LANE,
    HDMIRX_AUTORST_AUDIO_FIFO,
    HDMIRX_AUTORST_DDC_TOUT,
    HDMIRX_AUTORST_DDC_PWR,
    HDMIRX_AUTORST_DDC_HPD,
    HDMIRX_AUTORST_ALL,
    HDMIRX_AUTORST_MAX
} hdmirx_ctrl_auto_reset_type;

typedef enum {
    HDMIRX_MUTE_VDO,
    HDMIRX_MUTE_AUD,
    HDMIRX_MUTE_ALL,
    HDMIRX_MUTE_MAX
} hdmirx_ctrl_mute_type;

typedef enum {
    HDMIRX_CLOCK_TMDS,
    HDMIRX_CLOCK_FRL,
    HDMIRX_CLOCK_PIXEL,
    HDMIRX_CLOCK_AUDIO,
    HDMIRX_CLOCK_TMDS_D4,
    HDMIRX_CLOCK_FRL_D4,
    HDMIRX_CLOCK_MAX
} hdmirx_ctrl_clock_type;

typedef enum {
    HDMIRX_HW_MUTE_SYS_EVENT_VIDEO,
    HDMIRX_HW_MUTE_SYS_EVENT_AUDID,
    HDMIRX_HW_MUTE_SYS_EVENT_AUDID_UNMUTE,
    HDMIRX_HW_MUTE_SYS_EVENT_DECODE,
    HDMIRX_HW_MUTE_SYS_EVENT_DEPACK,
    HDMIRX_HW_MUTE_VID_EVENT_VIDEO,
    HDMIRX_HW_MUTE_AUD_EVENT_AUDID,
    HDMIRX_HW_MUTE_TMDS_EVENT_VIDEO,
    HDMIRX_HW_MUTE_TMDS_EVENT_AUDID,
    HDMIRX_HW_MUTE_TMDS_EVENT_AUDID_UNMUTE,
    HDMIRX_HW_MUTE_DPK_EVENT_VIDEO,
    HDMIRX_HW_MUTE_DPK_EVENT_AUDIO,
    HDMIRX_HW_MUTE_DPK_EVENT_AUDIO_UNMUTE,
    HDMIRX_HW_MUTE_MAX
} hdmirx_ctrl_hw_mute_type;

typedef enum {
    HDMIRX_PACKET_GCP,
    HDMIRX_PACKET_ACP,
    HDMIRX_PACKET_ISRC1,
    HDMIRX_PACKET_ISRC2,
    HDMIRX_PACKET_GMP,
    HDMIRX_PACKET_VSI,
    HDMIRX_PACKE_HFVSI_3D,
    HDMIRX_PACKET_HFVSI_HDR,
    HDMIRX_PACKET_AVI,
    HDMIRX_PACKET_SPD,
    HDMIRX_PACKET_AIF,
    HDMIRX_PACKET_MPEG,
    HDMIRX_PACKET_HDR,
    HDMIRX_PACKET_AUD_META,
    HDMIRX_PACKET_UNREC,
    HDMIRX_PACKET_MAX
} hdmirx_ctrl_packet_type;

typedef enum {
    HDMIRX_UNREC_EMP_VI,
    HDMIRX_UNREC_EMP_DSC,
    HDMIRX_GEN_EMP_VI,
    HDMIRX_VS0_EMP_VI,
    HDMIRX_VS1_EMP_VI,
    HDMIRX_FVAVRR_EMP_VI,
    HDMIRX_DSC_EMP_DSC,
    HDMIRX_HDR_EMP_VI,
    HDMIRX_MLDS_EMP_BYP,
    HDMIRX_ALL_EMP_VI,
    HDMIRX_EMP_MAX
} hdmirx_ctrl_emp_type;

typedef enum {
    HDMIRX_TIMING_HSYNC_POL_CHANGE,
    HDMIRX_TIMING_VSYNC_POL_CHANGE,
    HDMIRX_TIMING_INTERLACE_CHANGE,
    HDMIRX_TIMING_HACTIVE_CHANGE,
    HDMIRX_TIMING_HSYNC_CHANGE,
    HDMIRX_TIMING_HTOTAL_CHANGE,
    HDMIRX_TIMING_HFRONT_CHANGE,
    HDMIRX_TIMING_HBACK_CHANGE,
    HDMIRX_TIMING_VACTIVE_CHANGE,
    HDMIRX_TIMING_VSYNC_EVEN_CHANGE,
    HDMIRX_TIMING_VSYNC_ODD_CHANGE,
    HDMIRX_TIMING_VTOTAL_EVEN_CHANGE,
    HDMIRX_TIMING_VTOTAL_ODD_CHANGE,
    HDMIRX_TIMING_VFRONT_EVEN_CHANGE,
    HDMIRX_TIMING_VFRONT_ODD_CHANGE,
    HDMIRX_TIMING_VBACK_EVEN_CHANGE,
    HDMIRX_TIMING_VBACK_ODD_CHANGE,
    HDMIRX_TIMING_FRAME_RATE_CHANGE,
    HDMIRX_TIMING_CHANGE_MAX
} hdmirx_ctrl_video_timing_change_type;

typedef struct {
    hi_u32  bch_err_cnt;
    hi_u32  pkt_cnt;
    hi_u32  t4_err_cnt;
} hdmirx_hdcp_err_cnt;

typedef struct {
    hi_u32 hsync_pol_ref;
    hi_u32 vsync_pol_ref;
    hi_u32 interlace_ref;
    hi_u32 hactive_ref;
    hi_u32 hsync_ref;
    hi_u32 htotal_ref;
    hi_u32 hfront_ref;
    hi_u32 hback_ref;
    hi_u32 vactive_ref;
    hi_u32 vsync_even_ref;
    hi_u32 vsync_odd_ref;
    hi_u32 vtotal_even_ref;
    hi_u32 vtotal_odd_ref;
    hi_u32 vfront_even_ref;
    hi_u32 vfront_odd_ref;
    hi_u32 vback_even_ref;
    hi_u32 vback_odd_ref;
    hi_u32 frame_rate_ref;
} hdmirx_ctrl_video_timing_change_ref;

typedef enum {
    HDMIRX_SYS_EVENT_ALL = 0x8,
    HDMIRX_SYS_EVENT_MAX
} hdmirx_ctrl_sys_event_type;

typedef struct {
    hdmirx_ctrl_video_timing_change_type timing_change_type;
    hdmirx_ctrl_video_timing_change_ref  timing_change_ref;
} hdmirx_ctrl_video_event_type;

typedef enum {
    HDMIRX_AUDIO_EVENT_ALL = 0x14,
    HDMIRX_AUDIO_EVENT_MAX
} hdmirx_ctrl_audio_event_type;

typedef enum {
    HDMIRX_TMDS_EVENT_ALL = 0x8,
    HDMIRX_TMDS_EVENT_MAX
} hdmirx_ctrl_tmds_event_type;

typedef enum {
    HDMIRX_DEPACK_EVENT_ALL = 0x10,
    HDMIRX_DEPACK_EVENT_MAX
} hdmirx_ctrl_depack_event_type;

typedef struct {
    hdmirx_ctrl_sys_event_type    sys_event;
    hdmirx_ctrl_video_event_type  video_event;
    hdmirx_ctrl_audio_event_type  audio_event;
    hdmirx_ctrl_tmds_event_type   tmds_event;
    hdmirx_ctrl_depack_event_type depack_event;
} hdmirx_ctrl_event_type;

hi_s32 hdmirx_hal_reg_init(hi_void);
hi_void hdmirx_hal_reg_deinit(hi_void);
hi_u32 hdmirx_hal_reg_read(hi_drv_hdmirx_port port, hi_u32 addr);
hi_void hdmirx_hal_reg_write(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 value);
hi_u32 hdmirx_hal_reg_read_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask);
hi_void hdmirx_hal_reg_write_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask, hi_u32 value);
hi_void hdmirx_hal_reg_read_block(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 *dst, hi_u32 num);
hi_void hdmirx_hal_reg_set_bits(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask, hi_bool value);
hi_void hdmirx_hal_reg_write_block(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 *src, hi_u32 num);
hi_void hdmirx_hal_aon_reg_write(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 value);
hi_u32 hdmirx_hal_aon_reg_read(hi_drv_hdmirx_port port, hi_u32 addr);
hi_u32 hdmirx_hal_aon_reg_read_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask);
hi_void hdmirx_hal_aon_reg_write_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask, hi_u32 value);
hi_void hdmirx_hal_aon_reg_write_block(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 *src, hi_u32 num);
hi_void hdmirx_hal_dphy_reg_write(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 value);
hi_u32 hdmirx_hal_dphy_reg_read(hi_drv_hdmirx_port port, hi_u32 addr);
hi_u32 hdmirx_hal_dphy_reg_read_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask);
hi_void hdmirx_hal_dphy_reg_write_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask, hi_u32 value);
hi_void hdmirx_hal_damix_reg_write(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 value);
hi_u32 hdmirx_hal_damix_reg_read(hi_drv_hdmirx_port port, hi_u32 addr);
hi_u32 hdmirx_hal_damix_reg_read_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask);
hi_void hdmirx_hal_damix_reg_write_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask, hi_u32 value);
hi_void hdmirx_hal_damix_reg_set_bits(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask, hi_bool value);
hi_u32 hdmirx_hal_hdcp1x_reg_read_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask);
hi_void hdmirx_hal_hdcp1x_reg_write_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask, hi_u32 value);
hi_u32 hdmirx_hal_hdcp1x_reg_read(hi_drv_hdmirx_port port, hi_u32 addr);
hi_void hdmirx_hal_hdcp1x_reg_write(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 value);
hi_u32 hdmirx_hal_hdcp2x_reg_read_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask);
hi_void hdmirx_hal_hdcp2x_reg_write_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask, hi_u32 value);
hi_u32 hdmirx_hal_hdcp2x_reg_read(hi_drv_hdmirx_port port, hi_u32 addr);
hi_void hdmirx_hal_hdcp2x_reg_write(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 value);
hi_void hdmirx_hal_hdcp_ram_reg_write(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 value);
hi_u32 hdmirx_hal_hdcp_ram_reg_read(hi_drv_hdmirx_port port, hi_u32 addr);
hi_void hdmirx_hal_frl_ram_reg_write(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 value);
hi_u32 hdmirx_hal_frl_ram_reg_read(hi_drv_hdmirx_port port, hi_u32 addr);
hi_void hdmirx_reg_write_map(hi_u32 reg_addr, hi_u32 value);
hi_u32 hdmirx_reg_read_map(hi_u32 reg_addr);

#endif /* __HAL_HDMIRX_COMM_H__ */
