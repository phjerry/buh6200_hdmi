/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_limit.h hander file vpss limit
 * Author: zhangjunyu
 * Create: 2016/07/03
 */

#ifndef __HAL_VPSS_PQBIN_H__
#define __HAL_VPSS_PQBIN_H__

#include "vpss_comm.h"
#include "hal_vpss_reg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
typedef enum {
    // write chn
    XDP_LAYER_ID_OUT0 = 0,
    XDP_LAYER_ID_OUT3,
    XDP_LAYER_ID_NRRFR,
    XDP_LAYER_ID_NRRFRH,
    XDP_LAYER_ID_NRRFRHV,
    XDP_LAYER_ID_CCRFR,
    XDP_LAYER_ID_CCRFR1,
    XDP_LAYER_ID_DIRFR,
    XDP_LAYER_ID_NRWMAD,
    XDP_LAYER_ID_NRWCNT,
    XDP_LAYER_ID_CCWCCNT,
    XDP_LAYER_ID_CCWYCNT,
    XDP_LAYER_ID_DIWSADY,
    XDP_LAYER_ID_DIWSADC,
    XDP_LAYER_ID_DIWHISM,
    XDP_LAYER_ID_RGWPRJH,
    XDP_LAYER_ID_RGWPRJV,
    XDP_LAYER_ID_RGCFRGMV,

    XDP_LAYER_ID_RSTT,
    XDP_LAYER_ID_WSTT,
    XDP_LAYER_ID_CHKSUM,

    XDP_LAYER_ID_MECF,
    XDP_LAYER_ID_MEREF,
    XDP_LAYER_ID_MEP1MV,
    XDP_LAYER_ID_MEPRMV,
    XDP_LAYER_ID_MEP1GMV,
    XDP_LAYER_ID_MEPRGMV,
    XDP_LAYER_ID_MEP1RGMV,
    XDP_LAYER_ID_MEP2RGMV,

    XDP_LAYER_ID_MECFMV_1,
    XDP_LAYER_ID_MECFGMV_1,
    XDP_LAYER_ID_MECFMV_2,
    XDP_LAYER_ID_MECFGMV_2,
    XDP_LAYER_ID_MECFMV_3,
    XDP_LAYER_ID_MECFGMV_3,
    XDP_LAYER_ID_MECFMV_4,
    XDP_LAYER_ID_MECFGMV_4,
    XDP_LAYER_ID_MECFMV_5,
    XDP_LAYER_ID_MECFGMV_5,
    XDP_LAYER_ID_ME_STT_1,
    XDP_LAYER_ID_ME_STT_2,
    XDP_LAYER_ID_ME_STT_3,
    XDP_LAYER_ID_ME_STT_4,
    XDP_LAYER_ID_ME_STT_5,

    XDP_LAYER_ID_MAX
} vpss_golden_layer_id;

typedef enum {
    XDP_LUM_ADDR = 0,
    XDP_CHM_ADDR,

    XDP_HEAD_LUM_ADDR,
    XDP_HEAD_CHM_ADDR,
    XDP_CHMCR_ADDR,
    XDP_BUF_ADDR_MAX
} vpss_golden_buf_addr_type;

typedef struct {
    hi_u64 phy_addr;
    hi_u8 *vir_addr;
} vpss_golden_addr;

#define vpss_golden_check_null_ptr(ptr)                                                                         \
    do {                                                                                                        \
        if ((ptr) == HI_NULL) {                                                                                 \
            osal_printk("\n NULL_PTR failed at:\n >File name:%s\n >Function:%s\n >Line No.:%d\n",               \
                         __FILE__, __FUNCTION__, __LINE__);                                                     \
            do_exit(-1);                                                                                        \
        }                                                                                                       \
    } while (0)
#define vpss_golden_assert(expr)                                                                                \
    do {                                                                                                        \
        if (!(expr)) {                                                                                          \
            osal_printk("\nASSERT failed at:\n >File name:%s\n >Function:%s\n >Line No.:%d\n >Condition: %s\n", \
                         __FILE__, __FUNCTION__, __LINE__, #expr);                                              \
            do_exit(-1);                                                                                        \
        }                                                                                                       \
    } while (0)

hi_void vpss_hal_refresh_golden_reg_table(vpss_reg_type *vpss_reg, vpss_mac_rchn_cfg *rchn_cfg,
        vpss_mac_wchn_cfg *wchn_cfg);
hi_s32 vpss_hal_write_chn_file(vpss_mac_wchn_cfg *wchn_cfg);
hi_void vpss_hal_refresh_die_reg_table(vpss_reg_type *vpss_reg, vpss_mac_rchn_cfg *rchn_cfg,
                                       vpss_mac_wchn_cfg *wchn_cfg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif





