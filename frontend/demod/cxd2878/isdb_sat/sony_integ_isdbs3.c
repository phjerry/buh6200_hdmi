/*------------------------------------------------------------------------------
  Copyright 2016-2019 Sony Semiconductor Solutions Corporation

  Last Updated    : 2019/02/14
  Modification ID : 83dbabba2734697e67e3d44647acf57b272c2caf
------------------------------------------------------------------------------*/
#include "sony_integ.h"
#include "sony_integ_isdbs3.h"
#include "sony_demod.h"
#include "sony_demod_isdbs3.h"
#include "sony_demod_isdbs3_monitor.h"

/*----------------------------------------------------------------------------
  Static Functions
----------------------------------------------------------------------------*/
/*
 @brief Polls the demodulator waiting for TSTLV lock at 10ms intervals up to a timeout of 3s.
*/
static sony_result_t isdbs3_WaitTSTLVLock (sony_integ_t * pInteg);

/*
 @brief Polls the demodulator waiting for TMCC lock at 10ms intervals up to a timeout of 2s.
*/
static sony_result_t isdbs3_WaitTMCCLock (sony_integ_t * pInteg);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_integ_isdbs3_Tune (sony_integ_t * pInteg,
                                      sony_isdbs3_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_isdbs3_tmcc_info_t tmccInfo;
    sony_isdbs3_tune_param_t tuneParamRelativeStream;

    SONY_TRACE_ENTER ("sony_integ_isdbs3_Tune");

    if ((!pInteg) || (!pTuneParam) || (!pInteg->pDemod)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pInteg->pDemod->state != SONY_DEMOD_STATE_SLEEP) && (pInteg->pDemod->state != SONY_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (SONY_DEMOD_CHIP_ID_2856_FAMILY (pInteg->pDemod->chipId)
        && pTuneParam->streamidType == SONY_ISDBS3_STREAMID_TYPE_RELATIVE_STREAM_NUMBER) {
        if (pTuneParam->streamid > 15) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
        }

        /* Create tune_param for relative stream number */
        tuneParamRelativeStream.centerFreqKHz = pTuneParam->centerFreqKHz;
        tuneParamRelativeStream.streamid = 0xFFFF;
        tuneParamRelativeStream.streamidType = SONY_ISDBS3_STREAMID_TYPE_STREAMID;

        result = sony_demod_isdbs3_Tune (pInteg->pDemod, &tuneParamRelativeStream);
        if (result != SONY_RESULT_OK){
            SONY_TRACE_RETURN (result);
        }
    } else {
        result = sony_demod_isdbs3_Tune (pInteg->pDemod, pTuneParam);
        if (result != SONY_RESULT_OK){
            SONY_TRACE_RETURN (result);
        }
    }

    if ((pInteg->pTuner) && (pInteg->pTuner->SatTune)) {
        /* Enable the I2C repeater */
        result = sony_demod_I2cRepeaterEnable (pInteg->pDemod, 0x01);
        if (result != SONY_RESULT_OK){
            SONY_TRACE_RETURN (result);
        }

        result = pInteg->pTuner->SatTune (pInteg->pTuner, pTuneParam->centerFreqKHz, SONY_DTV_SYSTEM_ISDBS3, 33756);
        if (result != SONY_RESULT_OK){
            SONY_TRACE_RETURN (result);
        }

        /* Disable the I2C repeater */
        result = sony_demod_I2cRepeaterEnable (pInteg->pDemod, 0x00);
        if (result != SONY_RESULT_OK){
            SONY_TRACE_RETURN (result);
        }
    }

    result = sony_demod_TuneEnd(pInteg->pDemod);
    if (result != SONY_RESULT_OK){
        SONY_TRACE_RETURN (result);
    }

    /* Wait for TMCC lock */
    result = isdbs3_WaitTMCCLock (pInteg);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (SONY_DEMOD_CHIP_ID_2856_FAMILY (pInteg->pDemod->chipId)
        && pTuneParam->streamidType == SONY_ISDBS3_STREAMID_TYPE_RELATIVE_STREAM_NUMBER) {
        /* Get StreamID from TMCC information */
        result = sony_demod_isdbs3_monitor_TMCCInfo(pInteg->pDemod, &tmccInfo);
        if (result == SONY_RESULT_ERROR_HW_STATE) {
            /* TMCC lock is lost causing monitor to fail, return UNLOCK instead of HW STATE */
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_UNLOCK);
        } else if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (tmccInfo.streamTypeForEachRelativeStream[pTuneParam->streamid] == SONY_ISDBS3_STREAM_TYPE_NO_TYPE_ALLOCATED) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_UNLOCK);
        }

        /* Set Stream ID */
        result = sony_demod_isdbs3_SetStreamID (pInteg->pDemod, tmccInfo.streamidForEachRelativeStream[pTuneParam->streamid],
            SONY_ISDBS3_STREAMID_TYPE_STREAMID);
        if (result != SONY_RESULT_OK){
            SONY_TRACE_RETURN (result);
        }
    } else {
        uint8_t streamIDError = 0;

        /* Check Stream ID */
        result = sony_demod_isdbs3_monitor_StreamIDError (pInteg->pDemod, &streamIDError);
        if (result == SONY_RESULT_ERROR_HW_STATE) {
            /* TMCC lock is lost causing monitor to fail, return UNLOCK instead of HW STATE */
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_UNLOCK);
        } else if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (streamIDError) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_UNLOCK);
        }
    }

    /* Wait for TSTLV lock */
    result = isdbs3_WaitTSTLVLock (pInteg);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_integ_isdbs3_monitor_RFLevel (sony_integ_t * pInteg, int32_t * pRFLeveldB)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_integ_isdbs3_monitor_RFLevel");

    if ((!pInteg) || (!pInteg->pDemod) || (!pRFLeveldB)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pInteg->pDemod->state != SONY_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pInteg->pDemod->system != SONY_DTV_SYSTEM_ISDBS3)  {
        /* Not ISDB-S3 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (!pInteg->pTuner) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
    }

    if (pInteg->pTuner->rfLevelFuncSat == SONY_TUNER_RFLEVEL_FUNC_READ && pInteg->pTuner->ReadRFLevel) {
        /* Enable the I2C repeater */
        result = sony_demod_I2cRepeaterEnable (pInteg->pDemod, 0x01);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        result = pInteg->pTuner->ReadRFLevel (pInteg->pTuner, pRFLeveldB);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        /* Disable the I2C repeater */
        result = sony_demod_I2cRepeaterEnable (pInteg->pDemod, 0x00);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    } else if (pInteg->pTuner->rfLevelFuncSat == SONY_TUNER_RFLEVEL_FUNC_CALCFROMAGC
        && pInteg->pTuner->CalcRFLevelFromAGC) {
        uint32_t ifAgc;

        result = sony_demod_isdbs3_monitor_IFAGCOut (pInteg->pDemod, &ifAgc);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        result = pInteg->pTuner->CalcRFLevelFromAGC (pInteg->pTuner, ifAgc, pRFLeveldB);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    } else {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
    }

    /* dBm * 100 -> dBm * 1000 */
    *pRFLeveldB *= 10;

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

/*----------------------------------------------------------------------------
  Static Functions
----------------------------------------------------------------------------*/
static sony_result_t isdbs3_WaitTSTLVLock (sony_integ_t * pInteg)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_demod_lock_result_t lock = SONY_DEMOD_LOCK_RESULT_NOTDETECT;
    sony_stopwatch_t timer;
    uint8_t continueWait = 1;
    uint32_t elapsed = 0;

    SONY_TRACE_ENTER ("isdbs3_WaitTSTLVLock");

    if ((!pInteg) || (!pInteg->pDemod)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pInteg->pDemod->state != SONY_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Wait for TSTLV lock */
    result = sony_stopwatch_start (&timer);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    for (;;) {
        result = sony_stopwatch_elapsed(&timer, &elapsed);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (elapsed >= SONY_ISDBS3_WAIT_TSTLV_LOCK) {
            continueWait = 0;
        }

        result = sony_demod_isdbs3_CheckTSTLVLock (pInteg->pDemod, &lock);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        switch (lock) {
        case SONY_DEMOD_LOCK_RESULT_LOCKED:
            SONY_TRACE_RETURN (SONY_RESULT_OK);

        case SONY_DEMOD_LOCK_RESULT_UNLOCKED:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_UNLOCK);

        default:
            /* continue waiting... */
            break;
        }

        /* Check cancellation. */
        result = sony_integ_CheckCancellation (pInteg);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (continueWait) {
            result = sony_stopwatch_sleep (&timer, SONY_ISDBS3_WAIT_LOCK_INTERVAL);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
        } else {
            result = SONY_RESULT_ERROR_TIMEOUT;
            break;
        }
    }

    SONY_TRACE_RETURN (result);
}

static sony_result_t isdbs3_WaitTMCCLock (sony_integ_t * pInteg)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_demod_lock_result_t lock = SONY_DEMOD_LOCK_RESULT_NOTDETECT;
    sony_stopwatch_t timer;
    uint8_t continueWait = 1;
    uint32_t elapsed = 0;

    SONY_TRACE_ENTER ("isdbs3_WaitTMCCLock");

    if ((!pInteg) || (!pInteg->pDemod)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pInteg->pDemod->state != SONY_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Wait for TMCC lock */
    result = sony_stopwatch_start (&timer);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    for (;;) {
        result = sony_stopwatch_elapsed(&timer, &elapsed);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (elapsed >= SONY_ISDBS3_WAIT_TMCC_LOCK) {
            continueWait = 0;
        }

        result = sony_demod_isdbs3_CheckTMCCLock (pInteg->pDemod, &lock);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        switch (lock) {
        case SONY_DEMOD_LOCK_RESULT_LOCKED:
            SONY_TRACE_RETURN (SONY_RESULT_OK);

        case SONY_DEMOD_LOCK_RESULT_UNLOCKED:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_UNLOCK);

        default:
            /* continue waiting... */
            break;
        }

        /* Check cancellation. */
        result = sony_integ_CheckCancellation (pInteg);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (continueWait) {
            result = sony_stopwatch_sleep (&timer, SONY_ISDBS3_WAIT_LOCK_INTERVAL);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
        } else {
            result = SONY_RESULT_ERROR_TIMEOUT;
            break;
        }
    }

    SONY_TRACE_RETURN (result);
}

