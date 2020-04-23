/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_adapter.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/01
 */
#ifndef __HAL_VPSS_ADAPTER_H__
#define __HAL_VPSS_ADAPTER_H__

#include "hal_vpss_reg.h"
#include "hal_vpss_zme_inst_para.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define align_16btye(x)             ((((x) + 15) / 16) * 16)
#define align_64btye(x)             ((((x) + 63) / 64) * 64)
#define align_10bit_16_btye(x)      ((((x) * 10 + 127) / 128) * 16)
#define align_8bit_16_btye(x)       ((((x) * 8 + 127) / 128) * 16)
#define align_anybw_16btye(x, bitw) ((((x) * (bitw) + 127) / 128) * 16)
#define XDP_MAX_NODE_NUM            4
#define XDP_MAX_SCAN_NUM            5
#define SCAN_OFFSET                 0xe000 /* different : VPSS_ME_CF_CTRL */
#define VPSS_REG_OFF 4

typedef enum {
    XDP_3DRS_MA = 0,
    XDP_3DRS_MC = 1,
    XDP_3DRS_VER_MAX
} xdp_3_drs_version;

typedef enum {
    XDP_ME_VER3 = 0,
    XDP_ME_VER4,
    XDP_ME_VER5,

    XDP_ME_VER_MAX
} xdp_me_version;

typedef struct {
    hi_u32 width;
    hi_u32 height;
    hi_u32 fr_width;
    hi_u32 fr_height;
    hi_u32 up_smp;
    hi_u32 dn_smp;
    hi_u32 layer;
    hi_u32 pro;
    hi_u32 me_en;
    hi_u32 me_scan;
    hi_bool meds_en;
    hi_bool me_cf_online_en;
    xdp_me_version me_version;
    hi_u32 scan_no;
    hi_u32 frm_no;
} vpss_me_cfg;

typedef struct {
    hi_u32 meds_en;
    xdp_3_drs_version en3_drs_version;
    xdp_me_version me_version;
    hi_u32 me_scan_num;
    hi_u32 pro;
    hi_void *stt_cfg;
    hi_void *wbc_cfg;
    hi_void *vc1_info;
} vpss_mc_info;

hi_void mc_set(vpss_reg_type *vpss_regs, hi_u64 node_phy_addr,
               vpss_mc_info *mc_info, vpss_mac_rchn_cfg *rchn_cfg, vpss_mac_wchn_cfg *wchn_cfg);

hi_void mc_set_scan_addr(vpss_reg_type *vpss_regs, hi_u64 node_phy_addr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif


