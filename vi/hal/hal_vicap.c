/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: hal vi
 * Author: sdk
 * Create: 2019-12-14
 */

#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/seq_file.h>
#include <linux/hisilicon/securec.h>
#include "hi_reg_common.h"
#include "hi_drv_sys.h"
#include "hal_vicap.h"
#include "drv_vicap_coef.h"
#include "vicap_reg.h"
#include "hi_osal.h"

static hi_void *g_vicap_all_reg_addr = HI_NULL;

#define vicap_hal_reg_read(reg_offset) ioread32(g_vicap_all_reg_addr + (reg_offset))
#define vicap_hal_reg_write(reg_offset, value, name) iowrite32((value), (g_vicap_all_reg_addr + (reg_offset)))

hi_s32 vicap_hal_init(hi_void)
{
    g_vicap_all_reg_addr = ioremap_nocache(VICAP_HAL_REGS_ADDR, VICAP_HAL_REGS_SIZE);
    if (g_vicap_all_reg_addr == HI_NULL) {
        HI_PRINT("vicap ioremap_nocache failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/* 去初始化寄存器 */
hi_void vicap_hal_de_init(hi_void)
{
    if (g_vicap_all_reg_addr != HI_NULL) {
        iounmap(g_vicap_all_reg_addr);
        g_vicap_all_reg_addr = HI_NULL;
    }
}

/*=================================CRG====================================*/
/* CRG总线时钟门控 */
hi_void vicap_hal_crg_set_bus_clk_en(hi_bool enable)
{
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();
    U_PERI_CRG360 peri_crg360;

    peri_crg360.u32 = reg_crg->PERI_CRG360.u32;
    peri_crg360.bits.vicap_bus_cken = enable;
    reg_crg->PERI_CRG360.u32 = peri_crg360.u32;
}

/* CRG PPC 时钟门控 */
hi_void vicap_hal_crg_set_ppc_clk_en(hi_bool enable)
{
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();
    U_PERI_CRG360 peri_crg360;

    peri_crg360.u32 = reg_crg->PERI_CRG360.u32;
    peri_crg360.bits.vicap_ppc0_cken = enable;
    peri_crg360.bits.vicap_ppc1_cken = enable;
    peri_crg360.bits.vicap1_sc_cken = enable;
    reg_crg->PERI_CRG360.u32 = peri_crg360.u32;
}

/* CRG SMMU 时钟门控 */
hi_void vicap_hal_crg_set_smmu_clk_en(hi_bool enable)
{
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();
    U_PERI_CRG360 peri_crg360;

    peri_crg360.u32 = reg_crg->PERI_CRG360.u32;
    peri_crg360.bits.vicap_smmu0_cken = enable;
    peri_crg360.bits.vicap_smmu1_cken = enable;
    reg_crg->PERI_CRG360.u32 = peri_crg360.u32;
}

/* CRG总线复位 */
hi_void vicap_hal_crg_set_bus_reset(hi_bool enable)
{
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();
    U_PERI_CRG360 peri_crg360;

    peri_crg360.u32 = reg_crg->PERI_CRG360.u32;
    peri_crg360.bits.vicap_srst_req_0 = enable;
    peri_crg360.bits.vicap_srst_req_1 = enable;
    reg_crg->PERI_CRG360.u32 = peri_crg360.u32;
}

/* Smmu Reset */
hi_void vicap_hal_crg_set_smmu_reset(hi_bool enable)
{
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();
    U_PERI_CRG360 peri_crg360;

    peri_crg360.u32 = reg_crg->PERI_CRG360.u32;
    peri_crg360.bits.vicap_smmu0_srst_req = enable;
    peri_crg360.bits.vicap_smmu1_srst_req = enable;
    reg_crg->PERI_CRG360.u32 = peri_crg360.u32;
}

/* CRG VI CHN CLK */
hi_void vicap_hal_crg_set_ppc_clk(vicap_hal_chn_workhz chn_work_hz)
{
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();
    U_PERI_CRG360 peri_crg360;

    peri_crg360.u32 = reg_crg->PERI_CRG360.u32;
    peri_crg360.bits.vicap1_cksel = chn_work_hz;
    reg_crg->PERI_CRG360.u32 = peri_crg360.u32;
}

/* PERI HDMI adapt to VI using in 4k [bit 16] */
hi_void vicap_hal_crg_set_hdmirx2_vi_clk(vicap_hal_chn_hdmirx_workhz chn_hdmirx_work_hz)
{
    u_hdmi2vi_adapter hdmi2vi_adapter;

    hdmi2vi_adapter.u32 = vicap_hal_reg_read(vicap_reg_offset_hdmi2vi_adapter(0));

    if (chn_hdmirx_work_hz == VICAP_HAL_CHN_HDMIRX_WORKHZ_NORMAL) {
        hdmi2vi_adapter.bits.hdmi2vi_adapter &= 0xfffe;
    } else if (chn_hdmirx_work_hz == VICAP_HAL_CHN_HDMIRX_WORKHZ_SPECIAL) {
        hdmi2vi_adapter.bits.hdmi2vi_adapter |= 0x1;
    }

    vicap_hal_reg_write(vicap_reg_offset_hdmi2vi_adapter(0), hdmi2vi_adapter.u32, "U_HDMI2VI_ADAPTER");
}

/*=================================WORK MODE REG CFG=============================*/
/* 设置功耗模式(正常模式和低功耗模式) */
hi_void vicap_hal_global_set_power_mode(hi_u32 vicap_id, hi_bool low_power)
{
    u_wk_mode wk_mode;

    wk_mode.u32 = vicap_hal_reg_read(vicap_reg_offset_wk_mode(vicap_id));
    wk_mode.bits.power_mode = low_power;

    vicap_hal_reg_write(vicap_reg_offset_wk_mode(vicap_id), wk_mode.u32, "U_WK_MODE");
}

hi_void vicap_hal_global_set_mix_mode(hi_u32 vicap_id, hi_bool mixen, vicap_hal_mix_mode mix_mode)
{
    u_wk_mode wk_mode;

    wk_mode.u32 = vicap_hal_reg_read(vicap_reg_offset_wk_mode(vicap_id));
    wk_mode.bits.mix_en = mixen;
    wk_mode.bits.mix_mode = mix_mode;

    vicap_hal_reg_write(vicap_reg_offset_wk_mode(vicap_id), wk_mode.u32, "U_WK_MODE");
}

hi_void vicap_hal_global_set_mix_oemode(hi_u32 vicap_id, hi_bool mixen, vicap_hal_mix_oe_mode mix_oe_mode)
{
    u_wk_mode wk_mode;

    wk_mode.u32 = vicap_hal_reg_read(vicap_reg_offset_wk_mode(vicap_id));
    wk_mode.bits.oe_mode = mix_oe_mode;

    vicap_hal_reg_write(vicap_reg_offset_wk_mode(vicap_id), wk_mode.u32, "U_WK_MODE");
}

hi_void vicap_hal_global_set_axi_cfg(hi_u32 vicap_id, vicap_hal_outstanding out_standing)
{
    u_axi_cfg axi_cfg;

    axi_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_axi_cfg(vicap_id));
    axi_cfg.bits.outstanding = out_standing;
    vicap_hal_reg_write(vicap_reg_offset_axi_cfg(vicap_id), axi_cfg.u32, "U_AXI_CFG");
}

hi_void vicap_hal_global_set_mac_cfg(hi_u32 vicap_id, hi_bool lower)
{
    u_mac_cfg mac_cfg;

    mac_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_mac_cfg(vicap_id));

    if (lower == HI_FALSE) {
        mac_cfg.bits.req_prio = 0xff;
    }

    if (lower == HI_TRUE) {
        mac_cfg.bits.req_prio = 0;
    }

    vicap_hal_reg_write(vicap_reg_offset_mac_cfg(vicap_id), mac_cfg.u32, "U_MAC_CFG");
}

hi_void vicap_hal_global_set_pt_sel(hi_u32 vicap_id)
{
    u_pt_sel pt_sel;

    pt_sel.u32 = vicap_hal_reg_read(vicap_reg_offset_pt_sel(vicap_id));

    if (vicap_id == HI_DRV_VICAP_PORT0) {
        pt_sel.bits.pt0_sel = 0;
        pt_sel.bits.pt1_sel = 0;
    } else if (vicap_id == HI_DRV_VICAP_PORT1) {
        pt_sel.bits.pt0_sel = 0;
        pt_sel.bits.pt1_sel = 0;
        pt_sel.bits.pt2_sel = 0;
        pt_sel.bits.pt3_sel = 0;
        pt_sel.bits.pt4_sel = 0;
        pt_sel.bits.pt5_sel = 0;
        pt_sel.bits.pt6_sel = 0;
        pt_sel.bits.pt7_sel = 0;
    }

    vicap_hal_reg_write(vicap_reg_offset_pt_sel(vicap_id), pt_sel.u32, "U_PT_SEL");
}

hi_void vicap_hal_global_set_mix_adapter_vsync_mode(hi_u32 vicap_id, hi_bool mixen, hi_bool enable)
{
    u_mix_adapter_cfg mix_adapter_mode;

    mix_adapter_mode.u32 = vicap_hal_reg_read(vicap_reg_offset_mix_adapter_cfg(vicap_id));
    mix_adapter_mode.bits.vsync_mode = enable;

    vicap_hal_reg_write(vicap_reg_offset_mix_adapter_cfg(vicap_id), mix_adapter_mode.u32, "U_MIX_ADAPTER_CFG");
}
hi_void vicap_hal_global_set_mix_adapter_hsync_mode(hi_u32 vicap_id, hi_bool mixen, hi_bool enable)
{
    u_mix_adapter_cfg mix_adapter_mode;

    mix_adapter_mode.u32 = vicap_hal_reg_read(vicap_reg_offset_mix_adapter_cfg(vicap_id));
    mix_adapter_mode.bits.hsync_mode = enable;

    vicap_hal_reg_write(vicap_reg_offset_mix_adapter_cfg(vicap_id), mix_adapter_mode.u32, "U_MIX_ADAPTER_CFG");
}

hi_void vicap_hal_global_set_apb_time_out(hi_u32 vicap_id)
{
    u_apb_timeout apb_timeout;

    apb_timeout.u32 = vicap_hal_reg_read(vicap_reg_offset_apb_timeout(vicap_id));

    vicap_hal_reg_write(vicap_reg_offset_apb_timeout(vicap_id), apb_timeout.u32, "U_APB_TIMEOUT");
}

hi_void vicap_hal_global_set_vicap_int(hi_u32 vicap_id)
{
    u_vicap_int vicap_int;

    vicap_int.u32 = vicap_hal_reg_read(vicap_reg_offset_vicap_int(vicap_id));

    vicap_hal_reg_write(vicap_reg_offset_vicap_int(vicap_id), vicap_int.u32, "U_VICAP_INT");
}

hi_void vicap_hal_global_set_pt_int_mask(hi_u32 vicap_id, vicap_hal_pt_index pt_index, hi_bool valid)
{
    u_vicap_int_mask vicap_int_mask;

    vicap_int_mask.u32 = vicap_hal_reg_read(vicap_reg_offset_vicap_int_mask(vicap_id));

    switch (pt_index) {
        case VICAP_HAL_PT_INDEX_PT0:
            vicap_int_mask.bits.int_pt0 = valid;
            break;
        case VICAP_HAL_PT_INDEX_PT1:
            vicap_int_mask.bits.int_pt1 = valid;
            break;
        case VICAP_HAL_PT_INDEX_PT2:
            vicap_int_mask.bits.int_pt2 = valid;
            break;
        case VICAP_HAL_PT_INDEX_PT3:
            vicap_int_mask.bits.int_pt3 = valid;
            break;
        case VICAP_HAL_PT_INDEX_PT4:
            vicap_int_mask.bits.int_pt4 = valid;
            break;
        case VICAP_HAL_PT_INDEX_PT5:
            vicap_int_mask.bits.int_pt5 = valid;
            break;
        case VICAP_HAL_PT_INDEX_PT6:
            vicap_int_mask.bits.int_pt6 = valid;
            break;
        case VICAP_HAL_PT_INDEX_PT7:
            vicap_int_mask.bits.int_pt7 = valid;
            break;
        default:

            break;
    }

    vicap_hal_reg_write(vicap_reg_offset_vicap_int_mask(vicap_id), vicap_int_mask.u32, "U_VICAP_INT_MASK");
}

hi_void vicap_hal_global_set_chn_int_mask(hi_u32 vicap_id, hi_bool valid)
{
    u_vicap_int_mask vicap_int_mask;

    vicap_int_mask.u32 = vicap_hal_reg_read(vicap_reg_offset_vicap_int_mask(vicap_id));

    vicap_int_mask.bits.int_ch0 = valid;

    vicap_hal_reg_write(vicap_reg_offset_vicap_int_mask(vicap_id), vicap_int_mask.u32, "U_VICAP_INT_MASK");
}

/*=============================HAL==============================================*/
hi_void vicap_hal_pt_set_intf_mod(hi_u32 vicap_id, vicap_hal_pt_index pt_index, vicap_hal_pt_intf_mode mod)
{
    u_pt_intf_mod pt_intf_mod;

    pt_intf_mod.u32 = vicap_hal_reg_read(vicap_reg_offset_pt_intf_mod(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE);

    pt_intf_mod.bits.mode = mod;
    vicap_hal_reg_write(vicap_reg_offset_pt_intf_mod(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE, pt_intf_mod.u32,
                        "U_PT_INTF_MOD");
}

hi_u32 vicap_hal_pt_get_en_status(hi_u32 vicap_id, vicap_hal_pt_index pt_index)
{
    u_pt_intf_mod pt_intf_mod;

    pt_intf_mod.u32 = vicap_hal_reg_read(vicap_reg_offset_pt_intf_mod(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE);
    return pt_intf_mod.u32;
}

hi_void vicap_hal_pt_set_en(hi_u32 vicap_id, vicap_hal_pt_index pt_index, hi_bool enalbe)
{
    u_pt_intf_mod pt_intf_mod;

    pt_intf_mod.u32 = vicap_hal_reg_read(vicap_reg_offset_pt_intf_mod(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE);
    pt_intf_mod.bits.enable = enalbe;
    vicap_hal_reg_write(vicap_reg_offset_pt_intf_mod(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE, pt_intf_mod.u32,
                        "U_PT_INTF_MOD");
}

hi_void vicap_hal_pt_set_offset(hi_u32 vicap_id, vicap_hal_pt_index pt_index, hi_u32 *offset, hi_u32 *comp_mask,
                                hi_u32 comp_len)
{
    u_pt_offset0 pt_offset0;
    u_pt_offset1 pt_offset1;
    u_pt_offset2 pt_offset2;

    if (comp_len < 3) { /* has 3 group value */
        return;
    }

    pt_offset0.u32 = vicap_hal_reg_read(vicap_reg_offset_pt_offset0(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE);
    pt_offset0.bits.offset = offset[0];
    pt_offset0.bits.mask = comp_mask[0];

    pt_offset1.u32 = vicap_hal_reg_read(vicap_reg_offset_pt_offset1(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE);
    pt_offset1.bits.offset = offset[1];
    pt_offset1.bits.mask = comp_mask[1];

    pt_offset2.u32 = vicap_hal_reg_read(vicap_reg_offset_pt_offset2(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE);
    pt_offset2.bits.offset = offset[2];  /* index 2 */
    pt_offset2.bits.mask = comp_mask[2]; /* index 2 */

    vicap_hal_reg_write(vicap_reg_offset_pt_offset0(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE, pt_offset0.u32,
                        "U_PT_OFFSET0");
    vicap_hal_reg_write(vicap_reg_offset_pt_offset1(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE, pt_offset1.u32,
                        "U_PT_OFFSET1");
    vicap_hal_reg_write(vicap_reg_offset_pt_offset2(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE, pt_offset2.u32,
                        "U_PT_OFFSET2");
}

/*
 * 时序配置寄存器
 * offset:0x130
 */
hi_void vicap_hal_pt_set_unitf_timing_cfg(hi_u32 vicap_id, vicap_hal_pt_index pt_index, vicap_hal_pt_sync_cfg *sync_cfg)
{
    u_pt_unify_timing_cfg pt_unify_timing_cfg;

    pt_unify_timing_cfg.u32 =
        vicap_hal_reg_read(vicap_reg_offset_pt_unify_timing_cfg(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE);

    pt_unify_timing_cfg.bits.field_inv = sync_cfg->field_inv;
    pt_unify_timing_cfg.bits.field_sel = sync_cfg->field_sel;
    pt_unify_timing_cfg.bits.vsync_mode = sync_cfg->vsync_mode;
    pt_unify_timing_cfg.bits.vsync_inv = sync_cfg->vsync_inv;
    pt_unify_timing_cfg.bits.vsync_sel = sync_cfg->vsync_sel;
    pt_unify_timing_cfg.bits.hsync_mode = sync_cfg->hsync_mode;
    pt_unify_timing_cfg.bits.hsync_and = sync_cfg->hsync_and;
    pt_unify_timing_cfg.bits.hsync_inv = sync_cfg->hsync_inv;
    pt_unify_timing_cfg.bits.hsync_sel = sync_cfg->hsync_sel;
    pt_unify_timing_cfg.bits.de_inv = sync_cfg->de_inv;
    pt_unify_timing_cfg.bits.de_sel = sync_cfg->de_sel;

    vicap_hal_reg_write(vicap_reg_offset_pt_unify_timing_cfg(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE,
                        pt_unify_timing_cfg.u32, "U_PT_UNIFY_TIMING_CFG");
}

/*
 * 时序恢复模块配置寄存器
 * offset:0x134
 */
hi_void vicap_hal_pt_set_gen_timing(hi_u32 vicap_id, vicap_hal_pt_index pt_index,
                                    vicap_hal_pt_gentiming_cfg gen_timing_cfg)
{
    u_pt_gen_timing_cfg pt_gen_timing_cfg;

    pt_gen_timing_cfg.u32 =
        vicap_hal_reg_read(vicap_reg_offset_pt_gen_timing_cfg(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE);

    pt_gen_timing_cfg.bits.vsync_mode = gen_timing_cfg.recover_vsync;
    pt_gen_timing_cfg.bits.hsync_mode = gen_timing_cfg.recover_hsync;
    pt_gen_timing_cfg.bits.gen_mode = gen_timing_cfg.gen_mode;
    pt_gen_timing_cfg.bits.enable = gen_timing_cfg.enable;

    vicap_hal_reg_write(vicap_reg_offset_pt_gen_timing_cfg(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE,
                        pt_gen_timing_cfg.u32, "U_PT_GEN_TIMING_CFG");
}

/*
 * 数据配置寄存器
 * offset:0x140
 */
hi_void vicap_hal_pt_set_unitf_data_cfg(hi_u32 vicap_id, vicap_hal_pt_index pt_index, vicap_hal_pt_comp_mode comp_mode)
{
    u_pt_unify_data_cfg pt_unify_data_cfg;

    pt_unify_data_cfg.u32 =
        vicap_hal_reg_read(vicap_reg_offset_pt_unify_data_cfg(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE);

    pt_unify_data_cfg.bits.comp_num = comp_mode;

    if (comp_mode == VICAP_HAL_PT_COMP_MODE_SINGLE) {
        pt_unify_data_cfg.bits.enable = HI_FALSE;
    } else {
        pt_unify_data_cfg.bits.enable = HI_TRUE;
    }

    vicap_hal_reg_write(vicap_reg_offset_pt_unify_data_cfg(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE,
                        pt_unify_data_cfg.u32, "U_PT_UNIFY_DATA_CFG");
}

/*
 * 数据产生模块配置寄存器
 * offset:0x144
 */
hi_void vicap_hal_pt_set_gen_data_cfg(hi_u32 vicap_id, vicap_hal_pt_index pt_index,
                                      vicap_hal_pt_gendata_cfg gen_data_cfg)
{
    u_pt_gen_data_cfg pt_gen_data_cfg;

    pt_gen_data_cfg.u32 =
        vicap_hal_reg_read(vicap_reg_offset_pt_gen_data_cfg(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE);

    pt_gen_data_cfg.bits.enable = gen_data_cfg.enable;
    pt_gen_data_cfg.bits.gen_mode = gen_data_cfg.gen_mode;
    pt_gen_data_cfg.bits.data0_move = gen_data_cfg.data0_move;
    pt_gen_data_cfg.bits.data1_move = gen_data_cfg.data1_move;
    pt_gen_data_cfg.bits.data2_move = gen_data_cfg.data2_move;
    pt_gen_data_cfg.bits.vsync_reset = gen_data_cfg.vsync_reset;
    pt_gen_data_cfg.bits.hsync_reset = gen_data_cfg.hsync_reset;
    pt_gen_data_cfg.bits.vsync_move = gen_data_cfg.vsync_move;
    pt_gen_data_cfg.bits.hsync_move = gen_data_cfg.hsync_move;
    pt_gen_data_cfg.bits.de_move = gen_data_cfg.de_move;
    vicap_hal_reg_write(vicap_reg_offset_pt_gen_data_cfg(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE,
                        pt_gen_data_cfg.u32, "U_PT_GEN_DATA_CFG");
}

/*
 * 数据产生模块系数寄存器
 * offset:0x148
 */
hi_void vicap_hal_pt_set_gen_data_coef(hi_u32 vicap_id, vicap_hal_pt_index pt_index,
                                       vicap_hal_pt_gendata_coef gen_data_coef)
{
    u_pt_gen_data_coef pt_gen_data_coef;

    pt_gen_data_coef.u32 =
        vicap_hal_reg_read(vicap_reg_offset_pt_gen_data_coef(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE);

    pt_gen_data_coef.bits.step_space = gen_data_coef.step_space;
    pt_gen_data_coef.bits.inc_space = gen_data_coef.inc_space;
    pt_gen_data_coef.bits.step_frame = gen_data_coef.step_frame;
    pt_gen_data_coef.bits.inc_frame = gen_data_coef.inc_frame;
    vicap_hal_reg_write(vicap_reg_offset_pt_gen_data_coef(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE,
                        pt_gen_data_coef.u32, "U_PT_GEN_DATA_COEF");
}

/*
 * 数据产生模块配置寄存器
 * offset:0x14C
 */
hi_void vicap_hal_pt_set_gen_data_init(hi_u32 vicap_id, vicap_hal_pt_index pt_index,
                                       vicap_hal_pt_gendata_init gen_data_init)
{
    u_pt_gen_data_init pt_gen_data_init;

    pt_gen_data_init.u32 =
        vicap_hal_reg_read(vicap_reg_offset_pt_gen_data_init(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE);

    pt_gen_data_init.bits.data0 = gen_data_init.data0;
    pt_gen_data_init.bits.data1 = gen_data_init.data1;
    pt_gen_data_init.bits.data2 = gen_data_init.data2;
    vicap_hal_reg_write(vicap_reg_offset_pt_gen_data_init(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE,
                        pt_gen_data_init.u32, "U_PT_GEN_DATA_INIT");
}

/*
 * YUV444配置寄存器
 * offset:0x150
 */
hi_void vicap_hal_pt_set_yuv444_cfg(hi_u32 vicap_id, vicap_hal_pt_index pt_index, hi_bool uptyuv444)
{
    u_pt_yuv444_cfg pt_yuv444_cfg;

    pt_yuv444_cfg.u32 =
        vicap_hal_reg_read(vicap_reg_offset_pt_yuv444_cfg(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE);

    pt_yuv444_cfg.bits.enable = uptyuv444;

    vicap_hal_reg_write(vicap_reg_offset_pt_yuv444_cfg(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE, pt_yuv444_cfg.u32,
                        "U_PT_YUV444_CFG");
}

/*
 * 端口fstart中断延时寄存器
 * offset:0x160
 */
hi_void vicap_hal_pt_set_fstart_dly(hi_u32 vicap_id, vicap_hal_pt_index pt_index, hi_u32 pt_fstart_dly)
{
    vicap_hal_reg_write(vicap_reg_offset_pt_fstart_dly(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE, pt_fstart_dly,
                        "U_PT_FSTART_DLY");
}

/*
 * 端口输入SKIP配置寄存器
 * offset:0x170
 */
hi_void vicap_hal_pt_set_hor_skip(hi_u32 vicap_id, vicap_hal_pt_index pt_index, hi_u32 skip_cfg)
{
    u_pt_hor_skip pt_hor_skip;

    pt_hor_skip.u32 = vicap_hal_reg_read(vicap_reg_offset_pt_hor_skip(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE);

    pt_hor_skip.bits.skip_cfg = skip_cfg;
    vicap_hal_reg_write(vicap_reg_offset_pt_hor_skip(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE, pt_hor_skip.u32,
                        "U_PT_HOR_SKIP");
}

/*
 * 数据产生寄存器相关寄存器:水平 垂直 垂直底场
 * offset:0x180 ~ 0x1A0
 */
hi_void vicap_hal_pt_set_gen_intf(hi_u32 vicap_id, vicap_hal_pt_index pt_index, vicap_hal_pt_gentiming_info intf_cfg)
{
    u_pt_intf_hfb pt_intf_hfb;
    u_pt_intf_hact pt_intf_hact;
    u_pt_intf_hbb pt_intf_hbb;
    u_pt_intf_vfb pt_intf_vfb;
    u_pt_intf_vact pt_intf_vact;
    u_pt_intf_vbb pt_intf_vbb;
    u_pt_intf_vbfb pt_intf_vbfb;
    u_pt_intf_vbact pt_intf_vbact;
    u_pt_intf_vbbb pt_intf_vbbb;
    hi_u32 offset = pt_index * VICAP_HAL_REGS_PT_SIZE;

    pt_intf_hfb.u32 = vicap_hal_reg_read(vicap_reg_offset_pt_intf_hfb(vicap_id) + offset);
    pt_intf_hfb.u32 = 0;
    pt_intf_hfb.bits.hfb = intf_cfg.hfb;
    vicap_hal_reg_write(vicap_reg_offset_pt_intf_hfb(vicap_id) + offset, pt_intf_hfb.u32, "U_PT_INTF_HFB");

    pt_intf_hact.u32 = vicap_hal_reg_read(vicap_reg_offset_pt_intf_hact(vicap_id) + offset);
    pt_intf_hact.u32 = 0;
    pt_intf_hact.bits.hact = intf_cfg.h_act;
    vicap_hal_reg_write(vicap_reg_offset_pt_intf_hact(vicap_id) + offset, pt_intf_hact.u32, "U_PT_INTF_HACT");

    pt_intf_hbb.u32 = vicap_hal_reg_read(vicap_reg_offset_pt_intf_hbb(vicap_id) + offset);
    pt_intf_hbb.u32 = 0;
    pt_intf_hbb.bits.hbb = intf_cfg.hbb;
    vicap_hal_reg_write(vicap_reg_offset_pt_intf_hbb(vicap_id) + offset, pt_intf_hbb.u32, "U_PT_INTF_HBB");

    pt_intf_vfb.u32 = vicap_hal_reg_read(vicap_reg_offset_pt_intf_vfb(vicap_id) + offset);
    pt_intf_vfb.u32 = 0;
    pt_intf_vfb.bits.vfb = intf_cfg.vfb;
    vicap_hal_reg_write(vicap_reg_offset_pt_intf_vfb(vicap_id) + offset, pt_intf_vfb.u32, "U_PT_INTF_VFB");

    pt_intf_vact.u32 = vicap_hal_reg_read(vicap_reg_offset_pt_intf_vact(vicap_id) + offset);
    pt_intf_vact.u32 = 0;
    pt_intf_vact.bits.vact = intf_cfg.v_act;
    vicap_hal_reg_write(vicap_reg_offset_pt_intf_vact(vicap_id) + offset, pt_intf_vact.u32, "U_PT_INTF_VACT");

    pt_intf_vbb.u32 = vicap_hal_reg_read(vicap_reg_offset_pt_intf_vbb(vicap_id) + offset);
    pt_intf_vbb.u32 = 0;
    pt_intf_vbb.bits.vbb = intf_cfg.vbb;
    vicap_hal_reg_write(vicap_reg_offset_pt_intf_vbb(vicap_id) + offset, pt_intf_vbb.u32, "U_PT_INTF_VBB");

    pt_intf_vbfb.u32 = vicap_hal_reg_read(vicap_reg_offset_pt_intf_vbfb(vicap_id) + offset);
    pt_intf_vbfb.u32 = 0;
    pt_intf_vbfb.bits.vbfb = intf_cfg.vbfb;
    vicap_hal_reg_write(vicap_reg_offset_pt_intf_vbfb(vicap_id) + offset, pt_intf_vbfb.u32, "U_PT_INTF_VBFB");

    pt_intf_vbact.u32 = vicap_hal_reg_read(vicap_reg_offset_pt_intf_vbact(vicap_id) + offset);
    pt_intf_vbact.u32 = 0;
    pt_intf_vbact.bits.vbact = intf_cfg.vbact;
    vicap_hal_reg_write(vicap_reg_offset_pt_intf_vbact(vicap_id) + offset, pt_intf_vbact.u32, "U_PT_INTF_VBACT");

    pt_intf_vbbb.u32 = vicap_hal_reg_read(vicap_reg_offset_pt_intf_vbbb(vicap_id) + offset);
    pt_intf_vbbb.u32 = 0;
    pt_intf_vbbb.bits.vbbb = intf_cfg.vbbb;
    vicap_hal_reg_write(vicap_reg_offset_pt_intf_vbbb(vicap_id) + offset, pt_intf_vbbb.u32, "U_PT_INTF_VBBB");
}

/*
 * Port Status Reg
 * offset:0x1E0
 */
vicap_hal_frame_filed vicap_hal_pt_get_field(hi_u32 vicap_id, vicap_hal_pt_index pt_index)
{
    vicap_hal_frame_filed frame_filed;
    u_pt_status pt_status;

    pt_status.u32 = vicap_hal_reg_read(vicap_reg_offset_pt_status(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE);

    if (!(pt_status.bits.field)) {
        frame_filed = VICAP_HAL_FRAME_FILED_BOTTOM;
    } else {
        frame_filed = VICAP_HAL_FRAME_FILED_TOP;
    }

    return frame_filed;
}

/*
 * BT565 Status Reg
 * offset:0x1E4
 */
hi_void vicap_hal_pt_get_bt565_status(hi_u32 vicap_id, vicap_hal_pt_index pt_index, hi_s32 *val)
{
    u_pt_bt656_status pt_bt565_status;

    pt_bt565_status.u32 =
        vicap_hal_reg_read(vicap_reg_offset_pt_bt656_status(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE);

    *val = pt_bt565_status.bits.seav;
}

/*
 * Port Input Frame Size Reg
 * offset:0x1EC
 */
hi_void vicap_hal_pt_get_frame_size(hi_u32 vicap_id, vicap_hal_pt_index pt_index, vicap_hal_frame_size *st_frame_size)
{
    u_pt_size pt_size;

    pt_size.u32 = vicap_hal_reg_read(vicap_reg_offset_pt_size(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE);

    st_frame_size->width = pt_size.bits.width;
    st_frame_size->height = pt_size.bits.height;
}

/*
 * Port Clear Interrupt
 * offset:0x1F0
 */
hi_void vicap_hal_pt_set_clear_int(hi_u32 vicap_id, vicap_hal_pt_index pt_index, hi_u32 clr_int)
{
    vicap_hal_reg_write(vicap_reg_offset_pt_int(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE, clr_int, "U_PT_INT");
}

/*
 * Port Get Interrupt Status
 * offset:0x1F0
 */
hi_s32 vicap_hal_pt_get_int_status(hi_u32 vicap_id, vicap_hal_pt_index pt_index)
{
    u_pt_int pt_int;

    pt_int.u32 = vicap_hal_reg_read(vicap_reg_offset_pt_int(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE);
    return pt_int.u32;
}

/*
 * Port Mask Reg
 * offset:0x1F8
 */
hi_void vicap_hal_pt_set_int_mask(hi_u32 vicap_id, vicap_hal_pt_index pt_index, hi_u32 pt_int_mask)
{
    vicap_hal_reg_write(vicap_reg_offset_pt_int_mask(vicap_id) + pt_index * VICAP_HAL_REGS_PT_SIZE, pt_int_mask,
                        "U_PT_INT_MASK");
}

/*
 * Channel Control Reg(Get Channel Status:[Enalbe/Disable])
 * offset:0x1000
 */
hi_u32 vicap_hal_chn_get_en_status(hi_u32 vicap_id)
{
    u_ch_ctrl ch_ctrl;

    ch_ctrl.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_ctrl(vicap_id));
    return ch_ctrl.u32;
}

/*
 * Channel Set Enalbe(Set Channel Status:[Enable/Disable])
 * offset:0x1000
 */
hi_void vicap_hal_chn_set_en(hi_u32 vicap_id, hi_bool chn_en)
{
    u_ch_ctrl ch_ctrl;

    ch_ctrl.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_ctrl(vicap_id));

    ch_ctrl.bits.enable = chn_en;

    vicap_hal_reg_write(vicap_reg_offset_ch_ctrl(vicap_id), ch_ctrl.u32, "U_CH_CTRL");
}

/*
 * Channel Acquire Frame Reg
 * offset:0x1004
 */
hi_void vicap_hal_chn_set_reg_newer(hi_u32 vicap_id)
{
    u_ch_reg_newer ch_reg_newer;

    ch_reg_newer.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_reg_newer(vicap_id));

    ch_reg_newer.bits.reg_newer = 1;

    vicap_hal_reg_write(vicap_reg_offset_ch_reg_newer(vicap_id), ch_reg_newer.u32, "U_CH_REG_NEWER");
}

/*
 * Channel Sync Adapter Reg
 * offset:0x1010
 */
hi_void vicap_hal_chn_set_adapter_cfg(hi_u32 vicap_id)
{
    u_ch_adapter_cfg ch_adapter_cfg;

    ch_adapter_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_adapter_cfg(vicap_id));

    vicap_hal_reg_write(vicap_reg_offset_ch_adapter_cfg(vicap_id), ch_adapter_cfg.u32, "U_CH_ADAPTER_CFG");
}

hi_void vicap_hal_chn_get_count(hi_u32 vicap_id, vicap_hal_chn_count *st_chn_count)
{
    u_ch_count ch_count;

    ch_count.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_count(vicap_id));

    st_chn_count->hcount = ch_count.bits.h_count;
    st_chn_count->vcount = ch_count.bits.v_count;
}

hi_void vicap_hal_chn_set_vdly_cfg(hi_u32 vicap_id, hi_u32 chn_v_dly)
{
    u_ch_dly0_cfg ch_dly_cfg;

    ch_dly_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_dly0_cfg(vicap_id));

    ch_dly_cfg.bits.v_dly_cfg = chn_v_dly;

    vicap_hal_reg_write(vicap_reg_offset_ch_dly0_cfg(vicap_id), ch_dly_cfg.u32, "U_CH_DLY_CFG");
}

hi_void vicap_hal_chn_set_fstart_dly_cfg(hi_u32 vicap_id, hi_u32 chn_dly_cycle)
{
    hi_u32 ch_fstart_dly_cfg;

    ch_fstart_dly_cfg = chn_dly_cycle;

    vicap_hal_reg_write(vicap_reg_offset_ch_fstart_dly_cfg(vicap_id), ch_fstart_dly_cfg, "U_CH_DLY_CFG");
}

hi_void vicap_hal_chn_set_remix420_cfg(hi_u32 vicap_id, vicap_hal_chn_420_cfg *pst420_cfg)
{
    /* v620 change the reg U_MIX_420_CFG */
    u_mix_420_cfg ch_remix420_cfg;

    ch_remix420_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_mix_420_cfg(vicap_id));

    ch_remix420_cfg.bits.enable = pst420_cfg->enable;
    ch_remix420_cfg.bits.cb_first = pst420_cfg->c_seq;

    vicap_hal_reg_write(vicap_reg_offset_mix_420_cfg(vicap_id), ch_remix420_cfg.u32, "U_MIX_420_CFG");
}

hi_void vicap_hal_chn_set_remix420_size(hi_u32 vicap_id, vicap_hal_frame_size *st_frame_size)
{
    /* v620 change the reg U_MIX_420_SIZE Width-1 Height-1 ? */
    u_mix_420_size ch_remix420_size;

    ch_remix420_size.u32 = vicap_hal_reg_read(vicap_reg_offset_mix_420_size(vicap_id));

    ch_remix420_size.bits.width = st_frame_size->width;
    ch_remix420_size.bits.height = st_frame_size->height;

    vicap_hal_reg_write(vicap_reg_offset_mix_420_size(vicap_id), ch_remix420_size.u32, "U_MIX_420_SIZE");
}

/*============================================Dolby=========================================*/
hi_void vicap_hal_chn_set_dolby_chk_en(hi_u32 vicap_id, hi_bool enable)
{
    u_ch_dolby_cfg ch_dolby_cfg;

    ch_dolby_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_dolby_cfg(vicap_id));

    ch_dolby_cfg.bits.dolby_chk_en = enable;

    vicap_hal_reg_write(vicap_reg_offset_ch_dolby_cfg(vicap_id), ch_dolby_cfg.u32, "U_CH_DOLBY_CFG");
}

hi_void vicap_hal_chn_set_dolby_write_metadata_en(hi_u32 vicap_id, hi_bool enable)
{
    u_ch_dolby_cfg ch_dolby_cfg;

    ch_dolby_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_dolby_cfg(vicap_id));

    ch_dolby_cfg.bits.dolby_wr_en = enable;

    vicap_hal_reg_write(vicap_reg_offset_ch_dolby_cfg(vicap_id), ch_dolby_cfg.u32, "U_CH_DOLBY_CFG");
}

hi_void vicap_hal_chn_set_dolby_tunnel_en(hi_u32 vicap_id, hi_bool enable)
{
    u_ch_dolby_cfg ch_dolby_cfg;

    ch_dolby_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_dolby_cfg(vicap_id));

    ch_dolby_cfg.bits.dolby_tunnel_en = enable;

    vicap_hal_reg_write(vicap_reg_offset_ch_dolby_cfg(vicap_id), ch_dolby_cfg.u32, "U_CH_DOLBY_CFG");
}

hi_void vicap_hal_chn_set_dolby_data_mode(hi_u32 vicap_id, vicap_hal_chn_dolby_data_mode hdmi_input_data_mode)
{
    u_ch_dolby_cfg ch_dolby_cfg;

    ch_dolby_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_dolby_cfg(vicap_id));

    ch_dolby_cfg.bits.data_mode = hdmi_input_data_mode;

    vicap_hal_reg_write(vicap_reg_offset_ch_dolby_cfg(vicap_id), ch_dolby_cfg.u32, "U_CH_DOLBY_CFG");
}
hi_u32 vicap_hal_chn_get_dolby_cur_frm_pack_num(hi_u32 vicap_id)
{
    u_ch_dolby_pack_num ch_dolby_pack_num;

    ch_dolby_pack_num.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_dolby_pack_num(vicap_id));
    return ch_dolby_pack_num.u32;
}

hi_u32 vicap_hal_chn_get_dolby_last_frm_pack_num(hi_u32 vicap_id)
{
    u_ch_dolby_pack_num ch_dolby_pack_num;

    ch_dolby_pack_num.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_dolby_pack_num_t(vicap_id));
    return ch_dolby_pack_num.u32;
}

hi_void vicap_hal_chn_set_dolby_wch_faddr(hi_u32 vicap_id, hi_u32 dobly_addr)
{
    hi_u32 ch_wch_dolby_faddr;

    ch_wch_dolby_faddr = dobly_addr;

    vicap_hal_reg_write(vicap_reg_offset_ch_dolby_faddr_l(vicap_id), ch_wch_dolby_faddr, "U_CH_DOLBY_FADDR");
}
/*============================================Dobly End====================================*/
hi_void vicap_hal_chn_set_wch_ycfg(hi_u32 vicap_id, vicap_hal_chn_wch_cfg *wch_cfg)
{
    u_ch_wch_y_cfg ch_wch_y_cfg;
    u_ch_wch_mmu ch_wch_mmu;

    ch_wch_y_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_wch_y_cfg(vicap_id));

    ch_wch_y_cfg.bits.enable = wch_cfg->enable;
    ch_wch_y_cfg.bits.fpki_mode = wch_cfg->fpk_interlace_mode;
    ch_wch_y_cfg.bits.bit_width = wch_cfg->dst_bit_width;
    ch_wch_y_cfg.bits.bfield = wch_cfg->field;
    ch_wch_y_cfg.bits.interleave = wch_cfg->interleave;

    ch_wch_mmu.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_wch_mmu(vicap_id));
    ch_wch_mmu.bits.wch_y_mmu_bypass = wch_cfg->mmu_bypass;

    vicap_hal_reg_write(vicap_reg_offset_ch_wch_y_cfg(vicap_id), ch_wch_y_cfg.u32, "U_CH_WCH_Y_CFG");
    vicap_hal_reg_write(vicap_reg_offset_ch_wch_mmu(vicap_id), ch_wch_mmu.u32, "U_CH_WCH_MMU");
}

hi_void vicap_hal_chn_get_wch_ycfg(hi_u32 vicap_id, vicap_hal_chn_wch_cfg *wch_cfg)
{
    u_ch_wch_y_cfg ch_wch_y_cfg;
    u_ch_wch_mmu ch_wch_mmu;

    ch_wch_y_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_wch_y_cfg(vicap_id));
    ch_wch_mmu.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_wch_mmu(vicap_id));

    wch_cfg->enable = ch_wch_y_cfg.bits.enable;
    wch_cfg->fpk_interlace_mode = ch_wch_y_cfg.bits.fpki_mode;
    wch_cfg->mmu_bypass = ch_wch_mmu.bits.wch_y_mmu_bypass;
    wch_cfg->dst_bit_width = ch_wch_y_cfg.bits.bit_width;
    wch_cfg->field = ch_wch_y_cfg.bits.bfield;
    wch_cfg->interleave = ch_wch_y_cfg.bits.interleave;
}

hi_void vicap_hal_chn_set_wch_ccfg(hi_u32 vicap_id, vicap_hal_chn_wch_cfg *wch_cfg)
{
    u_ch_wch_c_cfg ch_wch_c_cfg;
    u_ch_wch_mmu ch_wch_mmu;

    ch_wch_c_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_wch_c_cfg(vicap_id));

    ch_wch_c_cfg.bits.enable = wch_cfg->enable;
    ch_wch_c_cfg.bits.fpki_mode = wch_cfg->fpk_interlace_mode;
    ch_wch_c_cfg.bits.bit_width = wch_cfg->dst_bit_width;
    ch_wch_c_cfg.bits.bfield = wch_cfg->field;
    ch_wch_c_cfg.bits.interleave = wch_cfg->interleave;

    ch_wch_mmu.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_wch_mmu(vicap_id));
    ch_wch_mmu.bits.wch_c_mmu_bypass = wch_cfg->mmu_bypass;

    vicap_hal_reg_write(vicap_reg_offset_ch_wch_c_cfg(vicap_id), ch_wch_c_cfg.u32, "U_CH_WCH_C_CFG");
    vicap_hal_reg_write(vicap_reg_offset_ch_wch_mmu(vicap_id), ch_wch_mmu.u32, "U_CH_WCH_MMU");
}

hi_void vicap_hal_chn_get_wch_ccfg(hi_u32 vicap_id, vicap_hal_chn_wch_cfg *wch_cfg)
{
    u_ch_wch_y_cfg ch_wch_y_cfg;
    u_ch_wch_mmu ch_wch_mmu;

    ch_wch_y_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_wch_c_cfg(vicap_id));
    ch_wch_mmu.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_wch_mmu(vicap_id));

    wch_cfg->enable = ch_wch_y_cfg.bits.enable;
    wch_cfg->fpk_interlace_mode = ch_wch_y_cfg.bits.fpki_mode;
    wch_cfg->mmu_bypass = ch_wch_mmu.bits.wch_c_mmu_bypass;
    wch_cfg->dst_bit_width = ch_wch_y_cfg.bits.bit_width;
    wch_cfg->field = ch_wch_y_cfg.bits.bfield;
    wch_cfg->interleave = ch_wch_y_cfg.bits.interleave;
}

hi_void vicap_hal_chn_set_wch_ysize(hi_u32 vicap_id, vicap_hal_chn_dstsize *y_dest_size)
{
    u_ch_wch_y_size ch_wch_y_size;

    ch_wch_y_size.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_wch_y_size(vicap_id));

    ch_wch_y_size.bits.width = y_dest_size->width - 1;
    ch_wch_y_size.bits.height = y_dest_size->height - 1;

    vicap_hal_reg_write(vicap_reg_offset_ch_wch_y_size(vicap_id), ch_wch_y_size.u32, "U_CH_WCH_Y_SIZE");
}

hi_void vicap_hal_chn_set_wch_csize(hi_u32 vicap_id, vicap_hal_chn_dstsize *c_dest_size)
{
    u_ch_wch_c_size ch_wch_c_size;

    ch_wch_c_size.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_wch_c_size(vicap_id));

    ch_wch_c_size.bits.width = c_dest_size->width - 1;
    ch_wch_c_size.bits.height = c_dest_size->height - 1;

    vicap_hal_reg_write(vicap_reg_offset_ch_wch_c_size(vicap_id), ch_wch_c_size.u32, "U_CH_WCH_C_SIZE");
}

hi_void vicap_hal_chn_set_wch_faddr(hi_u32 vicap_id, vicap_hal_chn_wch_faddr *vicap_addr)
{
    hi_u32 ch_wch_y_faddr;
    hi_u32 ch_wch_c_faddr;

    ch_wch_y_faddr = vicap_addr->y_addr;
    ch_wch_c_faddr = vicap_addr->c_addr;

    vicap_hal_reg_write(vicap_reg_offset_ch_wch_y_faddr_l(vicap_id), ch_wch_y_faddr, "U_CH_WCH_Y_FADDR");
    vicap_hal_reg_write(vicap_reg_offset_ch_wch_c_faddr_l(vicap_id), ch_wch_c_faddr, "U_CH_WCH_C_FADDR");
}
hi_void vicap_hal_chn_set_wch_ystride(hi_u32 vicap_id, hi_u32 stride)
{
    u_ch_wch_y_stride ch_wch_y_stride;

    ch_wch_y_stride.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_wch_y_stride(vicap_id));
    ch_wch_y_stride.u32 = 0;
    ch_wch_y_stride.bits.stride = stride;

    vicap_hal_reg_write(vicap_reg_offset_ch_wch_y_stride(vicap_id), ch_wch_y_stride.u32, "U_CH_WCH_Y_STRIDE");
}

hi_void vicap_hal_chn_set_wch_cstride(hi_u32 vicap_id, hi_u32 stride)
{
    u_ch_wch_c_stride ch_wch_c_stride;

    ch_wch_c_stride.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_wch_c_stride(vicap_id));
    ch_wch_c_stride.u32 = 0;
    ch_wch_c_stride.bits.stride = stride;

    vicap_hal_reg_write(vicap_reg_offset_ch_wch_c_stride(vicap_id), ch_wch_c_stride.u32, "U_CH_WCH_C_STRIDE");
}

hi_void vicap_hal_chn_set_clear_int(hi_u32 vicap_id, hi_u32 clr_int)
{
    vicap_hal_reg_write(vicap_reg_offset_ch_int(vicap_id), clr_int, "U_CH_INT");
}

hi_s32 vicap_hal_chn_get_int_status(hi_u32 vicap_id)
{
    u_ch_int ch_int;

    ch_int.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_int(vicap_id));

    return ch_int.u32;
}

hi_s32 vicap_hal_chn_get_int_mask(hi_u32 vicap_id)
{
    u_ch_int_mask ch_int_mask;

    ch_int_mask.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_int_mask(vicap_id));

    return ch_int_mask.u32;
}

hi_s32 vicap_hal_chn_get_line_num(hi_u32 vicap_id)
{
    u_ch_line_num ch_line_num;

    ch_line_num.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_line_num(vicap_id));

    return ch_line_num.u32;
}

hi_void vicap_hal_chn_set_int_mask(hi_u32 vicap_id, hi_u32 int_mask)
{
    vicap_hal_reg_write(vicap_reg_offset_ch_int_mask(vicap_id), int_mask, "U_CH_INT_MASK");
}

/*
 * 设置通道裁剪区域
 * 图像一行的宽度(以像素为单位)，配置值为实际值减1
 * 获取图像的高度(以行为单位)，配置值为实际值减1
 */
hi_void vicap_hal_chn_set_crop(hi_u32 vicap_id, vicap_hal_chn_crop_cfg *crop_cfg)
{
    u_ch_crop_cfg ch_crop_cfg;
    u_ch_crop0_start ch_crop0_start;
    u_ch_crop0_size ch_crop0_size;
    u_ch_crop1_start ch_crop1_start;
    u_ch_crop1_size ch_crop1_size;

    if (crop_cfg->crop_en[0]) {
        ch_crop0_start.u32 = 0;
        ch_crop0_start.bits.x_start = crop_cfg->crop_rect[0].rect_x;
        ch_crop0_start.bits.y_start = crop_cfg->crop_rect[0].rect_y;

        ch_crop0_size.u32 = 0;
        ch_crop0_size.bits.width = crop_cfg->crop_rect[0].rect_w - 1;
        ch_crop0_size.bits.height = crop_cfg->crop_rect[0].rect_h - 1;

        vicap_hal_reg_write(vicap_reg_offset_ch_crop0_start(vicap_id), ch_crop0_start.u32, "U_CH_CROP0_START");
        vicap_hal_reg_write(vicap_reg_offset_ch_crop0_size(vicap_id), ch_crop0_size.u32, "U_CH_CROP0_SIZE");
    }

    if (crop_cfg->crop_en[1]) {
        ch_crop1_start.u32 = 0;
        ch_crop1_start.bits.x_start = crop_cfg->crop_rect[1].rect_x;
        ch_crop1_start.bits.y_start = crop_cfg->crop_rect[1].rect_y;

        ch_crop1_size.u32 = 0;
        ch_crop1_size.bits.width = crop_cfg->crop_rect[1].rect_w - 1;
        ch_crop1_size.bits.height = crop_cfg->crop_rect[1].rect_h - 1;

        vicap_hal_reg_write(vicap_reg_offset_ch_crop1_start(vicap_id), ch_crop1_start.u32, "U_CH_CROP1_START");
        vicap_hal_reg_write(vicap_reg_offset_ch_crop1_size(vicap_id), ch_crop1_size.u32, "U_CH_CROP1_SIZE");
    }

    ch_crop_cfg.u32 = 0;
    ch_crop_cfg.bits.n0_en = crop_cfg->crop_en[0];
    ch_crop_cfg.bits.n1_en = crop_cfg->crop_en[1];

    vicap_hal_reg_write(vicap_reg_offset_ch_crop_cfg(vicap_id), ch_crop_cfg.u32, "U_CH_CROP_CFG");
}

hi_void vicap_hal_chn_set_crop_win(hi_u32 vicap_id, hi_u32 win_width, hi_u32 win_height)
{
    u_ch_crop_win ch_crop_win;

    ch_crop_win.u32 = 0;
    ch_crop_win.bits.width = win_width - 1;
    ch_crop_win.bits.height = win_height - 1;

    vicap_hal_reg_write(vicap_reg_offset_ch_crop_win(vicap_id), ch_crop_win.u32, "U_CH_CROP_WIN");
}

hi_void vicap_hal_chn_get_crop_status(hi_u32 vicap_id, vicap_hal_chn_crop_cfg *crop_cfg)
{
    u_ch_crop_cfg ch_crop_cfg;
    u_ch_crop0_start ch_crop0_start;
    u_ch_crop0_size ch_crop0_size;
    u_ch_crop1_start ch_crop1_start;
    u_ch_crop1_size ch_crop1_size;

    ch_crop_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_crop_cfg(vicap_id));

    ch_crop0_start.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_crop0_start(vicap_id));
    ch_crop0_size.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_crop0_size(vicap_id));

    ch_crop1_start.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_crop1_start(vicap_id));
    ch_crop1_size.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_crop1_size(vicap_id));

    crop_cfg->crop_en[0] = ch_crop_cfg.bits.n0_en;
    crop_cfg->crop_en[1] = ch_crop_cfg.bits.n1_en;

    crop_cfg->crop_rect[0].rect_x = ch_crop0_start.bits.x_start;
    crop_cfg->crop_rect[0].rect_y = ch_crop0_start.bits.y_start;
    crop_cfg->crop_rect[0].rect_w = ch_crop0_size.bits.width + 1;
    crop_cfg->crop_rect[0].rect_h = ch_crop0_size.bits.height + 1;

    crop_cfg->crop_rect[1].rect_x = ch_crop1_start.bits.x_start;
    crop_cfg->crop_rect[1].rect_y = ch_crop1_start.bits.y_start;
    crop_cfg->crop_rect[1].rect_w = ch_crop1_size.bits.width + 1;
    crop_cfg->crop_rect[1].rect_h = ch_crop1_size.bits.height + 1;
}

/* CSC使能 */
hi_void vicap_hal_chn_set_csc_cfg(hi_u32 vicap_id, hi_bool enable_csc)
{
    u_ch_csc_cfg ch_csc_cfg;

    ch_csc_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_csc_cfg(vicap_id));

    ch_csc_cfg.bits.enable = enable_csc;

    vicap_hal_reg_write(vicap_reg_offset_ch_csc_cfg(vicap_id), ch_csc_cfg.u32, "U_CH_CSC_CFG");
}

