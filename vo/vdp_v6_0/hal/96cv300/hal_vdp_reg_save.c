/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_reg_save.h"
#include "drv_xdp_ion.h"
#include "hal_vdp_comm.h"

#define REG_ADDR_OFFSET(REG) ((hi_ulong) & (((vdp_regs_type *)0)->REG))
#define OFFSET_ELE_NUM       2

static hi_ulong g_reg_offset[][OFFSET_ELE_NUM] = {
    { REG_ADDR_OFFSET(voctrl),                   0x200 },
    { REG_ADDR_OFFSET(cbm_bkg1),                 0x200 },
    { REG_ADDR_OFFSET(vdp1_smmu_scr),            0x600 },
    { REG_ADDR_OFFSET(link_ctrl0),               0x100 },
    { REG_ADDR_OFFSET(para_haddr_vhd_chn00),     0x200 },
    { REG_ADDR_OFFSET(v1_ctrl),                  0x1000 },
    { REG_ADDR_OFFSET(v3_ctrl),                  0x1000 },
    { REG_ADDR_OFFSET(vp0_ctrl),                 0x300 },
    { REG_ADDR_OFFSET(g0_ctrl),                  0x300 },
    { REG_ADDR_OFFSET(g1_ctrl),                  0x300 },
    { REG_ADDR_OFFSET(g2_ctrl),                  0x300 },
    { REG_ADDR_OFFSET(g3_ctrl),                  0x400 },
    { REG_ADDR_OFFSET(gp0_f_ctrl),               0x300 },
    { REG_ADDR_OFFSET(dither_ctrl),              0x300 },
    { REG_ADDR_OFFSET(gp0_f_ctrl),               0x100 },
    { REG_ADDR_OFFSET(vdp_core_mst_outstanding), 0x1000 },
    { REG_ADDR_OFFSET(vdp1_smmu_scr),            0x500 },
    { REG_ADDR_OFFSET(vdp2_smmu_scr),            0x500 },
    { REG_ADDR_OFFSET(para_haddr_v0_chn00),      0x100 },
    { REG_ADDR_OFFSET(v0_ctrl),                  0x200 }, /* other v0 cfg in isr */
    { REG_ADDR_OFFSET(mst_outstanding),          0x200 },
    { REG_ADDR_OFFSET(vdp0_smmu_scr),            0x500 },
    { REG_ADDR_OFFSET(dispctrl),                 0x400 }, /* need check */
    { REG_ADDR_OFFSET(dhd_top_ctrl),             0x400 },
    { REG_ADDR_OFFSET(dhd1_ctrl),                0x200 },
};

drv_xdp_mem_info g_reg_save_buf = {
    .vir_addr = HI_NULL,
};

hi_u32 vdp_get_reg_save_size(hi_void)
{
    hi_u32 size = 0;
    hi_u32 idx;
    hi_u32 section_num = sizeof(g_reg_offset) / sizeof(hi_ulong) / OFFSET_ELE_NUM;

    for (idx = 0; idx < section_num; idx++) {
        size += g_reg_offset[idx][1];
    }

    return size;
}

hi_s32 vdp_ip_reg_save_record(hi_void)
{
    hi_u32 idx, section_num;
    hi_ulong offset = 0;

    if (g_reg_save_buf.vir_addr == HI_NULL) {
        drv_xdp_mem_attr memattr = {
            .mode = OSAL_MMZ_TYPE,
            .is_map_viraddr = HI_TRUE,
            .is_cache = HI_FALSE,
            .name = "VDP_SUSPEND_DDR",
        };

        memattr.size = vdp_get_reg_save_size();

        if (drv_xdp_mem_alloc(&memattr, &g_reg_save_buf) != HI_SUCCESS) {
            VDP_ERROR("malloc suspend ddr error\n");
            return HI_FAILURE;
        }
    }

    section_num = sizeof(g_reg_offset) / sizeof(hi_ulong) / OFFSET_ELE_NUM;

    for (idx = 0; idx < section_num; idx++) {
        memcpy((hi_void *)((hi_ulong)g_reg_save_buf.vir_addr + offset),
               (hi_void *)((hi_ulong)g_vdp_reg + g_reg_offset[idx][0]),
               g_reg_offset[idx][1]);
        offset += g_reg_offset[idx][1];
    }

    return HI_SUCCESS;
}

hi_s32 vdp_ip_reg_save_recover(hi_void)
{
    hi_u32 idx = 0;
    hi_u32 section_num;
    hi_ulong offset = 0;

    if (g_reg_save_buf.vir_addr == HI_NULL) {
        return HI_FAILURE;
    }

    section_num = sizeof(g_reg_offset) / sizeof(hi_ulong) / OFFSET_ELE_NUM;

    for (idx = 0; idx < section_num; idx++) {
        memcpy((hi_void *)((hi_ulong)g_vdp_reg + g_reg_offset[idx][0]),
               (hi_void *)((hi_ulong)g_reg_save_buf.vir_addr + offset),
               g_reg_offset[idx][1]);
        offset += g_reg_offset[idx][1];
    }

    drv_xdp_mem_free(&g_reg_save_buf);
    g_reg_save_buf.vir_addr = HI_NULL;

    return HI_SUCCESS;
}
