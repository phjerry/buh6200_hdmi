/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: dci register header file
 * Author: pq
 * Create: 2019-01-01
 */

#ifndef _PQ_HAL_DCI_REGSET_H_
#define _PQ_HAL_DCI_REGSET_H_

#include "hi_register_vdp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* *******************************DCI PARA REG*************************************** */
hi_void pq_reg_dci_setparaupv0chn(vdp_regs_type *vdp_reg, hi_u32 u32chnnum);
hi_void pq_reg_dci_para_haddr_v0_chn15(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_reg_dci_para_addr_v0_chn15(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_reg_dci_set_link_ctrl(vdp_regs_type *vdp_reg, hi_u32 link_type);

/* *******************************DCI IP REG*************************************** */
hi_void pq_reg_dci_set_en(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 enable);

hi_void pq_reg_dci_set_demo_en(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 demo_en);
hi_void pq_reg_dci_set_demo_mode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 demo_mode);

hi_void pq_reg_dci_set_global_gain0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 global_gain0);
hi_void pq_reg_dci_set_global_gain1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 global_gain1);
hi_void pq_reg_dci_set_global_gain2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 global_gain2);

hi_void pq_reg_dci_set_global_gain0_s0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 global_gain0);
hi_void pq_reg_dci_set_global_gain1_s0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 global_gain1);
hi_void pq_reg_dci_set_global_gain2_s0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 global_gain2);

hi_void pq_reg_dci_set_global_gain0_s1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 global_gain0);
hi_void pq_reg_dci_set_global_gain1_s1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 global_gain1);
hi_void pq_reg_dci_set_global_gain2_s1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 global_gain2);

hi_void pq_reg_dci_get_mean_value(vdp_regs_type *reg, hi_u32 offset, hi_u32 *mean_value);

hi_void pq_reg_dci_set_dci_masic_en(vdp_regs_type *reg, hi_u32 offset, hi_u32 dci_masic_en);
hi_void pq_reg_dci_set_dci_ck_gt_en(vdp_regs_type *reg, hi_u32 offset, hi_u32 dci_ck_gt_en);
hi_void pq_reg_dci_set_scd_en(vdp_regs_type *reg, hi_u32 offset, hi_u32 scd_en);

hi_void pq_reg_dci_set_dci_dither_en(vdp_regs_type *reg, hi_u32 offset, hi_u32 dci_dither_en);
hi_void pq_reg_dci_set_input_full_range(vdp_regs_type *reg, hi_u32 offset, hi_u32 input_full_range);
hi_void pq_reg_dci_set_output_full_range(vdp_regs_type *reg, hi_u32 offset, hi_u32 output_full_range);
hi_void pq_reg_dci_set_demo_split_pos(vdp_regs_type *reg, hi_u32 offset, hi_u32 demo_split_pos);

hi_void pq_reg_dci_set_flat_mode(vdp_regs_type *reg, hi_u32 offset, hi_u32 flat_mode);

hi_void pq_reg_dci_set_global_in_hor_end(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_hor_end);
hi_void pq_reg_dci_set_global_in_hor_start(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_hor_start);
hi_void pq_reg_dci_set_global_in_ver_end(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_ver_end);
hi_void pq_reg_dci_set_global_in_ver_start(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_ver_start);
hi_void pq_reg_dci_set_global_out_hor_end(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_hor_end);
hi_void pq_reg_dci_set_global_out_hor_start(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_hor_start);
hi_void pq_reg_dci_set_global_out_ver_end(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_ver_end);
hi_void pq_reg_dci_set_global_out_ver_start(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_ver_start);

hi_void pq_reg_dci_set_scd_flag(vdp_regs_type *reg, hi_u32 offset, hi_u32 scd_flag);
hi_void pq_reg_dci_get_scd_sad_thrsh(vdp_regs_type *reg, hi_u32 offset, hi_u32 *scd_sad_thrsh);
hi_void pq_reg_dci_get_scd_sad_shift(vdp_regs_type *reg, hi_u32 offset, hi_u32 *scd_sad_shift);

hi_void pq_reg_dci_set_local_hor_end(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_hor_end);
hi_void pq_reg_dci_set_local_hor_start(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_hor_start);

hi_void pq_reg_dci_set_local_ver_end(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_ver_end);
hi_void pq_reg_dci_set_local_ver_start(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_ver_start);

