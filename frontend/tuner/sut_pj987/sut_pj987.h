/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: SUT-PJ987 driver
 * Author: SDK
 * Create: 2019-9-18
 */
 
#ifndef _SUT_PJ987_H_
#define _SUT_PJ987_H_

#include "hi_type.h"
#if defined(DEMOD_DEV_TYPE_CXD2878)
#include "sony_tuner.h"
#endif

#if defined(DEMOD_DEV_TYPE_CXD2878)
hi_s32 sony_tuner_sut_pj987_create (sony_tuner_t *tuner, hi_u32 *port);
#endif

#endif

