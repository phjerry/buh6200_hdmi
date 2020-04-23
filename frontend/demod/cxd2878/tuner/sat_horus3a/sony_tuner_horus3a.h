/*------------------------------------------------------------------------------
  Copyright 2016-2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/07/05
  Modification ID : c7c68d70868a4a9fb9e0f480d8b940e8a1e8f651
------------------------------------------------------------------------------*/
/**
 @file    sony_tuner_horus3a.h

          This file provides the tuner control function for HORUS3A(CXD2832).
*/
/*----------------------------------------------------------------------------*/
#ifndef SONY_TUNER_HORUS3A_H
#define SONY_TUNER_HORUS3A_H

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tuner.h"
#include "sony_horus3a.h"

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
/**
 @brief Create a instance for HORUS3A driver.

 @param pTuner The tuner driver instance to create.
        Memory must have been allocated for this instance before creation.
 @param xtalFreqMHz The crystal frequency of the tuner (MHz).
        Supports 16MHz or 24MHz or 27MHz.
 @param i2cAddress The I2C address of the HORUS3A device.
        Typically 0xC0.
 @param pI2c The I2C driver that the tuner driver will use for
        communication.
 @param configFlags See ::SONY_HORUS3A_CONFIG_REFOUT_OFF,
                        ::SONY_HORUS3A_CONFIG_POWERSAVE_ENABLEXTAL,
                        ::SONY_HORUS3A_CONFIG_EXT_REF,
                        ::SONY_HORUS3A_CONFIG_LNA_ENABLE,
                        ::SONY_HORUS3A_CONFIG_POWERSAVE_STOPLNA,
                        ::SONY_HORUS3A_CONFIG_IQOUT_DIFFERENTIAL,
                        ::SONY_HORUS3A_CONFIG_IQOUT_SINGLEEND,
                        ::SONY_HORUS3A_CONFIG_IQOUT_SINGLEEND_LOWGAIN defined in
                        \link sony_horus3a.h \endlink
 @param pHorus3a The HORUS3A tuner driver pointer to use.
        Memory must have been allocated for the HORUS3A driver structure.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tuner_horus3a_Create (sony_tuner_t * pTuner,
                                         uint32_t xtalFreqMHz,
                                         uint8_t i2cAddress,
                                         sony_i2c_t * pI2c,
                                         uint32_t configFlags,
                                         sony_horus3a_t * pHorus3a);

/**
 @brief Write to GPO.

 @param pTuner  Instance of the tuner driver.
 @param value   Output logic level, 0 = Low, 1 = High

 @return SONY_RESULT_OK if successful.
 */
sony_result_t sony_tuner_horus3a_SetGPO (sony_tuner_t * pTuner, uint8_t value);

#endif /* SONY_TUNER_HORUS3A_H */
