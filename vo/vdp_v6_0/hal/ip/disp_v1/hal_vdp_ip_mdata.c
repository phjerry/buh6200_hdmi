/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_ip_mdata.h"
#include "hal_vdp_ip_dispchn.h"
#include "hal_vdp_reg_chn.h"
#include "hal_vdp_comm.h"
#include "hal_vdp_reg_mdata.h"
#include "drv_xdp_ion.h"
#include "linux/hisilicon/securec.h"

#define VDP_MTADATA_BUFF_SIZE   0x1000
#define VDP_MTADATA_BYTE_MAX    16
#define VDP_MTADATA_START_POINT 8
#define HIGH_ADDR_OFFSET        32
#define SEND_DATA_ADD           2

typedef struct {
    hi_u32 u32size;
    hi_u8 *vir_addr;
    hi_u64 phy_addr;
} vdp_matadata_addr;

drv_xdp_mem_info g_mem_addr = { 0 };
vdp_matadata_addr g_vdp_matadata_addr;

hi_void vdp_ip_mdata_init(hi_void)
{
    if (g_mem_addr.vir_addr == HI_NULL) {
        drv_xdp_mem_attr memattr = {
            .mode = OSAL_MMZ_TYPE,
            .is_map_viraddr = HI_TRUE,
            .is_cache = HI_FALSE,
            .size = VDP_MTADATA_BUFF_SIZE,
            .name = "VDP_BUFF_MATADATA",
        };

        if (drv_xdp_mem_alloc(&memattr, &g_mem_addr) != HI_SUCCESS) {
            return;
        }
        g_vdp_matadata_addr.vir_addr = g_mem_addr.vir_addr;
        g_vdp_matadata_addr.phy_addr = g_mem_addr.phy_addr;

    }
    return;
}

hi_void vdp_ip_mdata_deinit(hi_void)
{
    if (g_mem_addr.vir_addr != HI_NULL) {
        drv_xdp_mem_free(&g_mem_addr);
        g_mem_addr.vir_addr = HI_NULL;
    }
    return;
}

hi_void vdp_ip_mdata_set_cfg(vdp_mdata_cfg *cfg)
{
    if (cfg->enable == HI_FALSE) {
        vdp_mdata_set_regup(g_vdp_reg, HI_TRUE);
        vdp_mdata_set_chn_en(g_vdp_reg, HI_FALSE);
        vdp_mdata_set_avi_valid(g_vdp_reg, HI_FALSE);
        vdp_mdata_set_smd_valid(g_vdp_reg, HI_FALSE);
        vdp_ip_dispchn_regup(VDP_DISPCHN_CHN_DHD0);
        return;
    }

    memcpy_s((hi_void *)g_vdp_matadata_addr.vir_addr, cfg->data_len, (hi_void *)cfg->addr, cfg->data_len);

    vdp_mdata_set_chn_en(g_vdp_reg, cfg->enable);
    vdp_mdata_set_nosec_flag(g_vdp_reg, HI_TRUE);
    vdp_mdata_set_mmu_bypass(g_vdp_reg, HI_TRUE);
    vdp_mdata_set_avi_valid(g_vdp_reg, cfg->avi_enable);
    vdp_mdata_set_smd_valid(g_vdp_reg, cfg->smd_enable);
    vdp_disp_setprestartpos(g_vdp_reg, VDP_CHN_DHD0, 0);
    vdp_disp_setprestartlinepos(g_vdp_reg, VDP_CHN_DHD0, VDP_MTADATA_START_POINT);
    vdp_mdata_set_sreq_delay(g_vdp_reg, 1);
    vdp_mdata_set_maddr_h(g_vdp_reg, (g_vdp_matadata_addr.phy_addr >> HIGH_ADDR_OFFSET));
    vdp_mdata_set_maddr_l(g_vdp_reg, g_vdp_matadata_addr.phy_addr);
    vdp_mdata_set_mburst(g_vdp_reg, (cfg->data_len / VDP_MTADATA_BYTE_MAX));
    vdp_mdata_set_regup(g_vdp_reg, HI_TRUE);
    vdp_ip_dispchn_regup(VDP_DISPCHN_CHN_DHD0);
    return;
}


