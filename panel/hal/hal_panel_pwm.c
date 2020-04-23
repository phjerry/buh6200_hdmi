/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel hal pwm module
* Author: sdk
* Create: 2019-11-23
*/

#include "hal_panel_pwm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static hi_void hal_panel_set_dimming_pwm_enable(hi_bool enable)
{
    u_pwm_control pwm_control;

    pwm_control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_control.u32));
    pwm_control.bits.pwm_sel = 0x1;
    pwm_control.bits.dimming_pwm_en = enable;
    panel_reg_write(&(panel_vdp_reg()->pwm_control.u32), pwm_control.u32);
}

static hi_void hal_panel_set_pwm_1d1_enable(hi_bool enable)
{
    u_pwm_control pwm_control;
    u_pwm_1d1control pwm_1d1control;

    pwm_control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_control.u32));
    pwm_control.bits.pwm_sel_1 = 0x1;
    panel_reg_write(&(panel_vdp_reg()->pwm_control.u32), pwm_control.u32);

    pwm_1d1control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_1d1control.u32));
    pwm_1d1control.bits.dimming_pwm_1d1_en = enable;
    panel_reg_write(&(panel_vdp_reg()->pwm_1d1control.u32), pwm_1d1control.u32);
}

static hi_void hal_panel_set_pwm_1d2_enable(hi_bool enable)
{
    u_pwm_control pwm_control;
    u_pwm_1d2control pwm_1d2control;

    pwm_control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_control.u32));
    pwm_control.bits.pwm_sel_2 = 0x1;
    panel_reg_write(&(panel_vdp_reg()->pwm_control.u32), pwm_control.u32);

    pwm_1d2control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_1d2control.u32));
    pwm_1d2control.bits.dimming_pwm_1d2_en = enable;
    panel_reg_write(&(panel_vdp_reg()->pwm_1d2control.u32), pwm_1d2control.u32);
}

static hi_void hal_panel_set_pwm_1d3_enable(hi_bool enable)
{
    u_pwm_control pwm_control;
    u_pwm_1d3control pwm_1d3control;

    pwm_control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_control.u32));
    pwm_control.bits.pwm_sel_3 = 0x1;
    panel_reg_write(&(panel_vdp_reg()->pwm_control.u32), pwm_control.u32);

    pwm_1d3control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_1d3control.u32));
    pwm_1d3control.bits.dimming_pwm_1d3_en = enable;
    panel_reg_write((&(panel_vdp_reg()->pwm_1d3control.u32)), pwm_1d3control.u32);
}

static hi_void hal_panel_set_lr_sync_enable(hi_bool enable)
{
    u_lr_sync_control lr_sync_control;

    lr_sync_control.u32 = panel_reg_read(&(panel_vdp_reg()->lr_sync_control.u32));
    lr_sync_control.bits.lr_sync_en = enable;
    panel_reg_write((&(panel_vdp_reg()->lr_sync_control.u32)), lr_sync_control.u32);
}

static hi_void hal_panel_set_lr_glass_enable(hi_bool enable)
{
    u_lr_glass_control lr_glass_control;

    lr_glass_control.u32 = panel_reg_read(&(panel_vdp_reg()->lr_glass_control.u32));
    lr_glass_control.bits.lr_glass_en = enable;
    panel_reg_write((&(panel_vdp_reg()->lr_glass_control.u32)), lr_glass_control.u32);
}


hi_void hal_panel_set_pwm_enable(panel_pwm_type pwm_type, hi_bool enable)
{
    switch (pwm_type) {
        case PANEL_PWM_TYPE_PWM:
            hal_panel_set_dimming_pwm_enable(enable);
            break;
        case PANEL_PWM_TYPE_PWM1D1:
            hal_panel_set_pwm_1d1_enable(enable);
            break;
        case PANEL_PWM_TYPE_PWM1D2:
            hal_panel_set_pwm_1d2_enable(enable);
            break;
        case PANEL_PWM_TYPE_PWM1D3:
            hal_panel_set_pwm_1d3_enable(enable);
            break;
        case PANEL_PWM_TYPE_LRSYNC:
            hal_panel_set_lr_sync_enable(enable);
            break;
        case PANEL_PWM_TYPE_LRGLASS:
            hal_panel_set_lr_glass_enable(enable);
            break;
        default:
            break;
    }
}

hi_void hal_panel_set_pwm_inv(panel_pwm_type pwm_type, hi_bool inv)
{
    u_pwm_control pwm_control;
    u_pwm_1d1control pwm_1d1control;
    u_pwm_1d2control pwm_1d2control;
    u_pwm_1d3control pwm_1d3control;
    u_lr_sync_control lr_sync_control;
    u_lr_glass_control lr_glass_control;

    switch (pwm_type) {
        case PANEL_PWM_TYPE_PWM:
            pwm_control.u32 = panel_reg_read((&(panel_vdp_reg()->pwm_control.u32)));
            pwm_control.bits.dimming_pwm_inv = inv;
            panel_reg_write((&(panel_vdp_reg()->pwm_control.u32)), pwm_control.u32);
            break;

        case PANEL_PWM_TYPE_PWM1D1:
            pwm_1d1control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_1d1control.u32));
            pwm_1d1control.bits.dimming_pwm_1d1_inv = inv;
            panel_reg_write(&(panel_vdp_reg()->pwm_1d1control.u32), pwm_1d1control.u32);
            break;

        case PANEL_PWM_TYPE_PWM1D2:
            pwm_1d2control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_1d2control.u32));
            pwm_1d2control.bits.dimming_pwm_1d2_inv = inv;
            panel_reg_write(&(panel_vdp_reg()->pwm_1d2control.u32), pwm_1d2control.u32);
            break;

        case PANEL_PWM_TYPE_PWM1D3:
            pwm_1d3control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_1d3control.u32));
            pwm_1d3control.bits.dimming_pwm_1d3_inv = inv;
            panel_reg_write(&(panel_vdp_reg()->pwm_1d3control.u32), pwm_1d3control.u32);
            break;

        case PANEL_PWM_TYPE_LRSYNC:
            lr_sync_control.u32 = panel_reg_read(&(panel_vdp_reg()->lr_sync_control.u32));
            lr_sync_control.bits.lr_sync_inv = inv;
            panel_reg_write(&(panel_vdp_reg()->lr_sync_control.u32), lr_sync_control.u32);
            break;

        case PANEL_PWM_TYPE_LRGLASS:
            lr_glass_control.u32 = panel_reg_read(&(panel_vdp_reg()->lr_glass_control.u32));
            lr_glass_control.bits.lr_glass_inv = inv;
            panel_reg_write(&(panel_vdp_reg()->lr_glass_control.u32), lr_glass_control.u32);
            break;

        default:
            break;
    }
}

hi_void hal_panel_set_pwm_duty(panel_pwm_type pwm_type, hi_u32 duty)
{
    u_dimming_pwm_duty dimming_pwm_duty;
    u_dimming_pwm_1d1_duty dimming_pwm_1d1_duty;
    u_dimming_pwm_1d2_duty dimming_pwm_1d2_duty;
    u_dimming_pwm_1d3_duty dimming_pwm_1d3_duty;

    switch (pwm_type) {
        case PANEL_PWM_TYPE_PWM:
            dimming_pwm_duty.u32 = panel_reg_read(&(panel_vdp_reg()->dimming_pwm_duty.u32));
            dimming_pwm_duty.bits.dimming_pwm_duty = duty;
            panel_reg_write(&(panel_vdp_reg()->dimming_pwm_duty.u32), dimming_pwm_duty.u32);
            break;
        case PANEL_PWM_TYPE_PWM1D1:
            dimming_pwm_1d1_duty.u32 = panel_reg_read(&(panel_vdp_reg()->dimming_pwm_1d1_duty.u32));
            dimming_pwm_1d1_duty.bits.dimming_pwm_1d1_duty = duty;
            panel_reg_write(&(panel_vdp_reg()->dimming_pwm_1d1_duty.u32), dimming_pwm_1d1_duty.u32);
            break;
        case PANEL_PWM_TYPE_PWM1D2:
            dimming_pwm_1d2_duty.u32 = panel_reg_read(&(panel_vdp_reg()->dimming_pwm_1d2_duty.u32));
            dimming_pwm_1d2_duty.bits.dimming_pwm_1d2_duty = duty;
            panel_reg_write(&(panel_vdp_reg()->dimming_pwm_1d2_duty.u32), dimming_pwm_1d2_duty.u32);
            break;
        case PANEL_PWM_TYPE_PWM1D3:
            dimming_pwm_1d3_duty.u32 = panel_reg_read(&(panel_vdp_reg()->dimming_pwm_1d3_duty.u32));
            dimming_pwm_1d3_duty.bits.dimming_pwm_1d3_duty = duty;
            panel_reg_write(&(panel_vdp_reg()->dimming_pwm_1d3_duty.u32), dimming_pwm_1d3_duty.u32);
            break;
        default:
            break;
    }
}

