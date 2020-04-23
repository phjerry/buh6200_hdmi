/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019.All rights reserved.
 * Description: hdmi driver audo path reg header file
 * Author:  Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#ifndef __HAL_HDMITX_AUDIO_PATH_REG_H__
#define __HAL_HDMITX_AUDIO_PATH_REG_H__

/* audio_path module field info */
#define REG_TX_AUDIO_CTRL             0x1000
#define reg_cfg_cuv_sel(x)            (((x) & 1) << 22) /* [22] */
#define REG_CFG_CUV_SEL_M             (1 << 22)
#define reg_cfg_audio_sf_en(x)        (((x) & 1) << 21) /* [21] */
#define REG_CFG_AUDIO_SF_EN_M         (1 << 21)
#define reg_cfg_audio_data_mute_en(x) (((x) & 1) << 20) /* [20] */
#define REG_CFG_AUDIO_DATA_MUTE_EN_M  (1 << 20)
#define reg_aud_fifo3_map(x)          (((x) & 0x3) << 18) /* [19:18] */
#define REG_AUD_FIFO3_MAP_M           (0x3 << 18)
#define reg_aud_fifo2_map(x)          (((x) & 0x3) << 16) /* [17:16] */
#define REG_AUD_FIFO2_MAP_M           (0x3 << 16)
#define reg_aud_fifo1_map(x)          (((x) & 0x3) << 14) /* [15:14] */
#define REG_AUD_FIFO1_MAP_M           (0x3 << 14)
#define reg_aud_fifo0_map(x)          (((x) & 0x3) << 12) /* [13:12] */
#define REG_AUD_FIFO0_MAP_M           (0x3 << 12)
#define reg_aud_src_ctrl(x)           (((x) & 1) << 10) /* [10] */
#define REG_AUD_SRC_CTRL_M            (1 << 10)
#define reg_aud_src_en(x)             (((x) & 1) << 9) /* [9] */
#define REG_AUD_SRC_EN_M              (1 << 9)
#define reg_aud_spdif_en(x)           (((x) & 1) << 8) /* [8] */
#define REG_AUD_SPDIF_EN_M            (1 << 8)
#define reg_aud_i2s_en(x)             (((x) & 0xf) << 4) /* [7:4] */
#define REG_AUD_I2S_EN_M              (0xf << 4)
#define reg_aud_layout(x)             (((x) & 1) << 2) /* [2] */
#define REG_AUD_LAYOUT_M              (1 << 2)
#define reg_aud_mute_en(x)            (((x) & 1) << 1) /* [1] */
#define REG_AUD_MUTE_EN_M             (1 << 1)
#define reg_aud_in_en(x)              (((x) & 1) << 0) /* [0] */
#define REG_AUD_IN_EN_M               (1 << 0)

#define REG_AUD_I2S_CTRL       0x1004
#define reg_i2s_ch_swap(x)     (((x) & 0xf) << 12) /* [15:12] */
#define REG_I2S_CH_SWAP_M      (0xf << 12)
#define reg_i2s_length(x)      (((x) & 0xf) << 8) /* [11:8] */
#define REG_I2S_LENGTH_M       (0xf << 8)
#define reg_i2s_vbit(x)        (((x) & 1) << 5) /* [5] */
#define REG_I2S_VBIT_M         (1 << 5)
#define reg_i2s_data_dir(x)    (((x) & 1) << 4) /* [4] */
#define REG_I2S_DATA_DIR_M     (1 << 4)
#define reg_i2s_justify(x)     (((x) & 1) << 3) /* [3] */
#define REG_I2S_JUSTIFY_M      (1 << 3)
#define reg_i2s_ws_polarity(x) (((x) & 1) << 2) /* [2] */
#define REG_I2S_WS_POLARITY_M  (1 << 2)
#define reg_i2s_1st_shift(x)   (((x) & 1) << 1) /* [1] */
#define REG_I2S_1ST_SHIFT_M    (1 << 1)
#define reg_i2s_hbra_on(x)     (((x) & 1) << 0) /* [0] */
#define REG_I2S_HBRA_ON_M      (1 << 0)

#define AUD_SPDIF_CTRL      0x1008
#define reg_spdif_2ui_max(x)    (((x) & 0xff) << 20) /* [27:20] */
#define REG_SPDIF_2UI_MAX_M     (0xff << 20)
#define reg_spdif_1ui_max(x)    (((x) & 0xff) << 12) /* [19:12] */
#define REG_SPDIF_1UI_MAX_M     (0xff << 12)
#define reg_spdif_size_sw(x)    (((x) & 0x3) << 10) /* [11:10] */
#define REG_SPDIF_SIZE_SW_M     (0x3 << 10)
#define reg_spdif_err_thresh(x) (((x) & 0x3f) << 4) /* [9:4] */
#define REG_SPDIF_ERR_THRESH_M  (0x3f << 4)
#define reg_spdif_fs_ovr_en(x)  (((x) & 1) << 3) /* [3] */
#define REG_SPDIF_FS_OVR_EN_M   (1 << 3)
#define reg_i2s_cbit_order(x)   (((x) & 1) << 2) /* [2] */
#define REG_I2S_CBIT_ORDER_M    (1 << 2)
#define reg_spdif_2ui_lock(x)   (((x) & 1) << 1) /* [1] */
#define REG_SPDIF_2UI_LOCK_M    (1 << 1)
#define reg_spdif_1ui_lock(x)   (((x) & 1) << 0) /* [0] */
#define REG_SPDIF_1UI_LOCK_M    (1 << 0)

#define AUD_CHST_CFG0              0x100C
#define reg_chst_byte3_clk_accuracy(x) (((x) & 0x3) << 28)
#define REG_CHST_BYTE3_CLK_ACCURACY_M  (0x3 << 28)
#define reg_chst_byte3_fs(x)           (((x) & 0xf) << 24)
#define REG_CHST_BYTE3_FS_M            (0xf << 24)
#define reg_chst_byte2(x)              (((x) & 0xff) << 16)
#define REG_CHST_BYTE2_M               (0xff << 16)
#define reg_chst_byte1(x)              (((x) & 0xff) << 8)
#define REG_CHST_BYTE1_M               (0xff << 8)
#define reg_chst_byte0_other(x)        (((x) & 0x3f) << 2)
#define REG_CHST_BYTE0_OTHER_M         (0x3f << 2)
#define reg_chst_byte0_b(x)            (((x) & 1) << 1)
#define REG_CHST_BYTE0_B_M             (1 << 1)
#define reg_chst_byte0_a(x)            (((x) & 1) << 0)
#define REG_CHST_BYTE0_A_M             (1 << 0)

#define REG_AUD_CHST_CFG1        0x1010
#define reg_chst_byte6(x)        (((x) & 0xff) << 16)
#define REG_CHST_BYTE6_M         (0xff << 16)
#define reg_chst_byte5(x)        (((x) & 0xff) << 8)
#define REG_CHST_BYTE5_M         (0xff << 8)
#define reg_chst_byte4_org_fs(x) (((x) & 0xf) << 4)
#define REG_CHST_BYTE4_ORG_FS_M  (0xf << 4)
#define reg_chst_byte4_length(x) (((x) & 0xf) << 0)
#define REG_CHST_BYTE4_LENGTH_M  (0xf << 0)

#define REG_TX_AUDIO_STATE      0x1014
#define reg_spdif_max_2ui_st(x) (((x) & 0xff) << 24) /* [31:24] */
#define REG_SPDIF_MAX_2UI_ST_M  (0xff << 24)
#define reg_spdif_max_1ui_st(x) (((x) & 0xff) << 16) /* [23:16] */
#define REG_SPDIF_MAX_1UI_ST_M  (0xff << 16)
#define reg_aud_spdif_fs(x)     (((x) & 0x3f) << 8) /* [13:8] */
#define REG_AUD_SPDIF_FS_M      (0x3f << 8)
#define reg_aud_length(x)       (((x) & 0xf) << 4) /* [7:4] */
#define REG_AUD_LENGTH_M        (0xf << 4)
#define reg_aud_spdif_new_fs(x) (((x) & 1) << 1) /* [1] */
#define REG_AUD_SPDIF_NEW_FS_M  (1 << 1)
#define reg_aud_inavailable(x)  (((x) & 1) << 0) /* [0] */
#define REG_AUD_INAVAILABLE_M   (1 << 0)

#define REG_AUD_FIFO_CTRL        0x1018
#define reg_aud_fifo_ptr_diff(x) (((x) & 0x3f) << 16) /* [21:16] */
#define REG_AUD_FIFO_PTR_DIFF_M  (0x3f << 16)
#define reg_aud_fifo_hbr_mask(x) (((x) & 0x0f) << 8) /* [11:8] */
#define REG_AUD_FIFO_HBR_MASK_M  (0x0f << 8)
#define reg_aud_fifo_test(x)     (((x) & 0x1f) << 0) /* [4:0] */
#define REG_AUD_FIFO_TEST_M      (0x1f << 0)

