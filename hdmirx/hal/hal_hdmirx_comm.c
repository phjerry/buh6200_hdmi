/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Implementation of common functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2019/12/3
 */
#include "hal_hdmirx_comm.h"
#include <linux/kernel.h>
#include "hi_osal.h"
#include "hal_hdmirx_product.h"

static hi_u32 *g_ctrl_reg_base[HDMIRX_CTRL_CNT];
static hi_u32 *g_aon_reg_base[HDMIRX_CTRL_CNT];
static hi_u32 *g_dphy_reg_base[HDMIRX_PHY_CNT];
static hi_u32 *g_damix_reg_base[HDMIRX_PHY_CNT];
static hi_u32 *g_hdcp1x_reg_base[HDMIRX_CTRL_CNT];
static hi_u32 *g_hdcp2x_reg_base[HDMIRX_CTRL_CNT];
static hi_u32 *g_hdcp_ram_reg_base[HDMIRX_CTRL_CNT];
static hi_u32 *g_frl_ram_reg_base[HDMIRX_CTRL_CNT];

static hi_u32 get_right_high(hi_u32 mask)
{
    hi_u32 index;

    for (index = 0; index < 32; index++) { /* 32: check front 32 data */
        if ((mask & 0x01) == 0x01) {
            break;
        } else {
            mask >>= 1;
        }
    }
    return index;
}

static hi_u32 hdmirx_reg_read(hi_u32 *addr, hi_u32 offset)
{
    hi_u32 temp;
    if (addr == HI_NULL) {
        return 0;
    }
    temp = (hi_u32) (*((volatile hi_u32 *)(addr + (offset >> 2)))); /* 2: div 4 */
    return temp;
}

static hi_u32 hdmirx_reg_read_fld_align(hi_u32 *addr, hi_u32 offset, hi_u32 mask)
{
    hi_u32 temp;
    hi_u32 index;
    if ((mask == 0) || (addr == HI_NULL)) {
        return 0;
    }
    temp = hdmirx_reg_read(addr, offset);
    index = get_right_high(mask);
    return ((temp & mask) >> index);
}

static hi_void hdmirx_reg_read_block(hi_u32 *addr, hi_u32 offset, hi_u32 *dst, hi_u32 num)
{
    if ((dst == HI_NULL) || (addr == HI_NULL)) {
        return;
    }
    while (num > 0) {
        *dst = hdmirx_reg_read(addr, offset);
        dst++;
        addr += 1;
        num--;
    }
}

static hi_void hdmirx_reg_write(hi_u32 *addr, hi_u32 offset, hi_u32 value)
{
    if (addr == HI_NULL) {
        return;
    }
    *((volatile hi_u32 *)(addr + (offset >> 2))) = value; /* 2: div 4 */
}

static hi_void hdmirx_reg_write_block(hi_u32 *addr, hi_u32 offset, hi_u32 *src, hi_u32 num)
{
    if ((src == HI_NULL) || (addr == HI_NULL)) {
        return;
    }
    while (num > 0) {
        hdmirx_reg_write(addr, offset, *src);
        src++;
        addr += 1;
        num -= 1;
    }
}

static hi_void hdmirx_reg_write_fld_align(hi_u32 *addr, hi_u32 offset, hi_u32 mask, hi_u32 value)
{
    hi_u32 temp;
    hi_u32 index;

    if ((mask == 0) || (addr == HI_NULL)) {
        return;
    }

    temp = hdmirx_reg_read(addr, offset);
    index = get_right_high(mask);
    value <<= index;
    value &= mask;
    temp &= ~mask;
    temp |= value;
    hdmirx_reg_write(addr, offset, temp);
}

static hi_void hdmirx_reg_set_bits(hi_u32 *addr, hi_u32 offset, hi_u32 mask, hi_bool value)
{
    hi_u32 temp;

    if ((mask == 0) || (addr == HI_NULL)) {
        return;
    }
    temp = hdmirx_reg_read(addr, offset);
    if (value == HI_TRUE) {
        temp |= (hi_u32) mask;
    } else if (value == HI_FALSE) {
        temp &= ~(hi_u32) mask;
    }
    hdmirx_reg_write(addr, offset, temp);
}

static hi_s32 hdmirx_hal_aon_reg_init(hi_void)
{
    g_aon_reg_base[0] = (hi_u32 *) osal_ioremap_nocache(HDMIRX_AON_REG_P0_PAD_ADDR, HDMIRX_AON_REGS_SIZE);
    if (g_aon_reg_base[0] == HI_NULL) {
        return HI_FAILURE;
    }
    g_aon_reg_base[1] = (hi_u32 *) osal_ioremap_nocache(HDMIRX_AON_REG_P1_PAD_ADDR, HDMIRX_AON_REGS_SIZE);
    if (g_aon_reg_base[1] == HI_NULL) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 hdmirx_hal_pwd_reg_init(hi_void)
{
    g_ctrl_reg_base[0] = (hi_u32 *) osal_ioremap_nocache(HDMIRX_PWD_REG_P0_PAD_ADDR, HDMIRX_PWD_REGS_SIZE);
    if (g_ctrl_reg_base[0] == HI_NULL) {
        return HI_FAILURE;
    }
    g_ctrl_reg_base[1] = (hi_u32 *) osal_ioremap_nocache(HDMIRX_PWD_REG_P1_PAD_ADDR, HDMIRX_PWD_REGS_SIZE);
    if (g_ctrl_reg_base[1] == HI_NULL) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 hdmirx_hal_dphy_reg_init(hi_void)
{
    g_dphy_reg_base[0] = (hi_u32 *) osal_ioremap_nocache(HDMIRX_DPHY_REG_P0_PAD_ADDR, HDMIRX_DPHY_REGS_SIZE);
    if (g_dphy_reg_base[0] == HI_NULL) {
        return HI_FAILURE;
    }
    g_dphy_reg_base[1] = (hi_u32 *) osal_ioremap_nocache(HDMIRX_DPHY_REG_P1_PAD_ADDR, HDMIRX_DPHY_REGS_SIZE);
    if (g_dphy_reg_base[1] == HI_NULL) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 hdmirx_hal_damix_reg_init(hi_void)
{
    g_damix_reg_base[0] = (hi_u32 *) osal_ioremap_nocache(HDMIRX_DAMIX_REG_P0_PAD_ADDR, HDMIRX_DAMIX_REGS_SIZE);
    if (g_damix_reg_base[0] == HI_NULL) {
        return HI_FAILURE;
    }
    g_damix_reg_base[1] = (hi_u32 *) osal_ioremap_nocache(HDMIRX_DAMIX_REG_P1_PAD_ADDR, HDMIRX_DAMIX_REGS_SIZE);
    if (g_damix_reg_base[1] == HI_NULL) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 hdmirx_hal_hdcp1x_reg_init(hi_void)
{
    g_hdcp1x_reg_base[0] = (hi_u32 *) osal_ioremap_nocache(HDMIRX_HDCP1X_REG_P0_PAD_ADDR, HDMIRX_HDCP1X_REGS_SIZE);
    if (g_hdcp1x_reg_base[0] == HI_NULL) {
        return HI_FAILURE;
    }
    g_hdcp1x_reg_base[1] = (hi_u32 *) osal_ioremap_nocache(HDMIRX_HDCP1X_REG_P1_PAD_ADDR, HDMIRX_HDCP1X_REGS_SIZE);
    if (g_hdcp1x_reg_base[1] == HI_NULL) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 hdmirx_hal_hdcp2x_reg_init(hi_void)
{
    g_hdcp2x_reg_base[0] = (hi_u32 *) osal_ioremap_nocache(HDMIRX_HDCP2X_REG_P0_PAD_ADDR, HDMIRX_HDCP2X_REGS_SIZE);
    if (g_hdcp2x_reg_base[0] == HI_NULL) {
        return HI_FAILURE;
    }
    g_hdcp2x_reg_base[1] = (hi_u32 *) osal_ioremap_nocache(HDMIRX_HDCP2X_REG_P1_PAD_ADDR, HDMIRX_HDCP2X_REGS_SIZE);
    if (g_hdcp2x_reg_base[1] == HI_NULL) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 hdmirx_hal_hdcp_ram_init(hi_void)
{
    g_hdcp_ram_reg_base[0] = (hi_u32 *) osal_ioremap_nocache(HDMIRX_HDCP_RAM_P0_PAD_ADDR, HDMIRX_HDCP_RAM_SIZE);
    if (g_hdcp_ram_reg_base[0] == HI_NULL) {
        return HI_FAILURE;
    }
    g_hdcp_ram_reg_base[1] = (hi_u32 *) osal_ioremap_nocache(HDMIRX_HDCP_RAM_P1_PAD_ADDR, HDMIRX_HDCP_RAM_SIZE);
    if (g_hdcp_ram_reg_base[1] == HI_NULL) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 hdmirx_hal_frl_ram_init(hi_void)
{
    g_frl_ram_reg_base[0] = (hi_u32 *) osal_ioremap_nocache(HDMIRX_FRL_RAM_P0_PAD_ADDR, HDMIRX_FRL_RAM_SIZE);
    if (g_frl_ram_reg_base[0] == HI_NULL) {
        return HI_FAILURE;
    }
    g_frl_ram_reg_base[1] = (hi_u32 *) osal_ioremap_nocache(HDMIRX_FRL_RAM_P1_PAD_ADDR, HDMIRX_FRL_RAM_SIZE);
    if (g_frl_ram_reg_base[1] == HI_NULL) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void hdmirx_hal_aon_reg_deinit(hi_void)
{
    if (g_aon_reg_base[0] != HI_NULL) {
        osal_iounmap((hi_void *)g_aon_reg_base[0]);
        g_aon_reg_base[0] = HI_NULL;
    }
    if (g_aon_reg_base[1] != HI_NULL) {
        osal_iounmap((hi_void *)g_aon_reg_base[1]);
        g_aon_reg_base[1] = HI_NULL;
    }
}

static hi_void hdmirx_hal_pwd_reg_deinit(hi_void)
{
    if (g_ctrl_reg_base[0] != HI_NULL) {
        osal_iounmap((hi_void *)g_ctrl_reg_base[0]);
        g_ctrl_reg_base[0] = HI_NULL;
    }
    if (g_ctrl_reg_base[1] != HI_NULL) {
        osal_iounmap((hi_void *)g_ctrl_reg_base[1]);
        g_ctrl_reg_base[1] = HI_NULL;
    }
}

static hi_void hdmirx_hal_dphy_reg_deinit(hi_void)
{
    if (g_dphy_reg_base[0] != HI_NULL) {
        osal_iounmap((hi_void *)g_dphy_reg_base[0]);
        g_dphy_reg_base[0] = HI_NULL;
    }
    if (g_dphy_reg_base[1] != HI_NULL) {
        osal_iounmap((hi_void *)g_dphy_reg_base[1]);
        g_dphy_reg_base[1] = HI_NULL;
    }
}

static hi_void hdmirx_hal_damix_reg_deinit(hi_void)
{
    if (g_damix_reg_base[0] != HI_NULL) {
        osal_iounmap((hi_void *)g_damix_reg_base[0]);
        g_damix_reg_base[0] = HI_NULL;
    }
    if (g_damix_reg_base[1] != HI_NULL) {
        osal_iounmap((hi_void *)g_damix_reg_base[1]);
        g_damix_reg_base[1] = HI_NULL;
    }
}

static hi_void hdmirx_hal_hdcp1x_reg_deinit(hi_void)
{
    if (g_hdcp1x_reg_base[0] != HI_NULL) {
        osal_iounmap((hi_void *)g_hdcp1x_reg_base[0]);
        g_hdcp1x_reg_base[0] = HI_NULL;
    }
    if (g_hdcp1x_reg_base[1] != HI_NULL) {
        osal_iounmap((hi_void *)g_hdcp1x_reg_base[1]);
        g_hdcp1x_reg_base[1] = HI_NULL;
    }
}

static hi_void hdmirx_hal_hdcp2x_reg_deinit(hi_void)
{
    if (g_hdcp2x_reg_base[0] != HI_NULL) {
        osal_iounmap((hi_void *)g_hdcp2x_reg_base[0]);
        g_hdcp2x_reg_base[0] = HI_NULL;
    }
    if (g_hdcp2x_reg_base[1] != HI_NULL) {
        osal_iounmap((hi_void *)g_hdcp2x_reg_base[1]);
        g_hdcp2x_reg_base[1] = HI_NULL;
    }
}

static hi_void hdmirx_hal_hdcp_ram_reg_deinit(hi_void)
{
    if (g_hdcp_ram_reg_base[0] != HI_NULL) {
        osal_iounmap((hi_void *)g_hdcp_ram_reg_base[0]);
        g_hdcp_ram_reg_base[0] = HI_NULL;
    }
    if (g_hdcp_ram_reg_base[1] != HI_NULL) {
        osal_iounmap((hi_void *)g_hdcp_ram_reg_base[1]);
        g_hdcp_ram_reg_base[1] = HI_NULL;
    }
}

static hi_void hdmirx_hal_frl_ram_reg_deinit(hi_void)
{
    if (g_frl_ram_reg_base[0] != HI_NULL) {
        osal_iounmap((hi_void *)g_frl_ram_reg_base[0]);
        g_frl_ram_reg_base[0] = HI_NULL;
    }
    if (g_frl_ram_reg_base[1] != HI_NULL) {
        osal_iounmap((hi_void *)g_frl_ram_reg_base[1]);
        g_frl_ram_reg_base[1] = HI_NULL;
    }
}

hi_void hdmirx_reg_write_map(hi_u32 reg_addr, hi_u32 value)
{
    volatile hi_u32 *addr = HI_NULL;

    addr = (volatile hi_u32 *)osal_ioremap_nocache(reg_addr, 4); /* 4: the map length of a register */
    if (addr != HI_NULL) {
        *addr = value;
        osal_iounmap((hi_void *)addr);
    } else {
        osal_printk("HDMI_IO_MAP addr=0x%x err!\n", reg_addr);
    }
}

hi_u32 hdmirx_reg_read_map(hi_u32 reg_addr)
{
    volatile hi_u32 *addr = HI_NULL;
    hi_u32 value = 0;

    addr = (volatile hi_u32 *)osal_ioremap_nocache(reg_addr, 4); /* 4: the map length of a register */
    if (addr != HI_NULL) {
        value = *addr;
        osal_iounmap((hi_void *)addr);
    } else {
        osal_printk("HDMI_IO_MAP addr=0x%x\n err!\n", reg_addr);
    }

    return value;
}

hi_s32 hdmirx_hal_reg_init(hi_void)
{
    hi_s32 result;

    result = hdmirx_hal_aon_reg_init();
    if (result == HI_FAILURE) {
        return result;
    }
    result = hdmirx_hal_pwd_reg_init();
    if (result == HI_FAILURE) {
        return result;
    }
    result = hdmirx_hal_dphy_reg_init();
    if (result == HI_FAILURE) {
        return result;
    }
    result = hdmirx_hal_damix_reg_init();
    if (result == HI_FAILURE) {
        return result;
    }
    result = hdmirx_hal_hdcp1x_reg_init();
    if (result == HI_FAILURE) {
        return result;
    }
    result = hdmirx_hal_hdcp2x_reg_init();
    if (result == HI_FAILURE) {
        return result;
    }
    result = hdmirx_hal_hdcp_ram_init();
    if (result == HI_FAILURE) {
        return result;
    }
    result = hdmirx_hal_frl_ram_init();
    if (result == HI_FAILURE) {
        return result;
    }

    return HI_SUCCESS;
}

hi_void hdmirx_hal_reg_deinit(hi_void)
{
    hdmirx_hal_aon_reg_deinit();
    hdmirx_hal_pwd_reg_deinit();
    hdmirx_hal_dphy_reg_deinit();
    hdmirx_hal_damix_reg_deinit();
    hdmirx_hal_hdcp1x_reg_deinit();
    hdmirx_hal_hdcp2x_reg_deinit();
    hdmirx_hal_hdcp_ram_reg_deinit();
    hdmirx_hal_frl_ram_reg_deinit();

    return;
}

hi_u32 hdmirx_hal_reg_read(hi_drv_hdmirx_port port, hi_u32 addr)
{
    hi_u32 temp;

    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    temp = hdmirx_reg_read(g_ctrl_reg_base[port], addr);

    return temp;
}

hi_void hdmirx_hal_reg_write(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 value)
{
    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    hdmirx_reg_write(g_ctrl_reg_base[port], addr, value);
}

hi_u32 hdmirx_hal_reg_read_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask)
{
    hi_u32 temp;

    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    temp = hdmirx_reg_read_fld_align(g_ctrl_reg_base[port], addr, mask);

    return temp;
}

hi_void hdmirx_hal_reg_write_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask, hi_u32 value)
{
    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    hdmirx_reg_write_fld_align(g_ctrl_reg_base[port], addr, mask, value);
}

hi_void hdmirx_hal_reg_read_block(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 *dst, hi_u32 num)
{
    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    hdmirx_reg_read_block(g_ctrl_reg_base[port], addr, dst, num);
}

hi_void hdmirx_hal_reg_set_bits(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask, hi_bool value)
{
    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    hdmirx_reg_set_bits(g_ctrl_reg_base[port], addr, mask, value);
}

hi_void hdmirx_hal_reg_write_block(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 *src, hi_u32 num)
{
    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    hdmirx_reg_write_block(g_ctrl_reg_base[port], addr, src, num);
}

hi_void hdmirx_hal_aon_reg_write(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 value)
{
    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    hdmirx_reg_write(g_aon_reg_base[port], addr, value);
}

hi_u32 hdmirx_hal_aon_reg_read(hi_drv_hdmirx_port port, hi_u32 addr)
{
    hi_u32 temp;

    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    temp = hdmirx_reg_read(g_aon_reg_base[port], addr);

    return temp;
}

hi_u32 hdmirx_hal_aon_reg_read_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask)
{
    hi_u32 temp;

    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    temp = hdmirx_reg_read_fld_align(g_aon_reg_base[port], addr, mask);

    return temp;
}

hi_void hdmirx_hal_aon_reg_write_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask, hi_u32 value)
{
    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    hdmirx_reg_write_fld_align(g_aon_reg_base[port], addr, mask, value);
}

hi_void hdmirx_hal_aon_reg_write_block(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 *src, hi_u32 num)
{
    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    hdmirx_reg_write_block(g_aon_reg_base[port], addr, src, num);
}

hi_void hdmirx_hal_dphy_reg_write(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 value)
{
    if (port >= HDMIRX_PHY_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    hdmirx_reg_write(g_dphy_reg_base[port], addr, value);
}

hi_u32 hdmirx_hal_dphy_reg_read(hi_drv_hdmirx_port port, hi_u32 addr)
{
    hi_u32 temp;

    if (port >= HDMIRX_PHY_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    temp = hdmirx_reg_read(g_dphy_reg_base[port], addr);

    return temp;
}

hi_u32 hdmirx_hal_dphy_reg_read_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask)
{
    hi_u32 temp;

    if (port >= HDMIRX_PHY_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    temp = hdmirx_reg_read_fld_align(g_dphy_reg_base[port], addr, mask);

    return temp;
}

hi_void hdmirx_hal_dphy_reg_write_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask, hi_u32 value)
{
    if (port >= HDMIRX_PHY_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    hdmirx_reg_write_fld_align(g_dphy_reg_base[port], addr, mask, value);
}

hi_void hdmirx_hal_damix_reg_write(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 value)
{
    if (port >= HDMIRX_PHY_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    hdmirx_reg_write(g_damix_reg_base[port], addr, value);
}

hi_u32 hdmirx_hal_damix_reg_read(hi_drv_hdmirx_port port, hi_u32 addr)
{
    hi_u32 temp;

    if (port >= HDMIRX_PHY_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    temp = hdmirx_reg_read(g_damix_reg_base[port], addr);

    return temp;
}

hi_u32 hdmirx_hal_damix_reg_read_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask)
{
    hi_u32 temp;

    if (port >= HDMIRX_PHY_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    temp = hdmirx_reg_read_fld_align(g_damix_reg_base[port], addr, mask);

    return temp;
}

hi_void hdmirx_hal_damix_reg_write_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask, hi_u32 value)
{
    if (port >= HDMIRX_PHY_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    hdmirx_reg_write_fld_align(g_damix_reg_base[port], addr, mask, value);
}

hi_void hdmirx_hal_damix_reg_set_bits(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask, hi_bool value)
{
    if (port >= HDMIRX_PHY_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    hdmirx_reg_set_bits(g_damix_reg_base[port], addr, mask, value);
}

hi_u32 hdmirx_hal_hdcp1x_reg_read_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask)
{
    hi_u32 temp;

    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    temp = hdmirx_reg_read_fld_align(g_hdcp1x_reg_base[port], addr, mask);

    return temp;
}

hi_void hdmirx_hal_hdcp1x_reg_write_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask, hi_u32 value)
{
    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    hdmirx_reg_write_fld_align(g_hdcp1x_reg_base[port], addr, mask, value);
}

hi_u32 hdmirx_hal_hdcp1x_reg_read(hi_drv_hdmirx_port port, hi_u32 addr)
{
    hi_u32 temp;

    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    temp = hdmirx_reg_read(g_hdcp1x_reg_base[port], addr);

    return temp;
}

hi_void hdmirx_hal_hdcp1x_reg_write(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 value)
{
    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    hdmirx_reg_write(g_hdcp1x_reg_base[port], addr, value);
}

hi_u32 hdmirx_hal_hdcp2x_reg_read_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask)
{
    hi_u32 temp;

    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    temp = hdmirx_reg_read_fld_align(g_hdcp2x_reg_base[port], addr, mask);

    return temp;
}

hi_void hdmirx_hal_hdcp2x_reg_write_fld_align(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 mask, hi_u32 value)
{
    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    hdmirx_reg_write_fld_align(g_hdcp2x_reg_base[port], addr, mask, value);
}

hi_u32 hdmirx_hal_hdcp2x_reg_read(hi_drv_hdmirx_port port, hi_u32 addr)
{
    hi_u32 temp;

    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    temp = hdmirx_reg_read(g_hdcp2x_reg_base[port], addr);

    return temp;
}

hi_void hdmirx_hal_hdcp2x_reg_write(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 value)
{
    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    hdmirx_reg_write(g_hdcp2x_reg_base[port], addr, value);
}

hi_void hdmirx_hal_hdcp_ram_reg_write(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 value)
{
    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    hdmirx_reg_write(g_hdcp_ram_reg_base[port], addr, value);
}

hi_u32 hdmirx_hal_hdcp_ram_reg_read(hi_drv_hdmirx_port port, hi_u32 addr)
{
    hi_u32 temp;

    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    temp = hdmirx_reg_read(g_hdcp_ram_reg_base[port], addr);

    return temp;
}

hi_void hdmirx_hal_frl_ram_reg_write(hi_drv_hdmirx_port port, hi_u32 addr, hi_u32 value)
{
    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    hdmirx_reg_write(g_frl_ram_reg_base[port], addr, value);
}

hi_u32 hdmirx_hal_frl_ram_reg_read(hi_drv_hdmirx_port port, hi_u32 addr)
{
    hi_u32 temp;

    if (port >= HDMIRX_CTRL_CNT) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    temp = hdmirx_reg_read(g_frl_ram_reg_base[port], addr);

    return temp;
}