hi_void pq_reg_dci_set_cg_div_width(vdp_regs_type *reg, hi_u32 offset, hi_u32 cg_div_width);
hi_void pq_reg_dci_set_cg_div_height(vdp_regs_type *reg, hi_u32 offset, hi_u32 cg_div_height);

hi_void pq_reg_dci_set_ncount_rshf(vdp_regs_type *reg, hi_u32 offset, hi_u32 ncount_rshf);

hi_void pq_reg_dci_set_glb_adj_lcl_gain(vdp_regs_type *reg, hi_u32 offset, hi_u32 glb_adj_lcl_gain);
hi_void pq_reg_dci_get_bs_min_black_thrsh(vdp_regs_type *reg, hi_u32 offset, hi_u32 *min_black_thrsh);
hi_void pq_reg_dci_set_global_total_pix_num(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_total_pix_num);
hi_void pq_reg_dci_set_global_total_pix_num_bld(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_total_pix_num_bld);

hi_void pq_reg_dci_set_split_point_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 split_point_s0);
hi_void pq_reg_dci_set_split_type_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 split_type_s0);
hi_void pq_reg_dci_set_split_hist_mode_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 split_hist_mode_s0);
hi_void pq_reg_dci_set_demo_side_shift_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 demo_side_shift_s0);
hi_void pq_reg_dci_set_demo_split_pos_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 demo_split_pos_s0);
hi_void pq_reg_dci_set_demo_mode_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 demo_mode_s0);
hi_void pq_reg_dci_set_global_in_hor_end_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_hor_end_s0);
hi_void pq_reg_dci_set_global_in_hor_start_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_hor_start_s0);
hi_void pq_reg_dci_set_global_in_ver_end_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_ver_end_s0);
hi_void pq_reg_dci_set_global_in_ver_start_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_ver_start_s0);
hi_void pq_reg_dci_set_global_out_hor_end_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_hor_end_s0);
hi_void pq_reg_dci_set_global_out_hor_start_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_hor_start_s0);
hi_void pq_reg_dci_set_global_out_ver_end_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_ver_end_s0);
hi_void pq_reg_dci_set_global_out_ver_start_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_ver_start_s0);
hi_void pq_reg_dci_set_local_hor_end_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_hor_end_s0);
hi_void pq_reg_dci_set_local_hor_start_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_hor_start_s0);
hi_void pq_reg_dci_set_local_gain_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_gain_s0);
hi_void pq_reg_dci_set_local_centre_adj_wgt_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_centre_adj_wgt_s0);
hi_void pq_reg_dci_set_local_ver_end_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_ver_end_s0);
hi_void pq_reg_dci_set_local_ver_start_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_ver_start_s0);

hi_void pq_reg_dci_set_split_point_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 split_point_s1);
hi_void pq_reg_dci_set_split_type_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 split_type_s1);
hi_void pq_reg_dci_set_split_hist_mode_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 split_hist_mode_s1);
hi_void pq_reg_dci_set_demo_side_shift_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 demo_side_shift_s1);
hi_void pq_reg_dci_set_demo_split_pos_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 demo_split_pos_s1);
hi_void pq_reg_dci_set_demo_mode_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 demo_mode_s1);
hi_void pq_reg_dci_set_global_in_hor_end_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_hor_end_s1);
hi_void pq_reg_dci_set_global_in_hor_start_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_hor_start_s1);
hi_void pq_reg_dci_set_global_in_ver_end_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_ver_end_s1);
hi_void pq_reg_dci_set_global_in_ver_start_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_ver_start_s1);
hi_void pq_reg_dci_set_global_out_hor_end_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_hor_end_s1);
hi_void pq_reg_dci_set_global_out_hor_start_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_hor_start_s1);
hi_void pq_reg_dci_set_global_out_ver_end_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_ver_end_s1);
hi_void pq_reg_dci_set_global_out_ver_start_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_ver_start_s1);
hi_void pq_reg_dci_set_local_hor_end_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_hor_end_s1);
hi_void pq_reg_dci_set_local_hor_start_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_hor_start_s1);
hi_void pq_reg_dci_set_local_gain_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_gain_s1);
hi_void pq_reg_dci_set_local_centre_adj_wgt_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_centre_adj_wgt_s1);
hi_void pq_reg_dci_set_local_ver_end_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_ver_end_s1);
hi_void pq_reg_dci_set_local_ver_start_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_ver_start_s1);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
