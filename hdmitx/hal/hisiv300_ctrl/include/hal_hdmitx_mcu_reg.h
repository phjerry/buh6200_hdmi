/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019.All rights reserved.
 * Description: hdmi driver hdcp mcu reg header file
 * Author:  Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#ifndef __HAL_HDMITX_MCU_REG_H__
#define __HAL_HDMITX_MCU_REG_H__

#define REG_HDCP2X_HW_VER_B0 0x1C000
#define reg_hdcp2x_hw_ver_b0(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_HW_VER_B0_M   (0xff << 0)

#define REG_HDCP2X_HW_VER_B1 0x1C004
#define reg_hdcp2x_hw_ver_b1(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_HW_VER_B1_M   (0xff << 0)

#define REG_HDCP2X_HW_VER_B2 0x1C008
#define reg_hdcp2x_hw_ver_b2(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_HW_VER_B2_M   (0xff << 0)

#define REG_HDCP2X_HW_VER_B3 0x1C00C
#define reg_hdcp2x_hw_ver_b3(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_HW_VER_B3_M   (0xff << 0)

#define REG_HDCP2X_SW_VER_B0 0x1C010
#define reg_hdcp2x_sw_ver_b0(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_SW_VER_B0_M   (0xff << 0)

#define REG_HDCP2X_SW_VER_B1 0x1C014
#define reg_hdcp2x_sw_ver_b1(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_SW_VER_B1_M   (0xff << 0)

#define REG_HDCP2X_SW_VER_B2 0x1C018
#define reg_hdcp2x_sw_ver_b2(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_SW_VER_B2_M   (0xff << 0)

#define REG_HDCP2X_SW_VER_B3 0x1C01C
#define reg_hdcp2x_sw_ver_b3(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_SW_VER_B3_M   (0xff << 0)

#define REG_HDCP2X_AUTH_CTRL 0x1C054
#define reg_mcu_auth_stop(x)     (((x) & 1) << 1)
#define REG_MCU_AUTH_STOP_M      (1 << 1)
#define reg_mcu_auth_start(x)    (((x) & 1) << 0)
#define REG_MCU_AUTH_START_M     (1 << 0)

#define REG_HDCP2X_SEC_INTR_B0 0x1C080
#define reg_hdcp2x_sec_intr7(x)    (((x) & 1) << 7)
#define REG_HDCP2X_SEC_INTR7_M     (1 << 7)
#define reg_hdcp2x_sec_intr6(x)    (((x) & 1) << 6)
#define REG_HDCP2X_SEC_INTR6_M     (1 << 6)
#define reg_hdcp2x_sec_intr5(x)    (((x) & 1) << 5)
#define REG_HDCP2X_SEC_INTR5_M     (1 << 5)
#define reg_hdcp2x_sec_intr4(x)    (((x) & 1) << 4)
#define REG_HDCP2X_SEC_INTR4_M     (1 << 4)
#define reg_hdcp2x_sec_intr3(x)    (((x) & 1) << 3)
#define REG_HDCP2X_SEC_INTR3_M     (1 << 3)
#define reg_hdcp2x_sec_intr2(x)    (((x) & 1) << 2)
#define REG_HDCP2X_SEC_INTR2_M     (1 << 2)
#define reg_hdcp2x_sec_intr1(x)    (((x) & 1) << 1)
#define REG_HDCP2X_SEC_INTR1_M     (1 << 1)
#define reg_hdcp2x_sec_intr0(x)    (((x) & 1) << 0)
#define REG_HDCP2X_SEC_INTR0_M     (1 << 0)

#define REG_HDCP2X_SEC_INTR_B1 0x1C084
#define reg_hdcp2x_sec_intr15(x)   (((x) & 1) << 7)
#define REG_HDCP2X_SEC_INTR15_M    (1 << 7)
#define reg_hdcp2x_sec_intr14(x)   (((x) & 1) << 6)
#define REG_HDCP2X_SEC_INTR14_M    (1 << 6)
#define reg_hdcp2x_sec_intr13(x)   (((x) & 1) << 5)
#define REG_HDCP2X_SEC_INTR13_M    (1 << 5)
#define reg_hdcp2x_sec_intr12(x)   (((x) & 1) << 4)
#define REG_HDCP2X_SEC_INTR12_M    (1 << 4)
#define reg_hdcp2x_sec_intr11(x)   (((x) & 1) << 3)
#define REG_HDCP2X_SEC_INTR11_M    (1 << 3)
#define reg_hdcp2x_sec_intr10(x)   (((x) & 1) << 2)
#define REG_HDCP2X_SEC_INTR10_M    (1 << 2)
#define reg_hdcp2x_sec_intr9(x)    (((x) & 1) << 1)
#define REG_HDCP2X_SEC_INTR9_M     (1 << 1)
#define reg_hdcp2x_sec_intr8(x)    (((x) & 1) << 0)
#define REG_HDCP2X_SEC_INTR8_M     (1 << 0)

