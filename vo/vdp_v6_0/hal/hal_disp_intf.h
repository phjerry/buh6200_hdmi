/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: function define
* Author: image
* Create: 2019-04-12
 */

#ifndef __HAL_DISP_INTF_H__
#define __HAL_DISP_INTF_H__

#include "hi_drv_disp.h"
#include "vdp_base_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/************************* define data struct **********************/
typedef struct tag_vdp_capacity {
    hi_u32 version_part_low;
    hi_u32 version_part_high;
    hi_u32 lowpower_value;
} vdp_capacity;

typedef struct tag_fmt_section {
    hi_drv_disp_fmt section_start_fmt;
    hi_drv_disp_fmt section_end_fmt;
} fmt_section;

typedef struct tag_disp_capacity {
    hi_bool support;
    hi_bool support_wbc;
    hi_bool support_interlaced;
    hi_drv_display support_attach_disp;

    /* intf supprot */
    hi_u32 support_intf_num;
    hi_drv_disp_intf_id *support_intf;

    /* fmt supprot */
    hi_u32 support_fmt_section_num;
    fmt_section *support_fmt_section;

    /* csc support */
    hi_u32 support_csc_num;
    hi_drv_disp_output_color_space *support_csc;

    /* output type support */
    hi_u32 support_type_num;
    hi_drv_disp_out_type *support_type;

    /* hdr support */
    hi_bool support_hdr_hdr10;
    hi_bool support_hdr_hlg;
    hi_bool support_hdr_slf;
    hi_bool support_hdr_dolby;
    hi_bool support_hdr_jtp;
    hi_bool support_microvison;
} disp_capacity;

typedef enum tag_int_state_type {
    INT_STATE_TYPE_INTF_MASK = 0,
    INT_STATE_TYPE_INTF_UNMASK,
    INT_STATE_TYPE_SMMU_MASK,
    INT_STATE_TYPE_SMMU_UNMASK
} int_state_type;

typedef struct tag_disp_intf {
    hi_drv_disp_intf intf_attr;

    hi_bool intf_attached;
    hi_bool link_venc;
    disp_intf_venc intf_venc;
} disp_attach_intf_info;

typedef struct tag_aspect_ratio {
    hi_u32 aspect_ratio_w;
    hi_u32 aspect_ratio_h;
} hal_aspect_ratio;

typedef struct tag_disp_timing_info {
    hi_drv_disp_fmt disp_fmt;
    hi_drv_disp_stereo_mode disp_3d_mode;
    hi_drv_color_descript color_space;
    hi_bool interlace;
    hi_u32  refresh_rate;
    hi_u32  pixel_clk; /* unit is khz */
    hi_u32  vic_num;
    hal_aspect_ratio aspect_ratio;
    vdp_sync_info sync_info;
} disp_timing_info;

typedef struct {
    hi_bool enable;
    hi_bool avi_enable;
    hi_bool smd_enable;
    hi_u64  addr;
    hi_u32  data_len;
} hal_hdmi_infoframe;
/* vdp */
hi_s32 hal_vdp_init(hi_void);
hi_s32 hal_vdp_deinit(hi_void);
hi_s32 hal_vdp_resume(hi_void);
hi_s32 hal_vdp_suspend(hi_void);
/* 注意时钟配置是及时生效的，配置的是crg寄存器 而非vdp寄存器 */
hi_s32 hal_vdp_set_clock_enable(clock_module module, hi_bool enable);
hi_s32 hal_vdp_reg_check(hi_void);
vdp_capacity *hal_vdp_get_capacity(void);

/* disp */
hi_s32 hal_disp_set_bgcolor(hi_drv_display chn, hi_drv_color_descript cs, hi_drv_disp_color *bgcolor);
hi_void hal_disp_chn_mask(hi_drv_display chn, hi_bool mask_enale);
hi_s32 hal_disp_set_enable(hi_drv_display chn, hi_bool enable);
hi_s32 hal_disp_get_enable(hi_drv_display chn, hi_bool *enable);
/* rwzb */
hi_s32 hal_disp_set_rwzb_info(hi_drv_display chn, hi_u32 stream_index);

/* capacity */
disp_capacity *hal_disp_get_capacity(hi_drv_display chn);

/* format */
hi_s32 hal_disp_set_timing(hi_drv_display chn, hi_drv_disp_intf *intf_attr, disp_timing_info *timing);
hi_s32 hal_disp_set_auto_sync(hi_drv_display chn, hi_bool enable); /* modify arg */
hi_void hal_disp_send_infoframe(hal_hdmi_infoframe *hdmi_infoframe);


/* interrupt */
hi_void hal_disp_set_interrupt_enable(hi_u32 int_num, hi_bool enable);
hi_void hal_disp_get_interrupt_state(int_state_type type, hi_u32 *state);
hi_void hal_disp_clean_interrupt_state(int_state_type type, hi_u32 state);

/* err status */
hi_s32 hal_disp_get_dcmp_state(hi_drv_display disp_id);
hi_void hal_disp_clean_dcmp_state(hi_drv_display disp_id);
hi_s32 hal_disp_get_master_state(hi_void);
hi_void hal_disp_clean_master_state(hi_void);
hi_s32 hal_disp_get_mmu_error_addr_ns(hi_void);


/* state */
hi_void hal_disp_get_state(hi_drv_display chn, hi_bool *btm, hi_u32 *vcnt, hi_u32 *int_cnt);
hi_s32 hal_disp_check_gfx_state(hi_u32 *total_pixel, hi_u32 *zero_pixel);

/* debug */
hi_void hal_disp_debug_get_smmu_error_addr(smmu_error_addr *error_addr);
hi_void hal_disp_debug_colorbar(hi_drv_display chn, hi_bool colorbar_enable);
hi_void hal_disp_debug_get_checksum(hi_drv_display chn, hi_u32 *r, hi_u32 *g, hi_u32 *b);
/* INK需要等待2个中断时间，1个配置生效 1个逻辑取值，不能在中断调用 */
hi_void hal_disp_debug_ink(disp_ink_attr *attr, disp_color *rgb_date);

/* intf */
hi_s32 hal_disp_attach_intf(hi_drv_display chn, disp_attach_intf_info *intf);
hi_s32 hal_disp_detach_intf(hi_drv_display chn, disp_attach_intf_info *intf);
hi_s32 hal_disp_set_intf_format(hi_drv_display chn, disp_attach_intf_info *intf,
                                disp_timing_info *timing_info);
hi_s32 hal_disp_set_intf_enable(hi_drv_display chn, disp_attach_intf_info *intf, hi_bool enable);

/* o5 */
hi_s32 hal_disp_set_dac_enable(hi_bool enable);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_DISP_CMP_H__ */










