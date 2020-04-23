/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: function define
* Author: image
* Create: 2019-04-12
 */

#include "hal_disp_intf.h"
#ifdef __VDP_HAL_TEST_SUPPORT__
#include <linux/module.h>
#include "hal_vdp_vid.h"
#endif

#include "hal_vdp_comm.h"
#include "hal_vdp_ip_para.h"
#include "hal_vdp_ip_dispchn.h"
#include "hal_vdp_ip_csc.h"
#include "hal_vdp_ip_crg.h"
#include "hal_vdp_ip_mmu.h"
#include "hal_vdp_chip_config.h"
#include "hal_vdp_ip_master.h"
#include "hal_vdp_ip_fdr.h"
#include "hal_vdp_ip_mdata.h"
#ifndef CBB_OSAL_TYPE_SUPPORT
#include "hal_vdp_reg_save.h"
#endif
#include "hal_vdp_vmin.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define VDP_SMMU_ENABLE_STATE 1

#define VDP_VTHD_PERCENT_DHD0_THD1 50
#define VDP_VTHD_PERCENT_DHD1_THD1 60
#define VDP_VTHD_PERCENT_REF       100
#define DHD_VTXTHD_FIELD_MODE      1
#define DHD_VTXTHD_FRAME_MODE      0
#define DHD_VTHD_ID_1              1
#define DHD_VTHD_ID_2              2
#define DHD_VTHD_ID_3              3

#define HAL_TIME_2_MS 2
#define HAL_TIME_4_MS 4
#define HAL_TIME_7_MS 7
#define HAL_PERCENT_15 15
#define HAL_PERCENT_90 90
#define HAL_PERCENT_100 100
#define HAL_REFRESH_RATE_UNIT 100000
hi_void hal_layer_clean_flag_for_suspend(hi_void);

vdp_dispchn_chn disp_get_chn_id(hi_drv_display chn)
{
    switch (chn) {
        case HI_DRV_DISPLAY_0:
            return VDP_DISPCHN_CHN_DHD0;
        case HI_DRV_DISPLAY_1:
            return VDP_DISPCHN_CHN_DHD1;
        default:
            return VDP_DISPCHN_CHN_DHD0;
    }
}

/* vdp */
hi_s32 hal_vdp_init(hi_void)
{
    hi_s32 ret;

    /* map reg */
    ret = vdp_reg_map();
    if (ret != HI_SUCCESS) {
        return ret;
    }
    /*set vmin */
    hal_set_vdp_vmin();
    /* open vdp clk */
    vdp_ip_crg_work_clk_enable(HI_TRUE);

    /* init capacbility */
    vdp_chip_capaciblity_init();

    /* malloc buf for ip coef */
    vdp_ip_mmu_set(VDP_SMMU_ENABLE_STATE);
    vdp_ip_para_init();
    vdp_ip_dispchn_init();
    vdp_ip_master_init();
    vdp_ip_mdata_init();

#ifdef VDP_SUPPORT_PANEL
    /* tmp add for panel */
    vdp_ip_dispchn_attach_intf(VDP_DISPCHN_CHN_DHD0, VDP_DISP_INTF_VBO);
    vdp_ip_crg_attach_intf(VDP_DISPCHN_CHN_DHD0, VDP_DISP_INTF_VBO);
#endif

    return HI_SUCCESS;
}

hi_s32 hal_vdp_deinit(hi_void)
{
    vdp_ip_para_deinit();
    vdp_ip_mdata_deinit();
    vdp_reg_unmap();
    return HI_SUCCESS;
}

hi_s32 hal_vdp_resume(hi_void)
{
    hi_s32 ret;
    vdp_ip_crg_work_clk_enable(HI_TRUE);
#ifndef CBB_OSAL_TYPE_SUPPORT
    ret = vdp_ip_reg_save_recover();
    if (ret != HI_SUCCESS) {
        VDP_PRINT("vdp reg recover error!\n");
        return ret;
    }
#endif
    return ret;
}

