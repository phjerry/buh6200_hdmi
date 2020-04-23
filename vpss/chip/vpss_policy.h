/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: vpss policy
 * Author: zhangjunyu
 * Create: 2016/07/01
 */

#ifndef __DRV_VPSS_POLICY_H__
#define __DRV_VPSS_POLICY_H__

#include "vpss_comm.h"
#include "drv_vpss_instance.h"
#include "drv_vpss_dbg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VPSS_MAX_HEIGHT_TRANS          2304
#define VPSS_MAX_WIDTH_TRANS           4096
#define VPSS_MIN_HEIGHT_TRANS          106
#define VPSS_MIN_WIDTH_TRANS           136
#define VPSS_MIN_HEIGHT_INTER          212
#define VPSS_MIN_WIDTH_INTER           128
#define VPSS_HEIGHT_INTER              1080
#define VPSS_MAX_HEIGHT_INTER          1088
#define VPSS_MAX_WIDTH_INTER           1920
#define VPSS_MAX_FRAMERATE_FOR_4K_ROTATION 40000
#define VPSS_DEFAULT_WIDTH_FOR_ROTATE_ALG  4096
#define VPSS_MAX_FRAMERATE_FOR_INTER       50000

#define VPSS_MAX_WIDTH_NO_PQ           4096
#define VPSS_MAX_HEIGHT_NO_PQ          2304

#define VPSS_4K_3840_WIDTH           3840
#define VPSS_4K_2160_HEIGHT          2160

#define VPSS_MAX_PROC_SET_SRC_WIDTH   8192
#define VPSS_MAX_PROC_SET_SRC_HEIGHT  4320
#define VPSS_MIN_PROC_SET_SRC_WIDTH   64
#define VPSS_MIN_PROC_SET_SRC_HEIGHT  64

typedef enum {
    VPSS_POLICY_DATAFMT_400 = 0,
    VPSS_POLICY_DATAFMT_420,
    VPSS_POLICY_DATAFMT_422,
    VPSS_POLICY_DATAFMT_444,
    VPSS_POLICY_DATAFMT_MAX
} vpss_policy_datafmt;

typedef enum {
    VPSS_POLICY_SUPPORT_ZME = 0,
    VPSS_POLICY_SUPPORT_CROP,
    VPSS_POLICY_SUPPORT_LBX,
    VPSS_POLICY_SUPPORT_RWZB,
    VPSS_POLICY_SUPPORT_HDR,
    VPSS_POLICY_SUPPORT_DETECT_3D,
    VPSS_POLICY_SUPPORT_DECOUNTOR,
    VPSS_POLICY_SUPPORT_NPUWBC,

    VPSS_POLICY_SUPPORT_MAX
} vpss_policy_support_type;

typedef struct {
    hi_u32 width;
    hi_u32 height;
    hi_u32 frame_rate;
    hi_drv_sample_type sample_type;
} vpss_policy_alg_input_info;

hi_bool vpss_policy_support_height_and_width(hi_u32 width, hi_u32 height);
hi_s32 vpss_policy_support_cccl_height_and_width(hi_u32 *width, hi_u32 *height);
hi_bool vpss_policy_check_need_trans(hi_drv_vpss_video_frame *frame, vpss_instance *instance);
hi_bool vpss_policy_check_need_trans_withport(hi_drv_vpss_video_frame *frame, vpss_instance *instance);
hi_bool vpss_policy_get_port_buf_strategy(hi_void);
hi_bool vpss_policy_revise_image(hi_drv_rect *src_crop_rect, hi_bool use_crop_rect,
                                 hi_drv_crop_rect *offset_rect, hi_drv_vpss_video_frame *frame);