hi_void vicap_hal_chn_get_csc_cfg(hi_u32 vicap_id, hi_bool *pen_enable_csc)
{
    u_ch_csc_cfg ch_csc_cfg;

    ch_csc_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_csc_cfg(vicap_id));
    *pen_enable_csc = ch_csc_cfg.bits.enable;
}

/* 配置CSC参数 */
hi_void vicap_hal_chn_set_csc_param(hi_u32 vicap_id, vicap_hal_chn_csc_coef *vicap_csc_coef,
                                    vicap_hal_chn_csc_dccoef *vicap_cscdc_coef)
{
    u_ch_csc_coef0 ch_csc_coef0;
    u_ch_csc_coef1 ch_csc_coef1;
    u_ch_csc_coef2 ch_csc_coef2;
    u_ch_csc_coef3 ch_csc_coef3;
    u_ch_csc_coef4 ch_csc_coef4;
    u_ch_csc_in_dc0 ch_csc_in_dc0;
    u_ch_csc_in_dc1 ch_csc_in_dc1;
    u_ch_csc_out_dc0 ch_csc_out_dc0;
    u_ch_csc_out_dc1 ch_csc_out_dc1;

    ch_csc_coef0.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_csc_coef0(vicap_id));
    ch_csc_coef1.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_csc_coef1(vicap_id));
    ch_csc_coef2.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_csc_coef2(vicap_id));
    ch_csc_coef3.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_csc_coef3(vicap_id));
    ch_csc_coef4.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_csc_coef4(vicap_id));
    ch_csc_in_dc0.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_csc_in_dc0(vicap_id));
    ch_csc_in_dc1.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_csc_in_dc1(vicap_id));
    ch_csc_out_dc0.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_csc_out_dc0(vicap_id));
    ch_csc_out_dc1.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_csc_out_dc1(vicap_id));

    ch_csc_coef0.bits.coef00 = vicap_csc_coef->csc_coef00;
    ch_csc_coef0.bits.coef01 = vicap_csc_coef->csc_coef01;
    ch_csc_coef1.bits.coef02 = vicap_csc_coef->csc_coef02;
    ch_csc_coef1.bits.coef10 = vicap_csc_coef->csc_coef10;
    ch_csc_coef2.bits.coef11 = vicap_csc_coef->csc_coef11;
    ch_csc_coef2.bits.coef12 = vicap_csc_coef->csc_coef12;
    ch_csc_coef3.bits.coef20 = vicap_csc_coef->csc_coef20;
    ch_csc_coef3.bits.coef21 = vicap_csc_coef->csc_coef21;
    ch_csc_coef4.bits.coef22 = vicap_csc_coef->csc_coef22;
    ch_csc_in_dc0.bits.in_dc0 = vicap_cscdc_coef->csc_in_dc0;
    ch_csc_in_dc0.bits.in_dc1 = vicap_cscdc_coef->csc_in_dc1;
    ch_csc_in_dc1.bits.in_dc2 = vicap_cscdc_coef->csc_in_dc2;
    ch_csc_out_dc0.bits.out_dc0 = vicap_cscdc_coef->csc_out_dc0;
    ch_csc_out_dc0.bits.out_dc1 = vicap_cscdc_coef->csc_out_dc1;
    ch_csc_out_dc1.bits.out_dc2 = vicap_cscdc_coef->csc_out_dc2;

    vicap_hal_reg_write(vicap_reg_offset_ch_csc_coef0(vicap_id), ch_csc_coef0.u32, "U_CH_CSC_COEF0");
    vicap_hal_reg_write(vicap_reg_offset_ch_csc_coef1(vicap_id), ch_csc_coef1.u32, "U_CH_CSC_COEF1");
    vicap_hal_reg_write(vicap_reg_offset_ch_csc_coef2(vicap_id), ch_csc_coef2.u32, "U_CH_CSC_COEF2");
    vicap_hal_reg_write(vicap_reg_offset_ch_csc_coef3(vicap_id), ch_csc_coef3.u32, "U_CH_CSC_COEF3");
    vicap_hal_reg_write(vicap_reg_offset_ch_csc_coef4(vicap_id), ch_csc_coef4.u32, "U_CH_CSC_COEF4");
    vicap_hal_reg_write(vicap_reg_offset_ch_csc_in_dc0(vicap_id), ch_csc_in_dc0.u32, "U_CH_CSC_IN_DC0");
    vicap_hal_reg_write(vicap_reg_offset_ch_csc_in_dc1(vicap_id), ch_csc_in_dc1.u32, "U_CH_CSC_IN_DC1");
    vicap_hal_reg_write(vicap_reg_offset_ch_csc_out_dc0(vicap_id), ch_csc_out_dc0.u32, "U_CH_CSC_OUT_DC0");
    vicap_hal_reg_write(vicap_reg_offset_ch_csc_out_dc1(vicap_id), ch_csc_out_dc1.u32, "U_CH_CSC_OUT_DC1");
}

hi_void vicap_hal_chn_set_skip_ycfg(hi_u32 vicap_id, hi_u32 skip_cfg)
{
    vicap_hal_reg_write(vicap_reg_offset_ch_skip_y_cfg(vicap_id), skip_cfg, "U_CH_SKIP_Y_CFG");
}

hi_void vicap_hal_chn_set_skip_ccfg(hi_u32 vicap_id, hi_u32 skip_cfg)
{
    vicap_hal_reg_write(vicap_reg_offset_ch_skip_c_cfg(vicap_id), skip_cfg, "U_CH_SKIP_C_CFG");
}

hi_void vicap_hal_chn_set_skip_ywin(hi_u32 vicap_id, vicap_hal_chn_skip_win *st_skip_win)
{
    u_ch_skip_y_win skip_y_win;

    skip_y_win.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_skip_y_win(vicap_id));

    skip_y_win.bits.width = st_skip_win->width;
    skip_y_win.bits.height = st_skip_win->height;

    vicap_hal_reg_write(vicap_reg_offset_ch_skip_y_win(vicap_id), skip_y_win.u32, "U_CH_SKIP_Y_WIN");
}

hi_void vicap_hal_chn_set_skip_cwin(hi_u32 vicap_id, vicap_hal_chn_skip_win *st_skip_win)
{
    u_ch_skip_c_win skip_c_win;

    skip_c_win.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_skip_c_win(vicap_id));

    skip_c_win.bits.width = st_skip_win->width;
    skip_c_win.bits.height = st_skip_win->height;

    vicap_hal_reg_write(vicap_reg_offset_ch_skip_c_win(vicap_id), skip_c_win.u32, "U_CH_SKIP_C_WIN");
}

/* 设置通道DITHER模式信息 */
hi_void vicap_hal_chn_set_dither_en(hi_u32 vicap_id, hi_bool enable)
{
    u_ch_dither_cfg ch_dither_cfg;

    ch_dither_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_dither_cfg(vicap_id));

    ch_dither_cfg.bits.enable = enable;

    vicap_hal_reg_write(vicap_reg_offset_ch_dither_cfg(vicap_id), ch_dither_cfg.u32, "U_CH_DITHER_CFG");
}

/* 设置通道DITHER模式信息 */
hi_void vicap_hal_chn_set_dither_cfg(hi_u32 vicap_id, vicap_hal_chn_dither_cfg dither_cfg)
{
    u_ch_dither_cfg ch_dither_cfg;

    ch_dither_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_dither_cfg(vicap_id));

    ch_dither_cfg.bits.mode = dither_cfg;
    ch_dither_cfg.bits.round = VICAP_HAL_DITHER_MODE_ROUND; /* 0:Dither mode,1:Rounding mode */
    ch_dither_cfg.bits.enable = HI_TRUE;

    vicap_hal_reg_write(vicap_reg_offset_ch_dither_cfg(vicap_id), ch_dither_cfg.u32, "U_CH_DITHER_CFG");
}

hi_void vicap_hal_chn_get_dither_cfg(hi_u32 vicap_id, hi_bool *en_dither_cfg)
{
    u_ch_dither_cfg ch_dither_cfg;

    ch_dither_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_dither_cfg(vicap_id));
    *en_dither_cfg = ch_dither_cfg.bits.enable;
}

/* 设置亮度缩放参数更新 */
hi_void vicap_hal_chn_set_coef_yupdate(hi_u32 vicap_id, hi_bool valid)
{
    u_ch_coef_update ch_coef_update;

    ch_coef_update.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_coef_update(vicap_id));

    ch_coef_update.bits.lhcoef_update = valid;

    vicap_hal_reg_write(vicap_reg_offset_ch_coef_update(vicap_id), ch_coef_update.u32, "U_CH_COEF_UPDATE");
}

/* 设置色度缩放参数更新 */
hi_void vicap_hal_chn_set_coef_cupdate(hi_u32 vicap_id, hi_bool valid)
{
    u_ch_coef_update ch_coef_update;

    ch_coef_update.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_coef_update(vicap_id));

    ch_coef_update.bits.chcoef_update = valid;

    vicap_hal_reg_write(vicap_reg_offset_ch_coef_update(vicap_id), ch_coef_update.u32, "U_CH_COEF_UPDATE");
}

/* 设置亮度缩放系数读取选择 */
hi_void vicap_hal_chn_set_coef_yresl(hi_u32 vicap_id, vicap_hal_chn_scale_coef_type coef_type)
{
    u_ch_coef_rsel ch_coef_rsel;

    ch_coef_rsel.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_coef_rsel(vicap_id));

    ch_coef_rsel.bits.lhcoef_read_sel = coef_type;

    vicap_hal_reg_write(vicap_reg_offset_ch_coef_rsel(vicap_id), ch_coef_rsel.u32, "U_CH_COEF_RSEL");
}

