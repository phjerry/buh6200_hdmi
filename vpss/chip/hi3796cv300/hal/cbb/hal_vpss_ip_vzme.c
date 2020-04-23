/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: vpss hal_vpss_ip_vzme
 * Author: zhangjunyu
 * Create: 2016/07/01
 */

#include "vpss_comm.h"
#include "hal_vpss_ip_vzme.h"  // file gen
#include "hal_vpss_ip_hzme.h"  // file gen
#include "hal_vpss_zme_inst_para.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void vpss_func_set_vzme_mode(vpss_reg_type *vpss_reg, hi_u32 id, vpss_vzme_mode vzme_mode,
                                vpss_vzme_cfg *cfg, vpss_vzme_pq *pq_cfg)
{
    hi_u32 offset;
    xdp_zme_inst_para zme_inst_para = { 0 };
    hi_u32 zme_vprec = VPSS_VZME_PRECISION;
    hi_u32 out_fmt;
    hi_u32 ck_gt_en;
    hi_u32 out_height;
    hi_u32 lvfir_en;
    hi_u32 cvfir_en;
    hi_u32 lvmid_en;
    hi_u32 cvmid_en;
    hi_u32 lvfir_mode;
    hi_u32 cvfir_mode;
    hi_u32 vratio;
    hi_u32 lvfir_offset;
    hi_u32 cvfir_offset;
    hi_u32 lvfir_boffset;
    hi_u32 cvfir_boffset;
    hi_u32 vl_shootctrl_en;
    hi_u32 vl_shootctrl_mode;
    hi_u32 vl_flatdect_mode;
    hi_u32 vl_coringadj_en;
    hi_u32 vl_gain;
    hi_u32 vl_coring;
    hi_u32 vc_shootctrl_en;
    hi_u32 vc_shootctrl_mode;
    hi_u32 vc_flatdect_mode;
    hi_u32 vc_coringadj_en;
    hi_u32 vc_gain;
    hi_u32 vc_coring;
    vpss_get_zme_inst_para(id, &zme_inst_para);
    offset = zme_inst_para.offset;

    if (vzme_mode == VPSS_VZME_TYP) {
        // typ mode
        vl_shootctrl_mode = 0;
        vl_flatdect_mode = 0;
        vl_coringadj_en = 0;
        vl_gain = 0;
        vl_coring = 0;
        vc_shootctrl_mode = 0;
        vc_flatdect_mode = 0;
        vc_coringadj_en = 0;
        vc_gain = 0;
        vc_coring = 0;
    } else {
        osal_printk("VPSS_FUNC_SetVzmeMode enter into typ mode, mode %d\n", vzme_mode);
    }

    out_height = cfg->out_height - 1;
    out_fmt = cfg->out_fmt;
    ck_gt_en = cfg->ck_gt_en;
    lvfir_en = cfg->lv_fir_en;
    lvfir_mode = cfg->lv_fir_mode;
    cvfir_en = cfg->cv_fir_en;
    cvfir_mode = cfg->cv_fir_mode;
    // set PQ cfg
    vl_shootctrl_en = pq_cfg->lv_stc2nd_en;
    vc_shootctrl_en = pq_cfg->cv_stc2nd_en;
    lvmid_en = pq_cfg->lv_med_en;
    cvmid_en = pq_cfg->cv_med_en;

    if (cfg->out_height == 0) {
        vpss_error("zme out_h is zero ! \n");
    }

    vratio = osal_div64_u64((cfg->in_height * zme_vprec), cfg->out_height);

    if (id == XDP_ZME_ID_VPSS_OUT2_VZME) {
        vratio = (zme_vprec) / 4; /* 4 : dc zme fixed 4 times */
    }

    lvfir_offset = pq_cfg->lvfir_offset << 12; /* 12:para */
    cvfir_offset = pq_cfg->cvfir_offset << 12; /* 12:para */
    lvfir_boffset = pq_cfg->lvfir_boffset << 12; /* 12:para */
    cvfir_boffset = pq_cfg->cvfir_boffset << 12; /* 12:para */

    if (cfg->lv_fir_en == 1) {
        if (cfg->in_height % 2 != 0) { /* 2,0:para */
            vpss_error("in_height is not 2 align ! \n");
        }
    }

    // shell add
    // drv transfer
    vpss_vzme_set_out_fmt(vpss_reg, offset, out_fmt);
    vpss_vzme_set_ck_gt_en(vpss_reg, offset, ck_gt_en);
    vpss_vzme_set_out_height(vpss_reg, offset, out_height);
    vpss_vzme_set_lvfir_en(vpss_reg, offset, lvfir_en);
    vpss_vzme_set_cvfir_en(vpss_reg, offset, cvfir_en);
    vpss_vzme_set_lvmid_en(vpss_reg, offset, lvmid_en);
    vpss_vzme_set_cvmid_en(vpss_reg, offset, cvmid_en);
    vpss_vzme_set_lvfir_mode(vpss_reg, offset, lvfir_mode);
    vpss_vzme_set_cvfir_mode(vpss_reg, offset, cvfir_mode);
    vpss_vzme_set_vratio(vpss_reg, offset, vratio);
    vpss_vzme_set_vluma_offset(vpss_reg, offset, lvfir_offset);
    vpss_vzme_set_vchroma_offset(vpss_reg, offset, cvfir_offset);
    vpss_vzme_set_vbluma_offset(vpss_reg, offset, lvfir_boffset);
    vpss_vzme_set_vbchroma_offset(vpss_reg, offset, cvfir_boffset);
    vpss_vzme_set_vl_shootctrl_en(vpss_reg, offset, vl_shootctrl_en);
    vpss_vzme_set_vl_shootctrl_mode(vpss_reg, offset, vl_shootctrl_mode);
    vpss_vzme_set_vl_flatdect_mode(vpss_reg, offset, vl_flatdect_mode);
    vpss_vzme_set_vl_coringadj_en(vpss_reg, offset, vl_coringadj_en);
    vpss_vzme_set_vl_gain(vpss_reg, offset, vl_gain);
    vpss_vzme_set_vl_coring(vpss_reg, offset, vl_coring);
    vpss_vzme_set_vc_shootctrl_en(vpss_reg, offset, vc_shootctrl_en);
    vpss_vzme_set_vc_shootctrl_mode(vpss_reg, offset, vc_shootctrl_mode);
    vpss_vzme_set_vc_flatdect_mode(vpss_reg, offset, vc_flatdect_mode);
    vpss_vzme_set_vc_coringadj_en(vpss_reg, offset, vc_coringadj_en);
    vpss_vzme_set_vc_gain(vpss_reg, offset, vc_gain);
    vpss_vzme_set_vc_coring(vpss_reg, offset, vc_coring);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


