/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: cxd2878 driver
 * Author: SDK
 * Created: 2019-06-30
 */
#include "hi_drv_mem.h"
#include "linux/hisilicon/securec.h"
#include "drv_frontend_ctrl.h"
#include "drv_fe_math.h"
#include "sony_integ.h"
#include "sony_demod.h"
#include "hi_sony_i2c.h"
#include "sony_integ_dvbc.h"
#include "sony_demod_dvbc_monitor.h"
#include "sony_integ_j83b.h"
#include "sony_demod_j83b_monitor.h"
#include "sony_integ_isdbc.h"
#include "sony_demod_isdbc_monitor.h"
#include "sony_integ_dvbt2.h"
#include "sony_integ_dvbt.h"
#include "sony_integ_isdbt.h"
#include "sony_demod_isdbt_monitor.h"
#include "sony_demod_dvbt_monitor.h"
#include "sony_demod_dvbt2_monitor.h"
#include "sony_integ_dvbs_s2.h"
#include "sony_demod_dvbs_s2_monitor.h"
#include "sony_integ_isdbs.h"
#include "sony_demod_isdbs_monitor.h"
#include "sony_integ_isdbs3.h"
#include "sony_demod_isdbs3_monitor.h"
#include "sony_demod_sat_device_ctrl.h"
#ifdef TUNER_DEV_TYPE_CXD2871
#include "sony_tuner_ascot3.h"
#endif
#ifdef TUNER_DEV_TYPE_SUT_PJ987
#include "sut_pj987.h"
#endif

typedef struct {
    hi_u32                   port;
    sony_demod_t             demod;          /**< Demod IC driver struct instance */
    sony_integ_t             integ;          /**< Integ struct instance */
    sony_i2c_t               i2c;            /**< I2C struct instance */
    sony_tuner_t             tuner;          /**< tuner driver instance */
    sony_lnbc_t              lnbc;           /**< LNB controller driver instance */
#ifdef TUNER_DEV_TYPE_CXD2871
    sony_ascot3_t            ascot3;         /**< Sony ASCOT3 tuner driver instance */
#endif
} sony_driver_instance_t;

static sony_driver_instance_t *get_instance(hi_u32 port)
{
    drv_fe_ctrl_ctx *frontend_context = drv_frontend_get_context(port);
    if (frontend_context == NULL) {
        return NULL;
    }
    return (sony_driver_instance_t*)frontend_context->demod_private_para;
}

static hi_s32 convert_result(sony_result_t result)
{
    HI_DBG_PRINT_U32(result);
    switch (result) {
        case SONY_RESULT_OK:
            return HI_SUCCESS;
        case SONY_RESULT_ERROR_I2C:
            return HI_ERR_I2C_WRITE_TIMEOUT;
        case SONY_RESULT_ERROR_ARG:
            return HI_ERR_FRONTEND_INVALID_PARA;
        case SONY_RESULT_ERROR_UNLOCK:
        case SONY_RESULT_ERROR_HW_STATE:
            return HI_ERR_FRONTEND_NOT_CONNECT;
        default:
            return HI_FAILURE;
    }
}

static sony_dtv_bandwidth_t convert_band_width(hi_u32 band_width)
{
    switch (band_width) {
        case 1700: /* 1700:1.7M */
            return SONY_DTV_BW_1_7_MHZ;
        case 5000: /* 5000:5M */
            return SONY_DTV_BW_5_MHZ;
        case 6000: /* 6000:6M */
            return SONY_DTV_BW_6_MHZ;
        case 7000: /* 7000:7M */
            return SONY_DTV_BW_7_MHZ;
        case 8000: /* 8000:8M */
            return SONY_DTV_BW_8_MHZ;
        default:
            return SONY_DTV_BW_8_MHZ;
    }
}

static hi_s32 sony_tuner_init(sony_driver_instance_t *inst, hi_drv_tuner_dev_type tuner_type, hi_u8 tuner_addr)
{
    sony_result_t result;
    hi_s32 ret;

    switch (tuner_type) {
#ifdef TUNER_DEV_TYPE_CXD2871
        case HI_DRV_TUNER_DEV_TYPE_CXD2871:
            if (tuner_addr == 0xc0) {
                result = sony_tuner_ascot3_Create(&inst->tuner, SONY_ASCOT3_XTAL_16000KHz, tuner_addr, &inst->i2c,
                    SONY_ASCOT3_CONFIG_IFAGCSEL_ALL2 | SONY_ASCOT3_CONFIG_REFOUT_500mVpp, &inst->ascot3);
            } else {
                result = sony_tuner_ascot3_Create(&inst->tuner, SONY_ASCOT3_XTAL_16000KHz, tuner_addr, &inst->i2c,
                    SONY_ASCOT3_CONFIG_IFAGCSEL_ALL2 | SONY_ASCOT3_CONFIG_EXT_REF, &inst->ascot3);
            }
            if (result != SONY_RESULT_OK) {
                HI_ERR_PRINT_FUNC_RES(sony_tuner_ascot3_Create, result);
                return convert_result(result);
            }
            break;
#endif
#ifdef TUNER_DEV_TYPE_SUT_PJ987
        case HI_DRV_TUNER_DEV_TYPE_SUT_PJ987:
            ret = sony_tuner_sut_pj987_create(&inst->tuner, &inst->port);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(sony_tuner_sut_pj987_create, ret);
                return ret;
            }
            break;
#endif
        default:
            result = SONY_RESULT_OK;
            ret = HI_SUCCESS;
            break;
    }
    return HI_SUCCESS;
}

static hi_s32 sony_set_ts_type(sony_demod_t *demod, hi_drv_demod_ts_mode ts_type)
{
    sony_result_t result;
    switch (ts_type) {
        case HI_DRV_DEMOD_TS_MODE_SERIAL:
        case HI_DRV_DEMOD_TS_MODE_SERIAL_50:
            result = sony_demod_SetConfig(demod, SONY_DEMOD_CONFIG_PARALLEL_SEL, 0); /* 0: serial */
            if (result != SONY_RESULT_OK) {
                HI_ERR_PRINT_FUNC_RES(sony_integ_Create, result);
                return convert_result(result);
            }
            result = sony_demod_SetConfig(demod, SONY_DEMOD_CONFIG_SER_DATA_ON_MSB, 0); /* 0: use data0 */
            if (result != SONY_RESULT_OK) {
                HI_ERR_PRINT_FUNC_RES(sony_integ_Create, result);
                return convert_result(result);
            }
            break;
        case HI_DRV_DEMOD_TS_MODE_PARALLEL_MODE_A:
        case HI_DRV_DEMOD_TS_MODE_PARALLEL_MODE_B:
        case HI_DRV_DEMOD_TS_MODE_DEFAULT:
            result = sony_demod_SetConfig(demod, SONY_DEMOD_CONFIG_PARALLEL_SEL, 1); /* 1: PARALLEL */
            if (result != SONY_RESULT_OK) {
                HI_ERR_PRINT_FUNC_RES(sony_integ_Create, result);
                return convert_result(result);
            }
            break;
        case HI_DRV_DEMOD_TS_MODE_SERIAL_2BIT:
            result = sony_demod_SetConfig(demod, SONY_DEMOD_CONFIG_PARALLEL_SEL, 2); /* 2: serial 2 bit */
            if (result != SONY_RESULT_OK) {
                HI_ERR_PRINT_FUNC_RES(sony_integ_Create, result);
                return convert_result(result);
            }
            result = sony_demod_SetConfig(demod, SONY_DEMOD_CONFIG_SER_DATA_ON_MSB, 0);
            if (result != SONY_RESULT_OK) {
                HI_ERR_PRINT_FUNC_RES(sony_integ_Create, result);
                return convert_result(result);
            }
            break;
        default:
            LOG_ERR_PRINT("Not support this TS type:%d\n", ts_type);
            break;
    }
    return HI_SUCCESS;
}

static hi_s32 sony_demod_init(sony_driver_instance_t *inst, const hi_drv_frontend_attr *attr)
{
    sony_result_t result;
    sony_demod_create_param_t create_param;

    create_param.xtalFreq = SONY_DEMOD_XTAL_24000KHz;                   /* 24MHz Xtal */
    create_param.i2cAddressSLVT = attr->demod_addr;    /* I2C slave address is 0xD8 */
    create_param.tunerI2cConfig = SONY_DEMOD_TUNER_I2C_CONFIG_REPEATER; /* I2C repeater is used */
    create_param.atscCoreDisable = 0;                                   /* ATSC 1.0 core enable */

    result = sony_integ_Create(&inst->integ, &inst->demod, &create_param,
        &inst->i2c, &inst->tuner, NULL);
    if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_integ_Create, result);
        return convert_result(result);
    }
    result = sony_integ_Initialize(&inst->integ);
    if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_integ_Initialize, result);
        return convert_result(result);
    }
    if ((attr->tuner_dev_type == HI_DRV_TUNER_DEV_TYPE_SUT_PJ987) &&
        ((attr->sig_type == HI_DRV_FRONTEND_SIG_TYPE_ISDB_S) ||
        (attr->sig_type == HI_DRV_FRONTEND_SIG_TYPE_ISDB_S3))) {
        result = sony_demod_SetConfig(&inst->demod, SONY_DEMOD_CONFIG_SAT_IFAGCNEG, 1);
        if (result != SONY_RESULT_OK) {
            HI_ERR_PRINT_FUNC_RES(sony_integ_Create, result);
            return convert_result(result);
        }
    }
    return HI_SUCCESS;
}

hi_s32 cxd2878_init(hi_u32 port)
{
    hi_s32 ret;
    sony_driver_instance_t *inst = NULL;

    drv_fe_ctrl_ctx *frontend_context = drv_frontend_get_context(port);
    drv_fe_check_pointer(frontend_context);

    if (frontend_context->demod_private_para == NULL) {
        inst = (sony_driver_instance_t*)HI_KMALLOC(HI_ID_TUNER, sizeof(sony_driver_instance_t), GFP_KERNEL);
        if (inst == NULL) {
            return HI_ERR_FRONTEND_FAILED_ALLOC_MEM;
        }
        ret = memset_s(inst, sizeof(sony_driver_instance_t), 0,  sizeof(sony_driver_instance_t));
        if (ret != HI_SUCCESS) {
            return HI_ERR_FRONTEND_SECURE_CHECK;
        }
        frontend_context->demod_private_para = inst;
    } else {
        inst = (sony_driver_instance_t*)frontend_context->demod_private_para;
    }

    inst->port = port;

    ret = hi_sony_create_i2c(&inst->i2c, &frontend_context->attr.demod_i2c_channel);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hi_sony_create_i2c, ret);
    }

    ret = sony_tuner_init(inst, frontend_context->attr.tuner_dev_type, frontend_context->attr.tuner_addr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(sony_tuner_init, ret);
        return ret;
    }

    ret = sony_demod_init(inst, &frontend_context->attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(sony_demod_init, ret);
        return ret;
    }
    return HI_SUCCESS;
}

static hi_s32 cxd2878_connect_dvbc(sony_integ_t *integ, frontend_acc_qam_params *channel)
{
    sony_result_t result;

    sony_dvbc_tune_param_t tune_param_c;
    tune_param_c.centerFreqKHz = channel->frequency;    /* Channel center frequency in KHz */
    tune_param_c.bandwidth = convert_band_width(channel->tuner_band_width);
    HI_DBG_PRINT_U32(tune_param_c.centerFreqKHz);
    HI_DBG_PRINT_U32(tune_param_c.bandwidth);
    result = sony_integ_dvbc_Tune(integ, &tune_param_c);
    if (result == SONY_RESULT_ERROR_TIMEOUT || result == SONY_RESULT_ERROR_UNLOCK) {
        channel->sync_lock_status = HI_DRV_FRONTEND_LOCK_STATUS_LOCKED;
        return HI_SUCCESS;
    } else if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_integ_dvbc_Tune, result);
        return convert_result(result);
    }
    return HI_SUCCESS;
}

static hi_s32 cxd2878_connect_j83b(sony_integ_t *integ, frontend_acc_qam_params *channel)
{
    sony_result_t result;

    sony_j83b_tune_param_t tune_param;
    tune_param.centerFreqKHz = channel->frequency;    /* Channel center frequency in KHz */
    if (channel->srbw.symbol_rate <= 5361000) { /* 5361000:symb rate */
        tune_param.bandwidth = SONY_DTV_BW_J83B_5_06_5_36_MSPS;
    } else {
        tune_param.bandwidth = SONY_DTV_BW_J83B_5_60_MSPS;
    }
    HI_DBG_PRINT_U32(tune_param.centerFreqKHz);
    HI_DBG_PRINT_U32(tune_param.bandwidth);
    result = sony_integ_j83b_Tune(integ, &tune_param);
    if (result == SONY_RESULT_ERROR_TIMEOUT || result == SONY_RESULT_ERROR_UNLOCK) {
        channel->sync_lock_status = HI_DRV_FRONTEND_LOCK_STATUS_LOCKED;
        return HI_SUCCESS;
    } else if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_integ_j83b_Tune, result);
        return convert_result(result);
    }
    return HI_SUCCESS;
}

static hi_s32 cxd2878_connect_isdbc(sony_integ_t *integ, frontend_acc_qam_params *channel)
{
    sony_result_t result;

    sony_isdbc_tune_param_t tune_param;
    tune_param.centerFreqKHz = channel->frequency;    /* Channel center frequency in KHz */
    tune_param.tsmfMode = SONY_DEMOD_ISDBC_TSMF_MODE_SINGLE;
    tune_param.tsidType = SONY_ISDBC_TSID_TYPE_TSID;
    tune_param.tsid = 0;
    tune_param.networkId = 0;
    HI_DBG_PRINT_U32(tune_param.centerFreqKHz);
    result = sony_integ_isdbc_Tune(integ, &tune_param);
    if (result == SONY_RESULT_ERROR_TIMEOUT || result == SONY_RESULT_ERROR_UNLOCK) {
        channel->sync_lock_status = HI_DRV_FRONTEND_LOCK_STATUS_LOCKED;
        return HI_SUCCESS;
    } else if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_integ_j83b_Tune, result);
        return convert_result(result);
    }
    return HI_SUCCESS;
}

static hi_s32 cxd2878_connect_isdbt(sony_integ_t *integ, frontend_acc_qam_params *channel)
{
    sony_result_t result;

    sony_isdbt_tune_param_t tune_param_isdbt;
    tune_param_isdbt.centerFreqKHz = channel->frequency;    /* Channel center frequency in KHz */
    tune_param_isdbt.bandwidth = convert_band_width(channel->srbw.band_width);
    HI_DBG_PRINT_U32(tune_param_isdbt.centerFreqKHz);
    HI_DBG_PRINT_U32(tune_param_isdbt.bandwidth);
    result = sony_integ_isdbt_Tune(integ, &tune_param_isdbt);
    if (result == SONY_RESULT_ERROR_TIMEOUT || result == SONY_RESULT_ERROR_UNLOCK) {
        channel->sync_lock_status = HI_DRV_FRONTEND_LOCK_STATUS_LOCKED;
        return HI_SUCCESS;
    } else if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_integ_isdbt_Tune, result);
        return convert_result(result);
    }
    return HI_SUCCESS;
}

static hi_s32 cxd2878_connect_dvbt(sony_integ_t *integ, frontend_acc_qam_params *channel)
{
    sony_result_t result;

    sony_dvbt_tune_param_t tune_param;
    tune_param.centerFreqKHz = channel->frequency;    /* Channel center frequency in KHz */
    tune_param.bandwidth = convert_band_width(channel->srbw.band_width);
    if (channel->ter.dvbt == HI_DRV_FRONTEND_DVBT_TS_PRIORITY_LP) {
        tune_param.profile = SONY_DVBT_PROFILE_LP;
    } else {
        tune_param.profile = SONY_DVBT_PROFILE_HP;
    }
    HI_DBG_PRINT_U32(tune_param.centerFreqKHz);
    HI_DBG_PRINT_U32(tune_param.bandwidth);
    result = sony_integ_dvbt_Tune(integ, &tune_param);
    if (result == SONY_RESULT_ERROR_TIMEOUT || result == SONY_RESULT_ERROR_UNLOCK) {
        channel->sync_lock_status = HI_DRV_FRONTEND_LOCK_STATUS_LOCKED;
        return HI_SUCCESS;
    } else if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_integ_dvbt_Tune, result);
        return convert_result(result);
    }
    return HI_SUCCESS;
}

static hi_s32 cxd2878_connect_dvbt2(sony_integ_t *integ, frontend_acc_qam_params *channel)
{
    sony_result_t result;

    sony_dvbt2_tune_param_t tune_param;
    tune_param.centerFreqKHz = channel->frequency;    /* Channel center frequency in KHz */
    tune_param.bandwidth = convert_band_width(channel->srbw.band_width);
    if (channel->ter.dvbt2.channel_attr == HI_DRV_FRONTEND_DVBT2_MODE_LITE) {
        tune_param.profile = SONY_DVBT2_PROFILE_LITE;
    } else {
        tune_param.profile = SONY_DVBT2_PROFILE_BASE;
    }
    tune_param.dataPlpId = channel->ter.dvbt2.plp_id;
    HI_DBG_PRINT_U32(tune_param.centerFreqKHz);
    HI_DBG_PRINT_U32(tune_param.bandwidth);
    result = sony_integ_dvbt2_Tune(integ, &tune_param);
    if (result == SONY_RESULT_ERROR_TIMEOUT || result == SONY_RESULT_ERROR_UNLOCK) {
        channel->sync_lock_status = HI_DRV_FRONTEND_LOCK_STATUS_LOCKED;
        return HI_SUCCESS;
    } else if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_integ_dvbt2_Tune, result);
        return convert_result(result);
    }
    return HI_SUCCESS;
}

static hi_s32 cxd2878_connect_dvbs_s2(sony_integ_t *integ, frontend_acc_qam_params *channel)
{
    sony_result_t result;

    sony_dvbs_s2_tune_param_t tune_param;
    tune_param.system = SONY_DTV_SYSTEM_ANY;
    tune_param.centerFreqKHz = channel->frequency;    /* Channel center frequency in KHz */
    tune_param.symbolRateKSps = channel->srbw.symbol_rate / 1000; /* 1000:Hz to KHz */
    HI_DBG_PRINT_U32(tune_param.centerFreqKHz);
    HI_DBG_PRINT_U32(tune_param.symbolRateKSps);
    result = sony_integ_dvbs_s2_Tune(integ, &tune_param);
    if (result == SONY_RESULT_ERROR_TIMEOUT || result == SONY_RESULT_ERROR_UNLOCK) {
        channel->sync_lock_status = HI_DRV_FRONTEND_LOCK_STATUS_LOCKED;
        return HI_SUCCESS;
    } else if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_integ_dvbs_s2_Tune, result);
        return convert_result(result);
    }
    return HI_SUCCESS;
}

static hi_s32 cxd2878_connect_isdbs(sony_integ_t *integ, frontend_acc_qam_params *channel)
{
    sony_result_t result;
    sony_isdbs_tune_param_t tune_param;
    tune_param.centerFreqKHz = channel->frequency;    /* Channel center frequency in KHz */
    tune_param.tsidType = channel->stream_id_type;
    tune_param.tsid = channel->stream_id;
    HI_DBG_PRINT_U32(tune_param.centerFreqKHz);
    HI_DBG_PRINT_U32(tune_param.tsidType);
    HI_DBG_PRINT_U32(tune_param.tsid);
    result = sony_integ_isdbs_Tune(integ, &tune_param);
    if (result == SONY_RESULT_ERROR_TIMEOUT || result == SONY_RESULT_ERROR_UNLOCK) {
        channel->sync_lock_status = HI_DRV_FRONTEND_LOCK_STATUS_LOCKED;
        return HI_SUCCESS;
    } else if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_integ_isdbs_Tune, result);
        return convert_result(result);
    }
    return HI_SUCCESS;
}

static hi_s32 cxd2878_connect_isdbs3(sony_integ_t *integ, frontend_acc_qam_params *channel)
{
    sony_result_t result;
    sony_isdbs3_tune_param_t tune_param;
    tune_param.centerFreqKHz = channel->frequency;    /* Channel center frequency in KHz */
    tune_param.streamidType = channel->stream_id_type;
    tune_param.streamid = channel->stream_id;
    HI_DBG_PRINT_U32(tune_param.centerFreqKHz);
    HI_DBG_PRINT_U32(tune_param.streamidType);
    HI_DBG_PRINT_U32(tune_param.streamid);
    result = sony_integ_isdbs3_Tune(integ, &tune_param);
    if (result == SONY_RESULT_ERROR_TIMEOUT || result == SONY_RESULT_ERROR_UNLOCK) {
        HI_DBG_PRINT_U32(result);
        channel->sync_lock_status = HI_DRV_FRONTEND_LOCK_STATUS_LOCKED;
        return HI_SUCCESS;
    } else if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_integ_isdbs_Tune, result);
        return convert_result(result);
    }
    return HI_SUCCESS;
}

hi_s32 cxd2878_connect(hi_u32 port, frontend_acc_qam_params *channel)
{
    hi_s32 ret = HI_SUCCESS;
    drv_fe_ctrl_ctx *frontend_context = NULL;
    sony_driver_instance_t *inst = NULL;

    drv_fe_check_pointer(channel);

    frontend_context = drv_frontend_get_context(port);
    drv_fe_check_pointer(frontend_context);

    inst = (sony_driver_instance_t*)frontend_context->demod_private_para;
    drv_fe_check_pointer(inst);

    switch (frontend_context->attr.sig_type) {
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_C:
            ret = cxd2878_connect_dvbc(&inst->integ, channel);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_T:
            ret = cxd2878_connect_isdbt(&inst->integ, channel);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T:
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T2:
            if (channel->dvbt_mode) {
                ret = cxd2878_connect_dvbt(&inst->integ, channel);
            } else {
                ret = cxd2878_connect_dvbt2(&inst->integ, channel);
            }
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_J83B:
            ret = cxd2878_connect_j83b(&inst->integ, channel);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_C:
            ret = cxd2878_connect_isdbc(&inst->integ, channel);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_S:
            ret = cxd2878_connect_dvbs_s2(&inst->integ, channel);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_S:
            ret = cxd2878_connect_isdbs(&inst->integ, channel);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_S3:
            ret = cxd2878_connect_isdbs3(&inst->integ, channel);
            break;
        default:
            HI_ERR_PRINT_U32(frontend_context->attr.sig_type);
            ret = HI_ERR_FRONTEND_INVALID_SIGTYPE;
    }
    if (ret != HI_SUCCESS) {
        LOG_ERR_PRINT("cxd2878_connect error, ret = %d\n", ret);
    }
    return ret;
}

hi_s32 cxd2878_get_status (hi_u32 port, hi_drv_frontend_lock_status *lock_status)
{
    sony_result_t result;
    sony_demod_lock_result_t lock_result = SONY_DEMOD_LOCK_RESULT_NOTDETECT;
    hi_drv_frontend_sig_type sig_type;
    sony_driver_instance_t *inst = NULL;

    drv_fe_check_pointer(lock_status);

    inst = get_instance(port);
    drv_fe_check_pointer(inst);

    sig_type = drv_frontend_get_sig_type(port);
    switch (sig_type) {
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_C:
            result = sony_demod_dvbc_CheckTSLock(&inst->demod, &lock_result);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_T:
            result = sony_demod_isdbt_CheckTSLock(&inst->demod, &lock_result);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T:
            result = sony_demod_dvbt_CheckTSLock(&inst->demod, &lock_result);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T2:
            result = sony_demod_dvbt2_CheckTSLock(&inst->demod, &lock_result);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_J83B:
            result = sony_demod_j83b_CheckTSLock(&inst->demod, &lock_result);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_C:
            result = sony_demod_isdbc_CheckTSLock(&inst->demod, &lock_result);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_S:
            result = sony_demod_dvbs_s2_CheckTSLock(&inst->demod, &lock_result);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_S:
            result = sony_demod_isdbs_CheckTSLock(&inst->demod, &lock_result);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_S3:
            result = sony_demod_isdbs3_CheckTSTLVLock(&inst->demod, &lock_result);
            break;
        default:
            HI_ERR_PRINT_U32(sig_type);
            return HI_ERR_FRONTEND_INVALID_SIGTYPE;
    }
    if (result != SONY_RESULT_OK) {
        LOG_ERR_PRINT("sony demod check TS lock error, result = %d\n", result);
        return convert_result(result);
    }
    *lock_status = (lock_result == SONY_DEMOD_LOCK_RESULT_LOCKED) ?
        HI_DRV_FRONTEND_LOCK_STATUS_LOCKED : HI_DRV_FRONTEND_LOCK_STATUS_DROPPED;
    return HI_SUCCESS;
}

static sony_result_t cxd2878_get_ber_dvbs_s2(sony_demod_t *demod, uint32_t *ber_value)
{
    sony_result_t result;
    sony_dtv_system_t dtv_system = SONY_DTV_SYSTEM_DVBS;

    result = sony_demod_dvbs_s2_monitor_System(demod, &dtv_system);
    if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_demod_dvbs_s2_monitor_System, result);
        return result;
    }

    if (dtv_system == SONY_DTV_SYSTEM_DVBS) {
        result = sony_demod_dvbs_monitor_PreRSBER(demod, ber_value);
        HI_ERR_PRINT_FUNC_RES(sony_demod_dvbs_monitor_PreRSBER, result);
    } else {
        result = sony_demod_dvbs2_monitor_PreBCHBER(demod, ber_value);
        HI_ERR_PRINT_FUNC_RES(sony_demod_dvbs2_monitor_PreBCHBER, result);
    }
    return result;
}

static sony_result_t cxd2878_get_ber_isdbt(sony_demod_t *demod, hi_u32 *ber_value)
{
    sony_result_t result;
    hi_u32 ber_a, ber_b, ber_c; /* for isdbt */
    result = sony_demod_isdbt_monitor_PreRSBER(demod, &ber_a, &ber_b, &ber_c);
    *ber_value = ber_a;
    return result;
}

static sony_result_t cxd2878_get_ber_isdbs(sony_demod_t *demod, hi_u32 *ber_value)
{
    sony_result_t result;
    hi_u32 ber_h, ber_l, ber_tmcc; /* for isdbs */
    result = sony_demod_isdbs_monitor_PreRSBER(demod, &ber_h, &ber_l, &ber_tmcc);
    *ber_value = ber_h;
    return result;
}

static sony_result_t cxd2878_get_ber_isdbs3(sony_demod_t *demod, hi_u32 *ber_value)
{
    sony_result_t result;
    hi_u32 ber_h, ber_l; /* for isdbs3 */
    result = sony_demod_isdbs3_monitor_PreBCHBER(demod, &ber_h, &ber_l);
    *ber_value = ber_h;
    return result;
}

