/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: SUT-PJ987 driver
 * Author: SDK
 * Create: 2019-9-18
 */
#include "sut_pj987.h"
#include "sut_pj987_isdbt_v1000.h"
#include "sut_pj987_isdbs_v1000.h"
#include "hi_debug.h"

#if defined(DEMOD_DEV_TYPE_CXD2878)

static sony_result_t sony_tuner_sut_pj987_initialize (sony_tuner_t *ptuner)
{
    uint16_t tuner_block_id;
    if (ptuner == NULL || ptuner->user == NULL) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }
    tuner_block_id = *(hi_u32*)ptuner->user;
    isdbt_x_pon(tuner_block_id);
    isdbs_x_pon(tuner_block_id);
    ptuner->rfLevelFuncTerr = SONY_TUNER_RFLEVEL_FUNC_READ;
    ptuner->rfLevelFuncSat = SONY_TUNER_RFLEVEL_FUNC_READ;
    return SONY_RESULT_OK;
}

static sony_result_t sony_tuner_sut_pj987_isdbt_tune (sony_tuner_t *ptuner,
    uint32_t frequency_khz, sony_dtv_system_t system, sony_dtv_bandwidth_t bandwidth)
{
    uint16_t tuner_block_id;

    if (ptuner == NULL || ptuner->user == NULL) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    tuner_block_id = *(hi_u32*)ptuner->user;

    HI_DBG_PRINT_U32(tuner_block_id);
    HI_DBG_PRINT_U32(frequency_khz);

    isdbt_x_tune(tuner_block_id, frequency_khz);
    /* Allow the tuner time to settle */
    SONY_SLEEP(50); /* 50:delay */
    isdbt_x_tune_end(tuner_block_id);
    ptuner->system = system;
    return SONY_RESULT_OK;
}

static sony_result_t sony_tuner_sut_pj987_isdbs_tune (sony_tuner_t *ptuner,
    uint32_t frequency_khz, sony_dtv_system_t system, uint32_t symbolRateKSps)
{
    uint16_t tuner_block_id;

    if (ptuner == NULL || ptuner->user == NULL) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }
    tuner_block_id = *(hi_u32*)ptuner->user;

    HI_DBG_PRINT_U32(tuner_block_id);
    HI_DBG_PRINT_U32(frequency_khz);
    HI_DBG_PRINT_U32(system);

    if (system == SONY_DTV_SYSTEM_ISDBS) {
        isdbs_x_tune(tuner_block_id, frequency_khz, SONY_SAT_SYSTEM_ISDBS);
    } else if (system == SONY_DTV_SYSTEM_ISDBS3) {
        isdbs_x_tune(tuner_block_id, frequency_khz, SONY_SAT_SYSTEM_ISDBS3);
    } else {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }
    ptuner->system = system;
    return SONY_RESULT_OK;
}

static sony_result_t sony_tuner_sut_pj987_sleep (sony_tuner_t *ptuner)
{
    uint16_t tuner_block_id;

    if (ptuner == NULL || ptuner->user == NULL) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }
    HI_DBG_PRINT_U32(ptuner->system);
    tuner_block_id = *(hi_u32*)ptuner->user;
    if (ptuner->system == SONY_DTV_SYSTEM_ISDBT) {
        isdbt_x_fin(tuner_block_id);
    } else {
        isdbs_x_fin(tuner_block_id);
    }
    return SONY_RESULT_OK;
}

static sony_result_t sony_tuner_sut_pj987_read_rf_level (sony_tuner_t *ptuner, int32_t *rf_level)
{
    uint16_t tuner_block_id;
    uint8_t icoffset = 0;
    uint8_t ifagcreg = 0;
    uint8_t rfagcreg = 0;
    int16_t rssi;

    if (!ptuner || !ptuner->user || !rf_level) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }
    HI_DBG_PRINT_U32(ptuner->system);
    tuner_block_id = *(hi_u32*)ptuner->user;
    if (ptuner->system == SONY_DTV_SYSTEM_ISDBT) {
        isdbt_x_read_agc(tuner_block_id, &icoffset, &ifagcreg, &rfagcreg);
        rssi = isdbt_tuner_calcRSSI(tuner_block_id, icoffset, ifagcreg, rfagcreg);
        rssi -= 450; /* 450:-4.5dBm */
    } else {
        isdbs_x_read_agc(tuner_block_id, &icoffset, &ifagcreg, &rfagcreg);
        rssi = isdbs_tuner_calcRSSI(tuner_block_id, icoffset, ifagcreg, rfagcreg);
    }
    *rf_level = rssi;
    HI_DBG_PRINT_S32(rssi);
    return SONY_RESULT_OK;
}

hi_s32 sony_tuner_sut_pj987_create (sony_tuner_t *tuner, hi_u32 *port)
{
    if (tuner != NULL) {
        /* Create local copy of instance data. */
        tuner->Initialize = sony_tuner_sut_pj987_initialize;
        tuner->TerrCableTune = sony_tuner_sut_pj987_isdbt_tune;
        tuner->SatTune = sony_tuner_sut_pj987_isdbs_tune;
        tuner->Sleep = sony_tuner_sut_pj987_sleep;
        tuner->Shutdown = sony_tuner_sut_pj987_sleep;
        tuner->ReadRFLevel = sony_tuner_sut_pj987_read_rf_level;
        tuner->CalcRFLevelFromAGC = NULL;
        tuner->system = SONY_DTV_SYSTEM_ISDBT;
        tuner->bandwidth = SONY_DTV_BW_6_MHZ;
        tuner->frequencyKHz = 0;
        tuner->user = port;
    }
    return SONY_RESULT_OK;
}

#endif

