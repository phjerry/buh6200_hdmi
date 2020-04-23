/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/01
 */

#ifndef __HAL_VPSS_H__
#define __HAL_VPSS_H__

#include "vpss_comm.h"
#include "hal_vpss_stt.h"
#include "hal_vpss_wbc.h"
#include "drv_vpss_dbg.h"
#include "hi_reg_vpss.h"
#include "hal_vpss_reg_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    VPSS_HAL_SMMU_INT_TYPE_TBL_MISS = 1,
    VPSS_HAL_SMMU_INT_TYPE_PTW_TRANS = 2,
    VPSS_HAL_SMMU_INT_TYPE_TBL_R_INVALID = 4,
    VPSS_HAL_SMMU_INT_TYPE_TBL_W_INVALID = 8,
    VPSS_HAL_SMMU_INT_TYPE_ALLTYPE = 15,
    VPSS_HAL_SMMU_INT_TYPE_MAX
} vpss_hal_smmu_int_type;

typedef enum {
    VPSS_HAL_NODE_2D_FRAME = 0,
    VPSS_HAL_NODE_2D_4FIELD,
    VPSS_HAL_NODE_2D_5FIELD,
    VPSS_HAL_NODE_2D_3FIELD,
    VPSS_HAL_NODE_3D_FRAME_R,      // 用于配置读取偏移，在解码源为SBS/TAB，暂时不考虑拆分之后还有隔行的情况
    VPSS_HAL_NODE_PZME,            // 对应隔行，单场的源的类型
    VPSS_HAL_NODE_UHD,             // 4K*2K场景，后面看是否有UHD非标的特殊场景，再增加类型
    VPSS_HAL_NODE_UHD_HIGH_SPEED,  // 4K*2K场景，ATV 4k@50 4k@60场景,一拍两像素配置
    VPSS_HAL_NODE_3DDET,           // 3D检测通路，只需要Y分量
    VPSS_HAL_NODE_ZME_2L,          // 2级缩放节点
    VPSS_HAL_NODE_ROTATION_Y,
    VPSS_HAL_NODE_ROTATION_C,

    VPSS_HAL_NODE_8K,       /* 4ppc */
    VPSS_HAL_NODE_ONLY_MANR_DBG, /* all pq off */
    VPSS_HAL_NODE_MAX
} vpss_hal_node_type;

typedef struct {
} vpss_hal_alg_state;

typedef enum {
    VPSS_HAL_3DRS_TYPE_V3 = 0,
    VPSS_HAL_3DRS_TYPE_V4,
    VPSS_HAL_3DRS_TYPE_V5,
    VPSS_HAL_3DRS_TYPE_MAX
} vpss_hal_3drs_type;

typedef struct {
    hi_bool yuv;
    hi_u32 yh_ratio;
    hi_u32 ch_ratio;
    hi_u32 yv_ratio;
    hi_u32 cv_ratio;
} vpss_hal_zme_param;

typedef struct {
    hi_bool smmu_bypass;
    hi_u32 rch_cfg;
    hi_u32 wch_cfg;
} vpss_smmu_cfg;

typedef enum {
    VPSS_HAL_RMODE_PROGRESSIVE = 1,
    VPSS_HAL_RMODE_INTER_TOP = 2,
    VPSS_HAL_RMODE_INTER_BOTTOM = 3,
    VPSS_HAL_RMODE_PROG_TOP = 4,
    VPSS_HAL_RMODE_PROG_BOTTOM = 5,
    VPSS_HAL_RMODE_MAX
} vpss_hal_rmode;

typedef enum {
    VPSS_HAL_FRAME_TYPE_FIRST = 0,
    VPSS_HAL_FRAME_TYPE_NORMAL,
    VPSS_HAL_FRAME_TYPE_LAST,
    VPSS_HAL_FRAME_TYPE_MAX,
} vpss_hal_frame_type;

