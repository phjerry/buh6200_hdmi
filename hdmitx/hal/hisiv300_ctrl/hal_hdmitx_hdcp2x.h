/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi hal level hdcp2x head file.
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-11-15
 */
#ifndef __HAL_HDMITX_HDCP2X_H__
#define __HAL_HDMITX_HDCP2X_H__

#include "hi_type.h"
#include "drv_hdmitx_ioctl.h"

struct hisilicon_hdmi;

#define HDCP2X_SIZE_SW_VERSION 4
#define HDCP2X_RECVID_SIZE    5
#define HDCP2X_DEVICE_MAX_NUM 31
#define HDCP2X_RECVID_LIST_MAX_SIZE (HDCP2X_DEVICE_MAX_NUM * HDCP2X_RECVID_SIZE)
#define HDCP2X_DEPTH_MAX_SIZE 4
#define HDCP2X_RECVID_SIZE    5
#define HDCP2X_DEVICE_MAX_NUM 31
#define HDCP2X_DEPTH_MAX_SIZE 4

union hdcp2x_auth_state {
    struct {
        hi_u8 ake_init_done  :1; /* [0] */
        hi_u8 send_cert_done :1; /* [1] */
        hi_u8 km_done        :1; /* [2] */
        hi_u8 send_h_done    :1; /* [3] */
        hi_u8 pairing_done   :1; /* [4] */
        hi_u8 lc_init_done   :1; /* [5] */
        hi_u8 send_l_done    :1; /* [6] */
        hi_u8 send_sks_done  :1; /* [7] */
    } bits;
    hi_u8 u8;
};

union hdcp22_rpt_state {
    struct {
        hi_u8 id_list_done     :1; /* [0] */
        hi_u8 send_ack_done    :1; /* [1] */
        hi_u8 strm_manage_done :1; /* [2] */
        hi_u8 strm_rdy_done    :1; /* [3] */
        hi_u8 reserved         :4; /* [4~7] */
    } bits;
    hi_u8 u8;
};

union hdcp22_auth_state0 {
    struct {
        hi_u8 rxstatus_done         :1; /* [0] */
        hi_u8 calc_cert_sha256_done :1; /* [1] */
        hi_u8 check_signature_fail  :1; /* [2] */
        hi_u8 check_signature_ok    :1; /* [3] */
        hi_u8 km_is_stored          :1; /* [4] */
        hi_u8 km_isnot_stored       :1; /* [5] */
        hi_u8 calc_em_done          :1; /* [6] */
        hi_u8 calc_ekpub_km_done    :1; /* [7] */
    } bits;
    hi_u8 u8;
};

union hdcp22_auth_state1 {
    struct {
        hi_u8 ake_no_stored_km_done :1; /* [8] */
        hi_u8 ake_stored_km_done    :1; /* [9] */
        hi_u8 calc_kd_done          :1; /* [10] */
        hi_u8 calc_h_done           :1; /* [11] */
        hi_u8 cmp_h_done            :1; /* [12] */
        hi_u8 lc_init_cnt_exc       :1; /* [13] */
        hi_u8 calc_l_done           :1; /* [14] */
        hi_u8 cmp_l_done            :1; /* [15] */
    } bits;
    hi_u8 u8;
};

union hdcp22_auth_state2 {
    struct {
        hi_u8 calc_dkey2_done     :1; /* [16] */
        hi_u8 calc_edkey_ks_done  :1; /* [17] */
        hi_u8 calc_v_done         :1; /* [18] */
        hi_u8 cmp_v_done          :1; /* [19] */
        hi_u8 calc_sha256_kd_done :1; /* [20] */
        hi_u8 calc_m_done         :1; /* [21] */
        hi_u8 cmp_m_done          :1; /* [22] */
        hi_u8 enable_enc_top_done :1; /* [23] */
    } bits;
    hi_u8 u8;
};

union hdcp22_auth_state3 {
    struct {
        hi_u8 auth_state24 :1; /* [24] */
        hi_u8 auth_state25 :1; /* [25] */
        hi_u8 auth_state26 :1; /* [26] */
        hi_u8 auth_state27 :1; /* [27] */
        hi_u8 auth_state28 :1; /* [28] */
        hi_u8 auth_state29 :1; /* [29] */
        hi_u8 auth_state30 :1; /* [30] */
        hi_u8 auth_state31 :1; /* [31] */
    } bits;
    hi_u8 u8;
};

union hdcp2x_irq {
    struct {
        hi_u8 revid_ready      :1; /* mcu_intr0 ; [0]   1 - enable;  0 - disable */
        hi_u8 revid_list_ready :1; /* mcu_intr1 ; [1]   1 - enable;  0 - disable */
        hi_u8 reauth_requset   :1; /* mcu_intr2 ; [2]   1 - enable;  0 - disable */
        hi_u8 auth_fail        :1; /* mcu_intr3 ; [3]   1 - enable;  0 - disable */
        hi_u8 auth_done        :1; /* mcu_intr4 ; [4]   1 - enable;  0 - disable */
        hi_u8 stop_done        :1; /* mcu_intr5 ; [5]   1 - enable;  0 - disable */
        hi_u8 mcu_intr06       :1; /* mcu_intr6 ; [6]   1 - enable;  0 - disable */
        hi_u8 mcu_intr07       :1; /* mcu_intr7 ; [7]   1 - enable;  0 - disable */

        hi_u8 mcu_intr08       :1; /* mcu_intr8 ; [0]   1 - enable;  0 - disable */
        hi_u8 mcu_intr09       :1; /* mcu_intr9 ; [1]   1 - enable;  0 - disable */
        hi_u8 mcu_intr10       :1; /* mcu_intr10; [2]   1 - enable;  0 - disable */
        hi_u8 mcu_intr11       :1; /* mcu_intr11; [3]   1 - enable;  0 - disable */
        hi_u8 mcu_intr12       :1; /* mcu_intr12; [4]   1 - enable;  0 - disable */
        hi_u8 mcu_intr13       :1; /* mcu_intr13; [5]   1 - enable;  0 - disable */
        hi_u8 mcu_intr14       :1; /* mcu_intr14; [6]   1 - enable;  0 - disable */
        hi_u8 mcu_intr15       :1; /* mcu_intr15; [7]   1 - enable;  0 - disable */

        hi_u8 mcu_intr16       :1; /* mcu_intr16; [0]   1 - enable;  0 - disable */
        hi_u8 mcu_intr17       :1; /* mcu_intr17; [1]   1 - enable;  0 - disable */
        hi_u8 mcu_intr18       :1; /* mcu_intr18; [2]   1 - enable;  0 - disable */
        hi_u8 mcu_intr19       :1; /* mcu_intr19; [3]   1 - enable;  0 - disable */
        hi_u8 mcu_intr20       :1; /* mcu_intr20; [4]   1 - enable;  0 - disable */
        hi_u8 mcu_intr21       :1; /* mcu_intr21; [5]   1 - enable;  0 - disable */
        hi_u8 mcu_intr22       :1; /* mcu_intr22; [6]   1 - enable;  0 - disable */
        hi_u8 mcu_intr23       :1; /* mcu_intr23; [7]   1 - enable;  0 - disable */

        hi_u8 mcu_intr24       :1; /* mcu_intr24; [0]   1 - enable;  0 - disable */
        hi_u8 mcu_intr25       :1; /* mcu_intr25; [1]   1 - enable;  0 - disable */
        hi_u8 mcu_intr26       :1; /* mcu_intr26; [2]   1 - enable;  0 - disable */
        hi_u8 mcu_intr27       :1; /* mcu_intr27; [3]   1 - enable;  0 - disable */
        hi_u8 mcu_intr28       :1; /* mcu_intr28; [4]   1 - enable;  0 - disable */
        hi_u8 mcu_intr29       :1; /* mcu_intr29; [5]   1 - enable;  0 - disable */
        hi_u8 mcu_intr30       :1; /* mcu_intr30; [6]   1 - enable;  0 - disable */
        hi_u8 mcu_intr31       :1; /* mcu_intr31; [7]   1 - enable;  0 - disable */
    } bytes;
    hi_u32 u32_data;
};

union hdcp22_intr {
    struct {
        hi_u8 mcu_intr00 :1; /* [0]   1 - enable;  0 - disable */
        hi_u8 mcu_intr01 :1; /* [1]   1 - enable;  0 - disable */
        hi_u8 mcu_intr02 :1; /* [2]   1 - enable;  0 - disable */
        hi_u8 mcu_intr03 :1; /* [3]   1 - enable;  0 - disable */
        hi_u8 mcu_intr04 :1; /* [4]   1 - enable;  0 - disable */
        hi_u8 mcu_intr05 :1; /* [5]   1 - enable;  0 - disable */
        hi_u8 mcu_intr06 :1; /* [6]   1 - enable;  0 - disable */
        hi_u8 mcu_intr07 :1; /* [7]   1 - enable;  0 - disable */
    } bits;
    hi_u8 u8;
};

union hdcp22_mask {
    struct {
        hi_u8 mcu_mask00 :1; /* [0]   1 - enable;  0 - disable */
        hi_u8 mcu_mask01 :1; /* [1]   1 - enable;  0 - disable */
        hi_u8 mcu_mask02 :1; /* [2]   1 - enable;  0 - disable */
        hi_u8 mcu_mask03 :1; /* [3]   1 - enable;  0 - disable */
        hi_u8 mcu_mask04 :1; /* [4]   1 - enable;  0 - disable */
        hi_u8 mcu_mask05 :1; /* [5]   1 - enable;  0 - disable */
        hi_u8 mcu_mask06 :1; /* [6]   1 - enable;  0 - disable */
        hi_u8 mcu_mask07 :1; /* [7]   1 - enable;  0 - disable */
    } bits;
    hi_u8 u8;
};

struct hdcp2x_rx_info {
    hi_u8 depth;
    hi_u8 dev_cnt;
    hi_bool max_devs_exceeded;
    hi_bool max_cascade_exceeded;
    hi_bool max_hdcp20_down_stream;
    hi_bool max_hdcp1x_down_stream;
};

struct hdcp2x_stream_manage {
    hi_u8 seq_num_m0;
    hi_u8 seq_num_m1;
    hi_u8 seq_num_m2;
    hi_u8 k0;
    hi_u8 k1;
    hi_u8 stream_id;
    hi_u8 stream_type;
};

struct hdcp2x_sink_status {
    struct hdcp2x_rx_info rx_info;
    hi_u8 recv_id[HDCP2X_RECVID_SIZE];
    hi_u8 recv_id_list[HDCP2X_RECVID_LIST_MAX_SIZE];
};

struct hdcp2x_hal_ops {
    hi_s32 (*load_mcu_code)(struct hisilicon_hdmi *hdmi);
    void (*set_2x_srst)(struct hisilicon_hdmi *hdmi, hi_bool enable);
    void (*set_mode)(struct hisilicon_hdmi *hdmi);
    void (*set_mcu_start)(struct hisilicon_hdmi *hdmi, hi_bool start);
    void (*set_mcu_srst)(struct hisilicon_hdmi *hdmi, hi_bool enable);
    void (*clear_irq)(struct hisilicon_hdmi *hdmi, hi_u32 data);
    void (*set_irq)(struct hisilicon_hdmi *hdmi, hi_bool enable);
    void (*get_irq_status)(struct hisilicon_hdmi *hdmi, hi_u32 *data);
    void (*set_stream_msg)(struct hisilicon_hdmi *hdmi, struct hdcp2x_stream_manage *);
    void (*set_hw_version)(struct hisilicon_hdmi *hdmi);
    void (*get_sw_version)(struct hisilicon_hdmi *hdmi, hi_u8 *ver, hi_u32 size);
    void (*get_rx_info)(struct hisilicon_hdmi *hdmi, struct hdcp2x_rx_info *rx_info);
    void (*get_recv_id)(struct hisilicon_hdmi *hdmi, hi_u8*, hi_u32);
    hi_s32 (*get_recv_id_list)(struct hisilicon_hdmi *hdmi, hi_u8*, hi_u32);
    void (*get_mcu_cap)(struct hisilicon_hdmi *hdmi);
    void (*get_auth_follow)(struct hisilicon_hdmi *hdmi);
    void (*get_sink_capability)(struct hisilicon_hdmi *hdmi, struct hdcp_cap *cap);
    void (*set_encryption)(struct hisilicon_hdmi *hdmi, hi_bool enable);
    void (*set_sync_mode)(struct hisilicon_hdmi *hdmi, hi_bool enable);
    hi_bool (*is_ddc_idle)(struct hisilicon_hdmi *hdmi);
};

struct hdcp2x_hal_ops *hal_hdmitx_hdcp2x_get_ops(hi_void);

#endif /* __HAL_HDMITX_HDCP2X_H__ */
