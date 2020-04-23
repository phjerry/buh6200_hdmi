/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: gpio api functions
 */

#include "gpio.h"
#include <io8051.h>
#include <bitdef.h>
#include "base.h"
#include "lpmcu_ram_config.h"

static hi_void gpio_convert(hi_u8 gpio_group_num, hi_u32_data *base_addr)
{
    hi_u32_data reg_base_addr;

    if (base_addr == NULL) {
        return;
    }

    switch(gpio_group_num) {
        case AON_GPIO0:
            reg_base_addr.val32 = GPIO23_BASE_ADDR;
            break;
        case AON_GPIO1:
            reg_base_addr.val32 = GPIO24_BASE_ADDR;
            break;
        case AON_GPIO2:
            reg_base_addr.val32 = GPIO25_BASE_ADDR;
            break;
        case AON_GPIO3:
            reg_base_addr.val32 = GPIO26_BASE_ADDR;
            break;
        case AON_GPIO4:
            reg_base_addr.val32 = GPIO27_BASE_ADDR;
            break;
        default:
            return;
    }
    base_addr->val32 = reg_base_addr.val32;

    return;
}

__xdata_reentrant hi_void gpio_dirset_bit(hi_u8 gpio_group_num, hi_u8 gpio_bit_num, hi_u8 value)
{
    hi_u32_data reg_data;
    hi_u32_data reg_base_addr;

    gpio_convert(gpio_group_num, &reg_base_addr);

    reg_data.val32 = reg_get(reg_base_addr.val32 + GPIO_REG_DIR);
    if (value) {
        /* input */
        reg_data.val8[0] &= ~(1 << gpio_bit_num); /* (0 th) element */
    } else {
        /* output */
        reg_data.val8[0] |= (1 << gpio_bit_num); /* (0 th) element */
    }

    reg_set(reg_base_addr.val32 + GPIO_REG_DIR, reg_data.val32);

    return;
}

__xdata_reentrant hi_void gpio_write_bit(hi_u8 gpio_group_num, hi_u8 gpio_bit_num, hi_u8 value)
{
    hi_u32_data reg_data;
    hi_u32_data reg_base_addr;

    gpio_convert(gpio_group_num, &reg_base_addr);

    reg_base_addr.val32 += (0x4 << gpio_bit_num);

    if (value) {
        reg_data.val32 = 0;
        reg_data.val8[0] = (1 << gpio_bit_num); /* (0 th) element */
    } else {
        reg_data.val32 = 0;
    }

    reg_set(reg_base_addr.val32, reg_data.val32);

    return;
}

hi_void gpio_read_bit(hi_u8 gpio_group_num, hi_u8 gpio_bit_num, hi_u8* value)
{
    hi_u8 tmp = 0;
    hi_u32_data reg_addr;
    hi_u32_data reg_data;
    hi_u32_data reg_base_addr;

    gpio_convert(gpio_group_num, &reg_base_addr);
    reg_data.val32 = reg_get(reg_base_addr.val32 + GPIO_REG_DIR);
    tmp = reg_data.val8[0] & (1 << gpio_bit_num); /* (0 th) element of array */

    if (tmp) {
        *value = 0xff;
        return;
    }

    reg_addr.val32 = reg_base_addr.val32 + (0x4 << gpio_bit_num);
    reg_data.val32 = reg_get(reg_addr.val32);
    tmp = reg_data.val8[0] & (0x1 << gpio_bit_num); /* (0 th) element of array */

    if (tmp) {
        *value = 1;
    } else {
        *value = 0;
    }

    return;
}

hi_void gpio_set_edge_trigger_type(hi_u32_data reg_base_addr, hi_u8 gpio_bit_num, hi_u8 gpio_type)
{
    hi_u8 tmp;
    hi_u32_data reg_data;

    reg_data.val32 = reg_get(reg_base_addr.val32 + GPIO_REG_IS);
    tmp = reg_data.val8[0] & (~(0x1 << gpio_bit_num)); /* 0: edge (0 th) element */
    reg_data.val8[0] = tmp; /* (0 th) element of the array */
    reg_set(reg_base_addr.val32 + GPIO_REG_IS, reg_data.val32);

    reg_data.val32 = reg_get(reg_base_addr.val32 + GPIO_REG_IBE);

    if (gpio_type == HI_C51_GPIO_INTTYPE_UP) {
        tmp = reg_data.val8[0] & (~(1 << gpio_bit_num)); /* single edge trigger */
        reg_data.val8[0] = tmp; /* the 0nd element of the array */
        reg_set(reg_base_addr.val32 + GPIO_REG_IBE, reg_data.val32);

        reg_data.val32 = reg_get(reg_base_addr.val32 + GPIO_REG_IEV);
        tmp = reg_data.val8[0] | (1 << gpio_bit_num); /* up edge */
        reg_data.val8[0] = tmp;
        reg_set(reg_base_addr.val32 + GPIO_REG_IEV, reg_data.val32);
    } else if (gpio_type == HI_C51_GPIO_INTTYPE_DOWN) {
        tmp = reg_data.val8[0] & (~(1 << gpio_bit_num)); /* single edge trigger */
        reg_data.val8[0] = tmp;
        reg_set(reg_base_addr.val32 + GPIO_REG_IBE, reg_data.val32);

        reg_data.val32 = reg_get(reg_base_addr.val32 + GPIO_REG_IEV);
        tmp = reg_data.val8[0] & (~(1 << gpio_bit_num)); /* down edge */
        reg_data.val8[0] = tmp;
        reg_set(reg_base_addr.val32 + GPIO_REG_IEV, reg_data.val32);
    } else {
        tmp |= reg_data.val8[0] | (1 << gpio_bit_num); /* double edge */
        reg_data.val8[0] = tmp;
        reg_set(reg_base_addr.val32 + GPIO_REG_IBE, reg_data.val32);
    }
    return;
}

hi_void gpio_set_interrupt_type(hi_u8 gpio_group_num, hi_u8 gpio_bit_num, hi_u8 gpio_type)
{
    hi_u8 tmp = 0;
    hi_u32_data reg_base_addr;
    hi_u32_data reg_data;

    /* get gpioAON number 0-7 */
    gpio_convert(gpio_group_num, &reg_base_addr);

    reg_data.val32 = reg_get(reg_base_addr.val32 + GPIO_REG_DIR);
    tmp = reg_data.val8[0] & (~(0x1 << gpio_bit_num)); /* 0: input (0 th) element */
    reg_data.val8[0] = tmp;
    reg_set(reg_base_addr.val32 + GPIO_REG_DIR, reg_data.val32);

    if (gpio_type <= HI_C51_GPIO_INTTYPE_UPDOWN) { /* edge trigger */
        gpio_set_edge_trigger_type(reg_base_addr, gpio_bit_num, gpio_type);
    } else if (gpio_type > HI_C51_GPIO_INTTYPE_UPDOWN) { /* level trigger */
        reg_data.val32 = reg_get(reg_base_addr.val32 + GPIO_REG_IS);
        tmp = reg_data.val8[0] | (0x1 << gpio_bit_num); /* 1: level (0 th) element */
        reg_data.val8[0] = tmp; /* (0 th) element of the array */
        reg_set(reg_base_addr.val32 + GPIO_REG_IS, reg_data.val32);

        if (gpio_type == HI_C51_GPIO_INTTYPE_HIGH) {
            reg_data.val32 = reg_get(reg_base_addr.val32 + GPIO_REG_IEV);
            tmp = reg_data.val8[0] | (0x1 << gpio_bit_num); /*  high level trigger */
            reg_data.val8[0] = tmp; /* (0 th) element of array */
            reg_set(reg_base_addr.val32 + GPIO_REG_IEV, reg_data.val32);
        } else {
            reg_data.val32 = reg_get(reg_base_addr.val32 + GPIO_REG_IEV);
            tmp = reg_data.val8[0] & (~(1 << gpio_bit_num)); /* low level trigger */
            reg_data.val8[0] = tmp; /* (0 th) element of array */
            reg_set(reg_base_addr.val32 + GPIO_REG_IEV, reg_data.val32);
        }
    }
    return;
}

hi_void gpio_interrupt_enable(hi_u8 gpio_group_num, hi_u8 gpio_bit_num, hi_bool enable)
{
    hi_u8 tmp = 0;
    hi_u32_data reg_base_addr;
    hi_u32_data reg_data;

    /* get gpio number 0-7 */
    gpio_convert(gpio_group_num, &reg_base_addr);

    if (enable == HI_TRUE) {
        reg_data.val32 = reg_get(reg_base_addr.val32 + GPIO_REG_IE);
        reg_data.val8[0] |= (0x1 << gpio_bit_num); /* 1: enable interruput, the (0 th) element */
        reg_set(reg_base_addr.val32 + GPIO_REG_IE, reg_data.val32);
    } else {
        reg_data.val32 = reg_get(reg_base_addr.val32 + GPIO_REG_IE);
        tmp = reg_data.val8[0] & (~(0x1 << gpio_bit_num)); /* 0:Mask interruput, the (0 th) element */
        reg_data.val8[0] = tmp; /* the (0 th) element of the array */
        reg_set(reg_base_addr.val32 + GPIO_REG_IE, reg_data.val32);
    }

    return;
}

hi_void gpio_isr(hi_u8 gpio_group_num, hi_u8 gpio_bit_num)
{
    hi_u8 scan_key = 0;
    hi_u8 gpio_num = 0;
    hi_u32_data reg_data;
    hi_u32_data reg_base_addr;

    /* get gpio number 0-7 */
    gpio_convert(gpio_group_num, &reg_base_addr);

    gpio_num = gpio_group_num * 8 + gpio_bit_num; /* 8bit per group */
    reg_data.val32 = reg_get(reg_base_addr.val32 + GPIO_REG_MIS);
    scan_key = reg_data.val8[0] & (0x1 << gpio_bit_num); /* 0 th element of the array */

    if (scan_key) {
        /* save wakeup GPIO to ram */
        ram_set(LPMCU_WAKEUP_GPIO, gpio_num);

        g_wakeup_type = HI_PMOC_WAKEUP_TYPE_GPIO;
        g_resume_flag = HI_TRUE;

        gpio_interrupt_enable(gpio_group_num, gpio_bit_num, HI_FALSE);
    }

    reg_data.val8[0] = (0x1 << gpio_bit_num); /* clear interrupt, 0 th element */
    reg_set(reg_base_addr.val32 + GPIO_REG_IC, reg_data.val32);

    return;
}
