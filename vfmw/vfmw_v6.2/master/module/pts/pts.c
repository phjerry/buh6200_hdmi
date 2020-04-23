/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "pts_ext.h"
#include "vfmw_osal.h"
#include "dbg.h"
#include "frd.h"
#include "vfmw_adpt.h"

/* -------------------------------LOCAL MACRO---------------------------------- */
#define PTS_MAX_CHAN_NUM        VFMW_CHAN_NUM
#define PTS_MAX_FRAME_RATE      120                 /* fps */
#define PTS_MIN_FRAME_RATE      24                  /* fps */
#define PTS_MAX_PASS_TIME       (120 * 1000 * 1000) /* us */
#define PTS_MAX_FRAME_TIME_CNT  2
#define PTS_MAX_FRAME_TIME_JUMP 1000
#define PTS_DEFAULT_SAMPLE_NUM  10

#define PTS_CMD_NUM 2
#define PTS_CMD_LEN 50

#define PTS_CHAN_NUM VFMW_CHAN_NUM

/* -----------------------------LOCAL DEFINITION------------------------------- */
typedef struct {
    hi_bool inited;
    hi_s64 last_src_pts;           /* Last valid src pts */
    hi_u32 last_src_frm_idx;       /* The index of last frame with a valid src pts */
    hi_u32 frame_num;              /* Total frame number */
    hi_u32 frame_time_cnt;         /* Frame interval counter */
    hi_s64 last_calc_frm_time;     /* Last calculated frame time(us) */
    hi_s64 calc_frame_time;        /* Calculated frame time(us) */
    hi_s64 set_frame_time;         /* Setted frame time(us) */
    hi_s64 ges_frame_time;         /* Guessed frame time(us) */
    hi_s32 inter_pts_delta;        /* interleaved source, use to adjust pts for field to frame */
    vfmw_pts_frmrate user_frame_rate; /* User config frame rate */
    frd_alg_info pts_info;
    hi_u32 force_pts_mode;
} pts_chan;

typedef struct {
    hi_bool inited;
    pts_chan *chan[PTS_CHAN_NUM];
    OS_SEMA sema;
} pts_entry;

/* ------------------------------LOCAL VALUE----------------------------------- */
static pts_entry g_pts_entry;
static hi_u32 g_cfg_sample_num = PTS_DEFAULT_SAMPLE_NUM;
static hi_u32 g_cur_sample_num = PTS_DEFAULT_SAMPLE_NUM;

/* ----------------------------INTERNAL FUNCTION------------------------------- */
static pts_entry *pts_get_entry(hi_void)
{
    return &g_pts_entry;
}

static pts_chan *pts_get_chan(hi_s32 chan_id)
{
    pts_entry *entry = HI_NULL;
    pts_chan *chan = HI_NULL;

    VFMW_ASSERT_RET(chan_id >= 0, HI_NULL);
    VFMW_ASSERT_RET(chan_id < PTS_CHAN_NUM, HI_NULL);

    entry = pts_get_entry();
    VFMW_ASSERT_RET(entry != HI_NULL, HI_NULL);

    chan = entry->chan[chan_id];

    return chan;
}

static hi_void pst_info_init_param(hi_s32 chan_id)
{
    hi_s32 i = 0;
    pts_chan *chan;

    chan = pts_get_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
    }

    chan->last_src_pts = VFMW_INVALID_PTS;
    chan->last_src_frm_idx = -1;
    chan->frame_num = 0;
    chan->frame_time_cnt = 0;
    chan->last_calc_frm_time = VFMW_INVALID_PTS;
    chan->calc_frame_time = VFMW_INVALID_PTS;
    chan->ges_frame_time = VFMW_INVALID_PTS;
    chan->inter_pts_delta = 0;
    chan->force_pts_mode = 0;
    chan->pts_info.cal_pts = VFMW_INVALID_PTS;
    chan->pts_info.que_cnt = 0;
    chan->pts_info.que_stable = 0;

    for (i = 0; i < FRD_IN_PTS_SEQUENCE_LENGTH; i++) {
        chan->pts_info.sample_pts_info[i] = VFMW_INVALID_PTS;
    }

    chan->pts_info.sample_max_rate = 0;
    chan->pts_info.sample_min_rate = 0;
    chan->pts_info.sample_avg_rate = 0;
    chan->pts_info.sample_act_num = 0;
    chan->pts_info.sample_max_num = g_cur_sample_num;
    chan->pts_info.sample_last_frame_rate = 0;

    return;
}

static hi_s64 pts_info_guess_frame_time(pts_chan *chan, vfmw_image *image)
{
    if (chan->user_frame_rate.en_frm_rate_type == PTS_FRMRATE_TYPE_STREAM && image->format.frame_rate_valid == 1) {
        chan->ges_frame_time = 1000000 / image->frame_rate * 1000; /* 1000000 1000 is unit */
    } else {
        if (chan->ges_frame_time == VFMW_INVALID_PTS) {
            if ((image->image_width <= 720) && (image->image_height <= 480)) { /* 720 480 is threshold */
                chan->ges_frame_time = 33333; /* 33333 MAYBE NTSC */
            } else {
                chan->ges_frame_time = 40000; /* 40000 default */
            }

            if (image->hdr_input.be_dv_hdr_chan == 1) {
                chan->ges_frame_time = 40000; /* 40000 for dolby */
            }
        }
    }

    return chan->ges_frame_time;
}

static hi_s64 pts_info_get_frame_time_user(pts_chan *chan, vfmw_image *image)
{
    hi_s64 frame_time = VFMW_INVALID_PTS;

    /* Choose frame_time from: 1.SetFrameTime 2.CalcFrameTime 3.GuessFrameTime */
    if (chan->set_frame_time != VFMW_INVALID_PTS) {
        frame_time = chan->set_frame_time;
    } else if (chan->calc_frame_time != VFMW_INVALID_PTS) {
        frame_time = chan->calc_frame_time;
    } else {
        frame_time = pts_info_guess_frame_time(chan, image);
    }

    /* the max pts difference(us) should be less than the 2 * value of pts difference
       of FrameRateMin(12fps) and current rate */
    /* 1000 :a number 1000000 :a number 12 :a number 2 :a number */
    if (image->frame_rate >= 1000 && frame_time > (1000000 / 12 * 2) &&
        /* 1000000 :a number 1000 :a number 2 :a number */
        frame_time > 1000000 / (image->frame_rate / 1000) * 2) {
        /* 1000000 :a number 1000 :a number */
        frame_time = 1000000 / (image->frame_rate / 1000);
    }

    return frame_time;
}

static hi_s64 pts_info_get_frame_time_other(pts_chan *chan, vfmw_image *image)
{
    hi_s64 frame_time = VFMW_INVALID_PTS;

    /* Choose frame_time from: 1.calc_frame_time 2.set_frame_time 3.GuessFrameTime */
    if (chan->calc_frame_time != VFMW_INVALID_PTS) {
        frame_time = chan->calc_frame_time;
    } else if (chan->set_frame_time != VFMW_INVALID_PTS) {
        frame_time = chan->set_frame_time;
    } else {
        frame_time = pts_info_guess_frame_time(chan, image);
    }

    /* the max pts difference(us) should be less then the 2*value of
     pts difference of FrameRateMin(12fps) and current rate */
    /* CNcomment: ptsֵ(us)ӦСС֡µptsĲֵ2Сڸ֡ʼptsĲֵ2 */
    /* 1000 :a number 1000000 :a number 12 :a number 2 :a number */
    if (image->frame_rate >= 1000 && frame_time > (1000000 / 12 * 2) &&
        /* 1000000 :a number 1000 :a number 2 :a number */
        frame_time > 1000000 / (image->frame_rate / 1000) * 2) {
        /* 1000000 :a number 1000 :a number */
        frame_time = 1000000 / (image->frame_rate / 1000);
    }

    return frame_time;
}

static hi_void pts_info_cal_frm_time(hi_s32 chan_id, pts_chan *chan, hi_u32 num_pass, hi_s64 pts_pass)
{
    hi_u32 pts_diff_step;
    hi_u64 frame_time_abs;
    pts_chan *tmp_chan;

    tmp_chan = pts_get_chan(chan_id);
    if (tmp_chan == HI_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
    }

    pts_diff_step = (chan->pts_info.pts_unit == 1) ? 1000 : 1; /* 1000 :a number */

    if (chan->calc_frame_time != VFMW_INVALID_PTS) {
        frame_time_abs = FRD_ABS(frd_divide(pts_pass * pts_diff_step, num_pass) - chan->last_calc_frm_time);
        if (frame_time_abs > PTS_MAX_FRAME_TIME_JUMP) {
            chan->frame_time_cnt = 0;
        }

        chan->frame_time_cnt++;

        if (chan->frame_time_cnt >= PTS_MAX_FRAME_TIME_CNT) {
            chan->calc_frame_time = frd_divide(pts_pass * pts_diff_step, num_pass);
            chan->frame_time_cnt = PTS_MAX_FRAME_TIME_CNT;
        }
    } else {
        chan->calc_frame_time = frd_divide(pts_pass * pts_diff_step, num_pass);
        chan->frame_time_cnt++;
    }

    chan->last_calc_frm_time = frd_divide(pts_pass * pts_diff_step, num_pass);

    tmp_chan->inter_pts_delta = frd_divide(chan->calc_frame_time, 2000); /* 2000 :a number */
}

