/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2018. All rights reserved.
 * Description: lsadc
 * Author: DPT_BSP
 * Create: 2018-12-22
 */


#include "drv_lsadc_ioctl.h"
#include "drv_lsadc_priv.h"

#include "linux/hisilicon/securec.h"
#include "hi_errno.h"
#include "hi_osal.h"
#include <linux/module.h>

#define LSADC_MAX_CHANNEL 4
static struct lsadc_priv g_lsadc_local = { 0 };

static unsigned int g_lsadcregsave[LSADC_MAX_CHANNEL] = { 0 };

static int lsadc_open(hi_void *private_data)
{
    return HI_SUCCESS;
}

static int lsadc_close(hi_void *private_data)
{
    return HI_SUCCESS;
}

static int lsadc_set_param(unsigned int cmd, void *arg, void *private_data)
{
    int ret;
    unsigned int value = 0;
    hi_lsadc_param lsadc_param;
    struct lsadc_priv *lsadc = &g_lsadc_local;

    if (arg == NULL) {
        HI_ERR_LSADC(" NULL param!\n");
        return HI_ERR_LSADC_INVALID_PARA;
    }

    ret = osal_sem_down_interruptible(&lsadc->sem);
    if (ret) {
        HI_ERR_LSADC(" down_interruptible err ! \n");
        return ret;
    }

    (void)memset_s(&lsadc_param, sizeof(lsadc_param), 0, sizeof(hi_lsadc_param));
    /* Stop */
    osal_writel(LSADC_STOP, lsadc->base + LSADC_CTRL8);

    /* Config */
    if (memcpy_s(&lsadc_param, sizeof(lsadc_param), arg, sizeof(hi_lsadc_param))) {
        HI_ERR_LSADC("copy data from user fail!\n");
        ret = (long)HI_ERR_LSADC_INVALID_PARA;
        return ret;
    }

    value |= (lsadc_param.active_bit & 0xFF) << 24;     /* 24 means Set active bit[24-31] */
    value |= (lsadc_param.data_delta & 0xF) << 20;      /* 20 means Set data delta[20-23] */
    value |= (lsadc_param.deglitch_bypass & 0x1) << 17; /* 17 means Set deglitch bypass[17] */
    value |= (lsadc_param.lsadc_reset & 0x1) << 15;     /* 15 means Set lsadc reset[15] */
    value |= (lsadc_param.power_down_mod & 0x1) << 14;  /* 14 means Set power down model[14] */
    value |= (lsadc_param.model_sel & 0x1) << 13;       /* 13 means  Set model sel[13] */
#if defined (CHIP_TYPE_hi3751v560)
    value |= ((lsadc_param.channel_mask) & 0x3) << 10;     /* 10 means Enable channel[10-11] */
#else
    value |= (lsadc_param.channel_mask & 0xF) << 8;     /* 8 means Enable channel[8-11] */
#endif
    value |= lsadc_param.lsadc_zero & 0xFF;             /* 0xFF means Set lsadc zero[0-7] */
    osal_writel(value, lsadc->base + LSADC_CTRL0);

    /* Set glitch sample */
    osal_writel(lsadc_param.glitch_sample, lsadc->base + LSADC_CTRL1);

    /* Set time scan */
    osal_writel(lsadc_param.time_scan, lsadc->base + LSADC_CTRL2);

    /* start */
    osal_writel(LSADC_START, lsadc->base + LSADC_CTRL7);

    osal_sem_up(&lsadc->sem);
    HI_UNUSED(arg);
    return ret;
}

static int lsadc_get_param(unsigned int cmd, void *arg, void *private_data)
{
    long ret;
    unsigned int value;
    hi_lsadc_param lsadc_param;
    struct lsadc_priv *lsadc = &g_lsadc_local;

    if (arg == NULL) {
        HI_ERR_LSADC(" NULL param!\n");
        return HI_ERR_LSADC_INVALID_PARA;
    }

    ret = osal_sem_down_interruptible(&lsadc->sem);
    if (ret) {
        HI_ERR_LSADC(" down_interruptible err ! \n");
        return ret;
    }

    (void)memset_s(&lsadc_param, sizeof(lsadc_param), 0, sizeof(hi_lsadc_param));

    value = osal_readl(lsadc->base + LSADC_CTRL0);
    lsadc_param.active_bit = (value >> 24) & 0xFF;      // [24-31]
    lsadc_param.data_delta = (value >> 20) & 0xF;       // [20-23]
    lsadc_param.deglitch_bypass = (value >> 17) & 0x1;  // [17]
    lsadc_param.lsadc_reset = (value >> 15) & 0x1;      // [15]
    lsadc_param.power_down_mod = (value >> 14) & 0x1;   // [14]
    lsadc_param.model_sel = (value >> 13) & 0x1;        // [13]
#if defined (CHIP_TYPE_hi3751v560)
    lsadc_param.channel_mask = (value >> 10) & 0x3;      // [10-11]
#else
    lsadc_param.channel_mask = (value >> 8) & 0xF;      // [8-11]
#endif
    lsadc_param.lsadc_zero = value & 0xFF;              // [0-7]

    lsadc_param.glitch_sample = osal_readl(lsadc->base + LSADC_CTRL1);
    lsadc_param.time_scan = osal_readl(lsadc->base + LSADC_CTRL2);

    if (arg == NULL) {
        HI_ERR_LSADC(" NULL param!\n");
        return HI_ERR_LSADC_INVALID_PARA;
    }

    if (memcpy_s(arg, sizeof(hi_lsadc_param), &lsadc_param, sizeof(hi_lsadc_param))) {
        HI_ERR_LSADC(" copy data to user fail!\n");
        ret = (long)HI_ERR_LSADC_INVALID_PARA;
    }
    osal_sem_up(&lsadc->sem);
    return ret;
}

#ifndef  __LITEOS__
static int lsadc_get_value(unsigned int cmd, void *arg, void *private_data)
{
    int ret;
    unsigned int value;
    struct lsadc_priv *lsadc = &g_lsadc_local;

    ret = osal_sem_down_interruptible(&lsadc->sem);
    if (ret) {
        HI_ERR_LSADC(" down_interruptible err ! \n");
        return ret;
    }
    value = osal_readl(lsadc->base + LSADC_CTRL3);
    if (memcpy_s(arg, sizeof(unsigned int), &value, sizeof(unsigned int))) {
        HI_ERR_LSADC(" copy data to user fail!\n");
        ret = (long)HI_ERR_LSADC_INVALID_PARA;
    }
    osal_sem_up(&lsadc->sem);
    return ret;
}
#endif

static hi_s32 lsadc_pm_suspend(void *private_data)
{
    struct lsadc_priv *lsadc = &g_lsadc_local;

    g_lsadcregsave[0] = osal_readl(lsadc->base + LSADC_CTRL0); /* 0 means channel 1 */
    g_lsadcregsave[1] = osal_readl(lsadc->base + LSADC_CTRL1); /* 1 means channel 2 */
    g_lsadcregsave[2] = osal_readl(lsadc->base + LSADC_CTRL2); /* 2 means channel 3 */
    g_lsadcregsave[3] = osal_readl(lsadc->base + LSADC_CTRL4); /* 3 means channel 4 */

    HI_INFO_LSADC("LSADC suspend OK\n");
    return HI_SUCCESS;
}
static hi_s32 lsadc_pm_resume(void *private_data)
{
    struct lsadc_priv *lsadc = &g_lsadc_local;

    osal_writel(g_lsadcregsave[0], lsadc->base + LSADC_CTRL0); /* 0 means channel 1 */
    osal_writel(g_lsadcregsave[1], lsadc->base + LSADC_CTRL1); /* 1 means channel 2 */
    osal_writel(g_lsadcregsave[2], lsadc->base + LSADC_CTRL2); /* 2 means channel 3 */
    osal_writel(g_lsadcregsave[3], lsadc->base + LSADC_CTRL4); /* 3 means channel 4 */

    osal_writel(0xffffffff, lsadc->base + LSADC_CTRL7);

    HI_INFO_LSADC("LSADC resume OK\n");
    return HI_SUCCESS;
}

