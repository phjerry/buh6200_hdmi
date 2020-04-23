/*------------------------------------------------------------------------------
  Copyright 2017-2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/07/05
  Modification ID : c7c68d70868a4a9fb9e0f480d8b940e8a1e8f651
------------------------------------------------------------------------------*/
#include "sony_tuner_helene2.h"
#include "sony_stdlib.h"

/*------------------------------------------------------------------------------
 Driver Version
------------------------------------------------------------------------------*/
const char* sony_tuner_helene2_version =  SONY_HELENE2_VERSION;

/*------------------------------------------------------------------------------
 Static Prototypes
------------------------------------------------------------------------------*/
static sony_result_t sony_tuner_helene2_Initialize (sony_tuner_t * pTuner);

static sony_result_t sony_tuner_helene2_terr_Tune (sony_tuner_t * pTuner,
                                                   uint32_t centerFreqKHz,
                                                   sony_dtv_system_t system,
                                                   sony_dtv_bandwidth_t bandwidth);

static sony_result_t sony_tuner_helene2_sat_Tune (sony_tuner_t * pTuner,
                                                  uint32_t centerFreqKHz,
                                                  sony_dtv_system_t system,
                                                  uint32_t symbolRateKSps);

static sony_result_t sony_tuner_helene2_Sleep (sony_tuner_t * pTuner);

static sony_result_t sony_tuner_helene2_Shutdown (sony_tuner_t * pTuner);

static sony_result_t sony_tuner_helene2_ReadRFLevel (sony_tuner_t * pTuner, int32_t * pRFLevel);

static sony_result_t sony_tuner_helene2_CalcRFLevelFromAGC (sony_tuner_t * pTuner, uint32_t agcValue, int32_t * pRFLevel);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_tuner_helene2_Create (sony_tuner_t * pTuner,
                                         sony_helene2_xtal_t xtalFreq,
                                         uint8_t i2cAddress,
                                         sony_i2c_t * pI2c,
                                         uint32_t configFlags,
                                         sony_helene2_t * pHelene2Tuner)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tuner_helene2_Create");

    if ((!pI2c) || (!pHelene2Tuner) || (!pTuner)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    sony_memset (pTuner, 0, sizeof (sony_tuner_t)); /* Zero filled */

    /* Create the underlying HELENE2 reference driver. */
    result = sony_helene2_Create (pHelene2Tuner, xtalFreq, i2cAddress, pI2c, configFlags);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Populate / Initialize the sony_tuner_t structure */
    pTuner->i2cAddress = i2cAddress;
    pTuner->pI2c = pI2c;
    pTuner->flags = configFlags;
    pTuner->symbolRateKSpsForSpectrum = 45000;

    pTuner->rfLevelFuncTerr = SONY_TUNER_RFLEVEL_FUNC_READ; /* Use ReadRFLevel function for terrestrial */
    pTuner->rfLevelFuncSat = SONY_TUNER_RFLEVEL_FUNC_READ; /* Use ReadRFLevel function for satellite */

    pTuner->user = pHelene2Tuner;

    pTuner->Initialize = sony_tuner_helene2_Initialize;
    pTuner->TerrCableTune = sony_tuner_helene2_terr_Tune;
    pTuner->SatTune = sony_tuner_helene2_sat_Tune;
    pTuner->Sleep = sony_tuner_helene2_Sleep;
    pTuner->Shutdown = sony_tuner_helene2_Shutdown;
    pTuner->ReadRFLevel = sony_tuner_helene2_ReadRFLevel;
    pTuner->CalcRFLevelFromAGC = sony_tuner_helene2_CalcRFLevelFromAGC;

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tuner_helene2_SetGPO (sony_tuner_t * pTuner, uint8_t id, uint8_t value)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tuner_helene2_Write_GPIO");

    if (!pTuner || !pTuner->user) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_helene2_SetGPO (((sony_helene2_t *) pTuner->user), id, value);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tuner_helene2_GetGPI1 (sony_tuner_t * pTuner, uint8_t * pValue)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tuner_helene2_GetGPI1");

    if (!pTuner || !pTuner->user) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_helene2_GetGPI1 (((sony_helene2_t *) pTuner->user), pValue);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tuner_helene2_RFFilterConfig (sony_tuner_t * pTuner, uint8_t coeff, uint8_t offset)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tuner_helene2_RFFilterConfig");

    if (!pTuner || !pTuner->user) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_helene2_RFFilterConfig (((sony_helene2_t *) pTuner->user), coeff, offset);

    SONY_TRACE_RETURN (result);
}

/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sony_result_t sony_tuner_helene2_Initialize (sony_tuner_t * pTuner)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tuner_helene2_Initialize");

    if (!pTuner || !pTuner->user) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Call into underlying driver. */
    result = sony_helene2_Initialize (((sony_helene2_t *) pTuner->user));

    /* Device is in "Power Save" state. */
    pTuner->frequencyKHz = 0;
    pTuner->system = SONY_DTV_SYSTEM_UNKNOWN;
    pTuner->bandwidth = SONY_DTV_BW_UNKNOWN;
    pTuner->symbolRateKSps = 0;

    SONY_TRACE_RETURN (result);
}

static sony_result_t sony_tuner_helene2_terr_Tune (sony_tuner_t * pTuner,
                                                   uint32_t centerFreqKHz,
                                                   sony_dtv_system_t system,
                                                   sony_dtv_bandwidth_t bandwidth)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_helene2_tv_system_t hSystem = SONY_HELENE2_TV_SYSTEM_UNKNOWN;

    SONY_TRACE_ENTER ("sony_tuner_helene2_terr_Tune");

    if (!pTuner || !pTuner->user) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

   /* Call into underlying driver. Convert system, bandwidth into dtv system. */
    switch (system) {
    case SONY_DTV_SYSTEM_DVBC:
        switch (bandwidth) {
        case SONY_DTV_BW_6_MHZ:
            hSystem = SONY_HELENE2_DTV_DVBC_6;
            break;
        case SONY_DTV_BW_7_MHZ:
            /* 7MHZ BW setting is the same as 8MHz BW */
        case SONY_DTV_BW_8_MHZ:
            hSystem = SONY_HELENE2_DTV_DVBC_8;
            break;
        default:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
        }
        break;

    case SONY_DTV_SYSTEM_DVBT:
        switch (bandwidth) {
        case SONY_DTV_BW_5_MHZ:
            hSystem = SONY_HELENE2_DTV_DVBT_5;
            break;
        case SONY_DTV_BW_6_MHZ:
            hSystem = SONY_HELENE2_DTV_DVBT_6;
            break;
        case SONY_DTV_BW_7_MHZ:
            hSystem = SONY_HELENE2_DTV_DVBT_7;
            break;
        case SONY_DTV_BW_8_MHZ:
            hSystem = SONY_HELENE2_DTV_DVBT_8;
            break;
        default:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
        }
        break;

    case SONY_DTV_SYSTEM_DVBT2:
        switch (bandwidth) {
        case SONY_DTV_BW_1_7_MHZ:
            hSystem = SONY_HELENE2_DTV_DVBT2_1_7;
            break;
        case SONY_DTV_BW_5_MHZ:
            hSystem = SONY_HELENE2_DTV_DVBT2_5;
            break;
        case SONY_DTV_BW_6_MHZ:
            hSystem = SONY_HELENE2_DTV_DVBT2_6;
            break;
        case SONY_DTV_BW_7_MHZ:
            hSystem = SONY_HELENE2_DTV_DVBT2_7;
            break;
        case SONY_DTV_BW_8_MHZ:
            hSystem = SONY_HELENE2_DTV_DVBT2_8;
            break;
        default:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);

        }
        break;

    case SONY_DTV_SYSTEM_DVBC2:
        switch (bandwidth) {
        case SONY_DTV_BW_6_MHZ:
            hSystem = SONY_HELENE2_DTV_DVBC2_6;
            break;
        case SONY_DTV_BW_8_MHZ:
            hSystem = SONY_HELENE2_DTV_DVBC2_8;
            break;
        default:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
        }
        break;

    case SONY_DTV_SYSTEM_ISDBT:
        switch (bandwidth) {
        case SONY_DTV_BW_6_MHZ:
            hSystem = SONY_HELENE2_DTV_ISDBT_6;
            break;
        case SONY_DTV_BW_7_MHZ:
            hSystem = SONY_HELENE2_DTV_ISDBT_7;
            break;
        case SONY_DTV_BW_8_MHZ:
            hSystem = SONY_HELENE2_DTV_ISDBT_8;
            break;
        default:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
        }
        break;

    case SONY_DTV_SYSTEM_ISDBC:
        hSystem = SONY_HELENE2_DTV_DVBC_6; /* ISDB-C uses DVB-C 6MHz BW setting */
        break;

    case SONY_DTV_SYSTEM_ATSC:
        hSystem = SONY_HELENE2_DTV_8VSB;
        break;

    case SONY_DTV_SYSTEM_ATSC3:
        switch (bandwidth) {
        case SONY_DTV_BW_6_MHZ:
            hSystem = SONY_HELENE2_DTV_ATSC3_6;
            break;
        case SONY_DTV_BW_7_MHZ:
            hSystem = SONY_HELENE2_DTV_ATSC3_7;
            break;
        case SONY_DTV_BW_8_MHZ:
            hSystem = SONY_HELENE2_DTV_ATSC3_8;
            break;
        default:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
        }
        break;

    case SONY_DTV_SYSTEM_J83B:
        switch (bandwidth) {
        case SONY_DTV_BW_J83B_5_06_5_36_MSPS:
            hSystem = SONY_HELENE2_DTV_DVBC_6; /* J.83B (5.057, 5.361Msps commonly used in US) uses DVB-C 6MHz BW setting */
            break;
        case SONY_DTV_BW_J83B_5_60_MSPS:
            hSystem = SONY_HELENE2_DTV_J83B_5_6; /* J.83B (5.6Msps used in Japan) uses special setting */
            break;
        default:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
        }
        break;

    /* Intentional fall-through */
    case SONY_DTV_SYSTEM_UNKNOWN:
    default:
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_helene2_terr_Tune (((sony_helene2_t *) pTuner->user), centerFreqKHz, hSystem);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Allow the tuner time to settle */
    SONY_SLEEP(50);

    result = sony_helene2_terr_TuneEnd ((sony_helene2_t *) pTuner->user);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Assign current values. */
    pTuner->frequencyKHz = ((sony_helene2_t *) pTuner->user)->frequencykHz;
    pTuner->system = system;
    pTuner->bandwidth = bandwidth;
    pTuner->symbolRateKSps = 0;

    SONY_TRACE_RETURN (result);
}