hi_s32 hal_vdp_suspend(hi_void)
{
    hi_s32 ret;
#ifndef CBB_OSAL_TYPE_SUPPORT
    ret = vdp_ip_reg_save_record();
    if (ret != HI_SUCCESS) {
        VDP_PRINT("vdp reg record error!\n");
        return ret;
    }
#endif
    hal_layer_clean_flag_for_suspend();
    vdp_ip_crg_work_clk_enable(HI_FALSE);
    return HI_SUCCESS;
}

/* 注意时钟配置是及时生效的，配置的是crg寄存器 而非vdp寄存器 */
hi_s32 hal_vdp_set_clock_enable(clock_module module, hi_bool enable)
{
    return HI_SUCCESS;
}

hi_s32 hal_vdp_reg_check(hi_void)
{
    return HI_SUCCESS;
}

vdp_capacity *hal_vdp_get_capacity(void)
{
    return hal_vdp_get_chip_capacity();
}

/* disp */
hi_s32 hal_disp_set_bgcolor(hi_drv_display chn, hi_drv_color_descript cs, hi_drv_disp_color *bgcolor)
{
    vdp_bkg vdp_bkg = { 0 };
    vdp_dispchn_chn chn_id = disp_get_chn_id(chn);

    vdp_ip_csc_data_transfer(bgcolor, cs, &vdp_bkg);

    if (chn_id == VDP_DISPCHN_CHN_DHD0) {
        vdp_ip_dispchn_set_cbm_bkg(VDP_DISPCHN_CBM_MIX0, &vdp_bkg);
    } else {
        vdp_ip_dispchn_set_cbm_bkg(VDP_DISPCHN_CBM_MIX1, &vdp_bkg);
    }
    vdp_ip_dispchn_regup(chn_id);
    return HI_SUCCESS;
}

hi_void hal_disp_chn_mask(hi_drv_display chn, hi_bool mask_enale)
{
    /* only support for dhd0 mask */
    vdp_ip_dispchn_dhd0_mask_enalbe(mask_enale);
    return;
}

hi_s32 hal_disp_set_enable(hi_drv_display chn, hi_bool enable)
{
    vdp_ip_dispchn_set_enable(disp_get_chn_id(chn), enable);
    return HI_SUCCESS;
}

hi_s32 hal_disp_get_enable(hi_drv_display chn, hi_bool *enable)
{
    *enable = vdp_ip_dispchn_get_enable(disp_get_chn_id(chn));
    return HI_SUCCESS;
}

/* rwzb */
hi_s32 hal_disp_set_rwzb_info(hi_drv_display chn, hi_u32 stream_index)
{
    return HI_SUCCESS;
}

/* capacity */
disp_capacity *hal_disp_get_capacity(hi_drv_display chn)
{
    return hal_disp_get_chip_capacity(chn);
}

vdp_disp_mode transfer_disp_mode(hi_drv_disp_stereo_mode disp_3d_mode)
{
    vdp_disp_mode disp_mode;
    switch (disp_3d_mode) {
        case HI_DRV_DISP_STEREO_NONE:
            disp_mode = VDP_DISP_MODE_2D;
            break;
        case HI_DRV_DISP_STEREO_SBS_HALF:
            disp_mode = VDP_DISP_MODE_SBS;
            break;
        case HI_DRV_DISP_STEREO_LINE_ALT:
            disp_mode = VDP_DISP_MODE_2D;
            break;
        case HI_DRV_DISP_STEREO_FRAME_PACKING:
            disp_mode = VDP_DISP_MODE_FS;
            break;
        case HI_DRV_DISP_STEREO_TAB:
            disp_mode = VDP_DISP_MODE_TAB;
            break;
        default:
            disp_mode = VDP_DISP_MODE_2D;
            break;
    }

    return disp_mode;
}

