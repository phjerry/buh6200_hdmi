

#include <linux/pm_runtime.h>

#include <mali_kbase.h>
#include <mali_kbase_defs.h>
#include <mali_kbase_config.h>

#include <backend/gpu/mali_kbase_pm_internal.h>

#include "mali_kbase_config_platform.h"
#include "mali_kbase_runtime_pm.h"
#include "mali_kbase_dt.h"
#include "mali_kbase_voltage_update.h"

#include "linux/hisilicon/hi_license.h"

#define DELAY_US 100

#define CORES_BITS_2 0x11

static int g_power_status = 0; /* 1: power on; 0: power off */

#ifdef CONFIG_GPU_LICENSE_CORES
static int license_configure_cores(struct kbase_device *kbdev)
{
	int ret;
	int value = 1;
	struct devfreq_config *config = kbase_get_devfreq_config();

	ret = hi_drv_get_license_support(HI_LICENSE_GPU_CAP, &value);
	if (ret != 0) {
		hisi_error_info();
		return ret;
	}

	if (value == 0) {
		kbase_devfreq_set_core_mask(kbdev, CORES_BITS_2);

		kbdev->current_core_mask = CORES_BITS_2;

		config->dvfs_enable = 0;
	}

	return ret;
}
#endif

int kbase_init_mask_freq_volt(struct kbase_device *kbdev)
{
	int ret;
	struct dev_pm_opp *opp;
	unsigned long freq, volt, mask;
	unsigned long nominal_freq;

	mask         = (unsigned long)kbdev->opp_table[0].core_mask;
	freq         = (unsigned long)kbdev->opp_table[0].real_freq;
	nominal_freq = (unsigned long)kbdev->opp_table[0].opp_freq;

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
	rcu_read_lock();
#endif

	opp  = devfreq_recommended_opp(kbdev->dev, &nominal_freq, 0);
	volt = dev_pm_opp_get_voltage(opp);
#ifdef CONFIG_GPU_VOLTAGE_UPDATE
	kbase_voltage_update(freq, &volt);
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
	rcu_read_unlock();
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
	dev_pm_opp_put(opp);
#endif

	kbase_devfreq_set_core_mask(kbdev, mask);

	ret = regulator_set_voltage(kbdev->regulator, volt, volt);
	if (ret != 0) {
		hisi_error_info();
		return ret;
	}

	ret = clk_set_rate(kbdev->clock, freq);
	if (ret != 0) {
		hisi_error_info();
		return ret;
	}

	kbdev->current_core_mask = mask;
	kbdev->current_freq = freq;
	kbdev->current_voltage = volt;
	kbdev->current_nominal_freq = nominal_freq;

	/* initial_freq must be configured to avoid abnormal print info */
	kbdev->devfreq_profile.initial_freq = nominal_freq;

#ifdef CONFIG_GPU_LICENSE_CORES
	ret = license_configure_cores(kbdev);
	if (ret != 0) {
		hisi_error_info();
		return ret;
	}
#endif

	hisi_debug_info("Init default mask = 0x%llx, freq = %lu, volt = %lu\n",
		kbdev->current_core_mask, kbdev->current_freq, kbdev->current_voltage);

	return ret;
}

void kbase_init_min_max_freq(struct kbase_device *kbdev)
{
	int i;
	unsigned long min_freq = kbdev->opp_table[0].opp_freq;
	unsigned long max_freq = kbdev->opp_table[0].opp_freq;

	for (i = 1; i < kbdev->num_opps; i++) {
		if (min_freq > kbdev->opp_table[i].opp_freq) {
			min_freq = kbdev->opp_table[i].opp_freq;
		}

		if (max_freq < kbdev->opp_table[i].opp_freq) {
			max_freq = kbdev->opp_table[i].opp_freq;
		}
	}

	kbdev->devfreq->min_freq = min_freq;
	kbdev->devfreq->max_freq = max_freq;

	hisi_debug_info("Init min_freq = 0x%lu, max_freq = 0x%lu\n", min_freq, max_freq);

	return;
}

void power_off(struct kbase_device *kbdev)
{
	pm_runtime_mark_last_busy(kbdev->dev);
	pm_runtime_put_autosuspend(kbdev->dev);

	return;
}

int power_on(struct kbase_device *kbdev)
{
	int ret = 1; /* Assume GPU has been powered off */
	int error;

	error = pm_runtime_get_sync(kbdev->dev);
	if (error == 1) {
		/*
		 * Let core know that the chip has not been
		 * powered off, so we can save on re-initialization.
		 */
		ret = 0;
	}

	return ret;
}

void power_suspend(struct kbase_device *kbdev)
{
	g_power_status = 0;

	clk_disable(kbdev->clock);

	hisi_debug_trace();

	return;
}

void power_resume(struct kbase_device *kbdev)
{
	g_power_status = 1;

	clk_enable(kbdev->clock);

	hisi_debug_trace();

	return;
}

#ifdef KBASE_PM_RUNTIME
int power_runtime_init(struct kbase_device *kbdev)
{
	pm_runtime_set_autosuspend_delay(kbdev->dev, DELAY_US);
	pm_runtime_use_autosuspend(kbdev->dev);

	pm_runtime_set_active(kbdev->dev);
	pm_runtime_enable(kbdev->dev);

	if (!pm_runtime_enabled(kbdev->dev)) {
		hisi_error_info();
		return -ENOSYS;
	}

	hisi_debug_trace();

	return 0;
}
void power_runtime_term(struct kbase_device *kbdev)
{
	pm_runtime_disable(kbdev->dev);

	hisi_debug_trace();

	return;
}

void power_runtime_off(struct kbase_device *kbdev)
{
	g_power_status = 0;

	hisi_debug_trace();

	return;
}

int power_runtime_on(struct kbase_device *kbdev)
{
	g_power_status = 1;

	hisi_debug_trace();

	return 0;
}

int power_runtime_idle(struct kbase_device *kbdev)
{
	hisi_debug_trace();

	return 0;
}
#endif


struct kbase_pm_callback_conf pm_callbacks = {
	.power_off_callback = power_off,
	.power_on_callback = power_on,
	.power_suspend_callback = power_suspend,
	.power_resume_callback = power_resume,

#ifdef KBASE_PM_RUNTIME
	.power_runtime_init_callback = power_runtime_init,
	.power_runtime_term_callback = power_runtime_term,
	.power_runtime_off_callback = power_runtime_off,
	.power_runtime_on_callback = power_runtime_on,
	.power_runtime_idle_callback = power_runtime_idle,
#else
	.power_runtime_init_callback = NULL,
	.power_runtime_term_callback = NULL,
	.power_runtime_off_callback = NULL,
	.power_runtime_on_callback = NULL,
	.power_runtime_idle_callback = NULL,
#endif
};

int kbase_get_power_status(void)
{
	return g_power_status;
}

