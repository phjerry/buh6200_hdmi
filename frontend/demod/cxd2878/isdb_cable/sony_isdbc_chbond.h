/*------------------------------------------------------------------------------
  Copyright 2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/08/30
  Modification ID : 902dae58eb5249e028ccc5d7f60bb66cca9b43a9
------------------------------------------------------------------------------*/
/**
 @file    sony_isdbc_chbond.h

          This file provides ISDB-C channel bonding (J.183) related type definitions.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_ISDBC_CHBOND_H
#define SONY_ISDBC_CHBOND_H

/*------------------------------------------------------------------------------
 Enumerations
------------------------------------------------------------------------------*/
/**
 @brief Stream Type defined in TSMF header extension information.
*/
typedef enum sony_isdbc_chbond_stream_type_t {
    SONY_ISDBC_CHBOND_STREAM_TYPE_TLV,               /**< TLV */
    SONY_ISDBC_CHBOND_STREAM_TYPE_TS_NONE            /**< TS or no stream */
} sony_isdbc_chbond_stream_type_t;

/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief TSMF Header extension information structure for channel bonding case.
*/
typedef struct sony_isdbc_chbond_tsmf_header_ext_t {
    uint8_t aceewData[26];                          /**< 204 bit ISDB-T AC carrier data including EEW information + 4'b0 */
    sony_isdbc_chbond_stream_type_t streamType[15]; /**< Stream type (TLV or TS) */
    uint8_t groupID;                                /**< Group ID */
    uint8_t numCarriers;                            /**< Number of carriers for channel bonding (2 - 255) */
    uint8_t carrierSequence;                        /**< Order of carrier to bond carriers correctly */
    uint8_t numFrames;                              /**< Number of extension frames in super frame */
    uint8_t framePosition;                          /**< Position of the extension frame in the super frame */
} sony_isdbc_chbond_tsmf_header_ext_t;

#endif /* SONY_ISDBC_CHBOND_H */