#define REG_HDCP2X_SEC_MASK_B0 0x1C088
#define reg_hdcp2x_sec_mask7(x)    (((x) & 1) << 7)
#define REG_HDCP2X_SEC_MASK7_M     (1 << 7)
#define reg_hdcp2x_sec_mask6(x)    (((x) & 1) << 6)
#define REG_HDCP2X_SEC_MASK6_M     (1 << 6)
#define reg_hdcp2x_sec_mask5(x)    (((x) & 1) << 5)
#define REG_HDCP2X_SEC_MASK5_M     (1 << 5)
#define reg_hdcp2x_sec_mask4(x)    (((x) & 1) << 4)
#define REG_HDCP2X_SEC_MASK4_M     (1 << 4)
#define reg_hdcp2x_sec_mask3(x)    (((x) & 1) << 3)
#define REG_HDCP2X_SEC_MASK3_M     (1 << 3)
#define reg_hdcp2x_sec_mask2(x)    (((x) & 1) << 2)
#define REG_HDCP2X_SEC_MASK2_M     (1 << 2)
#define reg_hdcp2x_sec_mask1(x)    (((x) & 1) << 1)
#define REG_HDCP2X_SEC_MASK1_M     (1 << 1)
#define reg_hdcp2x_sec_mask0(x)    (((x) & 1) << 0)
#define REG_HDCP2X_SEC_MASK0_M     (1 << 0)

#define REG_HDCP2X_SEC_MASK_B1 0x1C08C
#define reg_hdcp2x_sec_mask15(x)   (((x) & 1) << 7)
#define REG_HDCP2X_SEC_MASK15_M    (1 << 7)
#define reg_hdcp2x_sec_mask14(x)   (((x) & 1) << 6)
#define REG_HDCP2X_SEC_MASK14_M    (1 << 6)
#define reg_hdcp2x_sec_mask13(x)   (((x) & 1) << 5)
#define REG_HDCP2X_SEC_MASK13_M    (1 << 5)
#define reg_hdcp2x_sec_mask12(x)   (((x) & 1) << 4)
#define REG_HDCP2X_SEC_MASK12_M    (1 << 4)
#define reg_hdcp2x_sec_mask11(x)   (((x) & 1) << 3)
#define REG_HDCP2X_SEC_MASK11_M    (1 << 3)
#define reg_hdcp2x_sec_mask10(x)   (((x) & 1) << 2)
#define REG_HDCP2X_SEC_MASK10_M    (1 << 2)
#define reg_hdcp2x_sec_mask9(x)    (((x) & 1) << 1)
#define REG_HDCP2X_SEC_MASK9_M     (1 << 1)
#define reg_hdcp2x_sec_mask8(x)    (((x) & 1) << 0)
#define REG_HDCP2X_SEC_MASK8_M     (1 << 0)

#define REG_HDCP2X_MCU_INTR_B0 0x1C0A4
#define reg_hdcp2x_mcu_intr7(x)    (((x) & 1) << 7)
#define REG_HDCP2X_MCU_INTR7_M     (1 << 7)
#define reg_hdcp2x_mcu_intr6(x)    (((x) & 1) << 6)
#define REG_HDCP2X_MCU_INTR6_M     (1 << 6)
#define reg_hdcp2x_mcu_intr5(x)    (((x) & 1) << 5)
#define REG_HDCP2X_MCU_INTR5_M     (1 << 5)
#define reg_hdcp2x_mcu_intr4(x)    (((x) & 1) << 4)
#define REG_HDCP2X_MCU_INTR4_M     (1 << 4)
#define reg_hdcp2x_mcu_intr3(x)    (((x) & 1) << 3)
#define REG_HDCP2X_MCU_INTR3_M     (1 << 3)
#define reg_hdcp2x_mcu_intr2(x)    (((x) & 1) << 2)
#define REG_HDCP2X_MCU_INTR2_M     (1 << 2)
#define reg_hdcp2x_mcu_intr1(x)    (((x) & 1) << 1)
#define REG_HDCP2X_MCU_INTR1_M     (1 << 1)
#define reg_hdcp2x_mcu_intr0(x)    (((x) & 1) << 0)
#define REG_HDCP2X_MCU_INTR0_M     (1 << 0)

