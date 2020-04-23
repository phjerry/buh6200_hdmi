/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_ip_master.h"
#include "hal_vdp_ip_dispchn.h"
#include "hal_vdp_comm.h"
#include "hal_vdp_reg_master.h"

static hi_u32 g_master_offset_addr[VDP_MASTER_LAYER_HARDEN_BUTT] = { MASTER0_OFFSET, MASTER1_OFFSET };

#define VDP_READ_OUT_STAND 48
#define VDP_WRITE_OUT_STAND 8
#define VDP_DEFAULT_QOS0 0x0
#define VDP_DEFAULT_QOS1 0x0

hi_void vdp_ip_master_update(vdp_dispchn_chn chn_id, hi_drv_disp_fmt fmt)
{
    hi_u32 lum_req;
    hi_u32 chm_req;
    hi_u32 gfx_req;
    hi_u32 vp0_master_sel;
    hi_u32 g3_master_sel;

    if (chn_id != VDP_DISPCHN_CHN_DHD0) {
        return;
    }

    if (fmt >= HI_DRV_DISP_FMT_7680X4320_100 &&
        fmt <= HI_DRV_DISP_FMT_7680X4320_120) {
        vp0_master_sel = VDP_MASTER_SEL_1;
        g3_master_sel = VDP_MASTER_SEL_1;
        lum_req = VDP_MASTER_BALENCE_1_1;
        chm_req = VDP_MASTER_BALENCE_1_1;
        gfx_req = VDP_MASTER_BALENCE_1_0;
    } else if (fmt  >= HI_DRV_DISP_FMT_7680X4320_23_976 &&
        fmt  <= HI_DRV_DISP_FMT_7680X4320_60) {
        vp0_master_sel = VDP_MASTER_SEL_0;
        g3_master_sel = VDP_MASTER_SEL_1;
        lum_req = VDP_MASTER_BALENCE_1_0;
        chm_req = VDP_MASTER_BALENCE_1_0;
        gfx_req = VDP_MASTER_BALENCE_1_0;
    } else { /* 4k ¨°??? */
        vp0_master_sel = VDP_MASTER_SEL_0;
        g3_master_sel = VDP_MASTER_SEL_0;
        lum_req = VDP_MASTER_BALENCE_1_0;
        chm_req = VDP_MASTER_BALENCE_1_0;
        gfx_req = VDP_MASTER_BALENCE_1_0;
    }

    vdp_master_setrchnsel(g_vdp_reg, 0, VDP_MASTER_RCHN_V0P, vp0_master_sel);
    vdp_master_setrchnsel(g_vdp_reg, 0, VDP_MASTER_RCHN_G3, g3_master_sel);
    vdp_master_vid_setreqbalance(g_vdp_reg, 0, lum_req, chm_req);
    vdp_master_gfx_setreqbalance(g_vdp_reg, 0, gfx_req);
}

