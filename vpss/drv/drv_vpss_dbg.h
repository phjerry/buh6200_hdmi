/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: drv_vpss_dbg.h hander file vpss debug define
 * Author: zhangjunyu
 * Create: 2016/07/01
 */
#ifndef __DRV_VPSS_DBG_H__
#define __DRV_VPSS_DBG_H__
#include "vpss_comm.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#define DEF_DBG_SRC_ID      0xffffffff
#define DEF_DBG_PORT0_ID    0x0
#define DEF_DBG_PORT1_ID    0x1
#define DEF_DBG_PORT2_ID    0x2
#define DEF_FILE_NAMELENGTH VPSS_FILE_NAMELENGTH

#define DEF_DBG_ROTA_MAX    3

typedef enum {
    VPSS_DBG_PATTERN_WIDTH_1PIX = 0,
    VPSS_DBG_PATTERN_WIDTH_2PIX,
    VPSS_DBG_PATTERN_WIDTH_MAX
} vpss_dbg_pattern_width;

typedef enum {
    VPSS_DBG_PATTERN_MODE_STATIC = 0,
    VPSS_DBG_PATTERN_MODE_RANDOM,
    VPSS_DBG_PATTERN_MODE_DYNAMIC,
    VPSS_DBG_PATTERN_MODE_MAX
} vpss_dbg_pattern_mode;

typedef enum {
    VPSS_IN_QUEUE_FRAME_TYPE = 0,
    VPSS_IN_DEQUEUE_FRAME_TYPE,
    VPSS_OUT_ACQUIRE_FRAME_TYPE,
    VPSS_OUT_RELEASE_FRAME_TYPE,
    VPSS_OUT_QUEUE_FRAME_TYPE,
    VPSS_OUT_DEQUEUE_FRAME_TYPE,
    VPSS_FRAME_TYPE_MAX
} vpss_debug_type;

typedef enum {
    VPSS_INPUT_TUNL_TYPE = 0,
    VPSS_OUTPUT_TUNL_TYPE,
    VPSS_TUNL_TYPE_MAX
} vpss_debug_tunl_type;

typedef enum {
    VPSS_REFCNT_INCREASE_TYPE = 0,
    VPSS_REFCNT_DECREASE_PRO_TYPE,
    VPSS_REFCNT_DECREASE_TRAN_TYPE,
    VPSS_REFCNT_DECREASE_UNPRO_TYPE,
    VPSS_REFCNT_TYPE_MAX
} vpss_debug_ref_count_type;

typedef struct {
    hi_bool ptn_en;
    vpss_dbg_pattern_width line_width;
    vpss_dbg_pattern_mode tst_pat_mode;
    hi_u32 tst_pat_speed;
    hi_u32 tst_pat_color;
} vpss_dbg_testptn;

typedef struct {
    hi_bool save_in_frame;
    hi_bool save_out_frame[4];  // 4 is the array size
    hi_bool print_src_info;
    hi_bool print_out_info[4];  // 4 is the array size
    hi_bool dump_reg;
    hi_bool vpss_bypass;
    hi_bool pq_bypass;
    hi_bool close_cmp;
    hi_bool set_prog;
    hi_bool set_me_version;
    hi_bool set_me_scan_num;
    hi_bool set_interlaced;
    hi_bool print_vo_info;
    hi_bool print_pq_info;
    hi_bool print_memc_info;
    hi_bool set_pause;
    hi_bool print_frame_addr;
    hi_u32 src_dbg_times;
    hi_u32 out_dbg_times[4];  // 4 is the array size
    hi_u32 me_version;
    hi_u32 me_scan_num;

    hi_u32 set_src_width;
    hi_u32 set_src_height;

    hi_u32 rot_alg_width;
    hi_u32 rot_alg_height;

    hi_bool set_rota_en[DEF_DBG_ROTA_MAX];
    hi_drv_vpss_rotation rotation[DEF_DBG_ROTA_MAX];

    vpss_dbg_testptn cf_pattern;
    vpss_dbg_testptn out_pattern;
    hi_bool print_tunl_info;
    hi_bool print_ref_cnt_info;
    hi_bool in_tunl;
    hi_bool out_tunl;
    hi_bool in_wtunl_enable;
    hi_bool out_wtunl_enable;
    hi_bool set_prog_inter_en;
    hi_bool set_buff_num;
    hi_u32 buff_num;
} vpss_debug_info;

hi_s32 vpss_dbg_print_frame(vpss_debug_info *dbg_info, hi_drv_vpss_video_frame *frame, hi_u32 dbg_part);
hi_s32 vpss_dbg_save_frame(vpss_debug_info *dbg_info, hi_drv_vpss_video_frame *frame, hi_u32 dbg_part);
hi_s32 vpss_dbg_print_frame_addr(vpss_debug_info *dbg_info, hi_drv_video_frame *frame, vpss_debug_type type);
hi_void vpss_dbg_print_tunl_info(vpss_debug_info *dbg_info, hi_drv_video_frame *frame,
                                 vpss_debug_tunl_type type);
hi_void vpss_dbg_print_ref_count_info(vpss_debug_info *dbg_info, hi_drv_vpss_video_frame *frame,
                                      vpss_debug_ref_count_type type);
hi_void vpss_dbg_print_ref_count_info_comm_frame(vpss_debug_info *dbg_info, hi_drv_video_frame *frame,
    vpss_debug_ref_count_type type);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif




