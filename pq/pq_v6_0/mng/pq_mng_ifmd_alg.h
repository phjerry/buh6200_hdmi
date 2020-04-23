/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq ifmd alg api define
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef PQ_MNG_IFMD_ALG_H
#define PQ_MNG_IFMD_ALG_H

#include "pq_hal_comm.h"
#include "pq_hal_fmd.h"
#include "pq_mng_fod_alg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MAXLINESIZE                 4096
#define TOTAL_MODES                 4
#define BUFF_SIZE                   110
#define EIGENVALUE_BUFFSIZE         10
#define SCDWINWIDTH                 7
#define SCD_COUNT_DOWN_MAX          8
#define TOP_FIRST                   0
#define BOTTOM_FIRST                1
#define TOP_FIELD_REF               0
#define BOTTOM_FIELD_REF            1
#define PCC_THRE_SIGMA_VARY_CADENCE 1550
#define PCC_THRE_COMB_CADENCE       850
#define PCC_MAX_VALUE               2000
#define DOUBLE_TO_INT_MULTIPLE      100
#define SCD_THRD                    90000
#define SCD_THRD_2                  10000
#define SCD_THRD_3                  1000
#define SCD_THRD_4                  100
#define SCD_THRD_5                  10
#define NOISE_THR                   20
#define MODE_COUNTER_THRD           4
#define MODE_UP_BOUND               100
#define MODE_DOWN_BOUND             0
#define MAX_UINT                    40000000
#define MIN_UINT                    0
#define MAX_INT                     20000000
#define MIN_INT                     (-20000000)
#define MODE_NONE                   0
#define MAX_PIXEL_VALUE             255

typedef enum {
    MODE_TOP_FIRST = 0,
    MODE_BOTTOM_FIRST = 1
} pq_ifmd_field_order_mode;

typedef enum {
    TOP_FIELD_FMD = 0,
    BOTTOM_FIELD_FMD = 1
} pq_ifmd_field_polarity;

typedef enum {
    PCC_SIGMA_VARY = 0,
    PCC_KF = 1,
    PCC_STILL_SCENE = 2
}pq_ifmd_pcc_mode;

typedef enum {
    NONE = 0,
    TF = 1,
    TBF = 2,
} pq_ifmd_fod;

typedef enum {
    PB_VIDEO = 0,
    PB_32_PD,
    PB_22_PD
} pq_ifmd_pbstate;

typedef enum {
    DIE_FALSE = 0,
    DIE_TRUE = 1
} pq_die_bool;

typedef enum {
    MODE_32 = 0,
    MODE_2332 = 1,
    MODE_32322 = 2,
    MODE_11_2_3 = 3
} pq_ifmd_pulldown_mode;

typedef struct {
    hi_s32 bt_mode;
    hi_s32 ref_fld;
    hi_s32 width;
    hi_s32 height;
} reg_pic_para;

typedef struct {
    /* the second field in buffer is current field, so last field is the first one. TOP_FIELD or BOTTOM_FIELD */
    pq_ifmd_field_polarity last_fld_polarity;
    /* 1--Not duplication field, sigma is large; 0--a duplication field, sigma is small */
    hi_bool b_n_duplication;
    /* start from 1, to max sample number in a period */
    hi_s32 sample_time;
} field_pol_dup_samp;

typedef struct {
    /* 以下数组为各模式的周期性特征总结，用于判断序列的Pulldown模式和相位。 */
    field_pol_dup_samp *spfield_pol_dup_samp;
    hi_s32 *p_sv_cadence;
    hi_s32 *p_mmm_cadence;
    hi_s32 *p_still_frm_cadence;
    /* 1--the next field is the match field to current one; */
    /* -1--the last field is the match field to current one. */
    /* This variable is related to delay. */
    hi_s32 *p_match_direction;
    /* 1--Is corresponding to a Key frame after de-interlace; */
    /* 0--Is corresponding to a duplicated frame. */
    hi_s32 *p_keyframe_position;
} pm_cadence_each;

typedef struct {
    hi_s32 period;
    hi_s32 period_tk;
    hi_s32 num_fod_cases;
    pm_cadence_each *sp_pm_cadence_each_fo;  /* memory should be allocated as num_fod_cases */
} basic_cadence;

typedef struct {
    hi_u32 handle_no;
    hi_u32 width;
    hi_u32 height;
    hi_void *pst_stt_reg;
    hi_void *p_dei_rgmv;
    hi_u32 rgmv_stride;
    hi_u32 scd;

    hi_s32 bt_mode;  /* 场序 */
    hi_s32 ref_fld;  /* 参考场 */
} ifmd_regpic_para_s;

typedef struct {
    /* 0--no subtitle is detected during current 2 period; */
    /* 1--still subtitle or start of a rolling subtitle; 2--rolling subtitle */
    hi_u8 is_still_or_rolling_subtitle;
    /* if the current field is the start of the appearence of a subtitle. */
    /* if it is, the match direction of the current field should be set as -1. */
    hi_s8 b_start;
} ifmd_subtitle_s;

typedef struct {
    /* 以下为场级特性记录，得到sigma的同时，按照不同的顺序写入不同场序的buffer内 */
    hi_s32 sigma;
    hi_s32 hist_bin_1;
    hi_s32 hist_bin_2;
    hi_s32 hist_bin_3;
    hi_s32 hist_bin_4;
    hi_s32 non_monotony;
    hi_s32 non_monotony_32;
    hi_s32 non_monotony_2332;
    hi_s32 non_monotony_32322;
    hi_s32 a_sigma[BUFF_SIZE];
    hi_s8 a_sigma_vary[BUFF_SIZE];
    hi_s32 a_field_comb[BUFF_SIZE];
    hi_s8 a_field_comb_vary[BUFF_SIZE];
    hi_s8 a_field_comb_vary_32[BUFF_SIZE];
    hi_s8 el_vary[BUFF_SIZE];
    hi_s32 kf_cnt_btw_period;
    hi_s32 other_mode_cv_cnt;
    hi_u8 big_small_cnt_btw_period;
    hi_u16 a_sc_counter[BUFF_SIZE];
    hi_s32 a_sigma_non_monotony[BUFF_SIZE];
    hi_s32 a_sigma_non_monotony_32[BUFF_SIZE];
    hi_s32 a_sigma_non_monotony_2332[BUFF_SIZE];
    hi_s32 a_sigma_non_monotony_32322[BUFF_SIZE];
    hi_s32 a_non_monotony[BUFF_SIZE];
    hi_s32 a_comb_non_monotony[BUFF_SIZE];
    hi_u8 a_still_frame[BUFF_SIZE];
    hi_s8 a_frame_motion_vary[BUFF_SIZE];
    hi_s8 a_mcomb_03_minus_mcomb_12[BUFF_SIZE];
} ifmd_frmdiff_characters_s;

typedef struct {
    hi_s32 hist_of_1st_frame[64]; /* array index 64 */
    hi_s32 hist_of_2nd_frame[64]; /* array index 64 */
    hi_u8 curr_pos;
    hi_u8 num_since_lst_scd;
    hi_s32 chd_window[SCDWINWIDTH];
} ifmd_scd_status_s;

typedef struct {
    hi_s32 m_comb_23;
    hi_s32 m_comb_03;
    hi_s32 m_comb_12;
    hi_s32 m_comb_crss;
    hi_s32 non_mono_23;
    hi_s32 non_mono_03;
    hi_s32 non_mono_12;
    hi_s32 non_mono_crss;
} ifmd_frm_comb_paras_s;

typedef struct {
    hi_u8 low_times;
    hi_u8 mid_times;
    hi_u8 high_times;
    hi_s32 sigma_thr_0;
    hi_s32 sigma_thr_1;
    hi_s32 sigma_thr_2;
    hi_s32 sigma_thr_3;
    hi_s32 sigma_thr_4;
} ifmd_sv_buff_ctrl_s;

typedef struct {
    hi_s32 still_scen_thr;
    hi_s32 ss_max_thr;
    hi_s32 ss_thr_0;
    hi_s32 ss_thr_1;
    hi_s32 still_add_thr;
    hi_s32 still_divi_thr;
    hi_s32 comb_chg_add_thr;
    hi_s32 comb_chg_mult_thr;
} ifmd_ss_buff_ctrl_s;

typedef struct  {
    hi_s32 mmm_add_thr;
    hi_s32 mmm_divi_thr;
} ifmd_mmm_buff_ctrl_s;

typedef struct {
    /* 以下为预留的为字幕检测算法改进的配置接口参数 */
    hi_s32 non_monotony_thr;  // NONMONOTONY_THRED;
    hi_s32 non_monotony_hist_range_start;
    hi_s32 non_monotony_hist_range_end;
} ifmd_non_mton_ctrl_s;

typedef struct {
    hi_s32 pcc_thre_ori;
    pq_ifmd_fod *fod_modes;
    hi_s8 num_fod_cases, phases_each_mode, period, period_tk;

    hi_s8 *p_sv_cadence_tf, *p_sv_cadence_tbf;
    hi_s8 *p_kf_cadence_tf, *p_kf_cadence_tbf;
    hi_s8 *p_md_cadence_tf, *p_md_cadence_tbf;
    hi_s8 *p_md_cadence[4][2]; /* array index [4][2] */
    hi_s8 ss_thr_tf;
    hi_s8 fmv_thr_tf;
} ifmd_cad_mode_para_set_s;

typedef struct {
    hi_s32 max_pcc_dup_cadence;
    hi_s32 max_pcc_mmm_cadence;
    hi_s32 max_pcc_ss_cadence;
    hi_s32 max_pcc;
    hi_s32 max_pcc_mode;
} ifmd_max_pcc_and_mode_s;

typedef struct {
    hi_s8 mode_counter_32[11]; /* 11: 32 模式array index 2 * 5 + 1 */
    hi_s8 mode_counter_2332[21]; /* 21: 2332 模式array index 4 * 5 + 1 */
    hi_s8 mode_counter_32322[25]; /* 25: 32322 模式array index 4 * 6 + 1 */
    hi_s8 mode_counter11_2_3[51]; /* 51: 11_2_3 模式array index 2 * 25 + 1 */
    hi_s8 *mode_counters[TOTAL_MODES];
    hi_u16 mode_locked[TOTAL_MODES];
    hi_s8 unlock_counter[TOTAL_MODES];
    hi_u16 last_trend[TOTAL_MODES];
    hi_u16 d_max_pcc_per_mode[TOTAL_MODES];
    hi_u16 max_pcc_phase[TOTAL_MODES];
    hi_u8 flag_adjust;
    hi_u8 ov_title_det_method;

    hi_s8 b_comb_large_sharp;
    hi_s8 b_sigma_has_vary;
    hi_s8 b_comb_diff_large;
    hi_s8 b_comb_large;
    hi_s8 key_value_from_module;
    hi_s8 mask_32_cnt;
    hi_s8 mask_1123_cnt;

    hi_s32 max_comb;
    hi_s32 min_comb;
    hi_s32 mask_22_cnt;
    hi_s32 mask_any_cnt;
    hi_s32 text_cnt;
    hi_s32 max_err_line[20]; /* array index 20 */
    hi_s32 el_32_confirm;
    hi_s32 cv_2_confirm;
    hi_s32 el_32_mask;
    hi_s32 other_mode_cnt;

    hi_s32 gmv_0_x;
    hi_s32 gmv_0_y;
    hi_s32 gmv_0_num;
    hi_s32 gmv_1_x;
    hi_s32 gmv_1_y;
    hi_s32 gmv_1_num;

    hi_s32 roma_flag[2]; /* array index 2 */
    hi_s32 music_flag;
    hi_s32 music_menu_flag;
    hi_s32 y_cnt;
    hi_s32 x_cnt;

    hi_s32 field_num;
    hi_s8 scene_change;
    hi_s8 scene_change_counter;
    hi_u16 mode;
    hi_u8 b_is_progressive;
    hi_u16 final_lock_mode_dei;
    hi_u16 final_dei_mode_include_22;

    hi_s8 om_lock_counter;
    hi_s8 om_unlock_counter;
    hi_s8 om_11_2_3_lock_counter;
    hi_s8 om_11_2_3_unlock_counter;
    hi_s8 other_mode_lock_counter;
    hi_s8 other_mode_unlock_counter;
    hi_u16 final_om_dei;
    ifmd_frmdiff_characters_s sp_frame_diff_characters;
    ifmd_frm_comb_paras_s s_frame_comb;
    ifmd_frm_comb_paras_s s_frame_last_comb;
    ifmd_subtitle_s s_still_or_rolling_subtitle[TOTAL_MODES];
    ifmd_scd_status_s s_scd_detector_status;

    ifmd_cad_mode_para_set_s s_cmd_para_set;

    basic_cadence sbasic_cadence[TOTAL_MODES];

} ifmd_32_soft_status_s;

typedef struct {
    hi_u8 ifmd_det_buff_size;
    hi_u8 field_order;
    pq_ifmd_field_polarity polarity_last;
    ifmd_sv_buff_ctrl_s sv_buff_ctrl;
    ifmd_ss_buff_ctrl_s ss_buff_ctrl;
    ifmd_mmm_buff_ctrl_s mmm_buff_ctrl;
    ifmd_non_mton_ctrl_s nm_buff_ctrl;

    hi_u8 a_lock_counter_thds[TOTAL_MODES];
    hi_u8 a_unlock_counter_thds[TOTAL_MODES];

    hi_u8 mode_32_en;
    hi_u8 mode_2332_en;
    hi_u8 mode_32322_en;
    hi_u8 mode_22_en;
    hi_u8 mode_55_64_87_en;
    hi_u8 mode_11_2_3_en;

    hi_u8 sigma_bin_start;
    hi_u8 comb_bin_start;

    hi_u8 err_line_thr;

} ifmd_32_soft_ctrl_s;

typedef struct {
    hi_u8 hist_check_dec;
    hi_u8 enter_lock_level;
    hi_u8 exit_lock_level;
} ifmd_22_lock_ctrl_s;

typedef struct {
    hi_u16 tkr_thr_level;
    hi_u16 ovlp_title_thr;
} ifmd_tkr_ctrl_s;

typedef struct {
    hi_u16 max_still_pcc;
    hi_u16 min_usable_pcc;
    hi_u16 non_match_crss_thr;
    hi_u16 max_non_match_pcc;

} ifmd_pcc_ctrl_s;

typedef struct {
    hi_s32 ref_start_pos;
    hi_s32 to_be_com_start_pos;
    hi_s8 length_ref;
    hi_s32 length_to_be_comp;

} ifmd_pcc_calc_pos_s;

typedef struct {
    hi_u16 still_um_thr;
    hi_u16 lower_non_match_thr_0;
    hi_u32 lower_non_match_thr_1;
    hi_u32 upper_match_thr;
    hi_u16 non_match_ratio;
} ifmd_um_ctrl_s;

