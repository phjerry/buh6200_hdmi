/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description:
 */
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include "hi_drv_gpioi2c.h"
#include "hi_debug.h"

extern int g_i2c_mode;
module_param(g_i2c_mode, int, S_IRUGO);

/*************************************************************************/

hi_s32 gpio_i2c_drv_module_init(hi_void)
{
#ifndef HI_MCE_SUPPORT
    hi_s32 ret;

    ret = hi_drv_gpio_i2c_init();
    if ( ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
#endif

#ifdef MODULE
    HI_PRINT("Load hi_gpioi2c.ko success.\t(%s)\n", VERSION_STRING);
#endif

    return HI_SUCCESS;
}

hi_void gpio_i2c_drv_module_exit(hi_void)
{
#ifndef HI_MCE_SUPPORT
    hi_drv_gpio_i2c_deinit();
#endif

    return;
}

#ifdef MODULE
module_init(gpio_i2c_drv_module_init);
module_exit(gpio_i2c_drv_module_exit);
#endif

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("HISILICON");

