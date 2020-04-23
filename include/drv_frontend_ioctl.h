/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description:
 * Author:
 * Created: 2013-12-16
 */

#ifndef __DRV_FRONTEND_IOCTL_H__
#define __DRV_FRONTEND_IOCTL_H__

#include "hi_drv_frontend.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */

#define QAM_PORT0_ADDR 0xA0
#define QAM_PORT1_ADDR 0xA2
#define QAM_PORT2_ADDR 0xA0
#define QAM_PORT3_ADDR 0xA2
#define QAM_PORT4_ADDR 0xA0
#define FRONTEND_PORT0_ADDR 0xC0
#define FRONTEND_PORT1_ADDR 0xC0
#define FRONTEND_PORT2_ADDR 0xC0
#define FRONTEND_PORT3_ADDR 0xC0
#define FRONTEND_PORT4_ADDR 0xC0


typedef enum {
    FRONTEND_DATA_SRC_ADC = 0,
    FRONTEND_DATA_SRC_EQU,
    FRONTEND_DATA_SRC_CIR_PLOT,
    FRONTEND_DATA_SRC_BUTT
} frontend_data_src;

typedef enum {
    QAM_TYPE_16 = 0,
    QAM_TYPE_32,
    QAM_TYPE_64,
    QAM_TYPE_128,
    QAM_TYPE_256,
    QAM_TYPE_AUTO,
    QAM_TYPE_BUTT
} frontend_qam_type;

typedef struct {
    hi_u8                       plp_id;            /* PLP id */
    hi_u8                       comm_plp_id;       /* common PLP id */
    hi_u8                       combination;       /* PLP combination */
    hi_drv_frontend_dvbt2_mode  channel_attr;      /* dvb-t2 */
} frontend_dvbt2_para;

typedef struct {
    hi_u32 frequency; /* frequency khz */
    union {
        hi_u32 symbol_rate; /* symbolrate hz */
        hi_u32 band_width; /* band_width khz */
    } srbw;
    union {
        frontend_qam_type qam_type;
        hi_drv_frontend_polarization polar;
    } modpol;
    hi_u32 scramble_code;
    hi_bool si; /* spectrum reverse */
    union {
        frontend_dvbt2_para dvbt2;
        hi_drv_frontend_dvbt_ts_priority dvbt;
    } ter;
    hi_u8 dvbt_mode; /* 0:dvb-t2;1:dvb-t */
    hi_u32 tuner_band_width;
    hi_drv_frontend_lock_status sync_lock_status; /* sync locking status */
    hi_u16 stream_id;
    hi_u16 stream_id_type;
} frontend_acc_qam_params;

typedef hi_s32 (*fe_ioctrl_func)(hi_void *arg);

typedef struct {
    hi_u32 cmd;
    fe_ioctrl_func func;
} fe_ioctrl_func_list;

typedef struct {
    hi_u32 port;
    hi_u32 data;
} fe_ioctrl_data;

typedef struct {
    hi_u32 port;
    hi_u32 data_buf[3]; /* 3:size */
} fe_ioctrl_data_buf;

typedef struct {
    hi_u32         port;
    hi_drv_frontend_signal_info info;
} fe_ioctrl_signal_info;

typedef struct {
    hi_u32                  port;
    hi_drv_frontend_sat_attr sat_attr;
} fe_ioctrl_sat_attr;

#define MAX_TP_NUM (100)

typedef struct {
    hi_u32 data; /* for extend */
    hi_u32 if_start_freq;
    hi_u32 if_stop_freq;
} frontend_blindscan_initpara;

typedef struct {
    hi_u32 port;
    frontend_blindscan_initpara para;
} fe_ioctrl_blindscan;

typedef struct {
    hi_u32 centre_freq; /* blind scan centre frequency, k_hz */
    hi_u16 count; /* channel find in this scanning */
    union {
        hi_drv_frontend_sat_tp_info sat[MAX_TP_NUM];
    } result;
} frontend_blindscan_para;

typedef struct {
    hi_u32 port;
    frontend_blindscan_para para;
} fe_ioctrl_blindscan_info;

typedef struct {
    hi_u32          port;
    hi_u32          stop_quit;             /* Blindscan Quit Flag */
} fe_ioctrl_blindscan_abort;

typedef enum {
    funct_mode_demod = 0,
    funct_mode_blind_scan = 1
} frontend_func_mode;

/* LNB out voltage enum */
typedef enum {
    FRONTEND_LNB_OUT_0V = 0,
    FRONTEND_LNB_OUT_13V = 13,
    FRONTEND_LNB_OUT_14V = 14, /* 14 or 15V */
    FRONTEND_LNB_OUT_18V = 18,
    FRONTEND_LNB_OUT_19V = 19, /* 19 or 20V */
    FRONTEND_LNB_OUT_BUTT
} frontend_lnb_out_level;

/* LNB out voltage struct */
typedef struct {
    hi_u32 port;
    frontend_lnb_out_level out;
} fe_ioctrl_lnb_out;

/* diseqc send message */
typedef struct {
    hi_u32 port;
    hi_drv_frontend_diseqc_send_msg send_msg;
} fe_ioctrl_diseqc_sendmsg;

typedef struct {
    hi_u32 port;
    hi_drv_frontend_diseqc_recv_msg recv_msg;
} fe_ioctrl_diseqc_recvmsg;

typedef struct {
    hi_drv_frontend_sig_type sig_type;
    hi_u32 port;
    frontend_acc_qam_params signal;
} fe_ioctrl_signal;

typedef struct {
    hi_u32 port;
    hi_u32 stream_num;
} fe_ioctrl_get_stream_num;

typedef struct {
    hi_u32 port;
    hi_u8 isi_id;
} fe_ioctrl_set_isi_id;

typedef struct {
    hi_u32 port;
    hi_u8 index;
    hi_u8 isi_id;
} fe_ioctrl_get_isi_id;

typedef struct {
    hi_u32 port;
} fe_ioctrl_unicable_scan_para;

typedef struct {
    hi_u32 frequency; /* <frequency k_hz */
    hi_u32 symbol_rate; /* <symbolrate k_baud */
    hi_drv_frontend_polarization polar; /* <TP polarization */
    hi_u8 cbs_reliablity; /* <TP reliability */
    hi_s32 cbs_threshold; /* <blindscan threshold */
    hi_u8 fs_grade;
    hi_u32 fec_ok_cnt; /* <TP number of fec ok */
    hi_u32 fec_no_ok_cnt; /* <TP number of fec  not ok */
} frontend_tp_verify_params;

typedef struct {
    hi_u32 port;
    frontend_tp_verify_params tp_verify_prm;
} fe_ioctrl_tp_verify_info;

typedef struct {
    frontend_data_src data_src;
    hi_u32 data_len;
    /* hi_drv_tuner_sample_data data; */
} fe_ioctrl_sample_data_param;

typedef struct {
    hi_u32 port;
    hi_u32 agc1;
    hi_u32 agc2;
    hi_bool lock_flag;
    hi_bool agc_lock_flag;
    hi_u32 count;
    hi_u8 bagc_ctrl12;
} fe_ioctrl_agc_test;

typedef struct {
    hi_u32 port;
    hi_drv_frontend_lock_status lock_status;
    hi_drv_frontend_connect_para connect_para;
} fe_ioctrl_status;

typedef struct {
    hi_u32 port;
    hi_drv_frontend_scientific_num num;
} fe_ioctrl_ber_per;

typedef struct {
    hi_u32 port;
    hi_u32 rs[3]; /* 3:size */
} fe_ioctrl_rs;

typedef struct {
    hi_u32 port;
    hi_s32 snr;
} fe_ioctrl_snr;

typedef struct {
    hi_u32 port;
    hi_u32 freq;
    hi_u32 symb;
    hi_u32 direction;
} fe_ioctrl_freq_symb_offset;

typedef struct {
    hi_u32 port;
    hi_u32 time_out;
} fe_ioctrl_connect_timeout;

typedef struct {
    hi_u32 port;
    hi_u32 continuous_22k;
} fe_ioctrl_continuous_22k;

typedef struct {
    hi_u32 port;
    hi_u32 tone;
} fe_ioctrl_tone;

typedef struct {
    hi_u32 port;
    hi_u32 standby;
} fe_ioctrl_standby;

typedef struct {
    hi_u32 port;
    frontend_func_mode func_mode;
} fe_ioctrl_setfuncmode;

typedef struct {
    hi_u32 port;
    hi_u32 plpid;
} fe_ioctrl_plpno;

typedef struct {
    hi_u32 port;
    hi_u32 plp_num;
} fe_ioctrl_plp_num;

typedef struct {
    hi_u32 port;
} fe_ioctrl_plp_type;

typedef struct {
    hi_u32 port;
    hi_drv_demod_ts_out ts_out;
} fe_ioctrl_ts_out;

typedef struct {
    hi_u32 port;
} fe_ioctrl_sample_data;

typedef struct {
    hi_u32 port;
    hi_drv_frontend_attr attr;
} fe_ioctrl_attr;

typedef struct {
    hi_u32 port;
    hi_u32 time_out;
} fe_ioctrl_time_out;

typedef struct {
    hi_u32 port;
    hi_u32 sig_type;
} fe_ioctrl_set_sig_type;

typedef struct {
    hi_u32 port;
    hi_drv_frontend_isdbt_receive_config receive_config;
} fe_ioctrl_receive_config;

typedef struct {
    hi_u32 port;
    hi_drv_frontend_dvbt2_plp_para   plp_para;
} fe_ioctrl_set_plp_para;

typedef struct {
    hi_u32 port;
    hi_u32 index;
    hi_drv_frontend_dvbt2_plp_info   plp_info;
} fe_ioctrl_get_plp_info;

/*---- TUNER COMMAND----*/
#define HI_FRONTEND_IOC_MAGIC 't'
#define FRONTEND_CONNECT_CMD               _IOWR(HI_FRONTEND_IOC_MAGIC, 1, fe_ioctrl_signal)
#define FRONTEND_GET_STATUS_CMD            _IOWR(HI_FRONTEND_IOC_MAGIC, 2, fe_ioctrl_status)
#define FRONTEND_GET_SIGNALSTRENGTH_CMD    _IOWR(HI_FRONTEND_IOC_MAGIC, 3, fe_ioctrl_data_buf)
#define FRONTEND_GET_BER_CMD               _IOWR(HI_FRONTEND_IOC_MAGIC, 4, fe_ioctrl_ber_per)
#define FRONTEND_GET_SNR_CMD               _IOWR(HI_FRONTEND_IOC_MAGIC, 5, fe_ioctrl_snr)
#define FRONTEND_SET_ATTR_CMD              _IOW(HI_FRONTEND_IOC_MAGIC,  6, fe_ioctrl_attr)
#define FRONTEND_DISCONNECT_CMD            _IOW(HI_FRONTEND_IOC_MAGIC,  7, hi_u32)
#define FRONTEND_GET_ATTR_CMD              _IOWR(HI_FRONTEND_IOC_MAGIC, 8, fe_ioctrl_attr)
#define FRONTEND_TEST_SINGLE_AGC           _IOWR(HI_FRONTEND_IOC_MAGIC, 17, fe_ioctrl_agc_test)
#define FRONTEND_GET_FREQ_SYMB_OFFSET      _IOWR(HI_FRONTEND_IOC_MAGIC, 18, fe_ioctrl_data_buf)
#define FRONTEND_CONNECT_TIMEOUT_CMD       _IOWR(HI_FRONTEND_IOC_MAGIC, 19, fe_ioctrl_time_out)
#define FRONTEND_GET_SIGANLQUALITY_CMD     _IOWR(HI_FRONTEND_IOC_MAGIC, 20, fe_ioctrl_data)
#define FRONTEND_GET_SIGANLINFO_CMD        _IOWR(HI_FRONTEND_IOC_MAGIC, 21, fe_ioctrl_signal_info)
#define FRONTEND_BLINDSCAN_INIT_CMD        _IOWR(HI_FRONTEND_IOC_MAGIC, 22, fe_ioctrl_blindscan)
#define FRONTEND_BLINDSCAN_ACTION_CMD      _IOWR(HI_FRONTEND_IOC_MAGIC, 23, fe_ioctrl_blindscan_info)
#define FRONTEND_SET_LNBOUT_CMD            _IOWR(HI_FRONTEND_IOC_MAGIC, 24, fe_ioctrl_lnb_out)
#define FRONTEND_SEND_CONTINUOUS_22K_CMD   _IOWR(HI_FRONTEND_IOC_MAGIC, 25, fe_ioctrl_continuous_22k)
#define FRONTEND_SEND_TONE_CMD             _IOWR(HI_FRONTEND_IOC_MAGIC, 26, fe_ioctrl_tone)
#define FRONTEND_DISEQC_SEND_MSG_CMD       _IOWR(HI_FRONTEND_IOC_MAGIC, 27, fe_ioctrl_diseqc_sendmsg)
#define FRONTEND_DISEQC_RECV_MSG_CMD       _IOWR(HI_FRONTEND_IOC_MAGIC, 28, fe_ioctrl_diseqc_recvmsg)
#define FRONTEND_STANDBY_CMD               _IOWR(HI_FRONTEND_IOC_MAGIC, 29, fe_ioctrl_standby)
#define FRONTEND_DISABLE_CMD               _IOWR(HI_FRONTEND_IOC_MAGIC, 30, fe_ioctrl_data)
#define FRONTEND_SETFUNCMODE_CMD           _IOW(HI_FRONTEND_IOC_MAGIC,  31, fe_ioctrl_setfuncmode)
#define FRONTEND_SET_PLPNO_CMD             _IOW(HI_FRONTEND_IOC_MAGIC,  32, fe_ioctrl_data_buf)
#define FRONTEND_GET_PLPNUM_CMD            _IOWR(HI_FRONTEND_IOC_MAGIC, 33, fe_ioctrl_plp_num)
#define FRONTEND_GET_CURPLPTYPE_CMD        _IOWR(HI_FRONTEND_IOC_MAGIC, 34, fe_ioctrl_plp_type)
#define FRONTEND_SETTSOUT_CMD              _IOWR(HI_FRONTEND_IOC_MAGIC, 35, fe_ioctrl_ts_out)
#define FRONTEND_TPVERIFY_CMD              _IOWR(HI_FRONTEND_IOC_MAGIC, 36, fe_ioctrl_tp_verify_info)
#define FRONTEND_SETSATATTR_CMD            _IOWR(HI_FRONTEND_IOC_MAGIC, 37, fe_ioctrl_sat_attr)
#define FRONTEND_SAMPLE_DATA_CMD           _IOWR(HI_FRONTEND_IOC_MAGIC, 38, fe_ioctrl_sample_data)
#define FRONTEND_SET_ISI_ID_CMD            _IOW(HI_FRONTEND_IOC_MAGIC,  39, fe_ioctrl_set_isi_id)
#define FRONTEND_GET_TOTAL_STREAM_CMD      _IOWR(HI_FRONTEND_IOC_MAGIC, 40, fe_ioctrl_get_stream_num)
#define FRONTEND_GET_ISI_ID_CMD            _IOWR(HI_FRONTEND_IOC_MAGIC, 41, fe_ioctrl_get_isi_id)
#define FRONTEND_GET_AGC_CMD               _IOWR(HI_FRONTEND_IOC_MAGIC, 42, fe_ioctrl_data_buf)
#define FRONTEND_SET_ANTENNA_POWER_CMD     _IOW(HI_FRONTEND_IOC_MAGIC,  47, fe_ioctrl_data)
#define FRONTEND_SET_SCRAMBLE_CMD          _IOW(HI_FRONTEND_IOC_MAGIC,  49, fe_ioctrl_data)
#define FRONTEND_CLOSE_CMD                 _IOWR(HI_FRONTEND_IOC_MAGIC, 52, fe_ioctrl_data)
#define FRONTEND_MONITOR_ISDBT_LAYER_CMD   _IOWR(HI_FRONTEND_IOC_MAGIC, 53, fe_ioctrl_receive_config)
#define FRONTEND_TS_OUTPUT_SWAP_CMD        _IOW(HI_FRONTEND_IOC_MAGIC,  54, fe_ioctrl_data)
#define FRONTEND_SET_PLP_PARA_CMD          _IOWR(HI_FRONTEND_IOC_MAGIC, 57, fe_ioctrl_set_plp_para)
#define FRONTEND_GET_PLP_INFO_CMD          _IOWR(HI_FRONTEND_IOC_MAGIC, 58, fe_ioctrl_get_plp_info)
#define FRONTEND_GETTSOUT_CMD              _IOWR(HI_FRONTEND_IOC_MAGIC, 59, fe_ioctrl_ts_out)
#define FRONTEND_GET_PER_CMD               _IOWR(HI_FRONTEND_IOC_MAGIC, 60, fe_ioctrl_ber_per)

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif /* __cplusplus */

#endif

