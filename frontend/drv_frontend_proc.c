/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: frontend proc
 * Author: SDK
 * Created: 2017-06-30
 */

#include "drv_frontend_proc.h"
#include "linux/hisilicon/securec.h"
#include "hi_osal.h"
#include "drv_frontend_ctrl.h"

static const hi_char* g_fe_tuner_type[] = {
    "XG_3BL",
    "CD1616",
    "ALPS_TDAE",
    "TDCC",
    "TDA18250",
    "CD1616",
    "MT2081",
    "TMX7070X",
    "R820C",
    "MXL203",
    "AV2011",
    "SHARP7903",
    "MXL101",
    "MXL603",
    "IT9170",
    "IT9133",
    "TDA6651",
    "TDA18250B",
    "M88TS2022",
    "RDA5815",
    "MXL254",
    "CXD2861",
    "SI2147",
    "RAFAEL836",
    "MXL608",
    "MXL214",
    "TDA18280",
    "TDA182I5A",
    "SI2144",
    "AV2018",
    "MXL251",
    "M88TC3800",
    "MXL601",
    "MXL683",
    "AV2026",
    "R850",
    "R858",
    "MXL541",
    "MXL581",
    "MXL582",
    "MXL661",
    "RT720",
    "CXD2871",
    "SUT_PJ987",
    "BUTT",
};

static const hi_char* g_fe_demod_type[] = {
    "3130I",
    "3130E",
    "J83B",
    "AVL6211",
    "MXL101",
    "MN88472",
    "IT9170",
    "IT9133",
    "HI3136",
    "INVALID",
    "MXL254",
    "CXD2837",
    "HI3137",
    "MXL214",
    "TDA18280",
    "INVALID",
    "INVALID",
    "MXL251",
    "INVALID",
    "ATBM888X",
    "MN88473",
    "MXL683",
    "TP5001",
    "HD2501",
    "AVL6381",
    "MXL541",
    "MXL581",
    "MXL582",
    "INTERNAL0",
    "CXD2856",
    "CXD2857",
    "CXD2878",
    "BUTT",
};

static const hi_char* g_fe_lnb_type[] = {
    "NONE",
    "MPS8125",
    "ISL9492",
    "A8300",
    "A8297",
    "TPS65233",
    "LNBH30",
    "BUTT"
};

static const hi_char* g_fe_dtv_sys[] = {
    "PAL_BG",
    "PAL_DK",
    "PAL_I",
    "PAL_M",
    "PAL_N",
    "SECAM_BG",
    "SECAM_DK",
    "SECAM_L_P",
    "SECAM_LL",
    "NTSC_M",
    "QAM_6MHz",
    "QAM_8MHz",
    "DVBT_1_7MHz",
    "DVBT_6MHz",
    "DVBT_7MHz",
    "DVBT_8MHz",
    "DVBT_10MHz",
    "DTMB_8MHz",
    "ATSC_6MHz",
    "ISDBT_6MHz",
    "FM_Radio",
    "NTSC_I",
    "NTSC_DK",
    "SAT",

    "BUTT",
};

static hi_s32 frontend_i2c_read_byte(i2c_device_type dev_type, hi_u32 port_id, hi_u8 reg_addr, hi_u8 *reg_val)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *frontend_context = NULL;
    drv_fe_demod_ops *frontend_demod_ops = NULL;
    drv_fe_tuner_ops *frontend_tuner_ops = NULL;
    drv_fe_lnb_ops *frontend_lnb_ops = NULL;

    drv_fe_ctrl_check_port(port_id);
    drv_fe_check_pointer(reg_val);
    frontend_context = drv_frontend_get_context(port_id);
    drv_fe_check_pointer(frontend_context);

    if (dev_type == I2C_DEMOD_TYPE) {
        frontend_demod_ops = &frontend_context->demod_ops;
        drv_fe_check_pointer(frontend_demod_ops);
        drv_fe_check_func_pointer(frontend_demod_ops->i2c_read_byte);
        ret = frontend_demod_ops->i2c_read_byte(port_id, reg_addr, reg_val);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(frontend_demod_ops->i2c_read_byte, ret);
            return ret;
        }
    } else if (dev_type == I2C_TUNER_TYPE) {
        frontend_tuner_ops = &frontend_context->tuner_ops;
        drv_fe_check_pointer(frontend_tuner_ops);
        drv_fe_check_func_pointer(frontend_tuner_ops->i2c_read_byte);
        ret = frontend_tuner_ops->i2c_read_byte(port_id, reg_addr, reg_val);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(frontend_tuner_ops->i2c_read_byte, ret);
            return ret;
        }
    } else {
        frontend_lnb_ops = &frontend_context->lnb_ops;
        drv_fe_check_pointer(frontend_lnb_ops);
        drv_fe_check_func_pointer(frontend_lnb_ops->i2c_read_byte);
        ret = frontend_lnb_ops->i2c_read_byte(port_id, reg_addr, reg_val);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(frontend_lnb_ops->i2c_read_byte, ret);
            return ret;
        }
    }
    hi_drv_proc_echo_helper("read_reg_addr=0x%02x,read_reg_val=0x%02x\n", reg_addr, *reg_val);

    return HI_SUCCESS;
}

static hi_s32 frontend_i2c_write_byte(i2c_device_type dev_type, hi_u32 port_id, hi_u8 reg_addr, hi_u8 reg_val)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *frontend_context = NULL;
    drv_fe_demod_ops *frontend_demod_ops = NULL;
    drv_fe_tuner_ops *frontend_tuner_ops = NULL;
    drv_fe_lnb_ops *frontend_lnb_ops = NULL;

    drv_fe_ctrl_check_port(port_id);
    frontend_context = drv_frontend_get_context(port_id);
    drv_fe_check_pointer(frontend_context);

    hi_drv_proc_echo_helper("write_reg_addr=0x%02x,write_reg_val=0x%02x\n", reg_addr, reg_val);

    if (dev_type == I2C_DEMOD_TYPE) {
        frontend_demod_ops = &frontend_context->demod_ops;
        drv_fe_check_pointer(frontend_demod_ops);
        drv_fe_check_func_pointer(frontend_demod_ops->i2c_write_byte);
        ret = frontend_demod_ops->i2c_write_byte(port_id, reg_addr, reg_val);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(frontend_demod_ops->i2c_write_byte, ret);
            return ret;
        }
    } else if (dev_type == I2C_TUNER_TYPE) {
        frontend_tuner_ops = &frontend_context->tuner_ops;
        drv_fe_check_pointer(frontend_tuner_ops);
        drv_fe_check_func_pointer(frontend_tuner_ops->i2c_write_byte);
        ret = frontend_tuner_ops->i2c_write_byte(port_id, reg_addr, reg_val);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(frontend_tuner_ops->i2c_write_byte, ret);
            return ret;
        }
    } else {
        frontend_lnb_ops = &frontend_context->lnb_ops;
        drv_fe_check_pointer(frontend_lnb_ops);
        drv_fe_check_func_pointer(frontend_lnb_ops->i2c_write_byte);
        ret = frontend_lnb_ops->i2c_write_byte(port_id, reg_addr, reg_val);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(frontend_lnb_ops->i2c_write_byte, ret);
            return ret;
        }
    }

    return HI_SUCCESS;
}

static const hi_char *frontend_proc_read_status(hi_drv_frontend_lock_status status)
{
    switch (status) {
        case HI_DRV_FRONTEND_LOCK_STATUS_DROPPED:
            return "unlocked";
        case HI_DRV_FRONTEND_LOCK_STATUS_LOCKED:
            return "locked";
        default:
            return "BUTT";
    }
    return "BUTT";
}

