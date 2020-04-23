/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:osal_math
* Author: seg
* Create: 2019-10-11
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include "hi_osal.h"

#define MAX_GPIO_NUM (28 * 8) /* The max gpio number in 96cv 900/300  */
static unsigned int g_gpio_to_irq[MAX_GPIO_NUM] = {0};

int osal_gpio_get_direction(unsigned int gpio)
{
    int ret_val;
    struct gpio_desc *desc = NULL;

    ret_val = gpio_request(gpio, NULL);
    if (ret_val != 0) {
        osal_printk("[%s]:request gpio[No:%d] failed.\n", gpio);
        return -1;
    }

    desc = gpio_to_desc(gpio);
    if (desc == NULL) {
        osal_printk("[%s]:gpio_to_desc failed.\n", __FUNCTION__);
        gpio_free(gpio);
        return -1;
    }

    ret_val = gpiod_get_direction(desc);
    if (ret_val != 0 && ret_val != 1) {
        osal_printk("[%s]:gpiod_get_direction failed. return %d\n", __FUNCTION__, ret_val);
        gpio_free(gpio);
        return -1;
    }

    gpio_free(gpio);

    return ret_val;
}
EXPORT_SYMBOL(osal_gpio_get_direction);

int osal_gpio_set_direction(unsigned int gpio, unsigned int dir)
{
    int ret;

    if (dir != 1 && dir != 0) {
        osal_printk("set direction failed. invalid dir\n");
        return -1;
    }

    ret = gpio_request(gpio, NULL);
    if (ret != 0) {
        osal_printk("[%s]:request gpio[No:%d] failed.\n", __FUNCTION__, gpio);
        return -1;
    }

    if (dir == 1) { /* set input */
        ret = gpio_direction_input(gpio);
        if (ret != 0) {
            osal_printk("set direction input failed. gpio[No:%d], return %d\n", gpio, ret);
            goto out;
        }
    } else { /* set output */
        ret = gpio_direction_output(gpio, 0); /* default output low level */
        if (ret != 0) {
            osal_printk("set direction output failed, gpio[No:%d], return %d\n", gpio, ret);
            goto out;
        }
    }

out:
    gpio_free(gpio);

    return ret;
}
EXPORT_SYMBOL(osal_gpio_set_direction);

void osal_gpio_set_value(unsigned int gpio, int value)
{
    int ret_val;

    ret_val = gpio_request(gpio, NULL);
    if (ret_val != 0) {
        osal_printk("[%s]:request gpio[No:%d] failed.\n", gpio);
        gpio_free(gpio);
        return;
    }
    gpio_set_value(gpio, value);
    gpio_free(gpio);

    return;
}
EXPORT_SYMBOL(osal_gpio_set_value);

int osal_gpio_get_value(unsigned int gpio)
{
    int ret_val;

    ret_val = gpio_request(gpio, NULL);
    if (ret_val != 0) {
        osal_printk("[%s]:request gpio[No:%d] failed.\n", gpio);
        gpio_free(gpio);
        return -1;
    }
    ret_val = gpio_get_value(gpio);
    gpio_free(gpio);

    return ret_val;
}
EXPORT_SYMBOL(osal_gpio_get_value);

int osal_gpio_irq_request(unsigned int gpio, osal_irq_handler handler,
    unsigned long flags, const char *name, void *dev)
{
    int ret, irq;

    if (name == NULL || gpio > MAX_GPIO_NUM) {
        printk("%s - param invalid ! \n", __FUNCTION__);
        return -1;
    }

    ret = gpio_request(gpio, NULL);
    if (ret != 0) {
        osal_printk("[%s]:request gpio[No:%d] failed.\n", __FUNCTION__, gpio);
        return -1;
    }

    irq = gpio_to_irq(gpio);
    if (irq < 0) {
        osal_printk("call gpio_to_irq failed. gpio[No:%d], return %d\n", gpio, ret);
        gpio_free(gpio);
        return -1;
    }
    g_gpio_to_irq[gpio] = irq;
    ret = request_threaded_irq(irq, (irq_handler_t)handler, NULL, flags, name, dev);
    if (ret != 0) {
        osal_printk("call request_threaded_irq failed. gpio[No:%d], return %d\n", gpio, ret);
        gpio_free(gpio);
        return -1;
    }
    gpio_free(gpio);

    return irq;
}
EXPORT_SYMBOL(osal_gpio_irq_request);

void osal_gpio_irq_free(unsigned int gpio, void *dev)
{
    unsigned int irq;

    if (gpio > MAX_GPIO_NUM) {
        osal_printk("[%s]:invalid gpio num.\n", __FUNCTION__);
        return;
    }

    if (g_gpio_to_irq[gpio] == 0) {
        osal_printk("[%s]:this gpio had not requested irq before.[GPIO:%d]\n", __FUNCTION__, gpio);
        return;
    }

    irq = g_gpio_to_irq[gpio];
    free_irq(irq, dev);
    g_gpio_to_irq[gpio] = 0;

    return;
}
EXPORT_SYMBOL(osal_gpio_irq_free);

