/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: ioctl functions
 */

#include "drv_pmoc.h"
#include "drv_pmoc_debug.h"

#include <linux/device.h>
#include <linux/hisilicon/securec.h>
#include <asm/io.h>

#include "hi_drv_sys.h"
#include "hi_type.h"
#include "hi_debug.h"
#include "hi_osal.h"
#include "linux/hisilicon/hi_drv_mbx.h"
#include "drv_pmoc_ioctl.h"
#include "lpmcu_ram_config.h"

#define PMC_TSENSOR0_CTRL0 0x00A15A00
#define PMC_TSENSOR0_CTRL2 0x00A15A08
#define TSENSOR0_OUT_REG 4
#define TSENSOR0_CTRL0_CONFIG 0x72

#define WAKEUP_TO_DDR 0x66031013
#define TIME_OUT_SEND 0x50000 /* test time in FPGA */
#define TIME_OUT_READ 0xF0000 /* test time in FPGA */
#define MBX_RETURN_SUCCESS 0x5A5AA5A5
#define MBX_CMD_STANDBY_PARAM 0x2 /* reference to hrf */
#define MBX_BUFFER_LEN 0x2
#define INVALID_VALUE 0xffffffff

#define GPIO_BITS_PER_GROUP 8

typedef struct {
    hi_u32 cmd;
    hi_s32(*f_driver_cmd_process)(hi_void *arg);
} pmoc_ioctl_map;

typedef struct {
    hi_pmoc_wakeup_src source;
    hi_s32(*f_driver_suspend_param_process)(hi_bool set, pmoc_cmd_suspend_attr *attr);
} pmoc_suspend_param_process_map;

typedef union {
    hi_u32 val32;
    hi_u16 val16[2];
    hi_u8 val8[4];
} u32_data;

static hi_u32 g_lpmcu_data[] = {
#ifndef HI_ADVCA_FUNCTION_RELEASE
    #include "output.txt"
#else
    #include "output_ca_release.txt"
#endif
};

static pmoc_wakeup_message g_wakeup_message;
static hi_u32 *g_lpmcu_base = HI_NULL;
static osal_semaphore g_pmoc_sem;
static hi_pmoc_wakeup_type g_wakeup_type = HI_PMOC_WAKEUP_TO_DDR;

