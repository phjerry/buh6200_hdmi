/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_stt.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/01
 */

#ifndef __DRV_VPSS_STT_H__
#define __DRV_VPSS_STT_H__

#include "vpss_comm.h"
#include "hal_vpss_stt_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_bool interlace;
    hi_bool secure;
    hi_u32 width;
    hi_u32 height;
    hi_drv_source source;
    hi_drv_pixel_format pixel_format;
} vpss_stt_attr;

typedef struct {
    vpss_stt_channel_cfg r_stt_chn;
    vpss_stt_channel_cfg w_stt_chn;
    vpss_stt_channel_cfg w_me1_stt_chn;
    vpss_stt_channel_cfg w_me2_stt_chn;
} vpss_stt_soft_cfg;

typedef struct {
    vpss_stt_channel_cfg r_mad_y_chn;
    vpss_stt_channel_cfg w_mad_y_chn;
    vpss_stt_channel_cfg r_mad_c_chn;
    vpss_stt_channel_cfg w_mad_c_chn;
} vpss_stt_cccl_cfg;

typedef struct {
    vpss_stt_channel_cfg r_mad_chn;
    vpss_stt_channel_cfg w_mad_chn;
    vpss_stt_channel_cfg r_sad_y_chn;
    vpss_stt_channel_cfg w_sad_y_chn;
    vpss_stt_channel_cfg r_sad_c_chn;
    vpss_stt_channel_cfg w_sad_c_chn;
    vpss_stt_channel_cfg r_me_mv_p1_chn;
    vpss_stt_channel_cfg r_me_mv_p2_chn;
    vpss_stt_channel_cfg r_me_mv_p3_chn;
} vpss_stt_dei_cfg;

typedef struct {
    vpss_stt_channel_cfg r_cur_chn; /* not solo data */
    vpss_stt_channel_cfg r_ref_chn; /* not solo data */
    vpss_stt_channel_cfg r_mv_p1_chn;
    vpss_stt_channel_cfg r_mv_pr_chn;
    vpss_stt_channel_cfg w_mv_chn;
    vpss_stt_channel_cfg rg_mv_p1_chn;
    vpss_stt_channel_cfg rg_mv_pr_chn;
    vpss_stt_channel_cfg wg_mv_chn;
    vpss_stt_channel_cfg r_rgmv_cf_chn;
    vpss_stt_channel_cfg r_rgmv_p1_chn;
    vpss_stt_channel_cfg r_rgmv_p2_chn;
    vpss_stt_channel_cfg w_me_for_nr_stt_chn;
    vpss_stt_channel_cfg w_me_for_di_stt_chn;
} vpss_stt_me_cfg;

typedef struct {
    vpss_stt_channel_cfg r_tnr_mad_chn;
    vpss_stt_channel_cfg r_snr_mad_chn;
    vpss_stt_channel_cfg w_mad_chn;
    vpss_stt_channel_cfg r_cnt_chn;
    vpss_stt_channel_cfg w_cnt_chn;
    vpss_stt_channel_cfg r_rgmv_p1_chn;
    vpss_stt_channel_cfg r_rgmv_p2_chn;
    vpss_stt_channel_cfg r_memv_for_tnr_chn;
} vpss_stt_nr_cfg;

typedef struct {
    vpss_stt_channel_cfg r_cnt_chn;
    vpss_stt_channel_cfg w_cnt_chn;
} vpss_stt_dmcnt_cfg;

typedef struct {
    vpss_stt_channel_cfg r_rgmv_p1_chn;
    vpss_stt_channel_cfg r_rgmv_p2_chn;
    vpss_stt_channel_cfg w_rgmv_chn;
    vpss_stt_channel_cfg r_prjh_chn;
    vpss_stt_channel_cfg w_prjh_chn;
    vpss_stt_channel_cfg r_prjv_chn;
    vpss_stt_channel_cfg w_prjv_chn;
} vpss_stt_rgme_cfg;

typedef struct {
    vpss_stt_soft_cfg global_cfg;
    vpss_stt_cccl_cfg cccl_cfg;
    vpss_stt_dei_cfg dei_cfg;
    vpss_stt_me_cfg me_cfg;
    vpss_stt_nr_cfg nr_cfg;
    vpss_stt_dmcnt_cfg dmcnt_cfg;
    vpss_stt_rgme_cfg rgme_cfg;
} vpss_stt_cfg;

typedef struct {
    hi_bool init;
    vpss_stt_attr stt_attr;
    hi_void *contex;
    hi_u32 complete_count;
} vpss_stt;

hi_s32 vpss_stt_init(vpss_stt *wbc, vpss_stt_attr *attr);
hi_s32 vpss_stt_deinit(vpss_stt *wbc);
hi_s32 vpss_stt_reset(vpss_stt *wbc);
hi_s32 vpss_stt_complete(vpss_stt *wbc);
hi_s32 vpss_stt_get_cfg(vpss_stt *wbc, vpss_stt_cfg *stt_cfg);
hi_s32 vpss_stt_fill_stt_info(hi_void *fill_instance, hi_drv_3d_eye_type en3_dtype);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif





