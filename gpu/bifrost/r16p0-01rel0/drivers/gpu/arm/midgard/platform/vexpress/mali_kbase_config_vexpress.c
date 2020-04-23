/*
 *
 * (C) COPYRIGHT 2011-2017 ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you can access it online at
 * http://www.gnu.org/licenses/gpl-2.0.html.
 *
 * SPDX-License-Identifier: GPL-2.0
 *
 */

#include <linux/ioport.h>
#include <mali_kbase.h>
#include <mali_kbase_defs.h>
#include <mali_kbase_config.h>
#include "mali_kbase_config_platform.h"

#define FPGA_HiFoneV320
#ifdef EMU
#define KBASE_HIGPU_REG_BASE	0x12010000
#define KBASE_HIGPU_REG_SIZE	0x1000
#define KBASE_HIGPU_REG_RESET 	0xd8

#define KBASE_GPUIRQ_NUM	148
#define KBASE_JOBIRQ_NUM	149
#define KBASE_MMUIRQ_NUM	150

#define KBASE_MALI_REG_BASE	0x11640000
#endif

#ifdef FPGA_HiFoneV320
#define KBASE_HIGPU_REG_BASE    0x12010000
#define KBASE_HIGPU_REG_SIZE    0x1000
#define KBASE_HIGPU_REG_RESET   0xfc

#define KBASE_GPUIRQ_NUM        93
#define KBASE_JOBIRQ_NUM        94
#define KBASE_MMUIRQ_NUM        77

#define KBASE_MALI_REG_BASE     0x11B00000
#endif


static volatile u32* g_pu32GPUBase = NULL;
#ifndef CONFIG_OF
static struct kbase_io_resources io_resources = {
	.job_irq_number = KBASE_JOBIRQ_NUM,
	.mmu_irq_number = KBASE_MMUIRQ_NUM,
	.gpu_irq_number = KBASE_GPUIRQ_NUM,
	.io_memory_region = {
	.start = KBASE_MALI_REG_BASE,
	.end = KBASE_MALI_REG_BASE + (4096 * 8) - 1
	}
};
#endif /* CONFIG_OF */

static int pm_callback_power_on(struct kbase_device *kbdev)
{
	if(NULL == g_pu32GPUBase)
	{
		g_pu32GPUBase = (volatile u32*)ioremap_nocache(KBASE_HIGPU_REG_BASE,  KBASE_HIGPU_REG_SIZE);
		if(NULL != g_pu32GPUBase)
		{
			u32 reset = readl(g_pu32GPUBase + (KBASE_HIGPU_REG_RESET/4));
#ifdef EMU
			/* reset */
			reset |= 0x11;
			writel(reset, g_pu32GPUBase + (KBASE_HIGPU_REG_RESET/4));

			/* cancel reset */
			reset &= 0xfffffffe;
			writel(reset, g_pu32GPUBase + (KBASE_HIGPU_REG_RESET/4));
#endif

#if defined(FPGA_HiFoneV320)
			/* reset */
			reset = 0x0fffffff;
			writel(reset, g_pu32GPUBase + (KBASE_HIGPU_REG_RESET/4));

			/* cancel reset */
			reset = 0x0effffff;
			writel(reset, g_pu32GPUBase + (KBASE_HIGPU_REG_RESET/4));
#endif

		}
	}

	return 0;
}

static void pm_callback_power_off(struct kbase_device *kbdev)
{
}

struct kbase_pm_callback_conf pm_callbacks = {
	.power_on_callback = pm_callback_power_on,
	.power_off_callback = pm_callback_power_off,
	.power_suspend_callback  = NULL,
	.power_resume_callback = NULL
};

static struct kbase_platform_config higpu_platform_config = {
#ifndef CONFIG_OF
	.io_resources = &io_resources
#endif
};

struct kbase_platform_config *kbase_get_platform_config(void)
{
	return &higpu_platform_config;
}

int kbase_platform_early_init(void)
{
	/* Nothing needed at this stage */
	g_pu32GPUBase = (volatile u32*)ioremap_nocache(KBASE_HIGPU_REG_BASE,  KBASE_HIGPU_REG_SIZE);

	if(NULL != g_pu32GPUBase)
	{
		u32 reset = readl(g_pu32GPUBase + (KBASE_HIGPU_REG_RESET/4));
#ifdef EMU
		/* reset */
		reset |= 0x11;
		writel(reset, g_pu32GPUBase + (KBASE_HIGPU_REG_RESET/4));

		/* cancel reset */
		reset &= 0xfffffffe;
		writel(reset, g_pu32GPUBase + (KBASE_HIGPU_REG_RESET/4));
#endif


#if defined(FPGA_HiFoneV320)
		/* reset */
		reset = 0x0fffffff;
		writel(reset, g_pu32GPUBase + (KBASE_HIGPU_REG_RESET/4));

		/* cancel reset */
		reset = 0x0effffff;
		writel(reset, g_pu32GPUBase + (KBASE_HIGPU_REG_RESET/4));
#endif
	}
	else
	{
		return 1;
	}

	return 0;
}
