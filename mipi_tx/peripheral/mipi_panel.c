/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: mipi panel module
* Author: sdk
* Create: 2019-11-20
*/
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/err.h>

#include "linux/hisilicon/securec.h"
#include "mipi_panel_func.h"
#include "mipi_panel_define.h"
#include "hi_drv_mipi.h"
#include "hi_drv_module.h"
#include "drv_gpio_ext.h"
#include "drv_mipi_tx_define.h"

mipi_panel_cmd g_power_on_cmd_0[POWER_ON_CMD_NUM_0]   = POWER_ON_CMD_0;
mipi_panel_cmd g_power_off_cmd_0[POWER_OFF_CMD_NUM_0] = POWER_OFF_CMD_0;

static struct task_struct *g_test_task = HI_NULL;

static gpio_ext_func *g_mipi_gpio_func = HI_NULL;

static hi_s32 check_power_on_cmd(panel_id id)
{
    return HI_SUCCESS;
}

static hi_s32 check_power_off_cmd(panel_id id)
{
    return HI_SUCCESS;
}

static hi_s32 threadfunc(void *data)
{
    g_mipi_gpio_func->pfn_gpio_direction_set_bit(PWM_SIM_GPIO_NO_0, 0);
    while (1) {
        if (kthread_should_stop()) {
            break;
        }

        g_mipi_gpio_func->pfn_gpio_write_bit(PWM_SIM_GPIO_NO_0, 1);
        usleep_range(300, 700); /* delay rang form 300 to 700, for tianma test */
        g_mipi_gpio_func->pfn_gpio_write_bit(PWM_SIM_GPIO_NO_0, 0);
        udelay(100); /* delay 100 us, for tianma test */
    }

    return HI_SUCCESS;
}

static hi_s32 gpio_simulate_pwm(panel_id id)
{
    hi_s32 ret;

    g_test_task = kthread_create(threadfunc, (void *)(&id), "mipi_task"); /* add mipi_task_1 */
    if (IS_ERR(g_test_task)) {
        hi_err_mipi("start kernel thread err.\n");
        ret = PTR_ERR(g_test_task);
        g_test_task = HI_NULL;
        return HI_FAILURE;
    }

    wake_up_process(g_test_task);

    return HI_SUCCESS;
}

static hi_void gpio_stop_simulate_pwm(panel_id id)
{
    if (g_test_task) {
        kthread_stop(g_test_task);

        g_test_task = HI_NULL;
    }
}

static hi_void do_lcd_en(panel_id id)
{
    if (LCD_EN_GPIO_NO_0 == -1) {
        hi_err_mipi("no need do lcd en.\n");
        return;
    }

    g_mipi_gpio_func->pfn_gpio_direction_set_bit(LCD_EN_GPIO_NO_0, 0); /* output */
    g_mipi_gpio_func->pfn_gpio_write_bit(LCD_EN_GPIO_NO_0, 0); /* down */
    mdelay(10); /* delay 10 ms */
    g_mipi_gpio_func->pfn_gpio_write_bit(LCD_EN_GPIO_NO_0, 1); /* up */
    mdelay(LCD_EN_DELAY_0);
}

static hi_void do_lcd_rst(panel_id id)
{
    if (LCD_RST_GPIO_NO_0 == -1) {
        hi_err_mipi("no need do lcd rst.\n");
        return;
    }

    g_mipi_gpio_func->pfn_gpio_direction_set_bit(LCD_RST_GPIO_NO_0, 0); /* output */
    g_mipi_gpio_func->pfn_gpio_write_bit(LCD_RST_GPIO_NO_0, 0); /* down */
    mdelay(10); /* delay 10 ms */
    g_mipi_gpio_func->pfn_gpio_write_bit(LCD_RST_GPIO_NO_0, 1); /* up */
    mdelay(LCD_RST_DELAY_0);
}

static hi_void do_lcd_de_en(panel_id id)
{
    if (LCD_EN_GPIO_NO_0 == -1) {
        hi_err_mipi("no need do lcd en.\n");
        return;
    }

    g_mipi_gpio_func->pfn_gpio_write_bit(LCD_EN_GPIO_NO_0, 0); /* down */
}

static hi_void do_lcd_de_rst(panel_id id)
{
    if (LCD_RST_GPIO_NO_0 == -1) {
        hi_err_mipi("no need do lcd rst.\n");
        return;
    }

    g_mipi_gpio_func->pfn_gpio_write_bit(LCD_RST_GPIO_NO_0, 0); /* down */
}

static hi_s32 start_pwm(panel_id id)
{
    hi_s32 ret;
    if (PWM_MODE_0 == PWM_MODE_GPIO_SIMULATIN) {
        ret = gpio_simulate_pwm(id);
        if (ret != HI_SUCCESS) {
            hi_err_mipi("start sim pwm err.\n");
        }
    } else {
        ret = -1;
    }

    return ret;
}

static hi_void stop_pwm(panel_id id)
{
    if (PWM_MODE_0 == PWM_MODE_GPIO_SIMULATIN) {
        gpio_stop_simulate_pwm(id);
    } else {
        ;
    }
}

hi_s32 get_power_on_cmd_num(panel_id id, hi_s32 *cmd_num)
{
    *cmd_num = POWER_ON_CMD_NUM_0;
    return HI_SUCCESS;
}

hi_s32 get_power_off_cmd_num(panel_id id, hi_s32 *cmd_num)
{
    *cmd_num = POWER_OFF_CMD_NUM_0;
    return HI_SUCCESS;
}

hi_s32 get_power_on_cmd(panel_id id, mipi_panel_cmd *on_cmd, hi_s32 *cmd_len)
{
    int ret;

    ret = memcpy_s(on_cmd, sizeof(g_power_on_cmd_0), g_power_on_cmd_0, sizeof(g_power_on_cmd_0));
    if (ret != HI_SUCCESS) {
        hi_err_mipi("get power on cmd err.\n");
        return ret;
    }

    *cmd_len = MAX_CMD_LEN;

    return HI_SUCCESS;
}

hi_s32 get_power_off_cmd(panel_id id, mipi_panel_cmd *off_cmd, hi_s32 *cmd_len)
{
    off_cmd = g_power_off_cmd_0;
    *cmd_len = MAX_CMD_LEN;
    return HI_SUCCESS;
}

hi_s32 panel_peripherial_init(panel_id id)
{
    hi_s32 ret;

    ret = check_power_on_cmd(id);
    if (ret != HI_SUCCESS) {
        hi_err_mipi("power_on_cmd invalid, please check again.\n");
        return ret;
    }

    ret = check_power_off_cmd(id);
    if (ret != HI_SUCCESS) {
        hi_err_mipi("power_off_cmd invalid, please check again.\n");
        return ret;
    }

    ret = hi_drv_module_get_func(HI_ID_GPIO, (hi_void **)&g_mipi_gpio_func);
    if (ret != HI_SUCCESS) {
        hi_err_mipi("call hi_drv_module_get_func err.\n");
        return ret;
    }

    do_lcd_en(id);
    do_lcd_rst(id);
    ret = start_pwm(id);
    if (ret != HI_SUCCESS) {
        hi_err_mipi("panel_init err.\n");
    }

    return ret;
}

hi_void panel_peripherial_deinit(panel_id id)
{
    if (g_mipi_gpio_func == HI_NULL) {
        hi_err_mipi("g_mipi_gpio_func not init.\n");
        return;
    }

    stop_pwm(id);
    do_lcd_de_rst(id);
    do_lcd_de_en(id);
}
