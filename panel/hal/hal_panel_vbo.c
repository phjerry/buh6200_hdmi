/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel hal vbone module
* Author: sdk
* Create: 2019-11-21
*/

#ifndef __HAL_PANEL_VBO_H__
#define __HAL_PANEL_VBO_H__

#include "hi_type.h"
#include "hal_panel_comm.h"
#include "hal_vbo_reg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void hal_panel_set_vbo_enable(hi_bool enable)
{
    u_vbo_ctrl0 vbo_ctrl0;

    vbo_ctrl0.u32 = panel_reg_read((&(panel_vbotx_reg()->vbo_ctrl0.u32)));
    vbo_ctrl0.bits.vbo_en = enable;
    panel_reg_write((&(panel_vbotx_reg()->vbo_ctrl0.u32)), vbo_ctrl0.u32);
}

hi_void hal_panel_get_vbo_enable(hi_bool *enable)
{
    u_vbo_ctrl0 vbo_ctrl0;

    vbo_ctrl0.u32 = panel_reg_read((&(panel_vbotx_reg()->vbo_ctrl0.u32)));
    *enable = vbo_ctrl0.bits.vbo_en;
}

hi_void hal_panel_set_vbo_data_mode(panel_vbo_data_mode data_mode)
{
    u_vbo_ctrl0 vbo_ctrl0;

    vbo_ctrl0.u32 = panel_reg_read((&(panel_vbotx_reg()->vbo_ctrl0.u32)));
    vbo_ctrl0.bits.vbo_data_mode = data_mode;
    panel_reg_write((&(panel_vbotx_reg()->vbo_ctrl0.u32)), vbo_ctrl0.u32);
}

hi_void vbo_set_lane_num(hal_panel_vbo_lan_number lane_num)
{
    u_vbo_ctrl0 vbo_ctrl0;

    vbo_ctrl0.u32 = panel_reg_read((&(panel_vbotx_reg()->vbo_ctrl0.u32)));
    vbo_ctrl0.bits.vbo_lane_num = lane_num;
    panel_reg_write((&(panel_vbotx_reg()->vbo_ctrl0.u32)), vbo_ctrl0.u32);
}

hi_void hal_panel_set_vbo_byte_num(hi_drv_panel_byte_num byte_num)
{
    u_vbo_ctrl0 vbo_ctrl0;

    vbo_ctrl0.u32 = panel_reg_read((&(panel_vbotx_reg()->vbo_ctrl0.u32)));
    vbo_ctrl0.bits.vbo_byte_num = byte_num;
    panel_reg_write((&(panel_vbotx_reg()->vbo_ctrl0.u32)), vbo_ctrl0.u32);
}

hi_void vbo_set_lane0_swap(hi_u32 lane_swap_num)
{
    u_lane_swap1 lane_swap1;

    lane_swap1.u32 = panel_reg_read((&(panel_vbotx_reg()->lane_swap1.u32)));
    lane_swap1.bits.lane0_swap = lane_swap_num;
    panel_reg_write((&(panel_vbotx_reg()->lane_swap1.u32)), lane_swap1.u32);
}

