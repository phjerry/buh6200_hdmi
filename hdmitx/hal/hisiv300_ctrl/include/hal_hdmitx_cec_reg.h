/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: header of hdmitx cec regiser.
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-11-07
 */

#ifndef __HAL_HDMITX_CEC_REG_H__
#define __HAL_HDMITX_CEC_REG_H__

#define REG_CEC_IRQ_SRC_PND         0x0000
#define REG_UNKNOWN_BRCST_SRC       (1 << 13)
#define REG_FA_OK_SRC               (1 << 12)
#define REG_VALUE_ABNM_SRC          (1 << 11)
#define REG_LOGIC_ABNM_SRC          (1 << 10)
#define REG_START_ABNM_SRC          (1 << 9)
#define REG_AUTO_PING_RDY_SRC       (1 << 8)
#define REG_FA_RETRY_OUT_SRC        (1 << 7)
#define REG_TX1_FRAME_RETRY_OUT_SRC (1 << 6)
#define REG_TX0_FRAME_RETRY_OUT_SRC (1 << 5)
#define REG_TX1_FRAME_RDY_SRC       (1 << 4)
#define REG_TX0_FRAME_RDY_SRC       (1 << 3)
#define REG_FA_REQ_SRC              (1 << 2)
#define REG_RX_BUFFER_OV_SRC        (1 << 1)
#define REG_RX_FRAME_RDY_SRC        (1 << 0)

#define REG_CEC_IRQ_EN_SET          0x0004
#define REG_UNKNOWN_BRCST_SRC_MASK  (1 << 13)
#define REG_FA_OK_SRC_MASK          (1 << 12)
#define REG_VALUE_ABNM_SRC_MASK     (1 << 11)
#define REG_LOGIC_ABNM_SRC_MASK     (1 << 10)
#define REG_START_ABNM_SRC_MASK     (1 << 9)
#define REG_AUTO_PING_RDY_SRC_MASK  (1 << 8)
#define REG_FA_RETRY_OUT_SRC_MASK   (1 << 7)
#define REG_TX1_FRAME_RETRY_OUT_SRC_MASK (1 << 6)
#define REG_TX0_FRAME_RETRY_OUT_SRC_MASK (1 << 5)
#define REG_TX1_FRAME_RDY_SRC_MASK  (1 << 4)
#define REG_TX0_FRAME_RDY_SRC_MASK  (1 << 3)
#define REG_FA_REQ_SRC_MASK         (1 << 2)
#define REG_RX_BUFFER_OV_SRC_MASK   (1 << 1)
#define REG_RX_FRAME_RDY_SRC_MASK   (1 << 0)

#define REG_CEC_CTRL                0x0008
#define REG_CFG_LAST_ACK1_EN        (1 << 11)
#define REG_CFG_DATA_EOM_ARB_EN     (1 << 10)
#define REG_CFG_FA_RCV_EN           (1 << 9)
#define REG_CFG_ERROR_NOTIFICATION_EN (1 << 8)
#define REG_RX_FULL_NACK_BRCST      (1 << 7)
#define REG_DEGLITCH_BYPASS         (1 << 6)
#define REG_RCV_NACK_HDR            (1 << 5)
#define REG_BRCST_INIT_RCV_EN       (1 << 4)
#define REG_FEATURE_ABORT_EN        (1 << 3)
#define REG_SNOOP_EN                (1 << 2)
#define REG_CEC_TRAN_EN             (1 << 1)
#define REG_CEC_RCV_EN              (1 << 0)

#define REG_CEC_PULSE               0x000C
#define REG_RX_FRAME_CLR_ALL        (1 << 11)
#define REG_RX_STATE_CLR            (1 << 10)
#define REG_TX_STATE_CLR            (1 << 9)
#define REG_TX1_BUFFER_FLUSH        (1 << 8)
#define REG_TX0_BUFFER_FLUSH        (1 << 7)
#define REG_AUTO_PING_EN            (1 << 6)
#define REG_RX_FRAME_CLR            (1 << 4)
#define REG_TX1_POLL_MESSAGE_EN     (1 << 3)
#define REG_TX1_FRAME_EN            (1 << 2)
#define REG_TX0_POLL_MESSAGE_EN     (1 << 1)
#define REG_TX0_FRAME_EN            (1 << 0)