typedef struct {
    hi_bool secure;
    hi_bool progressive;
    hi_bool vc1_en;
    hi_drv_3d_type frm_type;
    vpss_hal_rmode rd_mode;
    hi_u32 rect_x;
    hi_u32 rect_y;
    hi_u32 rect_w;
    hi_u32 rect_h;
    hi_drv_pixel_format pixel_format;
    hi_drv_compress_info cmp_info;
    hi_drv_field_mode field_mode;

    hi_drv_vpss_vid_frame_addr hal_addr;
    hi_drv_vpss_vid_frame_addr hal_addr_lb;

    hi_drv_pixel_bitwidth bit_width;
    hi_u64 tunnel_addr;
    hi_u64 w_tunnel_addr;
    hi_drv_vc1_range_info vc1_info;
    hi_u8 *vir_addr_y;
    hi_u8 *vir_addr_c;
    hi_u32 buff_size;
    hi_drv_source source;
    hi_bool single_filed;
    hi_u32 hw_index;
    hi_u32 src_id;
    hi_bool top_first;

    hi_drv_hdr_type src_hdr_type; /* src fill */
    hi_drv_color_descript src_color_desp;   /* src fill */
    hi_drv_hdr_type out_hdr_type; /* port fill */
    hi_drv_color_descript out_color_desp;   /* port fill */
} vpss_hal_frame;

typedef struct {
    hi_u64 phy_addr;
    hi_u8 *vir_addr;
    hi_u32 width;
    hi_u32 height;
    hi_u32 stride;
    hi_u32 size;
} vpss_hal_stt_channel_cfg;

typedef struct {
    vpss_hal_stt_channel_cfg r_stt_chn;
    vpss_hal_stt_channel_cfg w_stt_chn;
    vpss_hal_stt_channel_cfg w_me1_stt_chn;
    vpss_hal_stt_channel_cfg w_me2_stt_chn;
} vpss_hal_stt_soft_cfg;

typedef struct {
    vpss_hal_stt_channel_cfg r_mad_y_chn;
    vpss_hal_stt_channel_cfg w_mad_y_chn;
    vpss_hal_stt_channel_cfg r_mad_c_chn;
    vpss_hal_stt_channel_cfg w_mad_c_chn;
} vpss_hal_stt_cccl_cfg;

typedef struct {
    vpss_hal_stt_channel_cfg r_mad_chn;
    vpss_hal_stt_channel_cfg w_mad_chn;
    vpss_hal_stt_channel_cfg r_sad_y_chn;
    vpss_hal_stt_channel_cfg w_sad_y_chn;
    vpss_hal_stt_channel_cfg r_sad_c_chn;
    vpss_hal_stt_channel_cfg w_sad_c_chn;
    vpss_hal_stt_channel_cfg r_me_mv_p1_chn;
    vpss_hal_stt_channel_cfg r_me_mv_p2_chn;
    vpss_hal_stt_channel_cfg r_me_mv_p3_chn;
} vpss_hal_stt_dei_cfg;

typedef struct {
    vpss_hal_stt_channel_cfg r_cur_chn;  // not solo data
    vpss_hal_stt_channel_cfg r_ref_chn;  // not solo data
    vpss_hal_stt_channel_cfg r_mv_p1_chn;
    vpss_hal_stt_channel_cfg r_mv_pr_chn;
    vpss_hal_stt_channel_cfg w_mv_chn;
    vpss_hal_stt_channel_cfg rg_mv_p1_chn;
    vpss_hal_stt_channel_cfg rg_mv_pr_chn;
    vpss_hal_stt_channel_cfg wg_mv_chn;
    vpss_hal_stt_channel_cfg r_rgmv_cf_chn;
    vpss_hal_stt_channel_cfg r_rgmv_p1_chn;
    vpss_hal_stt_channel_cfg r_rgmv_p2_chn;
    vpss_hal_stt_channel_cfg w_me_for_nr_stt_chn;
    vpss_hal_stt_channel_cfg w_me_for_di_stt_chn;
} vpss_hal_stt_me_cfg;

