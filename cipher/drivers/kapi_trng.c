/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of kapi_trng
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include "drv_osal_lib.h"
#include "cryp_trng.h"

hi_s32 kapi_trng_init(void)
{
    hi_s32 ret;

    hi_log_func_enter();

    ret = cryp_trng_init();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_trng_init, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 kapi_trng_deinit(void)
{
    hi_s32 ret;

    hi_log_func_enter();

    ret = cryp_trng_deinit();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_trng_deinit, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 kapi_trng_get_rand_byte( hi_u8 *randbyte, hi_u32 size, hi_u32 timeout)
{
    hi_s32 ret;

    hi_log_func_enter();

    hi_log_check_param(randbyte == HI_NULL);

    ret = cryp_trng_get_random_bytes(randbyte, size, timeout);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 kapi_trng_get_random(hi_u32 *randnum, hi_u32 timeout)
{
    hi_s32 ret;

    hi_log_func_enter();

    hi_log_check_param(randnum == HI_NULL);

    ret = cryp_trng_get_random_bytes((hi_u8 *)randnum, WORD_WIDTH, timeout);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

