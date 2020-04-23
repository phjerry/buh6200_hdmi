/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: sr register header file
 * Author: pq
 * Create: 2019-09-29
 */

#ifndef _PQ_HAL_SR_REGSET_H_
#define _PQ_HAL_SR_REGSET_H_

#include "hi_register_vdp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* *******************************SR IP REG*************************************** */
hi_void pq_reg_sr_setoutresult(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 outresult);
hi_void pq_reg_sr_setsren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sr_en);
hi_void pq_reg_sr_setoutresult(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 outresult);
hi_void pq_reg_sr_sethippsrckgten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_sr_ck_gt_en);
hi_void pq_reg_sr_setscalemode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 scale_mode);
hi_void pq_reg_sr_setdemoen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 demo_en);
hi_void pq_reg_sr_setdemomode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 demo_mode);
hi_void pq_reg_sr_setdemopos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 demo_pos);

hi_void pq_reg_sr_setsr2dsrlsmoothdircoef(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sr_2dsr_l_smooth_dir_coef);
hi_void pq_reg_sr_setsr2dsrlsmoothnoncoef(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sr_2dsr_l_smooth_non_coef);
hi_void pq_reg_sr_setsr2dsrcdircoef(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sr_2dsr_c_dir_coef);
hi_void pq_reg_sr_setsr2dsrcnoncoef(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sr_2dsr_c_non_coef);
hi_void pq_reg_sr_setsr2dsrcoefdata(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sr_2dsr_coef_data);
hi_void pq_reg_sr_setdbsrdemoen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_demo_en);
hi_void pq_reg_sr_setdbsrdemomode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_demo_mode);
hi_void pq_reg_sr_setgraphsoften(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 graph_soft_en);
hi_void pq_reg_sr_setparamode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 para_mode);
hi_void pq_reg_sr_setwsumlmt(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 wsum_lmt);
hi_void pq_reg_sr_setfixiwendbsr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fix_iw_en_dbsr);
hi_void pq_reg_sr_setfixiwdbsr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fix_iw_dbsr);
hi_void pq_reg_sr_setcorekmagpdif(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 core_kmag_pdif);
hi_void pq_reg_sr_setcoremadpdif(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 core_mad_pdif);
hi_void pq_reg_sr_setkdifof(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_dif_of);
hi_void pq_reg_sr_setxconfw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x_confw);
hi_void pq_reg_sr_setkconfw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_confw);
hi_void pq_reg_sr_setxofw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x_ofw);
hi_void pq_reg_sr_setkofw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_ofw);
hi_void pq_reg_sr_setxpdifw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x_pdifw);
hi_void pq_reg_sr_setkpdifw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_pdifw);
hi_void pq_reg_sr_setx1magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x1_mag_dbw);
hi_void pq_reg_sr_setx0magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_mag_dbw);
hi_void pq_reg_sr_setk1magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_mag_dbw);
hi_void pq_reg_sr_setk0magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k0_mag_dbw);
hi_void pq_reg_sr_setk2magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k2_mag_dbw);
hi_void pq_reg_sr_setg2magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g2_mag_dbw);
hi_void pq_reg_sr_setg1magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g1_mag_dbw);
hi_void pq_reg_sr_setg0magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_mag_dbw);
hi_void pq_reg_sr_setlmtconflst(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lmt_conf_lst);
hi_void pq_reg_sr_setklmtconf0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_lmt_conf0);
hi_void pq_reg_sr_setk1magkconf(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_magk_conf);
hi_void pq_reg_sr_setg0magkconf(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_magk_conf);
hi_void pq_reg_sr_setx0magkconf(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_magk_conf);
hi_void pq_reg_sr_setk1magwconf(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_magw_conf);
hi_void pq_reg_sr_setg0magwconf(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_magw_conf);
hi_void pq_reg_sr_setx0magwconf(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_magw_conf);
hi_void pq_reg_sr_setx1magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x1_mag_dbw_bld);
hi_void pq_reg_sr_setx0magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_mag_dbw_bld);
hi_void pq_reg_sr_setk1magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_mag_dbw_bld);
hi_void pq_reg_sr_setk0magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k0_mag_dbw_bld);
hi_void pq_reg_sr_setk2magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k2_mag_dbw_bld);
hi_void pq_reg_sr_setg2magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g2_mag_dbw_bld);
hi_void pq_reg_sr_setg1magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g1_mag_dbw_bld);
hi_void pq_reg_sr_setg0magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_mag_dbw_bld);
hi_void pq_reg_sr_setkbpdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_bp_dbw_bld);
hi_void pq_reg_sr_setxbpdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x_bp_dbw_bld);
hi_void pq_reg_sr_setg1magdirwadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g1_mag_dirw_adj);
hi_void pq_reg_sr_setk1magdirwadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_mag_dirw_adj);
hi_void pq_reg_sr_setg0magdirwadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_mag_dirw_adj);
hi_void pq_reg_sr_setx0magdirwadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_mag_dirw_adj);
hi_void pq_reg_sr_setg1magsswadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g1_mag_ssw_adj);
hi_void pq_reg_sr_setk1magsswadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_mag_ssw_adj);
hi_void pq_reg_sr_setg0magsswadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_mag_ssw_adj);
hi_void pq_reg_sr_setx0magsswadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_mag_ssw_adj);
hi_void pq_reg_sr_setg1magsswkadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g1_mag_ssw_kadj);
hi_void pq_reg_sr_setk1magsswkadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_mag_ssw_kadj);
hi_void pq_reg_sr_setg0magsswkadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_mag_ssw_kadj);
hi_void pq_reg_sr_setx0magsswkadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_mag_ssw_kadj);
hi_void pq_reg_sr_setksswdirwadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_ssw_dirw_adj);
hi_void pq_reg_sr_setksswsswadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_ssw_ssw_adj);
hi_void pq_reg_sr_setxdbwsswadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x_dbw_ssw_adj);
hi_void pq_reg_sr_setkminwdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_minw_dbw_bld);
hi_void pq_reg_sr_setkidbwsswbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_idbw_ssw_bld);
hi_void pq_reg_sr_setxidbwsswbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x_idbw_ssw_bld);
hi_void pq_reg_sr_setklpfbpdb(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_lpf_dp_db);
hi_void pq_reg_sr_setcoringbpdb(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 coring_dp_db);
hi_void pq_reg_sr_setksswbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_ssw_bld);
hi_void pq_reg_sr_setbsswbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 b_ssw_bld);
hi_void pq_reg_sr_setkdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_dbw_bld);
hi_void pq_reg_sr_setbdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 b_dbw_bld);
hi_void pq_reg_sr_setc0graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c0_graph);
hi_void pq_reg_sr_setc1graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c1_graph);
hi_void pq_reg_sr_setc0graphsplit(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c0_graph_split);
hi_void pq_reg_sr_setc1graphsplit(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c1_graph_split);
hi_void pq_reg_sr_setr0graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 r0_graph);
hi_void pq_reg_sr_setr1graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 r1_graph);
hi_void pq_reg_sr_setdifth0graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 difth0_graph);
hi_void pq_reg_sr_setdifth1graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 difth1_graph);
hi_void pq_reg_sr_setdifth2graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 difth2_graph);
hi_void pq_reg_sr_setdifth3graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 difth3_graph);
hi_void pq_reg_sr_setcorek1graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 corek1_graph);
hi_void pq_reg_sr_setcorek2graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 corek2_graph);
hi_void pq_reg_sr_setcorek3graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 corek3_graph);
hi_void pq_reg_sr_setx0kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_kk_graph);
hi_void pq_reg_sr_setx1kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x1_kk_graph);
hi_void pq_reg_sr_setx2kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x2_kk_graph);
hi_void pq_reg_sr_setg1kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g1_kk_graph);
hi_void pq_reg_sr_setg2kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g2_kk_graph);
hi_void pq_reg_sr_setg0kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_kk_graph);
hi_void pq_reg_sr_setk3kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k3_kk_graph);
hi_void pq_reg_sr_setg3kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g3_kk_graph);
hi_void pq_reg_sr_setk1kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_kk_graph);
hi_void pq_reg_sr_setk2kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k2_kk_graph);
hi_void pq_reg_sr_setk1graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_graph);
hi_void pq_reg_sr_setk2graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k2_graph);
hi_void pq_reg_sr_setk3graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k3_graph);
hi_void pq_reg_sr_setwgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 w_graph);
hi_void pq_reg_sr_setdbsrbicubicphase03(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase03);
hi_void pq_reg_sr_setdbsrbicubicphase02(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase02);
hi_void pq_reg_sr_setdbsrbicubicphase01(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase01);
hi_void pq_reg_sr_setdbsrbicubicphase00(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase00);
hi_void pq_reg_sr_setdbsrbicubicphase13(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase13);
hi_void pq_reg_sr_setdbsrbicubicphase12(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase12);
hi_void pq_reg_sr_setdbsrbicubicphase11(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase11);
hi_void pq_reg_sr_setdbsrbicubicphase10(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase10);
hi_void pq_reg_sr_setopdstdneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_pdst_dneed);
hi_void pq_reg_sr_setopdstdrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_pdst_drdy);
hi_void pq_reg_sr_setiplrydneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_plry_dneed);
hi_void pq_reg_sr_setiplrydrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_plry_drdy);
hi_void pq_reg_sr_setipsrcylowdneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_psrcy_low_dneed);
hi_void pq_reg_sr_setipsrcylowdrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_psrcy_low_drdy);
hi_void pq_reg_sr_setipsrcyhighdneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_psrcy_high_dneed);
hi_void pq_reg_sr_setipsrcyhighdrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_psrcy_high_drdy);
hi_void pq_reg_sr_setwgtavgcnty(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 wgt_avg_cnt_y);
hi_void pq_reg_sr_setwgtavgcntx(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 wgt_avg_cnt_x);
hi_void pq_reg_sr_setdbsrparacoefdata(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_para_coef_data);
hi_void pq_reg_sr_setosrcdneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_c_dneed);
hi_void pq_reg_sr_setosrcdrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_c_drdy);
hi_void pq_reg_sr_setosrydneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_y_dneed);
hi_void pq_reg_sr_setosrydrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_y_drdy);
hi_void pq_reg_sr_seticlmcdneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_clm_c_dneed);
hi_void pq_reg_sr_seticlmcdrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_clm_c_drdy);
hi_void pq_reg_sr_seticlmydneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_clm_y_dneed);
hi_void pq_reg_sr_seticlmydrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_clm_y_drdy);
hi_void pq_reg_sr_setosrycnty(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_y_cnt_y);
hi_void pq_reg_sr_setosrycntx(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_y_cnt_x);
hi_void pq_reg_sr_setosrccnty(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_c_cnt_y);
hi_void pq_reg_sr_setosrccntx(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_c_cnt_x);
hi_void pq_reg_sr_setwgraphrtl(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 w_graph_rtl);
hi_void pq_reg_sr_setdifstatraw0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 difstat_raw0);
hi_void pq_reg_sr_setdifstatraw1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 difstat_raw1);
hi_void pq_reg_sr_setdifstatraw2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 difstat_raw2);
hi_void pq_reg_sr_setdifstatraw3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 difstat_raw3);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