hi_s32 cxd2878_get_ber(hi_u32 port, hi_drv_frontend_scientific_num *ber) /* 3:size */
{
    sony_driver_instance_t *inst = NULL;
    sony_result_t result;
    hi_u32 ber_value = 0;

    drv_fe_check_pointer(ber);

    inst = get_instance(port);
    drv_fe_check_pointer(inst);

    switch (drv_frontend_get_sig_type(port)) {
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_C:
            result = sony_demod_dvbc_monitor_PreRSBER(&inst->demod, &ber_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_T:
            result = cxd2878_get_ber_isdbt(&inst->demod, &ber_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T:
            result = sony_demod_dvbt_monitor_PreRSBER(&inst->demod, &ber_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T2:
            result = sony_demod_dvbt2_monitor_PreBCHBER(&inst->demod, &ber_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_J83B:
            result = sony_demod_j83b_monitor_PreRSBER(&inst->demod, &ber_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_C:
            result = sony_demod_isdbc_monitor_PreRSBER(&inst->demod, &ber_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_S:
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_S2:
            result = cxd2878_get_ber_dvbs_s2(&inst->demod, &ber_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_S:
            result = cxd2878_get_ber_isdbs(&inst->demod, &ber_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_S3:
            result = cxd2878_get_ber_isdbs3(&inst->demod, &ber_value);
            break;
        default:
            HI_ERR_PRINT_U32(drv_frontend_get_sig_type(port));
            return HI_ERR_FRONTEND_INVALID_SIGTYPE;
    }
    if (result == SONY_RESULT_ERROR_HW_STATE) {
        return convert_result(result);
    } else if (result != SONY_RESULT_OK) {
        LOG_ERR_PRINT("sony demod get BER error, result = %d\n", result);
        return convert_result(result);
    }
    drv_fe_ber_convert(ber_value, 1e6, ber);
    return HI_SUCCESS;
}

static sony_result_t cxd2878_get_per_isdbt(sony_demod_t *demod, hi_u32 *per_value)
{
    sony_result_t result;
    hi_u32 per_a, per_b, per_c; /* for isdbt */
    result = sony_demod_isdbt_monitor_PER(demod, &per_a, &per_b, &per_c);
    *per_value = per_a;
    return result;
}

static sony_result_t cxd2878_get_per_isdbs(sony_demod_t *demod, hi_u32 *per_value)
{
    sony_result_t result;
    hi_u32 per_h, per_l; /* for isdbs */
    result = sony_demod_isdbs_monitor_PER(demod, &per_h, &per_l);
    *per_value = per_h;
    return result;
}

static sony_result_t cxd2878_get_per_isdbs3(sony_demod_t *demod, hi_u32 *per_value)
{
    sony_result_t result;
    hi_u32 per_h, per_l; /* for isdbs3 */
    result = sony_demod_isdbs3_monitor_PostBCHFER(demod, &per_h, &per_l);
    *per_value = per_h;
    return result;
}

hi_s32 cxd2878_get_per(hi_u32 port, hi_drv_frontend_scientific_num *per) /* 3:size */
{
    sony_driver_instance_t *inst = NULL;
    sony_result_t result;
    hi_u32 per_value = 0;

    drv_fe_check_pointer(per);

    inst = get_instance(port);
    drv_fe_check_pointer(inst);

    switch (drv_frontend_get_sig_type(port)) {
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_C:
            result = sony_demod_dvbc_monitor_PER(&inst->demod, &per_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_T:
            result = cxd2878_get_per_isdbt(&inst->demod, &per_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T:
            result = sony_demod_dvbt_monitor_PER(&inst->demod, &per_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T2:
            result = sony_demod_dvbt2_monitor_PER(&inst->demod, &per_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_J83B:
            result = sony_demod_j83b_monitor_PER(&inst->demod, &per_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_C:
            result = sony_demod_isdbc_monitor_PER(&inst->demod, &per_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_S:
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_S2:
            result = sony_demod_dvbs_s2_monitor_PER(&inst->demod, &per_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_S:
            result = cxd2878_get_per_isdbs(&inst->demod, &per_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_S3:
            result = cxd2878_get_per_isdbs3(&inst->demod, &per_value);
            break;
        default:
            HI_ERR_PRINT_U32(drv_frontend_get_sig_type(port));
            return HI_ERR_FRONTEND_INVALID_SIGTYPE;
    }
    if (result == SONY_RESULT_ERROR_HW_STATE) {
        return convert_result(result);
    } else if (result != SONY_RESULT_OK) {
        LOG_ERR_PRINT("sony demod get PER error, result = %d\n", result);
        return convert_result(result);
    }
    drv_fe_ber_convert(per_value, 1e6, per);
    return HI_SUCCESS;
}

hi_s32 cxd2878_get_snr(hi_u32 port, hi_s32 *snr)
{
    sony_driver_instance_t *inst = NULL;
    sony_result_t result;
    hi_u32 snr_value = 0;

    drv_fe_check_pointer(snr);

    inst = get_instance(port);
    drv_fe_check_pointer(inst);

    switch (drv_frontend_get_sig_type(port)) {
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_C:
            result = sony_demod_dvbc_monitor_SNR(&inst->demod, &snr_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_T:
            result = sony_demod_isdbt_monitor_SNR(&inst->demod, &snr_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T:
            result = sony_demod_dvbt_monitor_SNR(&inst->demod, &snr_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T2:
            result = sony_demod_dvbt2_monitor_SNR(&inst->demod, &snr_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_J83B:
            result = sony_demod_j83b_monitor_SNR(&inst->demod, &snr_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_C:
            result = sony_demod_isdbc_monitor_SNR(&inst->demod, &snr_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_S:
            result = sony_demod_dvbs_s2_monitor_CNR(&inst->demod, &snr_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_S:
            result = sony_demod_isdbs_monitor_CNR(&inst->demod, &snr_value);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_S3:
            result = sony_demod_isdbs3_monitor_CNR(&inst->demod, &snr_value);
            break;
        default:
            HI_ERR_PRINT_U32(drv_frontend_get_sig_type(port));
            return HI_ERR_FRONTEND_INVALID_SIGTYPE;
    }
    if (result == SONY_RESULT_ERROR_HW_STATE) {
        return convert_result(result);
    } else if (result != SONY_RESULT_OK) {
        LOG_ERR_PRINT("sony demod get SNR error, result = %d\n", result);
        return convert_result(result);
    }
    *snr = snr_value / 10; /* 10:unit */
    HI_DBG_PRINT_U32(*snr);
    return HI_SUCCESS;
}

hi_s32 cxd2878_get_signal_strength(hi_u32 port, hi_u32 *signal_strength)
{
    int32_t strength;
    sony_driver_instance_t *inst = NULL;
    sony_result_t result;

    drv_fe_check_pointer(signal_strength);

    inst = get_instance(port);
    drv_fe_check_pointer(inst);

    switch (drv_frontend_get_sig_type(port)) {
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_C:
            result = sony_integ_dvbc_monitor_RFLevel(&inst->integ, &strength);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T:
            result = sony_integ_dvbt_monitor_RFLevel(&inst->integ, &strength);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T2:
            result = sony_integ_dvbt2_monitor_RFLevel(&inst->integ, &strength);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_T:
            result = sony_integ_isdbt_monitor_RFLevel(&inst->integ, &strength);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_J83B:
            result = sony_integ_j83b_monitor_RFLevel(&inst->integ, &strength);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_C:
            result = sony_integ_isdbc_monitor_RFLevel(&inst->integ, &strength);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_S:
            result = sony_integ_dvbs_s2_monitor_RFLevel(&inst->integ, &strength);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_S:
            result = sony_integ_isdbs_monitor_RFLevel(&inst->integ, &strength);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_S3:
            result = sony_integ_isdbs3_monitor_RFLevel(&inst->integ, &strength);
            break;
        default:
            HI_ERR_PRINT_U32(drv_frontend_get_sig_type(port));
            return HI_ERR_FRONTEND_INVALID_SIGTYPE;
    }

    if (result == SONY_RESULT_ERROR_HW_STATE) {
        return convert_result(result);
    } else if (result != SONY_RESULT_OK) {
        LOG_ERR_PRINT("sony demod get signal strength error, result = %d\n", result);
        return convert_result(result);
    }
    signal_strength[1] = strength / 1000 + 107; /* 1000:unit; 107:dBm to dBuV */
    HI_DBG_PRINT_U32(signal_strength[1]);
    return HI_SUCCESS;
}

hi_s32 cxd2878_set_ts_out(hi_u32 port, hi_drv_demod_ts_out *ts_out)
{
    sony_driver_instance_t *inst = NULL;
    sony_result_t result;

    if (ts_out == NULL) {
        LOG_ERR_PRINT("ts_out is null pointer\n");
        return HI_ERR_FRONTEND_INVALID_POINT;
    }

    HI_DBG_PRINT_U32(ts_out->ts_mode);

    inst = get_instance(port);
    if (inst == NULL) {
        LOG_ERR_PRINT("inst is null pointer\n");
        return HI_ERR_FRONTEND_INVALID_POINT;
    }

    result = sony_set_ts_type(&inst->demod, ts_out->ts_mode);
    if (result != SONY_RESULT_OK) {
        LOG_ERR_PRINT("sony demod set TS type error, result = %d\n", result);
        return convert_result(result);
    }
    return HI_SUCCESS;
}

static hi_drv_modulation_type convert_isdbt_mod_type(sony_isdbt_modulation_t modulation)
{
    switch (modulation) {
        case SONY_ISDBT_MODULATION_DQPSK:        /**< DQPSK */
            return HI_DRV_MOD_TYPE_DQPSK;
        case SONY_ISDBT_MODULATION_QPSK:         /**< QPSK  */
            return HI_DRV_MOD_TYPE_QPSK;
        case SONY_ISDBT_MODULATION_16QAM:        /**< 16QAM */
            return HI_DRV_MOD_TYPE_QAM_16;
        case SONY_ISDBT_MODULATION_64QAM:        /**< 64QAM */
            return HI_DRV_MOD_TYPE_QAM_64;
        default:
            return HI_DRV_MOD_TYPE_MAX;
    }
}

static hi_drv_frontend_fec_rate convert_isdbt_fec_rate(sony_isdbt_coding_rate_t coding_rate)
{
    switch (coding_rate) {
        case SONY_ISDBT_CODING_RATE_1_2:         /**< Code Rate : 1/2 */
            return HI_DRV_FRONTEND_FEC_RATE_1_2;
        case SONY_ISDBT_CODING_RATE_2_3:         /**< Code Rate : 2/3 */
            return HI_DRV_FRONTEND_FEC_RATE_2_3;
        case SONY_ISDBT_CODING_RATE_3_4:         /**< Code Rate : 3/4 */
            return HI_DRV_FRONTEND_FEC_RATE_3_4;
        case SONY_ISDBT_CODING_RATE_5_6:         /**< Code Rate : 5/6 */
            return HI_DRV_FRONTEND_FEC_RATE_5_6;
        case SONY_ISDBT_CODING_RATE_7_8:         /**< Code Rate : 7/8 */
            return HI_DRV_FRONTEND_FEC_RATE_7_8;
        default:
            return HI_DRV_FRONTEND_FEC_RATE_MAX;
    }
}

static hi_drv_frontend_isdbt_time_interleaver convert_isdbt_hier_mod(sony_isdbt_il_length_t il_length)
{
    switch (il_length) {
        case SONY_ISDBT_IL_LENGTH_0_0_0:        /**< Mode1: 0, Mode2: 0, Mode3: 0 */
            return HI_DRV_FRONTEND_ISDBT_TIME_INTERLEAVER_0;
        case SONY_ISDBT_IL_LENGTH_4_2_1:        /**< Mode1: 4, Mode2: 2, Mode3: 1 */
            return HI_DRV_FRONTEND_ISDBT_TIME_INTERLEAVER_1;
        case SONY_ISDBT_IL_LENGTH_8_4_2:        /**< Mode1: 8, Mode2: 4, Mode3: 2 */
            return HI_DRV_FRONTEND_ISDBT_TIME_INTERLEAVER_2;
        case SONY_ISDBT_IL_LENGTH_16_8_4:       /**< Mode1:16, Mode2: 8, Mode3: 4 */
            return HI_DRV_FRONTEND_ISDBT_TIME_INTERLEAVER_4;
        default:
            return HI_DRV_FRONTEND_ISDBT_TIME_INTERLEAVER_MAX;
    }
}

static hi_drv_frontend_fft convert_isdbt_fft_mode(sony_isdbt_mode_t isdbt_mode)
{
    switch (isdbt_mode) {
        case SONY_ISDBT_MODE_1:       /**< ISDB-T Mode 1 (Number of carriers = 1405) */
            return HI_DRV_FRONTEND_FFT_1K;
        case SONY_ISDBT_MODE_2:       /**< ISDB-T Mode 2 (Number of carriers = 2809) */
            return HI_DRV_FRONTEND_FFT_2K;
        case SONY_ISDBT_MODE_3:       /**< ISDB-T Mode 3 (Number of carriers = 5617) */
            return HI_DRV_FRONTEND_FFT_4K;
        default:
            return HI_DRV_FRONTEND_FFT_MAX;
    }
}

static hi_drv_frontend_guard_intv convert_isdbt_guard_intv(sony_isdbt_guard_t isdbt_guard)
{
    switch (isdbt_guard) {
        case SONY_ISDBT_GUARD_1_32:    /**< Guard Interval : 1/32 */
            return HI_DRV_FRONTEND_GUARD_INTV_1_32;
        case SONY_ISDBT_GUARD_1_16:    /**< Guard Interval : 1/16 */
            return HI_DRV_FRONTEND_GUARD_INTV_1_16;
        case SONY_ISDBT_GUARD_1_8:     /**< Guard Interval : 1/8  */
            return HI_DRV_FRONTEND_GUARD_INTV_1_8;
        case SONY_ISDBT_GUARD_1_4:     /**< Guard Interval : 1/4  */
            return HI_DRV_FRONTEND_GUARD_INTV_1_4;
        default:
            return HI_DRV_FRONTEND_GUARD_INTV_MAX;
    }
}

static hi_void convert_isdbt_layer(hi_drv_frontend_isdbt_layers_info *hisi_layer,
    const sony_isdbt_tmcc_layer_info_t *sony_layer)
{
    if (sony_layer->segmentsNum > 13) { /* 13:max segments number */
        hisi_layer->layer_seg_num = 0;
    } else {
        hisi_layer->layer_seg_num = sony_layer->segmentsNum;
    }
    hisi_layer->layer_mod_type = convert_isdbt_mod_type(sony_layer->modulation);
    hisi_layer->layer_fec_rate = convert_isdbt_fec_rate(sony_layer->codingRate);
    hisi_layer->layer_time_interleaver = convert_isdbt_hier_mod(sony_layer->ilLength);
}

static hi_s32 cxd2878_get_signal_info_isdbt(sony_demod_t *demod, hi_drv_frontend_signal_info *signal_info)
{
    sony_result_t result;
    sony_isdbt_tmcc_info_t tmcc_info = {0};
    sony_isdbt_mode_t isdbt_mode;
    sony_isdbt_guard_t isdbt_guard;
    hi_drv_frontend_isdbt_tmcc_info *hisi_tmcc_info = NULL;
    hi_drv_frontend_isdbt_layers_identify *hisi_layer = NULL;

    signal_info->sig_type = HI_DRV_FRONTEND_SIG_TYPE_ISDB_T;
    result = sony_demod_isdbt_monitor_TMCCInfo(demod, &tmcc_info);
    if (result == SONY_RESULT_ERROR_HW_STATE) {
        return convert_result(result);
    } else if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_demod_SetConfig, result);
        return convert_result(result);
    }

    hisi_tmcc_info = &signal_info->signal_info.isdbt.isdbt_tmcc_info;
    signal_info->signal_info.isdbt.isdbt_tmcc_info.emergency_flag = tmcc_info.ewsFlag;
    hisi_tmcc_info->isdbt_system_id = tmcc_info.systemId;
    hisi_tmcc_info->phase_shift_corr = tmcc_info.countDownIndex;
    hisi_tmcc_info->partial_flag = tmcc_info.currentInfo.isPartial;
    convert_isdbt_layer(&hisi_tmcc_info->isdbt_layers_a_info_bits, &tmcc_info.currentInfo.layerA);
    convert_isdbt_layer(&hisi_tmcc_info->isdbt_layers_b_info_bits, &tmcc_info.currentInfo.layerB);
    convert_isdbt_layer(&hisi_tmcc_info->isdbt_layers_c_info_bits, &tmcc_info.currentInfo.layerC);

    hisi_layer = &signal_info->signal_info.isdbt.isdbt_layers;
    hisi_layer->bits.layer_a_exist = hisi_tmcc_info->isdbt_layers_a_info_bits.layer_seg_num > 0 ? 1 : 0;
    hisi_layer->bits.layer_b_exist = hisi_tmcc_info->isdbt_layers_b_info_bits.layer_seg_num > 0 ? 1 : 0;
    hisi_layer->bits.layer_c_exist = hisi_tmcc_info->isdbt_layers_c_info_bits.layer_seg_num > 0 ? 1 : 0;

    result = sony_demod_isdbt_monitor_ModeGuard(demod, &isdbt_mode, &isdbt_guard);
    if (result == SONY_RESULT_ERROR_HW_STATE) {
        return convert_result(result);
    } else if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_demod_SetConfig, result);
        return convert_result(result);
    }
    signal_info->signal_info.isdbt.fft_mode = convert_isdbt_fft_mode(isdbt_mode);
    signal_info->signal_info.isdbt.guard_intv = convert_isdbt_guard_intv(isdbt_guard);
    return HI_SUCCESS;
}

static hi_drv_modulation_type convert_dvbt_constellation(sony_dvbt_constellation_t constellation)
{
    switch (constellation) {
        case SONY_DVBT_CONSTELLATION_QPSK:            /**< QPSK */
            return HI_DRV_MOD_TYPE_QPSK;
        case SONY_DVBT_CONSTELLATION_16QAM:           /**< 16-QAM */
            return HI_DRV_MOD_TYPE_QAM_16;
        case SONY_DVBT_CONSTELLATION_64QAM:           /**< 64-QAM */
            return HI_DRV_MOD_TYPE_QAM_64;
        default:
            return HI_DRV_MOD_TYPE_MAX;
    }
}

static hi_drv_frontend_dvbt_hierarchy convert_dvbt_hierarchy(sony_dvbt_hierarchy_t hierarchy)
{
    switch (hierarchy) {
        case SONY_DVBT_HIERARCHY_NON:                 /**< Non-hierarchical service. */
            return HI_DRV_FRONTEND_DVBT_HIERARCHY_NO;
        case SONY_DVBT_HIERARCHY_1:                   /**< a = 1. */
            return HI_DRV_FRONTEND_DVBT_HIERARCHY_ALHPA1;
        case SONY_DVBT_HIERARCHY_2:                   /**< a = 2. */
            return HI_DRV_FRONTEND_DVBT_HIERARCHY_ALHPA2;
        case SONY_DVBT_HIERARCHY_4:                   /**< a = 4. */
            return HI_DRV_FRONTEND_DVBT_HIERARCHY_ALHPA4;
        default:
            return HI_DRV_FRONTEND_DVBT_HIERARCHY_MAX;
    }
}

static hi_drv_frontend_fec_rate convert_dvbt_fec_rate(sony_dvbt_coderate_t fec_rate)
{
    switch (fec_rate) {
        case SONY_DVBT_CODERATE_1_2:                  /**< Code Rate : 1/2 */
            return HI_DRV_FRONTEND_FEC_RATE_1_2;
        case SONY_DVBT_CODERATE_2_3:                  /**< Code Rate : 2/3 */
            return HI_DRV_FRONTEND_FEC_RATE_2_3;
        case SONY_DVBT_CODERATE_3_4:                  /**< Code Rate : 3/4 */
            return HI_DRV_FRONTEND_FEC_RATE_3_4;
        case SONY_DVBT_CODERATE_5_6:                  /**< Code Rate : 5/6 */
            return HI_DRV_FRONTEND_FEC_RATE_5_6;
        case SONY_DVBT_CODERATE_7_8:                  /**< Code Rate : 7/8 */
            return HI_DRV_FRONTEND_FEC_RATE_7_8;
        default:
            return HI_DRV_FRONTEND_FEC_RATE_MAX;
    }
}

static hi_drv_frontend_guard_intv convert_dvbt_guard(sony_dvbt_guard_t guard)
{
    switch (guard) {
        case SONY_DVBT_GUARD_1_32:                    /**< Guard Interval : 1/32 */
            return HI_DRV_FRONTEND_GUARD_INTV_1_32;
        case SONY_DVBT_GUARD_1_16:                    /**< Guard Interval : 1/16 */
            return HI_DRV_FRONTEND_GUARD_INTV_1_16;
        case SONY_DVBT_GUARD_1_8:                     /**< Guard Interval : 1/8  */
            return HI_DRV_FRONTEND_GUARD_INTV_1_8;
        case SONY_DVBT_GUARD_1_4:                     /**< Guard Interval : 1/4  */
            return HI_DRV_FRONTEND_GUARD_INTV_1_4;
        default:
            return HI_DRV_FRONTEND_GUARD_INTV_MAX;
    }
}

static hi_drv_frontend_fft convert_dvbt_fft_mode(sony_dvbt_mode_t fft_mode)
{
    switch (fft_mode) {
        case SONY_DVBT_MODE_2K:                       /**< 2k mode */
            return HI_DRV_FRONTEND_FFT_1K;
        case SONY_DVBT_MODE_8K:                       /**< 8k mode */
            return HI_DRV_FRONTEND_FFT_8K;
        default:
            return HI_DRV_FRONTEND_FFT_MAX;
    }
}

static hi_s32 cxd2878_get_signal_info_dvbt(sony_demod_t *demod, hi_drv_frontend_signal_info *signal_info)
{
    sony_result_t result;
    sony_dvbt_tpsinfo_t tpsinfo;

    signal_info->sig_type = HI_DRV_FRONTEND_SIG_TYPE_DVB_T;
    result = sony_demod_dvbt_monitor_TPSInfo(demod, &tpsinfo);
    if (result == SONY_RESULT_ERROR_HW_STATE) {
        return convert_result(result);
    } else if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_demod_SetConfig, result);
        return convert_result(result);
    }
    signal_info->signal_info.dvbt.mod_type = convert_dvbt_constellation(tpsinfo.constellation);
    signal_info->signal_info.dvbt.hier_mod = convert_dvbt_hierarchy(tpsinfo.hierarchy);
    signal_info->signal_info.dvbt.fec_rate = convert_dvbt_fec_rate(tpsinfo.rateHP);
    signal_info->signal_info.dvbt.low_pri_fec_rate = convert_dvbt_fec_rate(tpsinfo.rateLP);
    signal_info->signal_info.dvbt.guard_intv = convert_dvbt_guard(tpsinfo.guard);
    signal_info->signal_info.dvbt.fft_mode = convert_dvbt_fft_mode(tpsinfo.mode);
    signal_info->signal_info.dvbt.cell_id = tpsinfo.cellID;
    return HI_SUCCESS;
}

static hi_drv_frontend_fec_rate convert_dvbt2_fec_rate(sony_dvbt2_plp_code_rate_t fec_rate)
{
    switch (fec_rate) {
        case SONY_DVBT2_R1_2:                     /**< Code rate : 1/2. */
            return HI_DRV_FRONTEND_FEC_RATE_1_2;
        case SONY_DVBT2_R3_5:                     /**< Code rate : 3/5. */
            return HI_DRV_FRONTEND_FEC_RATE_3_5;
        case SONY_DVBT2_R2_3:                     /**< Code rate : 2/3. */
            return HI_DRV_FRONTEND_FEC_RATE_2_3;
        case SONY_DVBT2_R3_4:                     /**< Code rate : 3/4. */
            return HI_DRV_FRONTEND_FEC_RATE_3_4;
        case SONY_DVBT2_R4_5:                     /**< Code rate : 4/5, DVB-T2-Base only. */
            return HI_DRV_FRONTEND_FEC_RATE_4_5;
        case SONY_DVBT2_R5_6:                     /**< Code rate : 5/6, DVB-T2-Base only. */
            return HI_DRV_FRONTEND_FEC_RATE_5_6;
        case SONY_DVBT2_R1_3:                     /**< Code rate : 1/3, DVB-T2-Lite only. */
            return HI_DRV_FRONTEND_FEC_RATE_1_3;
        case SONY_DVBT2_R2_5:                     /**< Code rate : 2/5, DVB-T2-Lite only. */
            return HI_DRV_FRONTEND_FEC_RATE_2_5;
        default:
            return HI_DRV_FRONTEND_FEC_RATE_MAX;
    }
}

static hi_drv_modulation_type convert_dvbt2_mod_type(sony_dvbt2_plp_constell_t mod_type)
{
    switch (mod_type) {
        case SONY_DVBT2_QPSK:                     /**< QPSK. */
            return HI_DRV_MOD_TYPE_QPSK;
        case SONY_DVBT2_QAM16:                    /**< QAM16. */
            return HI_DRV_MOD_TYPE_QAM_16;
        case SONY_DVBT2_QAM64:                    /**< QAM64. */
            return HI_DRV_MOD_TYPE_QAM_64;
        case SONY_DVBT2_QAM256:                   /**< QAM256. */
            return HI_DRV_MOD_TYPE_QAM_256;
        default:
            return HI_DRV_MOD_TYPE_MAX;
    }
}

static hi_drv_frontend_dvbt2_carrier convert_dvbt2_carrier_mode(hi_u8 bw_ext)
{
    return bw_ext ? HI_DRV_FRONTEND_DVBT2_CARRIER_EXTEND : HI_DRV_FRONTEND_DVBT2_CARRIER_NORMAL;
}

static hi_drv_frontend_fft convert_dvbt2_fft_mode(sony_dvbt2_mode_t fft_mode)
{
    switch (fft_mode) {
        case SONY_DVBT2_M2K:                      /**< 2K mode. */
            return HI_DRV_FRONTEND_FFT_2K;
        case SONY_DVBT2_M8K:                      /**< 8K mode. */
            return HI_DRV_FRONTEND_FFT_8K;
        case SONY_DVBT2_M4K:                      /**< 4K mode. */
            return HI_DRV_FRONTEND_FFT_4K;
        case SONY_DVBT2_M1K:                      /**< 1K mode. */
            return HI_DRV_FRONTEND_FFT_1K;
        case SONY_DVBT2_M16K:                     /**< 16K mode. */
            return HI_DRV_FRONTEND_FFT_16K;
        case SONY_DVBT2_M32K:                     /**< 32K mode. */
            return HI_DRV_FRONTEND_FFT_32K;
        default:
            return HI_DRV_FRONTEND_FFT_MAX;
    }
}

static hi_drv_frontend_guard_intv convert_dvbt2_guard(sony_dvbt2_guard_t guard)
{
    switch (guard) {
        case SONY_DVBT2_G1_32:                    /**< Guard Interval : 1/32. */
            return HI_DRV_FRONTEND_GUARD_INTV_1_32;
        case SONY_DVBT2_G1_16:                    /**< Guard Interval : 1/16. */
            return HI_DRV_FRONTEND_GUARD_INTV_1_16;
        case SONY_DVBT2_G1_8:                     /**< Guard Interval : 1/8. */
            return HI_DRV_FRONTEND_GUARD_INTV_1_8;
        case SONY_DVBT2_G1_4:                     /**< Guard Interval : 1/4. */
            return HI_DRV_FRONTEND_GUARD_INTV_1_4;
        case SONY_DVBT2_G1_128:                   /**< Guard Interval : 1/128. */
            return HI_DRV_FRONTEND_GUARD_INTV_1_128;
        case SONY_DVBT2_G19_128:                  /**< Guard Interval : 19/128. */
            return HI_DRV_FRONTEND_GUARD_INTV_19_128;
        case SONY_DVBT2_G19_256:                  /**< Guard Interval : 19/256. */
            return HI_DRV_FRONTEND_GUARD_INTV_19_256;
        default:
            return HI_DRV_FRONTEND_GUARD_INTV_MAX;
    }
}

static hi_drv_frontend_dvbt2_pilot_pattern convert_dvbt2_pp(sony_dvbt2_pp_t pp)
{
    switch (pp) {
        case SONY_DVBT2_PP1:                      /**< DVBT2 pilot pattern 1. */
            return HI_DRV_FRONTEND_DVBT2_PILOT_PATTERN_PP1;
        case SONY_DVBT2_PP2:                      /**< DVBT2 pilot pattern 2. */
            return HI_DRV_FRONTEND_DVBT2_PILOT_PATTERN_PP2;
        case SONY_DVBT2_PP3:                      /**< DVBT2 pilot pattern 3. */
            return HI_DRV_FRONTEND_DVBT2_PILOT_PATTERN_PP3;
        case SONY_DVBT2_PP4:                      /**< DVBT2 pilot pattern 4. */
            return HI_DRV_FRONTEND_DVBT2_PILOT_PATTERN_PP4;
        case SONY_DVBT2_PP5:                      /**< DVBT2 pilot pattern 5. */
            return HI_DRV_FRONTEND_DVBT2_PILOT_PATTERN_PP5;
        case SONY_DVBT2_PP6:                      /**< DVBT2 pilot pattern 6. */
            return HI_DRV_FRONTEND_DVBT2_PILOT_PATTERN_PP6;
        case SONY_DVBT2_PP7:                      /**< DVBT2 pilot pattern 7. */
            return HI_DRV_FRONTEND_DVBT2_PILOT_PATTERN_PP7;
        case SONY_DVBT2_PP8:                      /**< DVBT2 pilot pattern 8. */
            return HI_DRV_FRONTEND_DVBT2_PILOT_PATTERN_PP8;
        default:
            return HI_DRV_FRONTEND_DVBT2_PILOT_PATTERN_MAX;
    }
}

static hi_drv_frontend_dvbt2_mode convert_dvbt2_profile(sony_dvbt2_profile_t profile)
{
    switch (profile) {
        case SONY_DVBT2_PROFILE_BASE:    /**< Acquire to DVB-T2 base profile */
            return HI_DRV_FRONTEND_DVBT2_MODE_BASE;
        case SONY_DVBT2_PROFILE_LITE:    /**< Acquire to DVB-T2 lite profile */
            return HI_DRV_FRONTEND_DVBT2_MODE_LITE;
        default:     /**< Acquire to DVB-T2 base or lite profile (blind tune only) */
            return HI_DRV_FRONTEND_DVBT2_MODE_MAX;
    }
}

static hi_drv_frontend_dvbt2_stream_type convert_dvbt2_stream_type(sony_dvbt2_plp_payload_t payload,
    sony_dvbt2_plp_mode_t plp_mode)
{
    switch (payload) {
        case SONY_DVBT2_PLP_PAYLOAD_GFPS:         /**< DVBT2 GFPS payload type. */
            return HI_DRV_FRONTEND_DVBT2_STREAM_TYPE_GFPS;
        case SONY_DVBT2_PLP_PAYLOAD_GCS:          /**< DVBT2 GCS payload type. */
            return HI_DRV_FRONTEND_DVBT2_STREAM_TYPE_GCS;
        case SONY_DVBT2_PLP_PAYLOAD_GSE:          /**< DVBT2 GSE payload type. */
            return (plp_mode == SONY_DVBT2_PLP_MODE_HEM) ?
                HI_DRV_FRONTEND_DVBT2_STREAM_TYPE_GSE_HEM : HI_DRV_FRONTEND_DVBT2_STREAM_TYPE_GSE;
        case SONY_DVBT2_PLP_PAYLOAD_TS:           /**< DVBT2 TS payload type. */
            return (plp_mode == SONY_DVBT2_PLP_MODE_HEM) ?
                HI_DRV_FRONTEND_DVBT2_STREAM_TYPE_TS_HEM : HI_DRV_FRONTEND_DVBT2_STREAM_TYPE_TS;
        default:
            return HI_DRV_FRONTEND_DVBT2_STREAM_TYPE_MAX;
    }
}

static hi_s32 cxd2878_get_signal_info_dvbt2(sony_demod_t *demod, hi_drv_frontend_signal_info *signal_info)
{
    sony_result_t result;
    sony_dvbt2_l1pre_t l1pre;
    sony_dvbt2_profile_t profile;
    sony_dvbt2_plp_t plp_info;

    signal_info->sig_type = HI_DRV_FRONTEND_SIG_TYPE_DVB_T2;
    result = sony_demod_dvbt2_monitor_L1Pre(demod, &l1pre);
    if (result == SONY_RESULT_ERROR_HW_STATE) {
        return convert_result(result);
    } else if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_demod_SetConfig, result);
        return convert_result(result);
    }
    signal_info->signal_info.dvbt2.carrier_mode = convert_dvbt2_carrier_mode(l1pre.bwExt);
    signal_info->signal_info.dvbt2.fft_mode = convert_dvbt2_fft_mode(l1pre.fftMode);
    signal_info->signal_info.dvbt2.guard_intv = convert_dvbt2_guard(l1pre.gi);
    signal_info->signal_info.dvbt2.pilot_pattern = convert_dvbt2_pp(l1pre.pp);
    signal_info->signal_info.dvbt2.cell_id = l1pre.cellId;
    signal_info->signal_info.dvbt2.network_id = l1pre.networkId;
    signal_info->signal_info.dvbt2.system_id = l1pre.systemId;

    result = sony_demod_dvbt2_monitor_Profile(demod, &profile);
    if (result == SONY_RESULT_ERROR_HW_STATE) {
        return convert_result(result);
    } else if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_demod_SetConfig, result);
        return convert_result(result);
    }
    signal_info->signal_info.dvbt2.channel_mode = convert_dvbt2_profile(profile);

    result = sony_demod_dvbt2_monitor_ActivePLP(demod, SONY_DVBT2_PLP_DATA, &plp_info);
    if (result == SONY_RESULT_ERROR_HW_STATE) {
        return convert_result(result);
    } else if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_demod_SetConfig, result);
        return convert_result(result);
    }
    signal_info->signal_info.dvbt2.fec_rate = convert_dvbt2_fec_rate(plp_info.plpCr);
    signal_info->signal_info.dvbt2.mod_type = convert_dvbt2_mod_type(plp_info.constell);
    signal_info->signal_info.dvbt2.stream_type = convert_dvbt2_stream_type(plp_info.payload, plp_info.plpMode);
    return HI_SUCCESS;
}

static hi_drv_frontend_fec_rate convert_dvbs_fec_rate(sony_dvbs_coderate_t fec_rate)
{
    switch (fec_rate) {
        case SONY_DVBS_CODERATE_1_2:             /**< 1/2 */
            return HI_DRV_FRONTEND_FEC_RATE_1_2;
        case SONY_DVBS_CODERATE_2_3:             /**< 2/3 */
            return HI_DRV_FRONTEND_FEC_RATE_2_3;
        case SONY_DVBS_CODERATE_3_4:             /**< 3/4 */
            return HI_DRV_FRONTEND_FEC_RATE_3_4;
        case SONY_DVBS_CODERATE_5_6:             /**< 5/6 */
            return HI_DRV_FRONTEND_FEC_RATE_5_6;
        case SONY_DVBS_CODERATE_7_8:             /**< 7/8 */
            return HI_DRV_FRONTEND_FEC_RATE_7_8;
        default:
            return HI_DRV_FRONTEND_FEC_RATE_MAX;
    }
}

static hi_drv_frontend_fec_rate convert_dvbs2_fec_rate(sony_dvbs2_coderate_t fec_rate)
{
    switch (fec_rate) {
        case SONY_DVBS2_CODERATE_1_4:                /**< 1/4 */
            return HI_DRV_FRONTEND_FEC_RATE_1_4;
        case SONY_DVBS2_CODERATE_1_3:                /**< 1/3 */
            return HI_DRV_FRONTEND_FEC_RATE_1_3;
        case SONY_DVBS2_CODERATE_2_5:                /**< 2/5 */
            return HI_DRV_FRONTEND_FEC_RATE_2_5;
        case SONY_DVBS2_CODERATE_1_2:                /**< 1/2 */
            return HI_DRV_FRONTEND_FEC_RATE_1_2;
        case SONY_DVBS2_CODERATE_3_5:                /**< 3/5 */
            return HI_DRV_FRONTEND_FEC_RATE_3_5;
        case SONY_DVBS2_CODERATE_2_3:                /**< 2/3 */
            return HI_DRV_FRONTEND_FEC_RATE_2_3;
        case SONY_DVBS2_CODERATE_3_4:                /**< 3/4 */
            return HI_DRV_FRONTEND_FEC_RATE_3_4;
        case SONY_DVBS2_CODERATE_4_5:                /**< 4/5 */
            return HI_DRV_FRONTEND_FEC_RATE_4_5;
        case SONY_DVBS2_CODERATE_5_6:                /**< 5/6 */
            return HI_DRV_FRONTEND_FEC_RATE_5_6;
        case SONY_DVBS2_CODERATE_8_9:                /**< 8/9 */
            return HI_DRV_FRONTEND_FEC_RATE_8_9;
        case SONY_DVBS2_CODERATE_9_10:               /**< 9/10 */
            return HI_DRV_FRONTEND_FEC_RATE_9_10;
        default:
            return HI_DRV_FRONTEND_FEC_RATE_MAX;
    }
}

static hi_drv_modulation_type convert_dvbs2_mod_type(sony_dvbs2_modulation_t mod_type)
{
    switch (mod_type) {
        case SONY_DVBT2_QPSK:                     /**< QPSK. */
            return HI_DRV_MOD_TYPE_QPSK;
        case SONY_DVBT2_QAM16:                    /**< QAM16. */
            return HI_DRV_MOD_TYPE_QAM_16;
        case SONY_DVBT2_QAM64:                    /**< QAM64. */
            return HI_DRV_MOD_TYPE_QAM_64;
        case SONY_DVBT2_QAM256:                   /**< QAM256. */
            return HI_DRV_MOD_TYPE_QAM_256;
        default:
            return HI_DRV_MOD_TYPE_MAX;
    }
}

static hi_drv_frontend_roll_off convert_dvbs2_roll_off(sony_dvbs2_rolloff_t roll_off)
{
    switch (roll_off) {
        case SONY_DVBS2_ROLLOFF_35:           /**< Roll-off = 0.35. */
            return HI_DRV_FRONTEND_ROLL_OFF_35;
        case SONY_DVBS2_ROLLOFF_25:           /**< Roll-off = 0.25. */
            return HI_DRV_FRONTEND_ROLL_OFF_25;
        case SONY_DVBS2_ROLLOFF_20:           /**< Roll-off = 0.20. */
            return HI_DRV_FRONTEND_ROLL_OFF_20;
        default:
            return HI_DRV_FRONTEND_ROLL_OFF_MAX;
    }
}

static hi_drv_frontend_sat_stream_type convert_dvbs2_stream(sony_dvbs2_stream_t stream)
{
    switch (stream) {
        case SONY_DVBS2_STREAM_GENERIC_PACKETIZED:   /**< Generic Packetized Stream Input. */
            return HI_DRV_FRONTEND_SAT_STREAM_TYPE_GENERIC_PACKETIZED;
        case SONY_DVBS2_STREAM_GENERIC_CONTINUOUS:   /**< Generic Continuous Stream Input. */
            return HI_DRV_FRONTEND_SAT_STREAM_TYPE_GENERIC_CONTINUOUS;
        case SONY_DVBS2_STREAM_RESERVED:             /**< Reserved. */
            return HI_DRV_FRONTEND_SAT_STREAM_TYPE_GSE_HEM;
        case SONY_DVBS2_STREAM_TRANSPORT:             /**< Transport Stream Input. */
            return HI_DRV_FRONTEND_SAT_STREAM_TYPE_TRANSPORT;
        default:
            return HI_DRV_FRONTEND_SAT_STREAM_TYPE_MAX;
    }
}

static hi_s32 cxd2878_get_signal_info_dvbs2(sony_demod_t *demod, hi_drv_frontend_signal_info *signal_info)
{
    sony_result_t result;
    sony_dvbs2_plscode_t pls_code;
    sony_dvbs2_bbheader_t bbheader;

    result = sony_demod_dvbs2_monitor_PLSCode(demod, &pls_code);
    if (result == SONY_RESULT_ERROR_HW_STATE) {
        return convert_result(result);
    } else if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_demod_SetConfig, result);
        return convert_result(result);
    }
    signal_info->signal_info.dvbs2x.fec_rate = convert_dvbs2_fec_rate(pls_code.codeRate);
    signal_info->signal_info.dvbs2x.mod_type = convert_dvbs2_mod_type(pls_code.modulation);
    signal_info->signal_info.dvbs2x.fec_frame_mode =
        pls_code.isShortFrame ? HI_DRV_FRONTEND_SAT_FEC_FRAME_SHORT : HI_DRV_FRONTEND_SAT_FEC_FRAME_NORMAL;
    signal_info->signal_info.dvbs2x.pilot = pls_code.isPilotOn ? HI_DRV_FRONTEND_PILOT_ON : HI_DRV_FRONTEND_PILOT_OFF;

    result = sony_demod_dvbs2_monitor_BBHeader(demod, &bbheader);
    if (result == SONY_RESULT_ERROR_HW_STATE) {
        return convert_result(result);
    } else if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_demod_SetConfig, result);
        return convert_result(result);
    }
    signal_info->signal_info.dvbs2x.roll_off = convert_dvbs2_roll_off(bbheader.rollOff);
    signal_info->signal_info.dvbs2x.stream_type = convert_dvbs2_stream(bbheader.streamInput);
    if (bbheader.isConstantCodingModulation) {
        if (bbheader.isSingleInputStream) {
            signal_info->signal_info.dvbs2x.code_modulation = HI_DRV_FRONTEND_CODE_MODULATION_CCM;
        } else {
            signal_info->signal_info.dvbs2x.code_modulation = HI_DRV_FRONTEND_CODE_MODULATION_MULTISTREAM;
        }
    } else {
        signal_info->signal_info.dvbs2x.code_modulation = HI_DRV_FRONTEND_CODE_MODULATION_VCM_ACM;
    }

    return HI_SUCCESS;
}
static hi_s32 cxd2878_get_signal_info_dvbs_s2(sony_demod_t *demod, hi_drv_frontend_signal_info *signal_info)
{
    hi_s32 ret = HI_SUCCESS;
    sony_result_t result;
    sony_dtv_system_t dtv_system;
    sony_dvbs_coderate_t dvbs_cr;

    signal_info->sig_type = HI_DRV_FRONTEND_SIG_TYPE_DVB_S;
    result = sony_demod_dvbs_s2_monitor_System(demod, &dtv_system);
    if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_demod_dvbs_s2_monitor_System, result);
        return result;
    }
    if (dtv_system == SONY_DTV_SYSTEM_DVBS) {
        signal_info->sig_type = HI_DRV_FRONTEND_SIG_TYPE_DVB_S;
        result = sony_demod_dvbs_monitor_CodeRate(demod, &dvbs_cr);
        if (result == SONY_RESULT_ERROR_HW_STATE) {
            return convert_result(result);
        } else if (result != SONY_RESULT_OK) {
            HI_ERR_PRINT_FUNC_RES(sony_demod_SetConfig, result);
            return convert_result(result);
        }
        signal_info->signal_info.dvbs2x.fec_rate = convert_dvbs_fec_rate(dvbs_cr);
        signal_info->signal_info.dvbs2x.mod_type = HI_DRV_MOD_TYPE_QPSK;
    } else {
        signal_info->sig_type = HI_DRV_FRONTEND_SIG_TYPE_DVB_S2;
        ret = cxd2878_get_signal_info_dvbs2(demod, signal_info);
        if (ret != HI_SUCCESS && ret != HI_ERR_FRONTEND_NOT_CONNECT) {
            HI_ERR_PRINT_FUNC_RES(cxd2878_get_signal_info_dvbs2, ret);
        }
    }
    return ret;
}

hi_s32 cxd2878_get_signal_info(hi_u32 port, hi_drv_frontend_signal_info *signal_info)
{
    hi_s32 ret = HI_SUCCESS;
    sony_driver_instance_t *inst = NULL;
    hi_drv_frontend_sig_type sig_type;

    inst = get_instance(port);
    drv_fe_check_pointer(inst);

    sig_type = drv_frontend_get_sig_type(port);
    switch (sig_type) {
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_C:
        case HI_DRV_FRONTEND_SIG_TYPE_J83B:
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_C:
            signal_info->sig_type = sig_type;
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_T:
            ret = cxd2878_get_signal_info_isdbt(&inst->demod, signal_info);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T:
            ret = cxd2878_get_signal_info_dvbt(&inst->demod, signal_info);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T2:
            ret = cxd2878_get_signal_info_dvbt2(&inst->demod, signal_info);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_S:
            ret = cxd2878_get_signal_info_dvbs_s2(&inst->demod, signal_info);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_S:
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_S3:
            signal_info->sig_type = sig_type;
            break;
        default:
            HI_ERR_PRINT_U32(sig_type);
            return HI_ERR_FRONTEND_INVALID_SIGTYPE;
    }
    return ret;
}

static hi_s32 cxd2878_get_freq_offset(sony_demod_t *demod, hi_drv_frontend_sig_type sig_type, hi_s32 *freq_offset)
{
    sony_result_t result = SONY_RESULT_OK;
    switch (sig_type) {
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_C:
            result = sony_demod_dvbc_monitor_CarrierOffset(demod, freq_offset);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_S:
            result = sony_demod_dvbs_s2_monitor_CarrierOffset(demod, freq_offset);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T:
            result = sony_demod_dvbt_monitor_CarrierOffset(demod, freq_offset);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T2:
            result = sony_demod_dvbt2_monitor_CarrierOffset(demod, freq_offset);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_T:
            result = sony_demod_isdbt_monitor_CarrierOffset(demod, freq_offset);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_J83B:
            result = sony_demod_j83b_monitor_CarrierOffset(demod, freq_offset);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_C:
            result = sony_demod_isdbc_monitor_CarrierOffset(demod, freq_offset);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_S:
            result = sony_demod_isdbs_monitor_CarrierOffset(demod, freq_offset);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_S3:
            result = sony_demod_isdbs3_monitor_CarrierOffset(demod, freq_offset);
            break;
        default:
            LOG_INFO_PRINT("Not support get symbol rate\n");
            break;
    }
    if (result != SONY_RESULT_OK && result != SONY_RESULT_ERROR_HW_STATE) {
        LOG_ERR_PRINT("sony demod get symbol rate error, result = %d\n", result);
    }
    return convert_result(result);
}

hi_s32 cxd2878_get_freq_symb_offset(hi_u32 port, hi_s32 *freq_offset, hi_u32 *symb)
{
    hi_s32 ret;
    sony_result_t result = SONY_RESULT_OK;
    sony_driver_instance_t *inst = NULL;
    hi_drv_frontend_sig_type sig_type;

    drv_fe_check_pointer(symb);
    drv_fe_check_pointer(freq_offset);

    inst = get_instance(port);
    drv_fe_check_pointer(inst);

    sig_type = drv_frontend_get_sig_type(port);
    switch (sig_type) {
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_C:
            result = sony_demod_dvbc_monitor_SymbolRate(&inst->demod, symb);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_S:
            result = sony_demod_dvbs_s2_monitor_SymbolRate(&inst->demod, symb);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_J83B:
            result = sony_demod_j83b_monitor_SymbolRate(&inst->demod, symb);
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_C:
            result = sony_demod_isdbc_monitor_SymbolRate(&inst->demod, symb);
            break;
        default:
            LOG_INFO_PRINT("Not support get symbol rate\n");
            break;
    }
    if (result != SONY_RESULT_OK && result != SONY_RESULT_ERROR_HW_STATE) {
        LOG_ERR_PRINT("sony demod get symbol rate error, result = %d\n", result);
    }
    ret = cxd2878_get_freq_offset(&inst->demod, sig_type, freq_offset);
    if (ret != HI_SUCCESS && ret != HI_ERR_FRONTEND_NOT_CONNECT) {
        HI_ERR_PRINT_FUNC_RES(cxd2878_get_freq_offset, ret);
    }
    return ret;
}

hi_s32 cxd2878_get_plp_num(hi_u32 port, hi_u8 *plp_num)
{
    sony_result_t result;
    sony_driver_instance_t *inst = NULL;
    hi_drv_frontend_sig_type sig_type = drv_frontend_get_sig_type(port);

    inst = get_instance(port);
    drv_fe_check_pointer(inst);

    if (sig_type == HI_DRV_FRONTEND_SIG_TYPE_DVB_T) {
        *plp_num = 0;
        return HI_SUCCESS;
    }

    if (sig_type != HI_DRV_FRONTEND_SIG_TYPE_DVB_T2) {
        HI_ERR_PRINT_U32(sig_type);
        return HI_ERR_FRONTEND_INVALID_SIGTYPE;
    }

    /* get the number of PLPs */
    result = sony_demod_dvbt2_monitor_DataPLPs(&inst->demod, NULL, plp_num);
    if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_demod_dvbt2_monitor_DataPLPs, result);
        return convert_result(result);
    }

    return HI_SUCCESS;
}

hi_s32 cxd2878_get_plp_info(hi_u32 port, hi_u32 index, hi_drv_frontend_dvbt2_plp_info *plp_info)
{
    uint8_t plp_ids[256] = {0}; /* 256:max plp number */
    uint8_t plp_num = 0;
    sony_result_t result;
    sony_driver_instance_t *inst = NULL;
    hi_drv_frontend_sig_type sig_type = drv_frontend_get_sig_type(port);

    drv_fe_check_pointer(plp_info);

    inst = get_instance(port);
    drv_fe_check_pointer(inst);

    if (sig_type != HI_DRV_FRONTEND_SIG_TYPE_DVB_T2) {
        HI_ERR_PRINT_U32(sig_type);
        return HI_ERR_FRONTEND_INVALID_SIGTYPE;
    }

    /* get the number of PLPs */
    result = sony_demod_dvbt2_monitor_DataPLPs(&inst->demod, plp_ids, &plp_num);
    if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_demod_dvbt2_monitor_DataPLPs, result);
        return convert_result(result);
    }

    plp_info->plp_id = plp_ids[index];
    plp_info->plp_grp_id = 0;
    plp_info->plp_type = HI_DRV_FRONTEND_DVBT2_PLP_TYPE_DAT1;
    return HI_SUCCESS;
}

hi_s32 cxd2878_set_plp_para(hi_u32 port, hi_drv_frontend_dvbt2_plp_para *plp_para)
{
    hi_s32 ret;
    sony_driver_instance_t *inst = NULL;
    drv_fe_ctrl_ctx *frontend_context = NULL;
    frontend_acc_qam_params *channel = NULL;
    hi_drv_frontend_sig_type sig_type = drv_frontend_get_sig_type(port);

    drv_fe_check_pointer(plp_para);

    frontend_context = drv_frontend_get_context(port);
    drv_fe_check_pointer(frontend_context);

    inst = (sony_driver_instance_t*)frontend_context->demod_private_para;
    drv_fe_check_pointer(inst);

    if (sig_type != HI_DRV_FRONTEND_SIG_TYPE_DVB_T2) {
        HI_ERR_PRINT_U32(sig_type);
        return HI_ERR_FRONTEND_INVALID_SIGTYPE;
    }

    channel = &frontend_context->connect_attr;
    channel->ter.dvbt2.plp_id = plp_para->plp_id;
    ret = cxd2878_connect_dvbt2(&inst->integ, channel);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(cxd2878_connect_dvbt2, ret);
        return ret;
    }
    return HI_SUCCESS;
}

