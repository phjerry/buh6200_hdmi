/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description:
 * Author:
 * Create: 2019-9-18
 */

#ifndef _SUT_PJ987_ISDBS_V1000_H_
#define _SUT_PJ987_ISDBS_V1000_H_

#include <linux/types.h>

/*************************************************
 Receiving standard
**************************************************/
typedef enum {
    SONY_SAT_SYSTEM_UNKNOWN  = 0,
    SONY_SAT_SYSTEM_ISDBS,     /* ISDB-S  */
    SONY_SAT_SYSTEM_ISDBS3     /* ISDB-S3 */
} RECEIVE_MODE;

void isdbs_x_read_agc(uint16_t tuner_block_id, uint8_t* icoffset, uint8_t* ifagcreg , uint8_t* rfagcreg);
int16_t isdbs_tuner_calcRSSI(uint16_t tuner_block_id, uint8_t icoffset , uint8_t ifagcreg , uint8_t rfagcreg);
void isdbs_x_pon(uint16_t tuner_block_id);
void isdbs_x_tune(uint16_t tuner_block_id, uint32_t frequency, RECEIVE_MODE mode);
void isdbs_x_fin(uint16_t tuner_block_id);

#endif

