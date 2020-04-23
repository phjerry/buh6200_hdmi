/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: volt ctrl file
 */

#include "drv_dvfs_regulator.h"
#include "drv_dvfs_common.h"

#include <linux/device.h>
#include <linux/hisilicon/securec.h>
#include <asm/io.h>
#include "linux/hisilicon/hi_drv_mbx.h"

#include "hi_osal.h"
#include "hi_drv_sys.h"
#include "hi_type.h"
#include "hi_debug.h"
#include "hi_drv_sys.h"

typedef struct {
    unsigned int cmd;
    unsigned int message_len;
    unsigned int volt; /* Hz */
} reg_mailbox_msg;

#define SET_CORE_VOLT_CMD 0x3
#define MBX_TIMEOUT 500000
#define DVFS_SUCCESS 0x5A5AA5A5
#define DVFS_FAILURE 0xA5A55A5A

static hi_u32 *g_pmc_base = HI_NULL;
static hi_u32 g_core_volt;

static hi_s32 set_core_volt_to_hrf(hi_u32 volt)
{
    hi_s32 ret;
    hi_s32 handle;
    reg_mailbox_msg buf = {0};
    hi_u32 msg_len, result;

    handle = hi_drv_mbx_open(HI_MBX_ACPU2HRF_ACPU_DVFS);
    if (handle < 0) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_mbx_open, HI_FAILURE);
        return HI_FAILURE;
    }

    buf.cmd = SET_CORE_VOLT_CMD;
    buf.message_len = sizeof(reg_mailbox_msg);
    buf.volt = (unsigned int)volt;

    ret = hi_drv_mbx_tx(handle, (hi_u8 *)&buf, buf.message_len, &msg_len, MBX_TIMEOUT);
    if (ret < 0 || msg_len != buf.message_len) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_mbx_tx, ret);
        ret = HI_FAILURE;
        goto exit;
    }

    ret = hi_drv_mbx_rx(handle, (hi_u8 *)&result, sizeof(hi_u32), &msg_len, MBX_TIMEOUT);
    if (ret < 0 || msg_len != sizeof(unsigned int) || result != DVFS_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_mbx_rx, ret);
        ret = HI_FAILURE;
        goto exit;
    }

exit:
    ret |= hi_drv_mbx_close(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_mbx_close, ret);
        return ret;
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

hi_void dvfs_set_core_volt(hi_u32 new_volt, hi_u32 pwm_addr)
{
    hi_s32 ret;
    hi_u32 pwm_volt, tmp, pwm;

    if (new_volt == 0) {
        return;
    }

    if (new_volt > CORE_VOLT_MAX) {
        new_volt = CORE_VOLT_MAX;
        HI_WARN_PRINT_INFO("new_volt is out of range! Force it to vmax\n");
    } else if (new_volt < CORE_VOLT_MIN) {
        new_volt = CORE_VOLT_MIN;
        HI_WARN_PRINT_INFO("new_volt is out of range! Force it to vmin\n");
    }

    pwm = readl(g_pmc_base + pwm_addr / sizeof(hi_u32));

    tmp = (pwm >> 16) & 0xFFFF;
    pwm_volt = CORE_VOLT_MAX - ((tmp - 1) * (CORE_VOLT_MAX - CORE_VOLT_MIN)) / (PWM_STEP_NUM * PWM_CLASS);

    if (new_volt != pwm_volt) {
        ret = set_core_volt_to_hrf(new_volt);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(set_core_volt_to_hrf, ret);
            return;
        }
    }

    return;
}

hi_void dvfs_get_core_volt(hi_u32 *volt)
{
    hi_u32 duty, reg_value;

    reg_value = readl(g_pmc_base + PMC_PWM3_CTRL0 / sizeof(hi_u32));

    HI_INFO_PRINT_H32(reg_value);

    duty = (reg_value >> 16) & 0xFFFF;
    *volt = CORE_VOLT_MAX - ((duty - 1) * (CORE_VOLT_MAX - CORE_VOLT_MIN)) / (PWM_STEP_NUM * PWM_CLASS);

    return;
}

hi_void dvfs_save_core_volt(hi_void)
{
    dvfs_get_core_volt(&g_core_volt);
    return;
}

hi_void dvfs_restore_core_volt(hi_void)
{
    hi_s32 ret;

    /* set volt to hrf */
    ret = set_core_volt_to_hrf(g_core_volt);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(set_core_volt_to_hrf, ret);
        return;
    }

    return;
}

hi_s32 pmc_register_remap(hi_void)
{
    g_pmc_base = osal_ioremap_nocache(PMC_BASE_ADDR, 0x1000);
    if (g_pmc_base == HI_NULL) {
        HI_ERR_PRINT_FUNC_RES(osal_ioremap_nocache, HI_FAILURE);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void pmc_register_unmap(hi_void)
{
    osal_iounmap(g_pmc_base);
    return;
}

