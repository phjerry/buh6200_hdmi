
#ifndef _KBASE_VOLTAGE_UPDATE_H_
#define _KBASE_VOLTAGE_UPDATE_H_

int kbase_voltage_update(unsigned long freq, unsigned long *volt);

int kbase_voltage_update_init(void);

int kbase_voltage_update_deinit(void);

#endif /* _KBASE_VOLTAGE_UPDATE_H_ */