static hi_void pts_info_handle_valid_src_pts(hi_s32 chan_id, vfmw_image *image)
{
    hi_u32 num_pass = 0;
    hi_s64 pts_pass = 0;
    hi_s64 src_pts;
    hi_s64 img_pts;
    pts_chan *chan = HI_NULL;
    pts_entry *entry;

    entry = pts_get_entry();
    OS_SEMA_DOWN(entry->sema);
    chan = pts_get_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "Get PtsInfo %d failed!\n", chan_id);
        OS_SEMA_UP(entry->sema);
        return;
    }

    src_pts = image->src_pts;

    if (chan->last_src_pts != VFMW_INVALID_PTS) {
        num_pass = chan->frame_num - chan->last_src_frm_idx;

        if (src_pts >= chan->last_src_pts) {
            pts_pass = src_pts - chan->last_src_pts;
        } else if (src_pts < chan->last_src_pts) {
            pts_pass = VFMW_MAX_PTS - chan->last_src_pts + src_pts;
        }

        /* calculate the frame time */
        if (num_pass) {
            if (pts_pass <= PTS_MAX_PASS_TIME) {
                pts_info_cal_frm_time(chan_id, chan, num_pass, pts_pass);
            } else {
                /* keep the calc_frame_time while pts is abnormal */
                /* dprint(PRN_ERROR, "pts %lld is abnormal!\n", src_pts); */
                if (src_pts >= chan->last_src_pts) {
                    chan->inter_pts_delta = 20; /* 20 :a number */
                } else {
                    chan->inter_pts_delta = -20; /* pts back and loopback   -20 :a number */
                }
            }
        } else {
            dprint(PRN_ERROR, "Frame index %d repeat\n", chan->frame_num);
            chan->inter_pts_delta = 20; /* 20 :a number */
        }
    } else {
        chan->inter_pts_delta = 20; /* 20 :a number */
    }

    chan->last_src_pts = src_pts;
    chan->last_src_frm_idx = chan->frame_num;
    img_pts = src_pts;

    image->pts = img_pts;
    image->inter_pts_delta = chan->inter_pts_delta;
    OS_SEMA_UP(entry->sema);

    return;
}

static hi_void pts_info_handle_invalid_src_pts(hi_s32 chan_id, vfmw_image *image)
{
    hi_u32 num_pass = 0;
    hi_u32 pts_diff_step;
    hi_s64 frame_time = VFMW_INVALID_PTS;
    hi_s64 img_pts;
    pts_chan *chan = HI_NULL;
    pts_entry *entry;

    entry = pts_get_entry();
    OS_SEMA_DOWN(entry->sema);
    chan = pts_get_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "Get PtsInfo %d failed!\n", chan_id);
        OS_SEMA_UP(entry->sema);
        return;
    }

    if (chan->last_src_pts != VFMW_INVALID_PTS) {
        num_pass = chan->frame_num - chan->last_src_frm_idx;

        pts_diff_step = (chan->pts_info.pts_unit == 1) ? 1000 : 1; /* 1000 :a number */

#if 0
        if (((enVdecType == HI_UNF_VCODEC_TYPE_REAL8) ||
             (enVdecType == HI_UNF_VCODEC_TYPE_REAL9))
            && (chan->set_frame_time != -1)) {
            img_pts = chan->last_src_pts + chan->set_frame_time / 1000; /* 1000 :a number */
            chan->last_src_pts = img_pts;
            image->pts = img_pts;
            return img_pts;
        }
#endif

        if (chan->user_frame_rate.en_frm_rate_type == PTS_FRMRATE_TYPE_USER) {
            frame_time = pts_info_get_frame_time_user(chan, image);
        } else {
            frame_time = pts_info_get_frame_time_other(chan, image);
        }

        img_pts = chan->last_src_pts + frd_divide((frame_time * num_pass), pts_diff_step);
        chan->inter_pts_delta = frd_divide(frame_time, 2000); /* 2000 :a number */
    } else {
        img_pts = VFMW_INVALID_PTS;
        chan->inter_pts_delta = 0;
    }

    image->pts = img_pts;
    image->inter_pts_delta = chan->inter_pts_delta;
    OS_SEMA_UP(entry->sema);

    return;
}

static hi_s32 pts_info_guess_first_frame_rate(hi_s32 chan_id, hi_u32 src_frame_rate, vfmw_image *image)
{
    hi_s32 ret;
    vfmw_pts_frmrate frm_rate;

    VFMW_CHECK_SEC_FUNC(memset_s(&frm_rate, sizeof(vfmw_pts_frmrate), 0x0, sizeof(vfmw_pts_frmrate)));

    ret = pts_info_get_frm_rate(chan_id, &frm_rate);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "Get frame rate failed!\n");

        return HI_FAILURE;
    }
    /* 12 :a number */
    if ((frm_rate.st_set_frm_rate.fps_integer >= 12) &&
        (frm_rate.st_set_frm_rate.fps_integer <= 60)) { /* 60 :a number */
        /* 1000 :a number */
        image->frame_rate = frm_rate.st_set_frm_rate.fps_integer * 1000 + frm_rate.st_set_frm_rate.fps_decimal;
    } else if ((src_frame_rate >= 12) && (src_frame_rate <= 60)) { /* 12 :a number 60 :a number */
        image->frame_rate = src_frame_rate * 1000; /* 1000 :a number */
    } else {
        if (image->image_width <= 720 && image->image_height <= 480) { /* 720 :a number 480 :a number */
            image->frame_rate = 30 * 1000; /* < NTSC, asume the framerate is 30fps  30 :a number 1000 :a number */
        } else if (image->image_width < 1280 && image->image_height < 720) { /* 1280 :a number 720 :a number */
            image->frame_rate = 25 * 1000; /* > NTSC, asume the framerate is 25fps  25 :a number 1000 :a number */
        } else {
            image->frame_rate = 30 * 1000; /* < NTSC, asume the framerate is 30fps  30 :a number 1000 :a number */
        }
    }

    return HI_SUCCESS;
}

static hi_void pts_info_get_round_frame_rate(hi_s32 chan_id, vfmw_image *image)
{
    hi_u32 round_rate = 0;
    pts_chan *chan;

    chan = pts_get_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
    }

    if (((image->rwzb == 0) &&
         (chan->user_frame_rate.en_frm_rate_type != PTS_FRMRATE_TYPE_STREAM) &&
         (chan->user_frame_rate.en_frm_rate_type != PTS_FRMRATE_TYPE_USER)) ||
        ((chan->user_frame_rate.en_frm_rate_type == PTS_FRMRATE_TYPE_STREAM) &&
         (image->frame_rate == 0))) {
        frd_round_frm_rate(((image->frame_rate / 1000) * 10), &round_rate); /* 1000 :a number 10 :a number */

        if (round_rate > 0) {
            image->frame_rate = round_rate * 1000 / 10; /* 1000 :a number 10 :a number */
        }
    }
}

static hi_void pts_info_get_first_frame_rate(hi_s32 chan_id, vfmw_image *image, frd_alg_info *frd_info,
                                             hi_u32 frame_rate)
{
    hi_u32 src_frame_rate;
    hi_u32 round_rate_hiplayer = 0;
    hi_u32 round_rate = 0;
    vfmw_pts_frmrate frm_rate;
    hi_s32 ret = 0;

    src_frame_rate = image->frame_rate;
    VFMW_CHECK_SEC_FUNC(memset_s(&frm_rate, sizeof(vfmw_pts_frmrate), 0x0, sizeof(vfmw_pts_frmrate)));

    if (image->frame_rate == 0) {
        if (frd_info->sample_max_num != 0) {
            frame_rate = frd_sample_frm_rate_detect(frd_info, image->pts);
            if ((frd_info->sample_act_num >= frd_info->sample_max_num) && (frame_rate == 0)) {
                pts_info_guess_first_frame_rate(chan_id, src_frame_rate, image);
            } else if ((frd_info->sample_act_num >= frd_info->sample_max_num) && (frame_rate != 0)) {
                frd_round_frm_rate(frame_rate, &round_rate);
                ret = pts_info_get_frm_rate(chan_id, &frm_rate);
                frd_round_frm_rate(frm_rate.st_set_frm_rate.fps_integer * 10, &round_rate_hiplayer); /* 10 :a number */

                if (round_rate > 0) {
                    image->frame_rate = frame_rate * 1000 / 10; /* 1000 :a number 10 :a number */
                } else if ((ret == HI_SUCCESS) && (round_rate_hiplayer > 0)) {
                    image->frame_rate = round_rate_hiplayer * 1000 / 10; /* 1000 :a number 10 :a number */
                } else {
                    image->frame_rate = frame_rate * 1000 / 10; /* 1000 :a number 10 :a number */
                }
            } else {
                image->frame_rate = 0;
            }
        } else {
            pts_info_guess_first_frame_rate(chan_id, src_frame_rate, image);
        }
    }

    pts_info_get_round_frame_rate(chan_id, image);
}

static hi_void pts_info_frame_rate_detect(hi_s32 chan_id, vfmw_image *image)
{
    hi_u32 frame_rate = 0;
    pts_chan *chan = HI_NULL;
    frd_alg_info *frd_info = HI_NULL;
    pts_entry *entry = HI_NULL;
    vfmw_pts_frmrate frame_rate_info;

    VFMW_CHECK_SEC_FUNC(memset_s(&frame_rate_info, sizeof(vfmw_pts_frmrate), 0, sizeof(vfmw_pts_frmrate)));
    entry = pts_get_entry();
    OS_SEMA_DOWN(entry->sema);
    chan = pts_get_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        OS_SEMA_UP(entry->sema);
        return;
    }

    frd_info = &(chan->pts_info);

    /*
     * For RWZB test, VO will use default frame rate adapted to output norm.
     * If frame type was set to STREAM type, need not detect, use stream frame rate.
     * If frame type was set to USER type, need not detect, use the user set value.
      */
    if (((image->rwzb == 0) &&
         (chan->user_frame_rate.en_frm_rate_type != PTS_FRMRATE_TYPE_STREAM) &&
         (chan->user_frame_rate.en_frm_rate_type != PTS_FRMRATE_TYPE_USER)) ||
        (chan->user_frame_rate.en_frm_rate_type == PTS_FRMRATE_TYPE_STREAM && image->frame_rate == 0)) {
        frame_rate = frd_frame_rate_detect(frd_info, image->pts);

        image->frame_rate = frame_rate * 1000 / 10; /* 1000 :a number 10 :a number */
    } else if (chan->user_frame_rate.en_frm_rate_type == PTS_FRMRATE_TYPE_USER) {
        /* if frame rate type was USER type, use the value from user directly. */
        if (chan->set_frame_time != VFMW_INVALID_PTS) {
            VFMW_CHECK_SEC_FUNC(memcpy_s(&frame_rate_info, sizeof(vfmw_pts_frmrate),
                                         &chan->user_frame_rate, sizeof(vfmw_pts_frmrate)));
        }

        image->frame_rate = frame_rate_info.st_set_frm_rate.fps_integer * 1000 + /* 1000 is unit conversion */
                            frame_rate_info.st_set_frm_rate.fps_decimal;
        OS_SEMA_UP(entry->sema);
        return;
    }
    OS_SEMA_UP(entry->sema);

    /* For DPT, to avoid the framerate of the first frame is zero */
    pts_info_get_first_frame_rate(chan_id, image, frd_info, frame_rate);

    if (image->frame_rate < PTS_MIN_FRAME_RATE * 1000) { /* 1000 is unit */
        image->frame_rate = PTS_MIN_FRAME_RATE * 1000 ; /* 1000 is unit */
    }

    frd_info->sample_last_frame_rate = image->frame_rate;

    return;
}

/*
 For PTS_FRMRATE_TYPE_PTS:      Recover pts and detect frame rate;
 For PTS_FRMRATE_TYPE_STREAM:   Only Recover pts, use frame rate from stream;
 For PTS_FRMRATE_TYPE_USER:     Only Recover pts, use user set frame rate;
 For PTS_FRMRATE_TYPE_USER_PTS: Recover pts and detect frame rate;
 */
hi_s32 pts_info_calc_stamp(hi_s32 chan_id, vfmw_image *image)
{
    pts_chan *chan = HI_NULL;
    pts_entry *entry;

    entry = pts_get_entry();
    OS_SEMA_DOWN(entry->sema);
    chan = pts_get_chan(chan_id);
    if (chan == HI_NULL || chan->inited == HI_FALSE) {
        dprint(PRN_ERROR, "Start first:%d\n", chan_id);
        OS_SEMA_UP(entry->sema);
        return HI_FAILURE;
    }

    chan->frame_num++;
    chan->pts_info.pts_unit = 1000;  /* 1000: pts unit in us */
    OS_SEMA_UP(entry->sema);

    /* src_pts is valid */
    if (image->src_pts != VFMW_INVALID_PTS) {
        pts_info_handle_valid_src_pts(chan_id, image);
    } else {
        pts_info_handle_invalid_src_pts(chan_id, image);
    }

#if 0

    if ((image->format & 0x300) == 0) {
        g_PtsChan[chan_id].inter_pts_delta = 0;
    }

#endif

    pts_info_frame_rate_detect(chan_id, image);

    return HI_SUCCESS;
}

#ifdef VFMW_PROC_SUPPORT
static hi_void pts_info_handle_cmd(cmd_str_ptr cmd_str, hi_u32 cmd_len)
{
    hi_u32 value = 0;
    hi_s8 *cmd_id = ((*cmd_str)[0]);

    VFMW_ASSERT_PRNT(cmd_len >= 2, "Cmd param count %d invalid.\n", cmd_len); /* 2 :a number */

    /* Parse command */
    if (!OS_STRNCMP(cmd_id, LOG_CMD_SAMPLE_NUM, OS_STRLEN(cmd_id))) {
        if (str_to_val((*cmd_str)[1], &value) != 0) {
            dprint(PRN_ALWS, "error string to value '%s'!\n", (*cmd_str)[1]);
            return;
        }

        g_cfg_sample_num = value;
        dprint(PRN_ALWS, "Set CfgSampleNum %d\n", g_cfg_sample_num);
    } else {
        dprint(PRN_ALWS, "Unsupport command '%s', len %d!\n", cmd_id, cmd_len);
    }

    return;
}

static inline hi_u8 *pts_info_show_frm_rate_type(vfmw_pts_framrate_type pts_frm_type)
{
    switch (pts_frm_type) {
        case PTS_FRMRATE_TYPE_PTS:
            return "pts";
            break;

        case PTS_FRMRATE_TYPE_STREAM:
            return "STREAM";
            break;

        case PTS_FRMRATE_TYPE_USER:
            return "USER";
            break;

        case PTS_FRMRATE_TYPE_USER_PTS:
            return "USER_PTS";
            break;

        default:
            return "Unkown";
            break;
    }
}

