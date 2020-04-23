/*
 *
 * (C) COPYRIGHT 2015, 2017 ARM Limited. All rights reserved.
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
#include <linux/io.h>
#include <mali_kbase_config.h>

#define EMU 	1    /* also used for hifonev320 */
#define FPGA	0
#if EMU
#define KBASE_HIGPU_REG_BASE		0xF8A22000
#define KBASE_HIGPU_REG_SIZE		0x1000
#define KBASE_HIGPU_REG_RESET 	0x1D8


#define GPU_IRQEVENT_NUM	94	
#define KBASE_GPUIRQ_NUM	143
#define KBASE_JOBIRQ_NUM	142
#define KBASE_MMUIRQ_NUM	141

#define KBASE_MALI_REG_BASE	0xF9200000
#endif

#if FPGA
#define KBASE_HIGPU_REG_BASE    0x12010000
#define KBASE_HIGPU_REG_SIZE    0x1000
#define KBASE_HIGPU_REG_RESET   0xfc

#define KBASE_GPUIRQ_NUM        93
#define KBASE_JOBIRQ_NUM        94
#define KBASE_MMUIRQ_NUM        77

#define KBASE_MALI_REG_BASE     0x11B00000
#endif
static volatile u32* g_pu32GPUBase = NULL;

static int mali_kbase_reset(void)
{

	if(NULL == g_pu32GPUBase)
	{
		g_pu32GPUBase = (volatile u32*)ioremap_nocache(KBASE_HIGPU_REG_BASE,  KBASE_HIGPU_REG_RESET);
		if(NULL != g_pu32GPUBase)
		{
			u32 reset = readl(g_pu32GPUBase + (KBASE_HIGPU_REG_RESET/4));
#if EMU
			/* reset */
			reset = 0x76;
			writel(reset, g_pu32GPUBase + (KBASE_HIGPU_REG_RESET/4));

			/* cancel reset */
			reset = 0x67;
			writel(reset, g_pu32GPUBase + (KBASE_HIGPU_REG_RESET/4));
#endif

#if FPGA
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

int kbase_platform_early_init(void)
{
    printk("devices tree...\n");
	mali_kbase_reset();
	return 0;
}

static struct kbase_platform_config dummy_platform_config;

struct kbase_platform_config *kbase_get_platform_config(void)
{
	return &dummy_platform_config;
}

#ifndef CONFIG_OF
int kbase_platform_register(void)
{
	return 0;
}

void kbase_platform_unregister(void)
{
}
#endif