vdp_intf_split_mode get_disp_split_mode(disp_timing_info *timing, hi_drv_disp_intf *intf_attr)
{
#ifdef VDP_SUPPORT_PANEL
    return VDP_SPLIT_MODE_8P_2SPLIT;
#endif

    if ((intf_attr->intf_type == HI_DRV_DISP_INTF_TYPE_HDMI)
        && (intf_attr->un_intf.hdmi == HI_DRV_HDMI_ID_0)) {
        return VDP_SPLIT_MODE_8P_1SPLIT;
    }

    if (intf_attr->intf_type == HI_DRV_DISP_INTF_TYPE_MIPI) {
        return VDP_SPLIT_MODE_2P_2SPLIT;
    }

    return VDP_SPLIT_MODE_1P_1SPLIT;
}

vdp_dispchn_multichn get_disp_multichn(vdp_intf_split_mode split)
{
    vdp_dispchn_multichn multi;
    switch(split) {
        case VDP_SPLIT_MODE_1P_1SPLIT:
            multi = VDP_DISPCHN_MULTICHN_1_PIXEL;
            break;
        case VDP_SPLIT_MODE_2P_1SPLIT:
        case VDP_SPLIT_MODE_2P_2SPLIT:
        case VDP_SPLIT_MODE_2P_ODDEVEN:
            multi = VDP_DISPCHN_MULTICHN_2_PIXEL;
            break;
        case VDP_SPLIT_MODE_4P_1SPLIT:
        case VDP_SPLIT_MODE_4P_2SPLIT:
        case VDP_SPLIT_MODE_4P_4SPLIT:
            multi = VDP_DISPCHN_MULTICHN_4_PIXEL;
            break;
        case VDP_SPLIT_MODE_8P_1SPLIT:
        case VDP_SPLIT_MODE_8P_2SPLIT:
        case VDP_SPLIT_MODE_8P_4SPLIT:
        case VDP_SPLIT_MODE_8P_8SPLIT:
            multi = VDP_DISPCHN_MULTICHN_8_PIXEL;
            break;
        default:
            multi = VDP_DISPCHN_MULTICHN_1_PIXEL;
            break;
    }
    return multi;
}

hi_u32 disp_gfx_get_isr_thd2(hi_drv_display chn, disp_timing_info *timing)
{
    hi_u32 period_time;
    hi_u32 total_lines;
    hi_u32 thresh_hold_line;

    /* Generate time cost for every period in MicroSeconds. */
    period_time = HAL_REFRESH_RATE_UNIT / timing->refresh_rate;

    if (timing->disp_3d_mode == HI_DRV_DISP_STEREO_FRAME_PACKING) {
        total_lines = (timing->sync_info.vact +
                      timing->sync_info.vbb +
                      timing->sync_info.vfb +
                      timing->sync_info.bvact +
                      timing->sync_info.bvbb +
                      timing->sync_info.bvfb);
    } else {
        total_lines = (timing->sync_info.vact + timing->sync_info.vbb + timing->sync_info.vfb);
    }

    /* hd channel */
    if (chn == 0) {
        /* we set the theshhold for 1.5ms, so we can get the u32ThreshHoldLine. */
        thresh_hold_line = (HAL_TIME_2_MS * total_lines) / (period_time);
    } else {
        /* we set the theshhold for 1.5ms, so we can get the u32ThreshHoldLine. */
        thresh_hold_line = (HAL_TIME_7_MS * total_lines) / (HAL_TIME_4_MS * period_time);
    }

    /* To avoid the isr configure missing the update point in vbb in next ISR period,
     * we should consider this.
     */
    thresh_hold_line -= timing->sync_info.vfb;

    /* so the final isr point is as follows */
    if ((hi_s32)(total_lines - thresh_hold_line) < 0) {
        return ((total_lines * HAL_PERCENT_90) / HAL_PERCENT_100);
    }

    return (total_lines - thresh_hold_line);
}

