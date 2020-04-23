/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hdr regset api
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __HAL_PQ_HDR_REGSET_H__
#define __HAL_PQ_HDR_REGSET_H__

#include "hi_register_vdp.h"

/* *******************************HDR PARA REG*************************************** */
hi_void pq_para_setparaupv0chn(vdp_regs_type *vdp_reg, hi_u32 u32chnnum);
hi_void pq_para_setparaupvhdchn(vdp_regs_type *vdp_reg, hi_u32 u32chnnum);
hi_void pq_para_set_para_haddr_v0_chn02(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_para_set_para_addr_v0_chn02(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_para_set_para_haddr_v0_chn03(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_para_set_para_addr_v0_chn03(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_para_set_para_haddr_v0_chn04(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_para_set_para_addr_v0_chn04(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_para_set_para_haddr_v0_chn06(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_para_set_para_addr_v0_chn06(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_para_set_para_haddr_v0_chn07(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_para_set_para_addr_v0_chn07(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_para_set_para_addr_vhd_chn07(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_para_set_para_haddr_vhd_chn07(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_para_set_para_addr_vhd_chn08(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_para_set_para_haddr_vhd_chn08(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_para_set_para_addr_vhd_chn19(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_para_set_para_haddr_vhd_chn19(vdp_regs_type *vdp_reg, hi_u32 data);

/* *******************************HDR CM REG*************************************** */
hi_void pq_vhdr_cm_setvhdrcmckgten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_cm_ck_gt_en);
hi_void pq_vhdr_cm_setvhdrcmen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_cm_en);
hi_void pq_vhdr_cm_setvhdrcmclipcmax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_cm_clip_c_max);
hi_void pq_vhdr_cm_setvhdrcmclipcmin(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_cm_clip_c_min);
hi_void pq_vhdr_cm_setvhdrcmscale2p(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_cm_scale2p);
hi_void pq_vhdr_cm_setvcmx4step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x4_step);
hi_void pq_vhdr_cm_setvcmx3step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x3_step);
hi_void pq_vhdr_cm_setvcmx2step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x2_step);
hi_void pq_vhdr_cm_setvcmx1step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x1_step);
hi_void pq_vhdr_cm_setvcmx8step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x8_step);
hi_void pq_vhdr_cm_setvcmx7step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x7_step);
hi_void pq_vhdr_cm_setvcmx6step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x6_step);
hi_void pq_vhdr_cm_setvcmx5step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x5_step);
hi_void pq_vhdr_cm_setvcmx1pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x1_pos);
hi_void pq_vhdr_cm_setvcmx2pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x2_pos);
hi_void pq_vhdr_cm_setvcmx3pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x3_pos);
hi_void pq_vhdr_cm_setvcmx4pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x4_pos);
hi_void pq_vhdr_cm_setvcmx5pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x5_pos);
hi_void pq_vhdr_cm_setvcmx6pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x6_pos);
hi_void pq_vhdr_cm_setvcmx7pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x7_pos);
hi_void pq_vhdr_cm_setvcmx8pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x8_pos);
hi_void pq_vhdr_cm_setvcmx4num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x4_num);
hi_void pq_vhdr_cm_setvcmx3num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x3_num);
hi_void pq_vhdr_cm_setvcmx2num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x2_num);
hi_void pq_vhdr_cm_setvcmx1num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x1_num);
hi_void pq_vhdr_cm_setvcmx8num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x8_num);
hi_void pq_vhdr_cm_setvcmx7num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x7_num);
hi_void pq_vhdr_cm_setvcmx6num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x6_num);
hi_void pq_vhdr_cm_setvcmx5num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x5_num);
/* *******************************HDR V2 REG*************************************** */
hi_void pq_hdr_setvcvmipt2lmsen(vdp_regs_type *vdp_reg, hi_u32 v_ipt2lms_en);
hi_void pq_hdr_setvcvmlms2rgben(vdp_regs_type *vdp_reg, hi_u32 v_lms2rgb_en);
hi_void pq_hdr_setvcvmrgb2yuven(vdp_regs_type *vdp_reg, hi_u32 v_rgb2yuv_en);
hi_void pq_hdr_setvcvmomaprangemin(vdp_regs_type *vdp_reg, hi_u32 v_omap_range_min);
hi_void pq_hdr_setvcvmomaprangeover(vdp_regs_type *vdp_reg, hi_u32 v_omap_range_over);
hi_void pq_hdr_setcvmoutbits(vdp_regs_type *vdp_reg, hi_u32 cvm_out_bits);
hi_void pq_hdr_setcvmoutcolor(vdp_regs_type *vdp_reg, hi_u32 cvm_out_color);
hi_void pq_hdr_setvcvmomapiptoff(vdp_regs_type *vdp_reg, hi_s32 omap_iptoff[3]); /* 3 is reg index */
hi_void pq_hdr_setvcvmomapiptscale(vdp_regs_type *vdp_reg, hi_u32 scale);
hi_void pq_hdr_setvdmomaplms2rgbmin(vdp_regs_type *vdp_reg, hi_u32 v_omap_lms2rgbmin);
hi_void pq_hdr_setvdmomaplms2rgbmax(vdp_regs_type *vdp_reg, hi_u32 v_omap_lms2rgbmax);
hi_void pq_hdr_setvdmomapipt2lmsmin(vdp_regs_type *vdp_reg, hi_u32 v_omap_ipt2lmsmin);
hi_void pq_hdr_setvdmomapipt2lmsmax(vdp_regs_type *vdp_reg, hi_u32 v_omap_ipt2lmsmax);
hi_void pq_hdr_setvcvmomapipt2lmsoutdc(vdp_regs_type *vdp_reg, hi_s32 *v_omap_ipt2lms_out_dc);
hi_void pq_hdr_setvcvmomapipt2lmsindc(vdp_regs_type *vdp_reg, hi_s32 *v_omap_ipt2lms_in_dc);
hi_void pq_hdr_setvcvmomapipt2lms(vdp_regs_type *vdp_reg, hi_s16 v_omap_ipt2lms[3][3]); /* 3 is reg index */
hi_void pq_hdr_setvcvmomapipt2lmsscale2p(vdp_regs_type *vdp_reg, hi_u32 v_omap_ipt2lmsscale2p);
hi_void pq_hdr_setvcvmomaplms2rgb(vdp_regs_type *vdp_reg, hi_s16 v_omap_lms2rgb[3][3]);     /* 3 is reg index */
hi_void pq_hdr_setvcvmomaplms2rgbdemo(vdp_regs_type *vdp_reg, hi_s16 v_omap_lms2rgb[3][3]); /* 3 is reg index */
hi_void pq_hdr_setvcvmomaplms2rgbscale2p(vdp_regs_type *vdp_reg, hi_u32 v_omap_lms2rgbscale2p);
hi_void pq_hdr_setvcvmomaplms2rgbscale2pdemo(vdp_regs_type *vdp_reg, hi_u32 v_omap_lms2rgbscale2p);
hi_void pq_hdr_setvcvmomaprgb2yuv(vdp_regs_type *vdp_reg, hi_s16 v_omap_rgb2yuv[3][3]); /* 3 is reg index */
hi_void pq_hdr_setvcvmomaprgb2yuvscale2p(vdp_regs_type *vdp_reg, hi_u32 v_omap_rgb2yuvscale2p);
hi_void pq_hdr_setvcvmomaprgb2yuvoutdc(vdp_regs_type *vdp_reg, hi_s32 *v_omap_rgb2yuv_out_dc);
hi_void pq_hdr_setvcvmomaprgb2yuvmin(vdp_regs_type *vdp_reg, hi_u32 v_omap_rgb2yuvmin);
hi_void pq_hdr_setvcvmomaprgb2yuvmax(vdp_regs_type *vdp_reg, hi_u32 v_omap_rgb2yuvmax);
hi_void pq_hdr_setvcvmomaprgb2yuvindc(vdp_regs_type *vdp_reg, hi_s32 *v_omap_rgb2yuv_in_dc);
hi_void pq_hdr_setvcvmomaprangeover(vdp_regs_type *vdp_reg, hi_u32 v_omap_range_over);
hi_void pq_hdr_setvcvmomaprangemin(vdp_regs_type *vdp_reg, hi_u32 v_omap_range_min);

hi_void pq_hipp_dbhdr_setomen(vdp_regs_type *vdp_reg, hi_u32 om_en);
hi_void pq_hipp_dbhdr_setomu2sen(vdp_regs_type *vdp_reg, hi_u32 om_u2s_en);
hi_void pq_hipp_dbhdr_setomlshiften(vdp_regs_type *vdp_reg, hi_u32 om_lshift_en);
hi_void pq_hipp_dbhdr_setcvm1ipt2lmsen(vdp_regs_type *vdp_reg, hi_u32 cvm1_ipt2lms_en);
hi_void pq_hipp_dbhdr_setomdegammaen(vdp_regs_type *vdp_reg, hi_u32 om_degamma_en);
hi_void pq_hipp_dbhdr_setcvm1lms2rgben(vdp_regs_type *vdp_reg, hi_u32 cvm1_lms2rgb_en);
hi_void pq_hipp_dbhdr_setomgammaen(vdp_regs_type *vdp_reg, hi_u32 om_gamma_en);
hi_void pq_hipp_dbhdr_setvdenormen(vdp_regs_type *vdp_reg, hi_u32 v_denorm_en);
hi_void pq_hipp_dbhdr_setcvm1rgb2yuven(vdp_regs_type *vdp_reg, hi_u32 cvm1_rgb2yuv_en);
hi_void pq_hdr_setvcvmoutoetf(vdp_regs_type *vdp_reg, hi_u32 cvm_out_oetf);
hi_void pq_hipp_dbhdr_setomcmen(vdp_regs_type *vdp_reg, hi_u32 om_cm_en);
hi_void pq_hipp_dbhdr_setomcmckgten(vdp_regs_type *vdp_reg, hi_u32 om_cm_ck_gt_en);
hi_void pq_hipp_dbhdr_setomcmbitdepthinmode(vdp_regs_type *vdp_reg, hi_u32 om_cm_bitdepth_in_mode);
hi_void pq_hipp_dbhdr_setomcmbitdepthoutmode(vdp_regs_type *vdp_reg, hi_u32 om_cm_bitdepth_out_mode);
hi_void pq_hipp_dbhdr_setomcmdemoen(vdp_regs_type *vdp_reg, hi_u32 om_cm_demo_en);
hi_void pq_hipp_dbhdr_setomcmdemomode(vdp_regs_type *vdp_reg, hi_u32 om_cm_demo_mode);
hi_void pq_hipp_dbhdr_setomcmdemopos(vdp_regs_type *vdp_reg, hi_u32 om_cm_demo_pos);
hi_void pq_hipp_dbhdr_setomvcmen(vdp_regs_type *vdp_reg, hi_u32 om_vcvm_en);
/* *******************************TMAP V2 REG************************************************ */
hi_void pq_tmap_v2_settmrshiftrounden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tm_rshift_round_en);
hi_void pq_tmap_v2_setstbtonemapen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 stb_tonemap_en);
hi_void pq_tmap_v2_settmapsclutrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_sclut_rd_en);
hi_void pq_tmap_v2_settmapsslutrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_sslut_rd_en);
hi_void pq_tmap_v2_settmaptslutrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_tslut_rd_en);
hi_void pq_tmap_v2_settmapsilutrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_silut_rd_en);
hi_void pq_tmap_v2_settmaptilutrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_tilut_rd_en);
hi_void pq_tmap_v2_settmapparardata(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_para_rdata);
hi_void pq_tmap_v2_settmc1expan(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tm_c1_expan);
hi_void pq_tmap_v2_settmsmcenable(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tm_smc_enable);
hi_void pq_tmap_v2_setimtmrshiftbit(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imtm_rshift_bit);
hi_void pq_tmap_v2_setimtmrshiften(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imtm_rshift_en);
hi_void pq_tmap_v2_setimtms2uen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imtm_s2u_en);
/* *************************MAIN REG*********************************************** */
hi_void pq_hal_set_hdr_cfg(hi_drv_pq_xdr_layer_id layer, pq_hal_hdr_cfg *hdr_info);
hi_s32 pq_cfg_distribute_addr(pq_hdr_coef_addr *vdp_coef_buf_addr);
#endif