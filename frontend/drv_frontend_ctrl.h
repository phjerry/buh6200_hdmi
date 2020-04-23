/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: frontend ctrl
 * Author: SDK
 * Created: 2017-06-30
 */
#ifndef __DRV_FRONTEND_CTRL_H__
#define __DRV_FRONTEND_CTRL_H__

#include "drv_frontend.h"
#include "drv_frontend_ioctl.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    hi_bool attr_inited;
    hi_bool auto_test;
    hi_bool print_enable; /* use proc cmd to control */
    hi_bool attr_set_err_print_enable;
    hi_bool reg_def_print_enable;
    hi_void *demod_private_para;
    hi_void *tuner_private_para;
    frontend_acc_qam_params connect_attr;
    hi_drv_frontend_attr attr;
    hi_drv_frontend_sat_attr sat_attr;
    drv_fe_tv_system cur_tuner_system;
    drv_fe_tuner_ops tuner_ops;
    drv_fe_demod_ops demod_ops;
    drv_fe_lnb_ops lnb_ops;
    drv_fe_resume_info resume_info;
} drv_fe_ctrl_ctx;

typedef struct {
    struct seq_file *seq_file;
    hi_bool print_enable;
} drv_fe_reg_para;

typedef struct {
    hi_drv_tuner_dev_type tuner_type;
    hi_u8  tuner_addr;
    hi_u32 reg_addr;
    hi_u32 _nb_bytes;
    hi_u8 chip_id;
    hi_u32 data_len;
    hi_bool sel_i2c_mode;
} drv_fe_ctrl_auto_select_tuner;

typedef struct {
    hi_drv_demod_dev_type demod_type;
    hi_s32 (*drv_fe_adp_demod_regist_func)(drv_fe_demod_ops *demod_ops);
} drv_fe_ctrl_demod_regist_func_map;

typedef struct {
    hi_drv_tuner_dev_type tuner_type;
    hi_s32 (*drv_fe_adp_tuner_regist_func)(drv_fe_tuner_ops *tuner_ops);
} drv_fe_ctrl_tuner_regist_func_map;

typedef struct {
    hi_drv_lnb_ctrl_dev_type lnb_type;
    hi_s32 (*drv_fe_adp_lnb_regist_func)(drv_fe_lnb_ops *lnb_ops);
} drv_fe_ctrl_lnb_regist_func_map;

drv_fe_ctrl_ctx *drv_frontend_get_context(hi_u32 port_id);
hi_drv_frontend_sig_type drv_frontend_get_sig_type(hi_u32 port_id);
drv_fe_ctrl_func *drv_frontend_get_ctrl_func(hi_void);

hi_s32 drv_fe_ctrl_init(hi_void);
hi_void drv_fe_ctrl_deinit(hi_void);
hi_s32 drv_fe_ctrl_connect(fe_ioctrl_signal *pst_signal);
hi_s32 drv_fe_ctrl_close(hi_u32 port);
hi_s32 drv_fe_ctrl_get_status(fe_ioctrl_status *pst_status);
hi_s32 drv_fe_ctrl_get_signal_strength(fe_ioctrl_data_buf *pst_signal_strength);
hi_s32 drv_fe_ctrl_get_ber(fe_ioctrl_ber_per *pst_ber);
hi_s32 drv_fe_ctrl_get_per(fe_ioctrl_ber_per *per);
hi_s32 drv_fe_ctrl_get_snr(fe_ioctrl_snr *pst_snr);
hi_s32 drv_fe_ctrl_set_attr(fe_ioctrl_attr *pst_attr);
hi_s32 drv_fe_ctrl_get_attr(fe_ioctrl_attr *pst_attr);
hi_s32 drv_fe_ctrl_get_freq_symb_offset(fe_ioctrl_data_buf *pst_freq_symb_offset);
hi_s32 drv_fe_ctrl_get_signal_info(fe_ioctrl_signal_info *pst_signal_info);
hi_s32 drv_fe_ctrl_set_standby(fe_ioctrl_standby *pst_standby);
hi_s32 drv_fe_ctrl_set_ts_out(fe_ioctrl_ts_out *pst_ts_out);
hi_s32 drv_fe_ctrl_get_ts_out(fe_ioctrl_ts_out *pst_ts_out);
hi_s32 drv_fe_ctrl_get_quality(fe_ioctrl_data *pst_quality);
hi_s32 drv_fe_ctrl_suspend(hi_void);
hi_s32 drv_fe_ctrl_resume(hi_void);
hi_s32 drv_fe_ctrl_monitor_layers_config(fe_ioctrl_receive_config *receive_config);

hi_s32 drv_fe_ctrl_get_plp_num(fe_ioctrl_plp_num *pst_plp_num);
hi_s32 drv_fe_ctrl_set_plp_para(fe_ioctrl_set_plp_para *pst_set_plp_para);
hi_s32 drv_fe_ctrl_get_plp_info(fe_ioctrl_get_plp_info *pst_get_plp_info);

/* sat */
hi_s32 drv_fe_ctrl_set_sat_attr(fe_ioctrl_sat_attr *sat_attr);
hi_s32 drv_fe_ctrl_set_blindscan_init(fe_ioctrl_blindscan *blindscan);
hi_s32 drv_fe_ctrl_set_blindscan_action(fe_ioctrl_blindscan_info *blindscan_action);
hi_s32 drv_fe_ctrl_set_blindscan_abort(fe_ioctrl_blindscan_abort *blindscan_abort);
hi_s32 drv_fe_ctrl_set_lnb_out(fe_ioctrl_lnb_out *lnb_out);
hi_s32 drv_fe_ctrl_set_continuous_22k(fe_ioctrl_continuous_22k *continuous_22k);
hi_s32 drv_fe_ctrl_set_send_tone(fe_ioctrl_tone *tone);
hi_s32 drv_fe_ctrl_set_diseqc_send_msg(fe_ioctrl_diseqc_sendmsg *diseqc_send_msg);
hi_s32 drv_fe_ctrl_set_diseqc_recv_msg(fe_ioctrl_diseqc_recvmsg *diseqc_recv_msg);
hi_s32 drv_fe_ctrl_get_stream_num(fe_ioctrl_get_stream_num *arg);
hi_s32 drv_fe_ctrl_get_isi_id(fe_ioctrl_get_isi_id *arg);
hi_s32 drv_fe_ctrl_set_isi_id(fe_ioctrl_set_isi_id *arg);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif /* __cplusplus */

#endif
