/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019.All rights reserved.
 * Description: hdmi driver hdcp reg header file
 * Author:  Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#ifndef __HAL_HDMITX_HDCP_REG_H__
#define __HAL_HDMITX_HDCP_REG_H__

/* tx_REG_HDCP_reg module field info */
#define REG_HDCP1X_MEM_CTRL        0x2000
#define reg_hdcp1x_bist2_err(x)     (((x) & 0x1) << 10)
#define REG_HDCP1X_BIST2_ERR_M      (1 << 10)
#define reg_hdcp1x_bist1_err(x)     (((x) & 0x1) << 9)
#define REG_HDCP1X_BIST1_ERR_M      (1 << 9)
#define reg_hdcp1x_bist0_err(x)     (((x) & 0x1) << 8)
#define REG_HDCP1X_BIST0_ERR_M      (1 << 8)
#define reg_hdcp1x_cmd_done(x)      (((x) & 0x1) << 7)
#define REG_HDCP1X_CMD_DONE_M       (1 << 7)
#define reg_hdcp1x_key_cmd_valid(x) (((x) & 0x1) << 6)
#define REG_HDCP1X_KEY_CMD_VALID_M  (1 << 6)
#define reg_hdcp1x_key_cmd(x)       (((x) & 0x1f) << 1)
#define REG_HDCP1X_KEY_CMD_M        (0x1f << 1)
#define reg_hdcp1x_key_load(x)      (((x) & 0x1) << 0)
#define REG_HDCP1X_KEY_LOAD_M       (1 << 0)

#define REG_HDCP1X_KEY_AKSV0        0x2004
#define reg_hdcp1x_key_aksv_byte3(x) (((x) & 0xff) << 24)
#define REG_HDCP1X_KEY_AKSV_BYTE3_M  (0xff << 24)
#define reg_hdcp1x_key_aksv_byte2(x) (((x) & 0xff) << 16)
#define REG_HDCP1X_KEY_AKSV_BYTE2_M  (0xff << 16)
#define reg_hdcp1x_key_aksv_byte1(x) (((x) & 0xff) << 8)
#define REG_HDCP1X_KEY_AKSV_BYTE1_M  (0xff << 8)
#define reg_hdcp1x_key_aksv_byte0(x) (((x) & 0xff) << 0)
#define REG_HDCP1X_KEY_AKSV_BYTE0_M  (0xff << 0)

#define REG_HDCP1X_KEY_AKSV1       0x2008
#define reg_hdcp1x_key_aksv_byte4(x) (((x) & 0xff) << 0)
#define REG_HDCP1X_KEY_AKSV_BYTE4_M  (0xff << 0)

#define REG_HDCP_MUTE_CTRL         0x200C
#define reg_hdcp2x_amute_ctrl(x)    (((x) & 0x3)) << 4)
#define REG_HDCP2X_AMUTE_CTRL_M     (0x3 << 4)
#define reg_hdcp2x_vmute_ctrl(x)    (((x) & 0x1) << 3)
#define REG_HDCP2X_VMUTE_CTRL_M     (1 << 3)
#define reg_hdcp1x_amute_ctrl(x)    (((x) & 0x3)) << 1)
#define REG_HDCP1X_AMUTE_CTRL_M     (0x3 << 1)
#define reg_hdcp1x_vmute_ctrl(x)    (((x) & 0x1) << 0)
#define REG_HDCP1X_VMUTE_CTRL_M     (1 << 0)

#define REG_HDCP_FUN_SEL           0x2014
#define reg_hdcp_fun_sel(x)         (((x) & 0x1) << 0)
#define REG_HDCP_FUN_SEL_M          (1 << 0)

#define REG_HDCP1X_SEC_CFG         0x201C
#define reg_hdcp1x_sec_rpt_on(x)    (((x) & 0x1) << 1)
#define REG_HDCP1X_SEC_RPT_ON_M     (1 << 1)
#define reg_hdcp1x_sec_enc_en(x)    (((x) & 0x1) << 1)
#define REG_HDCP1X_SEC_ENC_EN_M     (1 << 0)