static sony_result_t sony_tuner_helene2_sat_Tune (sony_tuner_t * pTuner,
                                                  uint32_t centerFreqKHz,
                                                  sony_dtv_system_t system,
                                                  uint32_t symbolRateKSps)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_helene2_tv_system_t hSystem = SONY_HELENE2_STV_DVBS2;

    SONY_TRACE_ENTER ("sony_tuner_helene2_sat_Tune");

    if ((!pTuner) || (!pTuner->user)){
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    switch(system)
    {
    case SONY_DTV_SYSTEM_DVBS:
        hSystem = SONY_HELENE2_STV_DVBS;
        break;

    case SONY_DTV_SYSTEM_DVBS2:
        hSystem = SONY_HELENE2_STV_DVBS2;
        break;

    case SONY_DTV_SYSTEM_ISDBS:
        hSystem = SONY_HELENE2_STV_ISDBS;
        break;

    case SONY_DTV_SYSTEM_ISDBS3:
        hSystem = SONY_HELENE2_STV_ISDBS3;
        break;

    default:
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_helene2_sat_Tune (((sony_helene2_t *) pTuner->user), centerFreqKHz, hSystem, symbolRateKSps);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    pTuner->frequencyKHz = ((sony_helene2_t *) pTuner->user)->frequencykHz;
    pTuner->system = system;
    pTuner->bandwidth = SONY_DTV_BW_UNKNOWN;
    pTuner->symbolRateKSps = symbolRateKSps;

    /* Tuner stabillization time */
    SONY_SLEEP (50);

    SONY_TRACE_RETURN (result);
}

static sony_result_t sony_tuner_helene2_Sleep (sony_tuner_t * pTuner)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tuner_helene2_Sleep");

    if (!pTuner || !pTuner->user) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Call into underlying driver. */
    result = sony_helene2_Sleep (((sony_helene2_t *) pTuner->user));

    /* Device is in "Power Save" state. */
    pTuner->frequencyKHz = 0;
    pTuner->system = SONY_DTV_SYSTEM_UNKNOWN;
    pTuner->bandwidth = SONY_DTV_BW_UNKNOWN;
    pTuner->symbolRateKSps = 0;

    SONY_TRACE_RETURN (result);
}

