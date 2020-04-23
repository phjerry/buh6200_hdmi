/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_ip_mmu.h"

#include "hal_vdp_comm.h"
#include "hal_vdp_reg_mmu.h"
#include "hi_osal.h"

static hi_u32 g_mmu_offset_addr[VDP_MMU_SEL_BUTT] = {
    VDP_MMU0_OFFSET,
    0,
    VDP_MMU1_OFFSET,
};

hi_void vdp_ip_mmu_set(hi_bool smmu_enable)
{
#ifdef HI_SMMU_SUPPORT
    hi_u32 i;
    hi_u32 offset;

#ifdef CBB_OSAL_TYPE_SUPPORT
    hi_u32 page_addr = 0;
    hi_u32 err_read_addr = 0;
    hi_u32 err_write_addr = 0;
#else
    hi_ulong page_addr;
    hi_ulong err_read_addr;
    hi_ulong err_write_addr;
#endif

    for (i = 0; i < VDP_MMU_SEL_BUTT; i++) {
        offset = g_mmu_offset_addr[i];

        vdp_mmu_setpagetyps(g_vdp_reg, offset, 0);
        vdp_mmu_setglbbypass(g_vdp_reg, offset, !smmu_enable);
        vdp_mmu_setautoclkgten(g_vdp_reg, offset, HI_FALSE);
        vdp_mmu_setinten(g_vdp_reg, offset, HI_FALSE);
        vdp_mmu_setptwpf(g_vdp_reg, offset, 15); /* 15 is a reg value  */

#ifdef CBB_OSAL_TYPE_SUPPORT
        HI_DRV_SMMU_GetPageTableAddr(&page_addr, &err_read_addr, &err_write_addr);
        vdp_mmu_setcbttbr(g_vdp_reg, offset, page_addr);
        vdp_mmu_seterrnsrdaddr(g_vdp_reg, offset, err_read_addr);
        vdp_mmu_seterrnswraddr(g_vdp_reg, offset, err_write_addr);
#else
        osal_mem_get_nssmmu_pgtinfo(&page_addr, &err_read_addr, &err_write_addr);
        vdp_mmu_setcbttbr(g_vdp_reg, offset, page_addr & 0xffffffff);
        vdp_mmu_setcbttbrh(g_vdp_reg, offset, page_addr);
        vdp_mmu_seterrnsrdaddr(g_vdp_reg, offset, err_read_addr & 0xffffffff);
        vdp_mmu_seterrnsrdaddrh(g_vdp_reg, offset, (err_read_addr >> 32) & 0xffffffff); // 32 addr offset
        vdp_mmu_seterrnswraddr(g_vdp_reg, offset, err_write_addr & 0xffffffff);
        vdp_mmu_seterrnswraddrh(g_vdp_reg, offset, (err_write_addr >> 32) & 0xffffffff); // 32 addr offset
#endif

        vdp_mmu_setintnstlbinvalidrdmsk(g_vdp_reg, offset, HI_FALSE);
        vdp_mmu_setintnstlbinvalidwrmsk(g_vdp_reg, offset, HI_FALSE);
        vdp_mmu_setintnsptwtransmsk(g_vdp_reg, offset, HI_FALSE);
        vdp_mmu_setintnstlbmissmsk(g_vdp_reg, offset, HI_FALSE);
    }
#endif
    return;
}

hi_s32 vdp_ip_mmu_get_interrupt_state(hi_void)
{
    hi_u32 i, offset;
    hi_u32 smmu_err_state = 0;

    for (i = 0; i < VDP_MMU_SEL_BUTT; i++) {
        offset = g_mmu_offset_addr[i];
        smmu_err_state |= vdp_mmu_get_error_state(g_vdp_reg, offset);
    }

    return smmu_err_state;
}

hi_void vdp_ip_mmu_clean_interrupt_state(hi_s32 state)
{
    hi_u32 i,offset;

    for (i = 0; i < VDP_MMU_SEL_BUTT; i++) {
        offset = g_mmu_offset_addr[i];
        vdp_mmu_clear_error_state(g_vdp_reg, offset, state);
    }

    return;
}

hi_s32 vdp_ip_mmu_get_error_addr_ns(hi_void)
{
    hi_u32 offset;
    hi_u32 smmu_err_addr;

    offset = g_mmu_offset_addr[VDP_MMU_SEL_VDP_0];
    smmu_err_addr = vdp_mmu_get_error_addr_ns(g_vdp_reg, offset);

    return smmu_err_addr;
}

