/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2010-2019. All rights reserved.
* Description:drv of gpio
*/
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/poll.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/miscdevice.h>
#include <linux/printk.h>
#include <linux/gpio/driver.h>
#include <linux/list.h>
#include <linux/semaphore.h>
#include <linux/version.h>

#include "drv_gpio_ioctl.h"
#include "hi_drv_sys.h"
#include "hi_module.h"
#include "hi_drv_module.h"
#include "hi_reg_common.h"
#include "drv_gpio_ext.h"
#include "drv_gpio.h"
#include "hi_errno.h"

#define  GPIO_MAX_BUF 256
#define  GPIO_BUF_HEAD g_gpio_attr.gpio_irq_list[g_gpio_attr.head]
#define  GPIO_BUF_TAIL g_gpio_attr.gpio_irq_list[g_gpio_attr.tail]
#define  INC_BUF(x, len) (((x) + 1) % (len))

typedef struct {
    hi_bool                gpio_interrupt_enable;
    hi_gpio_interrupt_type gpio_interrupt_type;
    hi_void (*gpio_server) (hi_u32 gpio_num);
} gpio_interrupt_attr;

typedef struct {
    hi_u32              head; /* gpio interrupt list */
    hi_u32              tail;
    hi_u32              gpio_irq_list[GPIO_MAX_BUF];
    hi_u32              gpio_irq_list_size;
    hi_u32              gpio_wait_timeout;
    osal_wait           gpio_interrupt_wait_queue; /* gpio wait queue */
    hi_size_t           gpio_request[GPIO_MAX_BUF];
    gpio_interrupt_attr gpio_irq_attr[GPIO_MAX_BUF];
    osal_semaphore      mutex;
} gpio_attr;

static gpio_get_gpio_num g_gpio_num;
static osal_atomic g_gpio_init_counter = { HI_NULL };
static gpio_attr g_gpio_attr;

hi_s32 hi_drv_gpio_pm_suspend(hi_void *private_data)
{
    HI_PRINT("GPIO suspend OK\n");
    return 0;
}

hi_s32 hi_drv_gpio_pm_resume(hi_void *private_data)
{
    HI_PRINT("GPIO resume OK\n");
    return 0;
}

static gpio_ext_func g_gpio_export_funcs = {
    .pfn_gpio_direction_get_bit       = hi_drv_gpio_get_direction_bit,
    .pfn_gpio_direction_set_bit       = hi_drv_gpio_set_direction_bit,
    .pfn_gpio_read_bit                = hi_drv_gpio_read_bit,
    .pfn_gpio_write_bit               = hi_drv_gpio_write_bit,
    .pfn_gpio_get_num                 = hi_drv_gpio_get_gpio_num,
    .pfn_gpio_register_server_func    = hi_drv_gpio_register_server_func,
    .pfn_gpio_unregister_server_func  = hi_drv_gpio_unregister_server_func,
    .pfn_gpio_set_interrupt_type      = hi_drv_gpio_set_interrupt_type,
    .pfn_gpio_set_interrupt_enable    = hi_drv_gpio_set_bit_interrupt_enable,
    .pfn_gpio_clear_group_interrupt   = hi_drv_gpio_clear_group_interrupt,
    .pfn_gpio_clear_bit_interrupt     = hi_drv_gpio_clear_bit_interrupt,
    .pfn_gpio_suspend                 = hi_drv_gpio_pm_suspend,
    .pfn_gpio_resume                  = hi_drv_gpio_pm_resume
};