hi_s32 pts_info_read_proc(hi_void *buffer, hi_s32 chan_id)
{
    pts_chan *chan = HI_NULL;
    frd_alg_info *frd_info = HI_NULL;
    vfmw_pts_frmrate *set_fps = HI_NULL;
    hi_s32 used = 0;
    hi_s32 len = 0;

    OS_DUMP_PROC(buffer, len, &used, 0, LOG_FMT_HEAD, "pts");

    chan = pts_get_chan(chan_id);
    if (chan == HI_NULL || chan->inited == HI_FALSE) {
        OS_DUMP_PROC(buffer, len, &used, 0, "  pts chan %d not exist.\n\n", chan_id);
        return HI_FAILURE;
    }

    frd_info = &(chan->pts_info);
    set_fps = &(chan->user_frame_rate);

    OS_DUMP_PROC(buffer, len, &used, 0, LOG_FMT_D, "Chan", chan_id);
    OS_DUMP_PROC(buffer, len, &used, 0, LOG_FMT_D, "Inite", chan->inited);
    OS_DUMP_PROC(buffer, len, &used, 0, LOG_FMT_S, "Type",
                 pts_info_show_frm_rate_type(set_fps->en_frm_rate_type));
    OS_DUMP_PROC(buffer, len, &used, 0, LOG_FMT_I_D, "User Fps", set_fps->st_set_frm_rate.fps_integer,
                 set_fps->st_set_frm_rate.fps_decimal);
    OS_DUMP_PROC(buffer, len, &used, 0, LOG_FMT_L, "Set Frame Time", chan->set_frame_time);
    OS_DUMP_PROC(buffer, len, &used, 0, LOG_FMT_S, "pts Unit",
                 (frd_info->pts_unit == 1) ? "ms" : "us");
    OS_DUMP_PROC(buffer, len, &used, 0, LOG_FMT_D, "FRD Enable", frd_info->sample_frd_enable);
    OS_DUMP_PROC(buffer, len, &used, 0, "\n");

    if (frd_info->sample_frd_enable == HI_TRUE) {
        OS_DUMP_PROC(buffer, len, &used, 0, LOG_FMT_D, "Cfg Sample Num", g_cfg_sample_num);
        OS_DUMP_PROC(buffer, len, &used, 0, LOG_FMT_D, "Act Sample Num", frd_info->sample_act_num);
        OS_DUMP_PROC(buffer, len, &used, 0, LOG_FMT_D, "Max Sample Num", frd_info->sample_max_num);
        OS_DUMP_PROC(buffer, len, &used, 0, LOG_FMT_I_D, "Max  Fps", frd_info->sample_max_rate / 10, /* 10 :a number */
                     frd_info->sample_max_rate % 10); /* 10 :a number */
        OS_DUMP_PROC(buffer, len, &used, 0, LOG_FMT_I_D, "Min  Fps", frd_info->sample_min_rate / 10, /* 10 :a number */
                     frd_info->sample_min_rate % 10); /* 10 :a number */
        OS_DUMP_PROC(buffer, len, &used, 0, LOG_FMT_I_D, "Avg  Fps", frd_info->sample_avg_rate / 10, /* 10 :a number */
                     frd_info->sample_avg_rate % 10); /* 10 :a number */
        OS_DUMP_PROC(buffer, len, &used, 0, LOG_FMT_I_D, "Last Fps",
                     frd_info->sample_last_frame_rate / 1000, /* 1000 :a number */
                     frd_info->sample_last_frame_rate % 1000); /* 1000 :a number */
        OS_DUMP_PROC(buffer, len, &used, 0, "\n");
    }

    OS_DUMP_PROC(buffer, len, &used, 0, LOG_FMT_D, "Total Frame", chan->frame_num);
    OS_DUMP_PROC(buffer, len, &used, 0, LOG_FMT_I_D, "Output Fps",
                 frd_info->sample_last_frame_rate / 1000, /* 1000 :a number */
                 frd_info->sample_last_frame_rate % 1000); /* 1000 :a number */
    OS_DUMP_PROC(buffer, len, &used, 0, "\n");

    return HI_SUCCESS;
}

hi_s32 pts_info_write_proc(cmd_str_ptr buffer, hi_u32 count)
{
    pts_info_handle_cmd(buffer, count);

    return count;
}
#endif

/* ----------------------------EXTERNAL FUNCTION------------------------------- */
hi_s32 pts_info_init(hi_void)
{
    pts_entry *entry;

    entry = pts_get_entry();
    entry->inited = HI_TRUE;
    entry->sema = 0;
    OS_SEMA_INIT(&entry->sema);

    return HI_SUCCESS;
}

hi_s32 pts_info_exit(hi_void)
{
    pts_entry *entry;

    entry = pts_get_entry();
    entry->inited = HI_FALSE;
    OS_SEMA_EXIT(entry->sema);

    return HI_SUCCESS;
}

hi_s32 pts_info_alloc(hi_s32 chan_id)
{
    pts_entry *entry = HI_NULL;
    pts_chan *chan = HI_NULL;

    entry = pts_get_entry();
    VFMW_ASSERT_RET(entry->inited == HI_TRUE, HI_FAILURE);
    OS_SEMA_DOWN(entry->sema);
    chan = pts_get_chan(chan_id);
    if (chan != HI_NULL) {
        dprint(PRN_ERROR, "%s chan %d already inited.\n", __func__, chan_id);
        OS_SEMA_UP(entry->sema);
        return HI_FAILURE;
    }

    chan = OS_ALLOC_VIR("PTS_Inst", sizeof(pts_chan));
    if (chan == HI_NULL) {
        OS_SEMA_UP(entry->sema);
        return HI_FAILURE;
    }
    entry->chan[chan_id] = chan;

    pst_info_init_param(chan_id);
    frd_reset(&chan->pts_info, 0);
    VFMW_CHECK_SEC_FUNC(memset_s(&chan->user_frame_rate, sizeof(vfmw_pts_frmrate), 0, sizeof(vfmw_pts_frmrate)));
    chan->set_frame_time = VFMW_INVALID_PTS;
    chan->pts_info.cal_pts = VFMW_INVALID_PTS;
    chan->pts_info.que_cnt = 0;
    chan->pts_info.que_stable = 0;
    chan->inited = HI_TRUE;
    OS_SEMA_UP(entry->sema);

    return HI_SUCCESS;
}

hi_s32 pts_info_free(hi_s32 chan_id)
{
    pts_chan *chan = HI_NULL;
    pts_entry *entry = HI_NULL;

    entry = pts_get_entry();
    VFMW_ASSERT_RET(entry->inited == HI_TRUE, HI_FAILURE);

    chan = pts_get_chan(chan_id);
    VFMW_ASSERT_RET(chan != HI_NULL, HI_FAILURE);
    VFMW_ASSERT_RET(chan->inited == HI_TRUE, HI_FAILURE);

    OS_SEMA_DOWN(entry->sema);
    OS_FREE_VIR(chan);
    entry->chan[chan_id] = HI_NULL;
    OS_SEMA_UP(entry->sema);
    return HI_SUCCESS;
}

hi_s32 pts_info_reset(hi_s32 chan_id)
{
    pts_chan *chan = HI_NULL;
    pts_entry *entry = HI_NULL;

    entry = pts_get_entry();
    OS_SEMA_DOWN(entry->sema);
    chan = pts_get_chan(chan_id);
    if (chan == HI_NULL || chan->inited == HI_FALSE) {
        dprint(PRN_ERROR, "%s chan %d is not inited.\n", __func__, chan_id);
        OS_SEMA_UP(entry->sema);
        return HI_FAILURE;
    }

    pst_info_init_param(chan_id);
    frd_reset(&chan->pts_info, 0);
    OS_SEMA_UP(entry->sema);

    return HI_SUCCESS;
}

hi_s32 pts_info_set_frm_rate(hi_s32 chan_id, vfmw_pts_frmrate *frame_rate)
{
    hi_u32 div_num;
    pts_chan *chan = HI_NULL;
    pts_entry *entry = HI_NULL;

    entry = pts_get_entry();
    VFMW_ASSERT_RET(frame_rate != HI_NULL, HI_FAILURE);

    dprint(PRN_PTS, "%s: Chan %d en_frm_rate_type %d, fps_integer %d, fps_decimal %d.\n", __func__,
           chan_id, frame_rate->en_frm_rate_type, frame_rate->st_set_frm_rate.fps_integer,
           frame_rate->st_set_frm_rate.fps_decimal);

    OS_SEMA_DOWN(entry->sema);
    chan = pts_get_chan(chan_id);
    if (chan == HI_NULL || chan->inited == HI_FALSE) {
        dprint(PRN_ERROR, "%s chan %d is not inited.\n", __func__, chan_id);
        OS_SEMA_UP(entry->sema);
        return HI_FAILURE;
    }

    if (frame_rate->en_frm_rate_type == PTS_FRMRATE_TYPE_USER ||
        frame_rate->en_frm_rate_type == PTS_FRMRATE_TYPE_USER_PTS) {
        /* 1000 :a number */
        div_num = frame_rate->st_set_frm_rate.fps_integer * 1000 + frame_rate->st_set_frm_rate.fps_decimal;

        if ((div_num > PTS_MAX_FRAME_RATE * 1000) || (div_num == 0)) { /* 1000 :a number */
            dprint(PRN_ERROR, "invalid frame rate, Integer(%d), Decimal(%d) \n",
                   frame_rate->st_set_frm_rate.fps_integer,
                   frame_rate->st_set_frm_rate.fps_decimal);
            OS_SEMA_UP(entry->sema);
            return HI_FAILURE;
        }

        chan->set_frame_time = 1000000000 / div_num; /* 1000000000 :a number */
    } else {
        chan->set_frame_time = VFMW_INVALID_PTS;
    }

    VFMW_CHECK_SEC_FUNC(memcpy_s(&chan->user_frame_rate, sizeof(vfmw_pts_frmrate),
                                 frame_rate, sizeof(vfmw_pts_frmrate)));
    OS_SEMA_UP(entry->sema);

    return HI_SUCCESS;
}

hi_s32 pts_info_get_frm_rate(hi_s32 chan_id, vfmw_pts_frmrate *frame_rate)
{
    pts_chan *chan = HI_NULL;
    pts_entry *entry = HI_NULL;

    entry = pts_get_entry();
    VFMW_ASSERT_RET(frame_rate != HI_NULL, HI_FAILURE);

    OS_SEMA_DOWN(entry->sema);
    chan = pts_get_chan(chan_id);
    if (chan == HI_NULL || chan->inited == HI_FALSE) {
        dprint(PRN_ERROR, "%s chan %d is not inited.\n", __func__, chan_id);
        OS_SEMA_UP(entry->sema);
        return HI_FAILURE;
    }

    if (chan->force_pts_mode == 1 ||
        chan->set_frame_time != VFMW_INVALID_PTS) {
        VFMW_CHECK_SEC_FUNC(memcpy_s(frame_rate, sizeof(vfmw_pts_frmrate),
                                     &chan->user_frame_rate, sizeof(vfmw_pts_frmrate)));
    } else {
        VFMW_CHECK_SEC_FUNC(memset_s(frame_rate, sizeof(vfmw_pts_frmrate), 0, sizeof(vfmw_pts_frmrate)));
        frame_rate->en_frm_rate_type = chan->user_frame_rate.en_frm_rate_type;
    }

    OS_SEMA_UP(entry->sema);
    dprint(PRN_PTS, "%s: Chan %d en_frm_rate_type %d, fps_integer %d, fps_decimal %d.\n", __func__,
           chan_id, frame_rate->en_frm_rate_type, frame_rate->st_set_frm_rate.fps_integer,
           frame_rate->st_set_frm_rate.fps_decimal);

    return HI_SUCCESS;
}

hi_s32 pts_info_convert(hi_s32 chan_id, vfmw_image *image)
{
    hi_s64 in_pts;
    hi_u32 in_frame_rate;
    pts_chan *chan = HI_NULL;
    pts_entry *entry = HI_NULL;

    entry = pts_get_entry();
    VFMW_ASSERT_RET(image != HI_NULL, HI_FAILURE);
    OS_SEMA_DOWN(entry->sema);
    chan = pts_get_chan(chan_id);
    if (chan == HI_NULL || chan->inited == HI_FALSE) {
        dprint(PRN_ERROR, "%s chan %d is not inited.\n", __func__, chan_id);
        OS_SEMA_UP(entry->sema);
        return HI_FAILURE;
    }

    if (image->format.frame_rate_valid && chan->user_frame_rate.en_frm_rate_type != PTS_FRMRATE_TYPE_USER) {
        chan->user_frame_rate.en_frm_rate_type = PTS_FRMRATE_TYPE_STREAM;
        chan->force_pts_mode = 1;
    } else {
        chan->force_pts_mode = 0;
    }

    OS_SEMA_UP(entry->sema);
    in_pts = image->pts;
    in_frame_rate = image->frame_rate;

    pts_info_calc_stamp(chan_id, image);

    dprint(PRN_PTS, "pts info: w %d, h %d, pts{src %lld, in %lld, out %lld, delta %d}, frm_rate{in %d, out %d}\n",
           image->image_width, image->image_height,
           image->src_pts, in_pts, image->pts, image->inter_pts_delta,
           in_frame_rate, image->frame_rate);

    return HI_SUCCESS;
}

hi_s32 pts_info_enable_sample_frd(hi_s32 chan_id)
{
    pts_chan *chan = HI_NULL;
    frd_alg_info *frd_info = HI_NULL;
    pts_entry *entry = HI_NULL;

    entry = pts_get_entry();
    OS_SEMA_DOWN(entry->sema);
    chan = pts_get_chan(chan_id);
    if (chan == HI_NULL || chan->inited == HI_FALSE) {
        dprint(PRN_ERROR, "%s chan %d is not inited.\n", __func__, chan_id);
        OS_SEMA_UP(entry->sema);
        return HI_FAILURE;
    }

    frd_info = &(chan->pts_info);

    /* Use config sample num in prority */
    g_cur_sample_num = g_cfg_sample_num;
    frd_info->sample_max_num = g_cfg_sample_num;
    frd_info->sample_frd_enable = HI_TRUE;
    OS_SEMA_UP(entry->sema);

    return HI_SUCCESS;
}

hi_s32 pts_info_disable_sample_frd(hi_s32 chan_id)
{
    pts_chan *chan = HI_NULL;
    frd_alg_info *frd_info = HI_NULL;
    pts_entry *entry = HI_NULL;

    entry = pts_get_entry();
    OS_SEMA_DOWN(entry->sema);
    chan = pts_get_chan(chan_id);
    if (chan == HI_NULL || chan->inited == HI_FALSE) {
        dprint(PRN_ERROR, "%s chan %d is not inited.\n", __func__, chan_id);
        OS_SEMA_UP(entry->sema);
        return HI_FAILURE;
    }

    frd_info = &(chan->pts_info);

    g_cur_sample_num = 0;
    frd_info->sample_max_num = 0;
    frd_info->sample_frd_enable = HI_FALSE;
    OS_SEMA_UP(entry->sema);

    return HI_SUCCESS;
}

hi_s32 pts_info_read(hi_void *buffer, hi_s32 chan_id)
{
#ifdef VFMW_PROC_SUPPORT
    pts_info_read_proc(buffer, chan_id);
#endif

    return HI_SUCCESS;
}

hi_s32 pts_info_write(cmd_str_ptr buffer, hi_u32 count)
{
#ifdef VFMW_PROC_SUPPORT
    pts_info_write_proc(buffer, count);
#endif

    return HI_SUCCESS;
}


