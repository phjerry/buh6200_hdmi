/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description:
 * Author: guoqingbo
 * Create: 2013/08/26
 */

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_osal.h"

#include "drv_reg.h"
#include "hi_reg_common.h"
#include "hi_drv_mem.h"

#define SYS_OTP_LEN 0x800

volatile hi_reg_sys_ctrl *g_reg_sys_ctrl      = HI_NULL;
volatile hi_reg_peri     *g_reg_peri          = HI_NULL;
volatile hi_reg_io       *g_reg_io            = HI_NULL;
volatile hi_reg_crg      *g_reg_crg           = HI_NULL;
hi_u8                    *g_otp_base_vir_addr = HI_NULL;

hi_s32 drv_reg_init(hi_void)
{
    hi_u8 *otp_addr = HI_NULL;

    g_reg_sys_ctrl = (hi_reg_sys_ctrl *)osal_ioremap_nocache(HI_REG_SYS_BASE_ADDR, sizeof(S_SYSCTRL_REGS_TYPE));
    if (g_reg_sys_ctrl == HI_NULL) {
        HI_PRINT("HI_REG_SYS_BASE_ADDR ioremap error!\n");
        goto err5;
    }

    g_reg_peri = (hi_reg_peri *)osal_ioremap_nocache(HI_REG_PERI_BASE_ADDR, sizeof(hi_reg_peri));
    if (g_reg_peri == HI_NULL) {
        HI_PRINT("HI_REG_PERI_BASE_ADDR ioremap error!\n");
        goto err4;
    }

    g_reg_io = (hi_reg_io *)osal_ioremap_nocache(HI_REG_IO_BASE_ADDR, sizeof(hi_reg_io));
    if (g_reg_io == HI_NULL) {
        HI_PRINT("HI_REG_IO_BASE_ADDR ioremap error!\n");
        goto err3;
    }

    g_reg_crg = (hi_reg_crg *)osal_ioremap_nocache(HI_REG_CRG_BASE_ADDR, sizeof(hi_reg_crg));
    if (g_reg_crg == HI_NULL) {
        HI_PRINT("HI_REG_CRG_BASE_ADDR ioremap error!\n");
        goto err2;
    }

    otp_addr = (hi_u8 *)osal_ioremap_nocache(HI_REG_OTP_BASE_ADDR, 0x1000);
    if (otp_addr == HI_NULL) {
        HI_PRINT("HI_REG_OTP_BASE_ADDR ioremap error!\n");
        goto err1;
    }

    g_otp_base_vir_addr = HI_KZALLOC(HI_ID_SYS, 0x1000, GFP_KERNEL);
    if (g_otp_base_vir_addr == HI_NULL) {
        HI_PRINT("g_otp_base_vir_addr kmalloc error!\n");
        goto err0;
    }

    memcpy(g_otp_base_vir_addr, otp_addr, SYS_OTP_LEN);

    osal_iounmap((hi_void *)otp_addr);
    otp_addr = HI_NULL;

    return HI_SUCCESS;

err0:
    osal_iounmap((hi_void *)otp_addr);
    otp_addr = HI_NULL;

err1:
    osal_iounmap((hi_void *)g_reg_crg);
    g_reg_crg = HI_NULL;

err2:
    osal_iounmap((hi_void *)g_reg_io);
    g_reg_io = HI_NULL;

err3:
    osal_iounmap((hi_void *)g_reg_peri);
    g_reg_peri = HI_NULL;

err4:
    osal_iounmap((hi_void *)g_reg_sys_ctrl);
    g_reg_sys_ctrl = HI_NULL;

err5:
    return HI_FAILURE;
}

hi_void drv_reg_exit(hi_void)
{
    if (g_otp_base_vir_addr != HI_NULL) {
        HI_KFREE(HI_ID_SYS, g_otp_base_vir_addr);
        g_otp_base_vir_addr = HI_NULL;
    }

    if (g_reg_sys_ctrl != HI_NULL) {
        osal_iounmap((hi_void *)g_reg_sys_ctrl);
        g_reg_sys_ctrl = HI_NULL;
    }

    if (g_reg_peri != HI_NULL) {
        osal_iounmap((hi_void *)g_reg_peri);
        g_reg_peri = HI_NULL;
    }

    if (g_reg_io != HI_NULL) {
        osal_iounmap((hi_void *)g_reg_io);
        g_reg_io = HI_NULL;
    }

    if (g_reg_crg != HI_NULL) {
        osal_iounmap((hi_void *)g_reg_crg);
        g_reg_crg = HI_NULL;
    }

    return;
}

volatile hi_reg_sys_ctrl *drv_sys_get_ctrl_reg_ptr(hi_void)
{
    return g_reg_sys_ctrl;
}

volatile hi_reg_peri *drv_sys_get_peri_reg_ptr(hi_void)
{
    return g_reg_peri;
}

volatile hi_reg_io *drv_sys_get_io_reg_ptr(hi_void)
{
    return g_reg_io;
}

volatile hi_reg_crg *drv_sys_get_crg_reg_ptr(hi_void)
{
    return g_reg_crg;
}

hi_u8 *drv_sys_get_otp_base_vir_addr(hi_void)
{
    return g_otp_base_vir_addr;
}