hi_u32 disp_gfx_get_isr_thd3(hi_drv_display chn, disp_timing_info *timing)
{
    hi_u32 total_lines;

    if (timing->disp_3d_mode == HI_DRV_DISP_STEREO_FRAME_PACKING) {
        total_lines = (timing->sync_info.vact +
                      timing->sync_info.vbb +
                      timing->sync_info.vfb +
                      timing->sync_info.bvact +
                      timing->sync_info.bvbb +
                      timing->sync_info.bvfb);
    } else {
        total_lines = (timing->sync_info.vact + timing->sync_info.vbb + timing->sync_info.vfb);
    }

    return ((total_lines * HAL_PERCENT_15) / HAL_PERCENT_100);
}

hi_void disp_set_intf_thd(hi_drv_display chn, disp_timing_info *timing)
{
    hi_u32 thd1;
    hi_u32 thd2;
    hi_u32 thd3;
    hi_u32 vtotal;
    hi_u32 thd_mode = (timing->disp_3d_mode == HI_DRV_DISP_STEREO_FRAME_PACKING) ?
        DHD_VTXTHD_FIELD_MODE : DHD_VTXTHD_FRAME_MODE;

    vtotal = timing->sync_info.vact + timing->sync_info.vbb + timing->sync_info.vfb;

    if ((timing->refresh_rate == 0) ||
        (timing->refresh_rate > HAL_REFRESH_RATE_UNIT)) {
        VDP_PRINT("refresh_rate(%d) is error!\n", timing->refresh_rate);
        return;
    }

    if (chn == HI_DRV_DISPLAY_0) {
        thd1 = vtotal * VDP_VTHD_PERCENT_DHD0_THD1 / VDP_VTHD_PERCENT_REF;
    } else {
        thd1 = vtotal * VDP_VTHD_PERCENT_DHD1_THD1 / VDP_VTHD_PERCENT_REF;
    }

    thd2 = disp_gfx_get_isr_thd2(chn, timing);
    thd3 = disp_gfx_get_isr_thd3(chn, timing);

    vdp_ip_dispchn_set_vtthd(disp_get_chn_id(chn), DHD_VTHD_ID_1, thd1, thd_mode);
    vdp_ip_dispchn_set_vtthd(disp_get_chn_id(chn), DHD_VTHD_ID_2, thd2, thd_mode);
    vdp_ip_dispchn_set_vtthd(disp_get_chn_id(chn), DHD_VTHD_ID_3, thd3, thd_mode);
}

/* format */
hi_s32 hal_disp_set_timing(hi_drv_display chn,
                           hi_drv_disp_intf *intf_attr,
                           disp_timing_info *timing)
{
    vdp_dispchn_info dispchn_info;
    vdp_disp_crg crg_info;

    dispchn_info.split_mode = get_disp_split_mode(timing, intf_attr);
    dispchn_info.disp_mode = transfer_disp_mode(timing->disp_3d_mode);
    dispchn_info.timing.iop = timing->sync_info.iop;
    dispchn_info.timing.hmid = timing->sync_info.hmid;
    dispchn_info.disp_fmt = timing->disp_fmt;

    dispchn_info.timing.vact = timing->sync_info.vact;
    dispchn_info.timing.vfb = timing->sync_info.vfb;
    dispchn_info.timing.vbb = timing->sync_info.vbb;
    dispchn_info.timing.hact = timing->sync_info.hact;
    dispchn_info.timing.hfb = timing->sync_info.hfb;
    dispchn_info.timing.hbb = timing->sync_info.hbb;
    dispchn_info.timing.bvact = timing->sync_info.bvact;
    dispchn_info.timing.bvfb = timing->sync_info.bvfb;
    dispchn_info.timing.bvbb = timing->sync_info.bvbb;
    dispchn_info.timing.hpw = timing->sync_info.hpw;
    dispchn_info.timing.vpw = timing->sync_info.vpw;
    dispchn_info.timing.idv = timing->sync_info.idv;
    dispchn_info.timing.ihs = timing->sync_info.ihs;
    dispchn_info.timing.ivs = timing->sync_info.ivs;

    /* set timing */
    vdp_ip_dispchn_set_timming(disp_get_chn_id(chn), &dispchn_info);

    /* set thd */
    disp_set_intf_thd(chn, timing);

    /* set chn clk */
    crg_info.disp_fmt = timing->disp_fmt;
    crg_info.pixel_clk = timing->pixel_clk;
    crg_info.multichn_en = get_disp_multichn(dispchn_info.split_mode);
    vdp_ip_crg_set_chn_clk(disp_get_chn_id(chn), intf_attr, &crg_info);

    /* update master */
    vdp_ip_master_update(disp_get_chn_id(chn), timing->disp_fmt);
    vdp_ip_dispchn_regup(disp_get_chn_id(chn));

    return HI_SUCCESS;
}

