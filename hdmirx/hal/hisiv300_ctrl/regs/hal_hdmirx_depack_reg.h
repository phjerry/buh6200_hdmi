/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: register definition of depack  module
 * Author: Hisilicon multimedia interface software group
 * Create: 2019/12/4
 */
#ifndef __HAL_HDMIRX_DEPACK_REG_H__
#define __HAL_HDMIRX_DEPACK_REG_H__

#define REG_RX_ECC_CTRL 0x1000
#define CFG_CAPTURE_COUNTS    0x1 /* mask bit 0 */
#define CFG_PKT_ERR_IGNORE_EN 0x2 /* mask bit 1 */

#define REG_RX_BCH_THRES 0x1004
#define CFG_BCH_THRESH 0x1F /* mask bit 4:0 */

#define REG_RX_PKT_THRESH 0x1008
#define CFG_HDMI_PKT_THRESH 0x3FFF /* mask bit 13:0 */

#define REG_RX_T4_PKT_THRES 0x100C
#define CFG_T4_PKT_THRESH 0x3FFF /* mask bit 13:0 */

#define REG_RX_BCH_PKT_THRES 0x1010
#define CFG_BCH_PKT_THRESH 0x3FFF /* mask bit 13:0 */

#define REG_RX_HDCP_THRES 0x1014
#define CFG_HDCP_PKT_THRESH 0x3FFF /* mask bit 13:0 */

#define REG_RX_PKT_CNT 0x1018
#define PACKET_RECEIVED_COUNT 0x3FFF /* mask bit 13:0 */

#define REG_RX_T4_ERR 0x101C
#define T4_PACKET_ERR_COUNT 0x3FFF /* mask bit 13:0 */

#define REG_RX_BCH_ERR 0x1020
#define BCH_PACKET_ERR_COUNT 0x3FFF /* mask bit 13:0 */

#define REG_RX_HDCP_ERR 0x1024
#define HDCP_PACKET_ERR_COUNT 0x3FFF /* mask bit 13:0 */

#define REG_DEC_AV_MUTE 0x1028
#define CFG_VIDEO_MUTE_SW  0x1  /* mask bit 0 */
#define CFG_VIDEO_MUTE_OVR 0x2  /* mask bit 1 */
#define CFG_AV_MUTE_SW     0x20 /* mask bit 5 */
#define CFG_AV_MUTE_OVR    0x40 /* mask bit 6 */

#define REG_RX_PHASE_LUT 0x102C
#define CFG_PHASE0_LUT 0x3  /* mask bit 1:0 */
#define CFG_PHASE1_LUT 0xC  /* mask bit 3:2 */
#define CFG_PHASE2_LUT 0x30 /* mask bit 5:4 */
#define CFG_PHASE3_LUT 0xC0 /* mask bit 7:6 */

#define REG_GCP_CONFIG 0x1030
#define CFG_TMDS_MODE_SW      0x7     /* mask bit 2:0 */
#define CFG_TMDS_MODE_OVR     0x8     /* mask bit 3 */
#define CFG_DEFAULT_PHASE_SW  0x10    /* mask bit 4 */
#define CFG_PP_SW             0x60    /* mask bit 6:5 */
#define CFG_PHASE_OVR         0x80    /* mask bit 7 */
#define CFG_PHASE_UPDATE      0x100   /* mask bit 8 */
#define CFG_DP_LOST_CNT       0xF000  /* mask bit 15:12 */
#define CFG_DEFAULT_PHASE_BYP 0x10000 /* mask bit 16 */
#define CFG_PP_CHECK_EN       0x20000 /* mask bit 17 */

#define REG_DEEPCOLOR_INFO 0x1034
#define TMDS_MODE     0x7  /* mask bit 2:0 */
#define TMDS_PHASE    0x38 /* mask bit 5:3 */
#define DEFAULT_PHASE 0x40 /* mask bit 6 */

#define REG_VSI_IEEE_CHK 0x1038
#define CFG_VSI_IEEE_EN       0x1 /* mask bit 0 */
#define CFG_HFVSI_3D_IEEE_EN  0x2 /* mask bit 1 */
#define CFG_HFVSI_HDR_IEEE_EN 0x4 /* mask bit 2 */

#define REG_RX_AUDP_STAT 0x103C
#define HDMI_MODE_DET 0x1  /* mask bit 0 */
#define HDMI_MODE_EN  0x2  /* mask bit 1 */
#define HDMI_MUTE     0x4  /* mask bit 2 */
#define HDMI_LAYOUT   0x18 /* mask bit 4:3 */
#define AUD_DSD_ON    0x20 /* mask bit 5 */
#define HDMI_HBRA_ON  0x40 /* mask bit 6 */
#define VIDEO_MUTE    0x80 /* mask bit 7 */

#define REG_LINK_LOST_STATUS 0x1040
#define LINK_LOST 0x1 /* mask bit 0 */

#define REG_DSC_STATUS 0x1044
#define DSC_EN 0x1 /* mask bit 0 */

#define REG_START_LOCATION 0x1048
#define CFG_FAPA_START_LOCATION 0x1 /* mask bit 0 */

#define REG_AUDIO_FS 0x104C
#define CEA_AIF_FS 0xF /* mask bit 3:0 */

#define REG_GCPRX_WORD0 0x1060
#define CEA_GCP_DATA0 0xFFFFFFFF /* mask bit 31:0 */

#define REG_GCPRX_WORD1 0x1064
#define CEA_GCP_DATA1 0xFFFFFFFF /* mask bit 31:0 */

#define REG_GCPRX_WORD2 0x1068
#define CEA_GCP_DATA2 0xFFFFFFFF /* mask bit 31:0 */

#define REG_GCPRX_WORD3 0x106C
#define CEA_GCP_DATA3 0xFFFFFFFF /* mask bit 31:0 */

#define REG_GCPRX_WORD4 0x1070
#define CEA_GCP_DATA4 0xFFFFFFFF /* mask bit 31:0 */

#define REG_GCPRX_WORD5 0x1074
#define CEA_GCP_DATA5 0xFFFFFFFF /* mask bit 31:0 */

#define REG_GCPRX_WORD6 0x1078
#define CEA_GCP_DATA6 0xFFFFFFFF /* mask bit 31:0 */

#define REG_GCPRX_WORD7 0x107C
#define CEA_GCP_DATA7 0x1FFFFFF /* mask bit 24:0 */

#define REG_ACPRX_WORD0 0x1080
#define CEA_ACP_DATA0 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ACPRX_WORD1 0x1084
#define CEA_ACP_DATA1 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ACPRX_WORD2 0x1088
#define CEA_ACP_DATA2 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ACPRX_WORD3 0x108C
#define CEA_ACP_DATA3 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ACPRX_WORD4 0x1090
#define CEA_ACP_DATA4 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ACPRX_WORD5 0x1094
#define CEA_ACP_DATA5 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ACPRX_WORD6 0x1098
#define CEA_ACP_DATA6 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ACPRX_WORD7 0x109C
#define CEA_ACP_DATA7 0x1FFFFFF /* mask bit 24:0 */

#define REG_ISRC1RX_WORD0 0x10A0
#define CEA_ISRC1_DATA0 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ISRC1RX_WORD1 0x10A4
#define CEA_ISRC1_DATA1 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ISRC1RX_WORD2 0x10A8
#define CEA_ISRC1_DATA2 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ISRC1RX_WORD3 0x10AC
#define CEA_ISRC1_DATA3 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ISRC1RX_WORD4 0x10B0
#define CEA_ISRC1_DATA4 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ISRC1RX_WORD5 0x10B4
#define CEA_ISRC1_DATA5 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ISRC1RX_WORD6 0x10B8
#define CEA_ISRC1_DATA6 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ISRC1RX_WORD7 0x10BC
#define CEA_ISRC1_DATA7 0x1FFFFFF /* mask bit 24:0 */

#define REG_ISRC2RX_WORD0 0x10C0
#define CEA_ISRC2_DATA0 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ISRC2RX_WORD1 0x10C4
#define CEA_ISRC2_DATA1 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ISRC2RX_WORD2 0x10C8
#define CEA_ISRC2_DATA2 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ISRC2RX_WORD3 0x10CC
#define CEA_ISRC2_DATA3 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ISRC2RX_WORD4 0x10D0
#define CEA_ISRC2_DATA4 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ISRC2RX_WORD5 0x10D4
#define CEA_ISRC2_DATA5 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ISRC2RX_WORD6 0x10D8
#define CEA_ISRC2_DATA6 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ISRC2RX_WORD7 0x10DC
#define CEA_ISRC2_DATA7 0x1FFFFFF /* mask bit 24:0 */

#define REG_GMPRX_WORD0 0x10E0
#define CEA_GMP_DATA0 0xFFFFFFFF /* mask bit 31:0 */

#define REG_GMPRX_WORD1 0x10E4
#define CEA_GMP_DATA1 0xFFFFFFFF /* mask bit 31:0 */

#define REG_GMPRX_WORD2 0x10E8
#define CEA_GMP_DATA2 0xFFFFFFFF /* mask bit 31:0 */

#define REG_GMPRX_WORD3 0x10EC
#define CEA_GMP_DATA3 0xFFFFFFFF /* mask bit 31:0 */

