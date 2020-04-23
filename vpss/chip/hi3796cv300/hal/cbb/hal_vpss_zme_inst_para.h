/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_zme_inst_para.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/03
 */
#ifndef __HAL_VPSS_ZME_INST_PARA_H__
#define __HAL_VPSS_ZME_INST_PARA_H__

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#define VPSS_HZME_PRECISION (1 << 20)
#define VPSS_VZME_PRECISION (1 << 12)

typedef struct {
    hi_u32 offset;
    hi_u32 zme_y_en;
    hi_u32 zme_c_en;
    hi_u32 zme_dw;
    hi_u32 zme_cw;
    hi_u32 zme_tap_hl;
    hi_u32 zme_tap_hc;
    hi_u32 zme_tap_vl;
    hi_u32 zme_tap_vc;
    hi_u32 zme_coeff_norm;
    hi_u32 zme_phase;
    hi_u32 zme_hl_rat;
    hi_u32 zme_hc_rat;
    hi_u32 zme_vl_rat;
    hi_u32 zme_vc_rat;

    hi_u32 zme_y_coef_fix;
    hi_u32 zme_y_coef_set;
    hi_u32 zme_c_coef_fix;
    hi_u32 zme_c_coef_set;
    hi_u32 max_iw;
    hi_u32 max_ih;
    hi_u32 max_ow;
    hi_u32 max_oh;
} xdp_zme_inst_para;

typedef enum {
    XDP_ZME_ID_VPSS_WR0_HDS_HZME = 0,
    XDP_ZME_ID_VPSS_WR1_HVDS_HZME = 1,
    XDP_ZME_ID_VPSS_WR1_HVDS_VZME = 2,
    XDP_ZME_ID_VPSS_ME_CF_HDS_HZME = 3,
    XDP_ZME_ID_VPSS_ME_P2_HDS_HZME = 4,
    XDP_ZME_ID_VPSS_CH_HZME = 5,   /* 主通路缩放 */
    XDP_ZME_ID_VPSS_CH_VZME = 6,   /* 主通路缩放 */
    XDP_ZME_ID_VPSS_OUT2_HZME = 7, /* dc zme */
    XDP_ZME_ID_VPSS_OUT2_VZME = 8, /* dc zme */
    XDP_ZME_ID_VPSS_HDR_IFIR = 9,
    XDP_ZME_ID_VPSS_HDR_HDS = 10,
    XDP_ZME_ID_VPSS_HDR_VDS = 11,
    XDP_ZME_ID_VPSS_NPU_HZME = 12,
    XDP_ZME_ID_VPSS_NPU_VZME = 13,

    XDP_ZME_ID_MAX
} xdp_zme_id;

hi_void vpss_get_zme_inst_para(hi_u32 id, xdp_zme_inst_para *zme_inst_para);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  // __VDP_ZME_H__