hi_s32 hal_disp_set_auto_sync(hi_drv_display chn, hi_bool enable)
{
    return HI_SUCCESS;
}

/* interrupt */
hi_void hal_disp_set_interrupt_enable(hi_u32 int_num, hi_bool enable)
{
    if (enable == HI_TRUE) {
        vdp_ip_dispchn_set_mask(VDP_DISPCHN_INT_ON, int_num);
    } else {
        vdp_ip_dispchn_set_mask(VDP_DISPCHN_INT_OFF, int_num);
    }

    return;
}

hi_void hal_disp_get_interrupt_state(int_state_type type, hi_u32 *state)
{
    if (type == INT_STATE_TYPE_INTF_UNMASK) {
        *state = vdp_ip_dispchn_get_int_state();
    } else if (type == INT_STATE_TYPE_INTF_MASK) {
        *state = vdp_ip_dispchn_get_int_mask_state();
    } else if ((type == INT_STATE_TYPE_SMMU_MASK) ||
        (type == INT_STATE_TYPE_SMMU_UNMASK)) {
        *state = vdp_ip_mmu_get_interrupt_state();
    }

    return;
}

hi_void hal_disp_clean_interrupt_state(int_state_type type, hi_u32 state)
{
    if ((type == INT_STATE_TYPE_INTF_MASK) ||
        (type == INT_STATE_TYPE_INTF_UNMASK)) {
        vdp_ip_dispchn_set_mask(VDP_DISPCHN_INT_CLEAN, state);
    } else if ((type == INT_STATE_TYPE_SMMU_MASK) ||
        (type == INT_STATE_TYPE_SMMU_UNMASK)) {
        vdp_ip_mmu_clean_interrupt_state(state);
    }

    return;
}

hi_s32 hal_disp_get_mmu_error_addr_ns(hi_void)
{
    return vdp_ip_mmu_get_error_addr_ns();
}

hi_s32 hal_disp_get_dcmp_state(hi_drv_display disp_id)
{
    return vdp_ip_fdr_get_dcmp_error(disp_get_chn_id(disp_id));
}

hi_void hal_disp_clean_dcmp_state(hi_drv_display disp_id)
{
    vdp_ip_fdr_clean_dcmp_error(disp_get_chn_id(disp_id));
}

hi_s32 hal_disp_get_master_state(hi_void)
{
    return vdp_ip_master_get_error();
}

hi_void hal_disp_clean_master_state(hi_void)
{
    vdp_ip_master_clean_error();
}

/* state */
hi_void hal_disp_get_state(hi_drv_display chn, hi_bool *btm, hi_u32 *vcnt, hi_u32 *int_cnt)
{
    vdp_ip_dispchn_get_state(disp_get_chn_id(chn), btm, vcnt, int_cnt);
    return;
}

hi_s32 hal_disp_check_gfx_state(hi_u32 *total_pixel, hi_u32 *zero_pixel)
{
    vdp_ip_dispchn_get_gfx_state(total_pixel, zero_pixel);
    return HI_SUCCESS;
}

/* debug */
hi_void hal_disp_debug_get_smmu_error_addr(smmu_error_addr *error_addr)
{
    return;
}