static const hi_char *frontend_dtv_fec_rate(hi_drv_frontend_fec_rate fec_rate_index)
{
    switch (fec_rate_index) {
        case HI_DRV_FRONTEND_FEC_RATE_1_2:
            return "1/2";
        case HI_DRV_FRONTEND_FEC_RATE_2_3:
            return "2/3";
        case HI_DRV_FRONTEND_FEC_RATE_3_4:
            return "3/4";
        case HI_DRV_FRONTEND_FEC_RATE_4_5:
            return "4/5";
        case HI_DRV_FRONTEND_FEC_RATE_5_6:
            return "5/6";
        case HI_DRV_FRONTEND_FEC_RATE_6_7:
            return "6/7";
        case HI_DRV_FRONTEND_FEC_RATE_7_8:
            return "7/8";
        case HI_DRV_FRONTEND_FEC_RATE_1_3:
            return "1/3";
        case HI_DRV_FRONTEND_FEC_RATE_2_5:
            return "2/5";
        case HI_DRV_FRONTEND_FEC_RATE_3_5:
            return "3/5";
        default:
            return "INVALID";
    }
    return "INVALID";
}

static const hi_char *frontend_dtv_mode_type(hi_drv_modulation_type mode_type_index)
{
    switch (mode_type_index) {
        case HI_DRV_MOD_TYPE_DQPSK:
            return "DQPSK";
        case HI_DRV_MOD_TYPE_QPSK:
            return "QPSK";
        case HI_DRV_MOD_TYPE_QAM_16:
            return "QAM_16";
        case HI_DRV_MOD_TYPE_QAM_64:
            return "QAM_64";
        case HI_DRV_MOD_TYPE_QAM_128:
            return "QAM_128";
        case HI_DRV_MOD_TYPE_QAM_256:
            return "QAM_256";
        default:
            return "INVALID";
    }
    return "INVALID";
}

static hi_s32 frontend_proc_read_snr(hi_u32 i, hi_char *snr, hi_u32 snr_length)
{
    hi_s32 ret;
    hi_s32 snr_integer = 0;
    hi_s32 snr_fraction = 0;
    fe_ioctrl_snr signal_snr = {0};

    signal_snr.port = i;
    ret = drv_fe_ctrl_get_snr(&signal_snr);
    if (ret != HI_SUCCESS && ret != HI_ERR_FRONTEND_NOT_CONNECT) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_get_snr, ret);
        return ret;
    }

    if (signal_snr.snr == 0) {
        ret = strncpy_s(snr, snr_length, "0", sizeof("0"));
        if (ret != HI_SUCCESS) {
            return HI_ERR_FRONTEND_SECURE_CHECK;
        }
    } else {
        snr_fraction =  abs(signal_snr.snr % 100); /* 10:value */
        snr_integer =  signal_snr.snr / 100; /* 10:div */
        ret = snprintf(snr, snr_length, "%d.%02d", snr_integer, snr_fraction);
        if (ret == HI_FAILURE) {
            return HI_ERR_FRONTEND_SECURE_CHECK;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 frontend_proc_read_isdbt_common(struct seq_file *s, hi_u32 i)
{
    hi_s32 ret;
    const hi_char *tuner_lock = NULL;
    hi_char snr[PROC_SIGNALINFO_LEN];
    drv_fe_ctrl_ctx *frontend_context = NULL;

    fe_ioctrl_ber_per signal_ber = {0};
    fe_ioctrl_status signal_status = {0};
    fe_ioctrl_data_buf signal_strength = {0};

    drv_fe_ctrl_check_port(i);
    frontend_context = drv_frontend_get_context(i);
    drv_fe_check_pointer(frontend_context);

    signal_ber.port = i;
    ret = drv_fe_ctrl_get_ber(&signal_ber);
    if (ret != HI_SUCCESS && ret != HI_ERR_FRONTEND_NOT_CONNECT) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_get_ber, ret);
        return ret;
    }

    signal_strength.port = i;
    ret = drv_fe_ctrl_get_signal_strength(&signal_strength);
    if (ret != HI_SUCCESS && ret != HI_ERR_FRONTEND_NOT_CONNECT) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_get_signal_strength, ret);
        return ret;
    }

    signal_status.port = i;
    ret = drv_fe_ctrl_get_status(&signal_status);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_get_status, ret);
        return ret;
    }
    tuner_lock = frontend_proc_read_status(signal_status.lock_status);

    ret = frontend_proc_read_snr(i, snr, PROC_SIGNALINFO_LEN);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_proc_read_snr, ret);
        return ret;
    }

    osal_proc_print(s, "Lock State    :%-10s |Frequency(KHz) :%-10d |Bandwith       :%-10d\n",
        tuner_lock, frontend_context->connect_attr.frequency, frontend_context->connect_attr.srbw.band_width);
    osal_proc_print(s, "BER           :%d.%03dE%d   |SNR(dB)        :%-10s |RSSI(d_buv)    :%-8d\n",
        signal_ber.num.decimal_val, signal_ber.num.integer_val, signal_ber.num.power, /* 2:index */
        snr, signal_strength.data_buf[1]);
    return HI_SUCCESS;
}

static const hi_char *frontend_isdbt_time_interleaver(hi_drv_frontend_isdbt_time_interleaver time_il)
{
    switch (time_il) {
        case HI_DRV_FRONTEND_ISDBT_TIME_INTERLEAVER_0:
            return "I0";
        case HI_DRV_FRONTEND_ISDBT_TIME_INTERLEAVER_1:
            return "I1";
        case HI_DRV_FRONTEND_ISDBT_TIME_INTERLEAVER_2:
            return "I2";
        case HI_DRV_FRONTEND_ISDBT_TIME_INTERLEAVER_4:
            return "I4";
        default:
            return "INVALID";
    }
    return "INVALID";
}

static hi_void frontend_proc_read_isdbt_lock_info_layer(struct seq_file *s,
    hi_drv_frontend_isdbt_layers_info *layer_info, hi_drv_frontend_isdbt_layer layer, hi_bool layer_exist)
{
    const hi_char *layer_str = HI_NULL;
    const hi_char *qam_type = HI_NULL;
    const hi_char *fec_rate = HI_NULL;
    const hi_char *time_interleaver = HI_NULL;

    hi_u8 seg_num = layer_info->layer_seg_num;
    hi_drv_modulation_type layer_mode_type = layer_info->layer_mod_type;
    hi_drv_frontend_fec_rate layer_fec_rate = layer_info->layer_fec_rate;
    hi_drv_frontend_isdbt_time_interleaver time_il = layer_info->layer_time_interleaver;

    if (layer == HI_DRV_FRONTEND_ISDBT_LAYER_A) {
        layer_str = "A";
    } else if (layer == HI_DRV_FRONTEND_ISDBT_LAYER_B) {
        layer_str = "B";
    } else {
        layer_str = "C";
    }

    if (!layer_exist) {
        osal_proc_print(s, "Layer         :%-10s |Exist          :%-10s \n", layer_str, "NO");
    } else {
        qam_type = frontend_dtv_mode_type(layer_mode_type);
        fec_rate = frontend_dtv_fec_rate(layer_fec_rate);
        time_interleaver = frontend_isdbt_time_interleaver(time_il);
        osal_proc_print(s, "Layer         :%-10s |Exist          :%-10s |Layer Number   :%-10d\n",
            layer_str, "YES", seg_num);
        osal_proc_print(s, "QAM Mode      :%-10s |FEC Rate       :%-10s |Invt Time      :%-10s\n",
            qam_type, fec_rate, time_interleaver);
    }
}

static hi_s32 frontend_proc_read_isdbt_lock_info(struct seq_file *s, hi_u32 i)
{
    hi_s32 ret;
    hi_bool layer_exist = HI_FALSE;
    fe_ioctrl_status status = {0};
    fe_ioctrl_signal_info signal_info = {0};

    status.port = i;
    ret = drv_fe_ctrl_get_status(&status);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_get_status, ret);
        return ret;
    }

    if (status.lock_status == HI_DRV_FRONTEND_LOCK_STATUS_LOCKED) {
        signal_info.port = i;
        ret = drv_fe_ctrl_get_signal_info(&signal_info);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_get_signal_info, ret);
            return ret;
        }
        osal_proc_print(s, "Emergency     :%-10d |Phase Shift    :%-10d |Partial        :%-10d\n",
            signal_info.info.signal_info.isdbt.isdbt_tmcc_info.emergency_flag,
            signal_info.info.signal_info.isdbt.isdbt_tmcc_info.phase_shift_corr,
            signal_info.info.signal_info.isdbt.isdbt_tmcc_info.partial_flag);
        layer_exist = signal_info.info.signal_info.isdbt.isdbt_layers.bits.layer_a_exist;
        frontend_proc_read_isdbt_lock_info_layer(s,
            &signal_info.info.signal_info.isdbt.isdbt_tmcc_info.isdbt_layers_a_info_bits,
            HI_DRV_FRONTEND_ISDBT_LAYER_A, layer_exist);

        layer_exist = signal_info.info.signal_info.isdbt.isdbt_layers.bits.layer_b_exist;
        frontend_proc_read_isdbt_lock_info_layer(s,
            &signal_info.info.signal_info.isdbt.isdbt_tmcc_info.isdbt_layers_b_info_bits,
            HI_DRV_FRONTEND_ISDBT_LAYER_B, layer_exist);

        layer_exist = signal_info.info.signal_info.isdbt.isdbt_layers.bits.layer_c_exist;
        frontend_proc_read_isdbt_lock_info_layer(s,
            &signal_info.info.signal_info.isdbt.isdbt_tmcc_info.isdbt_layers_c_info_bits,
            HI_DRV_FRONTEND_ISDBT_LAYER_C, layer_exist);
    }

    return HI_SUCCESS;
}

static hi_s32 frontend_proc_read_isdbt(struct seq_file *s, hi_u32 i)
{
    hi_s32 ret;

    ret = frontend_proc_read_isdbt_common(s, i);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_proc_read_isdbt_common, ret);
        return ret;
    }

    ret = frontend_proc_read_isdbt_lock_info(s, i);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_proc_read_isdbt_lock_info, ret);
        return ret;
    }
    return HI_SUCCESS;
}

static const hi_char *frontend_dvbt_fft_mode(hi_drv_frontend_fft fft_mode_index)
{
    switch (fft_mode_index) {
        case HI_DRV_FRONTEND_FFT_1K:
            return "1K";
        case HI_DRV_FRONTEND_FFT_2K:
            return "2K";
        case HI_DRV_FRONTEND_FFT_4K:
            return "4K";
        case HI_DRV_FRONTEND_FFT_8K:
            return "8K";
        case HI_DRV_FRONTEND_FFT_16K:
            return "16K";
        case HI_DRV_FRONTEND_FFT_32K:
            return "32K";
        case HI_DRV_FRONTEND_FFT_64K:
            return "64K";
        default:
            return "INVALID";
    }
    return "INVALID";
}

static const hi_char *frontend_dvbt_guard_interval(hi_drv_frontend_guard_intv guard_interval_index)
{
    switch (guard_interval_index) {
        case HI_DRV_FRONTEND_GUARD_INTV_1_128:
            return "1/128";
        case HI_DRV_FRONTEND_GUARD_INTV_1_32:
            return "1/32";
        case HI_DRV_FRONTEND_GUARD_INTV_1_16:
            return "1/16";
        case HI_DRV_FRONTEND_GUARD_INTV_1_8:
            return "1/8";
        case HI_DRV_FRONTEND_GUARD_INTV_1_4:
            return "1/4";
        case HI_DRV_FRONTEND_GUARD_INTV_19_128:
            return "19/128";
        case HI_DRV_FRONTEND_GUARD_INTV_19_256:
            return "19/256";
        default:
            return "INVALID";
    }
    return "INVALID";
}

static const hi_char *frontend_dvbt_hier_mode(hi_drv_frontend_dvbt_hierarchy hier_mode_index)
{
    switch (hier_mode_index) {
        case HI_DRV_FRONTEND_DVBT_HIERARCHY_NO:
            return "NONE";
        case HI_DRV_FRONTEND_DVBT_HIERARCHY_ALHPA1:
            return "alpha1";
        case HI_DRV_FRONTEND_DVBT_HIERARCHY_ALHPA2:
            return "alpha2";
        case HI_DRV_FRONTEND_DVBT_HIERARCHY_ALHPA4:
            return "alpha4";
        default:
            return "INVALID";
    }
    return "INVALID";
}

static const hi_char *frontend_dvbt_ts_priority(hi_drv_frontend_dvbt_ts_priority ts_priority_index)
{
    switch (ts_priority_index) {
        case HI_DRV_FRONTEND_DVBT_TS_PRIORITY_NONE:
            return "NONE";
        case HI_DRV_FRONTEND_DVBT_TS_PRIORITY_HP:
            return "HP";
        case HI_DRV_FRONTEND_DVBT_TS_PRIORITY_LP:
            return "LP";
        default:
            return "INVALID";
    }
    return "INVALID";
}

static hi_s32 frontend_proc_read_dvbt_t2_common(struct seq_file *s, hi_u32 i)
{
    hi_s32 ret;
    const hi_char *tuner_lock = NULL;
    fe_ioctrl_ber_per signal_ber = {0};
    fe_ioctrl_status  status = {0};
    hi_char snr[PROC_SIGNALINFO_LEN];
    fe_ioctrl_data_buf signal_strength = {0};
    drv_fe_ctrl_ctx *frontend_context = NULL;

    status.port = i;
    ret = drv_fe_ctrl_get_status(&status);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_get_status, ret);
        return ret;
    }
    tuner_lock = frontend_proc_read_status(status.lock_status);

    frontend_context = drv_frontend_get_context(i);
    drv_fe_check_pointer(frontend_context);

    signal_ber.port = i;
    ret = drv_fe_ctrl_get_ber(&signal_ber);
    if (ret != HI_SUCCESS && ret != HI_ERR_FRONTEND_NOT_CONNECT) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_get_ber, ret);
        return ret;
    }

    signal_strength.port = i;
    ret = drv_fe_ctrl_get_signal_strength(&signal_strength);
    if (ret != HI_SUCCESS && ret != HI_ERR_FRONTEND_NOT_CONNECT) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_get_signal_strength, ret);
        return ret;
    }

    ret = frontend_proc_read_snr(i, snr, PROC_SIGNALINFO_LEN);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_proc_read_snr, ret);
        return ret;
    }

    osal_proc_print(s, "Lock State    :%-10s |Frequency(KHz) :%-10d |Bandwith       :%-10d\n",
        tuner_lock, frontend_context->connect_attr.frequency, frontend_context->connect_attr.srbw.band_width);
    osal_proc_print(s, "BER           :%d.%03dE%d   |SNR(dB)        :%-10s |RSSI(dBuv)     :%-10d\n",
        signal_ber.num.decimal_val, signal_ber.num.integer_val, signal_ber.num.power,
        snr, signal_strength.data_buf[1]);

    return HI_SUCCESS;
}

static hi_s32 frontend_proc_read_dvbt_t2_lock_info(struct seq_file *s, hi_u32 i)
{
    hi_s32 ret;
    const hi_char *guard_interval = NULL;
    const hi_char *qam_type = NULL;
    const hi_char *fec_rate = NULL;
    const hi_char *fft_mode = NULL;
    const hi_char *hier_mode = NULL;
    const hi_char *ts_priority = NULL;
    const hi_char *base_lite = NULL;
    fe_ioctrl_signal_info signal_info = {0};

    signal_info.port = i;
    ret = drv_fe_ctrl_get_signal_info(&signal_info);
    if (ret == HI_ERR_FRONTEND_NOT_CONNECT) {
        return HI_SUCCESS;
    } else if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_get_signal_info, ret);
        return ret;
    }

    if (signal_info.info.sig_type == HI_DRV_FRONTEND_SIG_TYPE_DVB_T) {
        qam_type = frontend_dtv_mode_type(signal_info.info.signal_info.dvbt.mod_type);
        fec_rate = frontend_dtv_fec_rate(signal_info.info.signal_info.dvbt.fec_rate);
        fft_mode = frontend_dvbt_fft_mode(signal_info.info.signal_info.dvbt.fft_mode);
        guard_interval = frontend_dvbt_guard_interval(signal_info.info.signal_info.dvbt.guard_intv);
        hier_mode = frontend_dvbt_hier_mode(signal_info.info.signal_info.dvbt.hier_mod);
        ts_priority = frontend_dvbt_ts_priority(signal_info.info.signal_info.dvbt.ts_priority);

        osal_proc_print(s, "QAM Mode      :%-10s |FEC rate        :%-10s |FFT Mode       :%-10s\n",
            qam_type, fec_rate, fft_mode);
        osal_proc_print(s, "GI            :%-10s |HierMod         :%-10s |TSPrio         :%-10s\n",
            guard_interval, hier_mode, ts_priority);
    } else {
        qam_type = frontend_dtv_mode_type(signal_info.info.signal_info.dvbt2.mod_type);
        fec_rate = frontend_dtv_fec_rate(signal_info.info.signal_info.dvbt2.fec_rate);
        fft_mode = frontend_dvbt_fft_mode(signal_info.info.signal_info.dvbt2.fft_mode);
        guard_interval = frontend_dvbt_guard_interval(signal_info.info.signal_info.dvbt2.guard_intv);
        if (signal_info.info.signal_info.dvbt2.channel_mode == HI_DRV_FRONTEND_DVBT2_MODE_BASE) {
            base_lite = "Base";
        } else {
            base_lite = "Lite";
        }

        osal_proc_print(s, "QAM Mode      :%-10s |FEC rate       :%-10s |FFT Mode       :%-10s\n",
            qam_type, fec_rate, fft_mode);
        osal_proc_print(s, "GI            :%-10s |Pilot Pattern  :PP%-8d |Channel Mode   :%-10s\n",
            guard_interval, signal_info.info.signal_info.dvbt2.pilot_pattern + 1, base_lite);
    }

    return HI_SUCCESS;
}

static hi_s32 frontend_proc_read_dvbt_t2(struct seq_file *s, hi_u32 i)
{
    hi_s32 ret;

    ret = frontend_proc_read_dvbt_t2_common(s, i);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_proc_read_dvbt_t2_common, ret);
        return ret;
    }

    ret = frontend_proc_read_dvbt_t2_lock_info(s, i);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_proc_read_dvbt_t2_lock_info, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static const hi_char *frontend_proc_read_cab_qam(frontend_qam_type qam_type)
{
    switch (qam_type) {
        case QAM_TYPE_16:
            return "QAM_16";
        case QAM_TYPE_32:
            return "QAM_32";
        case QAM_TYPE_64:
            return "QAM_64";
        case QAM_TYPE_128:
            return "QAM_128";
        case QAM_TYPE_256:
            return "QAM_256";
        default :
            return "INVALID";
    }
    return "INVALID";
}

static hi_s32 frontend_proc_read_cab(struct seq_file *s, hi_u32 i)
{
    hi_s32 ret;
    const hi_char *qam_type = NULL;
    const hi_char *tuner_lock = NULL;
    hi_char snr[PROC_SIGNALINFO_LEN] = {0};

    fe_ioctrl_ber_per signal_ber = {0};
    fe_ioctrl_status status = {0};
    fe_ioctrl_data_buf signal_strength = {0};
    drv_fe_ctrl_ctx *frontend_context = NULL;

    status.port = i;
    ret = drv_fe_ctrl_get_status(&status);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_get_status, ret);
        return ret;
    }
    tuner_lock = frontend_proc_read_status(status.lock_status);

    drv_fe_ctrl_check_port(i);
    frontend_context = drv_frontend_get_context(i);
    qam_type = frontend_proc_read_cab_qam(frontend_context->connect_attr.modpol.qam_type);

    ret = frontend_proc_read_snr(i, snr, PROC_SIGNALINFO_LEN);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_proc_read_snr, ret);
        return ret;
    }

    signal_ber.port = i;
    ret = drv_fe_ctrl_get_ber(&signal_ber);
    if (ret != HI_SUCCESS && ret != HI_ERR_FRONTEND_NOT_CONNECT) {
        HI_ERR_PRINT_FUNC_RES(frontend_proc_read_snr, ret);
        return ret;
    }

    signal_strength.port = i;
    ret = drv_fe_ctrl_get_signal_strength(&signal_strength);
    if (ret != HI_SUCCESS && ret != HI_ERR_FRONTEND_NOT_CONNECT) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_get_signal_strength, ret);
        return ret;
    }

    osal_proc_print(s, "Lock State    :%-10s |Frequency(KHz) :%-10d |Symbol Rate    :%-10d\n",
        tuner_lock, frontend_context->connect_attr.frequency, frontend_context->connect_attr.srbw.symbol_rate);
    osal_proc_print(s, "QAM Mode      :%-10s |BER            :%d.%03dE%d   |SNR(dB)        :%-10s\n",
        qam_type, signal_ber.num.decimal_val, signal_ber.num.integer_val, signal_ber.num.power, snr); /* 2:index */
    osal_proc_print(s, "RSSI(dBuv)    :%-10d\n", signal_strength.data_buf[1]);
    return HI_SUCCESS;
}

static hi_s32 frontend_proc_read_sat_common(struct seq_file *s, hi_u32 i)
{
    hi_s32 ret;
    const hi_char *lnb_type = NULL;
    const hi_char *iq_spectrum = NULL;
    fe_ioctrl_status status = {0};
    const hi_char *tuner_lock = NULL;
    drv_fe_ctrl_ctx *frontend_context = NULL;

    frontend_context = drv_frontend_get_context(i);

    status.port = i;
    ret = drv_fe_ctrl_get_status(&status);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_get_status, ret);
        return ret;
    }
    tuner_lock = frontend_proc_read_status(status.lock_status);
    if (frontend_context->sat_attr.lnb_ctrl_dev < HI_DRV_LNB_CTRL_DEV_TYPE_MAX) {
        lnb_type = g_fe_lnb_type[frontend_context->sat_attr.lnb_ctrl_dev];
    } else {
        lnb_type = "INVALID";
    }

    if (frontend_context->sat_attr.iq_spectrum == HI_DRV_TUNER_IQ_SPECTRUM_MODE_NORMAL) {
        iq_spectrum = "Normal";
    } else if (frontend_context->sat_attr.iq_spectrum == HI_DRV_TUNER_IQ_SPECTRUM_MODE_INVERT) {
        iq_spectrum = "Invert";
    } else {
        iq_spectrum = "INVALID";
    }

    osal_proc_print(s, "LNB Type      :%-10s |LNB Addres     :%-10d |IQ Spectrum    :%-10s\n",
        lnb_type, frontend_context->sat_attr.lnb_dev_address, iq_spectrum);
    osal_proc_print(s, "Lock State    :%-10s |Frequency(KHz) :%-10d |Symbol Rate    :%-10d\n",
        tuner_lock, frontend_context->connect_attr.frequency, frontend_context->connect_attr.srbw.symbol_rate);
    return HI_SUCCESS;
}

static hi_s32 frontend_proc_read_sat_info(struct seq_file *s, hi_u32 i)
{
    hi_s32 ret;
    const hi_char *qam_type = NULL;
    const hi_char *fec_rate = NULL;
    hi_char snr[PROC_SIGNALINFO_LEN] = {0};

    fe_ioctrl_signal_info signal_info = {0};
    fe_ioctrl_ber_per signal_ber = {0};
    fe_ioctrl_data_buf signal_strength = {0};

    ret = frontend_proc_read_snr(i, snr, PROC_SIGNALINFO_LEN);
    if (ret != HI_SUCCESS && ret != HI_ERR_FRONTEND_NOT_CONNECT) {
        HI_ERR_PRINT_FUNC_RES(frontend_proc_read_snr, ret);
        return ret;
    }

    signal_ber.port = i;
    ret = drv_fe_ctrl_get_ber(&signal_ber);
    if (ret != HI_SUCCESS && ret != HI_ERR_FRONTEND_NOT_CONNECT) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_get_ber, ret);
        return ret;
    }

    signal_strength.port = i;
    ret = drv_fe_ctrl_get_signal_strength(&signal_strength);
    if (ret != HI_SUCCESS && ret != HI_ERR_FRONTEND_NOT_CONNECT) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_get_signal_strength, ret);
        return ret;
    }

    osal_proc_print(s, "BER           :%d.%03dE%d   |SNR(dB)        :%-10s |RSSI(dBuV)     :%-10d\n",
        signal_ber.num.decimal_val, signal_ber.num.integer_val, signal_ber.num.power,
        snr, signal_strength.data_buf[1]);

    ret = drv_fe_ctrl_get_signal_info(&signal_info);
    if (ret == HI_ERR_FRONTEND_NOT_CONNECT) {
        return HI_SUCCESS;
    } else if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_get_signal_info, ret);
        return ret;
    }

    qam_type = frontend_dtv_mode_type(signal_info.info.signal_info.dvbs2x.mod_type);
    fec_rate = frontend_dtv_fec_rate(signal_info.info.signal_info.dvbs2x.fec_rate);
    osal_proc_print(s, "Modulation    :%-10s |FEC Rate        :%-10s\n", qam_type, fec_rate);
    return HI_SUCCESS;
}

static hi_s32 frontend_proc_read_sat(struct seq_file *s, hi_u32 i)
{
    hi_s32 ret;

    ret = frontend_proc_read_sat_common(s, i);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_get_signal_strength, ret);
        return ret;
    }

    ret = frontend_proc_read_sat_info(s, i);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_proc_read_sat_info, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_void frontend_proc_write_help(hi_void)
{
    hi_drv_proc_echo_helper("-------------------------------------------------");
    hi_drv_proc_echo_helper("frontend--------------------------------------------\n");
    hi_drv_proc_echo_helper("echo cmd parameter > /proc/msp/demod_reg: all of the demod register.\n");
    hi_drv_proc_echo_helper("echo standby [0/1] > /proc/msp/frontend : standby mode(0:disable 1:enable) \n");
    hi_drv_proc_echo_helper("echo autotest [0/1] > /proc/msp/frontend : autotest mode(0:disable 1:enable) \n");
    hi_drv_proc_echo_helper("echo dr [port 0~4] reg_addr > /proc/msp/frontend : demod register value.\n");
    hi_drv_proc_echo_helper("echo dw [port 0~4] reg_addr reg_value > /proc/msp/frontend : demod register value..\n");
    hi_drv_proc_echo_helper("echo tr [port 0~4] reg_addr > /proc/msp/frontend : demod register value.\n");
    hi_drv_proc_echo_helper("echo tw [port 0~4] reg_addr reg_value > /proc/msp/frontend : demod register value.\n");
    hi_drv_proc_echo_helper("echo lr [port 0~4] reg_addr > /proc/msp/frontend : demod register value.\n");
    hi_drv_proc_echo_helper("echo lw [port 0~4] reg_addr reg_value > /proc/msp/frontend : ");
    hi_drv_proc_echo_helper("demod register value.'lnbwrite' can also be written in 'tw'.\n");
    hi_drv_proc_echo_helper("echo qtc_mem_size record_file_size   > /proc/msp/frontend : mem_size ->the size of mem");
    hi_drv_proc_echo_helper(" which we malloc it;record_file_size->the size of qtc savefile .unit:MB.\n");
    hi_drv_proc_echo_helper("                            record_file_size = 0, do not test the data storage");
    hi_drv_proc_echo_helper(" device write speed,write 'max_ram' MB to data storage device.\n");
    hi_drv_proc_echo_helper("                            record_file_size != 0, test the data storage");
    hi_drv_proc_echo_helper(" device write speed,write 'record_file_size' MB to data storage device.\n");
    hi_drv_proc_echo_helper("echo cir [port 0~4] > /proc/msp/frontend : channel impulse response data.\n");
    hi_drv_proc_echo_helper("echo cir [port 0~4] > /proc/msp/frontend : channel impulse response data.\n");
    hi_drv_proc_echo_helper("echo catch [port 0~4]  catch_sel iq_sel > /proc/msp/frontend : constellation data.\n");
    hi_drv_proc_echo_helper("echo mcu [port 0~4]  [update/on/off] > /proc/msp/frontend : update:need to tftp");
    hi_drv_proc_echo_helper(" conved_i2c_w_set_port1.tcl file to /mnt directory,the cmd will update new mcu.\n");
    hi_drv_proc_echo_helper("echo print [port 0~4] [on/off] > /proc/msp/frontend:");
    hi_drv_proc_echo_helper(" on :enable print;off :disable print.\n");
    hi_drv_proc_echo_helper("echo help > /proc/msp/frontend : help signal\n");
    hi_drv_proc_echo_helper("---------------------------------------------------");
    hi_drv_proc_echo_helper("end---------------------------------------------------\n");
}

static hi_s32 frontend_proc_write_get_arg(hi_u32 *arg_data, hi_char *arg_string, hi_u32 mode)
{
    if ((arg_string == HI_NULL) || (arg_data == HI_NULL)) {
        hi_drv_proc_echo_helper("parameter is not enough\n");
        frontend_proc_write_help();
        return HI_FAILURE;
    }

    *arg_data = (hi_u32)simple_strtol(arg_string, NULL, mode);

    return HI_SUCCESS;
}

static hi_s32 frontend_proc_write_qtc_func(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX])
{
    hi_s32  ret;
    hi_u32 mem_size;
    hi_u32 record_file_size = 0;
    hi_drv_frontend_connect_para  connect_para = {0};
    drv_fe_demod_ops *frontend_demod_ops = NULL;
    drv_fe_ctrl_ctx  *frontend_context = NULL;

    frontend_context = drv_frontend_get_context(0);
    drv_fe_check_pointer(frontend_context);
    frontend_demod_ops  = &frontend_context->demod_ops;
    drv_fe_check_pointer(frontend_demod_ops);

    ret = frontend_proc_write_get_arg(&mem_size, argv[1], 10); /* 10:para */
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_proc_write_get_arg, ret);
        return ret;
    }
    ret = frontend_proc_write_get_arg(&record_file_size, argv[2], 10); /* 2:index 10:para */
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_proc_write_get_arg, ret);
        return ret;
    }
    osal_printk("QTC mem_size:%d, record_file_size:%d\n", mem_size, record_file_size);
    if (mem_size > 0) {
        ret = memset_s(&connect_para, sizeof(hi_drv_frontend_connect_para),
            0, sizeof(hi_drv_frontend_connect_para));
        if (ret != EOK) {
            LOG_ERR_PRINT("memset_s failed. ret: %d\n", ret);
            return  HI_FAILURE;
        }
        connect_para.sig_type = frontend_context->attr.sig_type;
        connect_para.connect_para.ter.freq = frontend_context->connect_attr.frequency;

        drv_fe_check_func_pointer(frontend_demod_ops->get_qtc_data);
        ret = frontend_demod_ops->get_qtc_data(0, &connect_para, mem_size, record_file_size);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(frontend_demod_ops->get_qtc_data, ret);
            return ret;
        }
    } else {
        osal_printk("qtc memory size should be positive number.\n");
    }

    return HI_SUCCESS;
}

static hi_s32 drv_frontend_proc_write_standby(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
    hi_void *private)
{
    hi_char *proc_cmd = HI_NULL;
    hi_char *proc_cmd_para = HI_NULL;

    if (argc != 2) { /* 2:3:index */
        LOG_ERR_PRINT("argc is invalid!argc:%u\n", argc);
        return HI_FAILURE;
    }
    drv_fe_check_pointer(argv);
    drv_fe_check_pointer(private);
    proc_cmd = argv[0];
    proc_cmd_para = argv[1];
    drv_fe_check_pointer(proc_cmd);
    drv_fe_check_pointer(proc_cmd_para);

    if (strncmp("standby", proc_cmd, strlen("standby")) == 0) {
        if (strncmp("0", proc_cmd_para, 1) == 0) {
            drv_fe_ctrl_resume();
            hi_drv_proc_echo_helper("drv_fe_ctrl_resume ok!\n");
        } else if (strncmp("1", proc_cmd_para, 1) == 0) {
            drv_fe_ctrl_suspend();
            hi_drv_proc_echo_helper("drv_fe_ctrl_suspend ok!\n");
        } else {
            frontend_proc_write_help();
        }
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_frontend_proc_write_autotest(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
    hi_void *private)
{
    drv_fe_ctrl_ctx *frontend_context = NULL;
    hi_u32 port;
    hi_char *proc_cmd = HI_NULL;
    hi_char *proc_cmd_para = HI_NULL;

    if (argc != 2) { /* 2:3:index */
        LOG_ERR_PRINT("argc is invalid!argc:%u\n", argc);
        return HI_FAILURE;
    }
    drv_fe_check_pointer(argv);
    drv_fe_check_pointer(private);
    proc_cmd = argv[0];
    port = simple_strtol(argv[1], NULL, 16); /* 16:data format */
    proc_cmd_para = argv[2]; /* 2:index */
    drv_fe_check_pointer(proc_cmd);
    drv_fe_check_pointer(proc_cmd_para);

    if (strncmp("autotest", proc_cmd, strlen("autotest")) == 0) {
        frontend_context = drv_frontend_get_context(0);
        if (strncmp("1", proc_cmd_para, 1) == 0) {
            frontend_context->auto_test = HI_TRUE;
            hi_drv_proc_echo_helper("set autotest true!\n");
        } else if (strncmp("0", proc_cmd_para, 1) == 0) {
            frontend_context->auto_test = HI_FALSE;
            hi_drv_proc_echo_helper("set autotest false!\n");
        } else {
            frontend_proc_write_help();
        }
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_frontend_proc_write_print(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
    hi_void *private)
{
    drv_fe_ctrl_ctx *frontend_context = NULL;
    hi_u32 port;
    hi_char *proc_cmd = HI_NULL;
    hi_char *proc_cmd_para = HI_NULL;

    if (argc != 3) { /* 2:3:index */
        LOG_ERR_PRINT("argc is invalid!argc:%u\n", argc);
        return HI_FAILURE;
    }
    drv_fe_check_pointer(argv);
    drv_fe_check_pointer(private);
    proc_cmd = argv[0];
    port = simple_strtol(argv[1], NULL, 16); /* 16:data format */
    proc_cmd_para = argv[2]; /* 2:index */
    drv_fe_check_pointer(proc_cmd);
    drv_fe_check_pointer(proc_cmd_para);

    /* echo  hi_tuner=4 > /proc/msp/log, then use this cmd to control print. */
    frontend_context = drv_frontend_get_context(port);
    drv_fe_check_pointer(frontend_context);

    if (strncmp("print", proc_cmd, strlen("print")) == 0) {
        if (strncmp("off", proc_cmd_para, strlen("off")) == 0) {
            frontend_context->print_enable = HI_FALSE;
            hi_drv_proc_echo_helper("set print_enable disable!\n");
        } else {
            frontend_context->print_enable = HI_TRUE;
            hi_drv_proc_echo_helper("set print_enable enable!\n");
        }
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_frontend_proc_write_regprint(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
    hi_void *private)
{
    drv_fe_ctrl_ctx *frontend_context = NULL;
    hi_u32 port;
    hi_char *proc_cmd = HI_NULL;
    hi_char *proc_cmd_para = HI_NULL;

    if (argc != 3) { /* 2:3:index */
        LOG_ERR_PRINT("argc is invalid!argc:%u\n", argc);
        return HI_FAILURE;
    }
    drv_fe_check_pointer(argv);
    drv_fe_check_pointer(private);
    proc_cmd = argv[0];
    port = simple_strtol(argv[1], NULL, 16); /* 16:data format */
    proc_cmd_para = argv[2]; /* 2:index */
    drv_fe_check_pointer(proc_cmd);
    drv_fe_check_pointer(proc_cmd_para);

    frontend_context = drv_frontend_get_context(port);
    drv_fe_check_pointer(frontend_context);

    if (strncmp("regprint", proc_cmd, strlen("regprint")) == 0) {
        if (strncmp("off", proc_cmd_para, strlen("off")) == 0) {
            frontend_context->reg_def_print_enable = HI_FALSE;
            hi_drv_proc_echo_helper("set b_reg_def_print_enable disable!\n");
        } else {
            frontend_context->reg_def_print_enable = HI_TRUE;
            hi_drv_proc_echo_helper("set b_reg_def_print_enable enable!\n");
        }
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static const hi_char *drv_frontend_get_sig_type_str(hi_drv_frontend_sig_type sig_type)
{
    switch (sig_type) {
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_C:
            return "DVB-C";
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_C2:
            return "DVB-C2";
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_C:
            return "ISDB-C";
        case HI_DRV_FRONTEND_SIG_TYPE_J83B:
            return "J83B";
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_S:
            return "DVB-S";
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_S2:
            return "DVB_S2";
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_S2X:
            return "DVB_S2X";
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_S:
            return "ISDB-S";
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_S3:
            return "ISDB-S3";
        case HI_DRV_FRONTEND_SIG_TYPE_ABSS:
            return "ABSS";
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T:
            return "DVB-T";
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T2:
            return "DVB-T2";
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_T:
            return "ISDB-T";
        case HI_DRV_FRONTEND_SIG_TYPE_ATSC_T:
            return "ATSC-T";
        case HI_DRV_FRONTEND_SIG_TYPE_ATSC_T3:
            return "ATSC-T3";
        case HI_DRV_FRONTEND_SIG_TYPE_DTMB:
            return "DTMB";
        default:
            return NULL;
    }
    return NULL;
}

static hi_s32 drv_frontend_proc_read_valid(drv_fe_ctrl_ctx *frontend_context)
{
    if (drv_frontend_get_sig_type_str(frontend_context->attr.sig_type) == NULL) {
        LOG_ERR_PRINT("sig_type:0x%x is invalid\n", frontend_context->attr.sig_type);
        return HI_FAILURE;
    }

    if (frontend_context->attr.tuner_dev_type >= (sizeof(g_fe_tuner_type) / sizeof(g_fe_tuner_type[0]))) {
        LOG_ERR_PRINT("tuner_dev_type is out of bound\n");
        LOG_ERR_PRINT("tuner_dev_type:%d\n", frontend_context->attr.tuner_dev_type);
        LOG_ERR_PRINT("bound:%d\n", (sizeof(g_fe_tuner_type) / sizeof(g_fe_tuner_type[0])));
        return HI_FAILURE;
    }

    if ((frontend_context->attr.demod_dev_type - HI_DRV_DEMOD_DEV_TYPE_3130I)
            >= (sizeof(g_fe_demod_type) / sizeof(g_fe_demod_type[0]))) {
        LOG_ERR_PRINT("demod_dev_type is out of bound\n");
        LOG_ERR_PRINT("demod_dev_type:%d\n", frontend_context->attr.demod_dev_type);
        LOG_ERR_PRINT("bound:%d\n", (sizeof(g_fe_demod_type) / sizeof(g_fe_demod_type[0])));
        return HI_FAILURE;
    }

    if (frontend_context->cur_tuner_system >= (sizeof(g_fe_dtv_sys) / sizeof(g_fe_dtv_sys[0]))) {
        LOG_ERR_PRINT("en_cur_tuner_system is out of bound\n");
        LOG_ERR_PRINT("en_cur_tuner_system:%d\n", frontend_context->cur_tuner_system);
        LOG_ERR_PRINT("bound:%d\n", (sizeof(g_fe_dtv_sys) / sizeof(g_fe_dtv_sys[0])));
        return HI_FAILURE;
    }
    LOG_INFO_PRINT("tuner_sys:%s.\n", g_fe_dtv_sys[frontend_context->cur_tuner_system]);

    return HI_SUCCESS;
}

static hi_s32 drv_frontend_proc_read_execute(struct seq_file *s, hi_u32 i, drv_fe_ctrl_ctx *frontend_context)
{
    hi_s32 ret = HI_FAILURE;

    osal_proc_print(s, "Signal Type   :%-10s |Tuner Type     :%-10s |Tuner Address  :0x%-8X\n",
        drv_frontend_get_sig_type_str(frontend_context->attr.sig_type),
        g_fe_tuner_type[frontend_context->attr.tuner_dev_type],  frontend_context->attr.tuner_addr);
    osal_proc_print(s, "Demod Type    :%-10s |Demod Address  :0x%-8X |I2C Channel    :%-10d\n",
        g_fe_demod_type[(frontend_context->attr.demod_dev_type - HI_DRV_DEMOD_DEV_TYPE_3130I)],
        frontend_context->attr.demod_addr, frontend_context->attr.demod_i2c_channel);
    if (frontend_context->attr.sig_type == HI_DRV_FRONTEND_SIG_TYPE_ISDB_T) {
        ret = frontend_proc_read_isdbt(s, i);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(frontend_proc_read_isdbt, ret);
            return ret;
        }
    } else if ((frontend_context->attr.sig_type == HI_DRV_FRONTEND_SIG_TYPE_DVB_T) ||
        (frontend_context->attr.sig_type == HI_DRV_FRONTEND_SIG_TYPE_DVB_T2)) {
        ret = frontend_proc_read_dvbt_t2(s, i);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(frontend_proc_read_dvbt_t2, ret);
            return ret;
        }
    } else if ((frontend_context->attr.sig_type >= HI_DRV_FRONTEND_SIG_TYPE_DVB_C) &&
        (frontend_context->attr.sig_type <= HI_DRV_FRONTEND_SIG_TYPE_J83B)) {
        ret = frontend_proc_read_cab(s, i);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(frontend_proc_read_cab, ret);
            return ret;
        }
    } else if ((frontend_context->attr.sig_type >= HI_DRV_FRONTEND_SIG_TYPE_DVB_S) &&
        (frontend_context->attr.sig_type <= HI_DRV_FRONTEND_SIG_TYPE_ABSS)) {
        ret = frontend_proc_read_sat(s, i);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(frontend_proc_read_sat, ret);
            return ret;
        }
    } else {
        LOG_ERR_PRINT("sigal type:%d\n", frontend_context->attr.sig_type);
    }

    return HI_SUCCESS;
}

hi_s32 drv_frontend_proc_read(hi_void *seqfile, hi_void *private)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 i;
    drv_fe_ctrl_ctx *frontend_context = NULL;

    for (i = 0; i < DRV_FRONTEND_NUM; i++) {
        drv_fe_ctrl_check_port(i);
        frontend_context = drv_frontend_get_context(i);
        drv_fe_check_pointer(frontend_context);
        if (frontend_context->attr_inited != HI_TRUE) {
            continue;
        } else {
            osal_proc_print(seqfile, "------------------------------FRONTEND : PORT");
            osal_proc_print(seqfile, "[%d] ATTR --------------------------------------\n", i);
        }

        if (frontend_context->attr.demod_dev_type < HI_DRV_DEMOD_DEV_TYPE_3130I) {
            osal_proc_print(seqfile,
                "------------------------------demod type is NULL-------------------------------\n");
            continue;
        }

        ret = drv_frontend_proc_read_valid(frontend_context);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(drv_frontend_proc_read_valid, ret);
            continue;
        }

        ret = drv_frontend_proc_read_execute(seqfile, i, frontend_context);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(drv_frontend_proc_read_execute, ret);
            continue;
        }
    }

    osal_proc_print(seqfile,
        "----------------------------------------------------------------------------------------------\n");

    return HI_SUCCESS;
}

hi_s32 drv_frontend_proc_read_reg(hi_void *seqfile, hi_void *private)
{
    hi_u8 i;
    errno_t err;

    drv_fe_ctrl_ctx *frontend_context = NULL;
    drv_fe_demod_ops *frontend_demod_ops = NULL;
    drv_fe_reg_para reg_para;

    err = memset_s(&reg_para, sizeof(drv_fe_reg_para), 0, sizeof(drv_fe_reg_para));
    if (err != EOK) {
        return HI_ERR_FRONTEND_SECURE_CHECK;
    }
    reg_para.seq_file = seqfile;

    for (i = 0; i < DRV_FRONTEND_NUM; i++) {
        frontend_context = drv_frontend_get_context(i);
        drv_fe_check_pointer(frontend_context);

        if (frontend_context->attr_inited == HI_FALSE) {
            continue;
        }

        frontend_demod_ops = &frontend_context->demod_ops;
        drv_fe_check_pointer(frontend_demod_ops);
        drv_fe_check_func_pointer(frontend_demod_ops->get_registers);
        frontend_demod_ops->get_registers(i, (hi_void *)seqfile);
    }

    return HI_SUCCESS;
}

static hi_s32 frontend_proc_write_help_process(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
    hi_void *private)
{
    frontend_proc_write_help();
    return 0;
}

static hi_s32 drv_frontend_proc_write_reg_rd(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
    hi_void *private)
{
    hi_s32 ret;
    hi_u32 port = 0;
    hi_u32 reg_addr = 0;
    hi_u8 reg_value = 0;
    hi_char *cmd_mode = HI_NULL;
    i2c_device_type dev_type = I2C_DEMOD_TYPE;

    if (argc < 3) { /* 3: arg num */
        LOG_ERR_PRINT("argc is invalid! argc:%u\n", argc);
        return HI_FAILURE;
    }
    drv_fe_check_pointer(private);
    drv_fe_check_pointer(argv);

    ret = frontend_proc_write_get_arg(&port, argv[1], 16); /* 16:hex */
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_proc_write_get_arg, ret);
        return ret;
    }
    ret = frontend_proc_write_get_arg(&reg_addr, argv[2], 16); /* 2:index, 16:hex */
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_proc_write_get_arg, ret);
        return ret;
    }

    drv_fe_ctrl_check_port(port);
    cmd_mode = argv[0];
    if (strncmp("dr", cmd_mode, strlen("dr")) == 0) {
        dev_type = I2C_DEMOD_TYPE;
    } else if (strncmp("tr", cmd_mode, strlen("tr")) == 0) {
        dev_type = I2C_TUNER_TYPE;
    } else if (strncmp("lr", cmd_mode, strlen("lr")) == 0) {
        dev_type = I2C_LNB_TYPE;
    }
    ret = frontend_i2c_read_byte(dev_type, port, (hi_u8)reg_addr, &reg_value);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_i2c_read_byte, ret);
        return ret;
    }
    return HI_SUCCESS;
}

static hi_s32 drv_frontend_proc_write_reg_wr(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
    hi_void *private)
{
    hi_s32 ret;
    hi_u32 port = 0;
    hi_u32 reg_addr = 0;
    hi_u32 reg_value = 0;
    hi_char *cmd_mode = HI_NULL;
    i2c_device_type dev_type = I2C_DEMOD_TYPE;

    if (argc < 4) { /* 4: arg num */
        LOG_ERR_PRINT("argc is invalid! argc:%u\n", argc);
        return HI_FAILURE;
    }
    drv_fe_check_pointer(private);
    drv_fe_check_pointer(argv);

    ret = frontend_proc_write_get_arg(&port, argv[1], 16); /* 16:hex */
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_proc_write_get_arg, ret);
        return ret;
    }
    ret = frontend_proc_write_get_arg(&reg_addr, argv[2], 16); /* 2:index, 16:hex */
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_proc_write_get_arg, ret);
        return ret;
    }
    ret = frontend_proc_write_get_arg(&reg_value, argv[3], 16); /* 3:index, 16:hex */
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_proc_write_get_arg, ret);
        return ret;
    }
    drv_fe_ctrl_check_port(port);
    cmd_mode = argv[0];
    if (strncmp("dw", cmd_mode, strlen("dw")) == 0) {
        dev_type = I2C_DEMOD_TYPE;
    } else if (strncmp("tw", cmd_mode, strlen("tr")) == 0) {
        dev_type = I2C_TUNER_TYPE;
    } else if (strncmp("lw", cmd_mode, strlen("lw")) == 0) {
        dev_type = I2C_LNB_TYPE;
    }
    ret = frontend_i2c_write_byte(dev_type, port, (hi_u8)reg_addr, (hi_u8)reg_value);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_i2c_write_byte, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_frontend_proc_write_qtc(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    if (argc != 3) { /* 3: arg num */
        LOG_ERR_PRINT("argc is invalid!argc:%u\n", argc);
        return HI_FAILURE;
    }

    drv_fe_check_pointer(argv);
    drv_fe_check_pointer(private);
    ret = frontend_proc_write_qtc_func(argc, argv);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_proc_write_qtc_func, ret);
        return ret;
    }
    return HI_SUCCESS;
}

