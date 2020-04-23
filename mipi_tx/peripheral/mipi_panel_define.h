/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: mipi panel define module
* Author: sdk
* Create: 2019-11-20
*/
#ifndef __MIPI_PANEL_H__
#define __MIPI_PANEL_H__

typedef enum {
    GPIO_LOW,
    GPIO_HIGH
} gpio_status;

#define GPIO_STATUS_HIGH 1
#define GPIO_STATUS_LOW  0

#define PWM_MODE_GPIO_SIMULATIN 0
#define PWM_MODE_PWM_CTRL  1

#define LPDT 0
#define HSDT 1

/*
*   attention The pin number is defined as follows: Pin number = GPIO group ID x 8 + GPIO pin ID in the group,
*   For example, GPIO1_2 indicates pin 2 in group 1, and the pin number is 10 (1 x 8 + 2).
*   Both pin group ID and pin number are numbered from 0.
*   Each HD chip provides GPIO pins number reference to HD chip hardware manual.
*/
#define PANEL_USESD_0        1 /* 1: useed, other: do not support */
/* for lcd en */
#define LCD_EN_GPIO_NO_0     141 /* -1: not use, other: specified gpio */
#define LCD_EN_GPIO_ACTIVE_STATUS_0  GPIO_STATUS_HIGH /* gpio status, when lcd en is active */
#define LCD_EN_DELAY_0       110 /* ms */

/* for lcd rst */
#define LCD_RST_GPIO_NO_0    172 /* -1: not use, other: specified gpio */
#define LCD_RST_GPIO_ACTIVE_STATUS_0 GPIO_STATUS_HIGH /* gpio status, when lcd rst is active */
#define LCD_RST_DELAY_0       175 /* ms */

/* for pwm(used for panel backlight) */
#define PWM_MIN_FREQUENCY_0  30
#define PWM_MAX_FREQUENCY_0  30

#define PWM_MODE_0           PWM_MODE_GPIO_SIMULATIN

/* for pwm ---->>>> PWM_MODE_GPIO_SIMULATIN */
#define PWM_SIM_GPIO_NO_0     173 /* -1: not use, other: specified gpio */

/* for pwm ---->>>> PWM_MODE_PWM_CTRL */
/* add later */
/* for panel init cmd */
#define POWER_ON_CMD_NUM_0 2
#define POWER_ON_CMD_0 {                                                 \
    /* {cmd_type, data_type, cmd_len, p0, p1...} */                      \
    {                                                                    \
        LPDT, 0x05, 120, 1, /* cmd_type/data_type/delay(ms)/cmd_len */   \
        /* cmd: each line must contain 20 parameters. */                 \
        {0x11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
    },                                                                   \
    {                                                                    \
        LPDT, 0x05, 10, 1, /* cmd_type/data_type/delay(ms)/cmd_len */    \
        /* cmd: each line must contain 20 parameters. */                 \
        {0x29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
    },                                                                   \
}
#define POWER_OFF_CMD_NUM_0 0
#define POWER_OFF_CMD_0 { \
                          \
}

#endif