#define REG_GMPRX_WORD4 0x10F0
#define CEA_GMP_DATA4 0xFFFFFFFF /* mask bit 31:0 */

#define REG_GMPRX_WORD5 0x10F4
#define CEA_GMP_DATA5 0xFFFFFFFF /* mask bit 31:0 */

#define REG_GMPRX_WORD6 0x10F8
#define CEA_GMP_DATA6 0xFFFFFFFF /* mask bit 31:0 */

#define REG_GMPRX_WORD7 0x10FC
#define CEA_GMP_DATA7 0x1FFFFFF /* mask bit 24:0 */

#define REG_VSIRX_WORD0 0x1100
#define CEA_VSI_DATA0 0xFFFFFFFF /* mask bit 31:0 */

#define REG_VSIRX_WORD1 0x1104
#define CEA_VSI_DATA1 0xFFFFFFFF /* mask bit 31:0 */

#define REG_VSIRX_WORD2 0x1108
#define CEA_VSI_DATA2 0xFFFFFFFF /* mask bit 31:0 */

#define REG_VSIRX_WORD3 0x110C
#define CEA_VSI_DATA3 0xFFFFFFFF /* mask bit 31:0 */

#define REG_VSIRX_WORD4 0x1110
#define CEA_VSI_DATA4 0xFFFFFFFF /* mask bit 31:0 */

#define REG_VSIRX_WORD5 0x1114
#define CEA_VSI_DATA5 0xFFFFFFFF /* mask bit 31:0 */

#define REG_VSIRX_WORD6 0x1118
#define CEA_VSI_DATA6 0xFFFFFFFF /* mask bit 31:0 */

#define REG_VSIRX_WORD7 0x111C
#define CEA_VSI_DATA7 0x1FFFFFF /* mask bit 24:0 */

#define REG_HF_VSI_3DRX_WORD0 0x1120
#define CEA_HFVSI_3D_DATA0 0xFFFFFFFF /* mask bit 31:0 */

#define REG_HF_VSI_3DRX_WORD1 0x1124
#define CEA_HFVSI_3D_DATA1 0xFFFFFFFF /* mask bit 31:0 */

#define REG_HF_VSI_3DRX_WORD2 0x1128
#define CEA_HFVSI_3D_DATA2 0xFFFFFFFF /* mask bit 31:0 */

#define REG_HF_VSI_3DRX_WORD3 0x112C
#define CEA_HFVSI_3D_DATA3 0xFFFFFFFF /* mask bit 31:0 */

#define REG_HF_VSI_3DRX_WORD4 0x1130
#define CEA_HFVSI_3D_DATA4 0xFFFFFFFF /* mask bit 31:0 */

#define REG_HF_VSI_3DRX_WORD5 0x1134
#define CEA_HFVSI_3D_DATA5 0xFFFFFFFF /* mask bit 31:0 */

#define REG_HF_VSI_3DRX_WORD6 0x1138
#define CEA_HFVSI_3D_DATA6 0xFFFFFFFF /* mask bit 31:0 */

#define REG_HF_VSI_3DRX_WORD7 0x113C
#define CEA_HFVSI_3D_DATA7 0x1FFFFFF /* mask bit 24:0 */

#define REG_HF_VSI_HDRRX_WORD0 0x1140
#define CEA_HFVSI_HDR_DATA0 0xFFFFFFFF /* mask bit 31:0 */

#define REG_HF_VSI_HDRRX_WORD1 0x1144
#define CEA_HFVSI_HDR_DATA1 0xFFFFFFFF /* mask bit 31:0 */

#define REG_HF_VSI_HDRRX_WORD2 0x1148
#define CEA_HFVSI_HDR_DATA2 0xFFFFFFFF /* mask bit 31:0 */

#define REG_HF_VSI_HDRRX_WORD3 0x114C
#define CEA_HFVSI_HDR_DATA3 0xFFFFFFFF /* mask bit 31:0 */

#define REG_HF_VSI_HDRRX_WORD4 0x1150
#define CEA_HFVSI_HDR_DATA4 0xFFFFFFFF /* mask bit 31:0 */

#define REG_HF_VSI_HDRRX_WORD5 0x1154
#define CEA_HFVSI_HDR_DATA5 0xFFFFFFFF /* mask bit 31:0 */

#define REG_HF_VSI_HDRRX_WORD6 0x1158
#define CEA_HFVSI_HDR_DATA6 0xFFFFFFFF /* mask bit 31:0 */

#define REG_HF_VSI_HDRRX_WORD7 0x115C
#define CEA_HFVSI_HDR_DATA7 0x1FFFFFF /* mask bit 24:0 */

#define REG_AVIRX_WORD0 0x1160
#define CEA_AVI_DATA0 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AVIRX_WORD1 0x1164
#define CEA_AVI_DATA1 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AVIRX_WORD2 0x1168
#define CEA_AVI_DATA2 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AVIRX_WORD3 0x116C
#define CEA_AVI_DATA3 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AVIRX_WORD4 0x1170
#define CEA_AVI_DATA4 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AVIRX_WORD5 0x1174
#define CEA_AVI_DATA5 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AVIRX_WORD6 0x1178
#define CEA_AVI_DATA6 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AVIRX_WORD7 0x117C
#define CEA_AVI_DATA7 0x1FFFFFF /* mask bit 24:0 */

#define REG_SPDRX_WORD0 0x1180
#define CEA_SPD_DATA0 0xFFFFFFFF /* mask bit 31:0 */

#define REG_SPDRX_WORD1 0x1184
#define CEA_SPD_DATA1 0xFFFFFFFF /* mask bit 31:0 */

#define REG_SPDRX_WORD2 0x1188
#define CEA_SPD_DATA2 0xFFFFFFFF /* mask bit 31:0 */

#define REG_SPDRX_WORD3 0x118C
#define CEA_SPD_DATA3 0xFFFFFFFF /* mask bit 31:0 */

#define REG_SPDRX_WORD4 0x1190
#define CEA_SPD_DATA4 0xFFFFFFFF /* mask bit 31:0 */

#define REG_SPDRX_WORD5 0x1194
#define CEA_SPD_DATA5 0xFFFFFFFF /* mask bit 31:0 */

#define REG_SPDRX_WORD6 0x1198
#define CEA_SPD_DATA6 0xFFFFFFFF /* mask bit 31:0 */

#define REG_SPDRX_WORD7 0x119C
#define CEA_SPD_DATA7 0x1FFFFFF /* mask bit 24:0 */

#define REG_AIFRX_WORD0 0x11A0
#define CEA_AIF_DATA0 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AIFRX_WORD1 0x11A4
#define CEA_AIF_DATA1 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AIFRX_WORD2 0x11A8
#define CEA_AIF_DATA2 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AIFRX_WORD3 0x11AC
#define CEA_AIF_DATA3 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AIFRX_WORD4 0x11B0
#define CEA_AIF_DATA4 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AIFRX_WORD5 0x11B4
#define CEA_AIF_DATA5 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AIFRX_WORD6 0x11B8
#define CEA_AIF_DATA6 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AIFRX_WORD7 0x11BC
#define CEA_AIF_DATA7 0x1FFFFFF /* mask bit 24:0 */

#define REG_MPEGRX_WORD0 0x11C0
#define CEA_MPEG_DATA0 0xFFFFFFFF /* mask bit 31:0 */

#define REG_MPEGRX_WORD1 0x11C4
#define CEA_MPEG_DATA1 0xFFFFFFFF /* mask bit 31:0 */

#define REG_MPEGRX_WORD2 0x11C8
#define CEA_MPEG_DATA2 0xFFFFFFFF /* mask bit 31:0 */

#define REG_MPEGRX_WORD3 0x11CC
#define CEA_MPEG_DATA3 0xFFFFFFFF /* mask bit 31:0 */

#define REG_MPEGRX_WORD4 0x11D0
#define CEA_MPEG_DATA4 0xFFFFFFFF /* mask bit 31:0 */

#define REG_MPEGRX_WORD5 0x11D4
#define CEA_MPEG_DATA5 0xFFFFFFFF /* mask bit 31:0 */

#define REG_MPEGRX_WORD6 0x11D8
#define CEA_MPEG_DATA6 0xFFFFFFFF /* mask bit 31:0 */

#define REG_MPEGRX_WORD7 0x11DC
#define CEA_MPEG_DATA7 0x1FFFFFF /* mask bit 24:0 */

#define REG_HDRRX_WORD0 0x11E0
#define CEA_HDR_DATA0 0xFFFFFFFF /* mask bit 31:0 */

#define REG_HDRRX_WORD1 0x11E4
#define CEA_HDR_DATA1 0xFFFFFFFF /* mask bit 31:0 */

#define REG_HDRRX_WORD2 0x11E8
#define CEA_HDR_DATA2 0xFFFFFFFF /* mask bit 31:0 */

#define REG_HDRRX_WORD3 0x11EC
#define CEA_HDR_DATA3 0xFFFFFFFF /* mask bit 31:0 */

#define REG_HDRRX_WORD4 0x11F0
#define CEA_HDR_DATA4 0xFFFFFFFF /* mask bit 31:0 */