#define REG_AUD_ACR_CTRL         0x1040
#define reg_acr_cts_ave_en(x)    (((x) & 1) << 5) /* [5] */
#define REG_ACR_CTS_AVE_EN_M     (1 << 5)
#define reg_acr_use_sw_cts(x)    (((x) & 1) << 4) /* [4] */
#define REG_ACR_USE_SW_CTS_M     (1 << 4)
#define reg_acr_cts_flt_en(x)    (((x) & 1) << 3) /* [3] */
#define REG_ACR_CTS_FLT_EN_M     (1 << 3)
#define reg_acr_cts_gen_sel(x)   (((x) & 1) << 2) /* [2] */
#define REG_ACR_CTS_GEN_SEL_M    (1 << 2)
#define reg_acr_cts_hw_sw_sel(x) (((x) & 1) << 1) /* [1] */
#define REG_ACR_CTS_HW_SW_SEL_M  (1 << 1)
#define reg_acr_cts_req_en(x)    (((x) & 1) << 0) /* [0] */
#define REG_ACR_CTS_REQ_EN_M     (1 << 0)

#define REG_AUD_ACR_CFG         0x1044
#define reg_acr_cts_chg_thre(x) (((x) & 0xff) << 16) /* [23:16] */
#define REG_ACR_CTS_CHG_THRE_M  (0xff << 16)
#define reg_acr_cts_thre(x)     (((x) & 0xff) << 8) /* [15:8] */
#define REG_ACR_CTS_THRE_M      (0xff << 8)
#define reg_acr_ave_max(x)      (((x) & 0x1f) << 3) /* [7:3] */
#define REG_ACR_AVE_MAX_M       (0x1f << 3)
#define reg_acr_fm_val_sw(x)    (((x) & 0x7) << 0) /* [2:0] */
#define REG_ACR_FM_VAL_SW_M     (0x7 << 0)

#define REG_ACR_N_VAL_SW 0x1048
#define reg_acr_n_val(x) (((x) & 0xfffff) << 0) /* [19:0] */
#define REG_ACR_N_VAL_M  (0xfffff << 0)

#define REG_ACR_CTS_VAL_SW 0x104C
#define reg_acr_cts_val(x) (((x) & 0xfffff) << 0) /* [19:0] */
#define REG_ACR_CTS_VAL_M  (0xfffff << 0)

#define REG_ACR_CTS_VAL_HW 0x1050

#define REG_AUDIO_PATH_ERROR_ST       0x106C
#define REG_AUDPATH_BLOCK_START_ERROR (1 << 1) /* [1] */
#define REG_AUDPATH_P_CHECK_ERROR     (1 << 0) /* [0] */

#define REG_AUDIO_PATTERN_CFG        0x1070
#define reg_cfg_audio_pattern_sel(x) (((x) & 1) << 1) /* [1] */
#define REG_CFG_AUDIO_PATTERN_SEL_M  (1 << 1)
#define reg_cfg_audio_pattern_en(x)  (((x) & 1) << 0) /* [0] */
#define REG_CFG_AUDIO_PATTERN_EN_M   (1 << 0)

#define REG_AUDIO_PATTERN_N_CFG  0x1074
#define reg_cfg_n_val_pattern(x) (((x) & 0xfffff) << 0) /* [19:0] */
#define REG_CFG_N_VAL_PATTERN_M  (0xfffff << 0)

#define REG_AUDIO_PATTERN_CTS_CFG  0x1078
#define reg_cfg_cts_val_pattern(x) (((x) & 0xfffff) << 0) /* [19:0] */
#define REG_CFG_CTS_VAL_PATTERN_M  (0xfffff << 0)

#define REG_AUDIO_SQW_CFG0        0x107C
#define reg_cfg_pattern_period(x) (((x) & 0xfff) << 0) /* [11:0] */
#define REG_CFG_PATTERN_PERIOD_M  (0xfff << 0)

#define REG_AUDIO_SQW_CFG1      0x1080
#define reg_cfg_pattern_high(x) (((x) & 0xffffff) << 0) /* [23:0] */
#define REG_CFG_PATTERN_HIGH_M  (0xffffff << 0)

#define REG_AUDIO_SQW_CFG2     0x1084
#define reg_cfg_pattern_low(x) (((x) & 0xffffff) << 0) /* [23:0] */
#define REG_CFG_PATTERN_LOW_M  (0xffffff << 0)

#define REG_AUDIO_CBIT_CH_SEL  0x1088
#define reg_cfg_cbit_ch_sel(x) (((x) & 0x7) << 0) /* [2:0] */
#define REG_CFG_CBIT_CH_SEL_M  (0x7 << 0)

#define REG_AUDIO_CBIT_P0 0x108C /* CBIT read register */
#define REG_AUDIO_CBIT_P1 0x1090 /* CBIT read register */
#define REG_AUDIO_CBIT_P2 0x1094 /* CBIT read register */
#define REG_AUDIO_CBIT_P3 0x1098 /* CBIT read register */
#define REG_AUDIO_CBIT_P4 0x109C /* CBIT read register */
#define REG_AUDIO_CBIT_P5 0x10A0 /* CBIT read register */

#endif