#define REG_HDCP2X_MCU_INTR_B1 0x1C0A8
#define reg_hdcp2x_mcu_intr15(x)   (((x) & 1) << 7)
#define REG_HDCP2X_MCU_INTR15_M    (1 << 7)
#define reg_hdcp2x_mcu_intr14(x)   (((x) & 1) << 6)
#define REG_HDCP2X_MCU_INTR14_M    (1 << 6)
#define reg_hdcp2x_mcu_intr13(x)   (((x) & 1) << 5)
#define REG_HDCP2X_MCU_INTR13_M    (1 << 5)
#define reg_hdcp2x_mcu_intr12(x)   (((x) & 1) << 4)
#define REG_HDCP2X_MCU_INTR12_M    (1 << 4)
#define reg_hdcp2x_mcu_intr11(x)   (((x) & 1) << 3)
#define REG_HDCP2X_MCU_INTR11_M    (1 << 3)
#define reg_hdcp2x_mcu_intr10(x)   (((x) & 1) << 2)
#define REG_HDCP2X_MCU_INTR10_M    (1 << 2)
#define reg_hdcp2x_mcu_intr9(x)    (((x) & 1) << 1)
#define REG_HDCP2X_MCU_INTR9_M     (1 << 1)
#define reg_hdcp2x_mcu_intr8(x)    (((x) & 1) << 0)
#define REG_HDCP2X_MCU_INTR8_M     (1 << 0)

#define REG_HDCP2X_MCU_INTR_B2 0x1C0AC
#define reg_hdcp2x_mcu_intr23(x)   (((x) & 1) << 7)
#define REG_HDCP2X_MCU_INTR23_M    (1 << 7)
#define reg_hdcp2x_mcu_intr22(x)   (((x) & 1) << 6)
#define REG_HDCP2X_MCU_INTR22_M    (1 << 6)
#define reg_hdcp2x_mcu_intr21(x)   (((x) & 1) << 5)
#define REG_HDCP2X_MCU_INTR21_M    (1 << 5)
#define reg_hdcp2x_mcu_intr20(x)   (((x) & 1) << 4)
#define REG_HDCP2X_MCU_INTR20_M    (1 << 4)
#define reg_hdcp2x_mcu_intr19(x)   (((x) & 1) << 3)
#define REG_HDCP2X_MCU_INTR19_M    (1 << 3)
#define reg_hdcp2x_mcu_intr18(x)   (((x) & 1) << 2)
#define REG_HDCP2X_MCU_INTR18_M    (1 << 2)
#define reg_hdcp2x_mcu_intr17(x)   (((x) & 1) << 1)
#define REG_HDCP2X_MCU_INTR17_M    (1 << 1)
#define reg_hdcp2x_mcu_intr16(x)   (((x) & 1) << 0)
#define REG_HDCP2X_MCU_INTR16_M    (1 << 0)

#define REG_HDCP2X_MCU_INTR_B3 0x1C0B0
#define reg_hdcp2x_mcu_intr31(x)   (((x) & 1) << 7)
#define REG_HDCP2X_MCU_INTR31_M    (1 << 7)
#define reg_hdcp2x_mcu_intr30(x)   (((x) & 1) << 6)
#define REG_HDCP2X_MCU_INTR30_M    (1 << 6)
#define reg_hdcp2x_mcu_intr29(x)   (((x) & 1) << 5)
#define REG_HDCP2X_MCU_INTR29_M    (1 << 5)
#define reg_hdcp2x_mcu_intr28(x)   (((x) & 1) << 4)
#define REG_HDCP2X_MCU_INTR28_M    (1 << 4)
#define reg_hdcp2x_mcu_intr27(x)   (((x) & 1) << 3)
#define REG_HDCP2X_MCU_INTR27_M    (1 << 3)
#define reg_hdcp2x_mcu_intr26(x)   (((x) & 1) << 2)
#define REG_HDCP2X_MCU_INTR26_M    (1 << 2)
#define reg_hdcp2x_mcu_intr25(x)   (((x) & 1) << 1)
#define REG_HDCP2X_MCU_INTR25_M    (1 << 1)
#define reg_hdcp2x_mcu_intr24(x)   (((x) & 1) << 0)
#define REG_HDCP2X_MCU_INTR24_M    (1 << 0)

#define REG_HDCP2X_MCU_MASK_B0 0x1C0B4
#define reg_hdcp2x_mcu_mask7(x)    (((x) & 1) << 7)
#define REG_HDCP2X_MCU_MASK7_M     (1 << 7)
#define reg_hdcp2x_mcu_mask6(x)    (((x) & 1) << 6)
#define REG_HDCP2X_MCU_MASK6_M     (1 << 6)
#define reg_hdcp2x_mcu_mask5(x)    (((x) & 1) << 5)
#define REG_HDCP2X_MCU_MASK5_M     (1 << 5)
#define reg_hdcp2x_mcu_mask4(x)    (((x) & 1) << 4)
#define REG_HDCP2X_MCU_MASK4_M     (1 << 4)
#define reg_hdcp2x_mcu_mask3(x)    (((x) & 1) << 3)
#define REG_HDCP2X_MCU_MASK3_M     (1 << 3)
#define reg_hdcp2x_mcu_mask2(x)    (((x) & 1) << 2)
#define REG_HDCP2X_MCU_MASK2_M     (1 << 2)
#define reg_hdcp2x_mcu_mask1(x)    (((x) & 1) << 1)
#define REG_HDCP2X_MCU_MASK1_M     (1 << 1)
#define reg_hdcp2x_mcu_mask0(x)    (((x) & 1) << 0)
#define REG_HDCP2X_MCU_MASK0_M     (1 << 0)

#define REG_HDCP2X_MCU_MASK_B1 0x1C0B8
#define reg_hdcp2x_mcu_mask15(x)   (((x) & 1) << 7)
#define REG_HDCP2X_MCU_MASK15_M    (1 << 7)
#define reg_hdcp2x_mcu_mask14(x)   (((x) & 1) << 6)
#define REG_HDCP2X_MCU_MASK14_M    (1 << 6)
#define reg_hdcp2x_mcu_mask13(x)   (((x) & 1) << 5)
#define REG_HDCP2X_MCU_MASK13_M    (1 << 5)
#define reg_hdcp2x_mcu_mask12(x)   (((x) & 1) << 4)
#define REG_HDCP2X_MCU_MASK12_M    (1 << 4)
#define reg_hdcp2x_mcu_mask11(x)   (((x) & 1) << 3)
#define REG_HDCP2X_MCU_MASK11_M    (1 << 3)
#define reg_hdcp2x_mcu_mask10(x)   (((x) & 1) << 2)
#define REG_HDCP2X_MCU_MASK10_M    (1 << 2)
#define reg_hdcp2x_mcu_mask9(x)    (((x) & 1) << 1)
#define REG_HDCP2X_MCU_MASK9_M     (1 << 1)
#define reg_hdcp2x_mcu_mask8(x)   (((x) & 1) << 0)
#define REG_HDCP2X_MCU_MASK8_M     (1 << 0)

#define REG_HDCP2X_MCU_MASK_B2 0x1C0BC
#define reg_hdcp2x_mcu_mask23(x)   (((x) & 1) << 7)
#define REG_HDCP2X_MCU_MASK23_M    (1 << 7)
#define reg_hdcp2x_mcu_mask22(x)   (((x) & 1) << 6)
#define REG_HDCP2X_MCU_MASK22_M    (1 << 6)
#define reg_hdcp2x_mcu_mask21(x)   (((x) & 1) << 5)
#define REG_HDCP2X_MCU_MASK21_M    (1 << 5)
#define reg_hdcp2x_mcu_mask20(x)   (((x) & 1) << 4)
#define REG_HDCP2X_MCU_MASK20_M    (1 << 4)
#define reg_hdcp2x_mcu_mask19(x)   (((x) & 1) << 3)
#define REG_HDCP2X_MCU_MASK19_M    (1 << 3)
#define reg_hdcp2x_mcu_mask18(x)   (((x) & 1) << 2)
#define REG_HDCP2X_MCU_MASK18_M    (1 << 2)
#define reg_hdcp2x_mcu_mask17(x)   (((x) & 1) << 1)
#define REG_HDCP2X_MCU_MASK17_M    (1 << 1)
#define reg_hdcp2x_mcu_mask16(x)   (((x) & 1) << 0)
#define REG_HDCP2X_MCU_MASK16_M    (1 << 0)

#define REG_HDCP2X_MCU_MASK_B3 0x1C0C0
#define reg_hdcp2x_mcu_mask31(x)   (((x) & 1) << 7)
#define REG_HDCP2X_MCU_MASK31_M    (1 << 7)
#define reg_hdcp2x_mcu_mask30(x)   (((x) & 1) << 6)
#define REG_HDCP2X_MCU_MASK30_M    (1 << 6)
#define reg_hdcp2x_mcu_mask29(x)   (((x) & 1) << 5)
#define REG_HDCP2X_MCU_MASK29_M    (1 << 5)
#define reg_hdcp2x_mcu_mask28(x)   (((x) & 1) << 4)
#define REG_HDCP2X_MCU_MASK28_M    (1 << 4)
#define reg_hdcp2x_mcu_mask27(x)   (((x) & 1) << 3)
#define REG_HDCP2X_MCU_MASK27_M    (1 << 3)
#define reg_hdcp2x_mcu_mask26(x)   (((x) & 1) << 2)
#define REG_HDCP2X_MCU_MASK26_M    (1 << 2)
#define reg_hdcp2x_mcu_mask25(x)   (((x) & 1) << 1)
#define REG_HDCP2X_MCU_MASK25_M    (1 << 1)
#define reg_hdcp2x_mcu_mask24(x)   (((x) & 1) << 0)
#define REG_HDCP2X_MCU_MASK24_M    (1 << 0)

