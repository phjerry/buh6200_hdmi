/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2010-2019. All rights reserved.
* Description:head of gpio
*/

#ifndef __DRV_GPIO_H__
#define __DRV_GPIO_H__

#include "hi_type.h"
#include "hi_osal.h"
#include "hi_drv_gpio.h"

#ifdef __cplusplus
#if __cplusplus
       extern "C"{
#endif
#endif

#define GPIO_CHECK_POINTER(p)\
    do {\
        if ((p) == HI_NULL) {\
            HI_ERR_GPIO("Pointer is NULL !\n");\
            return HI_ERR_GPIO_NULL_PTR; \
        }\
    } while (0)

hi_s32 drv_gpio_query_interrupt (gpio_interrupt *pgpio_int_value);
hi_s32 drv_gpio_open(hi_void *private_data);
hi_s32 drv_gpio_close(hi_void *private_data);
hi_s32 drv_gpio_set_output_type(hi_u32 gpio_num, hi_gpio_outputtype  output_type);
hi_s32 drv_gpio_get_output_type(hi_u32 gpio_num, hi_gpio_outputtype  *poutput_type);

extern hi_s32 hi_drv_gpio_pm_suspend(hi_void *private_data);
extern hi_s32 hi_drv_gpio_pm_resume(hi_void *private_data);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