static hi_s32 drv_frontend_proc_write_cir(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    hi_u32 port = 0;

    drv_fe_demod_ops *frontend_demod_ops = NULL;
    drv_fe_ctrl_ctx  *frontend_context = NULL;
    if (argc != 2) { /* 2: arg num */
        LOG_ERR_PRINT("argc is invalid!argc:%u\n", argc);
        return HI_FAILURE;
    }

    drv_fe_check_pointer(argv);
    drv_fe_check_pointer(private);

    ret = frontend_proc_write_get_arg(&port, argv[1], 16); /* 2:index,16:para */
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_proc_write_get_arg, ret);
        return ret;
    }
    drv_fe_ctrl_check_port(port);

    frontend_context = drv_frontend_get_context(port);
    drv_fe_check_pointer(frontend_context);
    frontend_demod_ops  = &frontend_context->demod_ops;
    drv_fe_check_pointer(frontend_demod_ops);

    drv_fe_check_func_pointer(frontend_demod_ops->get_cir);
    ret = frontend_demod_ops->get_cir(port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_demod_ops->get_cir, ret);
        return ret;
    }
    return HI_SUCCESS;
}

static hi_s32 drv_frontend_proc_write_catch(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
    hi_void *private)
{
    hi_s32 ret;
    hi_u32 port = 0;
    hi_u32 arg2 = 0;
    hi_u32 arg3 = 0;
    drv_fe_demod_ops *frontend_demod_ops = NULL;
    drv_fe_ctrl_ctx  *frontend_context = NULL;
    if (argc != 4) { /* 4: arg num */
        LOG_ERR_PRINT("argc is invalid!argc:%u\n", argc);
        return HI_FAILURE;
    }

    drv_fe_check_pointer(argv);
    drv_fe_check_pointer(private);
    ret = frontend_proc_write_get_arg(&port, argv[1], 16); /* 16:para */
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_proc_write_get_arg, ret);
        return ret;
    }
    drv_fe_ctrl_check_port(port);

    frontend_context = drv_frontend_get_context(port);
    drv_fe_check_pointer(frontend_context);
    frontend_demod_ops  = &frontend_context->demod_ops;
    drv_fe_check_pointer(frontend_demod_ops);

    ret = frontend_proc_write_get_arg(&arg2, argv[2], 16); /* 2:index 16:para */
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_proc_write_get_arg, ret);
        return ret;
    }
    ret = frontend_proc_write_get_arg(&arg3, argv[3], 16); /* 3:index 16:para */
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_proc_write_get_arg, ret);
        return ret;
    }
    LOG_DBG_PRINT("catch_sel:%d, iq_sel:%d\n", arg2, arg3);

    drv_fe_check_func_pointer(frontend_demod_ops->catch_data);
    ret = frontend_demod_ops->catch_data(port, arg2, arg3);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_demod_ops->catch_data, ret);
        return ret;
    }
    return HI_SUCCESS;
}


static hi_s32 drv_frontend_proc_write_mcu(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    hi_u32 port = 0;
    drv_fe_demod_ops *frontend_demod_ops = NULL;
    drv_fe_ctrl_ctx  *frontend_context = NULL;

    if (argc != 3) { /* 3: arg num */
        LOG_ERR_PRINT("argc is invalid!argc:%u\n", argc);
        return HI_FAILURE;
    }
    drv_fe_check_pointer(argv);
    drv_fe_check_pointer(private);
    ret = frontend_proc_write_get_arg(&port, argv[1], 16); /* 16:para */
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_proc_write_get_arg, ret);
        return ret;
    }
    drv_fe_ctrl_check_port(port);

    frontend_context = drv_frontend_get_context(port);
    drv_fe_check_pointer(frontend_context);
    frontend_demod_ops  = &frontend_context->demod_ops;
    drv_fe_check_pointer(frontend_demod_ops);

    drv_fe_check_pointer(argv[2]); /* 2:index */
    LOG_DBG_PRINT("function:%s\n", argv[2]); /* 2:index */
    drv_fe_check_func_pointer(frontend_demod_ops->mcu_operate);
    ret = frontend_demod_ops->mcu_operate(port, argv[2]); /* 2:index */
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(frontend_demod_ops->mcu_operate, ret);
        return ret;
    }
    return HI_SUCCESS;
}

static osal_proc_cmd g_fe_proc_cmd[] = {
    { "help",            frontend_proc_write_help_process },

    { "dr",        drv_frontend_proc_write_reg_rd },
    { "dw",        drv_frontend_proc_write_reg_wr },
    { "tr",        drv_frontend_proc_write_reg_rd },
    { "tw",        drv_frontend_proc_write_reg_wr },
    { "lr",        drv_frontend_proc_write_reg_rd },
    { "lw",        drv_frontend_proc_write_reg_wr },

    { "qtc",       drv_frontend_proc_write_qtc },
    { "cir",       drv_frontend_proc_write_cir },
    { "catch",     drv_frontend_proc_write_catch },
    { "mcu",       drv_frontend_proc_write_mcu },

    { "standby",   drv_frontend_proc_write_standby },
    { "autotest",  drv_frontend_proc_write_autotest },
    { "print",     drv_frontend_proc_write_print },
    { "regprint",  drv_frontend_proc_write_regprint },
};

hi_s32 fe_drv_proc_add(hi_void)
{
    osal_proc_entry *fe_proc_entry;

    fe_proc_entry = osal_proc_add("frontend", strlen("frontend"));
    if (fe_proc_entry == HI_NULL) {
        LOG_ERR_PRINT("frontend add proc fail!");
        return HI_FAILURE;
    }
    fe_proc_entry->read = drv_frontend_proc_read;
    fe_proc_entry->cmd_cnt = sizeof(g_fe_proc_cmd) / sizeof(osal_proc_cmd);
    fe_proc_entry->cmd_list = g_fe_proc_cmd;
    fe_proc_entry->private = fe_proc_entry->name;
    return HI_SUCCESS;
}

hi_s32 fe_drv_proc_reg_add(hi_void)
{
    osal_proc_entry *fe_proc_reg_entry;

    fe_proc_reg_entry = osal_proc_add("demod_reg", strlen("demod_reg"));
    if (fe_proc_reg_entry == HI_NULL) {
        LOG_ERR_PRINT("tuner_reg add proc fail!");
        return HI_FAILURE;
    }
    fe_proc_reg_entry->read = drv_frontend_proc_read_reg;
    fe_proc_reg_entry->cmd_cnt = 0;
    fe_proc_reg_entry->cmd_list = NULL;

    return HI_SUCCESS;
}


