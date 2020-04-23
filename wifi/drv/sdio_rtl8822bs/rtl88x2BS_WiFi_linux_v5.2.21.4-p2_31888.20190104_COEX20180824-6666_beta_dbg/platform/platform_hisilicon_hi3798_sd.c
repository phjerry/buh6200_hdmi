/******************************************************************************
 *
 * Copyright(c) 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#include <linux/delay.h>		/* mdelay() */
#include <mach/hardware.h>		/* __io_address(), readl(), writel() */
#include "platform_hisilicon_hi3798_sdio.h"	/* HI_S32() and etc. */

typedef enum hi_GPIO_DIR_E {
	HI_DIR_OUT = 0,
	HI_DIR_IN  = 1,
} HI_GPIO_DIR_E;

#define RTL_REG_ON_GPIO		(4*8 + 3)

#define REG_BASE_CTRL		__io_address(0xf8a20008)
#define SDIO_NUM                   1

int gpio_wlan_reg_on = RTL_REG_ON_GPIO;
#if 0
module_param(gpio_wlan_reg_on, uint, 0644);
MODULE_PARM_DESC(gpio_wlan_reg_on, "wlan reg_on gpio num (default:gpio4_3)");
#endif

static int hi_gpio_set_value(u32 gpio, u32 value)
{
	HI_S32 s32Status;

	s32Status = HI_DRV_GPIO_SetDirBit(gpio, HI_DIR_OUT);
	if (s32Status != HI_SUCCESS) {
		pr_err("gpio(%d) HI_DRV_GPIO_SetDirBit HI_DIR_OUT failed\n",
			gpio);
		return -1;
	}

	s32Status = HI_DRV_GPIO_WriteBit(gpio, value);
	if (s32Status != HI_SUCCESS) {
		pr_err("gpio(%d) HI_DRV_GPIO_WriteBit value(%d) failed\n",
			gpio, value);
		return -1;
	}

	return 0;
}

static int hisi_wlan_set_carddetect(bool present)
{
	u32 regval;
	u32 mask;

	pr_info("SDIO ID=%d\n", SDIO_NUM);

	regval = readl(REG_BASE_CTRL);
	if (present) {
		pr_info("====== Card detection to detect SDIO card! ======\n");
		/* set card_detect low to detect card */
		regval |= SDIO_NUM;
	} else {
		pr_info("====== Card detection to remove SDIO card! ======\n");
		/* set card_detect high to remove card */
		regval &= ~(SDIO_NUM);
	}
	writel(regval, REG_BASE_CTRL);

	return 0;
}

/*
 * Return:
 *	0:	power on successfully
 *	others: power on failed
 */
int platform_wifi_power_on(void)
{
	int ret = 0;

	hi_gpio_set_value(gpio_wlan_reg_on, 1);
	mdelay(100);
	hisi_wlan_set_carddetect(1);
	mdelay(800);
	pr_info("======== set_carddetect delay 200MS! ========\n");

	return ret;
}

void platform_wifi_power_off(void)
{
	hisi_wlan_set_carddetect(0);
	mdelay(100);
	hi_gpio_set_value(gpio_wlan_reg_on, 0);
}