hi_void vdp_ip_master_init(hi_void)
{
    hi_u32 i;
    vdp_master_layer_harden tmp_layer;
    hi_bool arb_mode = HI_FALSE; /* close */
    hi_u32 lum_req = VDP_MASTER_BALENCE_1_1;
    hi_u32 chm_req = VDP_MASTER_BALENCE_1_1;
    hi_u32 gfx_req = VDP_MASTER_BALENCE_1_0;

    if (vdp_ip_dispchn_get_enable(VDP_DISPCHN_CHN_DHD0) == HI_TRUE) {
        return;
    }

    for (tmp_layer = VDP_MASTER_LAYER_HARDEN0; tmp_layer < VDP_MASTER_LAYER_HARDEN_BUTT; tmp_layer++) {
        /* mul id */
        vdp_master_setmidenable(g_vdp_reg, g_master_offset_addr[tmp_layer], HI_TRUE);
        vdp_master_setarbmode(g_vdp_reg, g_master_offset_addr[tmp_layer], arb_mode);
        vdp_master_setsplitmode(g_vdp_reg, g_master_offset_addr[tmp_layer], VDP_AXI_EDGE_MODE_256);
        vdp_master_setwportsel(g_vdp_reg, g_master_offset_addr[tmp_layer], VDP_MASTER_SEL_1);
        vdp_master_setmstr0routstanding(g_vdp_reg, g_master_offset_addr[tmp_layer], VDP_READ_OUT_STAND);
        vdp_master_setmstr1routstanding(g_vdp_reg, g_master_offset_addr[tmp_layer], VDP_READ_OUT_STAND);
        vdp_master_setmstr2routstanding(g_vdp_reg, g_master_offset_addr[tmp_layer], VDP_READ_OUT_STAND);
        vdp_master_setmstr0woutstanding(g_vdp_reg, g_master_offset_addr[tmp_layer], VDP_WRITE_OUT_STAND);
        vdp_master_setmstr1woutstanding(g_vdp_reg, g_master_offset_addr[tmp_layer], VDP_WRITE_OUT_STAND);
        vdp_master_setmstr2woutstanding(g_vdp_reg, g_master_offset_addr[tmp_layer], VDP_WRITE_OUT_STAND);
        vdp_master_setqos(g_vdp_reg, g_master_offset_addr[tmp_layer], VDP_DEFAULT_QOS0, VDP_DEFAULT_QOS1);

        for (i = 0; i < VDP_MASTER_RCHN_BUTT; i++) {
            vdp_master_setrchnprio(g_vdp_reg, g_master_offset_addr[tmp_layer], i, 0);
        }

        for (i = 0; i < VDP_MASTER_WCHN_BUTT; i++) {
            vdp_master_setwchnprio(g_vdp_reg, g_master_offset_addr[tmp_layer], i, 0);
            vdp_master_setwchnsel(g_vdp_reg, g_master_offset_addr[tmp_layer], i, VDP_MASTER_SEL_1);
        }
    }

    vdp_master_setrchnsel(g_vdp_reg, 0, VDP_MASTER_RCHN_V0P, VDP_MASTER_SEL_1);
    vdp_master_setrchnsel(g_vdp_reg, 0, VDP_MASTER_RCHN_V1, VDP_MASTER_SEL_0); /* fix */
    vdp_master_setrchnsel(g_vdp_reg, 0, VDP_MASTER_RCHN_V3, VDP_MASTER_SEL_0); /* fix */

    vdp_master_setrchnsel(g_vdp_reg, 0, VDP_MASTER_RCHN_G0P, VDP_MASTER_SEL_1);
    vdp_master_setrchnsel(g_vdp_reg, 0, VDP_MASTER_RCHN_G0, VDP_MASTER_SEL_0); /* fix */
    vdp_master_setrchnsel(g_vdp_reg, 0, VDP_MASTER_RCHN_G1, VDP_MASTER_SEL_1); /* fix */
    vdp_master_setrchnsel(g_vdp_reg, 0, VDP_MASTER_RCHN_G3, VDP_MASTER_SEL_1);

    vdp_master_setrchnsel(g_vdp_reg, 0, VDP_MASTER_RCHN_PARA_CORE, VDP_MASTER_SEL_0); /* fix */
    vdp_master_setrchnsel(g_vdp_reg, 0, VDP_MASTER_RCHN_PARA_SUB, VDP_MASTER_SEL_0); /* fix */

    vdp_master_vid_setreqbalance(g_vdp_reg, 0, lum_req, chm_req);
    vdp_master_gfx_setreqbalance(g_vdp_reg, 0, gfx_req);

    vdp_master_setqosmode(g_vdp_reg, MASTER0_OFFSET, 1); /* use out qos */
    vdp_master_setqosmode(g_vdp_reg, MASTER1_OFFSET, 1); /* use out qos */

    return;
}

hi_s32 vdp_ip_master_get_error(hi_void)
{
    u_vdp_core_mst_bus_err vdp_core_mst_bus_err;
    u_mst_bus_err mst_bus_err;

    vdp_core_mst_bus_err.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->vdp_core_mst_bus_err.u32)));
    mst_bus_err.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->mst_bus_err.u32)));

    if (((vdp_core_mst_bus_err.u32 & 0x3F) != 0) || ((mst_bus_err.u32 & 0x3F) != 0)) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void vdp_ip_master_clean_error(hi_void)
{
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->vdp_core_mst_bus_err_clr.u32)), 0x1);
    vdp_regwrite((uintptr_t)(&(g_vdp_reg->mst_bus_err_clr.u32)), 0x1);
}