typedef struct {
    hi_s32 hist_frm_it_diff[5]; /* array index 5  */
    hi_s32 lst_frm_it_diff_mean;
    hi_s32 cur_frm_it_diff_mean;
    hi_s32 mean_frm_it_diff_acc;

    /* UM */
    hi_s32 frame_match_um;
    hi_s32 frame_non_match_um;
    hi_s32 frame_match_um_2;
    hi_s32 frame_non_match_um_2;

    /* pcc */
    hi_s32 frame_match_weave;
    hi_s32 frame_non_match_weave;
    hi_s32 frame_pcc_crss;
    hi_s32 frame_match_tkr;
    hi_s32 frame_non_match_tkr;

    hi_u8 pld_22_lock;

    hi_u8 a_like22_in_period[10]; /* array index 10  */
    hi_u8 a_um_like22_in_period[10]; /* array index 10  */

    hi_s8 phase_lock_cnt;
    hi_s8 phase_unlock_cnt;
    hi_s8 phase_last_lock_cnt;
    hi_s8 phase_last_unlock_cnt;

    hi_s32 mask_22;
    hi_u8 hd_flag;

} ifmd_22_contxt_s;

typedef struct {
    ifmd_22_contxt_s pld_22_ctx;
    ifmd_22_contxt_s pld_22_ctx_be;
    hi_u8 pld_22_lock_state;
    pq_ifmd_pbstate main_state;
    pq_ifmd_pbstate prev_main_state;

} ifmd_22_soft_status_s;

typedef struct {
    ifmd_22_lock_ctrl_s locker_ctrl;
    ifmd_tkr_ctrl_s tkr_ctrl;
    ifmd_pcc_ctrl_s pcc_ctrl;
    ifmd_um_ctrl_s um_ctrl;

    hi_u16 it_diff_mean_thd;

} ifmd_22_soft_ctrl_s;

typedef struct {
    ifmd_hard2soft_api s_ifmd_hard_to_soft_api;

    ifmd_32_soft_status_s s_ifmd_32_soft_status;
    ifmd_32_soft_ctrl_s s_ifmd_32_soft_ctrl;
    ifmd_22_soft_status_s s_ifmd_22_soft_status;
    ifmd_22_soft_ctrl_s s_ifmd_22_soft_ctrl;
} ifmd_total_soft_para_s;

/* ifmd 全局参数 */
typedef struct {
    ifmd_total_soft_para_s sg_ifmd_total_soft_para;
    ifmd_hard2soft_api sg_ifmd_hard_to_soft_api;

    hi_s8 sv_cadence_32_tf[10]; /* array index 10  */
    hi_s8 kf_cadence_32_tf[10]; /* array index 10  */
    hi_s8 md_cadence_32_tf[10]; /* array index 10  */

    hi_s8 sv_cadence_2332_tf[10]; /* array index 10  */
    hi_s8 kf_cadence_2332_tf[10]; /* array index 10  */
    hi_s8 md_cadence_2332_tf[10]; /* array index 10  */

    hi_s8 sv_cadence_2332_tbf[10]; /* array index 10  */
    hi_s8 kf_cadence_2332_tbf[10]; /* array index 10  */
    hi_s8 md_cadence_2332_tbf[10]; /* array index 10  */

    hi_s8 sv_cadence_32322_tf[12]; /* array index 12  */
    hi_s8 kf_cadence_32322_tf[12]; /* array index 12  */
    hi_s8 md_cadence_32322_tf[12]; /* array index 12  */

    hi_s8 sv_cadence_32322_tbf[12]; /* array index 12  */
    hi_s8 kf_cadence_32322_tbf[12]; /* array index 12  */
    hi_s8 md_cadence_32322_tbf[12]; /* array index 12  */

    hi_s8 sv_cadence_11_2_3_tf[50]; /* array index 50  */
    hi_s8 kf_cadence_11_2_3_tf[50]; /* array index 50  */
    hi_s8 md_cadence_11_2_3_tf[50]; /* array index 50  */

    pq_ifmd_fod gca_fod_modes_mode[TOTAL_MODES];
    hi_s8 gac_ss_thr_tf[TOTAL_MODES];
    hi_s8 gac_fmv_thr_tf[TOTAL_MODES];

    hi_s8 gca_period[TOTAL_MODES];
    hi_s8 gca_period_tk[TOTAL_MODES];
    hi_s8 gca_num_fo_cases[TOTAL_MODES];
    hi_s8 gca_key_frame_range[TOTAL_MODES];
} ifmd_parammeter_s;

