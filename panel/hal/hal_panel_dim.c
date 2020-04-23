/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel hal dim module
* Author: sdk
* Create: 2019-04-03
*/

#include "hal_panel_comm.h"
#include "hal_panel_dim.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void hal_panel_set_dim_final_gain(hi_u32 u32gain_final_0d)
{
    u_dim_final_gain_0d dim_final_gain_0d;

    dim_final_gain_0d.u32 = panel_reg_read(&(panel_vdp_reg()->dim_final_gain_0d.u32));
    dim_final_gain_0d.bits.gain_final_0d = u32gain_final_0d;
    dim_final_gain_0d.bits.offset_final_0d = 0;
    panel_reg_write(&(panel_vdp_reg()->dim_final_gain_0d.u32), dim_final_gain_0d.u32);
}

hi_void hal_panel_set_dim_lcd_comp0dk_value(hi_u32 value)
{
    u_dim_lcd_comp_0d dim_lcd_comp_0d;

    dim_lcd_comp_0d.u32 = panel_reg_read(&(panel_vdp_reg()->dim_lcd_comp_0d.u32));
    dim_lcd_comp_0d.bits.lcd_k_led0d = value;
    panel_reg_write(&(panel_vdp_reg()->dim_lcd_comp_0d.u32), dim_lcd_comp_0d.u32);
}

hi_void hal_panel_set_dim_enable(hi_bool enable)
{
    u_dim_glb_ctrl dim_glb_ctrl;

    dim_glb_ctrl.u32 = panel_reg_read(&(panel_vdp_reg()->dim_glb_ctrl.u32));
    dim_glb_ctrl.bits.dimming_en = enable;
    panel_reg_write(&(panel_vdp_reg()->dim_glb_ctrl.u32), dim_glb_ctrl.u32);
}

hi_void hal_panel_set_2d_dim_final_gain(hi_u32 u32gain_final_2d)
{
    u_dim_final_gain_2d dim_final_gain_2d;

    dim_final_gain_2d.u32 = panel_reg_read(&(panel_vdp_reg()->dim_final_gain_2d.u32));
    dim_final_gain_2d.bits.gain_final_2d = u32gain_final_2d;
    dim_final_gain_2d.bits.offset_final_2d = 0;
    panel_reg_write(&(panel_vdp_reg()->dim_final_gain_2d.u32), dim_final_gain_2d.u32);
}

hi_void hal_panel_set_dim_led_enable(hi_bool enable)
{
    u_dim_glb_ctrl dim_glb_ctrl;

    dim_glb_ctrl.u32 = panel_reg_read(&(panel_vdp_reg()->dim_glb_ctrl.u32));
    dim_glb_ctrl.bits.led_en = enable;
    panel_reg_write(&(panel_vdp_reg()->dim_glb_ctrl.u32), dim_glb_ctrl.u32);
}

hi_void hal_panel_set_dim_lcd_enable(hi_bool enable)
{
    u_dim_glb_ctrl dim_glb_ctrl;

    dim_glb_ctrl.u32 = panel_reg_read(&(panel_vdp_reg()->dim_glb_ctrl.u32));
    dim_glb_ctrl.bits.lcd_en = enable;
    panel_reg_write(&(panel_vdp_reg()->dim_glb_ctrl.u32), dim_glb_ctrl.u32);
}

hi_void hal_panel_set_2d_dim_enable(hi_bool enable)
{
    u_dim_glb_ctrl dim_glb_ctrl;

    dim_glb_ctrl.u32 = panel_reg_read(&(panel_vdp_reg()->dim_glb_ctrl.u32));
    dim_glb_ctrl.bits.dim2d_en = !enable;
    panel_reg_write(&(panel_vdp_reg()->dim_glb_ctrl.u32), dim_glb_ctrl.u32);
}

hi_void hal_panel_set_dim_led_num(hi_u32 hor_num, hi_u32 ver_num)
{
    u_dim_led_num dim_led_num;

    dim_led_num.u32 = panel_reg_read(&(panel_vdp_reg()->dim_led_num.u32));
    dim_led_num.bits.led_numv = ver_num - 1; /* 1 is index */
    dim_led_num.bits.led_numh = hor_num - 1; /* 1 is index */
    panel_reg_write(&(panel_vdp_reg()->dim_led_num.u32), dim_led_num.u32);
}

hi_void hal_panel_set_dim_segment_size(hi_u32 hor_size, hi_u32 ver_size)
{
    u_dim_seg_metircs dim_seg_metircs;

    dim_seg_metircs.u32 = panel_reg_read(&(panel_vdp_reg()->dim_seg_metircs.u32));
    dim_seg_metircs.bits.seg_height = ver_size;
    dim_seg_metircs.bits.seg_width = hor_size;
    panel_reg_write(&(panel_vdp_reg()->dim_seg_metircs.u32), dim_seg_metircs.u32);
}

