
#ifndef _KBASE_RUNTIME_PM_H_
#define _KBASE_RUNTIME_PM_H_

int kbase_init_mask_freq_volt(struct kbase_device *kbdev);

void kbase_init_min_max_freq(struct kbase_device *kbdev);

int kbase_get_power_status(void);

#endif /* _KBASE_RUNTIME_PM_H_ */