#define REG_CEC_RETRY_NUM           0x0010
#define reg_ping_retry_num(x)       (((x) & 0x7) << 4) /* [6:4] */
#define REG_PING_RETRY_NUM_M        (0x7 << 4)
#define reg_retry_num(x)            (((x) & 0x7) << 0) /* [2:0] */
#define REG_RETRY_NUM_M             (0x7  << 0)

#define REG_CEC_CAPTURE_ID_FLAG     0x0014
#define reg_capture_id_flag(x)      (((x) & 0xffff) << 0) /* [15:0] */
#define REG_CAPTURE_ID_FLAG_M       (0xffff << 0)

#define REG_CEC_SNOOP_ID_FLAG       0x0018
#define reg_snoop_id_flag(x)        (((x) & 0xffff) << 0) /* [15:0] */
#define REG_SNOOP_ID_FLAG_M         (0xffff << 0)

#define REG_CEC_CDC                 0x001C
#define REG_CEC_CDC_ARB_EN          (1 << 13)
#define reg_cec_cdc_arb_cnt(x)      (((x) & 0x1f) << 8) /* [12:8] */
#define REG_CEC_CDC_ARB_CNT_M       (0x1f << 8)
#define reg_cec_cdc_opcode(x)       (((x) & 0xff) << 0) /* [7:0] */
#define REG_CEC_CDC_OPCODE_M        (0xff << 0)

#define REG_CEC_AUTO_PING_MAP_RSLT  0x0020
#define reg_auto_ping_map(x)        (((x) & 0x7fff) << 0) /* [14:0] */
#define REG_AUTO_PING_MAP_M         (0x7fff << 0)

#define REG_CEC_AUTO_PING_MAP_EN    0x0024
#define reg_auto_ping_map_en(x)     (((x) & 0x7fff) << 0) /* [14:0] */
#define REG_AUTO_PING_MAP_EN_M      (7ffff  << 0)

#define REG_CEC_VERSION             0x0028
#define reg_cec_drv_version(x)      (((x) & 0xff) << 16) /* [23:16] */
#define reg_cec_reg_version(x)      (((x) & 0xff) << 8) /* [15:8] */
#define reg_cec_rtl_version(x)      (((x) & 0xff) << 0) /* [7:0] */

#define REG_CEC_COMPENSATE_R        0x002C
#define reg_cec_compensate(x)       (((x) & 0x1ff) << 0) /* [8:0] */
#define REG_CEC_COMPENSATE_M        (0x1ff << 0)

#define REG_CEC_FA_DEBUG            0x0030
#define reg_fa_opcode(x)            (((x) & 0xff) << 8) /* [15:8] */
#define REG_FA_OPCODE_M             (0xff << 8)
#define reg_fa_init_addr(x)         (((x) & 0xf) << 4) /* [7:4] */
#define REG_FA_INIT_ADDR_M          (0xf << 4)
#define reg_fa_dest_addr(x)         (((x) & 0xf) << 0) /* [3:0] */
#define REG_FA_DEST_ADDR_M          (0xf << 0)

#define reg_cec_fa_opcode(s)        (0x0034 + 4 * (s))
#define reg_fa_opcode_flag(x)       (((x) & 0xffffffff) << 0) /* [31:0] */
#define REG_FA_OPCODE_FLAG_M        (0xffffffff << 0) /* [31:0] */

#define REG_CEC_UMONITOR0           0x0054
#define reg_ctrl_state(x)           (((x) & 0x7) << 19) /* [21:19] */
#define REG_CTRL_STATE_M            (0x7 << 19)
#define reg_cec_rx_state(x)         (((x) & 0xf) << 14) /* [17:14] */
#define REG_CEC_RX_STATE_M          (0xf << 14)
#define reg_cec_tx_state(x)         (((x) & 0x1f) << 9) /* [13:9] */
#define REG_CEC_TX_STATE_M          (0x1f << 9)
#define reg_retry_cnt(x)            (((x) & 0x7) << 6) /* [8:6] */
#define REG_RETRY_CNT_M             (0x7 << 6)
#define REG_RX_BUFFER3_ST           (1 << 5)
#define REG_RX_BUFFER2_ST           (1 << 4)
#define REG_RX_BUFFER1_ST           (1 << 3)
#define REG_RX_BUFFER0_ST           (1 << 2)
#define REG_TX1_BUFFER_ST           (1 << 1)
#define REG_TX0_BUFFER_ST           (1 << 0)

#define REG_CEC_UMONITOR1           0x0058
#define REG_TX_DATA_EOM_ARBI_FAIL   (1 << 20)
#define REG_TX_START_LOW_LONG       (1 << 19)
#define REG_ERROR_NOTIFICATION_DET  (1 << 18)
#define REG_TX_EOM_VALUE_ERROR      (1 << 17)
#define REG_AP_NO_ACK               (1 << 16)
#define REG_FA_NO_ACK               (1 << 15)
#define REG_TX1_POLL_NO_ACK         (1 << 14)
#define REG_TX0_POLL_NO_ACK         (1 << 13)
#define REG_TX1_NO_ACK              (1 << 12)
#define REG_TX0_NO_ACK              (1 << 11)
#define REG_ARBI_FAIL               (1 << 10)
#define REG_LOGIC_ERROR             (1 << 9)
#define REG_LOGIC_BIT_SHORT         (1 << 8)
#define REG_LOGIC_BIT_LONG          (1 << 7)
#define REG_LOGIC1_LOW_SHORT        (1 << 6)
#define REG_LOGIC_LOW_MIDDLE        (1 << 5)
#define REG_LOGIC0_LOW_LONG         (1 << 4)
#define REG_START_BIT_SHORT         (1 << 3)
#define REG_START_BIT_LONG          (1 << 2)
#define REG_START_LOW_SHORT         (1 << 1)
#define REG_START_LOW_LONG          (1 << 0)

#define REG_CEC_RESERVED             0x0080

#define REG_CEC_TX0_HEAD_ADDR        0x0100
#define reg_tx0_init_addr(x)         (((x) & 0xf) << 4) /* [7:4] */
#define REG_TX0_INIT_ADDR_M          (0xf << 4)
#define reg_tx0_dest_addr(x)         (((x) & 0xf) << 0) /* [3:0] */
#define REG_TX0_DEST_ADDR_M          (0xf << 0)

#define REG_CEC_TX0_FRAME_LENGTH     0x0104
#define reg_tx0_frame_length(x)      (((x) & 0xf) << 0) /* [3:0] */
#define REG_TX0_FRAME_LENGTH_M       (0xf << 0)

#define REG_CEC_TX0_FRAME_DATA0      0x0108
#define reg_tx0_frame_data_byte3(x)  (((x) & 0xff) << 24) /* [31:24] */
#define REG_TX0_FRAME_DATA_BYTE3_M   (0xff << 24)
#define reg_tx0_frame_data_byte2(x)  (((x) & 0xff) << 16) /* [23:16] */
#define REG_TX0_FRAME_DATA_BYTE2_M   (0xff << 16)
#define reg_tx0_frame_data_byte1(x)  (((x) & 0xff) << 8) /* [15:8] */
#define REG_TX0_FRAME_DATA_BYTE1_M   (0xff << 8)
#define reg_tx0_frame_data_byte0(x)  (((x) & 0xff) << 0) /* [7:0] */
#define REG_TX0_FRAME_DATA_BYTE0_M   (0xff << 0)

#define REG_CEC_TX0_FRAME_DATA1      0x010C
#define reg_tx0_frame_data_byte7(x)  (((x) & 0xff) << 24) /* [31:24] */
#define REG_TX0_FRAME_DATA_BYTE7_M   (0xff << 24)
#define reg_tx0_frame_data_byte6(x)  (((x) & 0xff) << 16) /* [23:16] */
#define REG_TX0_FRAME_DATA_BYTE6_M   (0xff << 16)
#define reg_tx0_frame_data_byte5(x)  (((x) & 0xff) << 8) /* [15:8] */
#define REG_TX0_FRAME_DATA_BYTE5_M   (0xff << 8)
#define reg_tx0_frame_data_byte4(x)  (((x) & 0xff) << 0) /* [7:0] */
#define REG_TX0_FRAME_DATA_BYTE4_M   (0xff << 0)

#define REG_CEC_TX0_FRAME_DATA2      0x0110
#define reg_tx0_frame_data_byte11(x) (((x) & 0xff) << 24) /* [31:24] */
#define REG_TX0_FRAME_DATA_BYTE11_M  (0xff << 24)
#define reg_tx0_frame_data_byte10(x) (((x) & 0xff) << 16) /* [23:16] */
#define REG_TX0_FRAME_DATA_BYTE10_M  (0xff << 16)
#define reg_tx0_frame_data_byte9(x)  (((x) & 0xff) << 8) /* [15:8] */
#define REG_TX0_FRAME_DATA_BYTE9_M   (0xff << 8)
#define reg_tx0_frame_data_byte8(x)  (((x) & 0xff) << 0) /* [7:0] */
#define REG_TX0_FRAME_DATA_BYTE8_M   (0xff << 0)

#define REG_CEC_TX0_FRAME_DATA3      0x0114
#define reg_tx0_frame_data_byte14(x) (((x) & 0xff) << 16) /* [23:16] */
#define REG_TX0_FRAME_DATA_BYTE14_M  (0xff << 16)
#define reg_tx0_frame_data_byte13(x) (((x) & 0xff) << 8) /* [15:8] */
#define REG_TX0_FRAME_DATA_BYTE13_M  (0xff << 8)
#define reg_tx0_frame_data_byte12(x) (((x) & 0xff) << 0) /* [7:0] */
#define REG_TX0_FRAME_DATA_BYTE12_M  (0xff << 0)

#define REG_CEC_TX1_HEAD_ADDR        0x0120
#define reg_tx1_init_addr(x)         (((x) & 0xf) << 4) /* [7:4] */
#define REG_TX1_INIT_ADDR_M          (0xf << 4)
#define reg_tx1_dest_addr(x)         (((x) & 0xf) << 0) /* [3:0] */
#define REG_TX1_DEST_ADDR_M          (0xf << 0)

#define REG_CEC_TX1_FRAME_LENGTH     0x0124
#define reg_tx1_frame_length(x)      (((x) & 0xf) << 0) /* [3:0] */
#define REG_TX1_FRAME_LENGTH_M       (0xf << 0)

#define REG_CEC_TX1_FRAME_DATA0      0x0128
#define reg_tx1_frame_data_byte3(x)  (((x) & 0xff) << 24) /* [31:24] */
#define REG_TX1_FRAME_DATA_BYTE3_M   (0xff << 24)
#define reg_tx1_frame_data_byte2(x)  (((x) & 0xff) << 16) /* [23:16] */
#define REG_TX1_FRAME_DATA_BYTE2_M   (0xff << 16)
#define reg_tx1_frame_data_byte1(x)  (((x) & 0xff) << 8) /* [15:8] */
#define REG_TX1_FRAME_DATA_BYTE1_M   (0xff << 8)
#define reg_tx1_frame_data_byte0(x)  (((x) & 0xff) << 0) /* [7:0] */
#define REG_TX1_FRAME_DATA_BYTE0_M   (0xff << 0)

#define REG_CEC_TX1_FRAME_DATA1      0x012C
#define reg_tx1_frame_data_byte7(x)  (((x) & 0xff) << 24) /* [31:24] */
#define REG_TX1_FRAME_DATA_BYTE7_M   (0xff << 24)
#define reg_tx1_frame_data_byte6(x)  (((x) & 0xff) << 16) /* [23:16] */
#define REG_TX1_FRAME_DATA_BYTE6_M   (0xff << 16)
#define reg_tx1_frame_data_byte5(x)  (((x) & 0xff) << 8) /* [15:8] */
#define REG_TX1_FRAME_DATA_BYTE5_M   (0xff << 8)
#define reg_tx1_frame_data_byte4(x)  (((x) & 0xff) << 0) /* [7:0] */
#define REG_TX1_FRAME_DATA_BYTE4_M   (0xff << 0)

#define REG_CEC_TX1_FRAME_DATA2      0x0130
#define reg_tx1_frame_data_byte11(x) (((x) & 0xff) << 24) /* [31:24] */
#define REG_TX1_FRAME_DATA_BYTE11_M  (0xff << 24)
#define reg_tx1_frame_data_byte10(x) (((x) & 0xff) << 16) /* [23:16] */
#define REG_TX1_FRAME_DATA_BYTE10_M  (0xff << 16)
#define reg_tx1_frame_data_byte9(x)  (((x) & 0xff) << 8) /* [15:8] */
#define REG_TX1_FRAME_DATA_BYTE9_M   (0xff << 8)
#define reg_tx1_frame_data_byte8(x)  (((x) & 0xff) << 0) /* [7:0] */
#define REG_TX1_FRAME_DATA_BYTE8_M   (0xff << 0)

#define REG_CEC_TX1_FRAME_DATA3      0x0134
#define reg_tx1_frame_data_byte14(x) (((x) & 0xff) << 16) /* [23:16] */
#define REG_TX1_FRAME_DATA_BYTE14_M  (0xff << 16)
#define reg_tx1_frame_data_byte13(x) (((x) & 0xff) << 8) /* [15:8] */
#define REG_TX1_FRAME_DATA_BYTE13_M  (0xff << 8)
#define reg_tx1_frame_data_byte12(x) (((x) & 0xff) << 0) /* [7:0] */
#define REG_TX1_FRAME_DATA_BYTE12_M  (0xff << 0)

#define REG_CEC_RX_FRAME_LENGTH      0x0140
#define reg_rx_frame_length(x)       (((x) & 0xf) << 0) /* [3:0] */
#define REG_RX_FRAME_LENGTH_M        (0xf << 0)

#define REG_CEC_RX_HEAD_ADDR         0x0144
#define reg_rx_init_addr(x)          (((x) & 0xf) << 4) /* [7:4] */
#define REG_RX_INIT_ADDR_M           (0xf << 4)
#define reg_rx_dest_addr(x)          (((x) & 0xf) << 0) /* [3:0] */
#define REG_RX_DEST_ADDR_M           (0xf << 0)

#define REG_CEC_RX_FRAME_DATA0       0x0148
#define reg_rx_frame_data_byte3(x)   (((x) & 0xff) << 24) /* [31:24] */
#define REG_RX_FRAME_DATA_BYTE3_M    (0xff << 24)
#define reg_rx_frame_data_byte2(x)   (((x) & 0xff) << 16) /* [23:16] */
#define REG_RX_FRAME_DATA_BYTE2_M    (0xff << 16)
#define reg_rx_frame_data_byte1(x)   (((x) & 0xff) << 8) /* [15:8] */
#define REG_RX_FRAME_DATA_BYTE1_M    (0xff << 8)
#define reg_rx_frame_data_byte0(x)   (((x) & 0xff) << 0) /* [7:0] */
#define REG_RX_FRAME_DATA_BYTE0_M    (0xff << 0)

#define REG_CEC_RX_FRAME_DATA1       0x014C
#define reg_rx_frame_data_byte7(x)   (((x) & 0xff) << 24) /* [31:24] */
#define REG_RX_FRAME_DATA_BYTE7_M    (0xff << 24)
#define reg_rx_frame_data_byte6(x)   (((x) & 0xff) << 16) /* [23:16] */
#define REG_RX_FRAME_DATA_BYTE6_M    (0xff << 16)
#define reg_rx_frame_data_byte5(x)   (((x) & 0xff) << 8) /* [15:8] */
#define REG_RX_FRAME_DATA_BYTE5_M    (0xff << 8)
#define reg_rx_frame_data_byte4(x)   (((x) & 0xff) << 0) /* [7:0] */
#define REG_RX_FRAME_DATA_BYTE4_M    (0xff << 0)

#define REG_CEC_RX_FRAME_DATA2       0x0150
#define reg_rx_frame_data_byte11(x)  (((x) & 0xff) << 24) /* [31:24] */
#define REG_RX_FRAME_DATA_BYTE11_M    (0xff << 24)
#define reg_rx_frame_data_byte10(x)  (((x) & 0xff) << 16) /* [23:16] */
#define REG_RX_FRAME_DATA_BYTE10_M    (0xff << 16)
#define reg_rx_frame_data_byte9(x)   (((x) & 0xff) << 8) /* [15:8] */
#define REG_RX_FRAME_DATA_BYTE9_M    (0xff << 8)
#define reg_rx_frame_data_byte8(x)   (((x) & 0xff) << 0) /* [7:0] */
#define REG_RX_FRAME_DATA_BYTE8_M    (0xff << 0)

#define REG_CEC_RX_FRAME_DATA3       0x0154
#define reg_rx_frame_data_byte14(x)  (((x) & 0xff) << 16) /* [23:16] */
#define REG_RX_FRAME_DATA_BYTE14_M    (0xff << 16)
#define reg_rx_frame_data_byte13(x)  (((x) & 0xff) << 8) /* [15:8] */
#define REG_RX_FRAME_DATA_BYTE13_M    (0xff << 8)
#define reg_rx_frame_data_byte12(x)  (((x) & 0xff) << 0) /* [7:0] */
#define REG_RX_FRAME_DATA_BYTE12_M    (0xff << 0)

#define REG_CEC_RX_FRAME_CNT         0x0158
#define reg_rx_frame_cnt(x)          (((x) & 0x7) << 0) /* [2:0] */
#define REG_RX_FRAME_CNT_M           (0x7 << 0)

#define REG_CEC_LOW_MIN              0x015C
#define reg_cec_low_min_cnt(x)       (((x) & 0x3ffff) << 0) /* [17:0] */

#define REG_CEC_LOW_MAX              0x0160
#define reg_cec_low_max_cnt(x)       (((x) & 0x3ffff) << 0) /* [17:0] */

#define REG_CEC_HIGH_MIN             0x0164
#define reg_cec_high_min_cnt(x)      (((x) & 0x1ffff) << 0) /* [16:0] */

#define REG_CEC_CNT_CLR              0x0168
#define CFG_CEC_CNT_CLR              (1 << 0)

#define REG_CEC_RCV_WAIT_CNT         0x016C
#define reg_cfg_rcv_wait_cnt(x)      (((x) & 0x7ffff) << 0) /* [18:0] */

#define REG_CEC_ERROR_NOTIFICATION_CNT          0x0170
#define reg_cfg_error_notification_cnt(x)       (((x) & 0x3ffff) << 0) /* [17:0] */

#define REG_CEC_ERROR_NOTIFICATION_CNT_TX       0x0174
#define reg_cfg_transmit_error_notification_cnt(x) (((x) & 0x3ffff) << 0) /* [17:0] */

/* crg srst register */
#define REG_HDMITX_CRG_CLK_SRST                 0x00
#define REG_HDMITX_CTRL_CEC_SRST_REQ            (1 << 3)
#define REG_HDMITX_CTRL_CEC_CKEN                (1 << 0)

/* general function registers */
#define REG_SC_HDMITX_CPU_CEC_INFO_0            0x00
#define reg_cec_switch_info_pa_ab(x)            (((x) & 0xff) << 24) /* [31:24] */
#define REG_CEC_SWITCH_INFO_PA_AB_M             (0xff << 24)
#define reg_cec_switch_info_pa_cd(x)            (((x) & 0xff) << 16) /* [23:16] */
#define REG_CEC_SWITCH_INFO_PA_CD_M             (0xff << 16)
#define reg_cec_switch_info_la(x)               (((x) & 0xf) << 12)  /* [15:12] */
#define REG_CEC_SWITCH_INFO_LA_M                (0xf << 12)
#define reg_cec_switch_info_device_type(x)      (((x) & 0xf) << 8) /* [11:8] */
#define REG_CEC_SWITCH_INFO_DEVICE_TYPE_M       (0xf << 8)
#define reg_cec_backup_la(x)                    (((x) & 0xf) << 4)  /* [7:4] */
#define REG_CEC_BACKUP_LA_M                     (0xf << 4)
#define reg_cec_switch_info_wakeup_aduio_system(x) (((x) & 0x01) << 1)
#define REG_CEC_SWITCH_INFO_WAKEUP_ADUIO_SYSTEM_M  (1 << 1)
#define reg_cec_switch_info_enable(x)           (((x) & 0x01) << 0)
#define REG_CEC_SWITCH_INFO_ENABLE_M            (1 << 0)

#define REG_SC_HDMITX_CPU_CEC_INFO_1            0x04
#define reg_cec_switch_info_vendor_id2(x)       (((x) & 0xff) << 24) /* [31:24] */
#define REG_CEC_SWITCH_INFO_VENDOR_ID2_M        (0xff << 24)
#define reg_cec_switch_info_vendor_id1(x)       (((x) & 0xff) << 16) /* [23:16] */
#define REG_CEC_SWITCH_INFO_VENDOR_ID1_M        (0xff << 16)
#define reg_cec_switch_info_vendor_id0(x)       (((x) & 0xff) << 8)  /* [15:8] */
#define REG_CEC_SWITCH_INFO_VENDOR_ID0_M        (0xff << 8)
#define reg_cec_switch_info_osd_name13(x)       (((x) & 0xff) << 0)  /* [7:0] */
#define REG_CEC_SWITCH_INFO_OSD_NAME13_M        (0xff << 0)

#define REG_SC_HDMITX_CPU_CEC_INFO_2            0x08
#define reg_cec_switch_info_osd_name12(x)       (((x) & 0xff) << 24) /* [31:24] */
#define REG_CEC_SWITCH_INFO_OSD_NAME12_M        (0xff << 24)
#define reg_cec_switch_info_osd_name11(x)       (((x) & 0xff) << 16) /* [23:16] */
#define REG_CEC_SWITCH_INFO_OSD_NAME11_M        (0xff << 16)
#define reg_cec_switch_info_osd_name10(x)       (((x) & 0xff) << 8)  /* [15:8] */
#define REG_CEC_SWITCH_INFO_OSD_NAME10_M        (0xff << 8)
#define reg_cec_switch_info_osd_name9(x)        (((x) & 0xff) << 0)   /* [7:0] */
#define REG_CEC_SWITCH_INFO_OSD_NAME9_M         (0xff << 0)

#define REG_SC_HDMITX_CPU_CEC_INFO_3            0x0c
#define reg_cec_switch_info_osd_name8(x)        (((x) & 0xff) << 24) /* [31:24] */
#define REG_CEC_SWITCH_INFO_OSD_NAME8_M         (0xff << 24)
#define reg_cec_switch_info_osd_name7(x)        (((x) & 0xff) << 16) /* [23:16] */
#define REG_CEC_SWITCH_INFO_OSD_NAME7_M         (0xff << 16)
#define reg_cec_switch_info_osd_name6(x)        (((x) & 0xff) << 8)   /* [15:8] */
#define REG_CEC_SWITCH_INFO_OSD_NAME6_M         (0xff << 8)
#define reg_cec_switch_info_osd_name5(x)        (((x) & 0xff) << 0)   /* [7:0] */
#define REG_CEC_SWITCH_INFO_OSD_NAME5_M         (0xff << 0)

#define REG_SC_HDMITX_CPU_CEC_INFO_4            0x010
#define reg_cec_switch_info_osd_name4(x)        (((x) & 0xff) << 24) /* [31:24] */
#define REG_CEC_SWITCH_INFO_OSD_NAME4_M         (0xff << 24)
#define reg_cec_switch_info_osd_name3(x)        (((x) & 0xff) << 16) /* [23:16] */
#define REG_CEC_SWITCH_INFO_OSD_NAME3_M         (0xff << 16)
#define reg_cec_switch_info_osd_name2(x)        (((x) & 0xff) << 8)   /* [15:8] */
#define REG_CEC_SWITCH_INFO_OSD_NAME2_M         (0xff << 8)
#define reg_cec_switch_info_osd_name1(x)        (((x) & 0xff) << 0)   /* [7:0] */
#define REG_CEC_SWITCH_INFO_OSD_NAME1_M         (0xff << 0)

#define REG_SC_HDMITX_CPU_CEC_INFO_5            0x014
#define reg_cec_switch_info_osd_name0(x)        (((x) & 0xff) << 24) /* [31:24] */
#define REG_CEC_SWITCH_INFO_OSD_NAME0_M         (0xff << 24)

#define REG_SC_HDMITX_CPU_CEC_INFO_6            0x018
#define REG_SC_HDMITX_CPU_CEC_INFO_7            0x01c

#endif