#define REG_HDCP1X_ENG_CTRL        0x2020
#define reg_hdcp1x_ri_comp(x)       (((x) & 0x7f) << 5)
#define REG_HDCP1X_RI_COMP_M        (0x7f << 5)
#define reg_hdcp1x_ri_hold(x)       (((x) & 0x1) << 4)
#define REG_HDCP1X_RI_HOLD_M        (1 << 4)
#define reg_hdcp1x_ri_trush(x)      (((x) & 0x1) << 3)
#define REG_HDCP1X_RI_TRUSH_M       (1 << 3)
#define reg_hdcp1x_rpt_on(x)        (((x) & 0x1) << 2)
#define REG_HDCP1X_RPT_ON_M         (1 << 2)
#define reg_hdcp1x_an_stop(x)       (((x) & 0x1) << 1)
#define REG_HDCP1X_AN_STOP_M        (1 << 1)
#define reg_hdcp1x_enc_en(x)        (((x) & 0x1) << 0)
#define REG_HDCP1X_ENC_EN_M         (1 << 0)

#define REG_HDCP1X_ENG_STATUS      0x2024
#define reg_hdcp1x_ri_rdy(x)        (((x) & 0x1) << 8)
#define REG_HDCP1X_RI_RDY_M         (1 << 8)
#define reg_hdcp1x_bksv_err(x)      (((x) & 0x1) << 7)
#define REG_HDCP1X_BKSV_ERR_M       (1 << 7)
#define reg_hdcp1x_i_cnt(x)         (((x) & 0x7f) << 0)
#define REG_HDCP1X_I_CNT_M          (0x7f << 0)

#define REG_HDCP1X_ENG_BKSV0        0x2028
#define reg_hdcp1x_key_bksv_byte3(x) (((x) & 0xff) << 24)
#define REG_HDCP1X_KEY_BKSV_BYTE3_M  (0xff << 24)
#define reg_hdcp1x_key_bksv_byte2(x) (((x) & 0xff) << 16)
#define REG_HDCP1X_KEY_BKSV_BYTE2_M  (0xff << 16)
#define reg_hdcp1x_key_bksv_byte1(x) (((x) & 0xff) << 8)
#define REG_HDCP1X_KEY_BKSV_BYTE1_M  (0xff << 8)
#define reg_hdcp1x_key_bksv_byte0(x) (((x) & 0xff) << 0)
#define REG_HDCP1X_KEY_BKSV_BYTE0_M  (0xff << 0)

#define REG_HDCP1X_ENG_BKSV1        0x202C
#define reg_hdcp1x_key_bksv_byte4(x) (((x) & 0xff) << 0)
#define REG_HDCP1X_KEY_BKSV_BYTE4_M  (0xff << 0)

#define REG_HDCP1X_ENG_GEN_AN0     0x2030
#define REG_HDCP1X_ENG_GEN_AN1     0x2034
#define reg_hdcp1x_eng_gen_an(x)    ((x) & 0xffffffff)
#define REG_HDCP1X_ENG_GEN_AN_M     (0xffffffff << 0)

#define REG_HDCP1X_ENG_FUN_AN0     0x2038
#define REG_HDCP1X_ENG_FUN_AN1     0x203C
#define reg_hdcp1x_eng_fun_an(x)    ((x) & 0xffffffff)
#define REG_HDCP1X_ENG_FUN_AN_M     (0xffffffff << 0)

#define REG_HDCP1X_ENG_RI          0x2040
#define reg_hdcp1x_eng_ri1(x)       (((x) & 0xff) << 8)
#define REG_HDCP1X_ENG_RI1_M        (0xff << 8)
#define reg_hdcp1x_eng_ri0(x)       (((x) & 0xff) << 0)
#define REG_HDCP1X_ENG_RI0_M        (0xff << 0)

#define REG_HDCP1X_RPT_BSTATUS     0x2058
#define reg_hdcp1x_bstatus(x)       (((x) & 0xffff) << 0)
#define REG_HDCP1X_BSTATUS_M        (0xffff << 0)

#define REG_HDCP1X_RPT_KLIST       0x205C
#define reg_hdcp1x_rpt_klist(x)     (((x) & 0xff) << 0)
#define REG_HDCP1X_RPT_KLIST_M      (0xff << 0)

#define REG_HDCP1X_SHA_CTRL        0x2060
#define reg_hdcp1x_sha_ready(x)     (((x) & 0x1) << 2)
#define REG_HDCP1X_SHA_READY_M      (1 << 2)
#define reg_hdcp1x_sha_no_ds(x)     (((x) & 0x1) << 1)
#define REG_HDCP1X_SHA_NO_DS_M      (1 << 1)
#define reg_hdcp1x_sha_start(x)     (((x) & 0x1) << 0)
#define REG_HDCP1X_SHA_START        (1 << 0)

#define REG_HDCP1X_SHA_V0          0x2064
#define REG_HDCP1X_SHA_V1          0x2068
#define REG_HDCP1X_SHA_V2          0x206C
#define REG_HDCP1X_SHA_V3          0x2070
#define REG_HDCP1X_SHA_V4          0x2074
#define reg_hdcp1x_rpt_vh(x)        (((x) & 0xffffffff) << 0)
#define REG_HDCP1X_RPT_VH_M         (0xffffffff << 0)

#define REG_HDCP1X_CHK_CTRL           0x2080
#define reg_hdcp1x_check_errcnt_en(x)  (((x) & 0x1) << 4)
#define REG_HDCP1X_CHECK_ERRCNT_EN_M   (1 << 4)
#define reg_hdcp1x_check_start_sel(x)  (((x) & 0x3) << 2)
#define REG_HDCP1X_CHECK_START_SEL_M   (0x3 << 2)
#define reg_hdcp1x_ddc_req_priority(x) (((x) & 0x1) << 1)
#define REG_HDCP1X_DDC_REQ_PRIORITY_M  (1 << 1)
#define reg_hdcp1x_auto_check_en(x)    (((x) & 0x1) << 0)
#define REG_HDCP1X_AUTO_CHECK_EN_M     (1 << 0)

#define REG_HDCP1X_CHK_START        0x2084
#define reg_hdcp1x_linecnt_thre(x)   (((x) & 0xff) << 24)
#define REG_HDCP1X_LINECNT_THRE_M    (0xff << 24)
#define reg_hdcp1x_ddc_req_fcnt(x)   (((x) & 0xff) << 16)
#define REG_HDCP1X_DDC_REQ_FCNT_M    (0xff << 16)
#define reg_hdcp1x_ri_chk2_fcnt(x)   (((x) & 0xff) << 8)
#define REG_HDCP1X_RI_CHK2_FCNT_M    (0xff << 8)
#define reg_hdcp1x_ri_chk1_fcnt(x)   (((x) & 0xff) << 0)
#define REG_HDCP1X_RI_CHK1_FCNT_M    (0xff << 0)

#define REG_HDCP1X_CHK_ERR          0x2088
#define reg_hdcp1x_nodone_err_cnt(x) (((x) & 0xff) << 24)
#define REG_HDCP1X_NODONE_ERR_CNT_M  (0xff << 24)
#define reg_hdcp1x_notcfg_err_cnt(x) (((x) & 0xff) << 16)
#define REG_HDCP1X_NOTCHG_ERR_CNT_M  (0xff << 16)
#define reg_hdcp1x_000frm_err_cnt(x) (((x) & 0xff) << 8)
#define REG_HDCP1X_000FRM_ERR_CNT_M  (0xff << 8)
#define reg_hdcp1x_127frm_err_cnt(x) (((x) & 0xff) << 0)
#define REG_HDCP1X_127FRM_ERR_CNT_M  (0xff << 0)

#define REG_HDCP1X_CHK_STATE       0x208C
#define reg_hdcp1x_fsm_state(x)     (((x) & 0xf) << 0)
#define REG_HDCP1X_FSM_STATE_M      (0xf << 0)

#define REG_HDCP2X_ENC_CTRL        0x20A0
#define reg_hdcp2x_enc_en(x)        (((x) & 0x1) << 0)
#define REG_HDCP2X_ENC_EN_M         (1 << 0)

#define REG_HDCP2X_DBG_CTRL        0x20A4
#define reg_hdcp2x_uart_sel(x)      (((x) & 0x1) << 0)
#define REG_HDCP2X_UART_SEL_M       (1 << 0)

