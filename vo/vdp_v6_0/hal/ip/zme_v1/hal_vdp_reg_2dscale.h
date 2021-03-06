/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_REG_2DSCALE_H__
#define __HAL_VDP_REG_2DSCALE_H__

#include "hi_reg_vdp.h"

hi_void vdp_xdp_2dscale_setclk2dscalefixen(vdp_regs_type *vdp_reg, hi_u32 clk_2dscale_fix_en);
hi_void vdp_xdp_2dscale_setclk2dscalediv2en(vdp_regs_type *vdp_reg, hi_u32 clk_2dscale_div2_en);
hi_void vdp_xdp_2dscale_settapreduceen(vdp_regs_type *vdp_reg, hi_u32 tap_reduce_en);
hi_void vdp_xdp_2dscale_setckgten(vdp_regs_type *vdp_reg, hi_u32 ck_gt_en);
hi_void vdp_xdp_2dscale_setoutpro(vdp_regs_type *vdp_reg, hi_u32 out_pro);
hi_void vdp_xdp_2dscale_setoutfmt(vdp_regs_type *vdp_reg, hi_u32 out_fmt);
hi_void vdp_xdp_2dscale_setoutheight(vdp_regs_type *vdp_reg, hi_u32 out_height);
hi_void vdp_xdp_2dscale_setlvsclen(vdp_regs_type *vdp_reg, hi_u32 lv_scl_en);
hi_void vdp_xdp_2dscale_setcvsclen(vdp_regs_type *vdp_reg, hi_u32 cv_scl_en);
hi_void vdp_xdp_2dscale_setscl2den(vdp_regs_type *vdp_reg, hi_u32 scl2d_en);
hi_void vdp_xdp_2dscale_setcvmeden(vdp_regs_type *vdp_reg, hi_u32 cv_med_en);
hi_void vdp_xdp_2dscale_setlvflten(vdp_regs_type *vdp_reg, hi_u32 lv_flt_en);
hi_void vdp_xdp_2dscale_setcvflten(vdp_regs_type *vdp_reg, hi_u32 cv_flt_en);
hi_void vdp_xdp_2dscale_setvratio(vdp_regs_type *g_vdp_reg, hi_u32 vratio);
hi_void vdp_xdp_2dscale_setvlumaoffset(vdp_regs_type *g_vdp_reg, hi_u32 vluma_offset);
hi_void vdp_xdp_2dscale_setvchromaoffset(vdp_regs_type *vdp_reg, hi_u32 vchroma_offset);
hi_void vdp_xdp_2dscale_setvblumaoffset(vdp_regs_type *vdp_reg, hi_u32 vbluma_offset);
hi_void vdp_xdp_2dscale_setvbchromaoffset(vdp_regs_type *vdp_reg, hi_u32 vbchroma_offset);
hi_void vdp_xdp_2dscale_setlvstc2nden(vdp_regs_type *vdp_reg, hi_u32 lv_stc2nd_en);
hi_void vdp_xdp_2dscale_setlvstc2ndusen(vdp_regs_type *vdp_reg, hi_u32 lv_stc2ndus_en);
hi_void vdp_xdp_2dscale_setlvstc2ndosen(vdp_regs_type *vdp_reg, hi_u32 lv_stc2ndos_en);
hi_void vdp_xdp_2dscale_setlvstc2ndflatmode(vdp_regs_type *vdp_reg, hi_u32 lv_stc2nd_flatmode);
hi_void vdp_xdp_2dscale_setlvstc2ndcoradjen(vdp_regs_type *vdp_reg, hi_u32 lv_stc2nd_coradjen);
hi_void vdp_xdp_2dscale_setlvstc2ndgain(vdp_regs_type *vdp_reg, hi_u32 lv_stc2nd_gain);
hi_void vdp_xdp_2dscale_setlvstc2ndcor(vdp_regs_type *vdp_reg, hi_u32 lv_stc2nd_cor);
hi_void vdp_xdp_2dscale_setapbvhdvflren(vdp_regs_type *vdp_reg, hi_u32 apb_vhd_vf_lren);
hi_void vdp_xdp_2dscale_setapbvhdvfcren(vdp_regs_type *vdp_reg, hi_u32 apb_vhd_vf_cren);
hi_void vdp_xdp_2dscale_setapbvhdvcoefraddr(vdp_regs_type *vdp_reg, hi_u32 apb_vhd_vcoef_raddr);
hi_void vdp_xdp_2dscale_setoutwidth(vdp_regs_type *vdp_reg, hi_u32 out_width);
hi_void vdp_xdp_2dscale_setlhsclen(vdp_regs_type *vdp_reg, hi_u32 lh_scl_en);
hi_void vdp_xdp_2dscale_setchsclen(vdp_regs_type *vdp_reg, hi_u32 ch_scl_en);
hi_void vdp_xdp_2dscale_setchmeden(vdp_regs_type *vdp_reg, hi_u32 ch_med_en);
hi_void vdp_xdp_2dscale_setnonlinearsclen(vdp_regs_type *vdp_reg, hi_u32 nonlinear_scl_en);
hi_void vdp_xdp_2dscale_setlhflten(vdp_regs_type *vdp_reg, hi_u32 lh_flt_en);
hi_void vdp_xdp_2dscale_setchflten(vdp_regs_type *vdp_reg, hi_u32 ch_flt_en);
hi_void vdp_xdp_2dscale_sethratio(vdp_regs_type *vdp_reg, hi_u32 hratio);
hi_void vdp_xdp_2dscale_setlhfiroffset(vdp_regs_type *vdp_reg, hi_u32 lhfir_offset);
hi_void vdp_xdp_2dscale_setchfiroffset(vdp_regs_type *vdp_reg, hi_u32 chfir_offset);
hi_void vdp_xdp_2dscale_setzone0delta(vdp_regs_type *g_vdp_reg, hi_u32 zone0_delta);
hi_void vdp_xdp_2dscale_setzone2delta(vdp_regs_type *g_vdp_reg, hi_u32 zone2_delta);
hi_void vdp_xdp_2dscale_setzone1end(vdp_regs_type *g_vdp_reg, hi_u32 zone1_end);
hi_void vdp_xdp_2dscale_setzone0end(vdp_regs_type *g_vdp_reg, hi_u32 zone0_end);
hi_void vdp_xdp_2dscale_setlhstc2nden(vdp_regs_type *vdp_reg, hi_u32 lh_stc2nd_en);
hi_void vdp_xdp_2dscale_setlhstc2ndusen(vdp_regs_type *vdp_reg, hi_u32 lh_stc2ndus_en);
hi_void vdp_xdp_2dscale_setlhstc2ndosen(vdp_regs_type *vdp_reg, hi_u32 lh_stc2ndos_en);
hi_void vdp_xdp_2dscale_setlhstc2ndflatmode(vdp_regs_type *vdp_reg, hi_u32 lh_stc2nd_flatmode);
hi_void vdp_xdp_2dscale_setlhstc2ndcoradjen(vdp_regs_type *vdp_reg, hi_u32 lh_stc2nd_coradjen);
hi_void vdp_xdp_2dscale_setlhstc2ndgain(vdp_regs_type *vdp_reg, hi_u32 lh_stc2nd_gain);
hi_void vdp_xdp_2dscale_setlhstc2ndcor(vdp_regs_type *vdp_reg, hi_u32 lh_stc2nd_cor);
hi_void vdp_xdp_2dscale_setapbvhdhflren(vdp_regs_type *vdp_reg, hi_u32 apb_vhd_hf_lren);
hi_void vdp_xdp_2dscale_setapbvhdhfcren(vdp_regs_type *vdp_reg, hi_u32 apb_vhd_hf_cren);
hi_void vdp_xdp_2dscale_setapbvhdhcoefraddr(vdp_regs_type *vdp_reg, hi_u32 apb_vhd_hcoef_raddr);
hi_void vdp_xdp_2dscale_setlvdren(vdp_regs_type *vdp_reg, hi_u32 lv_dr_en);
hi_void vdp_xdp_2dscale_setlvdiren(vdp_regs_type *vdp_reg, hi_u32 lv_dir_en);
hi_void vdp_xdp_2dscale_setlvreslpfgain(vdp_regs_type *vdp_reg, hi_u32 lv_reslpf_gain);
hi_void vdp_xdp_2dscale_setlvreslpfen(vdp_regs_type *vdp_reg, hi_u32 lv_reslpf_en);
hi_void vdp_xdp_2dscale_setlvprelpfthr(vdp_regs_type *vdp_reg, hi_u32 lv_prelpf_thr);
hi_void vdp_xdp_2dscale_setlvdrgradk(vdp_regs_type *vdp_reg, hi_u32 lv_dr_gradk);
hi_void vdp_xdp_2dscale_setlvdrgradcor(vdp_regs_type *vdp_reg, hi_u32 lv_dr_gradcor);
hi_void vdp_xdp_2dscale_setlvdrlpfgain(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lpfgain);
hi_void vdp_xdp_2dscale_setlhdren(vdp_regs_type *vdp_reg, hi_u32 lh_dr_en);
hi_void vdp_xdp_2dscale_setlhdiren(vdp_regs_type *vdp_reg, hi_u32 lh_dir_en);
hi_void vdp_xdp_2dscale_setlhreslpfgain(vdp_regs_type *vdp_reg, hi_u32 lh_reslpf_gain);
hi_void vdp_xdp_2dscale_setlhreslpfen(vdp_regs_type *vdp_reg, hi_u32 lh_reslpf_en);
hi_void vdp_xdp_2dscale_setlhprelpfthr(vdp_regs_type *vdp_reg, hi_u32 lh_prelpf_thr);
hi_void vdp_xdp_2dscale_setlhdrgradlpfgain(vdp_regs_type *vdp_reg, hi_u32 lh_dr_gradlpfgain);
hi_void vdp_xdp_2dscale_setlhdrgradk(vdp_regs_type *vdp_reg, hi_u32 lh_dr_gradk);
hi_void vdp_xdp_2dscale_setlhdrgradcor(vdp_regs_type *vdp_reg, hi_u32 lh_dr_gradcor);
hi_void vdp_xdp_2dscale_setlhdrlpfgain(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lpfgain);
hi_void vdp_xdp_2dscale_setdemoen(vdp_regs_type *vdp_reg, hi_u32 demo_en);
hi_void vdp_xdp_2dscale_setdemomode(vdp_regs_type *vdp_reg, hi_u32 demo_mode);
hi_void vdp_xdp_2dscale_setdemoincol(vdp_regs_type *vdp_reg, hi_u32 demo_in_col);
hi_void vdp_xdp_2dscale_setdemooutcol(vdp_regs_type *vdp_reg, hi_u32 demo_out_col);
hi_void vdp_xdp_2dscale_setcolend(vdp_regs_type *vdp_reg, hi_u32 col_end);
hi_void vdp_xdp_2dscale_setcolstart(vdp_regs_type *vdp_reg, hi_u32 col_start);
hi_void vdp_xdp_2dscale_setgraphdeten(vdp_regs_type *vdp_reg, hi_u32 graph_det_en);
hi_void vdp_xdp_2dscale_setrowend(vdp_regs_type *vdp_reg, hi_u32 row_end);
hi_void vdp_xdp_2dscale_setrowstart(vdp_regs_type *vdp_reg, hi_u32 row_start);
hi_void vdp_xdp_2dscale_setdivheight(vdp_regs_type *vdp_reg, hi_u32 div_height);
hi_void vdp_xdp_2dscale_setdivwidth(vdp_regs_type *vdp_reg, hi_u32 div_width);
hi_void vdp_xdp_2dscale_setdifth3graph(vdp_regs_type *vdp_reg, hi_u32 difth3_graph);
hi_void vdp_xdp_2dscale_setdifth2graph(vdp_regs_type *vdp_reg, hi_u32 difth2_graph);
hi_void vdp_xdp_2dscale_setdifth1graph(vdp_regs_type *vdp_reg, hi_u32 difth1_graph);
hi_void vdp_xdp_2dscale_setdifth0graph(vdp_regs_type *vdp_reg, hi_u32 difth0_graph);
hi_void vdp_xdp_2dscale_setcorek3graph(vdp_regs_type *vdp_reg, hi_u32 corek3_graph);
hi_void vdp_xdp_2dscale_setcorek2graph(vdp_regs_type *vdp_reg, hi_u32 corek2_graph);
hi_void vdp_xdp_2dscale_setcorek1graph(vdp_regs_type *vdp_reg, hi_u32 corek1_graph);
hi_void vdp_xdp_2dscale_setx1kkgraph(vdp_regs_type *g_vdp_reg, hi_u32 x1_kk_graph);
hi_void vdp_xdp_2dscale_setx0kkgraph(vdp_regs_type *g_vdp_reg, hi_u32 x0_kk_graph);
hi_void vdp_xdp_2dscale_setg0kkgraph(vdp_regs_type *vdp_reg, hi_u32 g0_kk_graph);
hi_void vdp_xdp_2dscale_setx2kkgraph(vdp_regs_type *g_vdp_reg, hi_u32 x2_kk_graph);
hi_void vdp_xdp_2dscale_setg2kkgraph(vdp_regs_type *vdp_reg, hi_u32 g2_kk_graph);
hi_void vdp_xdp_2dscale_setg1kkgraph(vdp_regs_type *vdp_reg, hi_u32 g1_kk_graph);
hi_void vdp_xdp_2dscale_setk1kkgraph(vdp_regs_type *vdp_reg, hi_u32 k1_kk_graph);
hi_void vdp_xdp_2dscale_setg3kkgraph(vdp_regs_type *vdp_reg, hi_u32 g3_kk_graph);
hi_void vdp_xdp_2dscale_setk3kkgraph(vdp_regs_type *vdp_reg, hi_u32 k3_kk_graph);
hi_void vdp_xdp_2dscale_setk2kkgraph(vdp_regs_type *vdp_reg, hi_u32 k2_kk_graph);
hi_void vdp_xdp_2dscale_setk3graph(vdp_regs_type *vdp_reg, hi_u32 k3_graph);
hi_void vdp_xdp_2dscale_setk2graph(vdp_regs_type *vdp_reg, hi_u32 k2_graph);
hi_void vdp_xdp_2dscale_setk1graph(vdp_regs_type *vdp_reg, hi_u32 k1_graph);
hi_void vdp_xdp_2dscale_setshiftgsgraph(vdp_regs_type *vdp_reg, hi_u32 shift_gs_graph);
hi_void vdp_xdp_2dscale_setx1gsgraph(vdp_regs_type *g_vdp_reg, hi_u32 x1_gs_graph);
hi_void vdp_xdp_2dscale_setx0gsgraph(vdp_regs_type *g_vdp_reg, hi_u32 x0_gs_graph);
hi_void vdp_xdp_2dscale_setgraphpsb(vdp_regs_type *vdp_reg, hi_u32 graph_psb);
hi_void vdp_xdp_2dscale_setgscoremode(vdp_regs_type *vdp_reg, hi_u32 gscore_mode);
hi_void vdp_xdp_2dscale_setgscorecfg(vdp_regs_type *vdp_reg, hi_u32 gscore_cfg);
hi_void vdp_xdp_2dscale_setlvweightneiblmtgain(vdp_regs_type *vdp_reg, hi_u32 lv_weight_neiblmt_gain);
hi_void vdp_xdp_2dscale_setlvsadmononeibgain(vdp_regs_type *vdp_reg, hi_u32 lv_sadmono_neibgain);
hi_void vdp_xdp_2dscale_setlvweightnongain(vdp_regs_type *vdp_reg, hi_u32 lv_weightnon_gain);
hi_void vdp_xdp_2dscale_setlvsadneibbldgain(vdp_regs_type *vdp_reg, hi_u32 lv_sad_neibbld_gain);
hi_void vdp_xdp_2dscale_setlvweightnonb(vdp_regs_type *vdp_reg, hi_u32 lv_weight_non_b);
hi_void vdp_xdp_2dscale_setlvsadmonoaddk(vdp_regs_type *vdp_reg, hi_u32 lv_sadmono_add_k);
hi_void vdp_xdp_2dscale_setlvsaddifthr(vdp_regs_type *vdp_reg, hi_u32 lv_sad_dif_thr);
hi_void vdp_xdp_2dscale_setlvsadavgthr(vdp_regs_type *vdp_reg, hi_u32 lv_sad_avg_thr);
hi_void vdp_xdp_2dscale_setlvminsadcor(vdp_regs_type *vdp_reg, hi_u32 lv_minsad_cor);
hi_void vdp_xdp_2dscale_setlvsadamendlmtk(vdp_regs_type *vdp_reg, hi_u32 lv_sad_amendlmt_k);
hi_void vdp_xdp_2dscale_setlvsadamendlmtb(vdp_regs_type *vdp_reg, hi_u32 lv_sad_amendlmt_b);
hi_void vdp_xdp_2dscale_setlvsaddifhvk(vdp_regs_type *vdp_reg, hi_u32 lv_sad_difhv_k);
hi_void vdp_xdp_2dscale_setlvdirstopnum(vdp_regs_type *vdp_reg, hi_u32 lv_dir_stop_num);
hi_void vdp_xdp_2dscale_setlvsadmonopunishen(vdp_regs_type *vdp_reg, hi_u32 lv_sad_monopunish_en);
hi_void vdp_xdp_2dscale_setlvdirweightmixmode(vdp_regs_type *vdp_reg, hi_u32 lv_dir_weight_mixmode);
hi_void vdp_xdp_2dscale_setlvdirhvdiaglmten(vdp_regs_type *vdp_reg, hi_u32 lv_dir_hvdiaglmt_en);
hi_void vdp_xdp_2dscale_setlvdirhvlmten(vdp_regs_type *vdp_reg, hi_u32 lv_dir_hvlmt_en);
hi_void vdp_xdp_2dscale_setlvdirtblmten(vdp_regs_type *vdp_reg, hi_u32 lv_dir_tblmt_en);
hi_void vdp_xdp_2dscale_setlvdirweightmaxk(vdp_regs_type *vdp_reg, hi_u32 lv_dir_weightmax_k);
hi_void vdp_xdp_2dscale_setlvminsadgain(vdp_regs_type *vdp_reg, hi_u32 lv_minsad_gain);
hi_void vdp_xdp_2dscale_setlvweightbldgmin(vdp_regs_type *vdp_reg, hi_u32 lv_weight_bld_gmin);
hi_void vdp_xdp_2dscale_setlvweightbldgmax(vdp_regs_type *vdp_reg, hi_u32 lv_weight_bld_gmax);
hi_void vdp_xdp_2dscale_setlvweightbldk(vdp_regs_type *vdp_reg, hi_u32 lv_weight_bld_k);
hi_void vdp_xdp_2dscale_setlvweightbldcor(vdp_regs_type *vdp_reg, hi_u32 lv_weight_bld_cor);
hi_void vdp_xdp_2dscale_setlvdenselinetype(vdp_regs_type *vdp_reg, hi_u32 lv_denseline_type);
hi_void vdp_xdp_2dscale_setlvdenselinecor(vdp_regs_type *vdp_reg, hi_u32 lv_denseline_cor);
hi_void vdp_xdp_2dscale_setlvdenselinecork(vdp_regs_type *vdp_reg, hi_u32 lv_denseline_cork);
hi_void vdp_xdp_2dscale_setlvdenselinegain(vdp_regs_type *vdp_reg, hi_u32 lv_denseline_gain);
hi_void vdp_xdp_2dscale_setlvdenselineflattype(vdp_regs_type *vdp_reg, hi_u32 lv_denseline_flattype);
hi_void vdp_xdp_2dscale_setlvdenselinewintype(vdp_regs_type *vdp_reg, hi_u32 lv_denseline_wintype);
hi_void vdp_xdp_2dscale_setlvdenselinedeten(vdp_regs_type *vdp_reg, hi_u32 lv_denseline_det_en);
hi_void vdp_xdp_2dscale_setlvdirgain(vdp_regs_type *vdp_reg, hi_u32 lv_dir_gain);
hi_void vdp_xdp_2dscale_setlvdirweightlpfgain(vdp_regs_type *vdp_reg, hi_u32 lv_dir_weightlpf_gain);
hi_void vdp_xdp_2dscale_setlvdirweightlpfneibmode(vdp_regs_type *vdp_reg, hi_u32 lv_dir_weightlpf_neibmode);
hi_void vdp_xdp_2dscale_setlvdirweightlpfen(vdp_regs_type *vdp_reg, hi_u32 lv_dir_weightlpf_en);
hi_void vdp_xdp_2dscale_setlvdrlut103(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_03);
hi_void vdp_xdp_2dscale_setlvdrlut102(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_02);
hi_void vdp_xdp_2dscale_setlvdrlut101(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_01);
hi_void vdp_xdp_2dscale_setlvdrlut100(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_00);
hi_void vdp_xdp_2dscale_setlvdrlut107(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_07);
hi_void vdp_xdp_2dscale_setlvdrlut106(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_06);
hi_void vdp_xdp_2dscale_setlvdrlut105(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_05);
hi_void vdp_xdp_2dscale_setlvdrlut104(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_04);
hi_void vdp_xdp_2dscale_setlvdrlut111(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_11);
hi_void vdp_xdp_2dscale_setlvdrlut110(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_10);
hi_void vdp_xdp_2dscale_setlvdrlut109(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_09);
hi_void vdp_xdp_2dscale_setlvdrlut108(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_08);
hi_void vdp_xdp_2dscale_setlvdrlut115(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_15);
hi_void vdp_xdp_2dscale_setlvdrlut114(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_14);
hi_void vdp_xdp_2dscale_setlvdrlut113(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_13);
hi_void vdp_xdp_2dscale_setlvdrlut112(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_12);
hi_void vdp_xdp_2dscale_setlvdrlut203(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_03);
hi_void vdp_xdp_2dscale_setlvdrlut202(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_02);
hi_void vdp_xdp_2dscale_setlvdrlut201(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_01);
hi_void vdp_xdp_2dscale_setlvdrlut200(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_00);
hi_void vdp_xdp_2dscale_setlvdrlut207(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_07);
hi_void vdp_xdp_2dscale_setlvdrlut206(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_06);
hi_void vdp_xdp_2dscale_setlvdrlut205(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_05);
hi_void vdp_xdp_2dscale_setlvdrlut204(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_04);
hi_void vdp_xdp_2dscale_setlvdrlut211(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_11);
hi_void vdp_xdp_2dscale_setlvdrlut210(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_10);
hi_void vdp_xdp_2dscale_setlvdrlut209(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_09);
hi_void vdp_xdp_2dscale_setlvdrlut208(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_08);
hi_void vdp_xdp_2dscale_setlvdrlut215(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_15);
hi_void vdp_xdp_2dscale_setlvdrlut214(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_14);
hi_void vdp_xdp_2dscale_setlvdrlut213(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_13);
hi_void vdp_xdp_2dscale_setlvdrlut212(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_12);
hi_void vdp_xdp_2dscale_setlhweightneiblmtgain(vdp_regs_type *vdp_reg, hi_u32 lh_weight_neiblmt_gain);
hi_void vdp_xdp_2dscale_setlhsadmononeibgain(vdp_regs_type *vdp_reg, hi_u32 lh_sadmono_neibgain);
hi_void vdp_xdp_2dscale_setlhweightnongain(vdp_regs_type *vdp_reg, hi_u32 lh_weightnon_gain);
hi_void vdp_xdp_2dscale_setlhsadneibbldgain(vdp_regs_type *vdp_reg, hi_u32 lh_sad_neibbld_gain);
hi_void vdp_xdp_2dscale_setlhweightnonb(vdp_regs_type *vdp_reg, hi_u32 lh_weight_non_b);
hi_void vdp_xdp_2dscale_setlhsadmonoaddk(vdp_regs_type *vdp_reg, hi_u32 lh_sadmono_add_k);
hi_void vdp_xdp_2dscale_setlhsaddifthr(vdp_regs_type *vdp_reg, hi_u32 lh_sad_dif_thr);
hi_void vdp_xdp_2dscale_setlhsadavgthr(vdp_regs_type *vdp_reg, hi_u32 lh_sad_avg_thr);
hi_void vdp_xdp_2dscale_setlhminsadcor(vdp_regs_type *vdp_reg, hi_u32 lh_minsad_cor);
hi_void vdp_xdp_2dscale_setlhsadamendlmtk(vdp_regs_type *vdp_reg, hi_u32 lh_sad_amendlmt_k);
hi_void vdp_xdp_2dscale_setlhsadamendlmtb(vdp_regs_type *vdp_reg, hi_u32 lh_sad_amendlmt_b);
hi_void vdp_xdp_2dscale_setlhsaddifhvk(vdp_regs_type *vdp_reg, hi_u32 lh_sad_difhv_k);
hi_void vdp_xdp_2dscale_setlhdirstopnum(vdp_regs_type *vdp_reg, hi_u32 lh_dir_stop_num);
hi_void vdp_xdp_2dscale_setlhsadmonopunishen(vdp_regs_type *vdp_reg, hi_u32 lh_sad_monopunish_en);
hi_void vdp_xdp_2dscale_setlhdirweightmixmode(vdp_regs_type *vdp_reg, hi_u32 lh_dir_weight_mixmode);
hi_void vdp_xdp_2dscale_setlhdirhvdiaglmten(vdp_regs_type *vdp_reg, hi_u32 lh_dir_hvdiaglmt_en);
hi_void vdp_xdp_2dscale_setlhdirhvlmten(vdp_regs_type *vdp_reg, hi_u32 lh_dir_hvlmt_en);
hi_void vdp_xdp_2dscale_setlhdirtblmten(vdp_regs_type *vdp_reg, hi_u32 lh_dir_tblmt_en);
hi_void vdp_xdp_2dscale_setlhdirweightmaxk(vdp_regs_type *vdp_reg, hi_u32 lh_dir_weightmax_k);
hi_void vdp_xdp_2dscale_setlhminsadgain(vdp_regs_type *vdp_reg, hi_u32 lh_minsad_gain);
hi_void vdp_xdp_2dscale_setlhweightbldgmin(vdp_regs_type *vdp_reg, hi_u32 lh_weight_bld_gmin);
hi_void vdp_xdp_2dscale_setlhweightbldgmax(vdp_regs_type *vdp_reg, hi_u32 lh_weight_bld_gmax);
hi_void vdp_xdp_2dscale_setlhweightbldk(vdp_regs_type *vdp_reg, hi_u32 lh_weight_bld_k);
hi_void vdp_xdp_2dscale_setlhweightbldcor(vdp_regs_type *vdp_reg, hi_u32 lh_weight_bld_cor);
hi_void vdp_xdp_2dscale_setlhdenselinetype(vdp_regs_type *vdp_reg, hi_u32 lh_denseline_type);
hi_void vdp_xdp_2dscale_setlhdenselinecor(vdp_regs_type *vdp_reg, hi_u32 lh_denseline_cor);
hi_void vdp_xdp_2dscale_setlhdenselinecork(vdp_regs_type *vdp_reg, hi_u32 lh_denseline_cork);
hi_void vdp_xdp_2dscale_setlhdenselinegain(vdp_regs_type *vdp_reg, hi_u32 lh_denseline_gain);
hi_void vdp_xdp_2dscale_setlhdenselineflattype(vdp_regs_type *vdp_reg, hi_u32 lh_denseline_flattype);
hi_void vdp_xdp_2dscale_setlhdenselinewintype(vdp_regs_type *vdp_reg, hi_u32 lh_denseline_wintype);
hi_void vdp_xdp_2dscale_setlhdenselinedeten(vdp_regs_type *vdp_reg, hi_u32 lh_denseline_det_en);
hi_void vdp_xdp_2dscale_setlhdirgain(vdp_regs_type *vdp_reg, hi_u32 lh_dir_gain);
hi_void vdp_xdp_2dscale_setlhdirweightlpfgain(vdp_regs_type *vdp_reg, hi_u32 lh_dir_weightlpf_gain);
hi_void vdp_xdp_2dscale_setlhdirweightlpfneibmode(vdp_regs_type *vdp_reg, hi_u32 lh_dir_weightlpf_neibmode);
hi_void vdp_xdp_2dscale_setlhdirweightlpfen(vdp_regs_type *vdp_reg, hi_u32 lh_dir_weightlpf_en);
hi_void vdp_xdp_2dscale_setlhdrlut103(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_03);
hi_void vdp_xdp_2dscale_setlhdrlut102(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_02);
hi_void vdp_xdp_2dscale_setlhdrlut101(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_01);
hi_void vdp_xdp_2dscale_setlhdrlut100(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_00);
hi_void vdp_xdp_2dscale_setlhdrlut107(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_07);
hi_void vdp_xdp_2dscale_setlhdrlut106(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_06);
hi_void vdp_xdp_2dscale_setlhdrlut105(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_05);
hi_void vdp_xdp_2dscale_setlhdrlut104(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_04);
hi_void vdp_xdp_2dscale_setlhdrlut111(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_11);
hi_void vdp_xdp_2dscale_setlhdrlut110(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_10);
hi_void vdp_xdp_2dscale_setlhdrlut109(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_09);
hi_void vdp_xdp_2dscale_setlhdrlut108(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_08);
hi_void vdp_xdp_2dscale_setlhdrlut115(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_15);
hi_void vdp_xdp_2dscale_setlhdrlut114(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_14);
hi_void vdp_xdp_2dscale_setlhdrlut113(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_13);
hi_void vdp_xdp_2dscale_setlhdrlut112(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_12);
hi_void vdp_xdp_2dscale_setlhdrlut203(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_03);
hi_void vdp_xdp_2dscale_setlhdrlut202(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_02);
hi_void vdp_xdp_2dscale_setlhdrlut201(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_01);
hi_void vdp_xdp_2dscale_setlhdrlut200(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_00);
hi_void vdp_xdp_2dscale_setlhdrlut207(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_07);
hi_void vdp_xdp_2dscale_setlhdrlut206(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_06);
hi_void vdp_xdp_2dscale_setlhdrlut205(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_05);
hi_void vdp_xdp_2dscale_setlhdrlut204(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_04);
hi_void vdp_xdp_2dscale_setlhdrlut211(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_11);
hi_void vdp_xdp_2dscale_setlhdrlut210(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_10);
hi_void vdp_xdp_2dscale_setlhdrlut209(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_09);
hi_void vdp_xdp_2dscale_setlhdrlut208(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_08);
hi_void vdp_xdp_2dscale_setlhdrlut215(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_15);
hi_void vdp_xdp_2dscale_setlhdrlut214(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_14);
hi_void vdp_xdp_2dscale_setlhdrlut213(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_13);
hi_void vdp_xdp_2dscale_setlhdrlut212(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_12);

#endif


