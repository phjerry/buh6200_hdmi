/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: function define
* Author: image
* Create: 2019-04-12
 */

#include "hal_vdp_comm.h"
#include "hal_vdp_ip_para.h"

#ifndef __DISP_PLATFORM_BOOT__
#include <asm/io.h>
#endif

#define VDP_REGION_START_OFFSET 0x00f0b400
#define VDP_REGION_END_OFFSET 0x00f0C000

vdp_regs_type *g_vdp_reg = HI_NULL;
hi_reg_crg *g_vdp_crg_reg = HI_NULL;

hi_s32 vdp_reg_map(hi_void)
{
    if (g_vdp_reg == HI_NULL) {
#ifndef __DISP_PLATFORM_BOOT__
        g_vdp_reg = (vdp_regs_type *)osal_ioremap_nocache(VDP_BASE_ADDR, sizeof(vdp_regs_type));
#else
        g_vdp_reg = (vdp_regs_type *)VDP_BASE_ADDR;
#endif
        if (g_vdp_reg == HI_NULL) {
            VDP_ERROR("vdp map reg error!\n");
            return HI_FAILURE;
        }
    }

    if (g_vdp_crg_reg == HI_NULL) {
#ifndef __DISP_PLATFORM_BOOT__
        g_vdp_crg_reg = (hi_reg_crg *)osal_ioremap_nocache(VDP_CRG_BASE_ADDR, sizeof(hi_reg_crg));
#else
        g_vdp_crg_reg = (hi_reg_crg *)VDP_CRG_BASE_ADDR;
#endif
        if (g_vdp_crg_reg == HI_NULL) {
            VDP_ERROR("vdp map crg reg error!\n");
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

hi_void vdp_reg_unmap(hi_void)
{
    if (g_vdp_reg != HI_NULL) {
#ifndef __DISP_PLATFORM_BOOT__
        osal_iounmap(g_vdp_reg);
#endif
        g_vdp_reg = HI_NULL;
    }
    if (g_vdp_crg_reg != HI_NULL) {
#ifndef __DISP_PLATFORM_BOOT__
        osal_iounmap(g_vdp_crg_reg);
#endif
        g_vdp_crg_reg = HI_NULL;
    }
}

hi_u32 vdp_regread(uintptr_t a)
{
    return *((volatile hi_u32 *)a);
}

hi_void vdp_regwrite(uintptr_t a, hi_u32 b)
{
    *((volatile hi_u32 *)a) = b;
    return;
}

hi_u32 vdp_ddrread(uintptr_t a)
{
    hi_u64 addr = VDP_BASE_ADDR + (hi_u64)a - (hi_u64)(&(g_vdp_reg->voctrl.u32));

    if ((addr >= VDP_REGION_START_OFFSET) && (addr <= VDP_REGION_END_OFFSET)) {
        addr = (addr - VDP_REGION_START_OFFSET + (hi_u64)g_vdp_coef_buf_addr.vir_addr[VDP_COEF_BUF_REGION_V1]);
    }

    return *((volatile hi_u32 *)addr);
}

hi_void vdp_ddrwrite(uintptr_t a, hi_u32 b)
{
    hi_u32 *paddr;
    hi_u64 addr = VDP_BASE_ADDR + (hi_u64)a - (hi_u64)(&(g_vdp_reg->voctrl.u32));

    if ((addr >= VDP_REGION_START_OFFSET) && (addr <= VDP_REGION_END_OFFSET)) {
        addr = (addr - VDP_REGION_START_OFFSET + (hi_u64)g_vdp_coef_buf_addr.vir_addr[VDP_COEF_BUF_REGION_V1]);
        paddr = (hi_u32 *)addr;
        *paddr = b;
    }

    return;
}

