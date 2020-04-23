/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq hal fmd api
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __PQ_HAL_FMD_H__
#define __PQ_HAL_FMD_H__

#include "hi_type.h"
#include "pq_hal_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_s32 stillblk_thr;    /* 8bit [0:255] */
    hi_s32 diff_movblk_thr; /* 11bit [0:2047] */
} reg_stillblk_input;       /* still block */

typedef struct {
    hi_s32 still_blk_cnt;
    hi_s32 blk_sad[16]; /* 16: blk_sad len */
} reg_stillblk_stat;

/* FOD 与FMD 共用此结构体 */
typedef struct {
    hi_s32 lasi_cnt14;
    hi_s32 lasi_cnt32;
    hi_s32 lasi_cnt34;
} reg_lasi_stat;

typedef struct {
    hi_s32 lasi_mode;
    hi_s32 lasi_mov_thr;
    hi_s32 lasi_coring_thr;
    hi_s32 lasi_edge_thr;
    hi_s32 txt_alpha;
    hi_s32 txt_coring;
    hi_s32 txt_thr0;
    hi_s32 txt_thr1;
    hi_s32 txt_thr2;
    hi_s32 txt_thr3;
    hi_s32 endpos;
    hi_s32 startpos;
    hi_s32 diff_movblk_thr;
    hi_s32 bfield_first;
} alg_fod_reg_cfgpara;

/* submission information struct for hardware (exterior interface) */
typedef struct {
    hi_s32 field_order;
} alg_fod_outpara;

typedef struct {
    hi_s32 s_dir_mch;
    hi_s32 s_die_out_sel;
    hi_s32 field_order;   /* 顶底场序 */
    hi_s32 scene_changed; /* 场景切换信息 */
    hi_s32 film_type;     /* 电影模式 */
    hi_u32 key_value;     /* 输出关键帧信息 */
} ifmd_result;

typedef struct {
    /* 非22模式的灰度直方图（场景切换）和帧间差直方图(sigma) */
    hi_s32 histo_of_newest_field[64]; /* 64: histo_of_newest_field len */
    hi_s32 histo_bin[64];             /* 64: histo_bin len */

    /* 22 PCC数据 */
    hi_s32 pcc_match;
    hi_s32 pcc_non_match;
    hi_s32 pcc_crss;
    hi_s32 pcc_fwd_tkr;
    hi_s32 pcc_bwd_tkr;
    hi_s32 pcc_fwd_tkr_u;
    hi_s32 pcc_bwd_tkr_u;
    hi_s32 pcc_fwd_tkr_d;
    hi_s32 pcc_bwd_tkr_d;
    hi_s32 pcc_fwd_tkr_l;
    hi_s32 pcc_bwd_tkr_l;
    hi_s32 pcc_fwd_tkr_r;
    hi_s32 pcc_bwd_tkr_r;

    /* 22 UM数据 */
    hi_s32 match_um;
    hi_s32 non_match_um;
    hi_s32 match_um2;
    hi_s32 non_match_um2;
    /* 22 ITDiff数据 */
    hi_s32 frm_it_diff;
    hi_s32 frm_it_diff_be;

    /* Comb或PCC直方图输出 */
    hi_s32 comb_histo_bin03[64]; /* 64: comb_histo_bin03 len */
    hi_s32 comb_histo_bin23[64]; /* 64: comb_histo_bin23 len */
    hi_s32 comb_histo_bin12[64]; /* 64: comb_histo_bin12 len */

    /* OSD */
    hi_u8 flag[3]; /* 3: flag len */
    hi_u8 lock;
    hi_u16 i_begin_row[3]; /* 3: i_begin_row len */
    hi_u16 i_no_rows[3];   /* 3: i_no_rows len */
    hi_u16 i_diff_in_osd;
    hi_s32 i_osd_pix_count;
    hi_u8 is_osd_scene;
    hi_s32 max_err_line;
    hi_s32 gmv0_x;
    hi_s32 gmv0_y;
    hi_s32 gmv0_num;
    hi_s32 gmv1_x;
    hi_s32 gmv1_y;
    hi_s32 gmv1_num;
} ifmd_hard2soft_api;