static hi_s32 pmoc_ir_suspend_param(hi_bool set, pmoc_cmd_suspend_attr *attr)
{
    hi_u8 i;
    u32_data tmp;
    hi_u32 *lpmcu_vir_addr = HI_NULL;

    HI_FUNC_ENTER();

    lpmcu_vir_addr = g_lpmcu_base + LPMCU_IR_SUSPEND_PARAM_ADDR / sizeof(hi_u32);

    if (set) {
        if (attr->param.ir_param.ir_num >= HI_PMOC_WAKEUP_IRKEY_MAXNUM) {
            attr->param.ir_param.ir_num = HI_PMOC_WAKEUP_IRKEY_MAXNUM;
        }

        tmp.val32 = 0;
        tmp.val8[0] = attr->enable;
        tmp.val8[1] = attr->param.ir_param.ir_type;
        tmp.val8[2] = attr->param.ir_param.ir_num;
        writel(tmp.val32, lpmcu_vir_addr++);

        for (i = 0; i < attr->param.ir_param.ir_num; i++) {
            writel(attr->param.ir_param.ir_low_val[i], lpmcu_vir_addr++);
            writel(attr->param.ir_param.ir_high_val[i], lpmcu_vir_addr++);
        }
    } else {
        tmp.val32 = readl(lpmcu_vir_addr++);
        attr->enable = tmp.val8[0];
        attr->param.ir_param.ir_type = tmp.val8[1];
        attr->param.ir_param.ir_num = tmp.val8[2];

        if (attr->param.ir_param.ir_num >= HI_PMOC_WAKEUP_IRKEY_MAXNUM) {
            attr->param.ir_param.ir_num = HI_PMOC_WAKEUP_IRKEY_MAXNUM;
        }

        for (i = 0; i < attr->param.ir_param.ir_num; i++) {
            attr->param.ir_param.ir_low_val[i] = readl(lpmcu_vir_addr++);
            attr->param.ir_param.ir_high_val[i] = readl(lpmcu_vir_addr++);
        }
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_keyled_suspend_param(hi_bool set, pmoc_cmd_suspend_attr *attr)
{
    hi_u32 *lpmcu_vir_addr = HI_NULL;

    HI_FUNC_ENTER();

    lpmcu_vir_addr = g_lpmcu_base + LPMCU_KEYLED_SUSPEND_PARAM_ADDR / sizeof(hi_u32);

    if (set) {
        writel(attr->enable, lpmcu_vir_addr++);
        writel(attr->param.keyled_param.keyled_type, lpmcu_vir_addr++);
        writel(attr->param.keyled_param.wakeup_key, lpmcu_vir_addr++);
    } else {
        attr->enable = readl(lpmcu_vir_addr++);
        attr->param.keyled_param.keyled_type = readl(lpmcu_vir_addr++);
        attr->param.keyled_param.wakeup_key = readl(lpmcu_vir_addr++);
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_gpio_suspend_param(hi_bool set, pmoc_cmd_suspend_attr *attr)
{
    hi_u8 i;
    u32_data tmp;
    hi_u32 *lpmcu_vir_addr = HI_NULL;

    HI_FUNC_ENTER();

    lpmcu_vir_addr = g_lpmcu_base + LPMCU_GPIO_SUSPEND_PARAM_ADDR / sizeof(hi_u32);

    if (set) {
        if (attr->param.gpio_param.num >= HI_PMOC_WAKEUP_GPIO_MAXNUM) {
            attr->param.gpio_param.num = HI_PMOC_WAKEUP_GPIO_MAXNUM;
        }

        tmp.val32 = 0;
        tmp.val8[0] = attr->enable;
        tmp.val8[1] = attr->param.gpio_param.num;
        writel(tmp.val32, lpmcu_vir_addr++);

        for (i = 0; i < attr->param.gpio_param.num; i++) {
            tmp.val32 = 0;
            tmp.val8[0] = attr->param.gpio_param.group[i];
            tmp.val8[1] = attr->param.gpio_param.bit[i];
            tmp.val8[2] = attr->param.gpio_param.interrupt_type[i];
            writel(tmp.val32, lpmcu_vir_addr++);
        }
    } else {
        tmp.val32 = readl(lpmcu_vir_addr++);
        attr->enable = tmp.val8[0];
        attr->param.gpio_param.num = tmp.val8[1];

        if (attr->param.gpio_param.num >= HI_PMOC_WAKEUP_GPIO_MAXNUM) {
            attr->param.gpio_param.num = HI_PMOC_WAKEUP_GPIO_MAXNUM;
        }

        for (i = 0; i < attr->param.gpio_param.num; i++) {
            tmp.val32 = readl(lpmcu_vir_addr++);
            attr->param.gpio_param.group[i] = tmp.val8[0];
            attr->param.gpio_param.bit[i] = tmp.val8[1];
            attr->param.gpio_param.interrupt_type[i] = tmp.val8[2];
        }
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_lsadc_suspend_param(hi_bool set, pmoc_cmd_suspend_attr *attr)
{
    HI_FUNC_ENTER();

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_uart_suspend_param(hi_bool set, pmoc_cmd_suspend_attr *attr)
{
    u32_data tmp;
    hi_u32 *lpmcu_vir_addr = HI_NULL;

    HI_FUNC_ENTER();

    lpmcu_vir_addr = g_lpmcu_base + LPMCU_UART_SUSPEND_PARAM_ADDR / sizeof(hi_u32);

    if (set) {
        tmp.val32 = 0;
        tmp.val8[0] = attr->enable;
        tmp.val8[1] = attr->param.uart_param.wakeup_key;
        writel(tmp.val32, lpmcu_vir_addr);
    } else {
        tmp.val32 = readl(lpmcu_vir_addr);
        attr->enable = tmp.val8[0];
        attr->param.uart_param.wakeup_key = tmp.val8[1];
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_eth_suspend_param(hi_bool set, pmoc_cmd_suspend_attr *attr)
{
    u32_data tmp;
    hi_u32 *lpmcu_vir_addr = HI_NULL;

    HI_FUNC_ENTER();

    lpmcu_vir_addr = g_lpmcu_base + LPMCU_ETH_SUSPEND_PARAM_ADDR / sizeof(hi_u32);

    if (set) {
        tmp.val32 = 0;
        tmp.val8[0] = attr->enable;
        writel(tmp.val32, lpmcu_vir_addr++);
        writel(attr->param.eth_param.time_to_passive_standby, lpmcu_vir_addr);
    } else {
        tmp.val32 = readl(lpmcu_vir_addr++);
        attr->enable = tmp.val8[0];
        attr->param.eth_param.time_to_passive_standby = readl(lpmcu_vir_addr);
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_usb_suspend_param(hi_bool set, pmoc_cmd_suspend_attr *attr)
{
    u32_data tmp;
    hi_u32 *lpmcu_vir_addr = HI_NULL;
    volatile hi_reg_peri *reg_peri = hi_drv_sys_get_peri_reg_ptr();

    HI_FUNC_ENTER();

    lpmcu_vir_addr = g_lpmcu_base + LPMCU_USB_SUSPEND_PARAM_ADDR / sizeof(hi_u32);

    if (set) {
        tmp.val32 = 0;
        tmp.val8[0] = attr->enable;
        writel(tmp.val32, lpmcu_vir_addr);

        HI_INFO_PRINT_H32(attr->param.usb_param.usb_wakeup_mask);
        reg_peri->PERI_USB_RESUME_INT_MASK.u32 = attr->param.usb_param.usb_wakeup_mask;
    } else {
        tmp.val32 = readl(lpmcu_vir_addr);
        attr->enable = tmp.val8[0];

        attr->param.usb_param.usb_wakeup_mask = reg_peri->PERI_USB_RESUME_INT_MASK.u32;
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_vga_suspend_param(hi_bool set, pmoc_cmd_suspend_attr *attr)
{
    HI_FUNC_ENTER();

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_scart_suspend_param(hi_bool set, pmoc_cmd_suspend_attr *attr)
{
    HI_FUNC_ENTER();

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_hdmirx_plugin_suspend_param(hi_bool set, pmoc_cmd_suspend_attr *attr)
{
    HI_FUNC_ENTER();

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_hdmirx_cec_suspend_param(hi_bool set, pmoc_cmd_suspend_attr *attr)
{
    HI_FUNC_ENTER();

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_hdmitx_cec_suspend_param(hi_bool set, pmoc_cmd_suspend_attr *attr)
{
    u32_data tmp;
    hi_u8 i;
    hi_u32 *lpmcu_vir_addr = HI_NULL;

    HI_FUNC_ENTER();

    lpmcu_vir_addr = g_lpmcu_base + LPMCU_HDMI_SUSPEND_PARAM_ADDR / sizeof(hi_u32);

    if (set) {
        tmp.val32 = readl(lpmcu_vir_addr);
        tmp.val8[0] = attr->enable;
        writel(tmp.val32, lpmcu_vir_addr);
        lpmcu_vir_addr = g_lpmcu_base + LPMCU_HDMITX_CEC_PARAM_ADDR / sizeof(hi_u32);
        for (i = 0; i < LPMCU_WAKEUP_HDMITX_ID_MAXNUM; i++) {
            tmp.val32 = 0;
            tmp.val8[0] = attr->param.hdmitx_cec_param.id[i];
            tmp.val8[1] = attr->param.hdmitx_cec_param.cec_control[i];
            writel(tmp.val32, lpmcu_vir_addr++);
        }
    } else {
        tmp.val32 = readl(lpmcu_vir_addr);
        attr->enable = tmp.val8[0];
        lpmcu_vir_addr = g_lpmcu_base + LPMCU_HDMITX_CEC_PARAM_ADDR / sizeof(hi_u32);
        for (i = 0; i < LPMCU_WAKEUP_HDMITX_ID_MAXNUM; i++) {
            tmp.val32 = readl(lpmcu_vir_addr++);
            attr->param.hdmitx_cec_param.id[i] = tmp.val8[0];
            attr->param.hdmitx_cec_param.cec_control[i] = tmp.val8[1];
        }
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_timeout_suspend_param(hi_bool set, pmoc_cmd_suspend_attr *attr)
{
    u32_data tmp;
    hi_u32 *lpmcu_vir_addr = HI_NULL;

    HI_FUNC_ENTER();

    lpmcu_vir_addr = g_lpmcu_base + LPMCU_TIMEOUT_SUSPEND_PARAM_ADDR / sizeof(hi_u32);

    if (set) {
        tmp.val32 = 0;
        tmp.val8[0] = attr->enable;
        tmp.val8[1] = attr->param.timeout_param.pvr_enable;
        writel(tmp.val32, lpmcu_vir_addr++);
        writel(attr->param.timeout_param.suspend_period, lpmcu_vir_addr);
    } else {
        tmp.val32 = readl(lpmcu_vir_addr++);
        attr->enable = tmp.val8[0];
        attr->param.timeout_param.pvr_enable = tmp.val8[1];

        attr->param.timeout_param.suspend_period = readl(lpmcu_vir_addr);
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static pmoc_suspend_param_process_map g_suspend_param_process_func[] = {
    { HI_PMOC_WAKEUP_TYPE_IR,            pmoc_ir_suspend_param },
    { HI_PMOC_WAKEUP_TYPE_KEYLED,        pmoc_keyled_suspend_param },
    { HI_PMOC_WAKEUP_TYPE_GPIO,          pmoc_gpio_suspend_param },
    { HI_PMOC_WAKEUP_TYPE_LSADC,         pmoc_lsadc_suspend_param },
    { HI_PMOC_WAKEUP_TYPE_UART,          pmoc_uart_suspend_param },
    { HI_PMOC_WAKEUP_TYPE_ETH,           pmoc_eth_suspend_param },
    { HI_PMOC_WAKEUP_TYPE_USB,           pmoc_usb_suspend_param },
    { HI_PMOC_WAKEUP_TYPE_VGA,           pmoc_vga_suspend_param },
    { HI_PMOC_WAKEUP_TYPE_SCART,         pmoc_scart_suspend_param },
    { HI_PMOC_WAKEUP_TYPE_HDMIRX_PLUGIN, pmoc_hdmirx_plugin_suspend_param },
    { HI_PMOC_WAKEUP_TYPE_HDMIRX_CEC,    pmoc_hdmirx_cec_suspend_param },
    { HI_PMOC_WAKEUP_TYPE_HDMITX_CEC,    pmoc_hdmitx_cec_suspend_param },
    { HI_PMOC_WAKEUP_TYPE_TIMEOUT,       pmoc_timeout_suspend_param },
};

static hi_s32 pmoc_set_suspend_param(hi_void *arg)
{
    hi_s32 ret;
    hi_u8 i;
    hi_u32 source_num;
    pmoc_cmd_suspend_attr attr = {0};

    HI_FUNC_ENTER();

    PMOC_CHECK_PARAM(arg == HI_NULL, HI_FAILURE);

    ret = memcpy_s(&attr, sizeof(pmoc_cmd_suspend_attr), arg, sizeof(pmoc_cmd_suspend_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return HI_FAILURE;
    }

    source_num = sizeof(g_suspend_param_process_func) / sizeof(g_suspend_param_process_func[0]);

    for (i = 0; i < source_num; i++) {
        if (attr.source == g_suspend_param_process_func[i].source) {
            ret = g_suspend_param_process_func[i].f_driver_suspend_param_process(HI_TRUE, &attr);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_H32(attr.source);
                HI_ERR_PRINT_S32(ret);
                return ret;
            }

            HI_FUNC_EXIT();
            return HI_SUCCESS;
        }
    }

    return HI_FAILURE;
}

static hi_s32 pmoc_get_suspend_param(hi_void *arg)
{
    hi_s32 ret;
    hi_u8 i;
    hi_u32 source_num;
    pmoc_cmd_suspend_attr attr = {0};

    HI_FUNC_ENTER();

    PMOC_CHECK_PARAM(arg == HI_NULL, HI_FAILURE);

    ret = memcpy_s(&attr, sizeof(pmoc_cmd_suspend_attr), arg, sizeof(pmoc_cmd_suspend_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return HI_FAILURE;
    }

    source_num = sizeof(g_suspend_param_process_func) / sizeof(g_suspend_param_process_func[0]);

    for (i = 0; i < source_num; i++) {
        if (attr.source == g_suspend_param_process_func[i].source) {

            ret = g_suspend_param_process_func[i].f_driver_suspend_param_process(HI_FALSE, &attr);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_H32(attr.source);
                HI_ERR_PRINT_S32(ret);
                return ret;
            }

            ret = memcpy_s(arg, sizeof(pmoc_cmd_suspend_attr), &attr, sizeof(pmoc_cmd_suspend_attr));
            if (ret != EOK) {
                HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
                return HI_FAILURE;
            }

            HI_FUNC_EXIT();
            return HI_SUCCESS;
        }
    }

    return HI_FAILURE;
}

static hi_s32 pmoc_set_wakeup_type(hi_void *arg)
{
    hi_s32 ret;
    u32_data tmp;
    hi_pmoc_wakeup_type type;
    hi_u32 *lpmcu_vir_addr = HI_NULL;

    HI_FUNC_ENTER();

    PMOC_CHECK_PARAM(arg == HI_NULL, HI_FAILURE);

    ret = memcpy_s(&type, sizeof(hi_pmoc_wakeup_type), arg, sizeof(hi_pmoc_wakeup_type));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return HI_FAILURE;
    }

    g_wakeup_type = type;

    lpmcu_vir_addr = g_lpmcu_base + LPMCU_RESUME_RESET_ADDR / sizeof(hi_u32);
    tmp.val32 = readl(lpmcu_vir_addr);
    tmp.val8[0] = type;
    writel(tmp.val32, lpmcu_vir_addr);

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_standby_ready(hi_void *arg)
{
    HI_FUNC_ENTER();

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_set_gpio_power_off(hi_void *arg)
{
    HI_FUNC_ENTER();

    PMOC_CHECK_PARAM(arg == HI_NULL, HI_FAILURE);

    /* TBD: set param to LPMCU RAM */

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_set_display_param(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 *lpmcu_vir_addr = HI_NULL;
    hi_pmoc_display_param display_param = {0};

    HI_FUNC_ENTER();

    PMOC_CHECK_PARAM(arg == HI_NULL, HI_FAILURE);

    ret = memcpy_s(&display_param, sizeof(hi_pmoc_display_param), arg, sizeof(hi_pmoc_display_param));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return HI_FAILURE;
    }

    lpmcu_vir_addr = g_lpmcu_base + LPMCU_KEYLED_SUSPEND_PARAM_ADDR / sizeof(hi_u32);
    writel(display_param.keyled_type, lpmcu_vir_addr);

    lpmcu_vir_addr = g_lpmcu_base + LPMCU_KEYLED_DISPLAY_ADDR / sizeof(hi_u32);
    writel(display_param.display_type, lpmcu_vir_addr++);
    writel(display_param.display_value, lpmcu_vir_addr++);

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_get_standby_period(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 period;

    HI_FUNC_ENTER();

    PMOC_CHECK_PARAM(arg == HI_NULL, HI_FAILURE);

    if (g_wakeup_message.wakeup_type >= HI_PMOC_WAKEUP_TYPE_MAX) {
        HI_ERR_PRINT_INFO("wakeup type is illegal");
        return HI_FAILURE;
    }

    period = g_wakeup_message.standby_period;

    ret = memcpy_s(arg, sizeof(hi_u32), &period, sizeof(hi_u32));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return HI_FAILURE;
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_get_wakeup_attr(hi_void *arg)
{
    hi_s32 ret;
    hi_pmoc_wakeup_attr attr = {0};

    HI_FUNC_ENTER();

    PMOC_CHECK_PARAM(arg == HI_NULL, HI_FAILURE);

    if (g_wakeup_message.wakeup_type >= HI_PMOC_WAKEUP_TYPE_MAX) {
        HI_ERR_PRINT_INFO("wakeup type is illegal");
        return HI_FAILURE;
    }

    attr.source = g_wakeup_message.wakeup_type;

    if (attr.source == HI_PMOC_WAKEUP_TYPE_IR) {
        attr.wakeup_param.ir_param.ir_low_val = g_wakeup_message.ir_low_val;
        attr.wakeup_param.ir_param.ir_high_val = g_wakeup_message.ir_high_val;
    } else if (attr.source == HI_PMOC_WAKEUP_TYPE_GPIO) {
        attr.wakeup_param.gpio_param.group = g_wakeup_message.gpio_wakeup_port / GPIO_BITS_PER_GROUP;
        attr.wakeup_param.gpio_param.bit = g_wakeup_message.gpio_wakeup_port % GPIO_BITS_PER_GROUP;
    }

    ret = memcpy_s(arg, sizeof(hi_pmoc_wakeup_attr), &attr, sizeof(hi_pmoc_wakeup_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return HI_FAILURE;
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_clean_wakeup_param(hi_void *arg)
{
    hi_u32 *lpmcu_vir_addr = HI_NULL;

    HI_FUNC_ENTER();

    lpmcu_vir_addr = g_lpmcu_base + LPMCU_WAKEUP_DATA_ADDR / sizeof(hi_u32);

    writel(INVALID_VALUE, lpmcu_vir_addr++);
    writel(INVALID_VALUE, lpmcu_vir_addr++);
    writel(INVALID_VALUE, lpmcu_vir_addr++);
    writel(INVALID_VALUE, lpmcu_vir_addr++);
    writel(INVALID_VALUE, lpmcu_vir_addr);

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_enter_active_standby(hi_void *arg)
{
    hi_s32 ret;
    hi_pmoc_active_standby_modle modle;

    HI_FUNC_ENTER();

    PMOC_CHECK_PARAM(arg == HI_NULL, HI_FAILURE);

    ret = memcpy_s(&modle, sizeof(hi_pmoc_active_standby_modle), arg, sizeof(hi_pmoc_active_standby_modle));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return HI_FAILURE;
    }

    /* TBD: enter_active_standby */

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_quit_active_standby(hi_void *arg)
{
    HI_FUNC_ENTER();

    /* TBD: quit_active_standby */

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_get_chip_temprature(hi_void *arg)
{
    hi_s32 ret;
    hi_s16 real_temp;
    hi_s32 temperature;

    HI_FUNC_ENTER();

    PMOC_CHECK_PARAM(arg == HI_NULL, HI_FAILURE);

    ret = pmoc_tsensor_read(&real_temp);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(pmoc_tsensor_read, ret);
        return HI_FAILURE;
    }

    temperature = real_temp;

    ret = memcpy_s(arg, sizeof(hi_u32), &temperature, sizeof(hi_u32));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return HI_FAILURE;
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static pmoc_ioctl_map g_pmoc_func[] = {
    { CMD_PMOC_SET_SUSPEND_ATTR,       pmoc_set_suspend_param },
    { CMD_PMOC_GET_SUSPEND_ATTR,       pmoc_get_suspend_param },
    { CMD_PMOC_SET_WAKEUP_TYPE,        pmoc_set_wakeup_type },
    { CMD_PMOC_STANDBY_READY,          pmoc_standby_ready },
    { CMD_PMOC_GET_WAKEUP_ATTR,        pmoc_get_wakeup_attr },
    { CMD_PMOC_GET_STANDBY_PERIOD,     pmoc_get_standby_period },
    { CMD_PMOC_CLEAN_WAKEUP_PARAM,     pmoc_clean_wakeup_param },
    { CMD_PMOC_ENTER_ACTIVE_STANDBY,   pmoc_enter_active_standby },
    { CMD_PMOC_QUIT_ACTIVE_STANDBY,    pmoc_quit_active_standby },
    { CMD_PMOC_SET_DISPLAY_PARAM,      pmoc_set_display_param },
    { CMD_PMOC_SET_GPIO_POWEROFF,      pmoc_set_gpio_power_off },
    { CMD_PMOC_GET_CHIP_TEMPERATURE,   pmoc_get_chip_temprature },
};

hi_s32 pmoc_ioctl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_u32 cmd_num;
    hi_u8 i;
    hi_s32 ret;

    HI_FUNC_ENTER();

    cmd_num = sizeof(g_pmoc_func) / sizeof(g_pmoc_func[0]);

    for (i = 0; i < cmd_num; i++) {
        if (cmd == g_pmoc_func[i].cmd) {
            ret = osal_sem_down_interruptible(&g_pmoc_sem);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(osal_sem_down_interruptible, ret);
                return ret;
            }

            ret = g_pmoc_func[i].f_driver_cmd_process(arg);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_H32(cmd);
                HI_ERR_PRINT_S32(ret);
                osal_sem_up(&g_pmoc_sem);
                return ret;
            }

            osal_sem_up(&g_pmoc_sem);
            return HI_SUCCESS;
        }
    }

    return HI_FAILURE;
}

hi_s32 pmoc_tsensor_init(hi_void)
{
    hi_u32 *tsensor_vir_addr = HI_NULL;

    HI_FUNC_ENTER();

    tsensor_vir_addr = (hi_u32 *)osal_ioremap_nocache(PMC_TSENSOR0_CTRL0, sizeof(hi_u32));
    if (tsensor_vir_addr == HI_NULL) {
        HI_ERR_PRINT_FUNC_RES(osal_ioremap_nocache, HI_FAILURE);
        return HI_FAILURE;
    }

    writel(TSENSOR0_CTRL0_CONFIG, tsensor_vir_addr);

    osal_iounmap(tsensor_vir_addr);

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

hi_s32 pmoc_tsensor_read(hi_s16 *temperature)
{
    hi_u8 i;
    u32_data reg_value;
    hi_u16 tsensor_value = 0;
    hi_u32 *tsensor_vir_addr = HI_NULL;

    HI_FUNC_ENTER();

    PMOC_CHECK_PARAM(temperature == HI_NULL, HI_FAILURE);

    tsensor_vir_addr = (hi_u32 *)osal_ioremap_nocache(PMC_TSENSOR0_CTRL2, TSENSOR0_OUT_REG * sizeof(hi_u32));
    if (tsensor_vir_addr == HI_NULL) {
        HI_ERR_PRINT_FUNC_RES(osal_ioremap_nocache, HI_FAILURE);
        return HI_FAILURE;
    }

    for (i = 0; i < TSENSOR0_OUT_REG; i++) {
        reg_value.val32 = readl(tsensor_vir_addr + i);
        tsensor_value += reg_value.val16[0] & 0x3ff;
        tsensor_value += reg_value.val16[1] & 0x3ff;
    }

    tsensor_value /= 8; /* 8: tsensor out number */

    *temperature = ((tsensor_value - 176) * 165 / 736) - 40; /* test formula: (data-176)/736*165-40 */

    osal_iounmap(tsensor_vir_addr);

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

hi_s32 pmoc_get_proc_info(pmoc_proc_info *info)
{
    HI_FUNC_ENTER();

    PMOC_CHECK_PARAM(info == HI_NULL, HI_FAILURE);

    /* TBD: */
    info->wakeup_message = g_wakeup_message;

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

hi_void pmoc_get_wakeup_message(hi_void)
{
    hi_u32 *lpmcu_vir_addr = HI_NULL;

    HI_FUNC_ENTER();

    lpmcu_vir_addr = g_lpmcu_base + LPMCU_WAKEUP_DATA_ADDR / sizeof(hi_u32);

    g_wakeup_message.wakeup_type = readl(lpmcu_vir_addr++);
    g_wakeup_message.gpio_wakeup_port = readl(lpmcu_vir_addr++);
    g_wakeup_message.ir_low_val = readl(lpmcu_vir_addr++);
    g_wakeup_message.ir_high_val = readl(lpmcu_vir_addr++);
    g_wakeup_message.standby_period = readl(lpmcu_vir_addr);

    HI_FUNC_EXIT();
    return;
}

hi_void pmoc_set_lpmcu_dbg_level(hi_u32 value)
{
    u32_data tmp;
    hi_u32 *lpmcu_vir_addr = HI_NULL;

    HI_FUNC_ENTER();

    HI_INFO_PRINT_U32(value);

    if ((value != 0) && (value != 1)) {
        HI_ERR_PRINT_INFO("plese set dbg level: 0(lpmcu without log), 1(lpmcu with log)\n");
        return;
    }

    lpmcu_vir_addr = g_lpmcu_base + LPMCU_DBG_MASK_ADDR / sizeof(hi_u32);
    tmp.val32 = readl(lpmcu_vir_addr);
    tmp.val8[1] = value;
    writel(tmp.val32, lpmcu_vir_addr);


    HI_FUNC_EXIT();
    return;
}

hi_s32 pmoc_set_wakeup_type_to_hrf(hi_pmoc_wakeup_type type)
{
    hi_s32 ret = HI_FAILURE;
    hi_s32 handle;
    hi_u32 buf[MBX_BUFFER_LEN] = {0};
    hi_u32 msg_len;
    hi_u32 result;

    HI_FUNC_ENTER();

    handle = hi_drv_mbx_open(HI_MBX_ACPU2HRF_PMOC);
    if (handle < 0) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_mbx_open, ret);
        return HI_FAILURE;
    }

    buf[0] = MBX_CMD_STANDBY_PARAM;
    buf[1] = (type == HI_PMOC_WAKEUP_TO_DDR) ? WAKEUP_TO_DDR : 0x0;

    ret = hi_drv_mbx_tx(handle, (hi_u8 *)buf, MBX_BUFFER_LEN * sizeof(hi_u32), &msg_len, TIME_OUT_SEND);
    if (ret < 0 || msg_len != MBX_BUFFER_LEN * sizeof(hi_u32)) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_mbx_tx, ret);
        ret = HI_FAILURE;
        goto EXIT;
    }

    ret = hi_drv_mbx_rx(handle, (hi_u8 *)&result, sizeof(hi_u32), &msg_len, TIME_OUT_READ);
    if (ret < 0 || msg_len != sizeof(hi_u32) || result != MBX_RETURN_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_mbx_rx, ret);
        ret = HI_FAILURE;
        goto EXIT;
    }

 EXIT:
    ret |= hi_drv_mbx_close(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_mbx_close, ret);
        return ret;
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

hi_s32 pmoc_load_standby_params(hi_void)
{
    hi_s32 ret;
    hi_chip_type chip_type;
    hi_chip_version chip_version;
    hi_u32 *lpmcu_vir_addr = HI_NULL;

    HI_FUNC_ENTER();

    lpmcu_vir_addr = g_lpmcu_base + LPMCU_CHIP_TYPE_ADDR / sizeof(hi_u32);

    hi_drv_sys_get_chip_version(&chip_type, &chip_version);
    writel(chip_type, lpmcu_vir_addr++);
    writel(chip_version, lpmcu_vir_addr);

    ret = pmoc_set_wakeup_type_to_hrf(g_wakeup_type);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(pmoc_set_wakeup_type_to_hrf, ret);
        return HI_FAILURE;
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

hi_s32 pmoc_load_lpmcu(hi_void)
{
    hi_u32 i;
    hi_u32 lpmcu_data_size;
    hi_u32 *lpmcu_vir_addr = HI_NULL;

    HI_FUNC_ENTER();

    lpmcu_data_size = sizeof(g_lpmcu_data);
    PMOC_CHECK_PARAM(lpmcu_data_size > LPMCU_CODE_MAXSIZE, HI_FAILURE);

    for (i = 0; i < (lpmcu_data_size >> 2); i++) {
        writel(g_lpmcu_data[i], g_lpmcu_base + i);
    }

    lpmcu_vir_addr = g_lpmcu_base + LPMCU_ENTER_SUSPEND_IN_BOOT_ADDR / sizeof(hi_u32);

    if (readl(lpmcu_vir_addr) != ENTER_SUSPEND_IN_BOOT_FLAG) {
        lpmcu_vir_addr = g_lpmcu_base + LPMCU_STANDBY_DATA_ADDR / sizeof(hi_u32);
        for (i = 0; i < LPMCU_STANDBY_DATA_SIZE / sizeof(hi_u32); i++) {
            writel(0, lpmcu_vir_addr++);
        }
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

hi_s32 pmoc_sem_init(hi_void)
{
    hi_s32 ret;

    ret = osal_sem_init(&g_pmoc_sem, 1);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(osal_sem_init, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 pmoc_register_remap(hi_void)
{
    g_lpmcu_base = osal_ioremap_nocache(LPMCU_RAM_BASE, LPMCU_SIZE);
    if (g_lpmcu_base == HI_NULL) {
        HI_ERR_PRINT_FUNC_RES(osal_ioremap_nocache, HI_FAILURE);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void pmoc_register_unmap(hi_void)
{
    osal_iounmap(g_lpmcu_base);
    return;
}