#define REG_HDCP2X_MCU_STATE_B0 0x1C0CC
#define reg_hdcp2x_mcu_state_b0(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_MCU_STATE_B0_M   (0xff << 0)

#define REG_HDCP2X_MCU_STATE_B1 0x1C0D0
#define reg_hdcp2x_mcu_state_b1(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_MCU_STATE_B1_M   (0xff << 0)

#define REG_HDCP2X_MCU_STATE_B2 0x1C0D4
#define reg_hdcp2x_mcu_state_b2(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_MCU_STATE_B2_M   (0xff << 0)

#define REG_HDCP2X_MCU_STATE_B3 0x1C0D8
#define reg_hdcp2x_mcu_state_b3(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_MCU_STATE_B3_M   (0xff << 0)

#define REG_HDCP2X_RX_AUTH_STATE 0x1C0E8
#define reg_hdcp2x_send_sks_done(x)  (((x) & 1) << 7)
#define REG_HDCP2X_SEND_SKS_DONE_M   (1 << 7)
#define reg_hdcp2x_send_l_done(x)    (((x) & 1) << 6)
#define REG_HDCP2X_SEND_L_DONE_M     (1 << 6)
#define reg_hdcp2x_lc_init_done(x)   (((x) & 1) << 5)
#define REG_HDCP2X_LC_INIT_DONE_M    (1 << 5)
#define reg_hdcp2x_pairing_done(x)   (((x) & 1) << 4)
#define REG_HDCP2X_PAIRING_DONE_M    (1 << 4)
#define reg_hdcp2x_send_h_done(x)    (((x) & 1) << 3)
#define REG_HDCP2X_SEND_H_DONE_M     (1 << 3)
#define reg_hdcp2x_km_done(x)        (((x) & 1) << 2)
#define REG_HDCP2X_KM_DONE_M         (1 << 2)
#define reg_hdcp2x_send_cert_done(x) (((x) & 1) << 1)
#define REG_HDCP2X_SEND_CERT_DONE_M  (1 << 1)
#define reg_hdcp2x_ake_init_done(x)  (((x) & 1) << 0)
#define REG_HDCP2X_AKE_INIT_DONE_M   (1 << 0)

#define REG_HDCP2X_RPT_AUTH_STATE 0x1C0EC
#define reg_hdcp2x_strm_rdy_done(x)   (((x) & 1) << 3)
#define REG_HDCP2X_STRM_RDY_DONE_M    (1 << 3)
#define reg_hdcp2x_strm_id_done(x)    (((x) & 1) << 2)
#define REG_HDCP2X_STRM_ID_DONE_M     (1 << 2)
#define reg_hdcp2x_send_ack_done(x)   (((x) & 1) << 1)
#define REG_HDCP2X_SEND_ACK_DONE_M    (1 << 1)
#define reg_hdcp2x_id_list_done(x)    (((x) & 1) << 0)
#define REG_HDCP2X_ID_LIST_DONE_M     (1 << 0)

#define REG_HDCP2X_AUTH_STATE0 0x1C0F0
#define reg_hdcp2x_auth_state0(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_AUTH_STATE0_M   (0xff << 0)

#define REG_HDCP2X_AUTH_STATE1 0x1C0F4
#define reg_hdcp2x_auth_state1(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_AUTH_STATE1_M   (0xff << 0)

#define REG_HDCP2X_AUTH_STATE2 0x1C0F8
#define reg_hdcp2x_auth_state2(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_AUTH_STATE2_M   (0xff << 0)

#define REG_HDCP2X_AUTH_STATE3 0x1C0FC
#define reg_hdcp2x_auth_state3(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_AUTH_STATE3_M   (0xff << 0)

#define REG_HDCP2X_CERT_INFO 0x1C114
#define reg_hdcp2x_cert_rpt(x)   (((x) & 1) << 0)
#define REG_HDCP2X_CERT_RPT_M    (1 << 0)

#define REG_HDCP2X_CERT_VER 0x1C118
#define reg_hdcp2x_cert_ver(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_CERT_VER_M   (0xff << 0)