hi_bool vpss_policy_check_use_pq(hi_drv_vpss_video_frame *frame, vpss_debug_info *global_dbg_ctrl);
hi_bool vpss_policy_support_out0_zme(hi_void);
hi_bool vpss_policy_support_out1_zme(hi_void);
hi_bool vpss_policy_support_out2_zme(hi_void);
hi_bool vpss_policy_support_pzme(hi_void);
hi_bool vpss_policy_support_input_crop(hi_void);
hi_bool vpss_policy_support_output_crop(hi_void);
hi_bool vpss_policy_support_in_fmt(hi_drv_pixel_format pixel_format);
hi_bool vpss_policy_support_out_fmt(hi_drv_pixel_format pixel_format);
hi_drv_pixel_format vpss_policy_get_nr_frf_fmt(hi_u32 width, hi_drv_pixel_format pixel_format);
hi_drv_pixel_format vpss_policy_get_port_out_fmt(hi_drv_pixel_format src_fmt, hi_drv_pixel_format port_fmt);
vpss_policy_datafmt vpss_policy_get_data_fmt(hi_drv_pixel_format pixel_format);
hi_void vpss_policy_revise_out_frame_info(hi_drv_pixel_format *pixel_format, hi_drv_compress_info *cmp_info);
hi_bool vpss_policy_check_out_frame_cmp_fmt(hi_drv_pixel_format pixel_format, hi_drv_compress_info cmp_info);
hi_bool vpss_policy_check_out_frame_cmp_rote(hi_drv_vpss_rotation rotation, hi_drv_compress_info cmp_info);
hi_bool vpss_policy_check_in_frame_cmp(hi_drv_pixel_format pixel_format, hi_drv_compress_info cmp_info);
hi_bool vpss_policy_check_nr_cmp(hi_drv_pixel_format pixel_format, hi_drv_compress_info cmp_info);
hi_bool vpss_policy_check_nr_dcmp(hi_drv_pixel_format pixel_format, hi_drv_compress_info cmp_info);
hi_bool vpss_policy_support_zme_upsamp(hi_void);
hi_void vpss_policy_check_crop_rect(hi_drv_vpss_video_frame *frame, hi_drv_sample_type sample_type,
                                    hi_drv_rect *crop_rect);
hi_void vpss_policy_correct_port_rect(hi_drv_rect *input_reso, hi_bool use_crop_rect, hi_drv_rect *crop_rect,
    hi_drv_crop_rect *offset_rect, hi_drv_rect *real_crop_rect);
hi_bool vpss_policy_check_special_field(hi_drv_vpss_video_frame *image);

hi_bool vpss_policy_check_support_virtual_port(hi_void);
hi_bool vpss_policy_support_rota_alg(vpss_debug_info *global_dbg_ctrl, hi_u32 width, hi_u32 height);
void vpss_policy_get_nr_frf_cmp_info(hi_u32 width, hi_u32 height, hi_drv_compress_info *cmp_info);
hi_bool vpss_policy_support_logic_3d_detect_port(hi_void);
hi_void vpss_policy_revise_tran_frame_bitwidth(hi_drv_vpss_video_frame *src_frame);
hi_bool vpss_policy_support_lbx(hi_void);
hi_bool vpss_policy_support_rwzb(hi_void);
hi_bool vpss_policy_support_hdr(hi_void);
hi_bool vpss_policy_support_decountor(hi_void);
hi_bool vpss_policy_support_npu_wbc(hi_void);
hi_bool vpss_policy_support_cccl(hi_void);
hi_bool vpss_policy_support_lbd(hi_void);
hi_bool vpss_policy_support_dei(vpss_perfor_alg_ctrl vpss_alg_ctrl, vpss_policy_alg_input_info *policy_alg);
hi_bool vpss_policy_support_tnr(vpss_perfor_alg_ctrl vpss_alg_ctrl, vpss_policy_alg_input_info *policy_alg);
hi_bool vpss_policy_support_snr(vpss_perfor_alg_ctrl vpss_alg_ctrl, vpss_policy_alg_input_info *policy_alg);
hi_bool vpss_policy_support_db(vpss_perfor_alg_ctrl vpss_alg_ctrl, vpss_policy_alg_input_info *policy_alg);
hi_bool vpss_policy_support_dm(vpss_perfor_alg_ctrl vpss_alg_ctrl, vpss_policy_alg_input_info *policy_alg);
hi_bool vpss_policy_support_dm430(vpss_perfor_alg_ctrl vpss_alg_ctrl, vpss_policy_alg_input_info *policy_alg);
hi_bool vpss_policy_check_port_reso_limit(hi_drv_vpss_video_frame *src_frame, hi_drv_vpss_port_frame_cfg *port_cfg);
hi_void vpss_policy_distribute_out_meta_playcnt(hi_bool frame_index_repeat, hi_drv_vpss_video_frame *vpss_image);
hi_bool vpss_policy_support_fix_out_bitwidth10(hi_void);
hi_bool vpss_policy_check_need_csc(hi_drv_color_descript *in_color, hi_drv_color_descript *out_color);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif





