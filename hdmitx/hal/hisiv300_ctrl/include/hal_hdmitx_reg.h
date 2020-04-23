/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver hdmi tx general reg header file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#ifndef __HAL_HDMITX_REG_H__
#define __HAL_HDMITX_REG_H__

#define MAX_SUB_PKT_NUM 4
#define MAX_DSC_EMP_NUM 6

/* tx_hdmi_reg module field info */
#define REG_AVI_PKT_HEADER 0x1818
#define REG_AVI_SUB_PKT0_L 0x181C
#define REG_AVI_SUB_PKT0_H 0x1820

#define REG_AIF_PKT_HEADER 0x183C
#define REG_AIF_SUB_PKT0_L 0x1840
#define REG_AIF_SUB_PKT0_H 0x1844

#define REG_SPIF_PKT_HEADER 0x1860
#define REG_SPIF_SUB_PKT0_L 0x1864
#define REG_SPIF_SUB_PKT0_H 0x1868

#define REG_MPEG_PKT_HEADER 0x1884
#define REG_MPEG_SUB_PKT0_L 0x1888
#define REG_MPEG_SUB_PKT0_H 0x188C

#define REG_GEN_PKT_HEADER 0x18A8
#define REG_GEN_SUB_PKT0_L 0x18AC
#define REG_GEN_SUB_PKT0_H 0x18B0

#define REG_GEN2_PKT_HEADER 0x18CC
#define REG_GEN2_SUB_PKT0_L 0x18D0
#define REG_GEN2_SUB_PKT0_H 0x18D4

#define REG_GEN3_PKT_HEADER 0x18F0
#define REG_GEN3_SUB_PKT0_L 0x18F4
#define REG_GEN3_SUB_PKT0_H 0x18F8

#define REG_GEN4_PKT_HEADER 0x1914
#define REG_GEN4_SUB_PKT0_L 0x1918
#define REG_GEN4_SUB_PKT0_H 0x191C

#define REG_GEN5_PKT_HEADER 0x1938
#define REG_GEN5_SUB_PKT0_L 0x193C
#define REG_GEN5_SUB_PKT0_H 0x1940

#define REG_GAMUT_PKT_HEADER 0x195C
#define REG_GAMUT_SUB_PKT0_L 0x1960
#define REG_GAMUT_SUB_PKT0_H 0x1964

#define REG_VSIF_PKT_HEADER 0x1980
#define REG_VSIF_SUB_PKT0_L 0x1984
#define REG_VSIF_SUB_PKT0_H 0x1988

#define reg_sub_pkt_hb2(x) (((x) & 0xff) << 16)
#define reg_sub_pkt_hb1(x) (((x) & 0xff) << 8)
#define reg_sub_pkt_hb0(x) (((x) & 0xff) << 0)

#define reg_sub_pktx_pb3(x) (((x) & 0xff) << 24)
#define reg_sub_pktx_pb2(x) (((x) & 0xff) << 16)
#define reg_sub_pktx_pb1(x) (((x) & 0xff) << 8)
#define reg_sub_pktx_pb0(x) (((x) & 0xff) << 0)

#define reg_sub_pktx_pb6(x) (((x) & 0xff) << 16)
#define reg_sub_pktx_pb5(x) (((x) & 0xff) << 8)
#define reg_sub_pktx_pb4(x) (((x) & 0xff) << 0)

#define REG_CEA_AVI_CFG   0x19A4
#define REG_CEA_SPF_CFG   0x19A8
#define REG_CEA_AUD_CFG   0x19AC
#define REG_CEA_MPEG_CFG  0x19B0
#define REG_CEA_GEN_CFG   0x19B4
#define REG_CEA_CP_CFG    0x19B8
#define REG_CEA_GEN2_CFG  0x19BC
#define REG_CEA_GEN3_CFG  0x19C0
#define REG_CEA_GEN4_CFG  0x19C4
#define REG_CEA_GEN5_CFG  0x19C8
#define REG_CEA_GAMUT_CFG 0x19CC
#define REG_CEA_VSIF_CFG  0x19D0

#define reg_rpt_cnt(x) (((x) & 0x3ff) << 2)
#define REG_RPT_CNT_M  (0x3ff << 2)
#define reg_rpt_en(x)  (((x) & 1) << 1)
#define REG_RPT_EN_M   (1 << 1)
#define reg_en(x)      (((x) & 1) << 0)
#define REG_EN_M       (1 << 0)

#define REG_CEA_PKT_STATE    0x19E0
#define REG_CEA_DSC_STATE    (1 << 13)
#define REG_CEA_FVAVRR_STATE (1 << 12)
#define REG_CEA_VSIF_STATE   (1 << 11)
#define REG_CEA_GAMUT_STATE  (1 << 10)
#define REG_CEA_MPEG_STATE   (1 << 9)
#define REG_CEA_SPD_STATE    (1 << 8)
#define REG_CEA_GEN5_STATE   (1 << 7)
#define REG_CEA_GEN4_STATE   (1 << 6)
#define REG_CEA_GEN3_STATE   (1 << 5)
#define REG_CEA_GEN2_STATE   (1 << 4)
#define REG_CEA_GEN_STATE    (1 << 3)
#define REG_CEA_CP_STATE     (1 << 2)
#define REG_CEA_AUD_STATE    (1 << 1)
#define REG_CEA_AVI_STATE    (1 << 0)

#define REG_CP_PKT_PHASE      0x1A00
#define reg_phase_sw_value(x) (((x) & 0x3) << 2)
#define REG_PHASE_SW_VALUE_M  (0x3 << 2)
#define reg_phase_ovr_en(x)   (((x) & 1) << 1)
#define REG_PHASE_OVR_EN_M    (1 << 1)
#define reg_default_phase(x)  (((x) & 1) << 0)
#define REG_DEFAULT_PHASE_M   (1 << 0)

#define REG_DSC_HSYNC_CFG      0x1A04
#define reg_cfg_hsync_ovr(x)   (((x) & 1) << 1)
#define REG_CFG_HSYNC_OVR_M    (1 << 1)
#define reg_cfg_hsync_value(x) (((x) & 1) << 0)
#define REG_CFG_HSYNC_VALUE_M  (1 << 0)

#define REG_AVMIXER_CONFIG         0x1A08
#define reg_cfg_eess_mode_en(x)    (((x) & 1) << 12)
#define REG_CFG_EESS_MODE_EN_M     (1 << 12)
#define reg_cfg_hdmi_dvi_sel(x)    (((x) & 1) << 11)
#define REG_CFG_HDMI_DVI_SEL_M     (1 << 11)
#define reg_cfg_frl_mode(x)        (((x) & 1) << 10)
#define REG_CFG_FRL_MODE_M         (1 << 10)
#define reg_audio_prioroty_ctl(x)  (((x) & 1) << 9)
#define REG_AUDIO_PRIORITY_CTL_M   (1 << 9)
#define reg_avmute_in_phase(x)     (((x) & 1) << 8)
#define REG_AVMUTE_IN_PHASE_M      (1 << 8)
#define reg_pkt_bypass_mode(x)     (((x) & 1) << 7)
#define REG_PKT_BYPASS_MODE_M      (1 << 7)
#define reg_pb_priority_ctl(x)     (((x) & 1) << 6)
#define REG_PB_PRIOTITY_CTL_M      (1 << 6)
#define reg_pb_ovr_dc_pkt_en(x)    (((x) & 1) << 5)
#define REG_PB_OVR_DC_PKT_EN_M     (1 << 5)
#define reg_intr_encryption(x)     (((x) & 1) << 4)
#define REG_INTR_ENCRYPTION_M      (1 << 4)
#define reg_null_pkt_en_vs_high(x) (((x) & 1) << 3)
#define REG_NULL_PKT_EN_VS_HIGH_M  (1 << 3)
#define reg_null_pkt_en(x)         (((x) & 1) << 2)
#define REG_NULL_PKT_EN_M          (1 << 2)
#define reg_dc_pkt_en(x)           (((x) & 1) << 1)
#define REG_DC_PKT_EN_M            (1 << 1)
#define reg_hdmi_mode(x)           (((x) & 1) << 0)
#define REG_HDMI_MODE_M            (1 << 0)

#define REG_CP_PKT_AVMUTE    0x1A0C
#define reg_cp_clr_avmute(x) (((x) & 1) << 1)
#define REG_CP_CLR_AVMUTE_M  (1 << 1)
#define reg_cp_set_avmute(x) (((x) & 1) << 0)
#define REG_CP_SET_AVMUTE_M  (1 << 0)

#define REG_VIDEO_BLANK_CFG 0x1A10
#define reg_video_blank(x)  (((x) & 0xffffff) << 0)
#define REG_VIDEO_BLANK_M   (0xffffff << 0)

#define REG_TMDS_BIST_CTRL      0x1A54
#define reg_tbist_patt_sel(x)   (((x) & 0x1f) << 9)
#define REG_TBIST_PATT_SEL_M    (0x1f << 9)
#define reg_tbist_timing_sel(x) (((x) & 0x3f) << 3)
#define REG_TBIST_TIMING_SEL_M  (0x3f << 3)
#define reg_tbist_syn_pol(x)    (((x) & 0x3) << 1)
#define REG_TBIST_SYN_POL_M     (0x3 << 1)
#define reg_tbist_en(x)         (((x) & 1) << 0)
#define REG_TBIST_EN_M          (1 << 0)

#define REG_HDMI_ENC_CTRL     0x1A60
#define reg_enc_ck_div_sel(x) (((x) & 0x3) << 6)
#define REG_ENC_CK_DIV_SEL_M  (0x3 << 6)
#define reg_enc_bupass(x)     (((x) & 1) << 5)
#define REG_ENC_BYPASS_M      (1 << 5)
#define reg_enc_hdmi_ovr(x)   (((x) & 1) << 4)
#define REG_ENC_HDMI_OVR_M    (1 << 4)
#define reg_enc_hdmi_val(x)   (((x) & 1) << 3)
#define REG_ENC_HDMI_VAL_M    (1 << 3)
#define reg_enc_scr_md(x)     (((x) & 1) << 2)
#define REG_ENC_SCR_MD_M      (1 << 2)
#define reg_enc_scr_on(x)     (((x) & 1) << 1)
#define REG_ENC_SCR_ON_M      (1 << 1)
#define reg_enc_hdmi2_on(x)   (((x) & 1) << 0)
#define REG_ENC_HDMI2_ON_M    (1 << 0)

#define REG_ENC_CK_SHARP     0x1A64
#define reg_enc_ck_sharp2(x) (((x) & 0x3ff) << 20)
#define REG_ENC_CK_SHARP2_M  (0x3ff << 20)
#define reg_enc_ck_sharp1(x) (((x) & 0x3ff) << 10)
#define REG_ENC_CK_SHARP1_M  (0x3ff << 10)
#define reg_enc_ck_sharp0(x) (((x) & 0x3ff) << 0)
#define REG_ENC_CK_SHARP0_M  (0x3ff << 0)

#define reg_dsc_em_pkt_header(n)    (0x1A80 + +(n) * 0x24)
#define reg_dsc_em_packet_sub0_l(n) (0x1A84 + (n) * 0x24)
#define reg_dsc_em_packet_sub0_h(n) (0x1A88 + (n) * 0x24)

#define REG_CEA_DSC_CONTROL 0x1B58

#define REG_CTL_TYPE_CONFIG 0x1B78
#define reg_type_config(x)  (((x) & 0x3) << 0)
#define REG_TYPE_CONFIG_M   (0x3 << 0)

#define REG_KEEPOUT_WIN_CFG          0x1B7C
#define reg_cfg_keepout_win_end(x)   (((x) & 0x3ff) << 12)
#define REG_CFG_KEEPOUT_WIN_END_M    (0x3ff << 12)
#define reg_cfg_keepout_win_start(x) (((x) & 0x3ff) << 0)
#define REG_CFG_KEEPOUT_WIN_START_M  (0x3ff << 0)

#define REG_PKT_WIN_DE_CFG     0x1B80
#define reg_cfg_de_pre_sel(x)  (((x) & 0x3) << 8)
#define REG_CFG_DE_PRE_SEL_M   (0x3 << 8)
#define reg_cfg_de_post_ovr(x) (((x) & 1) << 6)
#define REG_CFG_DE_POST_OVR_M  (1 << 6)
#define reg_cfg_de_post_cnt(x) (((x) & 0x3f) << 0)
#define REG_CFG_DE_POST_CNT_M  (0x3f << 0)

#define REG_VSYNC_IND_CFG        0x1B84
#define reg_cfg_vsync_ind_end(x) (((x) & 0x7ff) << 0)
#define REG_CFG_VSYNC_IND_END_M  (0x7ff << 0)

#define REG_TX_METADATA_CTRL_ARST_REQ 0x1B9C
#define reg_tx_metadata_arst_req(x)   (((x) & 1) << 0)
#define REG_TX_METADATA_ARST_REQ_M    (1 << 0)

#define REG_TX_METADATA_CTRL      0x1BA0
#define reg_txmeta_vdp_path_en(x) (((x) & 1) << 25)
#define REG_TXMETA_VDP_PATH_EN_M  (1 << 25)

#define reg_txmeta_vdp_smd_en(x) (((x) & 1) << 24)
#define REG_TXMETA_VDP_SMD_EN_M  (1 << 24)

#define reg_txmeta_vdp_avi_en(x) (((x) & 1) << 23)
#define REG_TXMETA_VDP_AVI_EN_M  (1 << 23)

#define reg_txmeta_fapa_start_line(x) (((x) & 0xffff) << 0)
#define REG_TXMETA_FAPA_START_LINE_M  (0xffff << 0)

#define REG_VBALNK_HW           0x1BA4
#define reg_txmeta_vblank_hw(x) (((x) & 0xffff) << 0)
#define REG_TXMETA_VBLANK_HW_M  (0xffff << 0)

#define REG_FAPA_PKT_SUM         0x1BA8
#define reg_txmeta_raddr_fapa(x) (((x) & 0x3f) << 6)
#define REG_TXMETA_RADDR_FAPA_M  (0x3f << 6)
#define reg_txmeta_waddr_fapa(x) (((x) & 0x3f) << 0)
#define REG_TXMETA_WADDR_FAPA_M  (0x3f << 0)

#define REG_AVI_STATUS_0 0x1BAC
#define REG_AVI_STATUS_1 0x1BB0
#define REG_SMD_STATUS_0 0x1BB4
#define REG_SMD_STATUS_1 0x1BB8

#define REG_FAPA_LAST_CFG            0x1BBC
#define reg_txmeta_fapa_ovr(x)       (((x) & 1) << 16)
#define REG_TXMETA_FAPA_OVR_M        (1 << 16)
#define reg_txmeta_fapa_last_line(x) (((x) & 0xffff) << 0)
#define REG_TXMETA_FAPA_LAST_LINE_M  (0xffff << 0)

#define REG_HDCP_KEEPOUT              0x1BC0
#define reg_cfg_hdcp_keepout_end(x)   (((x) & 0x3ff) << 12)
#define REG_CFG_HDCP_KEEPOUT_END_M    (0x3ff << 12)
#define reg_cfg_hdcp_keepout_start(x) (((x) & 0x3ff) << 0)
#define REG_CFG_HDCP_KEEPOUT_START_M  (0x3ff << 0)

#define REG_DSC_EMP_WIN_CFG      0x1BC4
#define reg_cfg_dsc_win_end(x)   (((x) & 0xffff) << 16)
#define REG_CFG_DSC_WIN_END_M    (0xffff << 16)
#define reg_cfg_dsc_win_start(x) (((x) & 0xffff) << 0)
#define REG_CFG_DSC_WIN_START_M  (0xffff << 0)

#endif

