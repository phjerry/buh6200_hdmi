/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: cle register header file
 * Author: pq
 * Create: 2019-09-21
 */

#ifndef _PQ_HAL_CLE_REGSET_H_
#define _PQ_HAL_CLE_REGSET_H_

#include "hi_register_vdp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* *******************************CLE PARA REG*************************************** */
hi_void pq_reg_cle_setparaupv0chn(vdp_regs_type *vdp_reg, hi_u32 u32chnnum);
hi_void pq_reg_cle_set_8k_cle_para_src(vdp_regs_type *vdp_reg, hi_u32 src_mode);

hi_void pq_reg_cle_para_addr_v0_chn20(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void pq_reg_cle_para_addr_v0_chn22(vdp_regs_type *vdp_reg, hi_u32 data);

/* *******************************CLE IP REG*************************************** */
hi_void pq_reg_cle_set_lti_limit_clip_en(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 lti_limit_clip_en);
hi_void pq_reg_cle_set_enltih(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 enltih);
hi_void pq_reg_cle_set_cle_en(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 cle_en);
hi_void pq_reg_cle_set_demo_pos(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 demo_pos);
hi_void pq_reg_cle_set_demo_mode(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 demo_mode);
hi_void pq_reg_cle_set_demo_en(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 demo_en);

hi_void pq_reg_cle_set_raisr_gain(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 raisr_gain);
hi_void pq_reg_cle_set_nlsr_gain(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 nlsr_gain);
hi_void pq_reg_cle_set_peakgain(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 peakgain);

hi_void pq_reg_cle_set_shp_clut_rd_en(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 shp_clut_rd_en);
hi_void pq_reg_cle_set_clut_addr(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 clut_addr);
hi_void pq_reg_cle_set_clut_rdata(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 clut_rdata);
hi_void pq_reg_cle_set_cle_ck_gt_en(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 cle_ck_gt_en);
hi_void pq_reg_cle_set_peak_limit_clip_en(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 peak_limit_clip_en);
hi_void pq_reg_cle_set_contrastbit(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 contrastbit);
hi_void pq_reg_cle_set_contrastthr(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 contrastthr);
hi_void pq_reg_cle_set_flatbit(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 flatbit);
hi_void pq_reg_cle_set_flatthrl(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 flatthrl);
hi_void pq_reg_cle_set_a1slp(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 a1slp);
hi_void pq_reg_cle_set_kneg(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 kneg);
hi_void pq_reg_cle_set_a1thrl(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 a1thrl);
hi_void pq_reg_cle_set_gradmonoslp(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 gradmonoslp);
hi_void pq_reg_cle_set_gradmonomin(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 gradmonomin);
hi_void pq_reg_cle_set_gradmonomax(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 gradmonomax);
hi_void pq_reg_cle_set_pixmonothr(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 pixmonothr);
hi_void pq_reg_cle_set_ltiratio(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 ltiratio);
hi_void pq_reg_cle_set_ltigain(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 ltigain);

hi_void pq_reg_cle_set_stcv_sclwgtratio(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 stcv_sclwgtratio);

hi_void pq_reg_cle_set_skin_yvalue_mixratio(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 skin_yvalue_mixratio);

hi_void pq_reg_cle_set_face_num(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 face_num);
hi_void pq_reg_cle_set_face_confidence_en(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 face_confidence_en);
hi_void pq_reg_cle_set_facepos01(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos0_1);
hi_void pq_reg_cle_set_facepos00(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos0_0);
hi_void pq_reg_cle_set_facepos04(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos0_4);
hi_void pq_reg_cle_set_facepos03(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos0_3);
hi_void pq_reg_cle_set_facepos02(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos0_2);
hi_void pq_reg_cle_set_facepos11(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos1_1);
hi_void pq_reg_cle_set_facepos10(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos1_0);
hi_void pq_reg_cle_set_facepos14(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos1_4);
hi_void pq_reg_cle_set_facepos13(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos1_3);
hi_void pq_reg_cle_set_facepos12(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos1_2);
hi_void pq_reg_cle_set_facepos21(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos2_1);
hi_void pq_reg_cle_set_facepos20(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos2_0);
hi_void pq_reg_cle_set_facepos24(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos2_4);
hi_void pq_reg_cle_set_facepos23(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos2_3);
hi_void pq_reg_cle_set_facepos22(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos2_2);
hi_void pq_reg_cle_set_facepos31(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos3_1);
hi_void pq_reg_cle_set_facepos30(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos3_0);
hi_void pq_reg_cle_set_facepos34(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos3_4);
hi_void pq_reg_cle_set_facepos33(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos3_3);
hi_void pq_reg_cle_set_facepos32(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos3_2);
hi_void pq_reg_cle_set_facepos41(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos4_1);
hi_void pq_reg_cle_set_facepos40(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos4_0);
hi_void pq_reg_cle_set_facepos44(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos4_4);
hi_void pq_reg_cle_set_facepos43(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos4_3);
hi_void pq_reg_cle_set_facepos42(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos4_2);
hi_void pq_reg_cle_set_facepos51(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos5_1);
hi_void pq_reg_cle_set_facepos50(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos5_0);
hi_void pq_reg_cle_set_facepos54(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos5_4);
hi_void pq_reg_cle_set_facepos53(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos5_3);
hi_void pq_reg_cle_set_facepos52(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos5_2);
hi_void pq_reg_cle_set_facepos61(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos6_1);
hi_void pq_reg_cle_set_facepos60(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos6_0);
hi_void pq_reg_cle_set_facepos64(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos6_4);
hi_void pq_reg_cle_set_facepos63(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos6_3);
hi_void pq_reg_cle_set_facepos62(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos6_2);
hi_void pq_reg_cle_set_facepos71(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos7_1);
hi_void pq_reg_cle_set_facepos70(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos7_0);
hi_void pq_reg_cle_set_facepos74(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos7_4);
hi_void pq_reg_cle_set_facepos73(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos7_3);
hi_void pq_reg_cle_set_facepos72(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos7_2);
hi_void pq_reg_cle_set_facepos81(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos8_1);
hi_void pq_reg_cle_set_facepos80(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos8_0);
hi_void pq_reg_cle_set_facepos84(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos8_4);
hi_void pq_reg_cle_set_facepos83(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos8_3);
hi_void pq_reg_cle_set_facepos82(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos8_2);
hi_void pq_reg_cle_set_facepos91(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos9_1);
hi_void pq_reg_cle_set_facepos90(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos9_0);
hi_void pq_reg_cle_set_facepos94(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos9_4);
hi_void pq_reg_cle_set_facepos93(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos9_3);
hi_void pq_reg_cle_set_facepos92(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos9_2);
hi_void pq_reg_cle_set_facepos101(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos10_1);
hi_void pq_reg_cle_set_facepos100(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos10_0);
hi_void pq_reg_cle_set_facepos104(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos10_4);
hi_void pq_reg_cle_set_facepos103(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos10_3);
hi_void pq_reg_cle_set_facepos102(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos10_2);
hi_void pq_reg_cle_set_facepos111(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos11_1);
hi_void pq_reg_cle_set_facepos110(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos11_0);
hi_void pq_reg_cle_set_facepos114(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos11_4);
hi_void pq_reg_cle_set_facepos113(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos11_3);
hi_void pq_reg_cle_set_facepos112(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos11_2);
hi_void pq_reg_cle_set_facepos121(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos12_1);
hi_void pq_reg_cle_set_facepos120(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos12_0);
hi_void pq_reg_cle_set_facepos124(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos12_4);
hi_void pq_reg_cle_set_facepos123(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos12_3);
hi_void pq_reg_cle_set_facepos122(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos12_2);
hi_void pq_reg_cle_set_facepos131(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos13_1);
hi_void pq_reg_cle_set_facepos130(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos13_0);
hi_void pq_reg_cle_set_facepos134(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos13_4);
hi_void pq_reg_cle_set_facepos133(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos13_3);
hi_void pq_reg_cle_set_facepos132(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos13_2);
hi_void pq_reg_cle_set_facepos141(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos14_1);
hi_void pq_reg_cle_set_facepos140(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos14_0);
hi_void pq_reg_cle_set_facepos144(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos14_4);
hi_void pq_reg_cle_set_facepos143(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos14_3);
hi_void pq_reg_cle_set_facepos142(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos14_2);
hi_void pq_reg_cle_set_facepos151(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos15_1);
hi_void pq_reg_cle_set_facepos150(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos15_0);
hi_void pq_reg_cle_set_facepos154(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos15_4);
hi_void pq_reg_cle_set_facepos153(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos15_3);
hi_void pq_reg_cle_set_facepos152(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos15_2);
hi_void pq_reg_cle_set_facepos161(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos16_1);
hi_void pq_reg_cle_set_facepos160(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos16_0);
hi_void pq_reg_cle_set_facepos164(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos16_4);
hi_void pq_reg_cle_set_facepos163(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos16_3);
hi_void pq_reg_cle_set_facepos162(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos16_2);
hi_void pq_reg_cle_set_facepos171(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos17_1);
hi_void pq_reg_cle_set_facepos170(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos17_0);
hi_void pq_reg_cle_set_facepos174(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos17_4);
hi_void pq_reg_cle_set_facepos173(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos17_3);
hi_void pq_reg_cle_set_facepos172(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos17_2);
hi_void pq_reg_cle_set_facepos181(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos18_1);
hi_void pq_reg_cle_set_facepos180(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos18_0);
hi_void pq_reg_cle_set_facepos184(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos18_4);
hi_void pq_reg_cle_set_facepos183(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos18_3);
hi_void pq_reg_cle_set_facepos182(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos18_2);
hi_void pq_reg_cle_set_facepos191(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos19_1);
hi_void pq_reg_cle_set_facepos190(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos19_0);
hi_void pq_reg_cle_set_facepos194(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos19_4);
hi_void pq_reg_cle_set_facepos193(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos19_3);
hi_void pq_reg_cle_set_facepos192(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos19_2);





#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
