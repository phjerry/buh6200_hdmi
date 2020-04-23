/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description:
 * Author:
 * Create: 2019-9-18
 */

#ifndef _SUT_PJ987_ISDBT_V1000_H_
#define _SUT_PJ987_ISDBT_V1000_H_

#include <linux/types.h>

void isdbt_x_read_agc(uint16_t tuner_block_id, uint8_t* icoffset, uint8_t* ifagcreg , uint8_t* rfagcreg);
int16_t isdbt_tuner_calcRSSI(uint16_t tuner_block_id, uint8_t icoffset , uint8_t ifagcreg , uint8_t rfagcreg);
void isdbt_x_pon(uint16_t tuner_block_id);
void isdbt_x_tune(uint16_t tuner_block_id, uint32_t frequency);
void isdbt_x_tune_end(uint16_t tuner_block_id);
void isdbt_x_fin(uint16_t tuner_block_id);
void isdbt_tuner_gpio(uint16_t tuner_block_id, uint8_t val);

#endif