/* 设置色度缩放系数读取选择 */
hi_void vicap_hal_chn_set_coef_cresl(hi_u32 vicap_id, vicap_hal_chn_scale_coef_type coef_type)
{
    u_ch_coef_rsel ch_coef_rsel;

    ch_coef_rsel.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_coef_rsel(vicap_id));

    ch_coef_rsel.bits.chcoef_read_sel = coef_type;

    vicap_hal_reg_write(vicap_reg_offset_ch_coef_rsel(vicap_id), ch_coef_rsel.u32, "U_CH_COEF_RSEL");
}

/* 配置亮度水平缩放 */
hi_void vicap_hal_chn_set_hscale_yen(hi_u32 vicap_id, hi_bool enable)
{
    u_ch_lhfir_cfg ch_lhfir_cfg;

    ch_lhfir_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_lhfir_cfg(vicap_id));

    ch_lhfir_cfg.bits.enable = enable;

    vicap_hal_reg_write(vicap_reg_offset_ch_lhfir_cfg(vicap_id), ch_lhfir_cfg.u32, "U_CH_LHFIR_CFG");
}

hi_void vicap_hal_chn_set_hscale_ycfg(hi_u32 vicap_id, vicap_hal_chn_hscale_cfg *chn_hscale_cfg)
{
    u_ch_lhfir_cfg ch_lhfir_cfg;

    ch_lhfir_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_lhfir_cfg(vicap_id));

    ch_lhfir_cfg.bits.ratio = chn_hscale_cfg->ratio;
    ch_lhfir_cfg.bits.fir_en = chn_hscale_cfg->chn_hscale_mode;
    ch_lhfir_cfg.bits.mid_en = chn_hscale_cfg->mid_en;
    ch_lhfir_cfg.bits.enable = chn_hscale_cfg->scale_en;

    vicap_hal_reg_write(vicap_reg_offset_ch_lhfir_cfg(vicap_id), ch_lhfir_cfg.u32, "U_CH_LHFIR_CFG");
}

hi_void vicap_hal_chn_get_hscale_ycfg(hi_u32 vicap_id, vicap_hal_chn_hscale_cfg *chn_h_scale_cfg)
{
    u_ch_lhfir_cfg ch_lhfir_cfg;
    ch_lhfir_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_lhfir_cfg(vicap_id));

    chn_h_scale_cfg->scale_en = ch_lhfir_cfg.bits.enable;
}

/* 配置色度水平缩放 */
hi_void vicap_hal_chn_set_hscale_cen(hi_u32 vicap_id, hi_bool enable)
{
    u_ch_chfir_cfg ch_chfir_cfg;

    ch_chfir_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_chfir_cfg(vicap_id));

    ch_chfir_cfg.bits.enable = enable;

    vicap_hal_reg_write(vicap_reg_offset_ch_chfir_cfg(vicap_id), ch_chfir_cfg.u32, "U_CH_CHFIR_CFG");
}

hi_void vicap_hal_chn_set_hscale_ccfg(hi_u32 vicap_id, vicap_hal_chn_hscale_cfg *chn_hscale_cfg)
{
    u_ch_chfir_cfg ch_chfir_cfg;

    ch_chfir_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_chfir_cfg(vicap_id));

    ch_chfir_cfg.bits.ratio = chn_hscale_cfg->ratio;
    ch_chfir_cfg.bits.fir_en = chn_hscale_cfg->chn_hscale_mode;
    ch_chfir_cfg.bits.mid_en = chn_hscale_cfg->mid_en;
    ch_chfir_cfg.bits.enable = chn_hscale_cfg->scale_en;

    vicap_hal_reg_write(vicap_reg_offset_ch_chfir_cfg(vicap_id), ch_chfir_cfg.u32, "U_CH_CHFIR_CFG");
}

hi_void vicap_hal_chn_get_hscale_ccfg(hi_u32 vicap_id, vicap_hal_chn_hscale_cfg *chn_h_scale_cfg)
{
    u_ch_lhfir_cfg ch_lhfir_cfg;
    ch_lhfir_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_chfir_cfg(vicap_id));

    chn_h_scale_cfg->scale_en = ch_lhfir_cfg.bits.enable;
}

/* 配置亮度垂直缩放 */
hi_void vicap_hal_chn_set_vscale_yen(hi_u32 vicap_id, hi_bool enable)
{
    u_ch_lvfir_cfg ch_lvfir_cfg;

    ch_lvfir_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_lvfir_cfg(vicap_id));

    ch_lvfir_cfg.bits.enable = enable;

    vicap_hal_reg_write(vicap_reg_offset_ch_lvfir_cfg(vicap_id), ch_lvfir_cfg.u32, "U_CH_LVFIR_CFG");
}

hi_void vicap_hal_chn_set_vscale_ycfg(hi_u32 vicap_id, vicap_hal_chn_vscale_cfg *chn_vscale_cfg)
{
    u_ch_lvfir_cfg ch_lvfir_cfg;

    ch_lvfir_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_lvfir_cfg(vicap_id));

    ch_lvfir_cfg.bits.ratio = chn_vscale_cfg->ratio;
    ch_lvfir_cfg.bits.enable = chn_vscale_cfg->scale_en;

    vicap_hal_reg_write(vicap_reg_offset_ch_lvfir_cfg(vicap_id), ch_lvfir_cfg.u32, "U_CH_LVFIR_CFG");
}

hi_void vicap_hal_chn_get_vscale_ycfg(hi_u32 vicap_id, vicap_hal_chn_vscale_cfg *chn_v_scale_cfg)
{
    u_ch_lhfir_cfg ch_lvfir_cfg;
    ch_lvfir_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_lvfir_cfg(vicap_id));

    chn_v_scale_cfg->scale_en = ch_lvfir_cfg.bits.enable;
}

/* 配置色度垂直缩放 */
hi_void vicap_hal_chn_set_vscale_cen(hi_u32 vicap_id, hi_bool enable)
{
    u_ch_cvfir_cfg ch_cvfir_cfg;

    ch_cvfir_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_cvfir_cfg(vicap_id));
    ch_cvfir_cfg.bits.enable = enable;

    vicap_hal_reg_write(vicap_reg_offset_ch_cvfir_cfg(vicap_id), ch_cvfir_cfg.u32, "U_CH_CVFIR_CFG");
}

hi_void vicap_hal_chn_set_vscale_ccfg(hi_u32 vicap_id, vicap_hal_chn_vscale_cfg *chn_vscale_cfg)
{
    u_ch_cvfir_cfg ch_cvfir_cfg;

    ch_cvfir_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_cvfir_cfg(vicap_id));

    ch_cvfir_cfg.bits.ratio = chn_vscale_cfg->ratio;
    ch_cvfir_cfg.bits.enable = chn_vscale_cfg->scale_en;

    vicap_hal_reg_write(vicap_reg_offset_ch_cvfir_cfg(vicap_id), ch_cvfir_cfg.u32, "U_CH_CVFIR_CFG");
}

hi_void vicap_hal_chn_get_vscale_ccfg(hi_u32 vicap_id, vicap_hal_chn_vscale_cfg *chn_v_scale_cfg)
{
    u_ch_lhfir_cfg ch_cvfir_cfg;
    ch_cvfir_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_cvfir_cfg(vicap_id));

    chn_v_scale_cfg->scale_en = ch_cvfir_cfg.bits.enable;
}

/* 设置缩放输入 */
hi_void vicap_hal_chn_set_scale_hfir_in_size(hi_u32 vicap_id, vicap_hal_chn_scale_in_size *scale_in_size)
{
    u_ch_lfir_in_size ch_lfir_in_size;
    u_ch_cfir_in_size ch_cfir_in_size;

    ch_lfir_in_size.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_lfir_in_size(vicap_id));
    ch_lfir_in_size.u32 = 0;
    ch_lfir_in_size.bits.width = scale_in_size->y_in_pixel - 1;
    ch_lfir_in_size.bits.height = scale_in_size->y_in_line - 1;

    ch_cfir_in_size.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_cfir_in_size(vicap_id));
    ch_cfir_in_size.u32 = 0;
    ch_cfir_in_size.bits.width = scale_in_size->c_in_pixel - 1;
    ch_cfir_in_size.bits.height = scale_in_size->c_in_line - 1;

    vicap_hal_reg_write(vicap_reg_offset_ch_lfir_in_size(vicap_id), ch_lfir_in_size.u32, "U_CH_LFIR_IN_SIZE");
    vicap_hal_reg_write(vicap_reg_offset_ch_cfir_in_size(vicap_id), ch_cfir_in_size.u32, "U_CH_CFIR_IN_SIZE");
}

/* 设置缩放亮度输出 */
hi_void vicap_hal_chn_set_scale_hfir_out_size(hi_u32 vicap_id, vicap_hal_chn_scale_out_size *scale_out_size)
{
    u_ch_lfir_out_size ch_lfir_out_size;
    u_ch_cfir_out_size ch_cfir_out_size;

    ch_lfir_out_size.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_lfir_out_size(vicap_id));
    ch_lfir_out_size.u32 = 0;
    ch_lfir_out_size.bits.width = scale_out_size->y_out_pixel - 1;
    ch_lfir_out_size.bits.height = scale_out_size->y_out_line - 1;

    ch_cfir_out_size.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_cfir_out_size(vicap_id));
    ch_cfir_out_size.u32 = 0;
    ch_cfir_out_size.bits.width = scale_out_size->c_out_pixel - 1;
    ch_cfir_out_size.bits.height = scale_out_size->c_out_line - 1;

    vicap_hal_reg_write(vicap_reg_offset_ch_lfir_out_size(vicap_id), ch_lfir_out_size.u32, "U_CH_LFIR_OUT_SIZE");
    vicap_hal_reg_write(vicap_reg_offset_ch_cfir_out_size(vicap_id), ch_cfir_out_size.u32, "U_CH_CFIR_OUT_SIZE");
}

/* 设置亮度缩放系数 */
static hi_void vicap_hal_chn_set_scale_hlfir_coef(hi_u32 vicap_id, hi_u32 phase, vicap_reg_ch_lhfir_coef lhfir_coef)
{
    vicap_hal_reg_write(vicap_reg_offset_ch_lhfir_coef0(vicap_id) + phase * sizeof(vicap_reg_ch_lhfir_coef),
                        lhfir_coef.u_ch_lhfir_coef0.u32, "U_CH_LHFIR_COEF0");
    vicap_hal_reg_write(vicap_reg_offset_ch_lhfir_coef1(vicap_id) + phase * sizeof(vicap_reg_ch_lhfir_coef),
                        lhfir_coef.u_ch_lhfir_coef1.u32, "U_CH_LHFIR_COEF1");
    vicap_hal_reg_write(vicap_reg_offset_ch_lhfir_coef2(vicap_id) + phase * sizeof(vicap_reg_ch_lhfir_coef),
                        lhfir_coef.u_ch_lhfir_coef2.u32, "U_CH_LHFIR_COEF2");
}

/* 设置色度缩放系数 */
static hi_void vicap_hal_chn_set_scale_hcfir_coef(hi_u32 vicap_id, hi_u32 phase, vicap_reg_ch_chfir_coef chfir_coef)
{
    vicap_hal_reg_write(vicap_reg_offset_ch_chfir_coef0(vicap_id) + phase * sizeof(vicap_reg_ch_lhfir_coef),
                        chfir_coef.u_ch_chfir_coef0.u32, "U_CH_CHFIR_COEF0");
    vicap_hal_reg_write(vicap_reg_offset_ch_chfir_coef1(vicap_id) + phase * sizeof(vicap_reg_ch_lhfir_coef),
                        chfir_coef.u_ch_chfir_coef1.u32, "U_CH_CHFIR_COEF1");
    vicap_hal_reg_write(vicap_reg_offset_ch_chfir_coef2(vicap_id) + phase * sizeof(vicap_reg_ch_lhfir_coef),
                        chfir_coef.u_ch_chfir_coef2.u32, "U_CH_CHFIR_COEF2");
}

hi_s32 vicap_hal_chn_set_hscale_coef8_phase(hi_u32 vicap_id, vicap_hal_chn_scale_obj scaler_obj,
                                            vicap_hal_chn_scale_coef coef)
{
    hi_s32 i = 0;
    vicap_reg_ch_lhfir_coef ast_lhfir_coef[COEFF_GRPNUM] = {0};
    vicap_reg_ch_chfir_coef ast_chfir_coef[COEFF_GRPNUM] = {0};
    hi_s32(*as_coef_para)[COEFF_OFF_NUM] = HI_NULL;

    if (coef == VICAP_HAL_CHN_SCALE_COEF_NO) {
        as_coef_para = g_coef8_no_scale;
    } else if (coef == VICAP_HAL_CHN_SCALE_COEF_2X) {
        as_coef_para = g_coef8_2x_scale;
    } else if (coef == VICAP_HAL_CHN_SCALE_COEF_3X) {
        as_coef_para = g_coef8_3x_scale;
    } else if (coef == VICAP_HAL_CHN_SCALE_COEF_4X) {
        as_coef_para = g_coef8_4x_scale;
    } else {
        as_coef_para = g_coef8_other_scale;
    }

    for (i = 0; i < COEFF_GRPNUM; i++) {
        if (scaler_obj == VICAP_HAL_CHN_SCALE_OBJ_YH) {
            ast_lhfir_coef[i].u_ch_lhfir_coef0.bits.coef0 = as_coef_para[i][0]; /* index 0 */
            ast_lhfir_coef[i].u_ch_lhfir_coef0.bits.coef1 = as_coef_para[i][1]; /* index 1 */
            ast_lhfir_coef[i].u_ch_lhfir_coef0.bits.coef2 = as_coef_para[i][2]; /* index 2 */
            ast_lhfir_coef[i].u_ch_lhfir_coef1.bits.coef3 = as_coef_para[i][3]; /* index 3 */
            ast_lhfir_coef[i].u_ch_lhfir_coef1.bits.coef4 = as_coef_para[i][4]; /* index 4 */
            ast_lhfir_coef[i].u_ch_lhfir_coef1.bits.coef5 = as_coef_para[i][5]; /* index 5 */
            ast_lhfir_coef[i].u_ch_lhfir_coef2.bits.coef6 = as_coef_para[i][6]; /* index 6 */
            ast_lhfir_coef[i].u_ch_lhfir_coef2.bits.coef7 = as_coef_para[i][7]; /* index 7 */
            vicap_hal_chn_set_scale_hlfir_coef(vicap_id, i, ast_lhfir_coef[i]);
        }

        if (scaler_obj == VICAP_HAL_CHN_SCALE_OBJ_CH) {
            ast_chfir_coef[i].u_ch_chfir_coef0.bits.coef0 = as_coef_para[i][0]; /* index 0 */
            ast_chfir_coef[i].u_ch_chfir_coef0.bits.coef1 = as_coef_para[i][1]; /* index 1 */
            ast_chfir_coef[i].u_ch_chfir_coef0.bits.coef2 = as_coef_para[i][2]; /* index 2 */
            ast_chfir_coef[i].u_ch_chfir_coef1.bits.coef3 = as_coef_para[i][3]; /* index 3 */
            ast_chfir_coef[i].u_ch_chfir_coef1.bits.coef4 = as_coef_para[i][4]; /* index 4 */
            ast_chfir_coef[i].u_ch_chfir_coef1.bits.coef5 = as_coef_para[i][5]; /* index 5 */
            ast_chfir_coef[i].u_ch_chfir_coef2.bits.coef6 = as_coef_para[i][6]; /* index 6 */
            ast_chfir_coef[i].u_ch_chfir_coef2.bits.coef7 = as_coef_para[i][7]; /* index 7 */
            vicap_hal_chn_set_scale_hcfir_coef(vicap_id, i, ast_chfir_coef[i]);
        }
    }

    return HI_SUCCESS;
}

hi_void vicap_hal_chn_set_ink_ctrl(hi_u32 vicap_id, vicap_hal_chn_ink_ctrl *ink_ctrl_info)
{
    u_ch_csc_ink_ctrl ch_ink_ctrl;
    ch_ink_ctrl.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_csc_ink_ctrl(vicap_id));

    ch_ink_ctrl.u32 = 0;
    ch_ink_ctrl.bits.ink_en = ink_ctrl_info->enable;
    ch_ink_ctrl.bits.data_fmt = ink_ctrl_info->pix_fmt;
    ch_ink_ctrl.bits.ink_sel = ink_ctrl_info->ink_sel;
    ch_ink_ctrl.bits.cross_enable = ink_ctrl_info->enable;
    ch_ink_ctrl.bits.color_mode = ink_ctrl_info->color_mode;
    ch_ink_ctrl.bits.x_pos = ink_ctrl_info->x_pos;
    ch_ink_ctrl.bits.y_pos = ink_ctrl_info->y_pos;

    vicap_hal_reg_write(vicap_reg_offset_ch_csc_ink_ctrl(vicap_id), ch_ink_ctrl.u32, "U_CH_CSC_INK_CTRL");
}

hi_void vicap_hal_chn_get_ink_data(hi_u32 vicap_id, vicap_hal_chn_ink_data *ink_data_info)
{
    u_ch_csc_ink_data ch_csc_ink_data;
    ch_csc_ink_data.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_csc_ink_data(vicap_id));

    ink_data_info->data1 = ch_csc_ink_data.bits.ink_d0;
    ink_data_info->data2 = ch_csc_ink_data.bits.ink_d1;
    ink_data_info->data3 = ch_csc_ink_data.bits.ink_d2;
}

hi_void vicap_hal_chn_set_hdr_ctrl(hi_u32 vicap_id, vicap_hal_chn_hdr_ctrl *chn_hdr_ctrl)
{
    u_ch_hdr_ctrl ch_hdrctrl_cfg;

    ch_hdrctrl_cfg.u32 = vicap_hal_reg_read(vicap_reg_offset_ch_hdr_ctrl(vicap_id));

    ch_hdrctrl_cfg.bits.max_metadata_length = chn_hdr_ctrl->max_metadata_length;
    ch_hdrctrl_cfg.bits.hdr_det_en = chn_hdr_ctrl->en_hdr_det;
    ch_hdrctrl_cfg.bits.hdr_wr_en = chn_hdr_ctrl->en_hdr_wr;
    ch_hdrctrl_cfg.bits.hdr_en = chn_hdr_ctrl->en_hdr_en;

    vicap_hal_reg_write(vicap_reg_offset_ch_hdr_ctrl(vicap_id), ch_hdrctrl_cfg.u32, "U_CH_HDR_CFG");
}

hi_void vicap_hal_chn_get_hdr_status0(hi_u32 vicap_id, hi_u32 *chn_hdr_status0)
{
    hi_u32 ch_hdrstatus_cfg;
    ch_hdrstatus_cfg = vicap_hal_reg_read(vicap_reg_offset_ch_hdr_status0(vicap_id));

    *chn_hdr_status0 = ch_hdrstatus_cfg;
}

hi_void vicap_hal_chn_set_fstart_dly(hi_u32 vicap_id, hi_u32 f_start_dly_cycle)
{
    hi_u32 ch_fstartdly_cfg;

    ch_fstartdly_cfg = f_start_dly_cycle;

    vicap_hal_reg_write(vicap_reg_offset_ch_fstart_dly_cfg(vicap_id), ch_fstartdly_cfg, "U_CH_FSTART_DLY_CFG");
}

hi_void vicap_hal_chn_set_metadata_wch_addr_h(hi_u32 vicap_id, hi_u32 metadata_wch_addr_h)
{
    hi_u32 ch_hdrwchaddr_cfg;

    ch_hdrwchaddr_cfg = metadata_wch_addr_h;

    vicap_hal_reg_write(vicap_reg_offset_ch_hdr_wch_addr_h(vicap_id), ch_hdrwchaddr_cfg, "U_CH_HDR_CFG");
}

hi_void vicap_hal_chn_set_metadata_wch_addr_l(hi_u32 vicap_id, hi_u32 metadata_wch_addr_l)
{
    hi_u32 ch_hdrwchaddr_cfg;

    ch_hdrwchaddr_cfg = metadata_wch_addr_l;

    vicap_hal_reg_write(vicap_reg_offset_ch_hdr_wch_addr_l(vicap_id), ch_hdrwchaddr_cfg, "U_CH_HDR_CFG");
}

hi_u32 vicap_hal_chn_get_ycheck_sum(hi_u32 vicap_id)
{
    hi_u32 ch_check_sum_y_cfg;
    ch_check_sum_y_cfg = vicap_hal_reg_read(vicap_reg_offset_ch_check_sum_y_l(vicap_id));

    return ch_check_sum_y_cfg;
}

hi_u32 vicap_hal_chn_get_ccheck_sum(hi_u32 vicap_id)
{
    hi_u32 ch_check_sum_c_cfg;
    ch_check_sum_c_cfg = vicap_hal_reg_read(vicap_reg_offset_ch_check_sum_c_l(vicap_id));

    return ch_check_sum_c_cfg;
}

hi_void vicap_hal_chn_enable_tee_int(hi_u32 vicap_id, hi_bool enable)
{
    u_vicap_int_mask_tee mask;

    mask.u32 = vicap_hal_reg_read(vicap_reg_offset_vicap_int_mask_tee(vicap_id));
    mask.bits.int_ch0_tee = enable;
    vicap_hal_reg_write(vicap_reg_offset_vicap_int_mask_tee(vicap_id), mask.u32, "U_VICAP_INT_MASK_TEE");
}

/*=============================vicap smmu=====================================*/
/* SMMU全局控制寄存器 0xF000 */
hi_void vicap_hal_smmu_set_ptw(hi_u32 vicap_id, hi_u32 ptw)
{
    u_smmu_ctrl smmu_src;

    smmu_src.u32 = vicap_hal_reg_read(vicap_reg_offset_smmu_ctrl(vicap_id));

    smmu_src.bits.ptw_pf = ptw;

    vicap_hal_reg_write(vicap_reg_offset_smmu_ctrl(vicap_id), smmu_src.u32, "U_SMMU_SCR");
}

/* SMMU全局控制寄存器 0xF000 */
hi_void vicap_hal_smmu_set_int_en(hi_u32 vicap_id, hi_bool en)
{
    u_smmu_ctrl smmu_src;

    smmu_src.u32 = vicap_hal_reg_read(vicap_reg_offset_smmu_ctrl(vicap_id));

    smmu_src.bits.int_en = en;

    vicap_hal_reg_write(vicap_reg_offset_smmu_ctrl(vicap_id), smmu_src.u32, "U_SMMU_SCR");
}

/* SMMU全局控制寄存器 0xF000 */
hi_void vicap_hal_smmu_set_global_bypass(hi_u32 vicap_id, hi_bool en)
{
    u_smmu_scr smmu_src;

    smmu_src.u32 = vicap_hal_reg_read(vicap_reg_offset_smmu_scr(vicap_id));

    smmu_src.bits.glb_bypass = en;

    vicap_hal_reg_write(vicap_reg_offset_smmu_scr(vicap_id), smmu_src.u32, "U_SMMU_SCR");
}

hi_void vicap_hal_smmu_get_global_bypass(hi_u32 vicap_id, hi_bool *en)
{
    u_smmu_scr smmu_src;
    smmu_src.u32 = vicap_hal_reg_read(vicap_reg_offset_smmu_ctrl(vicap_id));
    *en = smmu_src.bits.glb_bypass;
}

/* SMMU低功耗控制寄存器 0xF008 */
hi_void vicap_hal_smmu_set_clk_gate_en(hi_u32 vicap_id, hi_bool en)
{
    u_smmu_lp_ctrl smmu_lp_ctrl;

    smmu_lp_ctrl.u32 = vicap_hal_reg_read(vicap_reg_offset_smmu_lp_ctrl(vicap_id));

    smmu_lp_ctrl.bits.auto_clk_gt_en = en;

    vicap_hal_reg_write(vicap_reg_offset_smmu_lp_ctrl(vicap_id), smmu_lp_ctrl.u32, "U_SMMU_LP_CTRL");
}

/* SMMU非安全屏蔽寄存器 0xF020 */
hi_void vicap_hal_smmu_set_int_mask(hi_u32 vicap_id, vicap_hal_smmu_safe_type type, hi_u32 int_mask)
{
    u_smmu_intmask_ns smmu_intmask_ns;

    smmu_intmask_ns.u32 = vicap_hal_reg_read(vicap_reg_offset_smmu_intmask_ns(vicap_id));

    smmu_intmask_ns.bits.intns_tlbmiss_msk = int_mask & VICAP_HAL_SMMU_INT_BIT_NS_TLBMISS;
    smmu_intmask_ns.bits.intns_ptw_trans_msk = int_mask & VICAP_HAL_SMMU_INT_BIT_NS_PTW_TRANS;
    smmu_intmask_ns.bits.intns_tlbinvalid_rd_msk = int_mask & VICAP_HAL_SMMU_INT_BIT_NS_TLBINVALID_RD;
    smmu_intmask_ns.bits.intns_tlbinvalid_wr_msk = int_mask & VICAP_HAL_SMMU_INT_BIT_NS_TLBINVALID_WR;

    vicap_hal_reg_write(vicap_reg_offset_smmu_intmask_ns(vicap_id), smmu_intmask_ns.u32, "U_SMMU_INTMASK_NS");
}

/* SMMU非安全中断源寄存器 0xF024 */
hi_void vicap_hal_smmu_get_int_raw(hi_u32 vicap_id, vicap_hal_smmu_safe_type type, hi_u32 *int_bit)
{
    u_smmu_intraw_ns smmu_intraw_ns;

    if (type == VICAP_HAL_SMMU_SAFE_TYPE_UNSAFE) {
        smmu_intraw_ns.u32 = vicap_hal_reg_read(vicap_reg_offset_smmu_intraw_ns(vicap_id));

        *int_bit =
            smmu_intraw_ns.u32 & (VICAP_HAL_SMMU_INT_BIT_NS_TLBINVALID_WR | VICAP_HAL_SMMU_INT_BIT_NS_TLBINVALID_RD |
                                  VICAP_HAL_SMMU_INT_BIT_NS_PTW_TRANS | VICAP_HAL_SMMU_INT_BIT_NS_TLBMISS);
    }
}

/* SMMU非安全中断状态寄存器 0xF028 */
hi_void vicap_hal_smmu_get_int_status(hi_u32 vicap_id, vicap_hal_smmu_safe_type type, hi_u32 *int_bit)
{
    u_smmu_intstat_ns smmu_intstat_ns;

    if (type == VICAP_HAL_SMMU_SAFE_TYPE_UNSAFE) {
        smmu_intstat_ns.u32 = vicap_hal_reg_read(vicap_reg_offset_smmu_intstat_ns(vicap_id));

        *int_bit =
            smmu_intstat_ns.u32 & (VICAP_HAL_SMMU_INT_BIT_NS_TLBINVALID_WR | VICAP_HAL_SMMU_INT_BIT_NS_TLBINVALID_RD |
                                   VICAP_HAL_SMMU_INT_BIT_NS_PTW_TRANS | VICAP_HAL_SMMU_INT_BIT_NS_TLBMISS);
    }
}

/* SMMU非安全中断清除寄存器 0xF028 */
hi_void vicap_hal_smmu_set_int_clr(hi_u32 vicap_id, vicap_hal_smmu_safe_type type, hi_u32 int_bit)
{
    u_smmu_intclr_ns smmu_intclr_ns;

    if (type == VICAP_HAL_SMMU_SAFE_TYPE_UNSAFE) {
        smmu_intclr_ns.u32 = vicap_hal_reg_read(vicap_reg_offset_smmu_intclr_ns(vicap_id));

        if (int_bit & VICAP_HAL_SMMU_INT_BIT_NS_TLBINVALID_WR) {
            smmu_intclr_ns.u32 |= VICAP_HAL_SMMU_INT_BIT_NS_TLBINVALID_WR;
        }

        if (int_bit & VICAP_HAL_SMMU_INT_BIT_NS_TLBINVALID_RD) {
            smmu_intclr_ns.u32 |= VICAP_HAL_SMMU_INT_BIT_NS_TLBINVALID_RD;
        }

        if (int_bit & VICAP_HAL_SMMU_INT_BIT_NS_PTW_TRANS) {
            smmu_intclr_ns.u32 |= VICAP_HAL_SMMU_INT_BIT_NS_PTW_TRANS;
        }

        if (int_bit & VICAP_HAL_SMMU_INT_BIT_NS_TLBMISS) {
            smmu_intclr_ns.u32 |= VICAP_HAL_SMMU_INT_BIT_NS_TLBMISS;
        }

        vicap_hal_reg_write(vicap_reg_offset_smmu_intclr_ns(vicap_id), smmu_intclr_ns.u32, "U_SMMU_INTCLR_NS");
    }
}

/* 设置非安全页表基地址 0xF20C */
hi_void vicap_hal_smmu_set_table_base_addr(hi_u32 vicap_id, vicap_hal_smmu_safe_type type, hi_u32 base_addr)
{
    hi_u32 smmu_scb_ttbr;

    if (type == VICAP_HAL_SMMU_SAFE_TYPE_UNSAFE) {
        smmu_scb_ttbr = base_addr;

        vicap_hal_reg_write(vicap_reg_offset_smmu_cb_ttbr(vicap_id), smmu_scb_ttbr, "U_SMMU_SCB_TTBR");
    }
}

/* SMMU非安全读错误地址默认寄存器 0xF304 */
hi_void vicap_hal_smmu_set_err_read_addr(hi_u32 vicap_id, vicap_hal_smmu_safe_type type, hi_u32 addr)
{
    hi_u32 smmu_err_rd_addr_ns;

    if (type == VICAP_HAL_SMMU_SAFE_TYPE_UNSAFE) {
        smmu_err_rd_addr_ns = addr;

        vicap_hal_reg_write(vicap_reg_offset_smmu_err_rdaddr_ns(vicap_id), smmu_err_rd_addr_ns,
                            "VICAP_REG_OFFSET_SMMU_ERR_RDADDR_NS");
    }
}

/* SMMU安全预取错误地址寄存器 0xF308 */
hi_void vicap_hal_smmu_set_err_write_addr(hi_u32 vicap_id, vicap_hal_smmu_safe_type type, hi_u32 addr)
{
    hi_u32 smmu_err_wr_addr_ns;

    if (type == VICAP_HAL_SMMU_SAFE_TYPE_UNSAFE) {
        smmu_err_wr_addr_ns = addr;

        vicap_hal_reg_write(vicap_reg_offset_smmu_err_wraddr_ns(vicap_id), smmu_err_wr_addr_ns, "U_SMMU_ERR_WRADDR_NS");
    }
}
