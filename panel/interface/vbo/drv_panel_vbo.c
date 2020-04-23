/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel vbo module
* Author: sdk
* Create: 2019-04-03
*/

#include "drv_panel.h"
#include "drv_panel_define.h"
#include "hal_panel_combophy.h"
#include "hal_panel_vbo.h"
#include "hal_panel_comm.h"
#include "drv_panel_vbo.h"
#include "linux/hisilicon/securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static hal_combo_phy_clkform g_st_panel_vbo_phy_clk_cfg[] = {
    /* 4k*2k 60hz 8link 10bit */
    { PANEL_WIDTH_4K, PANEL_HEIGHT_2K, HI_PANEL_INTF_TYPE_VBONE,
      HI_PANEL_BIT_DEPTH_10BIT, HI_DRV_PANEL_LINKTYPE_8LINK, PANEL_PORT_MODE_1_PORT,
      PANEL_PAIR_MODE_1_PAIR, 490000000, 600000000,
        { HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_PHY_DIV_IN_12_MULTIPLE,
          HAL_PANEL_PHY_DIV_FEEDBACK_20_MULTIPLE, HAL_PANEL_PHY_DIV_ICP_CURRENT_2UA
        }
    },
    /* 4k*2k 120hz 16link 10bit */
    { PANEL_WIDTH_4K, PANEL_HEIGHT_2K, HI_PANEL_INTF_TYPE_VBONE,
      HI_PANEL_BIT_DEPTH_10BIT, HI_DRV_PANEL_LINKTYPE_16LINK, PANEL_PORT_MODE_1_PORT,
      PANEL_PAIR_MODE_1_PAIR, 1000000000, 1200000000,
        { HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_PHY_DIV_IN_12_MULTIPLE,
          HAL_PANEL_PHY_DIV_FEEDBACK_20_MULTIPLE, HAL_PANEL_PHY_DIV_ICP_CURRENT_2UA
        }
    },
    /* FHD 60hz  2link 10bit */
    { PANEL_WIDTH_2K, PANEL_HEIGHT_1K, HI_PANEL_INTF_TYPE_VBONE,
      HI_PANEL_BIT_DEPTH_10BIT, HI_DRV_PANEL_LINKTYPE_2LINK, PANEL_PORT_MODE_1_PORT,
      PANEL_PAIR_MODE_1_PAIR, 130000000, 160000000,
        { HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_PHY_DIV_IN_12_MULTIPLE,
          HAL_PANEL_PHY_DIV_FEEDBACK_20_MULTIPLE, HAL_PANEL_PHY_DIV_ICP_CURRENT_2UA
        }
    },
    /* FHD 120hz  4link 10bit */
    { PANEL_WIDTH_2K, PANEL_HEIGHT_1K, HI_PANEL_INTF_TYPE_VBONE,
      HI_PANEL_BIT_DEPTH_10BIT, HI_DRV_PANEL_LINKTYPE_4LINK, PANEL_PORT_MODE_1_PORT,
      PANEL_PAIR_MODE_1_PAIR, 280000000, 300000000,
        { HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_PHY_DIV_IN_12_MULTIPLE,
          HAL_PANEL_PHY_DIV_FEEDBACK_20_MULTIPLE, HAL_PANEL_PHY_DIV_ICP_CURRENT_2UA
        }
    },
    /* 1280*720 1link 10bit */
    { PANEL_WIDTH_1280, PANEL_HEIGHT_720, HI_PANEL_INTF_TYPE_VBONE,
      HI_PANEL_BIT_DEPTH_10BIT, HI_DRV_PANEL_LINKTYPE_1LINK, PANEL_PORT_MODE_1_PORT,
      PANEL_PAIR_MODE_1_PAIR, 64000000, 80000000,
        { HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_PHY_DIV_IN_12_MULTIPLE,
          HAL_PANEL_PHY_DIV_FEEDBACK_20_MULTIPLE, HAL_PANEL_PHY_DIV_ICP_CURRENT_2UA
        }
    },
    /* 4k*2k 30hz 4link 10bit */
    { PANEL_WIDTH_4K, PANEL_HEIGHT_2K, HI_PANEL_INTF_TYPE_VBONE,
      HI_PANEL_BIT_DEPTH_10BIT, HI_DRV_PANEL_LINKTYPE_4LINK, PANEL_PORT_MODE_1_PORT,
      PANEL_PAIR_MODE_1_PAIR, 280000000, 300000000,
        { HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_PHY_DIV_IN_12_MULTIPLE,
          HAL_PANEL_PHY_DIV_FEEDBACK_20_MULTIPLE, HAL_PANEL_PHY_DIV_ICP_CURRENT_2UA
        }
    },

    /* 4k*2k 60hz 8link 8bit */
    { PANEL_WIDTH_4K, PANEL_HEIGHT_2K, HI_PANEL_INTF_TYPE_VBONE,
      HI_PANEL_BIT_DEPTH_8BIT, HI_DRV_PANEL_LINKTYPE_8LINK, PANEL_PORT_MODE_1_PORT,
      PANEL_PAIR_MODE_1_PAIR, 490000000, 600000000,
        { HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_PHY_DIV_IN_12_MULTIPLE,
          HAL_PANEL_PHY_DIV_FEEDBACK_15_MULTIPLE, HAL_PANEL_PHY_DIV_ICP_CURRENT_1POINT5UA
        }
    },

    /* 4k*720 60hz 8link 8bit */
    {PANEL_WIDTH_4K,  PANEL_HEIGHT_720, HI_PANEL_INTF_TYPE_VBONE,
     HI_PANEL_BIT_DEPTH_8BIT, HI_DRV_PANEL_LINKTYPE_8LINK, PANEL_PORT_MODE_1_PORT,
     PANEL_PAIR_MODE_1_PAIR, 150000000, 250000000,
     { HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_PHY_DIV_IN_12_MULTIPLE,
       HAL_PANEL_PHY_DIV_FEEDBACK_15_MULTIPLE, HAL_PANEL_PHY_DIV_ICP_CURRENT_1POINT5UA}
    },
    /* 4k*2k 30hz 4link 8bit */
    { PANEL_WIDTH_4K, PANEL_HEIGHT_2K, HI_PANEL_INTF_TYPE_VBONE,
      HI_PANEL_BIT_DEPTH_8BIT, HI_DRV_PANEL_LINKTYPE_4LINK, PANEL_PORT_MODE_1_PORT,
      PANEL_PAIR_MODE_1_PAIR, 280000000, 300000000,
        { HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_PHY_DIV_IN_12_MULTIPLE,
          HAL_PANEL_PHY_DIV_FEEDBACK_15_MULTIPLE, HAL_PANEL_PHY_DIV_ICP_CURRENT_1POINT5UA
        }
    },
    /* FHD 60hz  2link 8bit */
    { PANEL_WIDTH_2K, PANEL_HEIGHT_1K, HI_PANEL_INTF_TYPE_VBONE,
      HI_PANEL_BIT_DEPTH_8BIT, HI_DRV_PANEL_LINKTYPE_2LINK, PANEL_PORT_MODE_1_PORT,
      PANEL_PAIR_MODE_1_PAIR, 130000000, 160000000,
        { HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_PHY_DIV_IN_12_MULTIPLE,
          HAL_PANEL_PHY_DIV_FEEDBACK_15_MULTIPLE, HAL_PANEL_PHY_DIV_ICP_CURRENT_1POINT5UA
        }
    },
    /* FHD 120hz  4link 8bit */
    { PANEL_WIDTH_2K, PANEL_HEIGHT_1K, HI_PANEL_INTF_TYPE_VBONE,
      HI_PANEL_BIT_DEPTH_8BIT, HI_DRV_PANEL_LINKTYPE_4LINK, PANEL_PORT_MODE_1_PORT,
      PANEL_PAIR_MODE_1_PAIR, 280000000, 300000000,
        { HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_PHY_DIV_IN_12_MULTIPLE,
          HAL_PANEL_PHY_DIV_FEEDBACK_15_MULTIPLE, HAL_PANEL_PHY_DIV_ICP_CURRENT_1POINT5UA
        }
    },
    /* 5k*2k 60hz 16link 10bit */
    { PANEL_WIDTH_5K, PANEL_HEIGHT_2K, HI_PANEL_INTF_TYPE_VBONE,
      HI_PANEL_BIT_DEPTH_8BIT, HI_DRV_PANEL_LINKTYPE_16LINK, PANEL_PORT_MODE_1_PORT,
      PANEL_PAIR_MODE_1_PAIR, 600000000, 1200000000,
        { HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_PHY_DIV_IN_12_MULTIPLE,
          HAL_PANEL_PHY_DIV_FEEDBACK_20_MULTIPLE, HAL_PANEL_PHY_DIV_ICP_CURRENT_2UA
        }
    },
    /* 5k*2k 60hz 16link 8bit */
    { PANEL_WIDTH_5K, PANEL_HEIGHT_2K, HI_PANEL_INTF_TYPE_VBONE,
      HI_PANEL_BIT_DEPTH_8BIT, HI_DRV_PANEL_LINKTYPE_16LINK, PANEL_PORT_MODE_1_PORT,
      PANEL_PAIR_MODE_1_PAIR, 600000000, 1200000000,
        { HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE, HAL_PANEL_PHY_DIV_IN_12_MULTIPLE,
          HAL_PANEL_PHY_DIV_FEEDBACK_20_MULTIPLE, HAL_PANEL_PHY_DIV_ICP_CURRENT_2UA
        }
    },
};