hi_s32 ifmd_get_playback_config(hi_drv_pq_ifmd_playback *sp_ifmd_result);

hi_s32 ifmd_get_final_result(ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
    ifmd_22_soft_status_s *sp_ifmd_22_softs_tatus, ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl,
    hi_drv_pq_ifmd_playback *sp_ifmd_result);

hi_s32 ifmd_detect_pulldown_mode_phase(ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
    ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl);

hi_s8 ifmd_detect_om_mode_phase(ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
    ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl);

hi_s32 ifmd_detect_22_pulldown_mode(ifmd_hard2soft_api *sp_ifmd_hard_to_soft_api,
    ifmd_22_soft_status_s *sp_ifmd_22_softs_tatus, ifmd_22_soft_ctrl_s *sp_ifmd_22_soft_ctrl,
    ifmd_32_soft_status_s *sp_ifmd_32_soft_status, ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl);

hi_s32 ifmd_detect_22_pld(ifmd_hard2soft_api *sp_ifmd_hard_to_soft_api,
    ifmd_22_soft_status_s *sp_ifmd_22_softs_tatus, ifmd_22_soft_ctrl_s *sp_ifmd_22_soft_ctrl,
    ifmd_32_soft_status_s *sp_ifmd_32_soft_status, ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl);

hi_s32 ifmd_overlapped_title_check_with_ratio(ifmd_hard2soft_api *sp_ifmd_hard_to_soft_api,
    ifmd_22_soft_ctrl_s *sp_ifmd_22_soft_ctrl);

pq_ifmd_pbstate ifmd_get_playback_state(pq_die_bool pd_32_det, pq_die_bool pd_22_det,
    ifmd_22_soft_status_s *sp_ifmd_22_softs_tatus);

hi_s32 ifmd_locker_scheme(pq_ifmd_pulldown_mode curr_cadence_mode, ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
    hi_s32 num_total_modes, ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl);

hi_void ifmd_adjust_mode_counters(hi_s32 curr_cadence_mode,
    ifmd_32_soft_status_s *sp_ifmd_32_soft_status, hi_s32 curr_trend);

hi_s32 ifmd_check_scd(ifmd_hard2soft_api *sp_ifmd_hard_to_soft_api,
    ifmd_32_soft_status_s *sp_ifmd_32_soft_status, hi_s32 buff_size);

hi_void ifmd_update_character_buffs(ifmd_hard2soft_api *sp_ifmd_hard_to_soft_api,
    ifmd_32_soft_status_s *sp_ifmd_32_soft_status, ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl,
    ifmd_22_soft_status_s *sp_ifmd_22_softs_tatus, ifmd_22_soft_ctrl_s *sp_ifmd_22_soft_ctrl);

hi_s32 ifmd_get_non22_mode_key_frame_state(ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
    ifmd_32_soft_ctrl_s *sp_ifmd_32_soft_ctrl, hi_drv_pq_ifmd_playback *sp_ifmd_result);

hi_bool ifmd_get_22_mode_key_frame_state(ifmd_32_soft_status_s *sp_ifmd_32_soft_status,
    ifmd_22_soft_status_s *sp_ifmd_22_softs_tatus, hi_drv_pq_ifmd_playback *sp_ifmd_result, hi_s32 field_mode);

hi_s32 ifmd_deinit_ifmd_soft(hi_void);

hi_s32 pq_mng_get_ifmd_vir_reg(hi_void);

hi_s32 pq_mng_get_film_mode(hi_u32 *p_film_mode);

hi_s32 pq_mng_update_ifmd_api(hi_drv_pq_vpss_stt_info *pst_info_in,
    hi_drv_pq_ifmd_playback *sp_ifmd_result);

hi_s32 pq_mng_ifmd_sofeware_para_init(hi_u32 width, hi_u32 height);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