hi_void hal_panel_set_pwm_freq(panel_pwm_type pwm_type, hi_u32 freq)
{
    u_pwm_freq pwm_freq;
    u_pwm_1d1_freq pwm_1d1_freq;
    u_pwm_1d2_freq pwm_1d2_freq;
    u_pwm_1d3_freq pwm_1d3_freq;
    hi_u32 freq_cfg;

    if (freq == 0) {
        return;
    }

    freq_cfg = PWM_CLOCK / freq;
    switch (pwm_type) {
        case PANEL_PWM_TYPE_PWM:
            pwm_freq.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_freq.u32));
            pwm_freq.bits.pwm_freq = freq_cfg;
            panel_reg_write(&(panel_vdp_reg()->pwm_freq.u32), pwm_freq.u32);
            break;
        case PANEL_PWM_TYPE_PWM1D1:
            pwm_1d1_freq.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_1d1_freq.u32));
            pwm_1d1_freq.bits.pwm_1d1_freq = freq_cfg;
            panel_reg_write(&(panel_vdp_reg()->pwm_1d1_freq.u32), pwm_1d1_freq.u32);
            break;
        case PANEL_PWM_TYPE_PWM1D2:
            pwm_1d2_freq.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_1d2_freq.u32));
            pwm_1d2_freq.bits.pwm_1d2_freq = freq_cfg;
            panel_reg_write(&(panel_vdp_reg()->pwm_1d2_freq.u32), pwm_1d2_freq.u32);
            break;
        case PANEL_PWM_TYPE_PWM1D3:
            pwm_1d3_freq.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_1d3_freq.u32));
            pwm_1d3_freq.bits.pwm_1d3_freq = freq_cfg;
            panel_reg_write(&(panel_vdp_reg()->pwm_1d3_freq.u32), pwm_1d3_freq.u32);
            break;
        default:
            break;
    }
}

hi_void hal_panel_set_pwm_vsync_rise_count(panel_pwm_type pwm_type, hi_bool enable)
{
    u_pwm_control pwm_control;
    u_pwm_1d1control pwm_1d1control;
    u_pwm_1d2control pwm_1d2control;
    u_pwm_1d3control pwm_1d3control;
    u_lr_sync_control lr_sync_control;

    switch (pwm_type) {
        case PANEL_PWM_TYPE_PWM:
            pwm_control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_control.u32));
            pwm_control.bits.dimming_vsync_rise = enable;
            panel_reg_write(&(panel_vdp_reg()->pwm_control.u32), pwm_control.u32);
            break;
        case PANEL_PWM_TYPE_PWM1D1:
            pwm_1d1control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_1d1control.u32));
            pwm_1d1control.bits.dimming_vsync_rise = enable;
            panel_reg_write(&(panel_vdp_reg()->pwm_1d1control.u32), pwm_1d1control.u32);
            break;
        case PANEL_PWM_TYPE_PWM1D2:
            pwm_1d2control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_1d2control.u32));
            pwm_1d2control.bits.dimming_vsync_rise = enable;
            panel_reg_write(&(panel_vdp_reg()->pwm_1d2control.u32), pwm_1d2control.u32);
            break;
        case PANEL_PWM_TYPE_PWM1D3:
            pwm_1d3control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_1d3control.u32));
            pwm_1d3control.bits.dimming_vsync_rise = enable;
            panel_reg_write(&(panel_vdp_reg()->pwm_1d3control.u32), pwm_1d3control.u32);
            break;
        case PANEL_PWM_TYPE_LRSYNC:
            lr_sync_control.u32 = panel_reg_read(&(panel_vdp_reg()->lr_sync_control.u32));
            lr_sync_control.bits.lr_sync_rise = enable;
            panel_reg_write(&(panel_vdp_reg()->lr_sync_control.u32), lr_sync_control.u32);
            break;
        default:
            break;
    }
}

hi_void hal_panel_set_pwm_init_high_level(panel_pwm_type pwm_type, hi_bool high_level)
{
    u_lr_sync_control lr_sync_control;
    u_lr_glass_control lr_glass_control;

    switch (pwm_type) {
        case PANEL_PWM_TYPE_LRSYNC:
            lr_sync_control.u32 = panel_reg_read(&(panel_vdp_reg()->lr_sync_control.u32));
            lr_sync_control.bits.lr_sync_int = high_level;
            panel_reg_write(&(panel_vdp_reg()->lr_sync_control.u32), lr_sync_control.u32);
            break;
        case PANEL_PWM_TYPE_LRGLASS:
            lr_glass_control.u32 = panel_reg_read(&(panel_vdp_reg()->lr_glass_control.u32));
            lr_glass_control.bits.lr_glass_int = high_level;
            panel_reg_write(&(panel_vdp_reg()->lr_glass_control.u32), lr_glass_control.u32);
            break;
        default:
            break;
    }
}

hi_void hal_panel_set_pwm_refresh(panel_pwm_type pwm_type)
{
    u_pwm_control pwm_control;
    u_lr_sync_control lr_sync_control;
    u_lr_glass_control lr_glass_control;

    switch (pwm_type) {
        case PANEL_PWM_TYPE_PWM:
        case PANEL_PWM_TYPE_PWM1D1:
        case PANEL_PWM_TYPE_PWM1D2:
        case PANEL_PWM_TYPE_PWM1D3:
            pwm_control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_control.u32));
            pwm_control.bits.refresh_cmd = 0xAD;
            panel_reg_write(&(panel_vdp_reg()->pwm_control.u32), pwm_control.u32);
            break;
        case PANEL_PWM_TYPE_LRSYNC:
            lr_sync_control.u32 = panel_reg_read(&(panel_vdp_reg()->lr_sync_control.u32));
            lr_sync_control.bits.refresh_cmd = 0xAD;
            panel_reg_write(&(panel_vdp_reg()->lr_sync_control.u32), lr_sync_control.u32);
            break;
        case PANEL_PWM_TYPE_LRGLASS:
            lr_glass_control.u32 = panel_reg_read(&(panel_vdp_reg()->lr_glass_control.u32));
            lr_glass_control.bits.refresh_cmd = 0xAD;
            panel_reg_write(&(panel_vdp_reg()->lr_glass_control.u32), lr_glass_control.u32);
            break;
        default:
            break;
    }
}

hi_void hal_panel_set_pwm_lr_glass_signal_in(hi_bool b3d_sync_out)
{
    u_lr_glass_control lr_glass_control;

    lr_glass_control.u32 = panel_reg_read(&(panel_vdp_reg()->lr_glass_control.u32));
    lr_glass_control.bits.lr_glass_sel = !b3d_sync_out;
    panel_reg_write(&(panel_vdp_reg()->lr_glass_control.u32), lr_glass_control.u32);
}

hi_void hal_panel_set3d_sg_mode(panel_pwm_type pwm_type, hi_bool enable)
{
    u_pwm_control pwm_control;
    u_pwm_1d1control pwm_1d1control;
    u_pwm_1d2control pwm_1d2control;
    u_pwm_1d3control pwm_1d3control;

    switch (pwm_type) {
        case PANEL_PWM_TYPE_PWM:
            pwm_control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_control.u32));
            pwm_control.bits.dimming_3d_sg = enable;
            panel_reg_write(&(panel_vdp_reg()->pwm_control.u32), pwm_control.u32);
            break;
        case PANEL_PWM_TYPE_PWM1D1:
            pwm_1d1control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_1d1control.u32));
            pwm_1d1control.bits.dimming_3d_sg = enable;
            panel_reg_write(&(panel_vdp_reg()->pwm_1d1control.u32), pwm_1d1control.u32);
            break;
        case PANEL_PWM_TYPE_PWM1D2:
            pwm_1d2control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_1d2control.u32));
            pwm_1d2control.bits.dimming_3d_sg = enable;
            panel_reg_write(&(panel_vdp_reg()->pwm_1d2control.u32), pwm_1d2control.u32);
            break;
        case PANEL_PWM_TYPE_PWM1D3:
            pwm_1d3control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_1d3control.u32));
            pwm_1d3control.bits.dimming_3d_sg = enable;
            panel_reg_write(&(panel_vdp_reg()->pwm_1d3control.u32), pwm_1d3control.u32);
            break;
        default:
            break;
    }
}

hi_void hal_panel_set_pwm_bl_mode(panel_pwm_type pwm_type, hi_bool dynamic_bl)
{
    u_pwm_control pwm_control;
    u_pwm_1d1control pwm_1d1control;
    u_pwm_1d2control pwm_1d2control;
    u_pwm_1d3control pwm_1d3control;

    switch (pwm_type) {
        case PANEL_PWM_TYPE_PWM:
            pwm_control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_control.u32));
            pwm_control.bits.dimming_duty_sel = !dynamic_bl;
            panel_reg_write(&(panel_vdp_reg()->pwm_control.u32), pwm_control.u32);
            break;
        case PANEL_PWM_TYPE_PWM1D1:
            pwm_1d1control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_1d1control.u32));
            pwm_1d1control.bits.dimming_duty_sel = !dynamic_bl;
            panel_reg_write(&(panel_vdp_reg()->pwm_1d1control.u32), pwm_1d1control.u32);
            break;
        case PANEL_PWM_TYPE_PWM1D2:
            pwm_1d2control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_1d2control.u32));
            pwm_1d2control.bits.dimming_duty_sel = !dynamic_bl;
            panel_reg_write(&(panel_vdp_reg()->pwm_1d2control.u32), pwm_1d2control.u32);
            break;
        case PANEL_PWM_TYPE_PWM1D3:
            pwm_1d3control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_1d3control.u32));
            pwm_1d3control.bits.dimming_duty_sel = !dynamic_bl;
            panel_reg_write(&(panel_vdp_reg()->pwm_1d3control.u32), pwm_1d3control.u32);
            break;
        default:
            break;
    }
}

hi_void hal_panel_set_pwm_sync_whole_mode(panel_pwm_type pwm_type, hi_bool enable)
{
    u_pwm_control pwm_control;
    u_pwm_1d1control pwm_1d1control;
    u_pwm_1d2control pwm_1d2control;
    u_pwm_1d3control pwm_1d3control;

    switch (pwm_type) {
        case PANEL_PWM_TYPE_PWM:
            pwm_control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_control.u32));
            pwm_control.bits.sync_whole = enable;
            pwm_control.bits.sync_vsync = !enable;
            panel_reg_write(&(panel_vdp_reg()->pwm_control.u32), pwm_control.u32);
            break;
        case PANEL_PWM_TYPE_PWM1D1:
            pwm_1d1control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_1d1control.u32));
            pwm_1d1control.bits.sync_whole = enable;
            pwm_1d1control.bits.sync_vsync = !enable;
            panel_reg_write(&(panel_vdp_reg()->pwm_1d1control.u32), pwm_1d1control.u32);
            break;
        case PANEL_PWM_TYPE_PWM1D2:
            pwm_1d2control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_1d2control.u32));
            pwm_1d2control.bits.sync_whole = enable;
            pwm_1d2control.bits.sync_vsync = !enable;
            panel_reg_write(&(panel_vdp_reg()->pwm_1d2control.u32), pwm_1d2control.u32);
            break;
        case PANEL_PWM_TYPE_PWM1D3:
            pwm_1d3control.u32 = panel_reg_read(&(panel_vdp_reg()->pwm_1d3control.u32));
            pwm_1d3control.bits.sync_whole = enable;
            pwm_1d3control.bits.sync_vsync = !enable;
            panel_reg_write(&(panel_vdp_reg()->pwm_1d3control.u32), pwm_1d3control.u32);
            break;
        default:
            break;
    }
}

hi_void hal_panel_set_pwm3d_sg_phase(panel_pwm_type pwm_type, hi_u32 m, hi_u32 p, hi_u32 q)
{
    u_pwm_m pwm_m;
    u_pwm_p pwm_p;
    u_pwm_q pwm_q;
    u_lr_sync_m lr_sync_m;
    u_lr_glass_m lr_glass_m;

    switch (pwm_type) {
        case PANEL_PWM_TYPE_PWM:
            pwm_m.bits.pwm_m = m;
            pwm_p.bits.pwm_p = p;
            pwm_q.bits.pwm_q = q;
            panel_reg_write(&(panel_vdp_reg()->pwm_m.u32), pwm_m.u32);
            panel_reg_write(&(panel_vdp_reg()->pwm_p.u32), pwm_p.u32);
            panel_reg_write(&(panel_vdp_reg()->pwm_q.u32), pwm_q.u32);
            break;
        case PANEL_PWM_TYPE_LRSYNC:
            lr_sync_m.bits.lr_sync_m = m;
            panel_reg_write(&(panel_vdp_reg()->lr_sync_m.u32), lr_sync_m.u32);
            break;
        case PANEL_PWM_TYPE_LRGLASS:
            lr_glass_m.bits.lr_glass_m = m;
            panel_reg_write(&(panel_vdp_reg()->lr_glass_m.u32), lr_glass_m.u32);
            break;
        default:
            break;
    }
}

hi_void hal_panel_set_pwm_dynamic_mode(hi_drv_panel_pwm_type pwm_type, hi_bool enable)
{
    panel_pwm_type hal_pwm_type;

    hal_pwm_type = (panel_pwm_type)pwm_type;

    hal_panel_set_pwm_bl_mode(hal_pwm_type, enable);
}

hi_void hal_panel_set_pwm_signal_to_open_drain(hi_void)
{
    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