hi_s32 cxd2878_deinit(hi_u32 port)
{
    sony_result_t result;
    sony_driver_instance_t *inst = NULL;
    drv_fe_ctrl_ctx *frontend_context = NULL;

    frontend_context = drv_frontend_get_context(port);
    drv_fe_check_pointer(frontend_context);

    inst = (sony_driver_instance_t*)frontend_context->demod_private_para;
    drv_fe_check_pointer(inst);

    result = sony_integ_Shutdown(&inst->integ);
    if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_integ_Shutdown, result);
    }

    HI_KFREE(HI_ID_TUNER, frontend_context->demod_private_para);
    frontend_context->demod_private_para = NULL;
    inst = NULL;
    return HI_SUCCESS;
}

hi_s32 cxd2878_send_continuous22k(hi_u32 port, hi_u32 continuous22k)
{
    sony_result_t result;
    sony_driver_instance_t *inst = NULL;

    HI_DBG_PRINT_U32(continuous22k);

    inst = get_instance(port);
    drv_fe_check_pointer(inst);

    result = sony_demod_sat_device_ctrl_OutputTone(&inst->demod, continuous22k);
    if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_demod_sat_device_ctrl_OutputTone, result);
        return convert_result(result);
    }
    return HI_SUCCESS;
}

hi_s32 cxd2878_i2c_bypass(hi_u32 port, hi_bool bypass)
{
    sony_result_t result;
    sony_driver_instance_t *inst = NULL;

    inst = get_instance(port);
    drv_fe_check_pointer(inst);

    result = sony_demod_I2cRepeaterEnable(&inst->demod, bypass);
    if (result != SONY_RESULT_OK) {
        HI_ERR_PRINT_FUNC_RES(sony_demod_I2cRepeaterEnable, result);
        return convert_result(result);
    }
    return HI_SUCCESS;
}