hi_void hal_disp_debug_colorbar(hi_drv_display chn, hi_bool colorbar_enable)
{
    vdp_ip_dispchn_set_colobar(disp_get_chn_id(chn), colorbar_enable);
    vdp_ip_dispchn_regup(disp_get_chn_id(chn));
    return;
}

hi_void hal_disp_debug_get_checksum(hi_drv_display chn, hi_u32 *r, hi_u32 *g, hi_u32 *b)
{
    vdp_ip_dispchn_get_checksum(disp_get_chn_id(chn), r, g, b);
    return;
}

#define INK_DELAY_TIME_MS (50 * 3)
/* INK需要等待2个中断时间，1个配置生效 1个逻辑取值，不能在中断调用 */
hi_void hal_disp_debug_ink(disp_ink_attr *attr, disp_color *rgb_date)
{
    vdp_csc_id csc_id;
    vdp_csc_ink_info ink_info;
    vdp_bkg bkg;

    ink_info.ink_en = attr->ink_enable;
    ink_info.data_fmt = 0;
    ink_info.cross_en = attr->cross_display_enable;
    ink_info.color_mode = attr->ink_color_mode;
    ink_info.x = attr->x_pos;
    ink_info.y = attr->y_pos;

    switch (attr->ink_pos) {
        case INK_POS_V0_CSC_F:
            ink_info.ink_sel = 0;
            csc_id = VDP_CSC_ID_VID0;
            break;
        case INK_POS_V0_CSC_B:
            ink_info.ink_sel = 1;
            csc_id = VDP_CSC_ID_VID0;
            break;
        case INK_POS_V1_CSC_F:
            ink_info.ink_sel = 0;
            csc_id = VDP_CSC_ID_VID1;
            break;
        case INK_POS_V1_CSC_B:
            ink_info.ink_sel = 1;
            csc_id = VDP_CSC_ID_VID1;
            break;
        case INK_POS_V3_CSC_F:
            ink_info.ink_sel = 0;
            csc_id = VDP_CSC_ID_VID3;
            break;
        case INK_POS_V3_CSC_B:
            ink_info.ink_sel = 1;
            csc_id = VDP_CSC_ID_VID3;
            break;
        default:
            return;
    }

    vdp_ip_csc_set_ink(csc_id, &ink_info);
    osal_msleep_uninterruptible(INK_DELAY_TIME_MS);
    vdp_ip_csc_get_ink(csc_id, &bkg);
    rgb_date->red = bkg.y;
    rgb_date->greern = bkg.u;
    rgb_date->blue = bkg.v;

    return;
}

/* intf */
hi_s32 hal_disp_attach_intf(hi_drv_display chn, disp_attach_intf_info *intf)
{
    vdp_dispchn_chn chn_id = disp_get_chn_id(chn);

#ifdef VDP_SUPPORT_PANEL
    vdp_ip_dispchn_attach_intf(VDP_DISPCHN_CHN_DHD0, VDP_DISP_INTF_VBO);
    vdp_ip_crg_attach_intf(VDP_DISPCHN_CHN_DHD0, VDP_DISP_INTF_VBO);
    return HI_SUCCESS;
#endif

    if (intf->intf_attr.intf_type == HI_DRV_DISP_INTF_TYPE_HDMI) {
        if (intf->intf_attr.un_intf.hdmi == HI_DRV_HDMI_ID_1) {
            vdp_ip_dispchn_attach_intf(chn_id, VDP_DISP_INTF_HDMI1); /* hal hdmi1 默认 2.0 */
            vdp_ip_crg_attach_intf(chn_id, VDP_DISP_INTF_HDMI1);
        } else if (intf->intf_attr.un_intf.hdmi == HI_DRV_HDMI_ID_0) {
            vdp_ip_dispchn_attach_intf(chn_id, VDP_DISP_INTF_HDMI); /* hal hdmi0 默认 2.1 */
            vdp_ip_crg_attach_intf(chn_id, VDP_DISP_INTF_HDMI);
        }
    } else if (intf->intf_attr.intf_type == HI_DRV_DISP_INTF_TYPE_CVBS) {
        vdp_ip_dispchn_attach_intf(chn_id, VDP_DISP_INTF_CVBS);
        vdp_ip_crg_attach_intf(chn_id, VDP_DISP_INTF_CVBS);
    } else if (intf->intf_attr.intf_type == HI_DRV_DISP_INTF_TYPE_MIPI) {
        vdp_ip_dispchn_attach_intf(chn_id, VDP_DISP_INTF_MIPI);
        vdp_ip_crg_attach_intf(chn_id, VDP_DISP_INTF_MIPI);
    }

    return HI_SUCCESS;
}