hi_void vbo_set_lane1_swap(hi_u32 lane_swap_num)
{
    u_lane_swap1 lane_swap1;

    lane_swap1.u32 = panel_reg_read((&(panel_vbotx_reg()->lane_swap1.u32)));
    lane_swap1.bits.lane1_swap = lane_swap_num;
    panel_reg_write((&(panel_vbotx_reg()->lane_swap1.u32)), lane_swap1.u32);
}
hi_void vbo_set_lane2_swap(hi_u32 lane_swap_num)
{
    u_lane_swap1 lane_swap1;

    lane_swap1.u32 = panel_reg_read((&(panel_vbotx_reg()->lane_swap1.u32)));
    lane_swap1.bits.lane2_swap = lane_swap_num;
    panel_reg_write((&(panel_vbotx_reg()->lane_swap1.u32)), lane_swap1.u32);
}
hi_void vbo_set_lane3_swap(hi_u32 lane_swap_num)
{
    u_lane_swap1 lane_swap1;

    lane_swap1.u32 = panel_reg_read((&(panel_vbotx_reg()->lane_swap1.u32)));
    lane_swap1.bits.lane3_swap = lane_swap_num;
    panel_reg_write((&(panel_vbotx_reg()->lane_swap1.u32)), lane_swap1.u32);
}
hi_void vbo_set_lane4_swap(hi_u32 lane_swap_num)
{
    u_lane_swap2 lane_swap2;

    lane_swap2.u32 = panel_reg_read((&(panel_vbotx_reg()->lane_swap2.u32)));
    lane_swap2.bits.lane4_swap = lane_swap_num;
    panel_reg_write((&(panel_vbotx_reg()->lane_swap2.u32)), lane_swap2.u32);
}
hi_void vbo_set_lane5_swap(hi_u32 lane_swap_num)
{
    u_lane_swap2 lane_swap2;

    lane_swap2.u32 = panel_reg_read((&(panel_vbotx_reg()->lane_swap2.u32)));
    lane_swap2.bits.lane5_swap = lane_swap_num;
    panel_reg_write((&(panel_vbotx_reg()->lane_swap2.u32)), lane_swap2.u32);
}
hi_void vbo_set_lane6_swap(hi_u32 lane_swap_num)
{
    u_lane_swap2 lane_swap2;

    lane_swap2.u32 = panel_reg_read((&(panel_vbotx_reg()->lane_swap2.u32)));
    lane_swap2.bits.lane6_swap = lane_swap_num;
    panel_reg_write((&(panel_vbotx_reg()->lane_swap2.u32)), lane_swap2.u32);
}
hi_void vbo_set_lane7_swap(hi_u32 lane_swap_num)
{
    u_lane_swap2 lane_swap2;

    lane_swap2.u32 = panel_reg_read((&(panel_vbotx_reg()->lane_swap2.u32)));
    lane_swap2.bits.lane7_swap = lane_swap_num;
    panel_reg_write((&(panel_vbotx_reg()->lane_swap2.u32)), lane_swap2.u32);
}

hi_void vbo_set_lane8_swap(hi_u32 lane_swap_num)
{
    u_lane_swap3 lane_swap3;

    lane_swap3.u32 = panel_reg_read((&(panel_vbotx_reg()->lane_swap3.u32)));
    lane_swap3.bits.lane8_swap = lane_swap_num;
    panel_reg_write((&(panel_vbotx_reg()->lane_swap3.u32)), lane_swap3.u32);
}

hi_void vbo_set_lane9_swap(hi_u32 lane_swap_num)
{
    u_lane_swap3 lane_swap3;

    lane_swap3.u32 = panel_reg_read((&(panel_vbotx_reg()->lane_swap3.u32)));
    lane_swap3.bits.lane9_swap = lane_swap_num;
    panel_reg_write((&(panel_vbotx_reg()->lane_swap3.u32)), lane_swap3.u32);
}

hi_void vbo_set_lane10_swap(hi_u32 lane_swap_num)
{
    u_lane_swap3 lane_swap3;

    lane_swap3.u32 = panel_reg_read((&(panel_vbotx_reg()->lane_swap3.u32)));
    lane_swap3.bits.lane10_swap = lane_swap_num;
    panel_reg_write((&(panel_vbotx_reg()->lane_swap3.u32)), lane_swap3.u32);
}

hi_void vbo_set_lane11_swap(hi_u32 lane_swap_num)
{
    u_lane_swap3 lane_swap3;

    lane_swap3.u32 = panel_reg_read((&(panel_vbotx_reg()->lane_swap3.u32)));
    lane_swap3.bits.lane11_swap = lane_swap_num;
    panel_reg_write((&(panel_vbotx_reg()->lane_swap3.u32)), lane_swap3.u32);
}