hi_bool cxd2878_is_ctrl_tuner(hi_u32 fe_port)
{
    return HI_TRUE;
}


#ifdef SONY_TRACE_ENABLE
void sony_trace_log_return(sony_result_t result, const char* filename, unsigned int linenum)
{
    HI_PRINT("%s[%d]:return %d\n", filename, linenum, result);
}

void sony_trace_log_enter(const char* funcname, const char* filename, unsigned int linenum)
{
    HI_PRINT("%s->%s[%d]:entry\n", funcname, filename, linenum);
}
#endif

hi_s32 drv_fe_adp_demod_cxd2878_regist_func(drv_fe_demod_ops *demod_ops)
{
    drv_fe_check_pointer(demod_ops);

    /* COMMON */
    demod_ops->init = cxd2878_init;
    demod_ops->connect = cxd2878_connect;
    demod_ops->deinit = cxd2878_deinit;
    demod_ops->get_status = cxd2878_get_status;
    demod_ops->get_ber = cxd2878_get_ber;
    demod_ops->get_per = cxd2878_get_per;
    demod_ops->get_snr = cxd2878_get_snr;
    demod_ops->get_signal_strength = cxd2878_get_signal_strength;
    demod_ops->get_signal_info = cxd2878_get_signal_info;
    demod_ops->get_freq_symb_offset = cxd2878_get_freq_symb_offset;
    demod_ops->standby = NULL;
    demod_ops->set_ts_out = cxd2878_set_ts_out;
    demod_ops->is_ctrl_tuner = cxd2878_is_ctrl_tuner;

    demod_ops->set_sat_attr = NULL;
    demod_ops->send_continuous22k = cxd2878_send_continuous22k;
    demod_ops->blindscan_action = NULL;
    demod_ops->blindscan_init = NULL;
    demod_ops->tp_verify = NULL;
    demod_ops->send_tone = NULL;
    demod_ops->get_stream_num = NULL;
    demod_ops->get_isi_id = NULL;
    demod_ops->set_isi_id = NULL;

    demod_ops->get_plp_num = cxd2878_get_plp_num;
    demod_ops->get_plp_info = cxd2878_get_plp_info;
    demod_ops->set_plp_para = cxd2878_set_plp_para;
    demod_ops->connect_timeout = NULL;
    demod_ops->i2c_bypass = cxd2878_i2c_bypass;

    /* only use to proc cmd */
    demod_ops->get_registers = NULL;
    demod_ops->i2c_read_byte = NULL;
    demod_ops->i2c_write_byte = NULL;

    return HI_SUCCESS;
}