static sony_result_t sony_tuner_helene2_Shutdown (sony_tuner_t * pTuner)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tuner_helene2_Shutdown");

    if (!pTuner || !pTuner->user) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Call into underlying driver. */
    result = sony_helene2_Sleep (((sony_helene2_t *) pTuner->user));

    /* Device is in "Power Save" state. */
    pTuner->frequencyKHz = 0;
    pTuner->system = SONY_DTV_SYSTEM_UNKNOWN;
    pTuner->bandwidth = SONY_DTV_BW_UNKNOWN;
    pTuner->symbolRateKSps = 0;

    SONY_TRACE_RETURN (result);
}

static sony_result_t sony_tuner_helene2_ReadRFLevel (sony_tuner_t * pTuner, int32_t * pRFLevel)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tuner_helene2_ReadRFLevel");

    if (!pTuner || !pTuner->user || !pRFLevel) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (SONY_DTV_SYSTEM_IS_TERR_CABLE (pTuner->system)) {
        result = sony_helene2_terr_ReadRssi (((sony_helene2_t *) pTuner->user), pRFLevel);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        /* Add IFOUT value */
        switch (pTuner->system) {
        case SONY_DTV_SYSTEM_DVBT:
        case SONY_DTV_SYSTEM_DVBT2:
        case SONY_DTV_SYSTEM_DVBC2:
        case SONY_DTV_SYSTEM_ATSC3:
            *pRFLevel -= 400; /* -4.0dBm */
            break;

        case SONY_DTV_SYSTEM_DVBC:
        case SONY_DTV_SYSTEM_ISDBC:
        case SONY_DTV_SYSTEM_J83B:
            *pRFLevel -= 150; /* -1.5dBm */
            break;

        case SONY_DTV_SYSTEM_ISDBT:
            *pRFLevel -= 450; /* -4.5dBm */
            break;

        case SONY_DTV_SYSTEM_ATSC:
            *pRFLevel -= 500; /* -5.0dBm */
            break;

        default:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
        }
    } else if (SONY_DTV_SYSTEM_IS_SAT (pTuner->system)) {
        result = sony_helene2_sat_ReadRssi (((sony_helene2_t *) pTuner->user), pRFLevel);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        /* Add IFOUT value */
        *pRFLevel -= 300; /* -3.0dBm */
    } else {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t sony_tuner_helene2_CalcRFLevelFromAGC (sony_tuner_t * pTuner,
                                                            uint32_t agcValue,
                                                            int32_t * pRFLevel)
{
    /* This API is only for satellite. (blind scan) */

    int32_t tempA = 0;
    uint8_t isNegative = 0;
    uint32_t tempDiv = 0;
    uint32_t tempQ = 0;
    uint32_t tempR = 0;

    SONY_TRACE_ENTER("sony_tuner_helene2_CalcRFLevelFromAGC");

    if ((!pTuner) || (!pRFLevel)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (!SONY_DTV_SYSTEM_IS_SAT (pTuner->system)) {
        /* Satellite Only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /*------------------------------------------------
      Gain_db      = AGCLevel * (-14   / 403) + 97
      Gain_db_x100 = AGCLevel * (-1400 / 403) + 9700
    -------------------------------------------------*/
    tempA = (int32_t)agcValue * (-1400);

    tempDiv = 403;
    if (tempA > 0) {
        isNegative = 0;
        tempQ = (uint32_t)tempA / tempDiv;
        tempR = (uint32_t)tempA % tempDiv;
    } else {
        isNegative = 1;
        tempQ = (uint32_t)(tempA * (-1)) / tempDiv;
        tempR = (uint32_t)(tempA * (-1)) % tempDiv;
    }

    if (isNegative) {
        if (tempR >= (tempDiv/2)) {
            *pRFLevel = (int32_t)(tempQ + 1) * (int32_t)(-1);
        } else {
            *pRFLevel = (int32_t)tempQ * (int32_t)(-1);
        }
    } else {
        if (tempR >= (tempDiv/2)) {
            *pRFLevel = (int32_t)(tempQ + 1);
        } else {
            *pRFLevel = (int32_t)tempQ;
        }
    }

    *pRFLevel += 9700;

    /*
     * RF_Level = Input_Ref_Level - RF_Gain
     * -> Input_Ref_Level = 0dB for this demodulator
     */
    *pRFLevel *= -1;

    SONY_TRACE_RETURN(SONY_RESULT_OK);
}