#define REG_HDCP2X_RPT_INFO   0x1C11C
#define reg_hdcp2x_rpt_dev_exc(x) (((x) & 1) << 3)
#define REG_HDCP2X_RPT_DEV_EXC_M  (1 << 3)
#define reg_hdcp2x_rpt_cas_exc(x) (((x) & 1) << 2)
#define REG_HDCP2X_RPT_CAS_EXC_M  (1 << 2)
#define reg_hdcp20_rpt_on(x)      (((x) & 1) << 1)
#define REG_HDCP20_RPT_ON_M       (1 << 1)
#define reg_hdcp1x_dev_on(x)      (((x) & 1) << 0)
#define REG_HDCP1X_DEV_ON_M       (1 << 0)

#define REG_HDCP2X_RPT_DEV    0x1C120
#define reg_hdcp2x_rpt_dep_cnt(x) (((x) & 0x7) << 5)
#define REG_HDCP2X_RPT_DEP_CNT_M  (0x7 << 5)
#define reg_hdcp2x_rpt_dev_cnt(x) (((x) & 0x1f) << 0)
#define REG_HDCP2X_RPT_DEV_CNT_M  (0x1f << 0)

#define REG_HDCP2X_RPT_V0 0x1C124
#define reg_hdcp2x_rpt_v0(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_RPT_V0_M   (0xff << 0)

#define REG_HDCP2X_RPT_V1 0x1C128
#define reg_hdcp2x_rpt_v1(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_RPT_V1_M   (0xff << 0)

#define REG_HDCP2X_RPT_V2 0x1C12C
#define reg_hdcp2x_rpt_v2(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_RPT_V2_M   (0xff << 0)

#define REG_HDCP2X_RPT_K0 0x1C130
#define reg_hdcp2x_rpt_k0(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_RPT_K0_M   (0xff << 0)

#define REG_HDCP2X_RPT_K1 0x1C134
#define reg_hdcp2x_rpt_k1(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_RPT_K1_M   (0xff << 0)

#define REG_HDCP2X_RPT_M0 0x1C138
#define reg_hdcp2x_rpt_m0(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_RPT_M0_M   (0xff << 0)

#define REG_HDCP2X_RPT_M1 0x1C13C
#define reg_hdcp2x_rpt_m1(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_RPT_M1_M   (0xff << 0)

#define REG_HDCP2X_RPT_M2 0x1C140
#define reg_hdcp2x_rpt_m2(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_RPT_M2_M   (0xff << 0)

#define REG_HDCP2X_RPT_STRM_ID 0x1C144
#define reg_hdcp2x_rpt_strm_id(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_RPT_STRM_ID_M   (0xff << 0)

#define REG_HDCP2X_RPT_STRM_TPYE 0x1C148
#define reg_hdcp2x_rpt_strm_type(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_RPT_STRM_TYPE_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG0 0x1C1A4
#define reg_hdcp2x_timing_cfg0(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG0_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG1 0x1C1A8
#define reg_hdcp2x_timing_cfg1(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG1_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG2 0x1C1AC
#define reg_hdcp2x_timing_cfg2(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG2_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG3 0x1C1B0
#define reg_hdcp2x_timing_cfg3(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG3_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG4 0x1C1B4
#define reg_hdcp2x_timing_cfg4(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG4_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG5 0x1C1B8
#define reg_hdcp2x_timing_cfg5(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG5_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG6 0x1C1BC
#define reg_hdcp2x_timing_cfg6(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG6_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG7 0x1C1C0
#define reg_hdcp2x_timing_cfg7(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG7_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG8 0x1C1C4
#define reg_hdcp2x_timing_cfg8(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG8_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG9 0x1C1C8
#define reg_hdcp2x_timing_cfg9(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG9_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG10 0x1C1CC
#define reg_hdcp2x_timing_cfg10(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG10_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG11 0x1C1D0
#define reg_hdcp2x_timing_cfg11(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG11_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG12 0x1C1D4
#define reg_hdcp2x_timing_cfg12(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG12_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG13 0x1C1D8
#define reg_hdcp2x_timing_cfg13(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG13_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG14 0x1C1DC
#define reg_hdcp2x_timing_cfg14(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG14_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG15 0x1C1E0
#define reg_hdcp2x_timing_cfg15(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG15_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG16 0x1C1E4
#define reg_hdcp2x_timing_cfg16(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG16_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG17 0x1C1E8
#define reg_hdcp2x_timing_cfg17(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG17_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG18 0x1C1EC
#define reg_hdcp2x_timing_cfg18(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG18_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG19 0x1C1F0
#define reg_hdcp2x_timing_cfg19(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG19_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG20 0x1C1F4
#define reg_hdcp2x_timing_cfg20(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG20_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG21 0x1C1F8
#define reg_hdcp2x_timing_cfg21(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG21_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG22 0x1C1FC
#define reg_hdcp2x_timing_cfg22(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG22_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG23 0x1C200
#define reg_hdcp2x_timing_cfg23(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG23_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG24 0x1C204
#define reg_hdcp2x_timing_cfg24(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG24_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG25 0x1C208
#define reg_hdcp2x_timing_cfg25(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG25_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG26 0x1C20C
#define reg_hdcp2x_timing_cfg26(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG26_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG27 0x1C210
#define reg_hdcp2x_timing_cfg27(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG27_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG28 0x1C214
#define reg_hdcp2x_timing_cfg28(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG28_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG29 0x1C218
#define reg_hdcp2x_timing_cfg29(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG29_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG30 0x1C21C
#define reg_hdcp2x_timing_cfg30(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG30_M   (0xff << 0)

#define REG_HDCP2X_TIMING_CFG31 0x1C220
#define reg_hdcp2x_timing_cfg31(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_TIMING_CFG31_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN0 0x1C224
#define reg_hdcp2x_gen_in0(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN0_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN1 0x1C228
#define reg_hdcp2x_gen_in1(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN1_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN2 0x1C22C
#define reg_hdcp2x_gen_in2(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN2_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN3 0x1C230
#define reg_hdcp2x_gen_in3(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN3_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN4 0x1C234
#define reg_hdcp2x_gen_in4(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN4_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN5 0x1C238
#define reg_hdcp2x_gen_in5(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN5_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN6 0x1C23C
#define reg_hdcp2x_gen_in6(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN6_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN7 0x1C240
#define reg_hdcp2x_gen_in7(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN7_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN8 0x1C244
#define reg_hdcp2x_gen_in8(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN8_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN9 0x1C248
#define reg_hdcp2x_gen_in9(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN9_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN10 0x1C24C
#define reg_hdcp2x_gen_in10(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN10_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN11 0x1C250
#define reg_hdcp2x_gen_in11(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN11_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN12 0x1C254
#define reg_hdcp2x_gen_in12(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN12_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN13 0x1C258
#define reg_hdcp2x_gen_in13(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN13_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN14 0x1C25C
#define reg_hdcp2x_gen_in14(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN14_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN15 0x1C260
#define reg_hdcp2x_gen_in15(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN15_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN16 0x1C264
#define reg_hdcp2x_gen_in16(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN16_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN17 0x1C268
#define reg_hdcp2x_gen_in17(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN17_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN18 0x1C26C
#define reg_hdcp2x_gen_in18(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN18_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN19 0x1C270
#define reg_hdcp2x_gen_in19(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN19_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN20 0x1C274
#define reg_hdcp2x_gen_in20(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN20_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN21 0x1C278
#define reg_hdcp2x_gen_in21(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN21_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN22 0x1C27C
#define reg_hdcp2x_gen_in22(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN22_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN23 0x1C280
#define reg_hdcp2x_gen_in23(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN23_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN24 0x1C284
#define reg_hdcp2x_gen_in24(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN24_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN25 0x1C288
#define reg_hdcp2x_gen_in25(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN25_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN26 0x1C28C
#define reg_hdcp2x_gen_in26(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN26_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN27 0x1C290
#define reg_hdcp2x_gen_in27(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN27_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN28 0x1C294
#define reg_hdcp2x_gen_in28(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN28_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN29 0x1C298
#define reg_hdcp2x_gen_in29(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN29_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN30 0x1C29C
#define reg_hdcp2x_gen_in30(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN30_M   (0xff << 0)

#define REG_HDCP2X_GEN_IN31 0x1C2A0
#define reg_hdcp2x_gen_in31(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_IN31_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT0 0x1C2A4
#define reg_hdcp2x_gen_out0(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT0_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT1 0x1C2A8
#define reg_hdcp2x_gen_out1(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT1_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT2 0x1C2AC
#define reg_hdcp2x_gen_out2(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT2_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT3 0x1C2B0
#define reg_hdcp2x_gen_out3(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT3_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT4 0x1C2B4
#define reg_hdcp2x_gen_out4(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT4_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT5 0x1C2B8
#define reg_hdcp2x_gen_out5(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT5_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT6 0x1C2BC
#define reg_hdcp2x_gen_out6(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT6_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT7 0x1C2C0
#define reg_hdcp2x_gen_out7(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT7_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT8 0x1C2C4
#define reg_hdcp2x_gen_out8(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT8_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT9 0x1C2C8
#define reg_hdcp2x_gen_out9(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT9_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT10 0x1C2CC
#define reg_hdcp2x_gen_out10(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT10_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT11 0x1C2D0
#define reg_hdcp2x_gen_out11(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT11_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT12 0x1C2D4
#define reg_hdcp2x_gen_out12(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT12_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT13 0x1C2D8
#define reg_hdcp2x_gen_out13(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT13_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT14 0x1C2DC
#define reg_hdcp2x_gen_out14(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT14_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT15 0x1C2E0
#define reg_hdcp2x_gen_out15(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT15_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT16 0x1C2E4
#define reg_hdcp2x_gen_out16(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT16_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT17 0x1C2E8
#define reg_hdcp2x_gen_out17(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT17_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT18 0x1C2EC
#define reg_hdcp2x_gen_out18(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT18_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT19 0x1C2F0
#define reg_hdcp2x_gen_out19(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT19_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT20 0x1C2F4
#define reg_hdcp2x_gen_out20(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT20_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT21 0x1C2F8
#define reg_hdcp2x_gen_out21(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT21_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT22 0x1C2FC
#define reg_hdcp2x_gen_out22(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT22_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT23 0x1C300
#define reg_hdcp2x_gen_out23(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT23_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT24 0x1C304
#define reg_hdcp2x_gen_out24(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT24_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT25 0x1C308
#define reg_hdcp2x_gen_out25(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT25_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT26 0x1C30C
#define reg_hdcp2x_gen_out26(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT26_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT27 0x1C310
#define reg_hdcp2x_gen_out27(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT27_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT28 0x1C314
#define reg_hdcp2x_gen_out28(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT28_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT29 0x1C318
#define reg_hdcp2x_gen_out29(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT29_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT30 0x1C31C
#define reg_hdcp2x_gen_out30(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT30_M   (0xff << 0)

#define REG_HDCP2X_GEN_OUT31 0x1C320
#define reg_hdcp2x_gen_out31(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_GEN_OUT31_M   (0xff << 0)

#define REG_HDCP2X_SEC_IN0 0x1C380)
#define reg_hdcp2x_sec_in0(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_SEC_IN0_M   (0xff << 0)

#define REG_HDCP2X_SEC_IN1 0x1C384
#define reg_hdcp2x_sec_in1(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_SEC_IN1_M   (0xff << 0)

#define REG_HDCP2X_SEC_IN2 0x1C388
#define reg_hdcp2x_sec_in2(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_SEC_IN2_M   (0xff << 0)

#define REG_HDCP2X_SEC_IN3 0x1C38C
#define reg_hdcp2x_sec_in3(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_SEC_IN3_M   (0xff << 0)

#define REG_HDCP2X_SEC_IN4 0x1C390
#define reg_hdcp2x_sec_in4(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_SEC_IN4_M   (0xff << 0)

#define REG_HDCP2X_SEC_IN5 0x1C394
#define reg_hdcp2x_sec_in5(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_SEC_IN5_M   (0xff << 0)

#define REG_HDCP2X_SEC_IN6 0x1C398
#define reg_hdcp2x_sec_in6(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_SEC_IN6_M   (0xff << 0)

#define REG_HDCP2X_SEC_IN7 0x1C39C
#define reg_hdcp2x_sec_in7(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_SEC_IN7_M   (0xff << 0)

#define REG_HDCP2X_SEC_OUT0 0x1C3A0
#define reg_hdcp2x_sec_out0(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_SEC_OUT0_M   (0xff << 0)

#define REG_HDCP2X_SEC_OUT1 0x1C3A4
#define reg_hdcp2x_sec_out1(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_SEC_OUT1_M   (0xff << 0)

#define REG_HDCP2X_SEC_OUT2 0x1C3A8
#define reg_hdcp2x_sec_out2(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_SEC_OUT2_M   (0xff << 0)

#define REG_HDCP2X_SEC_OUT3 0x1C3AC
#define reg_hdcp2x_sec_out3(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_SEC_OUT3_M   (0xff << 0)

#define REG_HDCP2X_SEC_OUT4 0x1C3B0
#define reg_hdcp2x_sec_out4(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_SEC_OUT4_M   (0xff << 0)

#define REG_HDCP2X_SEC_OUT5 0x1C3B4
#define reg_hdcp2x_sec_out5(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_SEC_OUT5_M   (0xff << 0)

#define REG_HDCP2X_SEC_OUT6 0x1C3B8
#define reg_hdcp2x_sec_out6(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_SEC_OUT6_M   (0xff << 0)

#define REG_HDCP2X_SEC_OUT7 0x1C3BC
#define reg_hdcp2x_sec_out7(x)  (((x) & 0xff) << 0)
#define REG_HDCP2X_SEC_OUT7_M   (0xff << 0)

#endif // __HAL_HDMITX_MCU_REG_H__
