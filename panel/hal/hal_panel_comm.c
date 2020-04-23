/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel hal comm module
* Author: sdk
* Create: 2019-04-03
*/

#include "hi_type.h"
#include "hal_panel_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VBOTX_REGS_BASE_ADDR    0x018B2000
#define DPHY_REGS_BASE_ADDR     0x018B4000
#define VDP_REGS_BASE_ADDR      0x00F00000
#define PANEL_TCON_BASE_ADDR    0xff080000
#define PANEL_COMBOTX_BASE_ADDR 0xff1a0000
#define OTHER_BASE_ADDR         0xf8a20000
#define PANEL_GPIO_BASE_ADDR    0xF8B20000
#define REG_MAP_SIZE            0x100000
#define REG_ADDR_MAX_BIT        31
#define VDP_TCON_CFG_START_LINE    0x3C
#define VDP_TCON_CFG_FINISH_LINE    0x258

volatile s_dphy_regs_type *g_panel_dphy_reg = HI_NULL;
volatile s_vbotx_regs_type *g_panel_vbotx_reg = HI_NULL;
volatile s_panel_vdp_regs_type *g_panel_vdp_reg = HI_NULL;
/*******************************************************************************
                                PWM struct and enum
*******************************************************************************/
hi_u32 *g_pu32_other_reg = HI_NULL;
hi_u32 *g_pu32_gpio_reg  = HI_NULL;

hi_u32 panel_reg_read(volatile hi_u32 *a)
{
    if (a == HI_NULL) {
        return 0xFFFF; /* 0xFFFF: invalid data */
    }

    return (*a);
}

hi_void panel_reg_write(volatile hi_u32 *a, hi_u32 b)
{
    if (a == HI_NULL) {
        return;
    }
    *a = b;
    return;
}

hi_s32 panel_reg_init(hi_void)
{
    g_panel_vbotx_reg = (s_vbotx_regs_type *)osal_ioremap_nocache(VBOTX_REGS_BASE_ADDR,
        (sizeof(s_vbotx_regs_type) & 0xfffffff0) + 0x10); /* 0x10 for 128bits alignment */
    if (g_panel_vbotx_reg == HI_NULL) {
        goto VBOTX_MAP_FAIL;
    }

    g_panel_dphy_reg = (s_dphy_regs_type *)osal_ioremap_nocache(DPHY_REGS_BASE_ADDR,
        (sizeof(s_dphy_regs_type) & 0xfffffff0) + 0x10); /* 0x10 for 128bits alignment */
    if (g_panel_dphy_reg == HI_NULL) {
        goto DPHY_MAP_FAIL;
    }

    g_panel_vdp_reg = (s_panel_vdp_regs_type *)osal_ioremap_nocache(VDP_REGS_BASE_ADDR,
        (sizeof(s_panel_vdp_regs_type) & 0xfffffff0) + 0x10); /* 0x10 for 128bits alignment */
    if (g_panel_vdp_reg == HI_NULL) {
        goto VDP_MAP_FAIL;
    }

    return HI_SUCCESS;

VDP_MAP_FAIL:
    osal_iounmap((void *)g_panel_dphy_reg);
DPHY_MAP_FAIL:
    osal_iounmap((void *)g_panel_vbotx_reg);
VBOTX_MAP_FAIL:
    hi_log_err("panel ioremap failed!\n");
    return HI_FAILURE;
}

hi_void panel_reg_deinit(hi_void)
{
    return;
}

hi_void panel_tcon_reg_cfg_check(hi_void)
{
    return;
}

hi_void panel_reg_write_mask(hi_u32 addr, hi_u32 val, hi_u32 mask)
{
    hi_u32 value;
    hi_u32 offset;
    hi_u32 base_addr;
    hi_void *pv_reg_addr = NULL;

    offset   = addr & 0xFFFF;
    base_addr = addr & 0xFFFF0000;
    if (base_addr == VDP_REGS_BASE_ADDR) {
        pv_reg_addr = 0;
    }else if (base_addr == PANEL_COMBOTX_BASE_ADDR) {
        pv_reg_addr = (hi_void *)g_panel_dphy_reg + offset;
    }else if (base_addr == PANEL_TCON_BASE_ADDR) {
        pv_reg_addr = 0;
    }else if (base_addr == PANEL_GPIO_BASE_ADDR) {
        pv_reg_addr = (hi_void *)g_pu32_gpio_reg + offset;
    } else {
        pv_reg_addr = (hi_void *)g_pu32_other_reg + offset;
    }

    value = panel_reg_read((hi_u32 *)pv_reg_addr);
    value = (value & (~(mask))) | ((val) & (mask));

    panel_reg_write((hi_u32 *)pv_reg_addr, value);
}

volatile s_dphy_regs_type* panel_dphy_reg(hi_void)
{
    return g_panel_dphy_reg;
}

volatile s_vbotx_regs_type* panel_vbotx_reg(hi_void)
{
    return g_panel_vbotx_reg;
}

volatile s_panel_vdp_regs_type* panel_vdp_reg(hi_void)
{
    return g_panel_vdp_reg;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

