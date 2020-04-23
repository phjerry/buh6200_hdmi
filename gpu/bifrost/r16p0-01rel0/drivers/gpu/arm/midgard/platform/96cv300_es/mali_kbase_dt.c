
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/pm_opp.h>
#include <linux/ioport.h>

#include <mali_kbase.h>
#include <mali_kbase_defs.h>
#include <mali_kbase_config.h>
#include <backend/gpu/mali_kbase_device_internal.h>

#include "mali_kbase_config_platform.h"
#include "mali_kbase_dt.h"
#include "mali_kbase_voltage_update.h"

#ifdef CONFIG_GPU_TEE
#include "hi_drv_ssm.h"
#endif

#ifndef CONFIG_OF
#define KBASE_MALI_REG_BASE     0x1900000
#define KBASE_MALI_REG_SIZE     (4096 * 8 - 1)

#define KBASE_HIGPU_REG_BASE    0xA00000
#define KBASE_HIGPU_REG_SIZE    0x1000
#define KBASE_HIGPU_REG_RESET   0x744
#define KBASE_HIGPU_REG_FREQ    0x324

#define KBASE_GPUIRQ_NUM        162
#define KBASE_JOBIRQ_NUM        162
#define KBASE_MMUIRQ_NUM        162

static struct kbase_io_resources g_io_resources = {
	.job_irq_number = KBASE_JOBIRQ_NUM,
	.mmu_irq_number = KBASE_MMUIRQ_NUM,
	.gpu_irq_number = KBASE_GPUIRQ_NUM,
	.io_memory_region = {
	.start = KBASE_MALI_REG_BASE,
	.end   = KBASE_MALI_REG_BASE + KBASE_MALI_REG_SIZE
	}
};
#endif /* CONFIG_OF */

#define MAX_CORE_NUM 8

static struct kbase_platform_config g_platform_config = {
#ifndef CONFIG_OF
	.io_resources = &io_resources
#endif
};

static struct device *g_platform_device = NULL;

struct devfreq_config g_devfreq_config;

struct kbase_platform_config *kbase_get_platform_config(void)
{
	return &g_platform_config;
}

static int set_default_property(const struct device *dev)
{
	g_devfreq_config.dvfs_enable = 1;

	g_devfreq_config.temp_ctrl = 0;

	g_devfreq_config.input_ctrl = 0;

	return 0;
}

struct device* kbase_get_platform_device(void)
{
	return g_platform_device;
}

void* kbase_get_devfreq_config(void)
{
	return (void*)(&g_devfreq_config);
}

int kbase_get_core_num(int core_mask)
{
	int i;
	int core_num = 0;

	for (i = 0; i < MAX_CORE_NUM; i++) {
		if (core_mask & (1<<i)) {
			core_num++;
		}
	}

	return core_num;
}

int platform_init(struct kbase_device *kbdev)
{
	int ret;
	struct device *dev = kbdev->dev;

	if (dev == NULL) {
		hisi_error_info();
		return -ENODEV;
	}

	g_platform_device = dev;

	ret = set_default_property(dev);
	if (ret != 0) {
		hisi_error_info();
		return -EPERM;
	}

#ifdef CONFIG_GPU_VOLTAGE_UPDATE
	ret = kbase_voltage_update_init();
	if (ret != 0) {
		hisi_error_info();
		return -EPERM;
	}
#endif

#ifdef CONFIG_GPU_TEE
	/* config secure register in the secure side */
	hi_drv_ssm_iommu_config(LOGIC_MOD_ID_GPU);
#endif

	hisi_debug_trace();

	return 0;
}

void platform_term(struct kbase_device *kbdev)
{
#ifdef CONFIG_GPU_VOLTAGE_UPDATE
	kbase_voltage_update_deinit();
#endif

	hisi_debug_trace();
}

struct kbase_platform_funcs_conf platform_funcs = {
	.platform_init_func = platform_init,
	.platform_term_func = platform_term,
};