/* fod param */
typedef struct {
    reg_stillblk_stat still_blk_info;
    reg_lasi_stat lasi_stat1;
    reg_lasi_stat lasi_stat2;
} alg_fod_rtl_stttable;

/* still blocks */
typedef struct {
    hi_s32 stillblk_thr; /* 8bit [0:255] */
} stillblk_thd;

/* structure of pulldown software result */
typedef struct {
    hi_bool is2nd_fld;     /* 表示计算统计信息时, 参考场是否为第二场 */
    hi_bool nxt_is2nd_fld; /* 表示驱动下一次配置给逻辑的参考场是否为第二场 */
    hi_bool bt_mode;       /* 表示计算统计信息时, 配置给逻辑的场序 */
    /* 表示计算统计信息时, 配置给逻辑的参考场, 算法人员描述的参考场也是逻辑人员的当前场 */
    hi_bool ref_fld;
    hi_s32 sad_buf[16]; /* 16: sad_buf len */
    hi_s32 sad_diff_acc;
    hi_s32 field_order;
    hi_s32 sad_reg[16][3]; /* 16/3: sad_reg len */
    hi_s32 z32_frms[8];    /* 8: z32_frms len */
    hi_s32 fac_frmsdtv[8]; /* 8: fac_frmsdtv len */
} alg_fod_ctx;

typedef struct {
    hi_u32 init_flag;
    alg_fod_ctx fod_ctx;
    stillblk_thd still_blk_ctrl;
    alg_fod_outpara rtl_out_para_bak;
} alg_fod_softinfo;

typedef struct {
    hi_bool dei_en_lum;
    hi_bool dei_en_chr;
    hi_s32 dei_md_lum; /* 0-5 field; 1-4 filed; 2-3 field; 3-reserved */
    hi_s32 dei_md_chr;
    hi_bool dei_rst; /* 0-reset invalid; 1-reset valid, don't read history motion infomation; */
    /* field order detect enable on-off: 1-enable; 2-disable,forced to top first;
    3-disable,forced to bottom first; 0-disable,default; */
    hi_s32 fod_enable;
    hi_bool pld22_enable;   /* Interleaved/Progressive detect enable on-off: 1-enable; 0-disable */
    hi_bool pld32_enable;   /* pulldown detect enable on-off: 1-enable; 0-disable */
    hi_bool edge_smooth_en; /* edge smooth enable on-off: 1-disable; 0-enable */
    hi_s32 pld22_md;

    hi_s32 frm_height; /* height of source */
    hi_s32 frm_width;  /* width  of source */
    hi_bool bt_mode;   /* 为读统计信息时的那一场的场序， 0 topFirst */
    hi_bool ref_fld;   /* ref field  为读统计信息时的那一场，顶场 配0 当前场为底场 配1 */
} alg_dei_drv_para;

typedef enum {
    ALG_DEI_MODE_5FLD = 0,
    ALG_DEI_MODE_4FLD,
    ALG_DEI_MODE_3FLD,
    ALG_DEI_MODE_MAX
} alg_dei_mode;

hi_s32 pq_hal_set_bt_mode(hi_u32 handle_no, hi_s32 fieldorder);

hi_s32 pq_hal_update_ifmd_api_reg(hi_u32 handle_no, hi_drv_pq_ifmd_playback *sp_ifmd_result,
                                  hi_drv_pq_vpss_stt_info *info_in);

hi_s32 pq_hal_ifmd_white_block_ctrl(hi_u32 handle_no, hi_drv_pq_ifmd_playback *sp_ifmd_result);
hi_s32 pq_hal_set_ifmd_vir_reg(hi_void);
hi_s32 pq_hal_enable_ifmd(hi_bool enable);
hi_s32 pq_hal_set_ifmd_debug_en(hi_bool enable);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
