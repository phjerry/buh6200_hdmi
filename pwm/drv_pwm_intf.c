/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description:
 */
#include <linux/module.h>
#include "hi_osal.h"
#include "linux/hisilicon/securec.h"
#include "hi_debug.h"
#include "hi_module.h"
#include "hi_reg_common.h"
#include "drv_pwm_ioctl.h"
#include "hi_drv_gpio.h"
#include "hi_drv_sys.h"
#include "drv_gpio_ext.h"

gpio_ext_func *g_gpio_func;

#define PWM_CLK_FREQ      (54 * 1000 * 1000) /* pwm work clock freq */
#define LED_PWM_REGS_SIZE 0x1000
#define PWM_FREQ_CRITICAL 10

#define HI_PWM_SPACE_SIZE 0X1000

volatile hi_void *g_pwm_reg_base = NULL;
volatile hi_void *g_led_pwm_reg_base = NULL;

#define PWM_BASE_ADDR     0xF8B30000
#define LED_PWM_BASE_ADDR 0xF800A000

#ifdef CHIP_TYPE_hi3731v110
#define PWM_MODE_OFFSET 0x10
#else
#define PWM_MODE_OFFSET 0x20
#endif

#define PWM_REG_CFG    0x0
#define PWM_REG_CTRL   0x4
#define PWM_REG_STATE0 0x8
#define PWM_REG_STATE1 0xc

#define PWM_MAX_NUM 1

#define PWM_NAME "HI_PWM"

#define HI_PWM_REG_REMAP(pvAddr, u32RegAddr, ret) \
    do { \
        (pvAddr) = osal_ioremap_nocache((u32RegAddr), HI_PWM_SPACE_SIZE); \
        if (NULL == (pvAddr)) { \
            HI_ERR_PWM("ioremap PWM_BASE_ADDR failure!\n", (u32RegAddr)); \
            (ret) = HI_FAILURE; \
        } \
    } while (0)

#define HI_FATAL_PWM(fmt...)    HI_FATAL_PRINT(HI_ID_PWM, fmt)
#define HI_ERR_PWM(fmt...)      HI_ERR_PRINT(HI_ID_PWM, fmt)
#define HI_WARN_PWM(fmt...)     HI_WARN_PRINT(HI_ID_PWM, fmt)
#define HI_INFO_PWM(fmt...)     HI_INFO_PRINT(HI_ID_PWM, fmt)

#define HI_PWM_REG_UNMAP(pvAddr) osal_iounmap(pvAddr)

/* only used for s3v110 */
volatile hi_void* g_s3_pwm_reg_base[2] = {0, 0}; /* 2 leng */

#define HI_GPIO0_PWM_REG_BASE 0xF8B20800
#define HI_GPIO5_PWM_REG_BASE 0xF8B25800
#define PWM_MAX_CHN_NUM   7
#define HI_PWM_GPIO_GROUP 0x0  /* which gpio group is reuse as pwm output */
#ifdef CHIP_TYPE_hi3731v110
#define PWM_FLOAT_VAL0 0.5f
#define PWM_FLOAT_VAL1 1.0f
#define PWM_FLOAT_VAL2 10.0f
#define PWM_FLOAT_VAL3 1000.0f
#define PWM_FLOAT_VAL4 10000.0f
#else
/* 350/560 no need to use float */
#define PWM_FLOAT_VAL0 0
#define PWM_FLOAT_VAL1 1
#define PWM_FLOAT_VAL2 10
#define PWM_FLOAT_VAL3 1000
#define PWM_FLOAT_VAL4 10000
#endif
#define HI_BOARD_LED_PWM_NUM 1
/* ---------end-------- */
static  osal_semaphore g_pwm_sem_intf;

typedef struct hi_pwm_state {
    hi_handle pwm_handle[PWM_MAX_NUM];
} pwm_state;

static pwm_state g_pwm_state;

typedef struct {
    hi_u32 pwm_no;
    hi_u32 value;
    hi_u32 cmd;
} pwm_proc_data;

typedef enum  {
    PWM_DRV_CHIP_V350 = 0x350,
    PWM_DRV_CHIP_V560 = 0x560,
    PWM_DRV_CHIP_V110 = 0x110,

    PWM_DRV_CHIP_BUTT
} pwm_drv_chip;

hi_u32 pwm_drv_get_chip_version(hi_void)
{
    hi_chip_version chip_version = CHIP_VERSION_BUTT;
    hi_chip_type chip_type;
    pwm_drv_chip chip = PWM_DRV_CHIP_V350;
#ifndef CHIP_TYPE_hi3731v110
    hi_drv_sys_get_chip_version(&chip_type, &chip_version);
#endif

    chip = PWM_DRV_CHIP_V560;

    HI_INFO_PWM("get chip version:0x%x\n", chip);
    return chip;
}

#if !defined(CHIP_TYPE_HI3796CV300) && !defined(CHIP_TYPE_HI3751V900)
static hi_s32 pwm_get_regs(hi_u32 pwm_type, hi_u32 pwm_no, pwm_reg *pst_regs)
{
    hi_u32 chip;

    if (pst_regs == HI_NULL) {
        HI_ERR_PWM("ERR: pst_regs is null\n");
        return HI_FAILURE;
    }

    chip = pwm_drv_get_chip_version();
    if (chip == PWM_DRV_CHIP_V110) {
        if (pwm_no > PWM_MAX_CHN_NUM) {
            HI_ERR_PWM("PWM Type:%u Channel:%u is invalid, Channel valid range is [0,%u]\n",
                pwm_type, pwm_no, PWM_MAX_CHN_NUM);
            return HI_FAILURE;
        }

        if (pwm_type != 0 && pwm_type != 1) {
            HI_ERR_PWM("PWM Type:%u is invalid, valid range is [%u,%u].\n", pwm_type, 0, 1);
            return HI_FAILURE;
        }
        pst_regs->cfg = (uintptr_t)(g_s3_pwm_reg_base[pwm_type] + pwm_no * PWM_MODE_OFFSET + PWM_REG_CFG);
        pst_regs->ctrl = (uintptr_t)(g_s3_pwm_reg_base[pwm_type] + pwm_no * PWM_MODE_OFFSET + PWM_REG_CTRL);
        pst_regs->state0 = (uintptr_t)(g_s3_pwm_reg_base[pwm_type] + pwm_no * PWM_MODE_OFFSET + PWM_REG_STATE0);
        pst_regs->state1 = (uintptr_t)(g_s3_pwm_reg_base[pwm_type] + pwm_no * PWM_MODE_OFFSET + PWM_REG_STATE1);

        return HI_SUCCESS;
    }

    if (pwm_no > 3 && pwm_no != HI_BOARD_LED_PWM_NUM) { /* PWM0-PWM3 、LED PWM */
        HI_ERR_PWM("ERR: invalid param\n");
        return HI_FAILURE;
    }

    if (pwm_no == HI_BOARD_LED_PWM_NUM) { /* Led PWM */
        pst_regs->cfg = (uintptr_t)(g_led_pwm_reg_base + PWM_REG_CFG);
        pst_regs->ctrl = (uintptr_t)(g_led_pwm_reg_base + PWM_REG_CTRL);
        pst_regs->state0 = (uintptr_t)(g_led_pwm_reg_base + PWM_REG_STATE0);
        pst_regs->state1 = (uintptr_t)(g_led_pwm_reg_base + PWM_REG_STATE1);
    } else { /* PWM 0 -  PWM3 */
        pst_regs->cfg = (uintptr_t)(g_pwm_reg_base + pwm_no * PWM_MODE_OFFSET + PWM_REG_CFG);
        pst_regs->ctrl = (uintptr_t)(g_pwm_reg_base + pwm_no * PWM_MODE_OFFSET + PWM_REG_CTRL);
        pst_regs->state0 = (uintptr_t)(g_pwm_reg_base + pwm_no * PWM_MODE_OFFSET + PWM_REG_STATE0);
        pst_regs->state1 = (uintptr_t)(g_pwm_reg_base + pwm_no * PWM_MODE_OFFSET + PWM_REG_STATE1);
    }

    return HI_SUCCESS;
}
#endif

hi_s32 drv_pwm_init(hi_void)
{
#if !defined(CHIP_TYPE_HI3796CV300) && !defined(CHIP_TYPE_HI3751V900)
    U_PERI_CRG23 un_pwm_crg23 = {0};
    hi_s32 ret = 0;
    hi_u32 chip;

    chip = pwm_drv_get_chip_version();
    if (chip == PWM_DRV_CHIP_V110) {
        ret = osal_exportfunc_get(HI_ID_GPIO, (HI_VOID **)&g_gpio_func);
        if (ret != HI_SUCCESS) {
            HI_ERR_PWM("get gpio function err!\n");
            hi_err_print_err_code(ret);
            return ret;
        }

        HI_PWM_REG_REMAP(g_s3_pwm_reg_base[0], HI_GPIO0_PWM_REG_BASE, ret);
        if (ret == HI_FAILURE) {
            return ret;
        }
        HI_PWM_REG_REMAP(g_s3_pwm_reg_base[1], HI_GPIO5_PWM_REG_BASE, ret);
        if (ret == HI_FAILURE) {
            return ret;
        }
        /* 解复位 */
        osal_writel(0x15, (0xf8a22000 + 0x0200)); /* value: 0x15, regaddr: 0xf8a22000, offset: 0x0200 */
    } else {
        HI_PWM_REG_REMAP(g_pwm_reg_base, PWM_BASE_ADDR, ret);
        if (ret == HI_FAILURE) {
            return ret;
        }
        HI_PWM_REG_REMAP(g_led_pwm_reg_base, LED_PWM_BASE_ADDR, ret);
        if (ret == HI_FAILURE) {
            return ret;
        }

        un_pwm_crg23.u32 = g_pst_reg_crg->PERI_CRG23.u32;
        un_pwm_crg23.bits.pwm0_cken = 1;
        un_pwm_crg23.bits.pwm0_srst_req = 0;
        g_pst_reg_crg->PERI_CRG23.u32 = un_pwm_crg23.u32;
    }
#endif
    return HI_SUCCESS;
}

hi_s32 drv_pwm_deinit(hi_void)
{
#if !defined(CHIP_TYPE_HI3796CV300) && !defined(CHIP_TYPE_HI3751V900)
    U_PERI_CRG23 un_pwm_crg23 = {0};
    hi_u32 chip;

    chip = pwm_drv_get_chip_version();
    if (chip == PWM_DRV_CHIP_V110) {
        HI_PWM_REG_UNMAP((hi_void *)g_s3_pwm_reg_base[0]);
        g_s3_pwm_reg_base[0] = HI_NULL;
        HI_PWM_REG_UNMAP((hi_void *)g_s3_pwm_reg_base[1]);
        g_s3_pwm_reg_base[1] = HI_NULL;
        osal_writel(0x205, (0xf8a22000 + 0x0200)); /* value: 0x205, regaddr: 0xf8a22000, offset: 0x0200 */
    } else {
        HI_PWM_REG_UNMAP((hi_void *)g_pwm_reg_base);
        g_pwm_reg_base = HI_NULL;
        HI_PWM_REG_UNMAP((hi_void *)g_led_pwm_reg_base);
        g_led_pwm_reg_base = HI_NULL;

        un_pwm_crg23.bits.pwm0_cken = 0;
        un_pwm_crg23.bits.pwm0_srst_req = 0;
        g_pst_reg_crg->PERI_CRG23.u32 = un_pwm_crg23.u32;
    }
#endif
    return HI_SUCCESS;
}

hi_s32 pwm_get_state(hi_u32 pwm_type, hi_u32 pwm_no, pwm_para *pst_attr)
{
#if !defined(CHIP_TYPE_HI3796CV300) && !defined(CHIP_TYPE_HI3751V900)
    hi_s32 ret;
    pwm_reg st_regs = {0};
    hi_u32 state0, state1;

    if (pst_attr == HI_NULL) {
        HI_ERR_PWM("pst_attr is null!\n");
        return HI_FAILURE;
    }

    ret = pwm_get_regs(pwm_type, pwm_no, &st_regs);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    state0 = osal_readl((HI_VOID *)st_regs.state0);
    state1 = osal_readl((HI_VOID *)st_regs.state1);
    HI_INFO_PWM("State0 = 0x%x, State1 = 0x%x\n", state0, state1);
    pst_attr->freq = PWM_CLK_FREQ / (0x1 + state0);
    pst_attr->duty_ratio = (unsigned int)((state1 + PWM_FLOAT_VAL1) /
        (state0 + PWM_FLOAT_VAL1) * PWM_FLOAT_VAL4 / PWM_FLOAT_VAL2 + PWM_FLOAT_VAL0);
#endif
    return HI_SUCCESS;
}


hi_s32 pwm_get_attr(hi_u32 pwm_type, hi_u32 pwm_no, pwm_para *pst_attr)
{
#if !defined(CHIP_TYPE_HI3796CV300) && !defined(CHIP_TYPE_HI3751V900)
    hi_s32 ret;
    pwm_reg st_regs = {0};
    volatile hi_u32 pwm_cfg = 0;
    volatile hi_u32 pwm_ctrl = 0;
    volatile hi_u32 Pwm_inv;
    hi_u32 pwm_clk_freq, chip;

    pwm_clk_freq = PWM_CLK_FREQ;

    ret = pwm_get_regs(pwm_type, pwm_no, &st_regs);
    if (ret != HI_SUCCESS) {
        HI_ERR_PWM("ERR: call pwm_get_regs fail\n");
        return ret;
    }

    if (pst_attr == HI_NULL) {
        HI_ERR_PWM("ERR: pst_regs is null\n");
        return HI_FAILURE;
    }

    chip = pwm_drv_get_chip_version();
    if (chip == PWM_DRV_CHIP_V110) {
        pwm_cfg  = osal_readl((HI_VOID *)st_regs.cfg);
        pwm_ctrl = osal_readl((HI_VOID *)st_regs.ctrl);
        HI_INFO_PWM("Cfg = 0x%x, Ctrl = 0x%x\n", pwm_cfg, pwm_ctrl);
        pst_attr->freq = pwm_clk_freq / (0x1 + (pwm_cfg >> 2));  /* 2 bit */
        pst_attr->duty_ratio = (hi_u32)(((pwm_ctrl >> 2) + PWM_FLOAT_VAL1) / /* 2 bit */
            ((pwm_cfg >> 2) + PWM_FLOAT_VAL1) * PWM_FLOAT_VAL4 / PWM_FLOAT_VAL2 + PWM_FLOAT_VAL0); /* 2 bit */
        return HI_SUCCESS;
    }

    /* 350/560 */
    pwm_cfg = ((uintptr_t)osal_readl((hi_void *)((uintptr_t)(st_regs.cfg)))
        & 0xfffffff0) >> 4; /* 右移4位，获取周期值 */
    pwm_ctrl = ((uintptr_t)osal_readl((hi_void *)((uintptr_t)(st_regs.ctrl)))
        & 0xfffffff0) >> 4; /* 右移4位，获取周期内的高电平时长 */
    Pwm_inv = ((uintptr_t)osal_readl((hi_void *)((uintptr_t)(st_regs.ctrl))) & 0x2) >> 1;
    if (Pwm_inv == 1) {
        if (pwm_ctrl >= pwm_cfg) {
            pst_attr->duty_ratio = 0;
        } else {
            pst_attr->duty_ratio = (pwm_cfg - pwm_ctrl) * 1000 / (pwm_cfg + 1); /* 计算占空比，1000为ms转us */
        }
    } else {
        pst_attr->duty_ratio = (pwm_ctrl + 1) * 1000 / (pwm_cfg + 1); /* 计算占空比，1000为ms转us */
    }

    pst_attr->freq = pwm_clk_freq / (pwm_cfg + 1);
#endif
    return HI_SUCCESS;
}

#if !defined(CHIP_TYPE_HI3796CV300) && !defined(CHIP_TYPE_HI3751V900)
static hi_s32 pwm_set_attr(hi_u32 pwm_type, hi_u32 pwm_no, pwm_attr *pst_attr)
{
    hi_s32 ret;
    pwm_reg st_regs = {0};
    hi_u32 pwm_cfg, pwm_clk_freq, chip, pwm_ctrl;

    pwm_clk_freq = PWM_CLK_FREQ;

    if (pst_attr == HI_NULL) {
        HI_ERR_PWM("ERR: pst_regs is null\n");
        return HI_FAILURE;
    }

    chip = pwm_drv_get_chip_version();

    ret = pwm_get_regs(pwm_type, pwm_no, &st_regs);
    if (ret != HI_SUCCESS) {
        HI_ERR_PWM("ERR: call pwm_get_regs fail\n");
        return ret;
    }

    if (chip == PWM_DRV_CHIP_V110) {
        pwm_cfg = pwm_clk_freq / pst_attr->freq;
        HI_INFO_PWM("pwm_cfg:%u,0x%x\n", pwm_cfg, pwm_cfg);
        pwm_ctrl = ((pwm_cfg * (pst_attr->duty_ratio / PWM_FLOAT_VAL3)));
        HI_INFO_PWM("pwm_ctrl:%u,0x%x\n", pwm_ctrl, pwm_ctrl);

        if (pwm_ctrl < 1) {
            HI_ERR_PWM("Freq and DutyRatio lead to an invalid CtrlReg Value.\n");
            return HI_FAILURE;
        }
    } else {
        pwm_cfg = pwm_clk_freq / pst_attr->freq - 1;
        if (pst_attr->freq <= PWM_FREQ_CRITICAL) {
            pwm_ctrl = pwm_clk_freq / pst_attr->freq / 1000 * pst_attr->duty_ratio; /* 除1000 s转成ms值 */
        } else {
            pwm_ctrl = pwm_clk_freq / pst_attr->freq * pst_attr->duty_ratio / 1000 - 1; /* 除1000  s转成ms值 */
        }

        pwm_cfg = pwm_cfg << 4; /* 左移4位 配置cfg寄存器 */
        pwm_ctrl = pwm_ctrl << 4; /* 左移4 位配置ctrl寄存器 */

        if (pst_attr->duty_ratio == 0) {
            pwm_ctrl = pwm_cfg | 0x3;
        }
    }
    osal_writel (pwm_cfg, (hi_void *)((uintptr_t)(st_regs.cfg)));
    osal_writel (pwm_ctrl, (hi_void *)((uintptr_t)(st_regs.ctrl)));
    return HI_SUCCESS;
}
#endif

hi_s32 pwm_send_signal(hi_u32 pwm_no, hi_u32 carrier_sig_duration_ms, hi_u32 low_level_sig_duration_ms)
{
#if !defined(CHIP_TYPE_HI3796CV300) && !defined(CHIP_TYPE_HI3751V900)
    hi_s32 ret;
    pwm_reg st_regs = {0};
    volatile hi_u32 pwm_state0;
    volatile hi_u32 pwm_state1;
    volatile hi_u32 pwm_cfg;
    volatile hi_u32 pwm_ctrl;

    ret = pwm_get_regs(0, pwm_no, &st_regs);
    if (ret != HI_SUCCESS) {
        HI_ERR_PWM("ERR: call pwm_get_regs fail\n");
        return ret;
    }

    if (carrier_sig_duration_ms > 20 || low_level_sig_duration_ms > 20) { /* max wait time is 20 ms */
        HI_ERR_PWM("ERR: carrier_sig_duration_ms or low_level_sig_duration_ms over range\n");
        return HI_FAILURE;
    }
    /* enable pwm */
    pwm_ctrl = osal_readl((hi_void *)st_regs.ctrl);
    pwm_ctrl = pwm_ctrl | 0x01;
    osal_writel(pwm_ctrl, (hi_void *)st_regs.ctrl);

    /* confirm state is right */
    pwm_state0 = ((uintptr_t)(osal_readl((hi_void *)(uintptr_t)st_regs.state0)))
        & 0x0fffffff;

    pwm_state1 = ((uintptr_t)(osal_readl((hi_void *)(uintptr_t)st_regs.state1)))
        & 0x0fffffff;

    pwm_cfg = ((uintptr_t)(osal_readl((hi_void *)st_regs.cfg))
        & 0xfffffff0) >> 4; /* 右移4位，获取周期值 */

    pwm_ctrl = ((uintptr_t)(osal_readl((hi_void *)st_regs.ctrl))
        & 0xfffffff0) >> 4; /* 右移4位，获取周期内的高电平时长 */
    if (pwm_state0 != pwm_cfg && pwm_state1 != pwm_ctrl) {
        HI_ERR_PWM("ERR: call Pwm state is not right\n");
        return HI_FAILURE;
    }

    /* output carrier signal for needed times */
    osal_msleep(carrier_sig_duration_ms);

    /* disable pwm */
    pwm_ctrl = osal_readl((hi_void *)(uintptr_t)st_regs.ctrl);
    pwm_ctrl = pwm_ctrl & (~0x1);
    osal_writel(pwm_ctrl, (hi_void *)st_regs.ctrl);
    /* output low-level signal for needed times */
    osal_msleep(low_level_sig_duration_ms);
#endif
    return HI_SUCCESS;
}

hi_s32 pwm_set_enable(hi_u32 pwm_type, hi_u32 pwm_no, hi_bool b_enable)
{
#if !defined(CHIP_TYPE_HI3796CV300) && !defined(CHIP_TYPE_HI3751V900)
    hi_s32 ret;
    pwm_reg st_regs = {0};
    hi_u32 pwm_ctrl, chip, gpio_no;

    chip = pwm_drv_get_chip_version();

    ret = pwm_get_regs(pwm_type, pwm_no, &st_regs);
    if (ret != HI_SUCCESS) {
        HI_ERR_PWM("ERR: call pwm_get_regs fail\n");
        return ret;
    }

    if ((b_enable != HI_TRUE) && (b_enable != HI_FALSE)) {
        HI_ERR_PWM("ERR: invalid param!\n");
        return HI_FAILURE;
    }

    pwm_ctrl = osal_readl((hi_void *)(uintptr_t)st_regs.ctrl);
    if (b_enable == HI_TRUE) {
        if (chip == PWM_DRV_CHIP_V110) {
            /* enable pwm */
            gpio_no = HI_PWM_GPIO_GROUP * 8 + pwm_no; /* 8 pin */
            ret = g_gpio_func->pfn_gpio_dir_set_bit(gpio_no, 0x1); /* set dir out */
            if (ret != HI_SUCCESS) {
                return ret;
            }
            pwm_ctrl = pwm_ctrl | 0x5; /* value: 0x5 */
        } else {
            pwm_ctrl = pwm_ctrl | 0x01;
        }
    } else {
        if (chip == PWM_DRV_CHIP_V110) {
            ret = g_gpio_func->pfn_gpio_dir_set_bit(gpio_no, 0x0); /* set dir in */
            if (ret != HI_SUCCESS) {
                return ret;
            }
        }
        pwm_ctrl = pwm_ctrl & ((hi_u32)0xfffffffe | 0x0);
    }
    HI_INFO_PWM("pwm_ctrl val=0x%x\n", pwm_ctrl);
    osal_writel(pwm_ctrl, (hi_void *)(uintptr_t)st_regs.ctrl);
#endif
    return HI_SUCCESS;
}

#if !defined(CHIP_TYPE_HI3796CV300) && !defined(CHIP_TYPE_HI3751V900)
static hi_s32 pwm_proc_read(hi_void *seqfile, hi_void *private)
{
    return HI_SUCCESS;
}

static hi_void pwm_proc_write_cmd_process(hi_u32 cmd, hi_u32 pwm_no, hi_u32 value)
{
    hi_u32 ret = HI_SUCCESS;
    pwm_reg st_regs = {0};
    hi_u32 pwm_ctrl = 0;

    switch (cmd) { /* argv 有4组 */
        case 0: { /* 0条件判断是否pwm处于使能能状态 */
            ret = pwm_get_regs(0, pwm_no, &st_regs);
            if (ret != HI_SUCCESS) {
                HI_ERR_PWM("ERR: call pwm_get_regs fail\n");
                break;
            }
            pwm_ctrl = osal_readl((hi_void *)st_regs.ctrl);
            value = pwm_ctrl & 0x1;

            HI_PRINT("PWM%d  = %s\n", pwm_no, value ? "enable" : "disable");

            break;
        }
        case 1: { /* 1条件使能pwm */
            ret = pwm_get_regs(0, pwm_no, &st_regs);
            if (ret != HI_SUCCESS) {
                HI_ERR_PWM("ERR: call pwm_get_regs fail\n");
                break;
            }
            pwm_ctrl = osal_readl((hi_void *)st_regs.ctrl);

            if (value == 1) {
                pwm_ctrl = pwm_ctrl | value;
            } else {
                pwm_ctrl = pwm_ctrl & (0xfffffffe | value);
            }

            osal_writel(pwm_ctrl, (hi_void *)(uintptr_t)st_regs.ctrl);

            HI_PRINT("PWM%d  = %s\n", pwm_no, value ? "enable" : "disable");

            break;
        }
        default:
        {
            HI_PRINT("---------------------------------Hisilicon PWM Debug--------------------------------\n");
            HI_PRINT("echo command para1 para2         path          explanation\n");
            HI_PRINT("echo gpwm  [group][bit]       > /proc/msp/pwm Get pwm state,(enable or disable)\n");
            HI_PRINT("echo spwm  [group][bit] [0/1] > /proc/msp/pwm Set pwm state,1:enable 0:disable\n");
            HI_PRINT("-------------------------------------------------------------------------------------\n");
        }
    }
}

static hi_s32 pwm_proc_cmd_help(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    HI_PRINT("chip : 350 \n");
    HI_PRINT("pwm_no =0 : gpio1_0; pwm_no = 1 : gpio1_1; pwm_no = 2 : gpio3_1");
    HI_PRINT("pwm_no = 3 : gpio3_5; pwm_no = 4 : gpio18_1\n");
    HI_PRINT("chip : 560\n");
    HI_PRINT("pwm_no =0 : gpio4_1; pwm_no = 1 : gpio4_2; pwm_no = 2 : gpio0_2\n");
    HI_PRINT("pwm_no = 3 : gpio0_3; pwm_no = 4 : gpio18_1\n");
    HI_PRINT("---------------------------------Hisilicon PWM Debug--------------------------------\n");
    HI_PRINT("echo command para1 para2         path          explanation\n");
    HI_PRINT("echo gpwm  [pwm_no] > /proc/msp/pwm Get pwm state,(enable or disable)\n");
    HI_PRINT("echo spwm  [pwm_no]  [0/1] > /proc/msp/pwm Set pwm state,1:enable 0:disable\n");
    HI_PRINT("-------------------------------------------------------------------------------------\n");
    return HI_SUCCESS;
}

static hi_s32 pwm_proc_cmd_spwm(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    pwm_proc_data proc_data = {0};

    if (argc < 3) { /* argv 有3组 */
        HI_PRINT("chip : 350 \n");
        HI_PRINT("pwm_no =0 : gpio1_0; pwm_no = 1 : gpio1_1; pwm_no = 2 : gpio3_1");
        HI_PRINT("pwm_no = 3 : gpio3_5; pwm_no = 4 : gpio18_1\n");
        HI_PRINT("chip : 560\n");
        HI_PRINT("pwm_no =0 : gpio4_1; pwm_no = 1 : gpio4_2; pwm_no = 2 : gpio0_2\n");
        HI_PRINT("pwm_no = 3 : gpio0_3; pwm_no = 4 : gpio18_1\n");
        HI_PRINT("---------------------------------Hisilicon PWM Debug--------------------------------\n");
        HI_PRINT("echo command para1 para2         path          explanation\n");
        HI_PRINT("echo gpwm  [pwm_no] > /proc/msp/pwm Get pwm state,(enable or disable)\n");
        HI_PRINT("echo spwm  [pwm_no]  [0/1] > /proc/msp/pwm Set pwm state,1:enable 0:disable\n");
        HI_PRINT("-------------------------------------------------------------------------------------\n");
        return HI_FAILURE;
    }

    proc_data.pwm_no = osal_strtol(argv[1], NULL, 10); /* 字符串转成10进制 */
    proc_data.value = osal_strtol(argv[2], NULL, 10); /* 第2组字符转成10进制 */
    proc_data.cmd = 1; /* 1 :  set pwm bit */

    pwm_proc_write_cmd_process(proc_data.cmd, proc_data.pwm_no, proc_data.value);

    return HI_SUCCESS;
}

static hi_s32 pwm_proc_cmd_gpwm(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    pwm_proc_data proc_data = {0};

    if (argc < 2) { /* argv 有2组 */
        HI_PRINT("chip : 350 \n");
        HI_PRINT("pwm_no =0 : gpio1_0; pwm_no = 1 : gpio1_1; pwm_no = 2 : gpio3_1");
        HI_PRINT("pwm_no = 3 : gpio3_5; pwm_no = 4 : gpio18_1\n");
        HI_PRINT("chip : 560\n");
        HI_PRINT("pwm_no =0 : gpio4_1; pwm_no = 1 : gpio4_2; pwm_no = 2 : gpio0_2\n");
        HI_PRINT("pwm_no = 3 : gpio0_3; pwm_no = 4 : gpio18_1\n");
        HI_PRINT("---------------------------------Hisilicon PWM Debug--------------------------------\n");
        HI_PRINT("echo command para1 para2         path          explanation\n");
        HI_PRINT("echo gpwm  [pwm_no] > /proc/msp/pwm Get pwm state,(enable or disable)\n");
        HI_PRINT("echo spwm  [pwm_no]  [0/1] > /proc/msp/pwm Set pwm state,1:enable 0:disable\n");
        HI_PRINT("-------------------------------------------------------------------------------------\n");
        return HI_FAILURE;
    }

    proc_data.pwm_no = osal_strtol(argv[1], NULL, 10); /* 字符串转成10进制 */
    proc_data.cmd = 0;

    pwm_proc_write_cmd_process(proc_data.cmd, proc_data.pwm_no, proc_data.value);

    return HI_SUCCESS;
}

static osal_proc_cmd g_pwm_proc_cmd[] = {
    {"help", pwm_proc_cmd_help},
    {"spwm", pwm_proc_cmd_spwm},
    {"gpwm", pwm_proc_cmd_gpwm},
};
#endif

#if 1//!defined(CHIP_TYPE_HI3796CV300) && !defined(CHIP_TYPE_HI3751V900)

hi_s32 pwm_drv_open(void *private_data)
{
    return HI_SUCCESS;
}

hi_s32 pwm_drv_close(void *private_data)
{
    return HI_SUCCESS;
}

hi_s32 pwm_suspend(void *private_data)
{
    return HI_SUCCESS;
}

hi_s32 pwm_resume(void *private_data)
{
    return HI_SUCCESS;
}

hi_s32 pwm_lowpower_enter(hi_void *private_data)
{
    return HI_SUCCESS;
}

hi_s32 pwm_lowpower_exit(hi_void *private_data)
{
    return HI_SUCCESS;
}

static hi_s32 pwm_get_attr_ioctl(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    errno_t err_ret;
    pwm_attr_cmd_para st_attr_cmd = { 0, 0, { 0, 0 } };

    if (arg == NULL || private_data == NULL) {
        HI_ERR_PWM("param is null!\n");
        return HI_FAILURE;
    }

    ret = osal_sem_down_interruptible(&g_pwm_sem_intf);
    if (ret) {
        HI_ERR_PWM("Semaphore lock is error. \n");
        return HI_FAILURE;
    }

    err_ret = memcpy_s(&st_attr_cmd, sizeof(pwm_attr_cmd_para),
        (pwm_attr_cmd_para *)arg, sizeof(pwm_attr_cmd_para));
    if (err_ret != EOK) {
        HI_ERR_PWM("secure func call error\n");
        osal_sem_up(&g_pwm_sem_intf);
        return HI_FAILURE;
    }

    if (cmd == CMD_PWM_GETSTATE) {
        ret = pwm_get_state(st_attr_cmd.pwm_type, st_attr_cmd.pwm_no, &st_attr_cmd.st_pwm_attr);
    } else if (cmd == CMD_PWM_GETATTR) {
        ret = pwm_get_attr(st_attr_cmd.pwm_type, st_attr_cmd.pwm_no, &st_attr_cmd.st_pwm_attr);
    }
    err_ret = memcpy_s((pwm_attr_cmd_para *)arg, sizeof(pwm_attr_cmd_para),
        &st_attr_cmd, sizeof(pwm_attr_cmd_para));
    if (err_ret != EOK) {
        HI_ERR_PWM("secure func call error\n");
        osal_sem_up(&g_pwm_sem_intf);
        return HI_FAILURE;
    }

    osal_sem_up(&g_pwm_sem_intf);

    return HI_SUCCESS;
}

