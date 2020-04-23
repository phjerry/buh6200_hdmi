/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: lpmcu start function
 */

#include "base.h"
#include "gpio.h"
#include "keyled.h"
#include "ir.h"
#include "timer.h"
#include "REG51.h"
#include "lpmcu_ram_config.h"
#include "mcu_hdmitx_cec.h"

static hi_u8 g_chip_type = 0;
static hi_u16 g_chip_version = 0;
static hi_u8 g_suspend_type = PASSIVE_STANDBY;
static hi_u32_data g_gpio_wakeup_port;
static hi_u32_data g_gpio_iev; /* 0:Falling edge or low level; 1: Rising edge or high level */
static hi_u32_data g_gpio_ibe; /* 0:single edge; 1:double edge */
static hi_u32_data g_gpio_is;  /* 0:edge; 1: level */

static hi_u8 g_time_wakeup_enable = 0;
static hi_u8 g_ir_wakeup_enable = 0;

static hi_u8 g_keyled_wakeup_enable = 0;
static hi_u8 g_keyled_type = 0;
static hi_u8 g_cec_wakeup_enable = 0;
static hi_bool g_hdmitx_cec_wakeup_enable[LPMCU_WAKEUP_HDMITX_ID_MAXNUM] = {0};
static hi_u8 g_hdmitx_cec_control_type[LPMCU_WAKEUP_HDMITX_ID_MAXNUM] = {0};
static hi_u8 g_gpio_poweroff_num = 0;
static hi_u8 g_gpio_poweroff_group[LPMCU_POWEROFF_GPIO_MAXNUM] = {0};
static hi_u8 g_gpio_poweroff_bit[LPMCU_POWEROFF_GPIO_MAXNUM] = {0};
static hi_u8 g_gpio_poweroff_level[LPMCU_POWEROFF_GPIO_MAXNUM] = {0};
static hi_u8 g_resume_type;

static hi_u32 g_gpio_wakeup_enable = 0;
static hi_u32 g_gpio_wakeup_num = 0;
static hi_u8 g_gpio_wakeup_group[LPMCU_WAKEUP_GPIO_MAXNUM] = {0};
static hi_u8 g_gpio_wakeup_bit[LPMCU_WAKEUP_GPIO_MAXNUM] = {0};
static hi_u8 g_gpio_interrupt_type[LPMCU_WAKEUP_GPIO_MAXNUM] = {0};

static hi_u32 g_eth_wakeup_enable = 0;
static hi_u32 g_usb_wakeup_enable = 0;
hi_u32  g_switch_suspend_type_time = 0x0;
static hi_bool g_eth_to_passive_standby = HI_FALSE;

static struct mcu_hdmitx_cec g_hdmitx0_cec;
static struct mcu_hdmitx_cec_resource g_hdmitx0_cec_resource = {
    .id = 0,
    .regs_base = 0x008d0c00,
    .crg_regs_base = 0x00840064,
    .sysctrl_regs_base = 0x00840400,
    .wakeup_mode = MCU_HDMITX_CEC_WAKEUP_BILATERAL
};

#define MCU_LOAD_CODE 0x12345678
#define ENTER_PMOC_FLAG 0x80510001
#define MCU_BOOT_SUSPEND_FLAG 0x12345678

#define KEYLED_TYPE_GPIOKEY 5
#define GPIO_WAKEUP_PORT_NUM 4
#define GPIO_POWEROFF_PORT_NUM 4

static hi_void system_power_down(hi_void);

hi_u8 int_status, int_status1, int_status2, int_status3;

/*
 * bit 1 timer_int
 * bit 2 uart0_int
 * bit 3 ir_int
 * bit 4 ledc_int
 * bit 5 aon_gpio0_int
 * bit 6 aon_gpio1_int
 * bit 7 aon_gpio2_int
 * bit 8 aon_gpio3_int
 * bit 9 aon_gpio4_int
 * bit 23 usb_int
 * bit 27 net_int
 * bit 28 hdmirx_2pl_aon_intr
 * bit 29 tx_aon_intr_out
 */

#pragma vector = 0x23
__interrupt __root hi_void intr_process()
{
    hi_u8 i;

    if (g_resume_flag != HI_FALSE) {
        return;
    }

    EA = 0;
    INT_MASK_0 = 0x0;
    INT_MASK_1 = 0x0;
    INT_MASK_2 = 0x0;
    INT_MASK_3 = 0x0;

    int_status  = INT_STATUS_IP0;
    int_status1 = INT_STATUS_IP1;
    int_status2 = INT_STATUS_IP2;
    int_status3 = INT_STATUS_IP3;

    /* bit 1: timer_int */
    if (int_status & 0x02) {
        timer_isr();

        if ((g_switch_suspend_type_time == 0) && (g_eth_to_passive_standby == HI_TRUE)) {
            g_suspend_type = PASSIVE_STANDBY;
            system_power_down();
        }
    }

    /* bit 3: ir_int */
    if (g_ir_wakeup_enable && (int_status & 0x08)) {
        printf_str("ir_isr... \r\n");
        ir_isr();
    }

    /* bit 4: keyled_int */
#if (defined KEYLED_CT1642_INNER)
    if ((int_status & 0x10)) {
#endif
        if (g_keyled_wakeup_enable && (g_keyled_type != KEYLED_TYPE_GPIOKEY)) {
            keyled_isr();
        }
#if (defined KEYLED_CT1642_INNER)
    }
#endif

    /* bit 5~9: gpio_int */
    if (g_gpio_wakeup_enable && ((int_status & 0xe0) || (int_status1 & 0x3))) {
        for (i = 0; i < g_gpio_wakeup_num; i++) {
            gpio_isr(g_gpio_wakeup_group[i], g_gpio_wakeup_bit[i]);
            if (g_resume_flag == HI_TRUE) {
                break;
            }
        }
    }

    /* bit 23: usb_int */
    if (g_usb_wakeup_enable && (int_status2 & 0x80)) {
        printf_str("usb int... \r\n");

        g_wakeup_type = HI_PMOC_WAKEUP_TYPE_USB;
        g_resume_flag = HI_TRUE;
    }

    /* bit 27: net_int */
    if (g_eth_wakeup_enable && (int_status3 & 0x08)) {
        printf_str("net int... \r\n");

        g_wakeup_type = HI_PMOC_WAKEUP_TYPE_ETH;
        g_resume_flag = HI_TRUE;
    }

    /* bit 28 hdmirx_2pl_aon_intr */
    if ((int_status3 & 0x10) && (g_cec_wakeup_enable == HI_TRUE)) {
        /* MCU_HDMI_TaskIrqHandle(); */
    }

    if ((int_status3 & MCU_HDMITX_CEC_IRQ_MASK) && (g_hdmitx_cec_wakeup_enable[0] == HI_TRUE)) {
        mcu_hdmitx_cec_irq_handle(&g_hdmitx0_cec);
    }

    if (g_resume_flag == HI_FALSE) {
        INT_MASK_0 = 0xfa;
        INT_MASK_1 = 0x03;
        INT_MASK_2 = 0x80;
        INT_MASK_3 = 0x38;
    }

    EA = 1;

    return;
}

static hi_void lpmcu_init(hi_void)
{
    EA = 0;

    ARM_INTR_MASK = 0xff; /* intr mask */
    INT_MASK_0 = 0xfa;    /* timer/ir/ledc/gpio0/gpio1/gpio2 intr */
    INT_MASK_1 = 0x03;    /* gpio3/gpio4 intr */
    INT_MASK_2 = 0x80;    /* usb intr */
    INT_MASK_3 = 0x38;    /* net intr */

    RI = 0;
    TI = 0;
    ES = 1;

    g_resume_flag = HI_FALSE;

    return;
}