hi_void hal_panel_set_dim_state_size(hi_u32 hor_size, hi_u32 ver_size)
{
    u_dim_stat_metrics dim_stat_metrics;

    dim_stat_metrics.u32 = panel_reg_read(&(panel_vdp_reg()->dim_stat_metrics.u32));
    dim_stat_metrics.bits.stat_height = ver_size;
    dim_stat_metrics.bits.stat_width = hor_size;
    panel_reg_write(&(panel_vdp_reg()->dim_stat_metrics.u32), dim_stat_metrics.u32);
}

hi_void hal_panel_set_dim_glb_norm_unit(hi_u32 glb_norm_unit)
{
    u_dim_glb_norm_unit dim_glb_norm_unit;

    dim_glb_norm_unit.u32 = panel_reg_read(&(panel_vdp_reg()->dim_glb_norm_unit.u32));
    dim_glb_norm_unit.bits.glb_norm_unit = glb_norm_unit;
    panel_reg_write(&(panel_vdp_reg()->dim_glb_norm_unit.u32), dim_glb_norm_unit.u32);
}

hi_void hal_panel_set_dim_seg_norm_unit(hi_u32 seg_norm_unit)
{
    u_dim_seg_norm_unit dim_seg_norm_unit;

    dim_seg_norm_unit.u32 = panel_reg_read(&(panel_vdp_reg()->dim_seg_norm_unit.u32));
    dim_seg_norm_unit.bits.seg_norm_unit = seg_norm_unit;
    panel_reg_write(&(panel_vdp_reg()->dim_seg_norm_unit.u32), dim_seg_norm_unit.u32);
}

hi_void hal_panel_set_dim_ver_scl_ratio(hi_u32 ver_scl_ratio)
{
    u_dim_scl_ratio_v dim_scl_ratio_v;

    dim_scl_ratio_v.u32 = panel_reg_read(&(panel_vdp_reg()->dim_scl_ratio_v.u32));
    dim_scl_ratio_v.bits.lvfir_scl_int = 0;
    dim_scl_ratio_v.bits.lvfir_scl_dec = ver_scl_ratio;
    panel_reg_write(&(panel_vdp_reg()->dim_scl_ratio_v.u32), dim_scl_ratio_v.u32);
}

hi_void hal_panel_set_dim_hor_scl_ratio(hi_u32 hor_scl_ratio)
{
    u_dim_scl_ratio_h dim_scl_ratio_h;

    dim_scl_ratio_h.u32 = panel_reg_read(&(panel_vdp_reg()->dim_scl_ratio_h.u32));
    dim_scl_ratio_h.bits.lhfir_scl_int = 0;
    dim_scl_ratio_h.bits.lhfir_scl_dec = hor_scl_ratio;
    panel_reg_write(&(panel_vdp_reg()->dim_scl_ratio_h.u32), dim_scl_ratio_h.u32);
}

hi_void hal_panel_set_dim_init_value(hi_u32 init_val)
{
    u_dim_glb_ctrl dim_glb_ctrl;

    dim_glb_ctrl.u32 = panel_reg_read(&(panel_vdp_reg()->dim_glb_ctrl.u32));
    dim_glb_ctrl.bits.mode_init_led = init_val;
    panel_reg_write(&(panel_vdp_reg()->dim_glb_ctrl.u32), dim_glb_ctrl.u32);
}

hi_void hal_panel_set_dynamic_bl_gain_level(hi_u32 bl_level)
{
    hal_panel_set_dim_final_gain(bl_level);
    hal_panel_set_dim_init_value(bl_level);
}

hi_void hal_panel_set_dim_demo_mode_enable(hi_bool enable)
{
    u_dim_glb_ctrl dim_glb_ctrl;

    dim_glb_ctrl.u32 = panel_reg_read((&(panel_vdp_reg()->dim_glb_ctrl.u32)));
    dim_glb_ctrl.bits.demo_en = enable;
    panel_reg_write((&(panel_vdp_reg()->dim_glb_ctrl.u32)), dim_glb_ctrl.u32);
}

hi_void hal_panel_set_dim_demo_mode(hal_panel_2_ddim_demo_mode demo_mode)
{
    u_dim_glb_ctrl dim_glb_ctrl;

    dim_glb_ctrl.u32 = panel_reg_read((&(panel_vdp_reg()->dim_glb_ctrl.u32)));
    dim_glb_ctrl.bits.demo_mode = demo_mode;
    panel_reg_write((&(panel_vdp_reg()->dim_glb_ctrl.u32)), dim_glb_ctrl.u32);
}

hi_void hal_panel_set_dim_reg_up(hi_void)
{
    u_dhd0_ctrl dhd0_ctrl;

    dhd0_ctrl.u32 = panel_reg_read(&(panel_vdp_reg()->dhd0_ctrl.u32));
    dhd0_ctrl.bits.regup = 0x1;
    panel_reg_write(&(panel_vdp_reg()->dhd0_ctrl.u32), dhd0_ctrl.u32);
}

hi_s32 hal_panel_set_dim_gradule_change_step(hi_u32 change_step)
{
    u_dim_post_fir_gain dim_post_fir_gain;

    dim_post_fir_gain.u32 = panel_reg_read(&(panel_vdp_reg()->dim_post_fir_gain.u32));
    dim_post_fir_gain.bits.gain_tf = change_step;
    panel_reg_write(&(panel_vdp_reg()->dim_post_fir_gain.u32), dim_post_fir_gain.u32);

    return HI_SUCCESS;
}

hi_s32 hal_panel_dim_gradule_change_step_print(hi_void)
{
    u_dim_post_fir_gain dim_post_fir_gain;

    dim_post_fir_gain.u32 = panel_reg_read(&(panel_vdp_reg()->dim_post_fir_gain.u32));
    hi_log_info("--------------dim_change_step:%d\n", dim_post_fir_gain.bits.gain_tf);

    return HI_SUCCESS;
}

#ifdef PANEL_LDM_SUPPORT
hi_void hal_panel_set_data_bit_width(hal_panel_ldm_bit_width_sel bit_width)
{
    u_dim_glb_ctrl dim_glb_ctrl;

    dim_glb_ctrl.u32 = panel_reg_read((&(panel_vdp_reg()->dim_glb_ctrl.u32)));
    dim_glb_ctrl.bits.dim_2d_bits = bit_width;
    panel_reg_write((&(panel_vdp_reg()->dim_glb_ctrl.u32)), dim_glb_ctrl.u32);
}

hi_void hal_panel_set_dim_scd_enable(hi_bool enable)
{
    u_dim_glb_ctrl dim_glb_ctrl;

    dim_glb_ctrl.u32 = panel_reg_read((&(panel_vdp_reg()->dim_glb_ctrl.u32)));
    dim_glb_ctrl.bits.scd_en = enable;
    panel_reg_write((&(panel_vdp_reg()->dim_glb_ctrl.u32)), dim_glb_ctrl.u32);
}

hi_void hal_panel_set_dim_scdflag(hi_u32 scd_flag)
{
    u_dim_glb_ctrl dim_glb_ctrl;

    dim_glb_ctrl.u32 = panel_reg_read((&(panel_vdp_reg()->dim_glb_ctrl.u32)));
    dim_glb_ctrl.bits.scd_flg = scd_flag;
    panel_reg_write((&(panel_vdp_reg()->dim_glb_ctrl.u32)), dim_glb_ctrl.u32);
}

hi_void hal_panel_get_2d_dim_data(hal_panel_ldm_bit_width_sel bit_width, hi_u32 data_cnt, hi_u32 *buf_p)
{
    hi_u32 src_index;
    hi_u32 dimming_val = 0;
    hi_u32 *ptr_p = HI_NULL;

    /* local_dimming data read enable */
    panel_reg_write(&(panel_vdp_reg()->dim_para_ren.u32), 0x1);

    src_index = 0;
    ptr_p = buf_p;
    while (src_index < data_cnt) {
        dimming_val = panel_reg_read(&(panel_vdp_reg()->dim_para_data.u32));

        *ptr_p = dimming_val;
        ptr_p++;

        if (HAL_PANEL_LDM_BIT_WIDTH_8BIT == bit_width) {
            src_index += 4;     /* 4: default parm for 8bit  */
        } else if ((HAL_PANEL_LDM_BIT_WIDTH_12BIT == bit_width)
                   || (HAL_PANEL_LDM_BIT_WIDTH_16BIT == bit_width)) {
            src_index += 2;    /* 2: default parm for 12 16bit  */
        } else {
            hi_log_err("get 2_d_dim data bit_width is out of range!\n");
            return;
        }
    }
    /* local_dimming data read disable */
    panel_reg_write(&(panel_vdp_reg()->dim_para_ren.u32), 0x0);
}

hi_void hal_panel_set_dim_y_mul_value(hi_bool uhd_flag)
{
    return;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