#define REG_HDCP2X_ENC_STATE       0x20B4
#define reg_hdcp2x_enc_state(x)     (((x) & 0x1) << 0)
#define REG_HDCP2X_ENC_STATE_M      (1 << 0)

#define REG_HDCP2X_CIPHER_CTRL     0x20B8
#define reg_cfg_dis_ciph_mode(x)    (((x) & 0x7) << 2)
#define REG_CFG_DIS_CIPH_MODE_M     (0x7 << 2)
#define reg_cfg_hdcp_mode_syn_en(x) (((x) & 0x1) << 1)
#define REG_CFG_HDCP_MODE_SYN_EN_M  (1 << 1)
#define reg_cfg_auth_done_syn_en(x) (((x) & 0x1) << 0)
#define REG_CFG_AUTH_DONE_SYN_EN_M  (1 << 0)

#define REG_HDCP2X_CIPHER_STATE    0x20BC
#define reg_ake_frm_cnt_max(x)      (((x) & 0x1) << 25)
#define REG_AKE_FRM_CNT_MAX_M       (1 << 25)
#define reg_ake_pxl_cnt_max(x)      (((x) & 0x1) << 24)
#define REG_AKE_PXL_CNT_MAX_M       (1 << 24)
#define reg_hdcp2x_dec_fifo_werr_cnt(x) (((x) & 0xff) << 16)
#define REG_HDCP2X_DEC_FIFO_WERR_CNT_M  (0xff << 16)
#define reg_hdcp2x_dec_fifo_rerr_cnt(x) (((x) & 0xff) << 8)
#define REG_HDCP2X_DEC_FIFO_RERR_CNT_M  (0xff << 8)
#define reg_hdcp2x_dis_dec_st(x)    (((x) & 0x1) << 7)
#define REG_HDCP2X_DIS_DEC_ST_M     (1 << 7)
#define reg_hdcp2x_dec_fifo_cnt(x)  (((x) & 0x1f) << 2)
#define REG_HDCP2X_DEC_FIFO_CNT_M   (0x1f << 2)
#define reg_hdcp2x_dec_fifo_empty(x)    (((x) & 0x1) << 1)
#define REG_HDCP2X_DEC_FIFO_EMPTY_M (1 << 1)
#define reg_hdcp2x_dec_fifo_full(x)    (((x) & 0x1) << 0)
#define REG_HDCP2X_DEC_FIFO_FULL_M  (1 << 0)

#define REG_HDCP2X_FRM_CNT_LSB     0x20C0
#define reg_hdcp2x__frm_cnt_lsb(x)  (((x) & 0xffffffff) << 0)
#define REG_HDCP2X_FRM_CNT_LSB_M    (0xffffffff << 0)

#define REG_HDCP2X_FRM_CNT_MSB     0x20C4
#define reg_hdcp2x_frm_cnt_msb(x)   (((x) & 0xff) << 0)
#define REG_HDCP2X_FRM_CNT_MSB_M    (0xff << 0)

#define REG_HDCP1X_RPT_V0          0x20E0
#define REG_HDCP1X_RPT_V1          0x20E4
#define REG_HDCP1X_RPT_V2          0x20E8
#define REG_HDCP1X_RPT_V3          0x20EC
#define REG_HDCP1X_RPT_V4          0x20F0
#define reg_hdcp1x_rpt_calc_vh(x)   (((x) & 0xffffffff) << 0)
#define REG_HDCP1X_RPT_CALC_VH_M    (0xffffffff << 0)

#define REG_HDCP1X_SHA_CHECK       0x20F4
#define reg_hdcp1x_sha_check(x)     (((x) & 0x1) << 0)
#define REG_HDCP1X_SHA_CHECK_M      (1 << 0)

#define REG_HDCP1X_SHA_RESULT      0x20F8
#define reg_hdcp1x_sha_invalid(x)   (((x) & 0x1) << 1)
#define REG_HDCP1X_SHA_INVALID_M    (1 << 1)
#define reg_hdcp1x_sha_ok(x)        (((x) & 0x1) << 0)
#define REG_HDCP1X_SHA_OK_M         (1 << 0)

#endif