static osal_pmops g_lsadc_drvops = {
    .pm_suspend = lsadc_pm_suspend,
    .pm_resume = lsadc_pm_resume,
    .pm_lowpower_enter = NULL,
    .pm_lowpower_exit = NULL,
    .pm_poweroff = NULL,          /* shutdown */
    .private_data = &g_lsadc_local,
};

static osal_ioctl_cmd g_lsadc_cmd_list[] = {
    { CMD_LSADC_SET_CONFIG,        lsadc_set_param },
    { CMD_LSADC_GET_CONFIG,        lsadc_get_param },
#ifndef  __LITEOS__
    { CMD_LSADC_GET_VALUE,         lsadc_get_value },
#endif
};

static osal_fileops g_lsadc_fops = {
    .open = lsadc_open,
    .release = lsadc_close,
    .cmd_list = g_lsadc_cmd_list,
    .cmd_cnt = 0,
};

static osal_dev g_lsadc_register_data = {
    .fops = &g_lsadc_fops,
    .name = UMAP_DEVNAME_LSADC,
    .minor = UMAP_MIN_MINOR_LSADC,
    .pmops = &g_lsadc_drvops,
};

int lsadc_drv_mod_init(void)
{
    struct lsadc_priv *lsadc = &g_lsadc_local;
    hi_s32 ret;

    g_lsadc_fops.cmd_cnt = sizeof(g_lsadc_cmd_list) / sizeof(osal_ioctl_cmd);

    (void)osal_exportfunc_register(HI_ID_LSADC, "HI_LSADC", HI_NULL);

    /* LSADC driver register */
    if (osal_dev_register(&g_lsadc_register_data) < 0) {
        HI_ERR_LSADC("register LSADC failed.\n");
        return HI_FAILURE;
    }

    (void)memset_s(lsadc, sizeof(*lsadc), 0, sizeof(struct lsadc_priv));

    lsadc->base = osal_ioremap_nocache(LSADC_REG_BASE, HI_LSADC_SPACE_SIZE);
    if (lsadc->base == NULL) {
        HI_ERR_LSADC("ioremap REG_ADDR[0x%x] failure!\n", LSADC_REG_BASE);
        return HI_FAILURE;
    }

    lsadc->irq = LSADC_IRQ_NUM;
    ret = strncpy_s(lsadc->dev_name, LSADC_DEVICE_NAME_SIZE, LSADC_DEVICE_NAME, sizeof(LSADC_DEVICE_NAME));
    if (ret != EOK) {
        HI_ERR_LSADC("strncpy_s error,ret = 0x%x\n", ret);
        osal_iounmap(lsadc->base);
        osal_dev_unregister(&g_lsadc_register_data);
        return HI_FAILURE;
    }

    osal_sem_init(&lsadc->sem, 1);
    HI_PRINT("load hi_lsadc.ko success.\t\t(%s)\n", VERSION_STRING);
    return HI_SUCCESS;
}

void lsadc_drv_mod_exit(void)
{
    struct lsadc_priv *lsadc = &g_lsadc_local;

    osal_dev_unregister(&g_lsadc_register_data);
    (void)osal_exportfunc_unregister(HI_ID_LSADC);
    osal_iounmap(lsadc->base);
}

#ifdef MODULE
module_init(lsadc_drv_mod_init);
module_exit(lsadc_drv_mod_exit);
#else
EXPORT_SYMBOL(lsadc_drv_mod_init);
EXPORT_SYMBOL(lsadc_drv_mod_exit);
#endif

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("HISILICON");

