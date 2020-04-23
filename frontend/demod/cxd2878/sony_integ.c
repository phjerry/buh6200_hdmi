/*------------------------------------------------------------------------------
  Copyright 2016-2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/07/05
  Modification ID : c7c68d70868a4a9fb9e0f480d8b940e8a1e8f651
------------------------------------------------------------------------------*/

#include "sony_common.h"
#include "sony_demod.h"
#include "sony_integ.h"

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_integ_Create (sony_integ_t * pInteg,
                                 sony_demod_t * pDemod,
                                 sony_demod_create_param_t * pCreateParam,
                                 sony_i2c_t * pDemodI2c,
                                 sony_tuner_t * pTuner
#ifdef SONY_DEMOD_SUPPORT_SAT_DEVICE_CTRL
                                 ,sony_lnbc_t * pLnbc
#endif
                                 )
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_integ_Create");

    if ((!pInteg) || (!pDemod) || (!pCreateParam) || (!pDemodI2c)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Create demodulator instance */
    result = sony_demod_Create (pDemod, pCreateParam, pDemodI2c);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Populate the integration structure */
    pInteg->pDemod = pDemod;
    pInteg->pTuner = pTuner;

#ifdef SONY_DEMOD_SUPPORT_SAT_DEVICE_CTRL
    pInteg->pLnbc = pLnbc;
#endif

    sony_integ_ClearCancel (pInteg);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_integ_Initialize (sony_integ_t * pInteg)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_integ_Initialize");

    if ((!pInteg) || (!pInteg->pDemod)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_demod_Initialize (pInteg->pDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Enable the I2C repeater */
    result = sony_demod_I2cRepeaterEnable (pInteg->pDemod, 0x01);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if ((pInteg->pTuner) && (pInteg->pTuner->Initialize)) {
        /* Initialize the terrestrial / cable tuner. */
        result = pInteg->pTuner->Initialize (pInteg->pTuner);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    /* Disable the I2C repeater */
    result = sony_demod_I2cRepeaterEnable (pInteg->pDemod, 0x00);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

#ifdef SONY_DEMOD_SUPPORT_SAT_DEVICE_CTRL
    if ((pInteg->pLnbc) && (pInteg->pLnbc->Initialize)) {
        /* Initialize the lnb controller. */
        result = pInteg->pLnbc->Initialize (pInteg->pLnbc);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    if ((pInteg->pLnbc) && (pInteg->pLnbc->Sleep)) {
        /* Sleep the lnb controller */
        result = pInteg->pLnbc->Sleep (pInteg->pLnbc);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }
#endif

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_integ_Sleep (sony_integ_t * pInteg)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_integ_Sleep");

    if ((!pInteg) || (!pInteg->pDemod)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pInteg->pDemod->state != SONY_DEMOD_STATE_SLEEP) && (pInteg->pDemod->state != SONY_DEMOD_STATE_ACTIVE) &&
        (pInteg->pDemod->state != SONY_DEMOD_STATE_SHUTDOWN)) {
        /* This api is accepted in Sleep, Active and Shutdown states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Call the demodulator Sleep function */
    result = sony_demod_Sleep (pInteg->pDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Enable the I2C repeater */
    result = sony_demod_I2cRepeaterEnable (pInteg->pDemod, 0x01);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if ((pInteg->pTuner) && (pInteg->pTuner->Sleep)) {
        /* Call the terrestrial / cable tuner Sleep implementation */
        result = pInteg->pTuner->Sleep (pInteg->pTuner);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    /* Disable the I2C repeater */
    result = sony_demod_I2cRepeaterEnable (pInteg->pDemod, 0x00);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_integ_Shutdown (sony_integ_t * pInteg)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_integ_Shutdown");

    if ((!pInteg) || (!pInteg->pDemod)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pInteg->pDemod->state != SONY_DEMOD_STATE_SLEEP) && (pInteg->pDemod->state != SONY_DEMOD_STATE_ACTIVE) &&
        (pInteg->pDemod->state != SONY_DEMOD_STATE_SHUTDOWN)) {
        /* This api is accepted in Sleep, Active and Shutdown states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pInteg->pDemod->state == SONY_DEMOD_STATE_SHUTDOWN) {
        /* Nothing to do */
        SONY_TRACE_RETURN (SONY_RESULT_OK);
    }

    /* At first, set demod to Sleep state to stop TS output. */
    result = sony_demod_Sleep (pInteg->pDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Enable the I2C repeater */
    result = sony_demod_I2cRepeaterEnable (pInteg->pDemod, 0x01);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if ((pInteg->pTuner) && (pInteg->pTuner->Shutdown)) {
        /* Call the terrestrial / cable tuner Shutdown implementation */
        result = pInteg->pTuner->Shutdown (pInteg->pTuner);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    /* Disable the I2C repeater */
    result = sony_demod_I2cRepeaterEnable (pInteg->pDemod, 0x00);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

#ifdef SONY_DEMOD_SUPPORT_SAT_DEVICE_CTRL
    if ((pInteg->pLnbc) && (pInteg->pLnbc->Sleep)) {
        /* Sleep the lnb controller. */
        result = pInteg->pLnbc->Sleep (pInteg->pLnbc);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }
#endif

    /* After controlling tuner and LNBC, set demod to Shutdown state. */
    result = sony_demod_Shutdown (pInteg->pDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_integ_Cancel (sony_integ_t * pInteg)
{
    SONY_TRACE_ENTER ("sony_integ_Cancel");

    /* Argument verification. */
    if (!pInteg) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Set the cancellation flag. */
    sony_atomic_set (&(pInteg->cancel), 1);

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_integ_ClearCancel (sony_integ_t * pInteg)
{
    SONY_TRACE_ENTER ("sony_integ_ClearCancel");

    /* Argument verification. */
    if (!pInteg) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Clear the cancellation flag. */
    sony_atomic_set (&(pInteg->cancel), 0);

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_integ_CheckCancellation (sony_integ_t * pInteg)
{
    SONY_TRACE_ENTER ("sony_integ_CheckCancellation");

    /* Argument verification. */
    if (!pInteg) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Check the cancellation flag. */
    if (sony_atomic_read (&(pInteg->cancel)) != 0) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_CANCEL);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

