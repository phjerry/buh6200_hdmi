/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of cryp_trng
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#ifndef __CRYP_TRNG_H_
#define __CRYP_TRNG_H_

#include "drv_osal_lib.h"
#include "drv_trng.h"

/******************************* API Declaration *****************************/
/** \addtogroup      trng */
/** @{ */ /** <!--[trng] */

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Code *****************************/
/** \addtogroup      trng drivers */
/** @{ */ /** <!-- [trng] */

hi_s32 cryp_trng_init(void);
hi_s32 cryp_trng_deinit(void);

/**
\brief get rand number.
\param[out]  randnum rand number.
\param[in]   timeout time out.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 cryp_trng_get_random(hi_u32 *randnum, hi_u32 timeout);

/**
\brief get rand bytes.
\param[out]  randnum rand number.
\param[in]   size size of rand number.
\param[in]   timeout time out.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 cryp_trng_get_random_bytes(hi_u8 *randbyte, hi_u32 size, hi_u32 timeout);

/** @} */ /** <!-- ==== API declaration end ==== */
#endif