hi_s32 hi_drv_gpio_set_direction_bit(hi_u32 gpio_num, hi_u32 dir_bit)
{
    hi_s32 ret = HI_SUCCESS;

    ret = osal_gpio_set_direction(gpio_num, dir_bit);
    if (ret != HI_SUCCESS) {
        HI_ERR_GPIO("osal_gpio_set_direction failed! \n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 hi_drv_gpio_get_direction_bit(hi_u32 gpio_num, hi_u32 *pdir_bit)
{
    hi_s32 status = HI_FAILURE;

    status = osal_gpio_get_direction(gpio_num);
    if (status == HI_FAILURE) {
        HI_ERR_GPIO("osal_gpio_get_direction failed! \n");
        return HI_FAILURE;
    }

    if (status == GPIOF_DIR_IN) {
        *pdir_bit = HI_TRUE;
    } else if (status == GPIOF_DIR_OUT) {
        *pdir_bit = HI_FALSE;
    } else {
        HI_ERR_GPIO("Get gpio%d direction failed:%d\n", gpio_num, status);
        return HI_ERR_GPIO_FAILED_GETDIRECT;
    }

    return HI_SUCCESS;
}

hi_s32 hi_drv_gpio_write_bit(hi_u32 gpio_num, hi_u32 bit_value)
{
    hi_s32 status = HI_FAILURE;

    if ((bit_value != HI_TRUE) && (bit_value != HI_FALSE)) {
        HI_ERR_GPIO("Invalid WriteBit:%d\n", bit_value);
        return HI_ERR_GPIO_INVALID_PARA;
    }

    status = osal_gpio_get_direction(gpio_num);
    if (status != GPIOF_DIR_OUT) {
        HI_ERR_GPIO("Input direction, write denied\n");
        return HI_ERR_GPIO_INVALID_OPT;
    }

    osal_gpio_set_value(gpio_num, bit_value);

    return HI_SUCCESS;
}

hi_s32 hi_drv_gpio_read_bit(hi_u32 gpio_num, hi_u32 *pbit_value)
{
    *pbit_value = osal_gpio_get_value(gpio_num);
    return HI_SUCCESS;
}

/*
 * u32GpioGroup: gpio group number
 * u32BitX:      gpio bit offset
 * interrupt_trigger_mode: interruput type ,please see hi_gpio_interrupt_type
 */
hi_s32 hi_drv_gpio_set_interrupt_type(hi_u32 gpio_num, hi_gpio_interrupt_type interrupt_trigger_mode)
{
    if (gpio_num >= g_gpio_num.gpio_max_num) {
        HI_INFO_GPIO("Invalid parameter, u32GpioNum:%d\n", gpio_num);
        return HI_ERR_GPIO_INVALID_PARA;
    }

    if (interrupt_trigger_mode >= HI_GPIO_INTTYPE_MAX) {
        HI_INFO_GPIO("Invalid parameter, interrupt_trigger_mode:%d\n", interrupt_trigger_mode);
        return HI_ERR_GPIO_INVALID_PARA;
    }

    g_gpio_attr.gpio_irq_attr[gpio_num].gpio_interrupt_type = interrupt_trigger_mode;

    return HI_SUCCESS;
}

static hi_s32 gpio_isr(hi_s32 irq, hi_void *gpio_number)
{
    hi_size_t gpio_num;
    gpio_num = (hi_size_t)(uintptr_t)gpio_number;

    if (g_gpio_attr.gpio_irq_attr[gpio_num].gpio_interrupt_type == HI_GPIO_INTTYPE_HIGH ||
        g_gpio_attr.gpio_irq_attr[gpio_num].gpio_interrupt_type == HI_GPIO_INTTYPE_LOW) {
        disable_irq_nosync(gpio_to_irq(gpio_num));
        if (g_gpio_attr.gpio_irq_attr[gpio_num].gpio_interrupt_enable == HI_FALSE) {
            HI_ERR_GPIO("error irq! \n");
        }
    }

    if (g_gpio_attr.gpio_irq_attr[gpio_num].gpio_interrupt_enable == HI_TRUE) {
        if (g_gpio_attr.gpio_irq_attr[gpio_num].gpio_server != HI_NULL) {
            g_gpio_attr.gpio_irq_attr[gpio_num].gpio_server(gpio_num);
            HI_INFO_GPIO("gpio[%d].gpio_server executed\n", gpio_num);
        }

        HI_INFO_GPIO("Recive gpio interrupt: GPIO%d. \n", gpio_num);

        GPIO_BUF_HEAD = gpio_num;
        g_gpio_attr.head = INC_BUF(g_gpio_attr.head, g_gpio_attr.gpio_irq_list_size);
        osal_wait_wakeup(&(g_gpio_attr.gpio_interrupt_wait_queue));
    }

    return OSAL_IRQ_HANDLED;
}

hi_s32 hi_drv_gpio_set_bit_interrupt_enable(hi_u32 gpio_num, hi_bool enable)
{
    hi_u32 irq;
    hi_u32 flags = 0;
    hi_u32 irq_type;
    hi_s32 ret = HI_SUCCESS;

    if (enable) { /* not marsk */
        ret = osal_gpio_set_direction(gpio_num, 1); /* set direction input */
        if (ret != HI_SUCCESS) {
            HI_ERR_GPIO("set gpio %d input direction failed ! \n", gpio_num);
            return HI_FAILURE;
        }

        irq_type = g_gpio_attr.gpio_irq_attr[gpio_num].gpio_interrupt_type;

        switch (irq_type) {
            case HI_GPIO_INTTYPE_UP:
                flags |= IRQF_TRIGGER_RISING;
                break;
            case HI_GPIO_INTTYPE_DOWN:
                flags |= IRQF_TRIGGER_FALLING;
                break;
            case HI_GPIO_INTTYPE_UPDOWN:
                flags |= IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING;
                break;
            /* IRQF_TRIGGER_HIGH or IRQF_TRIGGER_LOW will trigger
             * continuously interrupts, must avoid it
             */
            case HI_GPIO_INTTYPE_HIGH:
                flags |= (IRQF_TRIGGER_HIGH | IRQF_ONESHOT);
                break;
            case HI_GPIO_INTTYPE_LOW:
                flags |= (IRQF_TRIGGER_LOW | IRQF_ONESHOT);
                break;
            default:
                flags = IRQF_TRIGGER_RISING;
                break;
        }

        if ((flags == IRQF_TRIGGER_HIGH || flags == IRQF_TRIGGER_LOW) &&
            (g_gpio_attr.gpio_irq_attr[gpio_num].gpio_interrupt_enable == HI_TRUE)) {
            osal_gpio_irq_free(gpio_num, (hi_void *)((uintptr_t)gpio_num));
            g_gpio_attr.gpio_irq_attr[gpio_num].gpio_interrupt_enable = HI_FALSE;
        }

        irq = osal_gpio_irq_request(gpio_num, gpio_isr, flags, "gpio", (hi_void *)((uintptr_t)gpio_num));
        if (irq == HI_FAILURE) {
            HI_ERR_GPIO("Request_irq irq%d on gpio%d failed: %d\n", irq, gpio_num, ret);
            g_gpio_attr.gpio_irq_attr[gpio_num].gpio_interrupt_enable = HI_FALSE;
            return HI_ERR_GPIO_FAILED_SETINT;
        }
        g_gpio_attr.gpio_irq_attr[gpio_num].gpio_interrupt_enable = HI_TRUE;
    } else { /*  marsk  */
        if (g_gpio_attr.gpio_irq_attr[gpio_num].gpio_interrupt_enable == HI_TRUE) {
            osal_gpio_irq_free(gpio_num, (hi_void *)((uintptr_t)gpio_num));
            g_gpio_attr.gpio_irq_attr[gpio_num].gpio_interrupt_enable = HI_FALSE;
        }
    }
    return HI_SUCCESS;
}

hi_s32 hi_drv_gpio_register_server_func(hi_u32 gpio_num, hi_void(*func)(hi_u32))
{
    if (gpio_num >= g_gpio_num.gpio_max_num) {
        HI_INFO_GPIO("Invalid parameter, u32GpioNum:%d\n", gpio_num);
        return HI_FAILURE;
    }

    if (func == HI_NULL) {
        HI_INFO_GPIO("Register func para is null, u32GpioNum%d \n", gpio_num);
        return HI_FAILURE;
    }

    if (g_gpio_attr.gpio_irq_attr[gpio_num].gpio_server != HI_NULL) {
        HI_INFO_GPIO("GPIO %d had registered gpio server pragram \n", gpio_num);
        return HI_FAILURE;
    }

    g_gpio_attr.gpio_irq_attr[gpio_num].gpio_server = func;
    HI_INFO_GPIO("Gpio %d finished register gpio server function \n", gpio_num);

    return HI_SUCCESS;
}

hi_s32 hi_drv_gpio_unregister_server_func(hi_u32 gpio_num)
{
    if (gpio_num >= g_gpio_num.gpio_max_num) {
        HI_INFO_GPIO("Invalid parameter, u32GpioNum:%d\n", gpio_num);
        return HI_FAILURE;
    }

    g_gpio_attr.gpio_irq_attr[gpio_num].gpio_server = HI_NULL;

    return HI_SUCCESS;
}

hi_s32 hi_drv_gpio_clear_bit_interrupt(hi_u32 gpio_num)
{
    if (gpio_num >= g_gpio_num.gpio_max_num) {
        HI_INFO_GPIO("Invalid parameter, u32GpioNum:%d\n", gpio_num);
        return HI_ERR_GPIO_INVALID_PARA;
    }

    return HI_SUCCESS;
}

hi_s32 hi_drv_gpio_clear_group_interrupt(hi_u32 gpio_group)
{
    if (gpio_group >= g_gpio_num.gpio_group_num) {
        HI_ERR_GPIO("Invalid parameter, gpio_group:%u\n", gpio_group);
        return HI_ERR_GPIO_INVALID_PARA;
    }

    return HI_SUCCESS;
}

hi_s32 hi_drv_gpio_get_gpio_num(gpio_get_gpio_num *gpio_num)
{
    gpio_num->gpio_group_num = HI_GPIO_GROUP_NUM;
    gpio_num->gpio_max_num = HI_GPIO_MAX_BIT_NUM;

    return HI_SUCCESS;
}

hi_s32 drv_gpio_open(hi_void *private_data)
{
    return HI_SUCCESS;
}


hi_s32 drv_gpio_close(hi_void *private_data)
{
    hi_s32 ret, i;

    if (osal_atomic_read(&g_gpio_init_counter) <= 0) {
        HI_ERR_GPIO("gpio not initialized\n");
        return HI_FAILURE;
    }

    ret = osal_sem_down_interruptible(&g_gpio_attr.mutex);
    if (ret) {
        HI_ERR_GPIO("osal_sem_down_interruptible:%d\n", ret);
        return HI_FAILURE;
    }

    for (i = 0; i < GPIO_MAX_BUF; i++) {
        if (g_gpio_attr.gpio_irq_attr[i].gpio_interrupt_enable == HI_TRUE) {
            osal_gpio_irq_free(i, (hi_void *)((uintptr_t)i));
            g_gpio_attr.gpio_irq_attr[i].gpio_interrupt_enable = HI_FALSE;
        }
    }

    osal_sem_up(&g_gpio_attr.mutex);

    return HI_SUCCESS;
}
hi_s32 drv_gpio_query_interrupt_wait_condition(const hi_void *para)
{
    return (g_gpio_attr.head != g_gpio_attr.tail);
}

hi_s32 drv_gpio_query_interrupt(gpio_interrupt *pgpio_interrupt_value)
{
    hi_s32 ret = 0;
    hi_u32 gpio_interrupt_num;

    g_gpio_attr.gpio_wait_timeout = pgpio_interrupt_value->timeout_ms;

    while (g_gpio_attr.head == g_gpio_attr.tail) {

        if (g_gpio_attr.gpio_wait_timeout == 0xffffffff) {
            ret = osal_wait_interruptible(&g_gpio_attr.gpio_interrupt_wait_queue,
                                          drv_gpio_query_interrupt_wait_condition, NULL);
            if (ret < 0) {
                HI_INFO_GPIO("osal_wait_interruptible failed:%d\n", ret);
                return -ERESTARTSYS;
            }
        } else {
            ret = osal_wait_timeout_interruptible(&g_gpio_attr.gpio_interrupt_wait_queue,
                                                  drv_gpio_query_interrupt_wait_condition,
                                                  /* 1000 "ms" converted to "s" */
                                                  NULL, (hi_slong)(g_gpio_attr.gpio_wait_timeout * HZ / 1000));
            if (ret < 0) {
                HI_INFO_GPIO("osal_wait_timeout_interruptible failed:%d\n", ret);
                return -ERESTARTSYS;
            } else if (ret == 0) {
                HI_INFO_GPIO("osal_wait_timeout_interruptible timrout\n");
                return HI_ERR_GPIO_GETINT_TIMEOUT;
            }
        }
    }

    if (g_gpio_attr.head != g_gpio_attr.tail) {
        gpio_interrupt_num = GPIO_BUF_TAIL;
        g_gpio_attr.tail = INC_BUF(g_gpio_attr.tail, g_gpio_attr.gpio_irq_list_size);
        pgpio_interrupt_value->gpio_num = gpio_interrupt_num;
    }

    return HI_SUCCESS;
}

hi_s32 drv_gpio_set_output_type(hi_u32 gpio_num, hi_gpio_outputtype output_type)
{
    return HI_SUCCESS;
}

hi_s32 drv_gpio_get_output_type(hi_u32 gpio_num, hi_gpio_outputtype  *poutput_type)
{
    return HI_SUCCESS;
}

hi_s32 drv_gpio_osal_resource_init(hi_void)
{
    hi_s32 ret = HI_SUCCESS;

    ret = osal_sem_init(&g_gpio_attr.mutex, 1);
    if (ret != HI_SUCCESS) {
        HI_ERR_GPIO("osal_sem_init failed! \n");
        return HI_FAILURE;
    }

    ret = osal_atomic_init(&g_gpio_init_counter);
    if (ret != HI_SUCCESS) {
        HI_ERR_GPIO("osal_atomic_init failed! \n");
        goto err0;
    }

    ret = osal_wait_init(&g_gpio_attr.gpio_interrupt_wait_queue);
    if (ret != HI_SUCCESS) {
        HI_ERR_GPIO("osal_wait_init failed! \n");
        goto err1;
    }
    return HI_SUCCESS;
err1:
    osal_atomic_destory(&g_gpio_init_counter);
err0:
    osal_sem_destory(&g_gpio_attr.mutex);
    return HI_FAILURE;
}

hi_s32 hi_drv_gpio_init(hi_void)
{
    hi_s32 ret;
    hi_u32 i;

    ret = drv_gpio_osal_resource_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_GPIO("drv_gpio_osal_resource_init failed! \n");
        return HI_FAILURE;
    }

    if (osal_atomic_inc_return(&g_gpio_init_counter) != 1) {
        HI_WARN_GPIO(" HI_DRV_GPIO already registered:%d\n",
                     osal_atomic_read(&g_gpio_init_counter));
        return HI_SUCCESS;
    }

    ret = osal_exportfunc_register(HI_ID_GPIO, "HI_GPIO", (hi_void *)&g_gpio_export_funcs);
    if (ret != HI_SUCCESS) {
        HI_ERR_GPIO(" GPIO Module register failed 0x%x.\n", ret);
        return HI_FAILURE;
    }

    g_gpio_attr.head = 0;
    g_gpio_attr.tail = 0;
    g_gpio_attr.gpio_irq_list_size = GPIO_MAX_BUF;
    g_gpio_attr.gpio_wait_timeout = 0xffffffff;

    ret = hi_drv_gpio_get_gpio_num(&g_gpio_num);
    if (ret != HI_SUCCESS) {
        HI_ERR_GPIO(" GPIO GetGpioNum failed: 0x%x\n", ret);
        return HI_FAILURE;
    }

    for (i = 0; i < GPIO_MAX_BUF; i++) {
        g_gpio_attr.gpio_irq_attr[i].gpio_interrupt_enable = HI_FALSE;
        g_gpio_attr.gpio_irq_attr[i].gpio_interrupt_type = HI_GPIO_INTTYPE_DOWN;
        g_gpio_attr.gpio_irq_attr[i].gpio_server = HI_NULL;
    }

    return HI_SUCCESS;
}

hi_void  hi_drv_gpio_deinit(hi_void)
{
    hi_u32 i;
    hi_s32 ret;

    if (osal_atomic_dec_return(&g_gpio_init_counter) != 0) {
        HI_WARN_GPIO("HI_DRV_GPIO_DeInit counter:%d\n",
                     osal_atomic_read(&g_gpio_init_counter));
        return;
    }

    for (i = 0; i < GPIO_MAX_BUF; i++) {
        if (g_gpio_attr.gpio_irq_attr[i].gpio_interrupt_enable == HI_TRUE) {
            osal_gpio_irq_free(i, (hi_void *)((uintptr_t)i));
            g_gpio_attr.gpio_irq_attr[i].gpio_interrupt_enable = HI_FALSE;
        }
        g_gpio_attr.gpio_irq_attr[i].gpio_interrupt_type = HI_GPIO_INTTYPE_DOWN;
        g_gpio_attr.gpio_irq_attr[i].gpio_server = HI_NULL;
    }

    ret = osal_exportfunc_unregister(HI_ID_GPIO);
    if (ret != HI_SUCCESS) {
        HI_ERR_GPIO(" GPIO Module unregister failed 0x%x.\n", ret);
    }

    osal_atomic_destory(&g_gpio_init_counter);
    osal_sem_destory(&g_gpio_attr.mutex);
    osal_wait_destroy(&g_gpio_attr.gpio_interrupt_wait_queue);
    return;
}

#ifdef MODULE
EXPORT_SYMBOL(hi_drv_gpio_init);
EXPORT_SYMBOL(hi_drv_gpio_deinit);
#endif

EXPORT_SYMBOL(hi_drv_gpio_get_direction_bit);
EXPORT_SYMBOL(hi_drv_gpio_set_direction_bit);
EXPORT_SYMBOL(hi_drv_gpio_write_bit);
EXPORT_SYMBOL(hi_drv_gpio_read_bit);
EXPORT_SYMBOL(hi_drv_gpio_get_gpio_num);
EXPORT_SYMBOL(hi_drv_gpio_register_server_func);
EXPORT_SYMBOL(hi_drv_gpio_unregister_server_func);
EXPORT_SYMBOL(hi_drv_gpio_set_interrupt_type);
EXPORT_SYMBOL(hi_drv_gpio_set_bit_interrupt_enable);
EXPORT_SYMBOL(hi_drv_gpio_clear_group_interrupt);
EXPORT_SYMBOL(hi_drv_gpio_clear_bit_interrupt);