#define REG_HDRRX_WORD5 0x11F4
#define CEA_HDR_DATA5 0xFFFFFFFF /* mask bit 31:0 */

#define REG_HDRRX_WORD6 0x11F8
#define CEA_HDR_DATA6 0xFFFFFFFF /* mask bit 31:0 */

#define REG_HDRRX_WORD7 0x11FC
#define CEA_HDR_DATA7 0x1FFFFFF /* mask bit 24:0 */

#define REG_AUD_METARX_WORD0 0x1200
#define CEA_AUD_META_DATA0 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AUD_METARX_WORD1 0x1204
#define CEA_AUD_META_DATA1 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AUD_METARX_WORD2 0x1208
#define CEA_AUD_META_DATA2 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AUD_METARX_WORD3 0x120C
#define CEA_AUD_META_DATA3 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AUD_METARX_WORD4 0x1210
#define CEA_AUD_META_DATA4 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AUD_METARX_WORD5 0x1214
#define CEA_AUD_META_DATA5 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AUD_METARX_WORD6 0x1218
#define CEA_AUD_META_DATA6 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AUD_METARX_WORD7 0x121C
#define CEA_AUD_META_DATA7 0x1FFFFFF /* mask bit 24:0 */

#define REG_UNRECRX_WORD0 0x1220
#define CEA_UNREC_DATA0 0xFFFFFFFF /* mask bit 31:0 */

#define REG_UNRECRX_WORD1 0x1224
#define CEA_UNREC_DATA1 0xFFFFFFFF /* mask bit 31:0 */

#define REG_UNRECRX_WORD2 0x1228
#define CEA_UNREC_DATA2 0xFFFFFFFF /* mask bit 31:0 */

#define REG_UNRECRX_WORD3 0x122C
#define CEA_UNREC_DATA3 0xFFFFFFFF /* mask bit 31:0 */

#define REG_UNRECRX_WORD4 0x1230
#define CEA_UNREC_DATA4 0xFFFFFFFF /* mask bit 31:0 */

#define REG_UNRECRX_WORD5 0x1234
#define CEA_UNREC_DATA5 0xFFFFFFFF /* mask bit 31:0 */

#define REG_UNRECRX_WORD6 0x1238
#define CEA_UNREC_DATA6 0xFFFFFFFF /* mask bit 31:0 */

#define REG_UNRECRX_WORD7 0x123C
#define CEA_UNREC_DATA7 0x1FFFFFF /* mask bit 24:0 */

#define REG_TRASH_CANRX_WORD0 0x1240
#define CEA_TRASH_DATA0 0xFFFFFFFF /* mask bit 31:0 */

#define REG_TRASH_CANRX_WORD1 0x1244
#define CEA_TRASH_DATA1 0xFFFFFFFF /* mask bit 31:0 */

#define REG_TRASH_CANRX_WORD2 0x1248
#define CEA_TRASH_DATA2 0xFFFFFFFF /* mask bit 31:0 */

#define REG_TRASH_CANRX_WORD3 0x124C
#define CEA_TRASH_DATA3 0xFFFFFFFF /* mask bit 31:0 */

#define REG_TRASH_CANRX_WORD4 0x1250
#define CEA_TRASH_DATA4 0xFFFFFFFF /* mask bit 31:0 */

#define REG_TRASH_CANRX_WORD5 0x1254
#define CEA_TRASH_DATA5 0xFFFFFFFF /* mask bit 31:0 */

#define REG_TRASH_CANRX_WORD6 0x1258
#define CEA_TRASH_DATA6 0xFFFFFFFF /* mask bit 31:0 */

#define REG_TRASH_CANRX_WORD7 0x125C
#define CEA_TRASH_DATA7 0x1FFFFFF /* mask bit 24:0 */

#define REG_PKT_ID_0 0x1260
#define CFG_GCP_ID   0xFF       /* mask bit 7:0 */
#define CFG_ACP_ID   0xFF00     /* mask bit 15:8 */
#define CFG_ISRC1_ID 0xFF0000   /* mask bit 23:16 */
#define CFG_ISRC2_ID 0xFF000000 /* mask bit 31:24 */

#define REG_PKT_ID_1 0x1264
#define CFG_GMP_ID      0xFF       /* mask bit 7:0 */
#define CFG_AUD_META_ID 0xFF00     /* mask bit 15:8 */
#define CFG_VSI_ID      0xFF0000   /* mask bit 23:16 */
#define CFG_AVI_ID      0xFF000000 /* mask bit 31:24 */

#define REG_PKT_ID_2 0x1268
#define CFG_SPD_ID  0xFF       /* mask bit 7:0 */
#define CFG_AIF_ID  0xFF00     /* mask bit 15:8 */
#define CFG_MPEG_ID 0xFF0000   /* mask bit 23:16 */
#define CFG_HDR_ID  0xFF000000 /* mask bit 31:24 */

#define REG_PKT_ID_3 0x126C
#define CFG_UNREC_ID 0xFF /* mask bit 7:0 */

#define REG_AUD_PKT_0 0x1270
#define CFG_ACR_ID 0xFF       /* mask bit 7:0 */
#define CFG_ASP_ID 0xFF00     /* mask bit 15:8 */
#define CFG_DSD_ID 0xFF0000   /* mask bit 23:16 */
#define CFG_DST_ID 0xFF000000 /* mask bit 31:24 */

#define REG_AUD_PKT_1 0x1274
#define CFG_HBR_ID    0xFF       /* mask bit 7:0 */
#define CFG_3D_ASP_ID 0xFF00     /* mask bit 15:8 */
#define CFG_3D_DSD_ID 0xFF0000   /* mask bit 23:16 */
#define CFG_MS_ASP_ID 0xFF000000 /* mask bit 31:24 */

#define REG_AUD_PKT_2 0x1278
#define CFG_1BIT_MS_ASP_ID 0xFF /* mask bit 7:0 */

#define REG_PKT_CLR_0 0x127C
#define CFG_GCP_CLR_EN     0x1    /* mask bit 0 */
#define CFG_GCP_AUTOCLR_EN 0x2    /* mask bit 1 */
#define CFG_GCP_LOST_CNT   0xFC   /* mask bit 7:2 */
#define CFG_ACP_CLR_EN     0x100  /* mask bit 8 */
#define CFG_ACP_AUTOCLR_EN 0x200  /* mask bit 9 */
#define CFG_ACP_LOST_CNT   0xFC00 /* mask bit 15:10 */

#define REG_PKT_CLR_1 0x1280
#define CFG_ISRC1_CLR_EN        0x1        /* mask bit 0 */
#define CFG_ISRC1_AUTOCLR_EN    0x2        /* mask bit 1 */
#define CFG_ISRC1_LOST_CNT      0xFC       /* mask bit 7:2 */
#define CFG_ISRC2_CLR_EN        0x100      /* mask bit 8 */
#define CFG_ISRC2_AUTOCLR_EN    0x200      /* mask bit 9 */
#define CFG_ISRC2_LOST_CNT      0xFC00     /* mask bit 15:10 */
#define CFG_GMP_CLR_EN          0x10000    /* mask bit 16 */
#define CFG_GMP_AUTOCLR_EN      0x20000    /* mask bit 17 */
#define CFG_GMP_LOST_CNT        0xFC0000   /* mask bit 23:18 */
#define CFG_AUD_META_CLR_EN     0x1000000  /* mask bit 24 */
#define CFG_AUD_META_AUTOCLR_EN 0x2000000  /* mask bit 25 */
#define CFG_AUD_META_LOST_CNT   0xFC000000 /* mask bit 31:26 */

#define REG_PKT_CLR_2 0x1284
#define CFG_VSI_CLR_EN     0x1        /* mask bit 0 */
#define CFG_VSI_AUTOCLR_EN 0x2        /* mask bit 1 */
#define CFG_VSI_LOST_CNT   0xFC       /* mask bit 7:2 */
#define CFG_AVI_CLR_EN     0x100      /* mask bit 8 */
#define CFG_AVI_AUTOCLR_EN 0x200      /* mask bit 9 */
#define CFG_AVI_LOST_CNT   0xFC00     /* mask bit 15:10 */
#define CFG_SPD_CLR_EN     0x10000    /* mask bit 16 */
#define CFG_SPD_AUTOCLR_EN 0x20000    /* mask bit 17 */
#define CFG_SPD_LOST_CNT   0xFC0000   /* mask bit 23:18 */
#define CFG_AIF_CLR_EN     0x1000000  /* mask bit 24 */
#define CFG_AIF_AUTOCLR_EN 0x2000000  /* mask bit 25 */
#define CFG_AIF_LOST_CNT   0xFC000000 /* mask bit 31:26 */

#define REG_PKT_CLR_3 0x1288
#define CFG_MPEG_CLR_EN      0x1        /* mask bit 0 */
#define CFG_MPEG_AUTOCLR_EN  0x2        /* mask bit 1 */
#define CFG_MPEG_LOST_CNT    0xFC       /* mask bit 7:2 */
#define CFG_HDR_CLR_EN       0x100      /* mask bit 8 */
#define CFG_HDR_AUTOCLR_EN   0x200      /* mask bit 9 */
#define CFG_HDR_LOST_CNT     0xFC00     /* mask bit 15:10 */
#define CFG_UNREC_CLR_EN     0x10000    /* mask bit 16 */
#define CFG_UNREC_AUTOCLR_EN 0x20000    /* mask bit 17 */
#define CFG_UNREC_LOST_CNT   0xFC0000   /* mask bit 23:18 */
#define CFG_TRASH_CLR_EN     0x1000000  /* mask bit 24 */
#define CFG_TRASH_AUTOCLR_EN 0x2000000  /* mask bit 25 */
#define CFG_TRASH_LOST_CNT   0xFC000000 /* mask bit 31:26 */

#define REG_PKT_CLR_4 0x128C
#define CFG_HFVSI_3D_CLR_EN      0x1    /* mask bit 0 */
#define CFG_HFVSI_3D_AUTOCLR_EN  0x2    /* mask bit 1 */
#define CFG_HFVSI_3D_LOST_CNT    0xFC   /* mask bit 7:2 */
#define CFG_HFVSI_HDR_CLR_EN     0x100  /* mask bit 8 */
#define CFG_HFVSI_HDR_AUTOCLR_EN 0x200  /* mask bit 9 */
#define CFG_HFVSI_HDR_LOST_CNT   0xFC00 /* mask bit 15:10 */

#define REG_AUD_PKT_DET 0x1290
#define CFG_ACR_EN         0x1   /* mask bit 0 */
#define CFG_ASP_EN         0x2   /* mask bit 1 */
#define CFG_DSD_EN         0x4   /* mask bit 2 */
#define CFG_DST_EN         0x8   /* mask bit 3 */
#define CFG_HBR_EN         0x10  /* mask bit 4 */
#define CFG_3D_ASP_EN      0x20  /* mask bit 5 */
#define CFG_3D_DSD_EN      0x40  /* mask bit 6 */
#define CFG_MS_ASP_EN      0x80  /* mask bit 7 */
#define CFG_1BIT_MS_ASP_EN 0x100 /* mask bit 8 */

#define REG_PKT_DET 0x1294
#define CFG_ACP_EN       0x1    /* mask bit 0 */
#define CFG_GCP_EN       0x2    /* mask bit 1 */
#define CFG_ISRC1_EN     0x4    /* mask bit 2 */
#define CFG_ISRC2_EN     0x8    /* mask bit 3 */
#define CFG_GMP_EN       0x10   /* mask bit 4 */
#define CFG_AUD_META_EN  0x20   /* mask bit 5 */
#define CFG_VSI_EN       0x40   /* mask bit 6 */
#define CFG_AVI_EN       0x80   /* mask bit 7 */
#define CFG_SPD_EN       0x100  /* mask bit 8 */
#define CFG_AIF_EN       0x200  /* mask bit 9 */
#define CFG_MPEG_EN      0x400  /* mask bit 10 */
#define CFG_HDR_EN       0x800  /* mask bit 11 */
#define CFG_UNREC_EN     0x1000 /* mask bit 12 */
#define CFG_TRASH_EN     0x2000 /* mask bit 13 */
#define CFG_HFVSI_3D_EN  0x4000 /* mask bit 14 */
#define CFG_HFVSI_HDR_EN 0x8000 /* mask bit 15 */

#define REG_PKT_ID_CFG_0 0x1298
#define CFG_PKT_ID0 0xFF       /* mask bit 7:0 */
#define CFG_PKT_ID1 0xFF00     /* mask bit 15:8 */
#define CFG_PKT_ID2 0xFF0000   /* mask bit 23:16 */
#define CFG_PKT_ID3 0xFF000000 /* mask bit 31:24 */

#define REG_PKT_ID_CFG_1 0x129C
#define CFG_PKT_ID4 0xFF       /* mask bit 7:0 */
#define CFG_PKT_ID5 0xFF00     /* mask bit 15:8 */
#define CFG_PKT_ID6 0xFF0000   /* mask bit 23:16 */
#define CFG_PKT_ID7 0xFF000000 /* mask bit 31:24 */

#define REG_PKT_ID_CFG_2 0x12A0
#define CFG_PKT_ID8 0xFF   /* mask bit 7:0 */
#define CFG_PKT_ID9 0xFF00 /* mask bit 15:8 */

#define REG_PKT_CAPTURE 0x12A4
#define CFG_PKT_EN 0x3FF /* mask bit 9:0 */

#define REG_VS0_EMP_CFG 0x12A8
#define CFG_VS0_EMP_ORG_ID   0xFF     /* mask bit 7:0 */
#define CFG_VS0_EMP_DATA_SET 0xFFFF00 /* mask bit 23:8 */

#define REG_VS1_EMP_CFG 0x12AC
#define CFG_VS1_EMP_ORG_ID   0xFF     /* mask bit 7:0 */
#define CFG_VS1_EMP_DATA_SET 0xFFFF00 /* mask bit 23:8 */

#define REG_VS0_OUI_CFG 0x12B0
#define CFG_VS0_EMP_IEEE_OUI 0xFFFFFF /* mask bit 23:0 */

#define REG_VS1_OUI_CFG 0x12B4
#define CFG_VS1_EMP_IEEE_OUI 0xFFFFFF /* mask bit 23:0 */

#define REG_DSC_EMP_CFG 0x12B8
#define CFG_DSC_EMP_ORG_ID   0xFF     /* mask bit 7:0 */
#define CFG_DSC_EMP_DATA_SET 0xFFFF00 /* mask bit 23:8 */

#define REG_HDR_EMP_CFG_0 0x12BC
#define CFG_HDR_EMP_ORG_ID   0xFF     /* mask bit 7:0 */
#define CFG_HDR_DATA_SET_LOW 0xFFFF00 /* mask bit 23:8 */

#define REG_HDR_EMP_CFG_1 0x12C0
#define CFG_HDR_DATA_SET_HIGH 0xFFFF /* mask bit 15:0 */

#define REG_FVAVRR_EMP_CFG 0x12C4
#define CFG_FVAVRR_EMP_ORG_ID   0xFF     /* mask bit 7:0 */
#define CFG_FVAVRR_EMP_DATA_SET 0xFFFF00 /* mask bit 23:8 */

#define REG_GEN_EMP_CFG 0x12C8
#define CFG_GEN_EMP_ORG_ID   0xFF     /* mask bit 7:0 */
#define CFG_GEN_EMP_DATA_SET 0xFFFF00 /* mask bit 23:8 */

#define REG_EMP_TIMEOUT_CNT 0x12CC
#define CFG_VS0_EMP_TIMEOUT    0xF      /* mask bit 3:0 */
#define CFG_VS1_EMP_TIMEOUT    0xF0     /* mask bit 7:4 */
#define CFG_DSC_EMP_TIMEOUT    0xF00    /* mask bit 11:8 */
#define CFG_HDR_EMP_TIMEOUT    0xF000   /* mask bit 15:12 */
#define CFG_FVAVRR_EMP_TIMEOUT 0xF0000  /* mask bit 19:16 */
#define CFG_GEN_EMP_TIMEOUT    0xF00000 /* mask bit 23:20 */

#define REG_EMP_LOST_CNT 0x12D0
#define CFG_VS0_EMP_LOST    0xF      /* mask bit 3:0 */
#define CFG_VS1_EMP_LOST    0xF0     /* mask bit 7:4 */
#define CFG_DSC_EMP_LOST    0xF00    /* mask bit 11:8 */
#define CFG_HDR_EMP_LOST    0xF000   /* mask bit 15:12 */
#define CFG_FVAVRR_EMP_LOST 0xF0000  /* mask bit 19:16 */
#define CFG_GEN_EMP_LOST    0xF00000 /* mask bit 23:20 */

#define REG_EMP_CFG_EN 0x12D4
#define CFG_VI_UNREC_EMP_EN  0x1   /* mask bit 0 */
#define CFG_DSC_UNREC_EMP_EN 0x2   /* mask bit 1 */
#define CFG_GEN_EMP_EN       0x4   /* mask bit 2 */
#define CFG_VS0_EMP_EN       0x8   /* mask bit 3 */
#define CFG_VS1_EMP_EN       0x10  /* mask bit 4 */
#define CFG_FVAVRR_EMP_EN    0x20  /* mask bit 5 */
#define CFG_DSC_EMP_EN       0x40  /* mask bit 6 */
#define CFG_HDR_EMP_EN       0x80  /* mask bit 7 */
#define CFG_VI_PATH_BYP      0x100 /* mask bit 8 */
#define CFG_MLDS_EMP_BYP     0x200 /* mask bit 9 */
#define CFG_DSC2VI_EMP_EN    0x400 /* mask bit 10 */

#define REG_CPRX_BYTE1 0x12D8
#define CEA_CP_SET_MUTE 0x1  /* mask bit 0 */
#define CEA_CP_CLR_MUTE 0x10 /* mask bit 4 */

#define REG_VSI_IEEE_ID 0x12DC
#define CFG_VSI_IEEE_ID 0xFFFFFF /* mask bit 23:0 */

#define REG_HF_VSI_3D_IEEE_ID 0x12E0
#define CFG_HFVSI_3D_IEEE_ID 0xFFFFFF /* mask bit 23:0 */

#define REG_HF_VSI_HDR_IEEE_ID 0x12E4
#define CFG_HFVSI_HDR_IEEE_ID 0xFFFFFF /* mask bit 23:0 */

#define REG_VS0_EMP_ERR_0 0x12E8
#define VS0_EMP_LOST_CNT      0xFF     /* mask bit 7:0 */
#define VS0_EMP_OVERLAP_CNT   0xFF00   /* mask bit 15:8 */
#define VS0_EMP_INDEX_ERR_CNT 0xFF0000 /* mask bit 23:16 */

#define REG_VS0_EMP_ERR_1 0x12EC
#define VS0_EMP_RETRANS_CNT  0xFF   /* mask bit 7:0 */
#define VS0_EMP_FAPA_ERR_CNT 0xFF00 /* mask bit 15:8 */

#define REG_VS1_EMP_ERR_0 0x12F0
#define VS1_EMP_LOST_CNT      0xFF     /* mask bit 7:0 */
#define VS1_EMP_OVERLAP_CNT   0xFF00   /* mask bit 15:8 */
#define VS1_EMP_INDEX_ERR_CNT 0xFF0000 /* mask bit 23:16 */

#define REG_VS1_EMP_ERR_1 0x12F4
#define VS1_EMP_RETRANS_CNT  0xFF   /* mask bit 7:0 */
#define VS1_EMP_FAPA_ERR_CNT 0xFF00 /* mask bit 15:8 */

#define REG_DSC_EMP_ERR_0 0x12F8
#define DSC_EMP_LOST_CNT      0xFF     /* mask bit 7:0 */
#define DSC_EMP_OVERLAP_CNT   0xFF00   /* mask bit 15:8 */
#define DSC_EMP_INDEX_ERR_CNT 0xFF0000 /* mask bit 23:16 */

#define REG_DSC_EMP_ERR_1 0x12FC
#define DSC_EMP_RETRANS_CNT  0xFF   /* mask bit 7:0 */
#define DSC_EMP_FAPA_ERR_CNT 0xFF00 /* mask bit 15:8 */

#define REG_HDR_EMP_ERR_0 0x1300
#define HDR_EMP_LOST_CNT      0xFF     /* mask bit 7:0 */
#define HDR_EMP_OVERLAP_CNT   0xFF00   /* mask bit 15:8 */
#define HDR_EMP_INDEX_ERR_CNT 0xFF0000 /* mask bit 23:16 */

#define REG_HDR_EMP_ERR_1 0x1304
#define HDR_EMP_RETRANS_CNT  0xFF   /* mask bit 7:0 */
#define HDR_EMP_FAPA_ERR_CNT 0xFF00 /* mask bit 15:8 */

#define REG_FVAVRR_EMP_ERR_0 0x1308
#define FVAVRR_EMP_LOST_CNT      0xFF     /* mask bit 7:0 */
#define FVAVRR_EMP_OVERLAP_CNT   0xFF00   /* mask bit 15:8 */
#define FVAVRR_EMP_INDEX_ERR_CNT 0xFF0000 /* mask bit 23:16 */

#define REG_FVAVRR_EMP_ERR_1 0x130C
#define FVAVRR_EMP_RETRANS_CNT 0xFF /* mask bit 7:0 */

#define REG_GEN_EMP_ERR_0 0x1310
#define GEN_EMP_LOST_CNT      0xFF     /* mask bit 7:0 */
#define GEN_EMP_OVERLAP_CNT   0xFF00   /* mask bit 15:8 */
#define GEN_EMP_INDEX_ERR_CNT 0xFF0000 /* mask bit 23:16 */

#define REG_GEN_EMP_ERR_1 0x1314
#define GEN_EMP_RETRANS_CNT  0xFF   /* mask bit 7:0 */
#define GEN_EMP_FAPA_ERR_CNT 0xFF00 /* mask bit 15:8 */

#define REG_PKT_ERR_CNT0 0x1318
#define HFVSI_HDR_RETRANS_CNT 0xFF       /* mask bit 7:0 */
#define HFVSI_3D_RETRANS_CNT  0xFF00     /* mask bit 15:8 */
#define HFVSI_HDR_LOST_CNT    0xFF0000   /* mask bit 23:16 */
#define HFVSI_3D_LOST_CNT     0xFF000000 /* mask bit 31:24 */

#define REG_PKT_ERR_CNT1 0x131C
#define VSI_LOST_CNT 0xFF     /* mask bit 7:0 */
#define AVI_LOST_CNT 0xFF00   /* mask bit 15:8 */
#define HDR_LOST_CNT 0xFF0000 /* mask bit 23:16 */

#define REG_EMP_ERR_CLEAR 0x1320
#define CFG_EMP_ERR_CLEAR 0x1 /* mask bit 0 */

#define REG_VI_TEST_PKT 0x1324
#define CFG_VI_TEST_INDEX    0xFF       /* mask bit 7:0 */
#define CFG_VI_TEST_ORG_ID   0xFF00     /* mask bit 15:8 */
#define CFG_VI_TEST_DATA_SET 0xFFFF0000 /* mask bit 31:16 */

#define REG_DSC_TEST_PKT 0x1328
#define CFG_DSC_TEST_INDEX    0xFF       /* mask bit 7:0 */
#define CFG_DSC_TEST_ORG_ID   0xFF00     /* mask bit 15:8 */
#define CFG_DSC_TEST_DATA_SET 0xFFFF0000 /* mask bit 31:16 */

#define REG_EMP_TEST_CFG 0x132C
#define CFG_VI_TEST_EN  0x1 /* mask bit 0 */
#define CFG_DSC_TEST_EN 0x2 /* mask bit 1 */

#define REG_VI_INTR_MASK_0 0x1330
#define CFG_VI_INTR0_MASK 0xFFFFFFFF /* mask bit 31:0 */

#define REG_VI_INTR_MASK_1 0x1334
#define CFG_VI_INTR1_MASK 0xFFFFFFFF /* mask bit 31:0 */

#define REG_DSC_INTR_MASK 0x1338
#define CFG_DSC_INTR0_MASK 0xFFFFFFFF /* mask bit 31:0 */

#define REG_VACTIVE_HW 0x133C
#define VACTIVE_HW 0xFFFFFF /* mask bit 23:0 */

#define REG_VFRONT_HW 0x1340
#define VFRONT_HW 0xFFFFFF /* mask bit 23:0 */

#define REG_VBACK_HW 0x1344
#define VBACK_HW 0xFFFFFF /* mask bit 23:0 */

#define REG_VTOTAL_DET_HW 0x1348
#define VTOTAL_HW 0xFFFFFF /* mask bit 23:0 */

#define REG_VRR_STATUS 0x134C
#define VRR_MODE 0x1 /* mask bit 0 */

#define REG_HTOTAL_HW 0x1350
#define HTOTAL_HW 0x7FFF /* mask bit 14:0 */

#define REG_VRR_MLDS_LAST_CFG 0x1354
#define CFG_MLDS_QUIT_VRR   0x1 /* mask bit 0 */
#define CFG_VRR_LAST_IGNORE 0x2 /* mask bit 1 */

#define REG_FVAVRRRX_WORD0 0x1360
#define CEA_FVAVRR_DATA0 0xFFFFFFFF /* mask bit 31:0 */

#define REG_FVAVRRRX_WORD1 0x1364
#define CEA_FVAVRR_DATA1 0xFFFFFFFF /* mask bit 31:0 */

#define REG_FVAVRRRX_WORD2 0x1368
#define CEA_FVAVRR_DATA2 0xFFFFFFFF /* mask bit 31:0 */

#define REG_FVAVRRRX_WORD3 0x136C
#define CEA_FVAVRR_DATA3 0xFFFFFFFF /* mask bit 31:0 */

#define REG_FVAVRRRX_WORD4 0x1370
#define CEA_FVAVRR_DATA4 0xFFFFFFFF /* mask bit 31:0 */

#define REG_FVAVRRRX_WORD5 0x1374
#define CEA_FVAVRR_DATA5 0xFFFFFFFF /* mask bit 31:0 */

#define REG_FVAVRRRX_WORD6 0x1378
#define CEA_FVAVRR_DATA6 0xFFFFFFFF /* mask bit 31:0 */

#define REG_FVAVRRRX_WORD7 0x137C
#define CEA_FVAVRR_DATA7 0xFFFFFFFF /* mask bit 31:0 */

#define REG_VI_TESTRX_WORD0 0x1380
#define CEA_VI_TEST_DATA0 0xFFFFFFFF /* mask bit 31:0 */

#define REG_VI_TESTRX_WORD1 0x1384
#define CEA_VI_TEST_DATA1 0xFFFFFFFF /* mask bit 31:0 */

#define REG_VI_TESTRX_WORD2 0x1388
#define CEA_VI_TEST_DATA2 0xFFFFFFFF /* mask bit 31:0 */

#define REG_VI_TESTRX_WORD3 0x138C
#define CEA_VI_TEST_DATA3 0xFFFFFFFF /* mask bit 31:0 */

#define REG_VI_TESTRX_WORD4 0x1390
#define CEA_VI_TEST_DATA4 0xFFFFFFFF /* mask bit 31:0 */

#define REG_VI_TESTRX_WORD5 0x1394
#define CEA_VI_TEST_DATA5 0xFFFFFFFF /* mask bit 31:0 */

#define REG_VI_TESTRX_WORD6 0x1398
#define CEA_VI_TEST_DATA6 0xFFFFFFFF /* mask bit 31:0 */

#define REG_VI_TESTRX_WORD7 0x139C
#define CEA_VI_TEST_DATA7 0xFFFFFFFF /* mask bit 31:0 */

#define REG_DSC_TESTRX_WORD0 0x13A0
#define CEA_DSC_TEST_DATA0 0xFFFFFFFF /* mask bit 31:0 */

#define REG_DSC_TESTRX_WORD1 0x13A4
#define CEA_DSC_TEST_DATA1 0xFFFFFFFF /* mask bit 31:0 */

#define REG_DSC_TESTRX_WORD2 0x13A8
#define CEA_DSC_TEST_DATA2 0xFFFFFFFF /* mask bit 31:0 */

#define REG_DSC_TESTRX_WORD3 0x13AC
#define CEA_DSC_TEST_DATA3 0xFFFFFFFF /* mask bit 31:0 */

#define REG_DSC_TESTRX_WORD4 0x13B0
#define CEA_DSC_TEST_DATA4 0xFFFFFFFF /* mask bit 31:0 */

#define REG_DSC_TESTRX_WORD5 0x13B4
#define CEA_DSC_TEST_DATA5 0xFFFFFFFF /* mask bit 31:0 */

#define REG_DSC_TESTRX_WORD6 0x13B8
#define CEA_DSC_TEST_DATA6 0xFFFFFFFF /* mask bit 31:0 */

#define REG_DSC_TESTRX_WORD7 0x13BC
#define CEA_DSC_TEST_DATA7 0xFFFFFFFF /* mask bit 31:0 */

#define REG_FAPA_CFG0 0x13C0
#define CFG_FAPA_END     0xFFFFFF  /* mask bit 23:0 */
#define CFG_FAPA_END_OVR 0x1000000 /* mask bit 24 */

#define REG_VBLANK_VIC 0x13C4
#define CFG_VBLANK_VIC 0xFFFFFF  /* mask bit 23:0 */
#define CFG_VBLANK_OVR 0x1000000 /* mask bit 24 */

#define REG_DEPACK_INTR1_MASK 0x13CC
#define DEPACK_INTR1_STAT0_MASK  0x1    /* mask bit 0 */
#define DEPACK_INTR1_STAT1_MASK  0x2    /* mask bit 1 */
#define DEPACK_INTR1_STAT2_MASK  0x4    /* mask bit 2 */
#define DEPACK_INTR1_STAT3_MASK  0x8    /* mask bit 3 */
#define DEPACK_INTR1_STAT4_MASK  0x10   /* mask bit 4 */
#define DEPACK_INTR1_STAT5_MASK  0x20   /* mask bit 5 */
#define DEPACK_INTR1_STAT6_MASK  0x40   /* mask bit 6 */
#define DEPACK_INTR1_STAT7_MASK  0x80   /* mask bit 7 */
#define DEPACK_INTR1_STAT8_MASK  0x100  /* mask bit 8 */
#define DEPACK_INTR1_STAT9_MASK  0x200  /* mask bit 9 */
#define DEPACK_INTR1_STAT10_MASK 0x400  /* mask bit 10 */
#define DEPACK_INTR1_STAT11_MASK 0x800  /* mask bit 11 */
#define DEPACK_INTR1_STAT12_MASK 0x1000 /* mask bit 12 */
#define DEPACK_INTR1_STAT13_MASK 0x2000 /* mask bit 13 */
#define DEPACK_INTR1_STAT14_MASK 0x4000 /* mask bit 14 */
#define DEPACK_INTR1_STAT15_MASK 0x8000 /* mask bit 15 */

#define REG_DEPACK_INTR1 0x13D0
#define DEPACK_INTR1_STAT0  0x1    /* mask bit 0 */
#define DEPACK_INTR1_STAT1  0x2    /* mask bit 1 */
#define DEPACK_INTR1_STAT2  0x4    /* mask bit 2 */
#define DEPACK_INTR1_STAT3  0x8    /* mask bit 3 */
#define DEPACK_INTR1_STAT4  0x10   /* mask bit 4 */
#define DEPACK_INTR1_STAT5  0x20   /* mask bit 5 */
#define DEPACK_INTR1_STAT6  0x40   /* mask bit 6 */
#define DEPACK_INTR1_STAT7  0x80   /* mask bit 7 */
#define DEPACK_INTR1_STAT8  0x100  /* mask bit 8 */
#define DEPACK_INTR1_STAT9  0x200  /* mask bit 9 */
#define DEPACK_INTR1_STAT10 0x400  /* mask bit 10 */
#define DEPACK_INTR1_STAT11 0x800  /* mask bit 11 */
#define DEPACK_INTR1_STAT12 0x1000 /* mask bit 12 */
#define DEPACK_INTR1_STAT13 0x2000 /* mask bit 13 */
#define DEPACK_INTR1_STAT14 0x4000 /* mask bit 14 */
#define DEPACK_INTR1_STAT15 0x8000 /* mask bit 15 */

#define REG_DEPACK_INTR2_MASK 0x13D4
#define DEPACK_INTR2_STAT0_MASK  0x1     /* mask bit 0 */
#define DEPACK_INTR2_STAT1_MASK  0x2     /* mask bit 1 */
#define DEPACK_INTR2_STAT2_MASK  0x4     /* mask bit 2 */
#define DEPACK_INTR2_STAT3_MASK  0x8     /* mask bit 3 */
#define DEPACK_INTR2_STAT4_MASK  0x10    /* mask bit 4 */
#define DEPACK_INTR2_STAT5_MASK  0x20    /* mask bit 5 */
#define DEPACK_INTR2_STAT6_MASK  0x40    /* mask bit 6 */
#define DEPACK_INTR2_STAT7_MASK  0x80    /* mask bit 7 */
#define DEPACK_INTR2_STAT8_MASK  0x100   /* mask bit 8 */
#define DEPACK_INTR2_STAT9_MASK  0x200   /* mask bit 9 */
#define DEPACK_INTR2_STAT10_MASK 0x400   /* mask bit 10 */
#define DEPACK_INTR2_STAT11_MASK 0x800   /* mask bit 11 */
#define DEPACK_INTR2_STAT12_MASK 0x1000  /* mask bit 12 */
#define DEPACK_INTR2_STAT13_MASK 0x2000  /* mask bit 13 */
#define DEPACK_INTR2_STAT14_MASK 0x4000  /* mask bit 14 */
#define DEPACK_INTR2_STAT15_MASK 0x8000  /* mask bit 15 */
#define DEPACK_INTR2_STAT16_MASK 0x10000 /* mask bit 16 */
#define DEPACK_INTR2_STAT17_MASK 0x20000 /* mask bit 17 */
#define DEPACK_INTR2_STAT18_MASK 0x40000 /* mask bit 18 */

#define REG_DEPACK_INTR2 0x13D8
#define DEPACK_INTR2_STAT0  0x1     /* mask bit 0 */
#define DEPACK_INTR2_STAT1  0x2     /* mask bit 1 */
#define DEPACK_INTR2_STAT2  0x4     /* mask bit 2 */
#define DEPACK_INTR2_STAT3  0x8     /* mask bit 3 */
#define DEPACK_INTR2_STAT4  0x10    /* mask bit 4 */
#define DEPACK_INTR2_STAT5  0x20    /* mask bit 5 */
#define DEPACK_INTR2_STAT6  0x40    /* mask bit 6 */
#define DEPACK_INTR2_STAT7  0x80    /* mask bit 7 */
#define DEPACK_INTR2_STAT8  0x100   /* mask bit 8 */
#define DEPACK_INTR2_STAT9  0x200   /* mask bit 9 */
#define DEPACK_INTR2_STAT10 0x400   /* mask bit 10 */
#define DEPACK_INTR2_STAT11 0x800   /* mask bit 11 */
#define DEPACK_INTR2_STAT12 0x1000  /* mask bit 12 */
#define DEPACK_INTR2_STAT13 0x2000  /* mask bit 13 */
#define DEPACK_INTR2_STAT14 0x4000  /* mask bit 14 */
#define DEPACK_INTR2_STAT15 0x8000  /* mask bit 15 */
#define DEPACK_INTR2_STAT16 0x10000 /* mask bit 16 */
#define DEPACK_INTR2_STAT17 0x20000 /* mask bit 17 */
#define DEPACK_INTR2_STAT18 0x40000 /* mask bit 18 */

#define REG_DEPACK_INTR3_MASK 0x13DC
#define DEPACK_INTR3_STAT0_MASK  0x1     /* mask bit 0 */
#define DEPACK_INTR3_STAT1_MASK  0x2     /* mask bit 1 */
#define DEPACK_INTR3_STAT2_MASK  0x4     /* mask bit 2 */
#define DEPACK_INTR3_STAT3_MASK  0x8     /* mask bit 3 */
#define DEPACK_INTR3_STAT4_MASK  0x10    /* mask bit 4 */
#define DEPACK_INTR3_STAT5_MASK  0x20    /* mask bit 5 */
#define DEPACK_INTR3_STAT6_MASK  0x40    /* mask bit 6 */
#define DEPACK_INTR3_STAT7_MASK  0x80    /* mask bit 7 */
#define DEPACK_INTR3_STAT8_MASK  0x100   /* mask bit 8 */
#define DEPACK_INTR3_STAT9_MASK  0x200   /* mask bit 9 */
#define DEPACK_INTR3_STAT10_MASK 0x400   /* mask bit 10 */
#define DEPACK_INTR3_STAT11_MASK 0x800   /* mask bit 11 */
#define DEPACK_INTR3_STAT12_MASK 0x1000  /* mask bit 12 */
#define DEPACK_INTR3_STAT13_MASK 0x2000  /* mask bit 13 */
#define DEPACK_INTR3_STAT14_MASK 0x4000  /* mask bit 14 */
#define DEPACK_INTR3_STAT15_MASK 0x8000  /* mask bit 15 */
#define DEPACK_INTR3_STAT16_MASK 0x10000 /* mask bit 16 */
#define DEPACK_INTR3_STAT17_MASK 0x20000 /* mask bit 17 */
#define DEPACK_INTR3_STAT18_MASK 0x40000 /* mask bit 18 */

#define REG_DEPACK_INTR3 0x13E0
#define DEPACK_INTR3_STAT0  0x1     /* mask bit 0 */
#define DEPACK_INTR3_STAT1  0x2     /* mask bit 1 */
#define DEPACK_INTR3_STAT2  0x4     /* mask bit 2 */
#define DEPACK_INTR3_STAT3  0x8     /* mask bit 3 */
#define DEPACK_INTR3_STAT4  0x10    /* mask bit 4 */
#define DEPACK_INTR3_STAT5  0x20    /* mask bit 5 */
#define DEPACK_INTR3_STAT6  0x40    /* mask bit 6 */
#define DEPACK_INTR3_STAT7  0x80    /* mask bit 7 */
#define DEPACK_INTR3_STAT8  0x100   /* mask bit 8 */
#define DEPACK_INTR3_STAT9  0x200   /* mask bit 9 */
#define DEPACK_INTR3_STAT10 0x400   /* mask bit 10 */
#define DEPACK_INTR3_STAT11 0x800   /* mask bit 11 */
#define DEPACK_INTR3_STAT12 0x1000  /* mask bit 12 */
#define DEPACK_INTR3_STAT13 0x2000  /* mask bit 13 */
#define DEPACK_INTR3_STAT14 0x4000  /* mask bit 14 */
#define DEPACK_INTR3_STAT15 0x8000  /* mask bit 15 */
#define DEPACK_INTR3_STAT16 0x10000 /* mask bit 16 */
#define DEPACK_INTR3_STAT17 0x20000 /* mask bit 17 */
#define DEPACK_INTR3_STAT18 0x40000 /* mask bit 18 */

#define REG_DEPACK_INTR4_MASK 0x13E4
#define DEPACK_INTR4_STAT0_MASK  0x1    /* mask bit 0 */
#define DEPACK_INTR4_STAT1_MASK  0x2    /* mask bit 1 */
#define DEPACK_INTR4_STAT2_MASK  0x4    /* mask bit 2 */
#define DEPACK_INTR4_STAT3_MASK  0x8    /* mask bit 3 */
#define DEPACK_INTR4_STAT4_MASK  0x10   /* mask bit 4 */
#define DEPACK_INTR4_STAT5_MASK  0x20   /* mask bit 5 */
#define DEPACK_INTR4_STAT6_MASK  0x40   /* mask bit 6 */
#define DEPACK_INTR4_STAT7_MASK  0x80   /* mask bit 7 */
#define DEPACK_INTR4_STAT8_MASK  0x100  /* mask bit 8 */
#define DEPACK_INTR4_STAT9_MASK  0x200  /* mask bit 9 */
#define DEPACK_INTR4_STAT10_MASK 0x400  /* mask bit 10 */
#define DEPACK_INTR4_STAT11_MASK 0x800  /* mask bit 11 */
#define DEPACK_INTR4_STAT12_MASK 0x1000 /* mask bit 12 */
#define DEPACK_INTR4_STAT13_MASK 0x2000 /* mask bit 13 */
#define DEPACK_INTR4_STAT14_MASK 0x4000 /* mask bit 14 */

#define REG_DEPACK_INTR4 0x13E8
#define DEPACK_INTR4_STAT0  0x1    /* mask bit 0 */
#define DEPACK_INTR4_STAT1  0x2    /* mask bit 1 */
#define DEPACK_INTR4_STAT2  0x4    /* mask bit 2 */
#define DEPACK_INTR4_STAT3  0x8    /* mask bit 3 */
#define DEPACK_INTR4_STAT4  0x10   /* mask bit 4 */
#define DEPACK_INTR4_STAT5  0x20   /* mask bit 5 */
#define DEPACK_INTR4_STAT6  0x40   /* mask bit 6 */
#define DEPACK_INTR4_STAT7  0x80   /* mask bit 7 */
#define DEPACK_INTR4_STAT8  0x100  /* mask bit 8 */
#define DEPACK_INTR4_STAT9  0x200  /* mask bit 9 */
#define DEPACK_INTR4_STAT10 0x400  /* mask bit 10 */
#define DEPACK_INTR4_STAT11 0x800  /* mask bit 11 */
#define DEPACK_INTR4_STAT12 0x1000 /* mask bit 12 */
#define DEPACK_INTR4_STAT13 0x2000 /* mask bit 13 */
#define DEPACK_INTR4_STAT14 0x4000 /* mask bit 14 */

#define REG_DEPACK_INTR5_MASK 0x13EC
#define DEPACK_INTR5_STAT0_MASK 0x1   /* mask bit 0 */
#define DEPACK_INTR5_STAT1_MASK 0x2   /* mask bit 1 */
#define DEPACK_INTR5_STAT2_MASK 0x4   /* mask bit 2 */
#define DEPACK_INTR5_STAT3_MASK 0x8   /* mask bit 3 */
#define DEPACK_INTR5_STAT4_MASK 0x10  /* mask bit 4 */
#define DEPACK_INTR5_STAT5_MASK 0x20  /* mask bit 5 */
#define DEPACK_INTR5_STAT6_MASK 0x40  /* mask bit 6 */
#define DEPACK_INTR5_STAT7_MASK 0x80  /* mask bit 7 */
#define DEPACK_INTR5_STAT8_MASK 0x100 /* mask bit 8 */

#define REG_DEPACK_INTR5 0x13F0
#define DEPACK_INTR5_STAT0 0x1   /* mask bit 0 */
#define DEPACK_INTR5_STAT1 0x2   /* mask bit 1 */
#define DEPACK_INTR5_STAT2 0x4   /* mask bit 2 */
#define DEPACK_INTR5_STAT3 0x8   /* mask bit 3 */
#define DEPACK_INTR5_STAT4 0x10  /* mask bit 4 */
#define DEPACK_INTR5_STAT5 0x20  /* mask bit 5 */
#define DEPACK_INTR5_STAT6 0x40  /* mask bit 6 */
#define DEPACK_INTR5_STAT7 0x80  /* mask bit 7 */
#define DEPACK_INTR5_STAT8 0x100 /* mask bit 8 */

#define REG_EMP_INTR1_MASK 0x13F4
#define EMP_INTR1_STAT0_MASK  0x1        /* mask bit 0 */
#define EMP_INTR1_STAT1_MASK  0x2        /* mask bit 1 */
#define EMP_INTR1_STAT2_MASK  0x4        /* mask bit 2 */
#define EMP_INTR1_STAT3_MASK  0x8        /* mask bit 3 */
#define EMP_INTR1_STAT4_MASK  0x10       /* mask bit 4 */
#define EMP_INTR1_STAT5_MASK  0x20       /* mask bit 5 */
#define EMP_INTR1_STAT6_MASK  0x40       /* mask bit 6 */
#define EMP_INTR1_STAT7_MASK  0x80       /* mask bit 7 */
#define EMP_INTR1_STAT8_MASK  0x100      /* mask bit 8 */
#define EMP_INTR1_STAT9_MASK  0x200      /* mask bit 9 */
#define EMP_INTR1_STAT10_MASK 0x400      /* mask bit 10 */
#define EMP_INTR1_STAT11_MASK 0x800      /* mask bit 11 */
#define EMP_INTR1_STAT12_MASK 0x1000     /* mask bit 12 */
#define EMP_INTR1_STAT13_MASK 0x2000     /* mask bit 13 */
#define EMP_INTR1_STAT14_MASK 0x4000     /* mask bit 14 */
#define EMP_INTR1_STAT15_MASK 0x8000     /* mask bit 15 */
#define EMP_INTR1_STAT16_MASK 0x10000    /* mask bit 16 */
#define EMP_INTR1_STAT17_MASK 0x20000    /* mask bit 17 */
#define EMP_INTR1_STAT18_MASK 0x40000    /* mask bit 18 */
#define EMP_INTR1_STAT19_MASK 0x80000    /* mask bit 19 */
#define EMP_INTR1_STAT20_MASK 0x100000   /* mask bit 20 */
#define EMP_INTR1_STAT21_MASK 0x200000   /* mask bit 21 */
#define EMP_INTR1_STAT22_MASK 0x400000   /* mask bit 22 */
#define EMP_INTR1_STAT23_MASK 0x800000   /* mask bit 23 */
#define EMP_INTR1_STAT24_MASK 0x1000000  /* mask bit 24 */
#define EMP_INTR1_STAT25_MASK 0x2000000  /* mask bit 25 */
#define EMP_INTR1_STAT26_MASK 0x4000000  /* mask bit 26 */
#define EMP_INTR1_STAT27_MASK 0x8000000  /* mask bit 27 */
#define EMP_INTR1_STAT28_MASK 0x10000000 /* mask bit 28 */

#define REG_EMP_INTR1 0x13F8
#define EMP_INTR1_STAT0  0x1        /* mask bit 0 */
#define EMP_INTR1_STAT1  0x2        /* mask bit 1 */
#define EMP_INTR1_STAT2  0x4        /* mask bit 2 */
#define EMP_INTR1_STAT3  0x8        /* mask bit 3 */
#define EMP_INTR1_STAT4  0x10       /* mask bit 4 */
#define EMP_INTR1_STAT5  0x20       /* mask bit 5 */
#define EMP_INTR1_STAT6  0x40       /* mask bit 6 */
#define EMP_INTR1_STAT7  0x80       /* mask bit 7 */
#define EMP_INTR1_STAT8  0x100      /* mask bit 8 */
#define EMP_INTR1_STAT9  0x200      /* mask bit 9 */
#define EMP_INTR1_STAT10 0x400      /* mask bit 10 */
#define EMP_INTR1_STAT11 0x800      /* mask bit 11 */
#define EMP_INTR1_STAT12 0x1000     /* mask bit 12 */
#define EMP_INTR1_STAT13 0x2000     /* mask bit 13 */
#define EMP_INTR1_STAT14 0x4000     /* mask bit 14 */
#define EMP_INTR1_STAT15 0x8000     /* mask bit 15 */
#define EMP_INTR1_STAT16 0x10000    /* mask bit 16 */
#define EMP_INTR1_STAT17 0x20000    /* mask bit 17 */
#define EMP_INTR1_STAT18 0x40000    /* mask bit 18 */
#define EMP_INTR1_STAT19 0x80000    /* mask bit 19 */
#define EMP_INTR1_STAT20 0x100000   /* mask bit 20 */
#define EMP_INTR1_STAT21 0x200000   /* mask bit 21 */
#define EMP_INTR1_STAT22 0x400000   /* mask bit 22 */
#define EMP_INTR1_STAT23 0x800000   /* mask bit 23 */
#define EMP_INTR1_STAT24 0x1000000  /* mask bit 24 */
#define EMP_INTR1_STAT25 0x2000000  /* mask bit 25 */
#define EMP_INTR1_STAT26 0x4000000  /* mask bit 26 */
#define EMP_INTR1_STAT27 0x8000000  /* mask bit 27 */
#define EMP_INTR1_STAT28 0x10000000 /* mask bit 28 */

#define REG_EMP_INTR2_MASK 0x13FC
#define EMP_INTR2_STAT0_MASK  0x1        /* mask bit 0 */
#define EMP_INTR2_STAT1_MASK  0x2        /* mask bit 1 */
#define EMP_INTR2_STAT2_MASK  0x4        /* mask bit 2 */
#define EMP_INTR2_STAT3_MASK  0x8        /* mask bit 3 */
#define EMP_INTR2_STAT4_MASK  0x10       /* mask bit 4 */
#define EMP_INTR2_STAT5_MASK  0x20       /* mask bit 5 */
#define EMP_INTR2_STAT6_MASK  0x40       /* mask bit 6 */
#define EMP_INTR2_STAT7_MASK  0x80       /* mask bit 7 */
#define EMP_INTR2_STAT8_MASK  0x100      /* mask bit 8 */
#define EMP_INTR2_STAT9_MASK  0x200      /* mask bit 9 */
#define EMP_INTR2_STAT10_MASK 0x400      /* mask bit 10 */
#define EMP_INTR2_STAT11_MASK 0x800      /* mask bit 11 */
#define EMP_INTR2_STAT12_MASK 0x1000     /* mask bit 12 */
#define EMP_INTR2_STAT13_MASK 0x2000     /* mask bit 13 */
#define EMP_INTR2_STAT14_MASK 0x4000     /* mask bit 14 */
#define EMP_INTR2_STAT15_MASK 0x8000     /* mask bit 15 */
#define EMP_INTR2_STAT16_MASK 0x10000    /* mask bit 16 */
#define EMP_INTR2_STAT17_MASK 0x20000    /* mask bit 17 */
#define EMP_INTR2_STAT18_MASK 0x40000    /* mask bit 18 */
#define EMP_INTR2_STAT19_MASK 0x80000    /* mask bit 19 */
#define EMP_INTR2_STAT20_MASK 0x100000   /* mask bit 20 */
#define EMP_INTR2_STAT21_MASK 0x200000   /* mask bit 21 */
#define EMP_INTR2_STAT22_MASK 0x400000   /* mask bit 22 */
#define EMP_INTR2_STAT23_MASK 0x800000   /* mask bit 23 */
#define EMP_INTR2_STAT24_MASK 0x1000000  /* mask bit 24 */
#define EMP_INTR2_STAT25_MASK 0x2000000  /* mask bit 25 */
#define EMP_INTR2_STAT26_MASK 0x4000000  /* mask bit 26 */
#define EMP_INTR2_STAT27_MASK 0x8000000  /* mask bit 27 */
#define EMP_INTR2_STAT28_MASK 0x10000000 /* mask bit 28 */
#define EMP_INTR2_STAT29_MASK 0x20000000 /* mask bit 29 */

#define REG_EMP_INTR2 0x1400
#define EMP_INTR2_STAT0  0x1        /* mask bit 0 */
#define EMP_INTR2_STAT1  0x2        /* mask bit 1 */
#define EMP_INTR2_STAT2  0x4        /* mask bit 2 */
#define EMP_INTR2_STAT3  0x8        /* mask bit 3 */
#define EMP_INTR2_STAT4  0x10       /* mask bit 4 */
#define EMP_INTR2_STAT5  0x20       /* mask bit 5 */
#define EMP_INTR2_STAT6  0x40       /* mask bit 6 */
#define EMP_INTR2_STAT7  0x80       /* mask bit 7 */
#define EMP_INTR2_STAT8  0x100      /* mask bit 8 */
#define EMP_INTR2_STAT9  0x200      /* mask bit 9 */
#define EMP_INTR2_STAT10 0x400      /* mask bit 10 */
#define EMP_INTR2_STAT11 0x800      /* mask bit 11 */
#define EMP_INTR2_STAT12 0x1000     /* mask bit 12 */
#define EMP_INTR2_STAT13 0x2000     /* mask bit 13 */
#define EMP_INTR2_STAT14 0x4000     /* mask bit 14 */
#define EMP_INTR2_STAT15 0x8000     /* mask bit 15 */
#define EMP_INTR2_STAT16 0x10000    /* mask bit 16 */
#define EMP_INTR2_STAT17 0x20000    /* mask bit 17 */
#define EMP_INTR2_STAT18 0x40000    /* mask bit 18 */
#define EMP_INTR2_STAT19 0x80000    /* mask bit 19 */
#define EMP_INTR2_STAT20 0x100000   /* mask bit 20 */
#define EMP_INTR2_STAT21 0x200000   /* mask bit 21 */
#define EMP_INTR2_STAT22 0x400000   /* mask bit 22 */
#define EMP_INTR2_STAT23 0x800000   /* mask bit 23 */
#define EMP_INTR2_STAT24 0x1000000  /* mask bit 24 */
#define EMP_INTR2_STAT25 0x2000000  /* mask bit 25 */
#define EMP_INTR2_STAT26 0x4000000  /* mask bit 26 */
#define EMP_INTR2_STAT27 0x8000000  /* mask bit 27 */
#define EMP_INTR2_STAT28 0x10000000 /* mask bit 28 */
#define EMP_INTR2_STAT29 0x20000000 /* mask bit 29 */

#define REG_EMP_TRANS_STATUS 0x1404
#define FVAVRR_EMP_STATUS    0x1  /* mask bit 0 */
#define GEN_EMP_STATUS       0x2  /* mask bit 1 */
#define VS0_EMP_STATUS       0x4  /* mask bit 2 */
#define VS1_EMP_STATUS       0x8  /* mask bit 3 */
#define DSC_EMP_STATUS       0x10 /* mask bit 4 */
#define HDR_EMP_STATUS       0x20 /* mask bit 5 */
#define DSC_UNREC_EMP_STATUS 0x40 /* mask bit 6 */
#define VI_UNREC_EMP_STATUS  0x80 /* mask bit 7 */
#endif /* __HAL_HDMIRX_DEPACK_REG_H__ */