hi_void vbo_set_lane12_swap(hi_u32 lane_swap_num)
{
    u_lane_swap4 lane_swap4;

    lane_swap4.u32 = panel_reg_read((&(panel_vbotx_reg()->lane_swap4.u32)));
    lane_swap4.bits.lane12_swap = lane_swap_num;
    panel_reg_write((&(panel_vbotx_reg()->lane_swap4.u32)), lane_swap4.u32);
}

hi_void vbo_set_lane13_swap(hi_u32 lane_swap_num)
{
    u_lane_swap4 lane_swap4;

    lane_swap4.u32 = panel_reg_read((&(panel_vbotx_reg()->lane_swap4.u32)));
    lane_swap4.bits.lane13_swap = lane_swap_num;
    panel_reg_write((&(panel_vbotx_reg()->lane_swap4.u32)), lane_swap4.u32);
}

hi_void vbo_set_lane14_swap(hi_u32 lane_swap_num)
{
    u_lane_swap4 lane_swap4;

    lane_swap4.u32 = panel_reg_read((&(panel_vbotx_reg()->lane_swap4.u32)));
    lane_swap4.bits.lane14_swap = lane_swap_num;
    panel_reg_write((&(panel_vbotx_reg()->lane_swap4.u32)), lane_swap4.u32);
}

hi_void vbo_set_lane15_swap(hi_u32 lane_swap_num)
{
    u_lane_swap4 lane_swap4;

    lane_swap4.u32 = panel_reg_read((&(panel_vbotx_reg()->lane_swap4.u32)));
    lane_swap4.bits.lane15_swap = lane_swap_num;
    panel_reg_write((&(panel_vbotx_reg()->lane_swap4.u32)), lane_swap4.u32);
}

hi_void vbo_set_lane_swap(hi_bool swap)
{
    if (swap == HI_TRUE) {
        vbo_set_lane0_swap(HAL_PANEL_VBO_LANE_NUM0);
        vbo_set_lane1_swap(HAL_PANEL_VBO_LANE_NUM4);
        vbo_set_lane2_swap(HAL_PANEL_VBO_LANE_NUM1);
        vbo_set_lane3_swap(HAL_PANEL_VBO_LANE_NUM5);

        vbo_set_lane4_swap(HAL_PANEL_VBO_LANE_NUM2);
        vbo_set_lane5_swap(HAL_PANEL_VBO_LANE_NUM6);
        vbo_set_lane6_swap(HAL_PANEL_VBO_LANE_NUM3);
        vbo_set_lane7_swap(HAL_PANEL_VBO_LANE_NUM7);

        vbo_set_lane8_swap(HAL_PANEL_VBO_LANE_NUM8);
        vbo_set_lane9_swap(HAL_PANEL_VBO_LANE_NUM12);
        vbo_set_lane10_swap(HAL_PANEL_VBO_LANE_NUM9);
        vbo_set_lane11_swap(HAL_PANEL_VBO_LANE_NUM13);

        vbo_set_lane12_swap(HAL_PANEL_VBO_LANE_NUM10);
        vbo_set_lane13_swap(HAL_PANEL_VBO_LANE_NUM14);
        vbo_set_lane14_swap(HAL_PANEL_VBO_LANE_NUM11);
        vbo_set_lane15_swap(HAL_PANEL_VBO_LANE_NUM15);
    } else {
        vbo_set_lane0_swap(HAL_PANEL_VBO_LANE_NUM0);
        vbo_set_lane1_swap(HAL_PANEL_VBO_LANE_NUM1);
        vbo_set_lane2_swap(HAL_PANEL_VBO_LANE_NUM2);
        vbo_set_lane3_swap(HAL_PANEL_VBO_LANE_NUM3);

        vbo_set_lane4_swap(HAL_PANEL_VBO_LANE_NUM4);
        vbo_set_lane5_swap(HAL_PANEL_VBO_LANE_NUM5);
        vbo_set_lane6_swap(HAL_PANEL_VBO_LANE_NUM6);
        vbo_set_lane7_swap(HAL_PANEL_VBO_LANE_NUM7);

        vbo_set_lane8_swap(HAL_PANEL_VBO_LANE_NUM8);
        vbo_set_lane9_swap(HAL_PANEL_VBO_LANE_NUM9);
        vbo_set_lane10_swap(HAL_PANEL_VBO_LANE_NUM10);
        vbo_set_lane11_swap(HAL_PANEL_VBO_LANE_NUM11);

        vbo_set_lane12_swap(HAL_PANEL_VBO_LANE_NUM12);
        vbo_set_lane13_swap(HAL_PANEL_VBO_LANE_NUM13);
        vbo_set_lane14_swap(HAL_PANEL_VBO_LANE_NUM14);
        vbo_set_lane15_swap(HAL_PANEL_VBO_LANE_NUM15);
    }
}

hi_void vbo_set_lr_swap(hi_bool lr_swap)
{
#ifndef HI_FPGA
    if (lr_swap) {
        vbo_set_lane0_swap(HAL_PANEL_VBO_LANE_NUM4);
        vbo_set_lane1_swap(HAL_PANEL_VBO_LANE_NUM5);
        vbo_set_lane2_swap(HAL_PANEL_VBO_LANE_NUM6);
        vbo_set_lane3_swap(HAL_PANEL_VBO_LANE_NUM7);

        vbo_set_lane4_swap(HAL_PANEL_VBO_LANE_NUM0);
        vbo_set_lane5_swap(HAL_PANEL_VBO_LANE_NUM1);
        vbo_set_lane6_swap(HAL_PANEL_VBO_LANE_NUM2);
        vbo_set_lane7_swap(HAL_PANEL_VBO_LANE_NUM3);
    } else {
        vbo_set_lane0_swap(HAL_PANEL_VBO_LANE_NUM0);
        vbo_set_lane1_swap(HAL_PANEL_VBO_LANE_NUM1);
        vbo_set_lane2_swap(HAL_PANEL_VBO_LANE_NUM2);
        vbo_set_lane3_swap(HAL_PANEL_VBO_LANE_NUM3);

        vbo_set_lane4_swap(HAL_PANEL_VBO_LANE_NUM4);
        vbo_set_lane5_swap(HAL_PANEL_VBO_LANE_NUM5);
        vbo_set_lane6_swap(HAL_PANEL_VBO_LANE_NUM6);
        vbo_set_lane7_swap(HAL_PANEL_VBO_LANE_NUM7);
    }
#endif
    vbo_set_lane0_swap(HAL_PANEL_VBO_LANE_NUM0);
    vbo_set_lane1_swap(HAL_PANEL_VBO_LANE_NUM4);
    vbo_set_lane2_swap(HAL_PANEL_VBO_LANE_NUM1);
    vbo_set_lane3_swap(HAL_PANEL_VBO_LANE_NUM5);

    vbo_set_lane4_swap(HAL_PANEL_VBO_LANE_NUM2);
    vbo_set_lane5_swap(HAL_PANEL_VBO_LANE_NUM6);
    vbo_set_lane6_swap(HAL_PANEL_VBO_LANE_NUM3);
    vbo_set_lane7_swap(HAL_PANEL_VBO_LANE_NUM7);
}

hi_void vbo_set_left_internal_swap(hi_bool l_int_swap)
{
    if (l_int_swap) {
        vbo_set_lane0_swap(HAL_PANEL_VBO_LANE_NUM3);
        vbo_set_lane1_swap(HAL_PANEL_VBO_LANE_NUM2);
        vbo_set_lane2_swap(HAL_PANEL_VBO_LANE_NUM1);
        vbo_set_lane3_swap(HAL_PANEL_VBO_LANE_NUM0);
    } else {
        vbo_set_lane0_swap(HAL_PANEL_VBO_LANE_NUM0);
        vbo_set_lane1_swap(HAL_PANEL_VBO_LANE_NUM1);
        vbo_set_lane2_swap(HAL_PANEL_VBO_LANE_NUM2);
        vbo_set_lane3_swap(HAL_PANEL_VBO_LANE_NUM3);
    }
}

