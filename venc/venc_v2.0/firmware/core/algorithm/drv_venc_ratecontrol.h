/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-08-13
 */
#ifndef __DRV_VENC_RATECONTROL_H__
#define __DRV_VENC_RATECONTROL_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#define RC_RECODE_USE   0
#define RC_FIRST_I_RECODE 0

#define GOP_THRESHOLD 1000
#define MODIFY_MINQP_FRAMENUM 200
#define ENLARGEMENT_FACTOR 6
#define MAX_INTER_LOADSIZE 5
#define MAX_INTRA_LOADSIZE 2

#define VENC_RC_FATAL(fmt...)
#define VENC_RC_ERR(fmt...)
#define VENC_RC_WARN(fmt...)
#define VENC_RC_INFO(fmt...)
#define VENC_RC_DBG(fmt...)

#ifdef VENC_UT_ENABLE
#define RC_PRIVATE
#else
#define RC_PRIVATE static
#endif

typedef struct {
    hi_u32 get_frame_num_try;
    hi_u32 put_frame_num_try;
    hi_u32 get_stream_num_try;
    hi_u32 put_stream_num_try;

    hi_u32 get_frame_num_ok;
    hi_u32 put_frame_num_ok;
    hi_u32 get_stream_num_ok;
    hi_u32 put_stream_num_ok;
    hi_u32 check_buf_size_num_fail;
    hi_u32 start_one_frame_fail;
    hi_u32 rc_calculate_fail;

    hi_u32 get_stream_buf_try;
    hi_u32 get_stream_buf_ok;
    hi_u32 buf_full_num;
    hi_u32 frm_rc_ctrl_skip;
    hi_u32 same_pts_skip;
    hi_u32 quick_encode_skip;
    hi_u32 too_few_buffer_skip;
    hi_u32 err_cfg_skip;
    hi_u32 too_many_bits_skip;

    hi_u32 queue_num;
    hi_u32 dequeue_num;                  /* OMX Channel not use this data */
    hi_u32 stream_queue_num;              /* just OMX Channel use this data */
    hi_u32 msg_queue_num;
    hi_u32 etb_cnt;
    hi_u32 ebd_cnt;
    hi_u32 ftb_cnt;
    hi_u32 fbd_cnt;

    hi_u32 used_stream_buf;

    hi_u32 stream_total_byte;

    hi_u32 real_send_input_frm_rate;       /* use to record curent Input FrameRate in use */
    hi_u32 real_send_output_frm_rate;      /* use to record curent Output FrameRate in use */
    hi_u32 frame_type;

    hi_u32 total_pic_bits;
    hi_u32 total_encode_num;
    hi_u32 rate_control_ratio;           /* use to know whether the current bit rate is controlled */
    hi_u32 recode_num;
} vedu_state_info;

#define MOVE_JUDGE_SIZE     8
#define MOVE_JUDGE_INTRA    2
typedef struct {
    hi_u32 skip_frame;
    hi_u32 min_qp;
    hi_u32 max_qp;
    hi_u32 pic_width;           /* done for 16 aligned @ venc */
    hi_u32 pic_height;          /* done for 16 aligned @ venc */
    hi_u32 rc_start;

    hi_u32 check_buf_size;
    hi_u32 pic_bits;

    hi_u32 mean_qp;
    hi_u32 mhb_bits;
    hi_u32 txt_bits;
    hi_u32 madi_val;
    hi_u32 madp_val;
    hi_u32 min_still_percent;

    hi_u32 vo_frm_rate;
    hi_u32 vi_frm_rate;
    hi_u32 bit_rate;
    hi_u32 gop;
    hi_u32 gop_type;
    hi_u32 b_frm_num;
    hi_u32 pic_type;
    hi_u32 is_intra_pic;
    hi_u32 rcn_idx;
    hi_u32 pme_info_idx;
    hi_u32 h264_frm_num;
    hi_u32 venc_buf_full;
    hi_u32 venc_p_bit_overflow;
    hi_s32 low_delay_mode;
    hi_u32 frm_rate_sata_error_flag;
    hi_u32 i_frm_insert_flag;

    hi_u32 frm_num_in_gop; /* frame number in gop minus 1 */
    hi_u32 frm_num_seq; /* frame number in sequence */

    /*******bits***********/
    hi_u64 gop_bits; /* bits in gop */
    hi_u32 average_frame_bits; /* average bits of one frame */

    /* <CNcomment: 统计前面若干帧 */
    hi_bool is_avbr;
    hi_u32 tar_bit_rate;
    hi_u32 tar_gop_bits;
    hi_u32 k_bits_per_gop;
    hi_u32 max_still_qp;

    hi_u32 movement_judge[2][MOVE_JUDGE_SIZE];   /* 2 0:  Intra;   1: Inter */
    hi_s32 movement_judge_qp[MOVE_JUDGE_SIZE];
    hi_u32 movement_judge_mb_h[MOVE_JUDGE_SIZE];
    hi_u32 movement_judge_res[MOVE_JUDGE_SIZE];
    hi_u32 res_percent;
    hi_s32 last_percent; /* in [1, 255] */

    hi_u32 movement_judge_madp[MOVE_JUDGE_SIZE];
    hi_u32 movement_judge_madi[MOVE_JUDGE_SIZE];

    hi_s32 gop_pointer;
    hi_s32 gop_size;
    hi_u64 gop_total;         /* GOP based bitrate */
    hi_u64 second_total;    /* frame based bitrate */

    hi_s32 inst_bit_rate;   /* <CNcomment: 按整个gop计算的瞬时码率 */
    hi_s32 inst_bit_rate2;  /* <CNcomment: 按最近5帧计算的瞬时码率 */

    /* only for debug */
    hi_s32 qp_delta0;
    hi_s32 qp_delta1;
    hi_s32 qp_delta3;
    hi_s32 qp_delta4;
    hi_s32 qp_delta5;
    hi_u32 tar_percen3;
    hi_u32 tar_percen2;
    hi_u32 tar_percen;

    /* every picure  */
    hi_u32 bits_statics_gop[GOP_THRESHOLD];   /* save one gop */

    hi_u32 i_pre_qp; /* Qp of previous six I frames */
    hi_u32 p_pre_qp;
    hi_u32 pre_target_bits;
    hi_u32 pre_pic_bits;

    /*********IMB***********/
    hi_s32 num_i_mb_current_frm; /* number of I MB in current  frame */
    hi_s32 i_mb[6]; /* 6 number of I MB of previous six frames */
    hi_s32 ave_of_i_mb; /* average number of I MB of previous six frames */

    /**********scence change detect *************/
    hi_u32 i_mb_ratio_current_scence[6]; /* 6 number of I MB of previous six frames just for scence change judge */

    /************RC Out**************/
    hi_u32 current_qp;
    hi_u32 target_bits;
    hi_u32 start_qp;
    hi_u32 initial_qp;

    /**********parameter set************/
    hi_s32 min_time_of_p;
    hi_s32 max_time_of_p;
    hi_s32 delta_time_of_p;
    hi_s32 i_qp_delta;
    hi_s32 p_qp_delta;

    hi_s32 current_time_of_p[2];    /* 2: array size 0:Intra    1:Inter */
    hi_s32 ip_qp_delta;
    hi_u32 water_line_init;
    hi_u32 water_line;
    hi_u32 last_frame_type_is_intra;
    /**********I/P bits set************/
    hi_u32 i_pic_bits[2];  /* 2: array size */
    hi_u32 p_pic_bits[5]; /* 5: array size */
    hi_u32 i_pic_bits_head;
    hi_u32 p_pic_bits_head;

    hi_u32 recode_cnt;         /* 1~n: frame need to be recoded(RecodeTime)   0: not need; */
    hi_u32 restart_qp;
    hi_u32 recode_flag;
    /**********instant bits set************/
    hi_u32 inst_bits[65];     /* 65 the instant bitrate of last frame */
    hi_u32 inst_bits_len;
    hi_u32 inst_bits_sum;
    hi_u32 inst_bits_head;
    hi_u32 per_inst_bits_sum;
    hi_u32 last_len;
    hi_u32 last_gop;
    hi_u32 last_bit_rate;
    /**********VBR set************/
    hi_bool is_vbr_mode;
    hi_u32 vbr_sum_of_qp;
    hi_s32 vbr_sum_of_delta_pic_bits;
    hi_s32 last_sum_of_delta_pic_bits;
    hi_u32 vbr_min_qp;
    hi_u32 vbr_max_bit_rate;

    hi_u32 last_sec_frame_cnt;
    hi_u32 last_sec_frame_cnt_for_usr;
} vedu_rc;

hi_s32 venc_drv_rc_open_one_frame(vedu_rc *pstRc);
hi_void VENC_DRV_RcCloseOneFrm(vedu_rc *pstRc);
hi_s32 venc_drv_rc_average(hi_s32* pData, hi_s32 n);
hi_void venc_drv_rc_update_info(vedu_rc* pstRc);
hi_s32 venc_drv_rc_is_need_recoding(vedu_rc* pstRc);
hi_s32 venc_drv_rc_start_qp_change(vedu_rc* pstRc);
hi_s32 venc_drv_calculate_rc_ratio(hi_u32 CurInstBits, hi_u32 bit_rate);

#ifdef VENC_UT_ENABLE
hi_void vedu_drv_rc_of_first_frame(vedu_rc* pstRc);
hi_u32  vedu_drv_rc_init_qp(hi_u32 bits, hi_u32 w, hi_u32 h);
hi_s32 vedu_drv_rc_modify_qp_for_large_frame_bits(vedu_rc* pstRc);
hi_s32 vedu_drv_rc_calculate_qp_delta_by_frame_bits(vedu_rc* pstRc);
hi_s32 vedu_drv_rc_calculate_qp_delta_by_instant_bits(vedu_rc* pstRc);
hi_s32 vedu_drv_rc_calculate_qp_delta_by_bit_rate(hi_u32 u32CurInstBits, hi_u32 bit_rate);
hi_s32 vedu_drv_rc_calculate_qp_delta_by_water_line(vedu_rc* pstRc);
hi_void vedu_drv_rc_calculate_current_qp(vedu_rc* pstRc);
hi_s32 vedu_drv_rc_update_frame_bits(vedu_rc* pstRc);
hi_s32 vedu_drv_rc_calculate_time_of_p(vedu_rc* pstRc);
hi_void vedu_drv_rc_modify_info_for_frame_rate_change(vedu_rc* pstRc);
hi_void vedu_drv_rc_update_instant_bits(vedu_rc* pstRc);
hi_void vedu_drv_rc_get_target_frame_bits(vedu_rc* pstRc);
#endif

#endif