static hi_void printf_suspend_params(hi_void)
{
    hi_u8 i;

    printf_str("======== [96cv300 LPMCU PARAMS ] ======== \r\n");
    printf_str("chip type:");
    printf_hex(g_chip_type);
    printf_str("chip version:");
    printf_hex(g_chip_version);
    printf_str("suspend type:");
    printf_hex(g_suspend_type);
    printf_str("debug mask:");
    printf_hex(g_dbg_mask);
    printf_str("eth wakeup enable:");
    printf_hex(g_eth_wakeup_enable);
    printf_str("eth wakeup mode to passive standby time:");
    printf_hex(g_switch_suspend_type_time);
    printf_str("usb wakeup enable:");
    printf_hex(g_usb_wakeup_enable);
    printf_str("hdmi cec wakeup enable:");
    printf_hex(g_cec_wakeup_enable);
    printf_str("gpio wakeup enable:");
    printf_hex(g_gpio_wakeup_enable);
    printf_str("gpio wakeup number:");
    printf_hex(g_gpio_wakeup_num);
    for (i = 0; i < g_gpio_wakeup_num; i++) {
        printf_str("gpio wakeup group:");
        printf_hex(g_gpio_wakeup_group[i]);
        printf_str("gpio wakeup bit:");
        printf_hex(g_gpio_wakeup_bit[i]);
    }
    printf_str("gpio poweroff number:");
    printf_hex(g_gpio_poweroff_num);
    for (i = 0; i < g_gpio_poweroff_num; i++) {
        printf_str("gpio poweroff group:");
        printf_hex(g_gpio_poweroff_group[i]);
        printf_str("gpio poweroff bit:");
        printf_hex(g_gpio_poweroff_bit[i]);
    }

    printf_str("keyled wakeup enable:");
    printf_hex(g_keyled_wakeup_enable);
    printf_str("keyled wakeup type:");
    printf_hex(g_keyled_type);
    printf_str("keyled wakeup key:");
    printf_hex(g_keyled_wakeup_key);
    printf_str("display mode:");
    printf_hex(g_keyled_display_type);
    if (g_keyled_display_type == TIME_DISPLAY) {
        printf_str("display time hour:");
        printf_hex(g_time_hour);
        printf_str("display time minute:");
        printf_hex(g_time_minute);
        printf_str("display time second:");
        printf_hex(g_time_second);
        printf_str("standby period:");
    }

    printf_str("timer wakeup enable:");
    printf_hex(g_time_wakeup_enable);
    printf_str("suspend period:");
    printf_hex(g_wait_time.val32);
    printf_str("ir type:");
    printf_hex(g_ir_type);
    printf_str("ir number:");
    printf_hex(g_ir_pmocnum);

    for (i = 0; i < g_ir_pmocnum; i++) {
        printf_str("ir low key:");
        printf_hex(g_ir_high_value[i].val32);
        printf_str("ir high key:");
        printf_hex(g_ir_low_value[i].val32);
    }

    printf_str("======== [MCU Init Ready] ======== \r\n");

    return;
}

static hi_void get_suspend_params(hi_void)
{
    hi_u32_data reg_data;
    hi_u8 i;

    g_chip_type = ram_get(LPMCU_CHIP_TYPE_ADDR);
    g_chip_version = ram_get(LPMCU_CHIP_VERSION_ADDR);

    reg_data.val32 = ram_get(LPMCU_ETH_SUSPEND_PARAM_ADDR);
    g_eth_wakeup_enable = reg_data.val8[0];
    g_switch_suspend_type_time = ram_get(LPMCU_ETH_TO_PASSIVE_STANDBY_ADDR);
    if ((g_switch_suspend_type_time != 0) && (g_eth_wakeup_enable != HI_FALSE)) {
        g_eth_to_passive_standby = HI_TRUE;
    }

    reg_data.val32 = ram_get(LPMCU_USB_SUSPEND_PARAM_ADDR);
    g_usb_wakeup_enable = reg_data.val8[0];

    reg_data.val32 = ram_get(LPMCU_HDMI_SUSPEND_PARAM_ADDR);
    g_cec_wakeup_enable = reg_data.val8[0];

    for (i = 0;i < LPMCU_WAKEUP_HDMITX_ID_MAXNUM; i++) {
        reg_data.val32 = ram_get(LPMCU_HDMITX_CEC_PARAM_ADDR + i * 4); /* offset: 4 byte */
        g_hdmitx_cec_wakeup_enable[i] = reg_data.val8[0];
        g_hdmitx_cec_control_type[i] = reg_data.val8[1];
    }

    reg_data.val32 = ram_get(LPMCU_GPIO_SUSPEND_PARAM_ADDR);
    g_gpio_wakeup_enable = reg_data.val8[0];
    g_gpio_wakeup_num = reg_data.val8[1];
    if (g_gpio_wakeup_num > LPMCU_WAKEUP_GPIO_MAXNUM) {
        g_gpio_wakeup_num = LPMCU_WAKEUP_GPIO_MAXNUM;
    }
    for (i = 0; i < g_gpio_wakeup_num; i++) {
        reg_data.val32 = ram_get(LPMCU_GPIO_SUSPEND_PARAM_ADDR + 4 + i * 4); /* offset: 4 byte */
        g_gpio_wakeup_group[i] = reg_data.val8[0];
        g_gpio_wakeup_bit[i] = reg_data.val8[1];
        g_gpio_interrupt_type[i] = reg_data.val8[2];
    }

    reg_data.val32 = ram_get(LPMCU_GPIO_POWEROFF_PARAM_ADDR);
    g_gpio_poweroff_num = reg_data.val8[0];
    if (g_gpio_poweroff_num > LPMCU_POWEROFF_GPIO_MAXNUM) {
        g_gpio_poweroff_num = LPMCU_POWEROFF_GPIO_MAXNUM;
    }
    for (i = 0; i < g_gpio_poweroff_num; i++) {
        reg_data.val32 = ram_get(LPMCU_GPIO_POWEROFF_PARAM0_ADDR + i * 4); /* offset: 4 byte */
        g_gpio_poweroff_group[i] = reg_data.val8[0];
        g_gpio_poweroff_bit[i] = reg_data.val8[1];
        g_gpio_poweroff_level[i] = reg_data.val8[2];
    }

    g_keyled_wakeup_enable = ram_get(LPMCU_KEYLED_SUSPEND_PARAM_ADDR);
    g_keyled_type = ram_get(LPMCU_KEYLED_TYPE_ADDR);
    g_keyled_wakeup_key = ram_get(LPMCU_KEYLED_WAKEUP_KEY_ADDR);

    /* Get timer param */
    if (g_keyled_type != KEYLED_TYPE_GPIOKEY) {
        g_keyled_display_type = ram_get(LPMCU_KEYLED_DISPLAY_ADDR);

        if (g_keyled_display_type == TIME_DISPLAY) {
            reg_data.val32 = ram_get(LPMCU_KEYLED_DISPLAY_VALUE_ADDR);

            /* timer display */
            if ((reg_data.val8[2] >= 24) ||
                (reg_data.val8[1] >= 60) ||
                (reg_data.val8[0] >= 60)) {
                /* default value */
                g_time_hour = 9;
                g_time_minute = 58;
                g_time_second = 0;
            } else {
                g_time_hour = reg_data.val8[2];
                g_time_minute = reg_data.val8[1];
                g_time_second = reg_data.val8[0];
            }
        }
    }

    /* Get the time out of wake up */
    reg_data.val32 = ram_get(LPMCU_TIMEOUT_SUSPEND_PARAM_ADDR);
    g_time_wakeup_enable = reg_data.val8[0];
    if (g_time_wakeup_enable) {
        g_wait_time.val32 = ram_get(LPMCU_TIMEOUT_SUSPEND_TIME_ADDR);
    }

    reg_data.val32 = ram_get(LPMCU_RESUME_RESET_ADDR);
    g_resume_type = reg_data.val8[0];
    /* Get the debug param */
    g_dbg_mask = reg_data.val8[1];

    reg_data.val32 = ram_get(LPMCU_IR_SUSPEND_PARAM_ADDR);
    g_ir_wakeup_enable = reg_data.val8[0];
    g_ir_type = reg_data.val8[1];
    g_ir_pmocnum = reg_data.val8[2];
    if (g_ir_pmocnum > LPMCU_WAKEUP_IRKEY_MAXNUM) {
        g_ir_pmocnum = LPMCU_WAKEUP_IRKEY_MAXNUM;
    }

    for (i = 0; i < g_ir_pmocnum; i++) {
        g_ir_low_value[i].val32 = ram_get(LPMCU_IR_LOW_VALUE_ADDR + i * 8); /* offset: 8 byte */
        g_ir_high_value[i].val32 = ram_get(LPMCU_IR_HIGH_VALUE_ADDR + i * 8); /* offset: 8 byte */
    }

    if (g_usb_wakeup_enable != 0 || g_eth_wakeup_enable != 0) {
        g_suspend_type = DEEP_ACTIVE_STANDBY;
    }

    printf_suspend_params();

    return;
}


static hi_void save_wakeup_params(hi_void)
{
    hi_u32 suspend_period = 0;

    /* save suspend period into LPMCU ram */
    suspend_period = ram_get(LPMCU_TIMEOUT_SUSPEND_TIME_ADDR) - g_wait_time.val32;
    ram_set(LPMCU_STANDBY_PERIOD, suspend_period);

    printf_str("Suspend Period:");
    printf_hex(suspend_period);

    /* save wakeup type in LPMCU ram */
    ram_set(LPMCU_WAKEUP_TYPE, g_wakeup_type);

    return;
}

static hi_void set_peripheral_power_off(hi_void)
{
    hi_u8 i;

    /* gpio power off */
    for (i = 0; i < g_gpio_poweroff_num; i++) {
        gpio_dirset_bit(g_gpio_poweroff_group[i], g_gpio_poweroff_bit[i], 0); /* output */
        gpio_write_bit(g_gpio_poweroff_group[i], g_gpio_poweroff_bit[i], g_gpio_poweroff_level[i]);
    }

    return;
}

static hi_void set_peripheral_power_on(hi_void)
{
    hi_u8 i;

    /* gpio power on */
    for (i = 0; i < g_gpio_poweroff_num; i++) {
        gpio_dirset_bit(g_gpio_poweroff_group[i], g_gpio_poweroff_bit[i], 0); /* output */
        gpio_write_bit(g_gpio_poweroff_group[i], g_gpio_poweroff_bit[i], !g_gpio_poweroff_level[i]);
    }

    return;
}

static hi_void system_power_down(hi_void)
{
    /* set the delay 100ms between power up and revocation of reset */
    reg_set(SC_CORE_PWRUP_COUNT, 0x124f80); /* (1000 / 12) * 0x124f80 ns */

    /* power down */
    reg_set(SC_POWER_STAT, 0x0);

    while (1) {
        /* bit[7:4] */
        if ((reg_get(SC_LOW_POWER_CTRL) & 0xf0) == 0x50) {
            break;
        }
    }

    /* set ddrphy to retension */
    reg_set(SC_DDRPHY_LP_EN, 0x0);

    return;
}

static hi_void system_suspend(hi_void)
{
    hi_u32_data reg_data;

    /* change LPMCU bus clock to 24M */
    reg_data.val32 = reg_get(SC_CRG_CTRL_0);
    reg_data.val8[0] &= 0xee; /* bit0 to 0, bit4 to 0 */
    reg_set(SC_CRG_CTRL_0, reg_data.val32);

    set_peripheral_power_off();

    if (g_suspend_type == PASSIVE_STANDBY) {
        system_power_down();
    }

    return;
}

static hi_void system_resume(hi_void)
{
    if (g_suspend_type != PASSIVE_STANDBY) {
        system_power_down();
    }

    set_peripheral_power_on();

    /* power up */
    reg_set(SC_POWER_STAT, 0x1);

    while (1) {
        wait_minute_2(10, 10);
    }

    return;
}

static hi_void enter_suspend_in_boot(hi_void)
{
    hi_u32 reg_value;

    reg_value = ram_get(LPMCU_ENTER_SUSPEND_IN_BOOT_ADDR);
    if (reg_value == ENTER_SUSPEND_IN_BOOT_FLAG) {
        return;
    }

    while (1) {
        wait_minute_2(200, 200);

        reg_value = reg_get(SC_LP_START);
        if ((reg_value == ENTER_PMOC_FLAG) || (reg_value == TEMP_CHECK_TO_SUSPEND)) {
            reg_set(SC_LP_START, 0);
            break;
        }
    }

    return;
}