typedef struct {
    vpss_hal_stt_channel_cfg r_tnr_mad_chn;
    vpss_hal_stt_channel_cfg r_snr_mad_chn;
    vpss_hal_stt_channel_cfg w_mad_chn;
    vpss_hal_stt_channel_cfg r_cnt_chn;
    vpss_hal_stt_channel_cfg w_cnt_chn;
    vpss_hal_stt_channel_cfg r_rgmv_p1_chn;
    vpss_hal_stt_channel_cfg r_rgmv_p2_chn;
    vpss_hal_stt_channel_cfg r_memv_for_tnr_chn;
} vpss_hal_stt_nr_cfg;

typedef struct {
    vpss_hal_stt_channel_cfg r_cnt_chn;
    vpss_hal_stt_channel_cfg w_cnt_chn;
} vpss_hal_stt_dm430_cfg;

typedef struct {
    vpss_hal_stt_channel_cfg r_rgmv_p1_chn;
    vpss_hal_stt_channel_cfg r_rgmv_p2_chn;
    vpss_hal_stt_channel_cfg w_rgmv_chn;
    vpss_hal_stt_channel_cfg r_prjh_chn;
    vpss_hal_stt_channel_cfg w_prjh_chn;
    vpss_hal_stt_channel_cfg r_prjv_chn;
    vpss_hal_stt_channel_cfg w_prjv_chn;
} vpss_hal_stt_rgme_cfg;

typedef struct {
    vpss_hal_stt_soft_cfg global_cfg;
    vpss_hal_stt_cccl_cfg cccl_cfg;
    vpss_hal_stt_dei_cfg dei_cfg;
    vpss_hal_stt_me_cfg me_cfg;
    vpss_hal_stt_nr_cfg nr_cfg;
    vpss_hal_stt_dm430_cfg dmcnt_cfg;
    vpss_hal_stt_rgme_cfg rgme_cfg;

    hi_u64 cf_rgmv_phy_addr;
} vpss_hal_stt_cfg;

typedef struct {
    vpss_hal_frame w_frame;
    vpss_hal_frame rp1_frame;
    vpss_hal_frame rp2_frame;
    vpss_hal_frame rp3_frame;
} vpss_hal_wbc_nr_cfg;

typedef struct {
    vpss_hal_frame w_frame;
    vpss_hal_frame rp2_frame;
    vpss_hal_frame rp4_frame;
    vpss_hal_frame rp8_frame;
} vpss_hal_wbc_cccl_cfg;

typedef struct {
    vpss_hal_frame w_frame;
    vpss_hal_frame r_frame;
} vpss_hal_wbc_dei_cfg;

typedef struct {
    vpss_hal_frame w_frame;
    vpss_hal_frame rp2_frame;
} vpss_hal_wbc_hds_cfg;

typedef struct {
    vpss_hal_frame w_frame;
    vpss_hal_frame rp2_frame;
} vpss_hal_wbc_hvds_cfg;

typedef struct {
    vpss_hal_wbc_nr_cfg wbc_nr_cfg;
    vpss_hal_wbc_cccl_cfg wbc_cccl_cfg;
    vpss_hal_wbc_dei_cfg wbc_dei_cfg;
    vpss_hal_wbc_hds_cfg wbc_hds_cfg;
    vpss_hal_wbc_hvds_cfg wbc_hvds_cfg;
} vpss_hal_wbc_cfg;

typedef struct {
    hi_bool enable;
    hi_bool config;
    hi_drv_vpss_port_type port_type;
    hi_bool need_flip;
    hi_bool need_mirror;
    hi_bool uv_change;
    hi_bool need_hdr;
    hi_drv_rect in_crop_rect;
    hi_drv_rect video_rect;
    hi_drv_vpss_rotation rotation;
    vpss_hal_frame out_info;
} vpss_hal_port_info;

typedef enum {
    VPSS_HAL_FIRST_NODE = 0,
    VPSS_HAL_SECOND_NODE,
    VPSS_HAL_THIRD_NODE,
    VPSS_HAL_NODE_NUM_MAX
} vpss_hal_node_num;

typedef struct {
    vpss_hal_node_num node_num;
    hi_bool vc1_en;
    hi_bool scd_en;
    hi_bool cccl_en;
    hi_bool tnr_en;
    hi_bool rgme_en;
    hi_bool dei_en;
    hi_bool snr_en;
} vpss_alg_enable;

typedef struct {
    hi_bool use_pq_module;
    hi_bool need_src_flip;
    hi_u32 complete_cnt;
    hi_u32 inst_id;
    hi_u32 port_id;

    vpss_hal_3drs_type en3_drs;
    hi_u32 me_scan_num;
    vpss_perfor_alg_ctrl vpss_alg_ctrl;

    vpss_hal_node_type node_type;
    vpss_hal_frame_type in_frame_type;
    vpss_reg_type pq_reg_data;
    hi_drv_pq_vpss_alg_enable alg_ctrl;
    vpss_smmu_cfg smmu_cfg;
    vpss_hal_frame in_info;
    vpss_hal_stt_cfg stt_cfg;
    vpss_hal_wbc_cfg wbc_cfg;
    vpss_hal_port_info port_info[VPSS_PORT_MAX_NUM];
    hi_bool port_used[VPSS_REG_PORT_MAX];

    vpss_dbg_testptn cf_pattern;
    vpss_dbg_testptn out_pattern;
    hi_bool tunl_en;
    hi_bool in_wtunl_en;
    hi_bool in_rtunl_en;
    hi_bool out_wtunl_en;
    hi_bool out_rtunl_en;
    hi_u64 pre_frame_addr;
} vpss_hal_info;

hi_void vpss_hal_init_hal_info(vpss_hal_info *hal_info);
hi_s32 vpss_hal_init(vpss_ip ip);
hi_s32 vpss_hal_deinit(vpss_ip ip);
hi_s32 vpss_hal_smmu_init(vpss_ip ip);
hi_s32 vpss_hal_smmu_deinit(vpss_ip ip);
hi_s32 vpss_hal_sys_init(vpss_ip ip, hi_bool init_en);
hi_s32 vpss_hal_get_int_state(vpss_ip ip, hi_u32 *int_state);
hi_s32 vpss_hal_process_interrupt(vpss_ip ip);
hi_s32 vpss_hal_check_int_state(vpss_ip ip);
hi_s32 vpss_hal_clear_int_state(vpss_ip ip, hi_u32 int_state);
hi_s32 vpss_hal_get_smmu_int_state(vpss_ip ip, hi_u32 *int_state);
hi_s32 vpss_hal_clear_smmu_int_state(vpss_ip ip, hi_u32 int_state);
hi_s32 vpss_hal_get_alg_cfg(vpss_ip ip, vpss_hal_alg_state *alg_state);
hi_s32 vpss_hal_set_alg_cfg(vpss_ip ip, vpss_hal_alg_state *alg_state);
hi_s32 vpss_hal_set_node_info(vpss_ip ip, vpss_hal_info *hal_info);
hi_s32 vpss_hal_start_logic(vpss_ip ip);
hi_s32 vpss_hal_complete_logic(vpss_ip ip);
hi_s32 vpss_hal_dump_reg(vpss_ip ip);
hi_void vpss_hal_golden_write_chn_file(hi_void);
hi_void vpss_hal_get_lbd_info(hi_u8 *vir_addr, hi_drv_crop_rect *lbx_info, hi_drv_crop_rect *lbx_shift_info);
hi_s32 vpss_hal_get_cmp_size(vpss_ip ip, hi_u32 *cmp_size);
#ifdef DPT
hi_void vpss_hal_hwbuff_reset(vpss_ip ip, hi_bool queue_reset);
hi_s32 vpss_hal_hwbuff_cfg(vpss_ip ip, vpss_hal_info *hal_info);
hi_void free_all_split_node_buffer(vpss_ip ip);
hi_void vpss_hal_modify_complete(vpss_ip ip, vpss_hwbuff_chan hwbuff_chan, vpss_hwbuff_modify_type modify_type);
#endif
hi_s32 vpss_hal_clear_task_reset(vpss_ip ip);
hi_void vpss_hal_get_rgmv_addr(vpss_ip ip, hi_u64 *rgmv_addr);
vpss_reg_type *vpss_hal_get_reg_vir_addr(vpss_ip ip);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif




