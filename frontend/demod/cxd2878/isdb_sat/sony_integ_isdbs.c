/*------------------------------------------------------------------------------
  Copyright 2016-2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/07/05
  Modification ID : c7c68d70868a4a9fb9e0f480d8b940e8a1e8f651
------------------------------------------------------------------------------*/
#include "sony_integ.h"
#include "sony_integ_isdbs.h"
#include "sony_demod.h"
#include "sony_demod_isdbs.h"
#include "sony_demod_isdbs_monitor.h"


/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_integ_isdbs_Tune (sony_integ_t * pInteg,
                                     sony_isdbs_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_integ_isdbs_Tune");

    if ((!pInteg) || (!pTuneParam) || (!pInteg->pDemod)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pInteg->pDemod->state != SONY_DEMOD_STATE_SLEEP) && (pInteg->pDemod->state != SONY_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    result = sony_demod_isdbs_Tune (pInteg->pDemod, pTuneParam);
    if (result != SONY_RESULT_OK){
        SONY_TRACE_RETURN (result);
    }

    if ((pInteg->pTuner) && (pInteg->pTuner->SatTune)) {
        /* Enable the I2C repeater */
        result = sony_demod_I2cRepeaterEnable (pInteg->pDemod, 0x01);
        if (result != SONY_RESULT_OK){
            SONY_TRACE_RETURN (result);
        }

        result = pInteg->pTuner->SatTune (pInteg->pTuner, pTuneParam->centerFreqKHz, SONY_DTV_SYSTEM_ISDBS, 28860);
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

    /* Wait for TS lock */
    result = sony_integ_isdbs_WaitTSLock (pInteg);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_integ_isdbs_WaitTSLock (sony_integ_t * pInteg)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_demod_lock_result_t lock = SONY_DEMOD_LOCK_RESULT_NOTDETECT;
    sony_stopwatch_t timer;
    uint8_t continueWait = 1;
    uint32_t elapsed = 0;

    SONY_TRACE_ENTER ("sony_integ_isdbs_WaitTSLock");

    if ((!pInteg) || (!pInteg->pDemod)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pInteg->pDemod->state != SONY_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Wait for TS lock */
    result = sony_stopwatch_start (&timer);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    for (;;) {
        result = sony_stopwatch_elapsed(&timer, &elapsed);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (elapsed >= SONY_ISDBS_WAIT_TS_LOCK) {
            continueWait = 0;
        }

        result = sony_demod_isdbs_CheckTSLock (pInteg->pDemod, &lock);
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
            result = sony_stopwatch_sleep (&timer, SONY_ISDBS_WAIT_LOCK_INTERVAL);
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

sony_result_t sony_integ_isdbs_monitor_RFLevel (sony_integ_t * pInteg, int32_t * pRFLeveldB)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_integ_isdbs_monitor_RFLevel");

    if ((!pInteg) || (!pInteg->pDemod) || (!pRFLeveldB)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pInteg->pDemod->state != SONY_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pInteg->pDemod->system != SONY_DTV_SYSTEM_ISDBS)  {
        /* Not ISDB-S */
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

        result = sony_demod_isdbs_monitor_IFAGCOut (pInteg->pDemod, &ifAgc);
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