hi_s32 hal_disp_detach_intf(hi_drv_display chn, disp_attach_intf_info *intf)
{
    return HI_SUCCESS;
}

hi_s32 hal_disp_set_intf_format(hi_drv_display chn, disp_attach_intf_info *intf,
                                disp_timing_info *timing_info)
{
    vdp_disp_syncinv syncinv = {0};
    vdp_dispchn_chn chn_id = disp_get_chn_id(chn);

#ifdef VDP_SUPPORT_PANEL
    return HI_SUCCESS;
#endif

    if (intf->intf_attr.intf_type == HI_DRV_DISP_INTF_TYPE_CVBS) {
        syncinv.hs_inv = 1;
        vdp_ip_dispchn_sync_inv(VDP_DISP_INTF_CVBS, &syncinv);
        vdp_ip_dispchn_date_coef(VDP_DISP_INTF_CVBS, timing_info->disp_fmt);
        vdp_ip_dispchn_set_intf(chn_id, VDP_DISP_INTF_CVBS);
    } else if (intf->intf_attr.intf_type == HI_DRV_DISP_INTF_TYPE_HDMI) {
        vdp_ip_dispchn_date_coef(VDP_DISP_INTF_HDMI, timing_info->disp_fmt);
        vdp_ip_dispchn_set_intf(chn_id, VDP_DISP_INTF_HDMI);
    }

    return HI_SUCCESS;
}

hi_s32 hal_disp_set_intf_enable(hi_drv_display chn, disp_attach_intf_info *intf, hi_bool enable)
{
    vdp_ip_dispchn_regup(disp_get_chn_id(chn));

    return HI_SUCCESS;
}
hi_void hal_disp_send_infoframe(hal_hdmi_infoframe *hdmi_infoframe)
{
    vdp_mdata_cfg cfg;
    cfg.enable = hdmi_infoframe->enable;
    cfg.avi_enable = hdmi_infoframe->avi_enable;
    cfg.smd_enable = hdmi_infoframe->smd_enable;
    cfg.addr = hdmi_infoframe->addr;
    cfg.data_len = hdmi_infoframe->data_len;
    vdp_ip_mdata_set_cfg(&cfg);
    return;
}

/* o5 */
hi_s32 hal_disp_set_dac_enable(hi_bool enable)
{
    return HI_SUCCESS;
}

#ifdef __VDP_HAL_TEST_SUPPORT__
static long vdp_hal_ioctl(struct file *file_ptr, hi_u32 cmd, hi_ulong u_l_arg)
{
    return HI_SUCCESS;
}
static hi_s32 vdp_hal_open(struct inode *inode, struct file *file_ptr)
{
    return HI_SUCCESS;
}

static hi_s32 vdp_hal_release(struct inode *inode, struct file *file_ptr)
{
    return HI_SUCCESS;
}

static ssize_t vdp_hal_read(struct file *file_ptr, hi_char __user *buf, size_t count, loff_t *offset)
{
    return count;
}

static struct file_operations g_vdp_hal_fop = {
owner: THIS_MODULE,
open: vdp_hal_open,
read: vdp_hal_read,
unlocked_ioctl: vdp_hal_ioctl,
release: vdp_hal_release,
};

static struct miscdevice g_xdp_dev = {
    MISC_DYNAMIC_MINOR,
    "hi_vou",
    &g_vdp_hal_fop,
};

static hi_s32 __init hal_vdp_module_init(hi_void)
{
    hi_s32 ret = misc_register(&g_xdp_dev);
    if (ret != HI_SUCCESS) {
        VDP_PRINT("Register vdp hal DEV Fail!\n");
        return ret;
    }
    ret = hal_vdp_init();
    if (ret != HI_SUCCESS) {
        return ret;
    }
    VDP_PRINT("Register vdp hal dev success.\n");
    return HI_SUCCESS;
}

static hi_void __exit hal_vdp_module_exit(hi_void)
{
    hal_vdp_deinit();

    misc_deregister(&g_xdp_dev);

    VDP_PRINT("UnRegister vdp hal dev success.\n");

    return;
}

module_init(hal_vdp_module_init);
module_exit(hal_vdp_module_exit);
MODULE_LICENSE("GPL");
#endif

#if defined(VDP_SDK_HAL_TEST_SUPPORT) || defined(__VDP_HAL_TEST_SUPPORT__)
EXPORT_SYMBOL(hal_vdp_init);
EXPORT_SYMBOL(hal_vdp_deinit);
EXPORT_SYMBOL(hal_vdp_resume);
EXPORT_SYMBOL(hal_vdp_suspend);
EXPORT_SYMBOL(hal_vdp_set_clock_enable);
EXPORT_SYMBOL(hal_vdp_reg_check);
EXPORT_SYMBOL(hal_vdp_get_capacity);
EXPORT_SYMBOL(hal_disp_set_bgcolor);
EXPORT_SYMBOL(hal_disp_chn_mask);
EXPORT_SYMBOL(hal_disp_set_enable);
EXPORT_SYMBOL(hal_disp_get_enable);
EXPORT_SYMBOL(hal_disp_set_rwzb_info);
EXPORT_SYMBOL(hal_disp_get_capacity);
EXPORT_SYMBOL(hal_disp_set_timing);
EXPORT_SYMBOL(hal_disp_set_auto_sync);
EXPORT_SYMBOL(hal_disp_set_interrupt_enable);
EXPORT_SYMBOL(hal_disp_get_interrupt_state);
EXPORT_SYMBOL(hal_disp_clean_interrupt_state);
EXPORT_SYMBOL(hal_disp_get_dcmp_state);
EXPORT_SYMBOL(hal_disp_clean_dcmp_state);
EXPORT_SYMBOL(hal_disp_get_master_state);
EXPORT_SYMBOL(hal_disp_clean_master_state);
EXPORT_SYMBOL(hal_disp_get_state);
EXPORT_SYMBOL(hal_disp_check_gfx_state);
EXPORT_SYMBOL(hal_disp_debug_get_smmu_error_addr);
EXPORT_SYMBOL(hal_disp_debug_colorbar);
EXPORT_SYMBOL(hal_disp_debug_get_checksum);
EXPORT_SYMBOL(hal_disp_debug_ink);
EXPORT_SYMBOL(hal_disp_attach_intf);
EXPORT_SYMBOL(hal_disp_detach_intf);
EXPORT_SYMBOL(hal_disp_set_intf_format);
EXPORT_SYMBOL(hal_disp_set_intf_enable);
EXPORT_SYMBOL(hal_disp_set_dac_enable);

EXPORT_SYMBOL(hal_layer_get_capacity);
EXPORT_SYMBOL(hal_layer_cfg_disable);
EXPORT_SYMBOL(hal_layer_cfg_hisi_process);
EXPORT_SYMBOL(hal_layer_cfg_dolby_process);
EXPORT_SYMBOL(hal_layer_set_zorder);
EXPORT_SYMBOL(hal_layer_get_zorder);
EXPORT_SYMBOL(hal_layer_get_water_mark_state);
EXPORT_SYMBOL(hal_layer_debug_get_logic_info);
EXPORT_SYMBOL(hal_layer_debug_pattern);
EXPORT_SYMBOL(hal_layer_debug_get_checksum);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

