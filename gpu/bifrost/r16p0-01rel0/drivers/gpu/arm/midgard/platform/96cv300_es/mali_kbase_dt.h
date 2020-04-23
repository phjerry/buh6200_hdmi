
#ifndef _KBASE_CONFIG_DEVICETREE_H_
#define _KBASE_CONFIG_DEVICETREE_H_

#ifdef CONFIG_GPU_DEBUG
#define hisi_debug_info(...) \
	pr_info(__VA_ARGS__);

#define hisi_debug_trace() \
	pr_info("%s\n", __func__);
#else
#define hisi_debug_info(...)

#define hisi_debug_trace()
#endif

#define hisi_error_info() \
	pr_info("Error: func = %s, line = %d\n", __func__, __LINE__);

typedef struct devfreq_config {
	unsigned int dvfs_enable;
	unsigned int temp_ctrl;
	unsigned int input_ctrl;
}devfreq_config;

struct kbase_platform_config *kbase_get_platform_config(void);

struct device* kbase_get_platform_device(void);

void* kbase_get_devfreq_config(void);

int kbase_get_core_num(int core_mask);

#endif /* _KBASE_CONFIG_DEVICETREE_H_ */

