/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: function define
* Author: image
* Create: 2019-04-12
 */

#ifndef __HAL_WIN_INTF_H__
#define __HAL_WIN_INTF_H__

#include "hi_drv_disp.h"
#include "vdp_base_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/********************* start define struct **********************/
typedef struct tag_layer_capacity {
    hi_bool support;

    /* region */
    hi_bool support_region;
    hi_u32 region_num;
    /* zme */
    hi_bool support_zme;
    /* cmp */
    hi_bool support_dcmp;
    hi_bool support_tile_dcmp;
    hi_bool support_afbc_dcmp;
    /* common ip */
    hi_bool supprot_commonip;

    vdp_rect max_read_rect;
    vdp_rect input_align;
    vdp_rect output_align;
    hi_u32 support_max_bit_width;

    /* pixformat */
    hi_u32 support_pixformat_num;
    hi_drv_pixel_format *support_pixformat;
} layer_capacity;

typedef struct tag_region_info {
    /* ctrl */
    hi_bool enable;
    hi_bool mute_en;
    hi_bool smmu_en;

    /* rect */
    vdp_rect in_rect;
    vdp_rect disp_rect;

    /* addr */
    vdp_vid_addr addr[VDP_FDR_MRG_ADDR_BUTT];
} region_info;

typedef struct tag_vdp_layer_info {
    /* ctrls */
    hi_bool enable;
    hi_bool flip;
    hi_bool hdcp_en;
    hi_bool pqbypass;  // debug
    hi_u32 vid_alpha;
    hi_u32 vp_alpha;
    vdp_rect crop_rect;
    hi_drv_display disp_chn;

    /* mute */
    hi_bool mute_en;
    hi_drv_disp_color mute_color;
    hi_drv_disp_color letterbox_color;

    /* src info */
    hi_bool frame_smmu_en;
    hi_bool frame_secure_en;
    hi_u32 frame_rate;
    vdp_rect frame_rect;
    hi_bool frame_inerlace;
    hi_drv_field_mode frame_field_mode;
    hi_drv_pixel_bitwidth frame_bit_width;
    hi_drv_hdr_type frame_hdr_type;
    hi_drv_pixel_format frame_pix_format;
    hi_drv_data_fmt data_fmt;

    /* cmp */
    hi_drv_compress_mode cmp_mode;
    hi_u32 ycmp_rate;
    hi_u32 ccmp_rate;

    hi_drv_color_descript frame_color_space;
    hi_drv_3d_type frame_3d_type;
    vdp_vid_addr frame_addr[VDP_FDR_VID_ADDR_BUTT];

    /* output info */
    vdp_rect vid_video_rect;
    vdp_rect vid_disp_rect;
    vdp_rect vp_rect;
    hi_u32 out_disp_rate;
    hi_bool out_inerlace;
    hi_drv_color_descript out_color_space;
    hi_drv_disp_stereo_mode out_3d_mode;
    hi_drv_disp_out_type out_hdr_type;

    /* multi region info */
    hi_bool multi_region_enable;
    hi_u32 region_num;
    region_info region[FDR_REGION_NUM_MAX];
} vdp_layer_info;

typedef struct tag_vdp_hdr_info {
    hi_u32 tmp_for_hdr;
} vdp_hdr_info;

typedef struct tag_vdp_dolby_info {
    hi_u32 tmp_for_dolby;
} vdp_dolby_info;

/* capacity */
layer_capacity *hal_layer_get_capacity(hi_u32 layer);

/* cfg layer */
hi_void hal_layer_cfg_disable(hi_u32 layer, hi_u32 region_index);
hi_void hal_layer_cfg_hisi_process(hi_u32 layer, vdp_layer_info *layer_info, vdp_hdr_info *hdr_info);
hi_void hal_layer_cfg_dolby_process(hi_u32 layer, vdp_layer_info *layer_info, vdp_dolby_info *hdr_info);
hi_void hal_layer_disp_complete_process(hi_drv_display disp_chn);

/* zorder */
hi_s32 hal_layer_set_zorder(hi_u32 layer, hi_u32 zorder);
hi_s32 hal_layer_get_zorder(hi_u32 layer, hi_u32 *zorder);

/* state */
hi_void hal_layer_get_water_mark_state(hi_u32 layer, hi_bool *water_mark);

/* debug */
hi_void hal_layer_debug_get_logic_info(hi_u32 layer, hi_u32 region, char *logic_buf);
hi_void hal_layer_debug_pattern(hi_u32 layer, vdp_fdr_testpattern *debug);
hi_void hal_layer_debug_get_checksum(hi_u32 layer, hi_u32 *checksum);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_VO_HAL_H__ */