hi_s32 drv_panel_check_vbo_phy(hi_drv_panel_intf_attr *intf_attr_p)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_check_vbo_attr(hi_drv_panel_vbo_attr *vbo_attr_p)
{
    if (vbo_attr_p->vbo_spread_ratio > PANEL_SPREAD_RATIO_MAX) {
        hi_log_err("error: vbo spread ratio invalid, must in range[0, PANEL_SPREAD_RATIO_MAX]\n");
        return HI_FAILURE;
    }

    if ((vbo_attr_p->vbo_spread_freq < PANEL_VBO_SSFREQ_46P875KHZ)
        || (vbo_attr_p->vbo_spread_freq >= PANEL_VBO_SSFREQ_MAX)) {
        hi_log_err("error: VBO spread freq invalid must in \
                    range[PANEL_VBO_SSFREQ_46P875KHZ, PANEL_VBO_SSFREQ_MAX)\n");
        return HI_FAILURE;
    }

    if (vbo_attr_p->vbo_current >= PANEL_CURRENT_MAX) {
        hi_log_err("error: VBO current invalid, must in range[0, PANEL_CURRENT_MAX)\n");
        return HI_FAILURE;
    }

    if (vbo_attr_p->vbo_emphasis >= PANEL_EMP_MAX) {
        hi_log_err("error: VBO emphasis invalid, must in range[0, PANEL_EMP_MAX)\n");
        return HI_FAILURE;
    }

    if (vbo_attr_p->data_mode >= PANEL_VBO_DATAMODE_MAX) {
        hi_log_err("error: VBO data_mode invalid, must in \
                    range[0, PANEL_VBO_DATAMODE_MAX)\n");
        return HI_FAILURE;
    }

    if (vbo_attr_p->vbo_byte_num >= HI_DRV_PANEL_BYTE_NUM_MAX) {
        hi_log_err("error: VBO byte num invalid, must in \
                    range[HI_DRV_PANEL_VBO_BYTE_NUM_3, HI_DRV_PANEL_VBO_BYTE_NUM_BUTT)\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void drv_panel_update_vbo_attr(hi_drv_panel_vbo_attr *vbo_attr_p, hi_drv_panel_intf_attr *intf_attr_p)
{
    intf_attr_p->intf_enable = HI_TRUE;
    intf_attr_p->spread_enable = HI_TRUE;
    intf_attr_p->vbo_spread_ratio = vbo_attr_p->vbo_spread_ratio;
    intf_attr_p->vbo_spread_freq = vbo_attr_p->vbo_spread_freq;
    intf_attr_p->vbo_emphasis = vbo_attr_p->vbo_emphasis;
    intf_attr_p->vbo_current = vbo_attr_p->vbo_current;
}

hi_void drv_panel_update_complete_vbo_attr(hi_drv_panel_info *base_info, panel_vbo_comp_attr *vbo_comp_attr_p)
{
    hi_u8 i = 0;
    drv_panel_contex *panel_ctx_p = HI_NULL;
    drv_panel_get_panel_ctx(&panel_ctx_p);

    vbo_comp_attr_p->group_swap = HI_FALSE;
    vbo_comp_attr_p->left_internal_swap = HI_FALSE;
    vbo_comp_attr_p->right_internal_swap = HI_FALSE;
    vbo_comp_attr_p->bit0_at_high_bit = HI_FALSE;
    vbo_comp_attr_p->locken_sw_mode = HI_FALSE;
    vbo_comp_attr_p->locken_high = HI_FALSE;
    vbo_comp_attr_p->pn_swap = HI_FALSE;
    vbo_comp_attr_p->rising_edge = HI_TRUE;
    /* WORKING: vbo_comp_attr_p->byte_num */
    vbo_comp_attr_p->current_en = base_info->signal_attr.drv_current;
    vbo_comp_attr_p->emphasis = base_info->signal_attr.emphasis;
    vbo_comp_attr_p->resistor = DRV_PANEL_VBO_RESISTOR_50OHM;
    vbo_comp_attr_p->spread_ratio = base_info->spread_info.spread_ratio;
    vbo_comp_attr_p->spread_freq = base_info->spread_info.spread_freq;
    vbo_comp_attr_p->division_type = base_info->division_type;

    for (i = 0; i < LANE_NUM; i++) {
        panel_ctx_p->combo_attr.au8_drv_current[i] = (hi_u8)base_info->signal_attr.drv_current;
        panel_ctx_p->combo_attr.au8_pre_emphasis[i] = (hi_u8)base_info->signal_attr.emphasis;
    }
}

hi_void drv_panel_set_vbo_cfg(panel_vbo_comp_attr *vb1_attr_p, drv_panel_combo_attr *combo_attr_p)
{
#ifdef PANEL_TMP
    hal_panel_set_vbo_lane_num(vb1_attr_p->link_type);
    hal_panel_set_vbo_lane_swap(vb1_attr_p->division_type, vb1_attr_p->link_type);
    hal_panel_set_vbo_channel_sel(vb1_attr_p->division_type, vb1_attr_p->link_type);
    hal_panel_set_vbo_partiton_sel(vb1_attr_p->division_type, vb1_attr_p->link_type);
    hal_panel_set_vbo_msb_mode(vb1_attr_p->bit0_at_high_bit);
    hal_panel_set_vbo_lockn_sw_mode(vb1_attr_p->locken_sw_mode);
    hal_panel_set_vbo_lockn_high(vb1_attr_p->locken_high);
    hal_panel_set_vbo_pn_swap(vb1_attr_p->pn_swap);
    hal_panel_set_vbo_data_mode(vb1_attr_p->data_mode);
    hal_panel_set_vbo_byte_num(vb1_attr_p->byte_num);
    hal_panel_set_combo_phy_current(HAL_PANEL_INTFTYPE_VBONE, (const hi_u8 *)combo_attr_p->au8_drv_current);
    hal_panel_set_combo_phy_pre_emphasis(HAL_PANEL_INTFTYPE_VBONE, (const hi_u8 *)combo_attr_p->au8_drv_current,
                                         (const hi_u8 *)combo_attr_p->au8_pre_emphasis);
#endif
}

hi_s32 drv_panel_set_vbo_power(hi_bool enable)
{
    /* 1. ctrl VBO data part */
    hal_panel_set_vbo_enable(enable);

    return HI_SUCCESS;
}

hi_s32 drv_panel_get_vbo_power(hi_bool *enable)
{
    hal_panel_get_vbo_enable(enable);

    return HI_SUCCESS;
}

hi_s32 drv_panel_set_vbo_atrr(hi_drv_panel_intf_attr *old_attr_p, hi_drv_panel_intf_attr *new_attr_p)
{
    hi_u8 i = 0;
    drv_panel_contex *panel_ctx_p = HI_NULL;
    drv_panel_get_panel_ctx(&panel_ctx_p);

    if (old_attr_p->intf_enable != new_attr_p->intf_enable) {
        hal_panel_set_vbo_enable(new_attr_p->intf_enable);
    }

    if (old_attr_p->vbo_emphasis != new_attr_p->vbo_emphasis) {
        for (i = 0; i < LANE_NUM; i++) {
            panel_ctx_p->combo_attr.au8_drv_current[i] = (hi_u8)new_attr_p->vbo_current;
            panel_ctx_p->combo_attr.au8_pre_emphasis[i] = (hi_u8)new_attr_p->vbo_emphasis;
        }

        hal_panel_set_combo_phy_pre_emphasis(HAL_PANEL_INTFTYPE_VBONE,
                                             (const hi_u8 *)panel_ctx_p->combo_attr.au8_drv_current,
                                             (const hi_u8 *)panel_ctx_p->combo_attr.au8_pre_emphasis);
    }

    if (old_attr_p->vbo_current != new_attr_p->vbo_current) {
        for (i = 0; i < LANE_NUM; i++) {
            panel_ctx_p->combo_attr.au8_drv_current[i] = (hi_u8)new_attr_p->vbo_current;
        }

        hal_panel_set_combo_phy_current(HAL_PANEL_INTFTYPE_VBONE,
                                        (const hi_u8 *)panel_ctx_p->combo_attr.au8_drv_current);
    }

    if (old_attr_p->vbo_spread_freq != new_attr_p->vbo_spread_freq) {
    }

    if (old_attr_p->vbo_spread_ratio != new_attr_p->vbo_spread_ratio) {
    }

    if (old_attr_p->spread_enable != new_attr_p->spread_enable) {
    }

    return HI_SUCCESS;
}

hi_void drv_panel_vbo_phy_init(hi_drv_panel_info *panel_info_p)
{
    drv_panel_contex *panel_ctx_p = HI_NULL;
    drv_panel_get_panel_ctx(&panel_ctx_p);

    hal_panel_set_dphy_port_pn_swap(0);
    hal_panel_set_aphy_intf_power_mode(HAL_PANEL_APHY_MODE_VBOTX);
    hal_panel_set_dphy_bit_width(HAL_PANEL_DPHY_BITWIDTH_VBO);

    if (panel_info_p->division_type <= HI_DRV_PANEL_DIVISION_ONE) {
        hal_panel_set_dphy_port_sort(HAL_PANLE_SORTTYPE_DIV_ONE,
            0x0, 0x0, 0x0, 0x0);
    } else {
        hal_panel_set_dphy_port_sort(HAL_PANLE_SORTTYPE_DIV_TWO_FOUR,
            0x0, 0x0, 0x0, 0x0);
    }

    if (panel_ctx_p->panel_cfg.link_type == HI_DRV_PANEL_LINKTYPE_2LINK) {
        /* when input two link data, combophy only use the first and second lane,use two_four line sequence type */
        hal_panel_set_dphy_port_sort(HAL_PANLE_SORTTYPE_DIV_TWO_FOUR,
            0x0, 0x0, 0x0, 0x0);
    }
}

hi_void drv_panel_set_vbo_phy_clk(hal_combo_phy_clkcfg *phy_clk_cfg_p, drv_panel_contex *panel_ctx_p)
{
    hi_u32 member = 0;
    hi_u32 pixel_clk = panel_ctx_p->image_info.base_info.timing_60hz.pixel_clk;
    hi_drv_panel_bit_depth bit_depth = panel_ctx_p->image_info.base_info.data_bit_depth;
    hi_u32 phy_clk_cfg_num = sizeof(g_st_panel_vbo_phy_clk_cfg) / sizeof(hal_combo_phy_clkform);

    for (member = 0; member < phy_clk_cfg_num; member++) {
        if ((bit_depth == g_st_panel_vbo_phy_clk_cfg[member].panel_bit_depth)
            && (pixel_clk >= g_st_panel_vbo_phy_clk_cfg[member].min_pixel_clk)
            && (pixel_clk <= g_st_panel_vbo_phy_clk_cfg[member].max_pixel_clk)) {
            if (memcpy_s(phy_clk_cfg_p, sizeof(hal_combo_phy_clkcfg),
                &(g_st_panel_vbo_phy_clk_cfg[member].phy_clk_cfg),
                sizeof(hal_combo_phy_clkcfg)) != HI_SUCCESS) {
                hi_log_err("memcpy failed!\n");
            }
            break;
        }
    }

    if (member > phy_clk_cfg_num) {
        hi_log_err("no suitable vbo phy_clk_cfg!\n");
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


