/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __FRD_ALG_H__
#define __FRD_ALG_H__

#include "vfmw.h"

/* ----------------------------EXTERNAL MACRO---------------------------------- */
#define FRD_IN_PTS_SEQUENCE_LENGTH (100)

#define FRD_ABS(x)   (((x) < 0) ? -(x) : (x))
#define FRD_ROUND(x) (((x % 10) > 4) ? (x / 10 + 1) * 10 : x)

/* ----------------------------EXTERNAL DEFINITION----------------------------- */
typedef struct {
    hi_u32 pts_unit;
    hi_u32 cur_pts_ptr; /* pointer of past pts information, pointing to the oldest frame rate in record */
    hi_u32 length;
    hi_u32 que_cnt;
    hi_u32 que_rate;
    hi_u32 que_stable;
    hi_u32 unable_time;
    hi_u32 in_frame_rate_last; /* last input frame rate */
    hi_u32 stable_threshold;
    /* counter of stable frame rate, to avoid display shake caused by shake of frame rate */
    hi_u32 in_frame_rate_eque_time;
    hi_u32 in_frame_rate;
    hi_s64 cal_pts;
    hi_s64 in_pts_sqn[FRD_IN_PTS_SEQUENCE_LENGTH]; /* past pts information */

    /* for sample frame rate detect */
    hi_bool sample_frd_enable;
    hi_u32 sample_max_rate;
    hi_u32 sample_min_rate;
    hi_u32 sample_avg_rate;
    hi_u32 sample_act_num;
    hi_u32 sample_max_num;
    hi_u32 sample_last_frame_rate;
    hi_s64 sample_pts_info[FRD_IN_PTS_SEQUENCE_LENGTH]; /* past pts information */
} frd_alg_info;

/* ----------------------------EXTERNAL FUNCTION------------------------------- */
hi_void frd_reset(frd_alg_info *frd_info, hi_u32 pts_num);
hi_void frd_round_frm_rate(hi_u32 frame_rate, hi_u32 *new_frame_rate);
hi_s64 frd_divide(hi_s64 dividend, hi_u32 divider);
hi_u32 frd_frame_rate_detect(frd_alg_info *frd_info, hi_s64 pts);
hi_u32 frd_sample_frm_rate_detect(frd_alg_info *frd_info, hi_s64 pts);

#endif


