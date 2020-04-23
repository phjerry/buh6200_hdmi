/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: mxl254.c
 * Author: Mariusz Murawski
 * Create: 2012
 */

#include "hi_drv_mem.h"
#include "linux/hisilicon/securec.h"
#include "drv_frontend_ctrl.h"
#include "drv_fe_math.h"
#include "hi_type.h"
#include "hi_drv_i2c.h"
#include "hi_debug.h"

#include "MxL_HRCLS_Common.h"
#include "MxL_HRCLS_CommonApi.h"
#include "MxL_HRCLS_PhyCtrl.h"
#include "MxL_HRCLS_CalCtrl.h"
#include "MxL_HRCLS_OEM_Drv.h"

/*
 * Device ID used in this sample code.
 * If more Hercules devices are used at the same time, they should have
 * consecutive.
 * This is customer's application responsibility to enumerate devices.
 */
#define HRCLS_DEVICE_ID 0

#define HRCLS_EXAMPLE_CODE_DEVICE  "mxl254"
#define HRCLS_EXAMPLE_CODE_VERSION "1.10"

/* Default carystal capacitors value. This value is platform dependent */
#define HRCLS_XTAL_CAP_VALUE 0
#define MAX_TUNER 4

/* Firmware filename */
#define MXL254_CONNCECT_TIMEOUT 1000 /* ms */
static hi_u32 g_mxl254_connect_timeout = MXL254_CONNCECT_TIMEOUT;

MXL_BOOL_E  g_supported;

#ifdef MXL_HRCLS_254_ENABLE

hi_bool g_mxl254_inin_flag = 0;
static oem_data_t g_oemdata;

MXL_STATUS_E mxl254_set_status(MXL_STATUS_E status)
{
    MXL_HRCLS_DEV_VER_T dev_ver_info;

    /* HRCLS_XTAL_CAP_VALUE, Can adjust the crystals */
    status = MxLWare_HRCLS_API_CfgDevXtalSetting(HRCLS_DEVICE_ID, HRCLS_XTAL_CAP_VALUE);
    if (status != MXL_SUCCESS) {
        LOG_INFO_PRINT("[HRCLS][%d] Xtal setting FAILED\n", HRCLS_DEVICE_ID);
        return status;
    }

    /* Check MxLWare, Firmware and Bootloader version */
    status = mxl_checkVersion(&dev_ver_info);
    if (status != MXL_SUCCESS) {
        LOG_INFO_PRINT("[HRCLS][%d] Version checking FAILED!\n", HRCLS_DEVICE_ID);
        return status;
    }

    /* Make sure firmware is not already downloaded */
    if (dev_ver_info.firmwareDownloaded == MXL_TRUE) {
        LOG_INFO_PRINT("[HRCLS][%d] Firmware already running. Forgot about h/w reset?\n", HRCLS_DEVICE_ID);
        return MXL_FAILURE;
    }

    /* Download firmware */
    status = mxl_downloadFirmware(&g_supported);
    if (status != MXL_SUCCESS) {
        LOG_ERR_PRINT("[HRCLS][%d] Firmware download FAILED\n", HRCLS_DEVICE_ID);
        return status;
    }

    status = mxl_enableFbTuner(); /* Enable fullband tuner */
    if (status != MXL_SUCCESS) {
        LOG_ERR_PRINT("[HRCLS][%d] Enable FB tuner FAILED\n", HRCLS_DEVICE_ID);
        return status;
    }

    if (g_supported == MXL_TRUE) {
        /* This step can be skipped as NO_MUX_4 is a default mode */
        status = MxLWare_HRCLS_API_CfgXpt(HRCLS_DEVICE_ID, MXL_HRCLS_XPT_MODE_NO_MUX_4);
        if (status != MXL_SUCCESS) {
            LOG_ERR_PRINT("[HRCLS][%d] MxLWare_HRCLS_API_CfgXpt FAILED\n", HRCLS_DEVICE_ID);
            return status;
        }
    }

#if 1
    /* Only needed for 4-wire TS mode, skip for 3-wire mode */
    status = MxLWare_HRCLS_API_CfgDemodMpegOutGlobalParams(HRCLS_DEVICE_ID, MXL_HRCLS_MPEG_CLK_POSITIVE,
                                                           MXL_HRCLS_MPEG_DRV_MODE_1X, MXL_HRCLS_MPEG_CLK_56_21MHz);
    if (status != MXL_SUCCESS) {
        LOG_ERR_PRINT("[HRCLS][%d] Global MPEG params setting FAILED\n", HRCLS_DEVICE_ID);
        return status;
    }
#endif

    return status;
}
hi_s32 mxl254_init(hi_u32 tuner_port)
{
    /* Example code for device initialization, and FB and NB tuner handover */
    MXL_STATUS_E status = 0;
    drv_fe_ctrl_ctx *frontend_context = drv_frontend_get_context(tuner_port);
    drv_fe_check_pointer(frontend_context);

    LOG_INFO_PRINT("Device: %s (version %s)\n", HRCLS_EXAMPLE_CODE_DEVICE, HRCLS_EXAMPLE_CODE_VERSION);
    if (tuner_port >= MAX_TUNER) {
        LOG_ERR_PRINT("Bad tuner_port\n");
        return HI_FAILURE;
    }

    g_oemdata.i2cAddress = frontend_context->attr.demod_addr;
    g_oemdata.i2cIndex = frontend_context->attr.demod_i2c_channel;

    if (!g_mxl254_inin_flag) {
        if (frontend_context->attr.tuner_dev_type == HI_DRV_TUNER_DEV_TYPE_MXL214) {
            status = MxLWare_HRCLS_API_CfgDrvInit(HRCLS_DEVICE_ID, (void *)&g_oemdata, MXL_HRCLS_DEVICE_214);
        } else {
            status = MxLWare_HRCLS_API_CfgDrvInit(HRCLS_DEVICE_ID, (void *)&g_oemdata, MXL_HRCLS_DEVICE_254);
        }
        if (status != MXL_SUCCESS) {
            LOG_ERR_PRINT("[HRCLS][%d] Driver initialization FAILED\n", HRCLS_DEVICE_ID);
            return status;
        }

        status = MxLWare_HRCLS_API_CfgDevReset(HRCLS_DEVICE_ID); /* Perform hardware reset of the device. */
        if (status != MXL_SUCCESS) {
            LOG_ERR_PRINT("[HRCLS][%d] . reset FAILED\n", HRCLS_DEVICE_ID);
            return status;
        }
        g_mxl254_inin_flag = HI_TRUE;
    }

    status = mxl254_set_status(status);
    return HI_SUCCESS;
}

MXL_HRCLS_QAM_TYPE_E mxl254_qamtype(frontend_acc_qam_params *channel)
{
    MXL_HRCLS_QAM_TYPE_E qamtype;
    switch (channel->modpol.qam_type) {
        case QAM_TYPE_16:
            qamtype = MXL_HRCLS_QAM16 ;
            break;
        case QAM_TYPE_32:
            qamtype = MXL_HRCLS_QAM32 ;
            break;
        case QAM_TYPE_64:
            qamtype = MXL_HRCLS_QAM64 ;
            break;
        case QAM_TYPE_128:
            qamtype = MXL_HRCLS_QAM128 ;
            break;
        case QAM_TYPE_256:
            qamtype = MXL_HRCLS_QAM256 ;
            break;
        default:
            LOG_ERR_PRINT("qamtype(%d) is unsupported \n", channel->modpol.qam_type);
            return HI_FAILURE;
            break;
    }

    return qamtype;
}

hi_s32 mxl254_set_value(hi_u32 tuner_port, MXL_HRCLS_DMD_ID_E *demodid, MXL_HRCLS_CHAN_ID_E *chanid)
{
    switch (tuner_port) {
        case 0: /* 0 argument */
            *chanid = MXL_HRCLS_CHAN0;
            *demodid = MXL_HRCLS_DEMOD0;
            break;
        case 1: /* 1 argument */
            *chanid = MXL_HRCLS_CHAN1;
            *demodid = MXL_HRCLS_DEMOD1;
            break;
        case 2: /* 2 argument */
            *chanid = MXL_HRCLS_CHAN2;
            *demodid = MXL_HRCLS_DEMOD2;
            break;
        case 3: /* 3 argument */
            *chanid = MXL_HRCLS_CHAN3;
            *demodid = MXL_HRCLS_DEMOD3;
            break;
        default:
            LOG_ERR_PRINT("tuner_port(%d) goes over the top(%d) \n", tuner_port, MAX_TUNER);
            return HI_FAILURE;
            break;
    }

    return HI_SUCCESS;
}
hi_s32 mxl254_connect(hi_u32 tuner_port, frontend_acc_qam_params *channel)
{
    MXL_HRCLS_CHAN_ID_E chanid = 0;
    MXL_HRCLS_DMD_ID_E demodid = 0;
    MXL_HRCLS_QAM_TYPE_E qamtype;
    hi_u32 ret;
    hi_u32 freqkhz;
    hi_u8 bwmhz = 8;
    hi_u16 symbol_rate_ksps;
    MXL_HRCLS_ANNEX_TYPE_E annex_type = MXL_HRCLS_ANNEX_A;
    MXL_STATUS_E status = 0;
    drv_fe_ctrl_ctx *frontend_context = NULL;

    frontend_context = drv_frontend_get_context(tuner_port);
    drv_fe_check_pointer(frontend_context);

    if (tuner_port >= MAX_TUNER) {
        LOG_ERR_PRINT("tuner_port(%d) goes over the top(%d) \n", tuner_port, MAX_TUNER);
        return HI_FAILURE;
    }

    ret = mxl254_set_value(tuner_port, &demodid, &chanid);
    qamtype = mxl254_qamtype(channel);

    switch (frontend_context->attr.sig_type) {
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_C:
            annex_type = MXL_HRCLS_ANNEX_A;
            bwmhz = 8; /* 8 argument */
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_J83B:
            annex_type = MXL_HRCLS_ANNEX_B;
            bwmhz = 6; /* 6 argument */
            break;
        default:
            break;
    }

    if (status != MXL_SUCCESS) {
        LOG_ERR_PRINT("[HRCLS][%d] down_interruptible FAILED\n", HRCLS_DEVICE_ID);
        return status;
    }
    symbol_rate_ksps = channel->srbw.symbol_rate / 1000; /* div 1000 convert to KHz */
    freqkhz = channel->frequency;
    status = mxl_lockDemod(chanid, demodid, freqkhz, bwmhz, annex_type, qamtype, symbol_rate_ksps, MXL_HRCLS_IQ_AUTO,
                           g_supported);
    if (status != MXL_SUCCESS) {
        return status;
    }
    return status;
}

hi_s32 mxl254_get_snr(hi_u32 tuner_port, hi_s32 *snr)
{
    MXL_STATUS_E status;
    hi_u16  snr_temp = 0;
    drv_fe_check_pointer(snr);
    status = MxLWare_HRCLS_API_ReqDemodSnr(HRCLS_DEVICE_ID, tuner_port, &snr_temp);
    if (status != MXL_SUCCESS) {
        LOG_ERR_PRINT("[HRCLS][%d] MxLWare_HRCLS_API_ReqDemodSnr failed \n", __LINE__);
        return status;
    }

    snr[0] = (hi_u32)snr_temp * 10; /* Multiplication 10 */

    return HI_SUCCESS;
}

hi_void mxl254_connect_timeout(hi_u32 connect_timeout)
{
    g_mxl254_connect_timeout = connect_timeout;
    return;
}

hi_s32 mxl254_set_ts_type(hi_u32 tuner_port, hi_drv_demod_ts_mode ts_type)
{
    return 0;
}

hi_s32 mxl254_get_status(hi_u32 tuner_port, hi_drv_frontend_lock_status  *tuner_status)
{
    MXL_BOOL_E qamlock = MXL_FALSE;
    MXL_BOOL_E relock = MXL_FALSE;
    MXL_BOOL_E mpeglock = MXL_FALSE;
    MXL_BOOL_E feclock = MXL_FALSE;

    drv_fe_check_pointer(tuner_status);
    if (tuner_port >= MAX_TUNER) { /* Check tuner port and init */
        LOG_ERR_PRINT("Bad tuner_port\n");
        return HI_FAILURE;
    }

    MxLWare_HRCLS_API_ReqDemodAllLockStatus(HRCLS_DEVICE_ID, tuner_port, &qamlock, &feclock, &mpeglock, &relock);

    if ((MXL_TRUE == qamlock) && (MXL_TRUE == feclock) && (MXL_TRUE == mpeglock)) {
        *tuner_status = HI_DRV_FRONTEND_LOCK_STATUS_LOCKED;
    } else {
        *tuner_status = HI_DRV_FRONTEND_LOCK_STATUS_DROPPED;
    }

    return HI_SUCCESS;
}

hi_s32 mxl254_get_signal_strength(hi_u32 tuner_port, hi_u32 *signal_strength)
{
    MXL_STATUS_E status;
    hi_u16 pwr_ind_buv = 0;
    MXL_HRCLS_RX_PWR_ACCURACY_E accuracy = MXL_HRCLS_PWR_AVERAGED;

    drv_fe_check_pointer(signal_strength);
    if (tuner_port >= MAX_TUNER) {
        LOG_ERR_PRINT("Bad tuner_port\n");
        return HI_FAILURE;
    }
    status = MxLWare_HRCLS_API_ReqTunerRxPwr(HRCLS_DEVICE_ID, tuner_port, &pwr_ind_buv, &accuracy);
    if ((status != MXL_SUCCESS) || (accuracy == MXL_HRCLS_PWR_INVALID)) {
        LOG_ERR_PRINT("mxl254_get_signal_strength error!\n");
        return status;
    }

    signal_strength[1] = (hi_u32)(pwr_ind_buv / 10 - 5); /* 5:value, 10:Division */
    return HI_SUCCESS;
}
hi_void mxl254_value(hi_u32 *tmp, hi_u32 *i, hi_u32 *ber_temp)
{
    while (*ber_temp >= 10) { /* 10: value */
        *ber_temp = *ber_temp / 10;
        (*i)++;
        if (*ber_temp >= 1000) { /* 1000: value */
            *tmp = *ber_temp;
        }
    }

    return;
}

hi_s32 mxl254_get_ber(hi_u32 tuner_port, hi_drv_frontend_scientific_num *num)
{
    hi_u32 tmp = 0;
    hi_u32 i = 0;
    hi_u32 ber_temp = 0;
    MXL_HRCLS_DMD_STAT_CNT_T stats_count = {0};
    MXL_STATUS_E status;

    if (tuner_port >= MAX_TUNER) {
        LOG_ERR_PRINT("Bad tuner_port\n");
        return HI_FAILURE;
    }
    status = MxLWare_HRCLS_API_ReqDemodErrorStat(HRCLS_DEVICE_ID, tuner_port, &stats_count);
    if (status != MXL_SUCCESS) {
        LOG_ERR_PRINT("mxl254_get_ber error!\n");
        return status;
    }
    if (stats_count.CwReceived > 0) {
        /* 10000000:value */
        tmp = (hi_u32) u64mod ((hi_u64)(stats_count.CwErrCount * 10000000), (stats_count.CwReceived));
         /* 6127 = 10000000 /(204*8) */
        ber_temp = (hi_u32) u64mod ((hi_u64)(stats_count.CorrBits * 6127), (stats_count.CwReceived));
        ber_temp = ber_temp + 26 * (tmp); /* 26: multiple */
        if (ber_temp != 0) {
            mxl254_value(&tmp, &i, &ber_temp);
        }
        num->integer_val = (hi_u16)ber_temp;
        num->decimal_val = (hi_u16)(tmp % 1000); /* division 1000 remainder */
        num->power = (hi_u16)(7 - i); /* 7:value */
    } else {
        num->integer_val = (hi_u16)1;
        num->decimal_val = (hi_u16)0;
        num->power = (hi_u16)0;
    }

    status = MxLWare_HRCLS_API_CfgDemodErrorStatClear(HRCLS_DEVICE_ID, tuner_port);
    if (status != MXL_SUCCESS) {
        LOG_ERR_PRINT("mxl254_get_ber error!\n");
        return status;
    }
    return HI_SUCCESS;
}

hi_s32 mxl254_get_powerspecdata(hi_u32 tuner_port, hi_u32 freq_start_inhz, hi_u32 freq_step_inhz,
                                hi_u32 numof_freq_steps, hi_s16 *power_data)
{
    MXL_STATUS_E status;
    drv_fe_check_pointer(power_data);
    if (tuner_port >= MAX_TUNER) {
        LOG_ERR_PRINT("Bad tuner_port\n");
        return HI_FAILURE;
    }
    status = MxLWare_HRCLS_API_ReqTunerPowerSpectrum(HRCLS_DEVICE_ID, freq_start_inhz, freq_step_inhz,
                                                     numof_freq_steps, power_data);
    if (status != MXL_SUCCESS) {
        LOG_ERR_PRINT("mxl254_get_powerspecdata error!\n");
        return status;
    }
    return HI_SUCCESS;
}

hi_s32 mxl254_get_signal_info(hi_u32 tuner_port, hi_drv_frontend_signal_info *info)
{
    hi_drv_frontend_sig_type sig_type;

    sig_type = drv_frontend_get_sig_type(tuner_port);

    switch (sig_type) {
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_C:
        case HI_DRV_FRONTEND_SIG_TYPE_J83B:
            info->sig_type = sig_type;
            break;
        default:
            break;
    }

    return HI_SUCCESS;
}

hi_s32 mxl254_standby(hi_u32 tuner_port, hi_u32 status)
{
    MXL_HRCLS_DEV_CONTEXT_T *dev_context = MxL_HRCLS_Ctrl_GetDeviceContext(tuner_port);

    if (dev_context) {
        dev_context->driverInitialized = 0;
        g_mxl254_inin_flag = 0;
        return HI_SUCCESS;
    } else {
        LOG_ERR_PRINT("MxL_HRCLS_Ctrl_GetDeviceContext is NULL!\n");
        return HI_FAILURE;
    }
}

hi_bool mxl254_is_ctrl_tuner(hi_u32 fe_port)
{
    return HI_TRUE;
}

hi_s32 drv_fe_adp_demod_mxl254_regist_func(drv_fe_demod_ops *demod_ops)
{
    drv_fe_check_pointer(demod_ops);

    /* COMMON */
    demod_ops->init = mxl254_init;
    demod_ops->connect = mxl254_connect;
    demod_ops->deinit = NULL;
    demod_ops->get_status = mxl254_get_status;
    demod_ops->get_ber = mxl254_get_ber;
    demod_ops->get_snr = mxl254_get_snr;
    demod_ops->get_signal_strength = mxl254_get_signal_strength;
    demod_ops->get_signal_info = mxl254_get_signal_info;
    demod_ops->get_freq_symb_offset = NULL;
    demod_ops->standby = mxl254_standby;
    demod_ops->is_ctrl_tuner = mxl254_is_ctrl_tuner;

    demod_ops->set_sat_attr = NULL;
    demod_ops->send_continuous22k = NULL;
    demod_ops->blindscan_action = NULL;
    demod_ops->blindscan_init = NULL;
    demod_ops->tp_verify = NULL;
    demod_ops->send_tone = NULL;
    demod_ops->get_stream_num = NULL;
    demod_ops->get_isi_id = NULL;
    demod_ops->set_isi_id = NULL;

    demod_ops->get_plp_num = NULL;
    demod_ops->get_plp_info = NULL;
    demod_ops->set_plp_para = NULL;
    demod_ops->connect_timeout = NULL;
    demod_ops->set_ts_out = NULL;
    demod_ops->i2c_bypass = NULL;

    /* only use to proc cmd */
    demod_ops->get_registers = NULL;
    demod_ops->i2c_read_byte = NULL;
    demod_ops->i2c_write_byte = NULL;

    return HI_SUCCESS;
}

#endif


