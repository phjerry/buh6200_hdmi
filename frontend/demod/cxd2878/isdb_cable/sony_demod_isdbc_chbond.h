/*------------------------------------------------------------------------------
  Copyright 2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/12/13
  Modification ID : e402a4dcb0e51a00c34b174334c082c1211eebf1
------------------------------------------------------------------------------*/
/**
 @file    sony_demod_isdbc_chbond.h

          This file provides the demodulator control and monitor interface for ISDB-C channel bonding.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_DEMOD_ISDBC_CHBOND_H
#define SONY_DEMOD_ISDBC_CHBOND_H

#include "sony_demod.h"
#include "sony_isdbc_chbond.h"

/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief Channel bonding state.

 @note  For each members, 1 is set if each check is OK.
*/
typedef struct {
    uint8_t bondOK;                 /**< Bonding OK or not */
    uint8_t groupIDCheck;           /**< Group ID values are same or not */
    uint8_t streamTypeCheck;        /**< Stream type values are same or not */
    uint8_t numCarriersCheck;       /**< Number of carriers values are same or not */
    uint8_t carrierSeqCheck;        /**< Carrier sequence value check is OK or not */
    uint8_t calcNumCarriersCheck;   /**< Number of valid input carriers are OK or not */
} sony_demod_isdbc_chbond_state_t;

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Enable/disable ISDB-C channel bonding function.

        This API is called internally to enable/disable channel bonding.
        Note that ISDB-C channel bonding function is available on CXD2878 family only.

 @param pDemod  The demodulator instance
 @param enable  Enable/disable setting.
                    - 0: Disable
                    - 1: Enable

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbc_chbond_Enable (sony_demod_t * pDemod,
                                              uint8_t enable);

/**
 @brief TSMF TS selection after channel bonding.

 @param pDemod    The demodulator instance
 @param tsidType  Absolute TSID or relative TS number selection
 @param tsid      TSID or relative TS number
 @param networkId Network ID. Ignored if relative TS number is selected.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbc_chbond_SetTSMFConfig (sony_demod_t * pDemod,
                                                     sony_isdbc_tsid_type_t tsidType,
                                                     uint16_t tsid,
                                                     uint16_t networkId);

/**
 @brief Soft reset the ISDB-C channel bonding function.

 @param pDemod The demod instance.

 @return SONY_RESULT_OK if successfully reset.
*/
sony_result_t sony_demod_isdbc_chbond_SoftReset (sony_demod_t * pDemod);

/**
 @brief Channel bonding status monitor (instant)

 @param pDemod  The demodulator instance
 @param pState  Channel bonding state

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbc_chbond_monitor_BondStat (sony_demod_t * pDemod,
                                                        sony_demod_isdbc_chbond_state_t * pState);

/**
 @brief Channel bonding status monitor (hold type)

 @note  This API is used to know some bonding error occurred or not in long time region.
        If a error is detected, member of sony_demod_isdbc_chbond_state_t become 0.
        ::sony_demod_isdbc_chbond_ClearBondStat can be used to clear error state.

 @param pDemod  The demodulator instance
 @param pState  Channel bonding state

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbc_chbond_monitor_BondStat_hold (sony_demod_t * pDemod,
                                                             sony_demod_isdbc_chbond_state_t * pState);

/**
 @brief Clear the channel bonding status monitor (hold type).

 @param pDemod The demodulator instance.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbc_chbond_ClearBondStat (sony_demod_t * pDemod);

/**
 @brief TLV conversion error monitor. (hold type)

 @param pDemod The demodulator instance.
 @param pError If 1, TLV conversion error is detected.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbc_chbond_monitor_TLVConvError (sony_demod_t * pDemod,
                                                            uint8_t * pError);

/**
 @brief Clear the TLV conversion error monitor.

 @param pDemod The demodulator instance.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbc_chbond_ClearTLVConvError (sony_demod_t * pDemod);

/**
 @brief Carrier delay monitor.

 @param pDemod The demodulator instance.
 @param pDelay Delay between valid input carriers. Please divide by 96 to convert us unit.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbc_chbond_monitor_CarrierDelay (sony_demod_t * pDemod,
                                                            uint32_t * pDelay);

/**
 @brief Emergency Warning Broadcasting System(EWS) detection monitor. (hold type)

 @param pDemod The demodulator instance.
 @param pEWSChange EWS change detection flag.
                   Bit[0] : Stream from external demod 0
                   Bit[1] : Stream from external demod 1
                   Bit[2] : Stream from external demod 2
                   Bit[3] : Stream from internal demod

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbc_chbond_monitor_EWSChange (sony_demod_t * pDemod,
                                                         uint8_t * pEWSChange);

/**
 @brief Clear the EWS detection flag.

 @param pDemod The demodulator instance.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbc_chbond_ClearEWSChange (sony_demod_t * pDemod);

/**
 @brief TSMF version number change monitor. (hold type)

 @param pDemod The demodulator instance.
 @param pVersionChange TSMF version change detection flag.
                       Bit[0] : Stream from external demod 0
                       Bit[1] : Stream from external demod 1
                       Bit[2] : Stream from external demod 2
                       Bit[3] : Stream from internal demod

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbc_chbond_monitor_VersionChange (sony_demod_t * pDemod,
                                                             uint8_t * pVersionChange);

/**
 @brief Clear the TSMF version change detection flag.

 @param pDemod The demodulator instance.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbc_chbond_ClearVersionChange (sony_demod_t * pDemod);

/**
 @brief Stream type (TLV or TS) monitor.

 @param pDemod         The demodulator instance.
 @param pStreamType    Stream type.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbc_chbond_monitor_StreamType (sony_demod_t * pDemod,
                                                          sony_isdbc_chbond_stream_type_t * pStreamType);

/**
 @brief TSMF header extension information monitor.

 @note  This API only convert "private data" in sony_isdbc_tsmf_header_t struct.
        ::sony_demod_isdbc_monitor_TSMFHeader should be called before this API
        to get sony_isdbc_tsmf_header_t data.

 @param pDemod         The demodulator instance.
 @param pTSMFHeader    TSMF header information got by ::sony_demod_isdbc_monitor_TSMFHeader.
 @param pTSMFHeaderExt Converted TSMF header extension information for channel bonding case.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbc_chbond_monitor_TSMFHeaderExt (sony_demod_t * pDemod,
                                                             sony_isdbc_tsmf_header_t * pTSMFHeader,
                                                             sony_isdbc_chbond_tsmf_header_ext_t * pTSMFHeaderExt);

#endif /* SONY_DEMOD_ISDBC_CHBOND_H */
