/*------------------------------------------------------------------------------
  Copyright 2016-2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/12/13
  Modification ID : e402a4dcb0e51a00c34b174334c082c1211eebf1
------------------------------------------------------------------------------*/
/**
 @file    sony_diseqc_command_2_2.h

          This file provides functions for generating "DiSEqC2.2" commands.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONY_DISEQC_COMMAND_2_2_H
#define SONY_DISEQC_COMMAND_2_2_H

#include "sony_common.h"
#include "sony_diseqc_command.h"

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
/**
 @brief Create DiSEqC command to read Positioner Status Register.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_diseqc_command_PosStat (sony_diseqc_message_t * pMessage,
                                           sony_diseqc_framing_t framing,
                                           sony_diseqc_address_t address);

#endif /* SONY_DISEQC_COMMAND_2_2_H */