hi_void vbo_set_right_internal_swap(hi_bool r_int_swap)
{
    if (r_int_swap) {
        vbo_set_lane4_swap(HAL_PANEL_VBO_LANE_NUM7);
        vbo_set_lane5_swap(HAL_PANEL_VBO_LANE_NUM6);
        vbo_set_lane6_swap(HAL_PANEL_VBO_LANE_NUM5);
        vbo_set_lane7_swap(HAL_PANEL_VBO_LANE_NUM4);
    } else {
        vbo_set_lane4_swap(HAL_PANEL_VBO_LANE_NUM4);
        vbo_set_lane5_swap(HAL_PANEL_VBO_LANE_NUM5);
        vbo_set_lane6_swap(HAL_PANEL_VBO_LANE_NUM6);
        vbo_set_lane7_swap(HAL_PANEL_VBO_LANE_NUM7);
    }
}

hi_void vbo_set_test_mode_enable(hi_bool enable)
{
    u_vbo_ctrl1 vbo_ctrl1;

    vbo_ctrl1.u32 = panel_reg_read((&(panel_vbotx_reg()->vbo_ctrl1.u32)));
    vbo_ctrl1.bits.vbo_test_en = enable;
    panel_reg_write((&(panel_vbotx_reg()->vbo_ctrl1.u32)), vbo_ctrl1.u32);
}

hi_void vbo_set_test_mode(hal_panel_vbo_test_mode vbo_test_mode)
{
    u_vbo_ctrl1 vbo_ctrl1;

    vbo_ctrl1.u32 = panel_reg_read((&(panel_vbotx_reg()->vbo_ctrl1.u32)));
    vbo_ctrl1.bits.vbo_test_mode = vbo_test_mode;
    panel_reg_write((&(panel_vbotx_reg()->vbo_ctrl1.u32)), vbo_ctrl1.u32);
}

hi_void hal_panel_set_vbo_lockn_sw_mode(hi_bool enable)
{
    u_vbo_ctrl1 vbo_ctrl1;

    vbo_ctrl1.u32 = panel_reg_read((&(panel_vbotx_reg()->vbo_ctrl1.u32)));
    vbo_ctrl1.bits.vbo_lockn_mode = enable;
    panel_reg_write((&(panel_vbotx_reg()->vbo_ctrl1.u32)), vbo_ctrl1.u32);
}
hi_void hal_panel_set_vbo_lockn_high(hi_bool bhigh)
{
    u_vbo_ctrl1 vbo_ctrl1;

    vbo_ctrl1.u32 = panel_reg_read((&(panel_vbotx_reg()->vbo_ctrl1.u32)));
    vbo_ctrl1.bits.vbo_lockn_cfg = bhigh;
    panel_reg_write((&(panel_vbotx_reg()->vbo_ctrl1.u32)), vbo_ctrl1.u32);
}


hi_void vbo_set_pn_swap(hal_panel_vbo_pn_swap swap_mode)
{
    return;
}

hi_void vbo_get_cdr_lock_status(hi_bool *pb_lockn)
{
    u_phystat0 phystat0;

    phystat0.u32 = panel_reg_read((&(panel_vbotx_reg()->phystat0.u32)));
    if (phystat0.bits.vbo_lockn_0 == 0) {
        *pb_lockn = HI_TRUE;
    } else {
        *pb_lockn = HI_FALSE;
    }
}

hi_void hal_panel_set_vbo_msb_mode(hi_bool msb_mode)
{
    u_vbo_ctrl0 vbo_ctrl0;
    msb_mode = HI_TRUE;
    vbo_ctrl0.u32 = panel_reg_read(&(panel_vbotx_reg()->vbo_ctrl0.u32));
    vbo_ctrl0.bits.msb_lsb_mode = !msb_mode;
    panel_reg_write(&(panel_vbotx_reg()->vbo_ctrl0.u32), vbo_ctrl0.u32);
}

hi_void vbo_set_channel_sel(hi_u32 part_sel_val)
{
    u_channel_sel channel_sel;

    channel_sel.u32 = panel_reg_read((&(panel_vbotx_reg()->channel_sel.u32)));
    channel_sel.bits.chan2_sel = part_sel_val;
    panel_reg_write((&(panel_vbotx_reg()->channel_sel.u32)), channel_sel.u32);
}

hi_void vbo_set_vbo_partiton_sel(hi_u32 division_sel)
{
    u_vbo_ctrl1 vbo_ctrl1;
    vbo_ctrl1.u32 = panel_reg_read((&(panel_vbotx_reg()->vbo_ctrl1.u32)));

    panel_reg_write((&(panel_vbotx_reg()->vbo_ctrl1.u32)), vbo_ctrl1.u32);
}

hi_void vbo_set_color_bar(hi_bool color_bar)
{
    u_cbar_ctrl cbar_ctrl;
    u_test_cbar_upd test_cbar_upd;
    cbar_ctrl.u32 = panel_reg_read(&(panel_vbotx_reg()->cbar_ctrl.u32));
    cbar_ctrl.bits.cbar_timing_en = color_bar;
    cbar_ctrl.bits.cbar_data_en = color_bar;

    test_cbar_upd.u32 = panel_reg_read(&(panel_vbotx_reg()->test_cbar_upd.u32));
    test_cbar_upd.bits.regup = 0x1;

    panel_reg_write(&(panel_vbotx_reg()->cbar_ctrl.u32), cbar_ctrl.u32);
    panel_reg_write(&(panel_vbotx_reg()->test_cbar_upd.u32), test_cbar_upd.u32);
}

hi_void hal_panel_set_vbo_lockn_sel(hi_void)
{
    u_vbo_ctrl0 vbo_ctrl0;

    vbo_ctrl0.u32 = panel_reg_read(&(panel_vbotx_reg()->vbo_ctrl0.u32));
    vbo_ctrl0.bits.vbo_lockn_sel = 0x3;  /* 0x3 is vbo_lockn_3 */

    panel_reg_write((&(panel_vbotx_reg()->vbo_ctrl0.u32)), vbo_ctrl0.u32);

    return ;
}

hi_void hal_panel_set_vbo_htpn_mode(hi_void)
{
    u_vbo_ctrl1 vbo_ctrl1;

    vbo_ctrl1.u32 = panel_reg_read(&(panel_vbotx_reg()->vbo_ctrl1.u32));
    vbo_ctrl1.bits.vbo_htpn_mode = 0x1;

    panel_reg_write((&(panel_vbotx_reg()->vbo_ctrl1.u32)), vbo_ctrl1.u32);

    return ;
}

hi_void hal_panel_set_vbo_frm_mask(hi_void)
{
    u_vbo_ctrl1 vbo_ctrl1;

    vbo_ctrl1.u32 = panel_reg_read(&(panel_vbotx_reg()->vbo_ctrl1.u32));
    vbo_ctrl1.bits.vbo_frm_mask = 0x1;

    panel_reg_write((&(panel_vbotx_reg()->vbo_ctrl1.u32)), vbo_ctrl1.u32);

    return ;
}

hi_void hal_panel_set_vbo_lane_num(hi_drv_panel_link_type link_mode)
{
    hal_panel_vbo_lan_number lane_num = HAL_PANEL_VBO_LAN_NUMBER_8;

    if (HI_DRV_PANEL_LINKTYPE_1LINK == link_mode) {
        lane_num = HAL_PANEL_VBO_LAN_NUMBER_1;
    } else if (HI_DRV_PANEL_LINKTYPE_2LINK == link_mode) {
        lane_num = HAL_PANEL_VBO_LAN_NUMBER_2;
    } else if (HI_DRV_PANEL_LINKTYPE_4LINK == link_mode) {
        lane_num = HAL_PANEL_VBO_LAN_NUMBER_4;
    } else if (HI_DRV_PANEL_LINKTYPE_16LINK == link_mode) {
        lane_num = HAL_PANEL_VBO_LAN_NUMBER_16;
    } else {
    }

    vbo_set_lane_num(lane_num);
}

hi_void hal_panel_set_vbo_lane_swap(hi_drv_panel_division_type division_type, hi_drv_panel_link_type link_type)
{
    hi_bool one_division8_link = HI_FALSE;
    hi_bool one_division16_link = HI_FALSE;
    hi_bool two_division16_link = HI_FALSE;

    if ((division_type == HI_DRV_PANEL_DIVISION_ONE) && (HI_DRV_PANEL_LINKTYPE_8LINK == link_type)) {
        one_division8_link = HI_FALSE;
    }

    if ((division_type == HI_DRV_PANEL_DIVISION_ONE) && (HI_DRV_PANEL_LINKTYPE_16LINK == link_type)) {
        one_division16_link = HI_TRUE;
    }

    if ((division_type == HI_DRV_PANEL_DIVISION_TWO) && (HI_DRV_PANEL_LINKTYPE_16LINK == link_type)) {
        two_division16_link = HI_TRUE;
    }

    if (one_division8_link || one_division16_link || two_division16_link) {
        vbo_set_lane_swap(HI_TRUE);
    } else {
        vbo_set_lane_swap(HI_FALSE);
    }
}

hi_void hal_panel_set_vbo_channel_sel(hi_drv_panel_division_type division_type, hi_drv_panel_link_type link_type)
{
    hi_u32 part_sel_val = 1;

    if (link_type == HI_DRV_PANEL_LINKTYPE_8LINK &&
        (division_type == HI_DRV_PANEL_DIVISION_ONE_OE ||
        division_type == HI_DRV_PANEL_DIVISION_ONE ||
        division_type == HI_DRV_PANEL_DIVISION_TWO)) {
        part_sel_val = 1;     /* 1: set channel sel 1 */
    } else {
        part_sel_val = 2;     /* 2: set channel sel 2 */
    }

    vbo_set_channel_sel(part_sel_val);
}

hi_void hal_panel_set_vbo_partiton_sel(hi_drv_panel_division_type division_type, hi_drv_panel_link_type link_type)
{
    hi_u32 division_sel = 0;

    if (division_type == HI_DRV_PANEL_DIVISION_FOUR
        && link_type == HI_DRV_PANEL_LINKTYPE_8LINK) {
        division_sel = 1; /* 1 is default param */
    }

    vbo_set_vbo_partiton_sel(division_sel);
}

hi_void hal_panel_set_vbo_pn_swap(hi_bool pn_swap)
{
    hal_panel_vbo_pn_swap swap_mode = HAL_PANEL_VBO_PN_SWAP_NONE;

    if (pn_swap) {
        swap_mode = HAL_PANEL_VBO_PN_SWAP_ALL;
    }

    vbo_set_pn_swap(swap_mode);
}

hi_void hal_panel_set_vbo_clk(hi_void)
{
    U_PERI_CRG285 peri_crg_285;

    peri_crg_285.u32 = panel_reg_read(&(hi_drv_sys_get_crg_reg_ptr()->PERI_CRG285.u32));
    peri_crg_285.u32 = 0x1821C1; /* 0x1821C1 is temporary value */

    panel_reg_write((&(hi_drv_sys_get_crg_reg_ptr()->PERI_CRG285.u32)), peri_crg_285.u32);

    return ;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
