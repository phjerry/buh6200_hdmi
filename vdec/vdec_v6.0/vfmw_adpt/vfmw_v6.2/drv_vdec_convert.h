#ifndef __DRV_VDEC_CONVERT_H__
#define __DRV_VDEC_CONVERT_H__

#include "vfmw_ext.h"
#include "hi_vdec_type.h"
#include "vdec_priv_type.h"

hi_vdec_std vdec_cnvt_std_2_vdec(vfmw_vid_std std);
hi_vdec_norm_type vdec_cnvt_disp_norm_2_vdec(hi_u32 norm);
hi_vdec_frm_rate_type vdec_cnvt_frmrate_type_2_vdec(vfmw_pts_framrate_type type);
hi_vdec_sampling_type vdec_cnvt_sampling_type_2_vdec(hi_u32 scan_type);
hi_vdec_bit_depth vdec_cnvt_bit_depth_2_vdec(hi_u32 bit_depth);
hi_vdec_color_space vdec_cnvt_color_space_2_vdec(vfmw_vid_std std, vfmw_color_desc *color_info,
    hi_u32 width, hi_u32 height);
hi_s32 vdec_cnvt_event_2_vdec(hi_s32 chan_id, hi_s32 type, hi_void *args, hi_s32 len,
    hi_vdec_event *out);
vfmw_vid_std vdec_cnvt_std_2_vfmw(hi_vdec_std std);
vfmw_dec_mode vdec_cnvt_decmode_2_vfmw(hi_vdec_dec_mode mode);
vfmw_cmp vdec_cnvt_cmp_2_vfmw(hi_vdec_cmp_mode mode);
hi_u32 vdec_cnvt_cmd_id_2_vfmw(hi_u32 cmd);
vfmw_flush_type vdec_cnvt_flush_type_2_vfmw(hi_vdec_flush_type type);
vfmw_pts_framrate_type vdec_cnvt_frmrate_type_2_vfmw(hi_vdec_frm_rate_type type);
hi_u32 vdec_cnvt_event_map_2_vfmw(hi_u32 vdec_event_map);
vfmw_yuv_fmt vdec_cnvt_yuv_fmt_2_vfmw(vdec_yuv_fmt vdec_fmt);
hi_void vdec_cnvt_standard_ext(hi_vdec_std std, const hi_vdec_std_ext *in_ext, vfmw_std_ext *out_ext);

#endif
