/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: frontend
 * Author: SDK
 * Created: 2017-06-30
 */
#ifndef __DRV_FRONTEND_H__
#define __DRV_FRONTEND_H__

#include "hi_debug.h"
#include "hi_errno.h"
#include "drv_frontend_ioctl.h"
#include "drv_i2c_ext.h"
#include "drv_gpioi2c_ext.h"
#include "drv_gpio_ext.h"
#include "hi_osal.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */


#define TIME_CONSUMING

#ifdef TIME_CONSUMING
    #define time_cost_define(lable)  osal_timeval st_start_time##lable, st_end_time##lable;

    #define time_cost_start(lable)  osal_get_timeofday(&st_start_time##lable)

    #define time_cost_end(lable, msg) do {\
            long total; \
            osal_get_timeofday(&st_end_time##lable);    \
            total = (st_end_time##lable.tv_sec - st_start_time##lable.tv_sec) * 1000 \
                + (st_end_time##lable.tv_usec - st_start_time##lable.tv_usec) / 1000; \
            LOG_WARN_PRINT(msg " cost %dms.\n", total); \
    } while (0)
#else
    #define time_cost_define(lable)
    #define time_cost_start(lable)
    #define time_cost_end(lable, msg)
#endif

#define hi_warn_print_reg8(reg, val)      LOG_WARN_PRINT("%s : %02x\n", #reg, val)
#define hi_info_print_reg8(reg, val)      LOG_INFO_PRINT("%s : %02x\n", #reg, val)
#define hi_dbg_print_reg8(reg, val)       LOG_DBG_PRINT("%s : %02x\n",  #reg, val)

#define drv_fe_check_pointer(p) \
    HI_CHECK_PARAM(((p) == NULL), HI_ERR_FRONTEND_INVALID_POINT, HI_ERR_FRONTEND_INVALID_POINT)

#define drv_fe_check_func_pointer(pfn)  \
    HI_CHECK_PARAM(((pfn) == NULL), HI_ERR_FRONTEND_INVALID_FUNCTION, HI_ERR_FRONTEND_INVALID_FUNCTION)

#define drv_fe_ctrl_check_is_dtv_c(signal_type) \
    HI_CHECK_PARAM(((signal_type) < HI_DRV_FRONTEND_SIG_TYPE_DVB_C || (signal_type) > HI_DRV_FRONTEND_SIG_TYPE_J83B),\
        HI_ERR_FRONTEND_INVALID_SIGTYPE, HI_ERR_FRONTEND_INVALID_SIGTYPE)

#define drv_fe_ctrl_check_is_dtv_t(signal_type) \
    HI_CHECK_PARAM(((signal_type) < HI_DRV_FRONTEND_SIG_TYPE_DVB_T || (signal_type) > HI_DRV_FRONTEND_SIG_TYPE_DTMB),\
        HI_ERR_FRONTEND_INVALID_SIGTYPE, HI_ERR_FRONTEND_INVALID_SIGTYPE)

#define drv_fe_ctrl_check_is_dtv_s(signal_type) \
    HI_CHECK_PARAM(((signal_type) < HI_DRV_FRONTEND_SIG_TYPE_DVB_S || (signal_type) > HI_DRV_FRONTEND_SIG_TYPE_ABSS),\
        HI_ERR_FRONTEND_INVALID_SIGTYPE, HI_ERR_FRONTEND_INVALID_SIGTYPE)

#define drv_fe_ctrl_check_signal_type(signal_type) \
    HI_CHECK_PARAM(((signal_type) >= HI_DRV_FRONTEND_SIG_TYPE_MAX),\
        HI_ERR_FRONTEND_INVALID_SIGTYPE, HI_ERR_FRONTEND_INVALID_SIGTYPE)

#define drv_fe_ctrl_check_port(port)    \
    HI_CHECK_PARAM(((port) >= DRV_FRONTEND_NUM), HI_ERR_FRONTEND_INVALID_PORT, HI_ERR_FRONTEND_INVALID_PORT)

#define drv_fe_ctrl_check_tuner_type(tuner_type)   \
    HI_CHECK_PARAM(((tuner_type) >= HI_DRV_TUNER_DEV_TYPE_MAX),\
        HI_ERR_FRONTEND_INVALID_TUNERTYPE, HI_ERR_FRONTEND_INVALID_TUNERTYPE)

#define drv_fe_ctrl_check_demod_type(demod_type)   \
    HI_CHECK_PARAM(((demod_type) >= HI_DRV_DEMOD_DEV_TYPE_MAX),\
        HI_ERR_FRONTEND_INVALID_DEMODTYPE, HI_ERR_FRONTEND_INVALID_DEMODTYPE)

#define BIT_0                        0
#define BIT_1                        1
#define BIT_2                        2
#define BIT_3                        3
#define BIT_4                        4
#define BIT_5                        5
#define BIT_6                        6
#define BIT_7                        7

#define BIT_OFFSET_0                        0
#define BIT_OFFSET_1                        1
#define BIT_OFFSET_2                        2
#define BIT_OFFSET_3                        3
#define BIT_OFFSET_4                        4
#define BIT_OFFSET_5                        5
#define BIT_OFFSET_6                        6
#define BIT_OFFSET_7                        7
#define BIT_OFFSET_8                        8
#define BIT_OFFSET_9                        9
#define BIT_OFFSET_10                       10
#define BIT_OFFSET_11                       11
#define BIT_OFFSET_12                       12
#define BIT_OFFSET_13                       13
#define BIT_OFFSET_14                       14
#define BIT_OFFSET_15                       15
#define BIT_OFFSET_16                       16
#define BIT_OFFSET_17                       17
#define BIT_OFFSET_18                       18
#define BIT_OFFSET_19                       19
#define BIT_OFFSET_20                       20
#define BIT_OFFSET_21                       21
#define BIT_OFFSET_22                       22
#define BIT_OFFSET_23                       23
#define BIT_OFFSET_24                       24
#define BIT_OFFSET_25                       25
#define BIT_OFFSET_26                       26
#define BIT_OFFSET_27                       27
#define BIT_OFFSET_28                       28
#define BIT_OFFSET_29                       29
#define BIT_OFFSET_30                       30
#define BIT_OFFSET_31                       31

typedef enum {
    DRV_FE_TV_SYSTEM_PAL_BG = 0,
    DRV_FE_TV_SYSTEM_PAL_DK = 1,
    DRV_FE_TV_SYSTEM_PAL_I = 2,
    DRV_FE_TV_SYSTEM_PAL_M = 3,
    DRV_FE_TV_SYSTEM_PAL_N = 4,
    DRV_FE_TV_SYSTEM_SECAM_BG = 5,
    DRV_FE_TV_SYSTEM_SECAM_DK = 6,
    DRV_FE_TV_SYSTEM_SECAM_L_PRIME = 7,
    DRV_FE_TV_SYSTEM_SECAM_LL = 8,
    DRV_FE_TV_SYSTEM_NTSC_M = 9,
    DRV_FE_TV_SYSTEM_QAM_6MHz = 10,
    DRV_FE_TV_SYSTEM_QAM_8MHz = 11,
    DRV_FE_TV_SYSTEM_DVBT_1_7MHz = 12,
    DRV_FE_TV_SYSTEM_DVBT_6MHz = 13,
    DRV_FE_TV_SYSTEM_DVBT_7MHz = 14,
    DRV_FE_TV_SYSTEM_DVBT_8MHz = 15,
    DRV_FE_TV_SYSTEM_DVBT_10MHz = 16,
    DRV_FE_TV_SYSTEM_DTMB_8MHz = 17,
    DRV_FE_TV_SYSTEM_ATSC_6MHz = 18,
    DRV_FE_TV_SYSTEM_ISDBT_6MHz = 19,
    DRV_FE_TV_SYSTEM_FM_RADIO = 20,
    DRV_FE_TV_SYSTEM_NTSC_I = 21,
    DRV_FE_TV_SYSTEM_NTSC_DK = 22,
    DRV_FE_TV_SYSTEM_SAT = 23,
    DRV_FE_TV_SYSTEM_DVBT2_8MHz = 24,

    DRV_FE_TV_SYSTEM_MAX,
} drv_fe_tv_system;

typedef enum {
    I2C_TUNER_TYPE = 0,
    I2C_DEMOD_TYPE,
    I2C_LNB_TYPE,
    I2C_DEVICE_TYPE_MAX
} i2c_device_type;

typedef struct {
    hi_u32 mcu_ver; /* svn or git code version */
    hi_char *mcu_commit_date; /* svn or git code commit date */
}drv_fe_mcu_info;

typedef struct {
    i2c_ext_func *i2c_func;
    gpio_i2c_ext_func *gpio_i2c_func;
    gpio_ext_func *gpio_func;
} drv_fe_ctrl_func;

typedef struct {
    hi_s32 (*init)(hi_u32 fe_port);
    hi_s32 (*deinit)(hi_u32 fe_port);
    hi_s32 (*set_system)(hi_u32 fe_port, drv_fe_tv_system tv_system);
    hi_s32 (*set_freq)(hi_u32 fe_port, hi_u32 freq); /* khz */
    hi_s32 (*set_freq_symb)(hi_u32 fe_port, hi_u32 freq, hi_u32 symb); /* khz */
    hi_void (*resume)(hi_u32 fe_port);
    hi_s32 (*get_signal_strength)(hi_u32 fe_port, hi_u32 signal_strength[3]); /* 3:index */
    hi_s32 (*i2c_read_byte)(hi_u32 fe_port, hi_u8 reg_addr, hi_u8 *reg_val);
    hi_s32 (*i2c_write_byte)(hi_u32 fe_port, hi_u8 reg_addr, hi_u8 reg_val);
} drv_fe_tuner_ops;

typedef struct {
    hi_s32 (*init)(hi_u32 fe_port);
    hi_s32 (*connect)(hi_u32 fe_port, frontend_acc_qam_params *connect_attr);
    hi_s32 (*deinit)(hi_u32 fe_port);
    hi_s32 (*get_status)(hi_u32 fe_port, hi_drv_frontend_lock_status *frontend_status);
    hi_s32 (*standby)(hi_u32 fe_port, hi_u32 standby);
    hi_s32 (*get_signal_info)(hi_u32 fe_port, hi_drv_frontend_signal_info *info);
    hi_s32 (*i2c_bypass)(hi_u32 fe_port, hi_bool bypass);
    hi_s32 (*get_ber)(hi_u32 fe_port, hi_drv_frontend_scientific_num *num);
    hi_s32 (*get_per)(hi_u32 fe_port, hi_drv_frontend_scientific_num *num);
    hi_s32 (*get_snr)(hi_u32 fe_port, hi_s32 *pu32_snr);
    hi_s32 (*get_signal_strength)(hi_u32 fe_port, hi_u32 signal_strength[3]); /* 3:index */
    hi_s32 (*get_signal_quality)(hi_u32 fe_port, hi_u32 *signal_quality);
    hi_s32 (*get_freq_symb_offset)(hi_u32 fe_port, hi_s32 *freq_offset, hi_u32 *symb);
    hi_void (*manage_after_chipreset)(hi_u32 fe_port);
    hi_void (*recalculate_signal_strength)(hi_u32 fe_port, hi_u32 *strength);
    hi_void (*connect_timeout)(hi_u32 fe_port, hi_u32 connect_timeout);
    hi_s32 (*set_ts_out)(hi_u32 fe_port, hi_drv_demod_ts_out *ts_out);
    hi_s32 (*set_sig_type)(hi_u32 fe_port, hi_drv_frontend_sig_type sig_type);
    hi_bool (*is_ctrl_tuner)(hi_u32 fe_port);

    /* PROC */
    hi_void (*get_registers)(hi_u32 fe_port, hi_void *p);
    hi_s32 (*i2c_read_byte)(hi_u32 fe_port, hi_u8 u8_reg_addr, hi_u8 *reg_val);
    hi_s32 (*i2c_write_byte)(hi_u32 fe_port, hi_u8 u8_reg_addr, hi_u8 reg_val);
    hi_s32 (*get_qtc_data)(hi_u32 fe_port, hi_drv_frontend_connect_para *connect_para,
        hi_u32 qtc_buff_size_mb, hi_u32 record_file_size_mb);
    hi_s32 (*get_cir)(hi_u32 fe_port); /* channel_impulse_response */
    hi_s32 (*catch_data)(hi_u32 fe_port, hi_u8 u8_catch_sel, hi_u8 u8_iq_sel);
    hi_s32 (*mcu_operate)(hi_u32 fe_port, const hi_char *p_mcu_cmd);
    hi_s32 (*mcu_info)(hi_u32 fe_port, drv_fe_mcu_info *pst_mcu_info);

    /* SAT */
    hi_s32 (*set_sat_attr)(hi_u32 fe_port, hi_drv_frontend_sat_attr *sat_frontend_attr);
    hi_s32 (*blindscan_init)(hi_u32 fe_port, frontend_blindscan_initpara *init_para);
    hi_s32 (*blindscan_action)(hi_u32 fe_port, frontend_blindscan_para *para);
    hi_s32 (*blindscan_abort)(hi_u32 fe_port, hi_u32 stop_quit);
    hi_s32 (*set_lnb_out)(hi_u32 fe_port, frontend_lnb_out_level out);
    hi_s32 (*send_continuous22k)(hi_u32 fe_port, hi_u32 continuous22k);
    hi_s32 (*send_tone)(hi_u32 fe_port, hi_u32 tone);
    hi_s32 (*diseqc_send_msg)(hi_u32 fe_port, hi_drv_frontend_diseqc_send_msg *send_msg);
    hi_s32 (*diseqc_recv_msg)(hi_u32 fe_port, hi_drv_frontend_diseqc_recv_msg *recv_msg);
    hi_s32 (*set_func_mode)(hi_u32 fe_port, frontend_func_mode func_mode);
    hi_s32 (*tp_verify)(hi_u32 fe_port, frontend_tp_verify_params *channel);
    hi_s32 (*set_isi_id)(hi_u32 fe_port, hi_u8 isi_id);
    hi_s32 (*get_isi_id)(hi_u32 fe_port, hi_u8 stream, hi_u8 *isi_id);
    hi_s32 (*get_stream_num)(hi_u32 fe_port, hi_u32 *stream_num);

    /* DVB-T2 */
    hi_s32 (*get_plp_num)(hi_u32 fe_port, hi_u8 *plp_num);
    hi_s32 (*set_plp_para)(hi_u32 fe_port, hi_drv_frontend_dvbt2_plp_para *plp_para);
    hi_s32 (*get_plp_info)(hi_u32 fe_port, hi_u32 index, hi_drv_frontend_dvbt2_plp_info *plp_info);

    /* ISDB-T */
    hi_s32 (*monitor_layers_config)(hi_u32 fe_port, hi_drv_frontend_isdbt_receive_config receive_config);
} drv_fe_demod_ops;

typedef struct {
    hi_s32 (*init)(hi_u32 fe_port, hi_u8 i2c_channel, hi_u8 dev_addr);
    hi_s32 (*deinit)(hi_u32 fe_port);
    hi_s32 (*standby)(hi_u32 fe_port, hi_u32 standby);
    hi_s32 (*set_lnb_out)(hi_u32 fe_port, frontend_lnb_out_level lnb_out_level);
    hi_s32 (*i2c_read_byte)(hi_u32 fe_port, hi_u8 reg_addr, hi_u8 *reg_val);
    hi_s32 (*i2c_write_byte)(hi_u32 fe_port, hi_u8 reg_addr, hi_u8 reg_val);
} drv_fe_lnb_ops;

typedef struct {
    hi_drv_frontend_lock_status lock_status;
    bool set_ts_out;
    hi_drv_demod_ts_out ts_out;
    frontend_lnb_out_level lnb_out_level;
    hi_u32 continuous22k;
} drv_fe_resume_info;

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif /* __cplusplus */

#endif