static hi_void enter_suspend_in_kernel(hi_void)
{
    hi_u32 reg_value;

    while (1) {
        wait_minute_2(200, 200);

        reg_value = reg_get(SC_LP_START);
        if ((reg_value == ENTER_PMOC_FLAG) || (reg_value == TEMP_CHECK_TO_SUSPEND)) {
            reg_set(SC_LP_START, 0);
            break;
        }
    }

    return;
}
static hi_void module_init(hi_void)
{
    hi_u8 i;

    if (g_keyled_wakeup_enable && g_keyled_type != KEYLED_TYPE_GPIOKEY) {
        /* keyled init */
        keyled_init();
    }

    timer_init();

    /* gpio init */
    if (g_gpio_wakeup_enable) {
        for (i = 0; i < g_gpio_wakeup_num; i++) {
            gpio_set_interrupt_type(g_gpio_wakeup_group[i],
                                    g_gpio_wakeup_bit[i],
                                    g_gpio_interrupt_type[i]);
        }
    }

    if (g_ir_wakeup_enable) {
        ir_init();
    }

    return;
}
static hi_void module_start(hi_void)
{
    hi_u8 i;

    if (g_keyled_wakeup_enable && g_keyled_type != KEYLED_TYPE_GPIOKEY) {
        /* keyled early display */
        keyled_early_display();
    }

    timer_enable();

    /* gpio enable */
    if (g_gpio_wakeup_enable) {
        for (i = 0; i < g_gpio_wakeup_num; i++) {
            gpio_interrupt_enable(g_gpio_wakeup_group[i], g_gpio_wakeup_bit[i], HI_TRUE);
        }
    }

    if (g_ir_wakeup_enable) {
        ir_start();
    }

    if (g_hdmitx_cec_wakeup_enable[0]) {
        g_hdmitx0_cec_resource.wakeup_mode = g_hdmitx_cec_control_type[0],
        mcu_hdmitx_cec_init(&g_hdmitx0_cec, &g_hdmitx0_cec_resource);
    }

    return;
}

static hi_void module_deinit(hi_void)
{
    hi_u8 i;

    if (g_keyled_wakeup_enable && g_keyled_type != KEYLED_TYPE_GPIOKEY) {
        /* keyled disable */
        keyled_disable();
    }

    if (g_hdmitx_cec_wakeup_enable[0]) {
        mcu_hdmitx_cec_deinit(&g_hdmitx0_cec);
    }

    if (g_ir_wakeup_enable) {
        ir_disable();
    }

    timer_disable();

    /* gpio disable */
    if (g_gpio_wakeup_enable) {
        for (i = 0; i < g_gpio_wakeup_num; i++) {
            gpio_interrupt_enable(g_gpio_wakeup_group[i], g_gpio_wakeup_bit[i], HI_FALSE);
        }
    }

    return;
}

hi_void main()
{
    lpmcu_init();

    /* suspend in boot or in kernel */
    if (ram_get(LPMCU_START_IN_BOOT_ADDR) == LPMCU_START_IN_BOOT_FLAG) {
        enter_suspend_in_boot();
    } else {
        enter_suspend_in_kernel();
    }

    g_dbg_mask = 1;

    printf_str("Enter LPMCU. \r\n");

    get_suspend_params();

    module_init();

    system_suspend();

    module_start();

    printf_str("Enter while circle \r\n");

    while (1) {
        wait_minute_2(10, 10);
        EA = 0;

        if (g_hdmitx_cec_wakeup_enable[0]) {
            mcu_hdmitx_cec_task_handle(&g_hdmitx0_cec, g_resume_flag, (hi_bool *)&g_resume_flag);
            if (g_resume_flag) {
                g_wakeup_type = HI_PMOC_WAKEUP_TYPE_HDMITX_CEC;
            }
        }

        if (g_resume_flag) {
            break;
        }

        EA = 1;
    }

    module_deinit();

    save_wakeup_params();

    printf_str("system resume... \r\n");
    system_resume();

    return;
}