static hi_s32 pwm_set_attr_ioctl(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
#if !defined(CHIP_TYPE_HI3796CV300) && !defined(CHIP_TYPE_HI3751V900)
    hi_s32 ret;
    errno_t err_ret;
    pwm_attr_cmd_para st_attr_cmd = { 0, 0, { 0, 0 } };

    if (arg == NULL || private_data == NULL) {
        HI_ERR_PWM("param is null!\n");
        return HI_FAILURE;
    }

    ret = osal_sem_down_interruptible(&g_pwm_sem_intf);
    if (ret) {
        HI_ERR_PWM("Semaphore lock is error. \n");
        return HI_FAILURE;
    }

    err_ret = memcpy_s(&st_attr_cmd, sizeof(pwm_attr_cmd_para),
        (pwm_attr_cmd_para *)arg, sizeof(pwm_attr_cmd_para));
    if (err_ret != EOK) {
        HI_ERR_PWM("secure func call error\n");
        osal_sem_up(&g_pwm_sem_intf);
        return HI_FAILURE;
    }

    if (st_attr_cmd.st_pwm_attr.freq == 0) {
        HI_ERR_PWM("ERR: Freq is 0\n");
        osal_sem_up(&g_pwm_sem_intf);
        return HI_FAILURE;
    }

    if (st_attr_cmd.st_pwm_attr.duty_ratio <= 0 ||
        st_attr_cmd.st_pwm_attr.duty_ratio > 1000) { /* 1000 duty_ratio max */
        HI_ERR_PWM("Invalid DutyRatio[%u]\n", st_attr_cmd.st_pwm_attr.duty_ratio);
        osal_sem_up(&g_pwm_sem_intf);
        return HI_FAILURE;
    }

    ret = pwm_set_attr(st_attr_cmd.pwm_type, st_attr_cmd.pwm_no, &st_attr_cmd.st_pwm_attr);

    osal_sem_up(&g_pwm_sem_intf);
#endif
    return HI_SUCCESS;
}

static hi_s32 pwm_send_signal_ioctl(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    errno_t err_ret;
    pwm_signal_cmd_para st_sig_cmd = {0, 0, 0};

    if (arg == NULL || private_data == NULL) {
        HI_ERR_PWM("param is null!\n");
        return HI_FAILURE;
    }

    ret = osal_sem_down_interruptible(&g_pwm_sem_intf);
    if (ret) {
        HI_ERR_PWM("Semaphore lock is error. \n");
        return HI_FAILURE;
    }

    err_ret = memcpy_s(&st_sig_cmd, sizeof(pwm_signal_cmd_para),
        (pwm_signal_cmd_para *)arg, sizeof(pwm_signal_cmd_para));
    if (err_ret != EOK) {
        HI_ERR_PWM("secure func call error\n");
        osal_sem_up(&g_pwm_sem_intf);
        return HI_FAILURE;
    }

    ret = pwm_send_signal(st_sig_cmd.pwm_no, st_sig_cmd.carrier_sig_duration_us,
        st_sig_cmd.low_level_sig_duration_us);

    osal_sem_up(&g_pwm_sem_intf);

    return HI_SUCCESS;
}

static hi_s32 pwm_set_enable_ioctl(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    errno_t err_ret;
    pwm_enable_cmd_para stEnableCmd = {0, 0, HI_FALSE};

    ret = osal_sem_down_interruptible(&g_pwm_sem_intf);
    if (ret) {
        HI_ERR_PWM("Semaphore lock is error. \n");
        return HI_FAILURE;
    }

    if (arg == NULL || private_data == NULL) {
        HI_ERR_PWM("param is null!\n");
        osal_sem_up(&g_pwm_sem_intf);
        return HI_FAILURE;
    }

    err_ret = memcpy_s(&stEnableCmd, sizeof(pwm_enable_cmd_para),
        (pwm_enable_cmd_para *)arg, sizeof(pwm_enable_cmd_para));
    if (err_ret != EOK) {
        HI_ERR_PWM("secure func call error\n");
        osal_sem_up(&g_pwm_sem_intf);
        return HI_FAILURE;
    }

    ret = pwm_set_enable(stEnableCmd.pwm_type, stEnableCmd.pwm_no, stEnableCmd.b_enable);

    osal_sem_up(&g_pwm_sem_intf);

    return HI_SUCCESS;
}

#endif

#if 1// !defined(CHIP_TYPE_HI3796CV300) && !defined(CHIP_TYPE_HI3751V900)
static osal_ioctl_cmd pwm_cmd_list[] = {
    { CMD_PWM_GETATTR, pwm_get_attr_ioctl },
    { CMD_PWM_SETATTR, pwm_set_attr_ioctl },
    { CMD_PWM_SENDSIGNAL, pwm_send_signal_ioctl },
    { CMD_PWM_SETENABLE, pwm_set_enable_ioctl },
    { CMD_PWM_GETSTATE, pwm_get_attr_ioctl },
};

static osal_fileops pwm_fileops = {
    .read = NULL,
    .write = NULL,
    .open = pwm_drv_open,
    .release = pwm_drv_close,
    .cmd_list = pwm_cmd_list,
    .cmd_cnt = 0,
};

static osal_pmops pwm_pmops = {
    .pm_suspend = pwm_suspend,
    .pm_resume =  pwm_resume,
    .pm_lowpower_enter = pwm_lowpower_enter,
    .pm_lowpower_exit = pwm_lowpower_exit,
    .pm_poweroff = NULL, /* shutdown */
    .private_data = &g_pwm_state,
};

static osal_dev pwm_device = {
    .minor = HI_DEV_PWM_MINOR,
    .fops = &pwm_fileops,
    .pmops = &pwm_pmops,
};
#endif

hi_s32 pwm_drv_proc_add(hi_void)
{
#if !defined(CHIP_TYPE_HI3796CV300) && !defined(CHIP_TYPE_HI3751V900)
    hi_s32 ret;
    hi_char proc_name[16] = {0}; /* 存放驱动名，最大长度16 */
    osal_proc_entry *pwm_proc_entry = NULL;
    hi_u32 len;
    ret = snprintf_s(proc_name, sizeof(proc_name), sizeof(proc_name) - 1, "%s", HI_MOD_PWM);
    if (ret < 0) {
        HI_ERR_PWM("secure func call error\n");
        return HI_FAILURE;
    }

    len = strlen(proc_name);
    pwm_proc_entry = osal_proc_add(proc_name, len);
    if (pwm_proc_entry == HI_NULL) {
        HI_ERR_PWM("gpio add proc failed!\n");
        return HI_FAILURE;
    }

    len = sizeof(g_pwm_proc_cmd) / sizeof(osal_proc_cmd);
    pwm_proc_entry->read = pwm_proc_read;
    pwm_proc_entry->cmd_cnt = len;
    pwm_proc_entry->cmd_list = g_pwm_proc_cmd;
    pwm_proc_entry->private = NULL;
#endif
    return HI_SUCCESS;
}

hi_s32 pwm_drv_proc_del(hi_s8 *proc_name)
{
#if !defined(CHIP_TYPE_HI3796CV300) && !defined(CHIP_TYPE_HI3751V900)
    hi_u32 len;

    if (proc_name == NULL) {
        HI_ERR_PWM("proc_name is null\n");
        return HI_FAILURE;
    }

    len = strlen(proc_name);
    osal_proc_remove(proc_name, len);
#endif
    return HI_SUCCESS;
}

hi_s32 pwm_drv_mod_init(hi_void)
{
    hi_s32 ret;

    ret = pwm_drv_proc_add();
    if (ret != HI_SUCCESS) {
        HI_ERR_PWM("call pwm_drv_proc_add failed!\n");
        return HI_FAILURE;
    }

    osal_sem_init(&g_pwm_sem_intf, 1);

    pwm_fileops.cmd_cnt = sizeof(pwm_cmd_list) / sizeof(osal_ioctl_cmd);

    ret = snprintf_s(pwm_device.name, sizeof(pwm_device.name),
        sizeof(pwm_device.name) - 1, "hi_pwm");
    if (ret < 0) {
        HI_ERR_PWM("secure func call error\n");
        return HI_FAILURE;
    }

	ret = osal_dev_register(&pwm_device);
    if (ret != HI_SUCCESS) {
        HI_ERR_PWM("register pwm failed.\n");
        return HI_FAILURE;
    }
#if !defined(CHIP_TYPE_HI3796CV300) && !defined(CHIP_TYPE_HI3751V900)
    ret = drv_pwm_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_PWM("Pwm int failure!\n");
        osal_dev_unregister(&pwm_device);
        return HI_FAILURE;
    }
#endif

#ifdef MODULE
    HI_PRINT("Load hi_pwm.ko success.\t(%s)\n", VERSION_STRING);
#endif
    return HI_SUCCESS;
}

hi_void pwm_drv_mod_exit(hi_void)
{
    hi_s32 ret;
    hi_char proc_name[16] = {0}; /* 存放驱动名，最大长度16 */
    hi_char *name = NULL;
    osal_sem_destory(&g_pwm_sem_intf);
    osal_dev_unregister(&pwm_device);

    ret = snprintf_s(proc_name, sizeof(proc_name),
                     sizeof(proc_name) - 1, "%s", "pwm");
    if (ret < 0) {
        HI_ERR_PWM("secure func call error\n");
        return;
    }
    name = proc_name;
    ret = pwm_drv_proc_del(name);
    if (ret != HI_SUCCESS) {
        HI_ERR_PWM("gpio_drv_proc_del failure!\n");
    }

#if !defined(CHIP_TYPE_HI3796CV300) && !defined(CHIP_TYPE_HI3751V900)
    ret = drv_pwm_deinit();
    if (ret != HI_SUCCESS) {
        HI_ERR_PWM("drv_pwm_deinit failure!\n");
    }
#endif
}

#ifdef MODULE
module_init(pwm_drv_mod_init);
module_exit(pwm_drv_mod_exit);
#endif

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");
