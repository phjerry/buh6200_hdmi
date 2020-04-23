/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: vpss hal
 * Author: zhangjunyu
 * Create: 2016/07/01
 */

#include "hal_vpss.h"
#include "hal_vpss_reg.h"
#include "hal_vpss_adapter.h"
#include "hal_vpss_limit.h"
#include "vpss_policy.h"
#include "hal_vpss_define.h"
#include "hal_vpss_ip_hzme.h"
#include "hal_vpss_ip_vzme.h"
#include "hal_vpss_para.h"
#ifdef HI_TEE_SUPPORT
#include "hi_drv_ssm.h"
#endif

#if (defined HI_VPSS_DRV_USE_GOLDEN) || (defined HI_VPSS_DRV_USE_GOLDEN_COEF)
#include "hal_vpss_pqbin.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DEF_VPSS_HAL_PORT_NUM             2
#define VPSS_HAL_MAX_PZME_MULTIPLY        8
#define HAL_VPSS_MALLOC_NODEBUFF_ONCE_NUM 1
#define HAL_VPSS_MALLOC_NODEBUFF_INIT_NUM 2
#define HAL_VPSS_SAFE_MODE                0X0
#define HAL_VPSS_NOSAFE_MODE              0X2
#define HAL_VPSS_SEL_MA                   0X0
#define HAL_VPSS_SEL_MC                   0X1
#define HAL_VPSS_ME_V3                    0X0
#define HAL_VPSS_ME_V4                    0X1
#define HAL_VPSS_ME_V5                    0X2
#define HAL_SUPPORT_CCCLCOUNT             8
#define HAL_TASK_NODE_MAX                 16

#define HAL_VERSION     0x96C3
#define VPSS0_BASE_ADDR 0x01300000
#define VPSS1_BASE_ADDR 0x01320000
#define VPSS_REG_SIZE   ((sizeof(vpss_reg_type) + 15) / 16 * 16)
#define HAL_FHD_WIDTH   1920
#define HAL_FHD_HEIGHT  1088

#define HAL_UHD_WIDTH  4096
#define HAL_UHD_HEIGHT 2304

#define HAL_CF_WIDTH 960

#define HAL_NODE_END_INT_MASK     0x800
#define HAL_TUNNEL_INT_MASK       0x200
#define HAL_BUS0_READ_INT_MASK    0x40
#define HAL_BUS0_WRITE_INT_MASK   0x20
#define HAL_NODE_TIMEOUT_INT_MASK 0x10
#define HAL_SUCCESS_INT_MASK      0x4

typedef enum {
    HAL_PARA_CHN_NONE = 0,

    HAL_PARA_CHN_HZME = 1 << 1,
    HAL_PARA_CHN_VZME = 1 << 2,

    HAL_PARA_CHN_HDRCM = 1 << 4,
    HAL_PARA_CHN_HDRDEGAMMA = 1 << 5,
    HAL_PARA_CHN_HDRGAMMA = 1 << 6,
    HAL_PARA_CHN_HDRTMAP = 1 << 7,
    HAL_PARA_CHN_HDRCACM = 1 << 8,

    HAL_PARA_CHN_MAX
} hal_para_chn;

#define vpss_hal_check_ip_vaild(ip)                       \
    do {                                                  \
        if (((ip) != VPSS_IP_0) && ((ip) != VPSS_IP_1)) { \
            vpss_error("VPSS IP%d, is Not Vaild\n", ip);  \
            return HI_FAILURE;                            \
        }                                                 \
    } while (0)

#define vpss_hal_check_init(init)                 \
    do {                                          \
        if ((init) == HI_FALSE) {                 \
            vpss_error("VPSS HAL Is Not Init\n"); \
            return HI_FAILURE;                    \
        }                                         \
    } while (0)

#define vpss_hal_check_null_ptr(ptr)          \
    do {                                      \
        if ((ptr) == HI_NULL) {               \
            vpss_error("pointer is NULL!\n"); \
            return HI_FAILURE;                \
        }                                     \
    } while (0)

typedef struct {
    hi_drv_pixel_format in_format;
    hi_drv_pixel_format out_format;
    hi_bool uv_invert;
} vpss_hal_uv;

typedef struct {
    hi_bool used;
    hi_u64 phy_addr;
    hi_u64 vir_addr;
    drv_vpss_mem_info reg_buf;
    list node;
} vpss_hal_buff;

typedef struct {
    drv_vpss_mem_attr attr;
    drv_vpss_mem_info vpss_mem;
    list node;
} vpss_hal_split_node_buffer;

typedef struct {
    hi_bool init;
    hi_bool init_en;
    hi_u32 logic_version;
    hi_u32 base_reg_phy;
    hi_u64 base_reg_vir;
    list buffer_list;
    list buffer_loading_list;
    list split_node_buf_empty_list;
    list split_node_buf_full_list;

    hi_bool use_pq_module;
} vpss_hal_ctx;

static vpss_hal_ctx g_hal_ctx[VPSS_IP_MAX] = {
    {
        .init = HI_FALSE,
        .init_en = HI_FALSE,
        .logic_version = HAL_VERSION,
        .base_reg_phy = VPSS0_BASE_ADDR,
        .base_reg_vir = 0,
    },

    {
        .init = HI_FALSE,
        .init_en = HI_FALSE,
        .logic_version = HAL_VERSION,
        .base_reg_phy = VPSS1_BASE_ADDR,
        .base_reg_vir = 0,
    }
};

static vpss_hal_uv g_uv_invert[] = {
    { HI_DRV_PIXEL_FMT_NV21,     HI_DRV_PIXEL_FMT_NV12,     HI_TRUE },
    { HI_DRV_PIXEL_FMT_NV21,     HI_DRV_PIXEL_FMT_NV21,     HI_FALSE },
    { HI_DRV_PIXEL_FMT_NV21,     HI_DRV_PIXEL_FMT_NV16_2X1, HI_TRUE },
    { HI_DRV_PIXEL_FMT_NV21,     HI_DRV_PIXEL_FMT_NV61_2X1, HI_FALSE },
    { HI_DRV_PIXEL_FMT_NV21,     HI_DRV_PIXEL_FMT_NV16,     HI_TRUE },
    { HI_DRV_PIXEL_FMT_NV21,     HI_DRV_PIXEL_FMT_NV61,     HI_FALSE },

    { HI_DRV_PIXEL_FMT_NV12,     HI_DRV_PIXEL_FMT_NV12,     HI_FALSE },
    { HI_DRV_PIXEL_FMT_NV12,     HI_DRV_PIXEL_FMT_NV21,     HI_TRUE },
    { HI_DRV_PIXEL_FMT_NV12,     HI_DRV_PIXEL_FMT_NV16_2X1, HI_FALSE },
    { HI_DRV_PIXEL_FMT_NV12,     HI_DRV_PIXEL_FMT_NV61_2X1, HI_TRUE },
    { HI_DRV_PIXEL_FMT_NV12,     HI_DRV_PIXEL_FMT_NV16,     HI_FALSE },
    { HI_DRV_PIXEL_FMT_NV12,     HI_DRV_PIXEL_FMT_NV61,     HI_TRUE },

    { HI_DRV_PIXEL_FMT_NV61_2X1, HI_DRV_PIXEL_FMT_NV61_2X1, HI_FALSE },
    { HI_DRV_PIXEL_FMT_NV16_2X1, HI_DRV_PIXEL_FMT_NV16_2X1, HI_FALSE },
    { HI_DRV_PIXEL_FMT_NV16_2X1, HI_DRV_PIXEL_FMT_NV61_2X1, HI_TRUE },
    { HI_DRV_PIXEL_FMT_NV61,     HI_DRV_PIXEL_FMT_NV61_2X1, HI_FALSE },
    { HI_DRV_PIXEL_FMT_NV16,     HI_DRV_PIXEL_FMT_NV16,     HI_FALSE },
    { HI_DRV_PIXEL_FMT_NV16,     HI_DRV_PIXEL_FMT_NV61_2X1, HI_TRUE },
    { HI_DRV_PIXEL_FMT_NV16,     HI_DRV_PIXEL_FMT_NV12,     HI_FALSE },
    { HI_DRV_PIXEL_FMT_NV16_2X1, HI_DRV_PIXEL_FMT_NV12,     HI_FALSE },
    { HI_DRV_PIXEL_FMT_NV61,     HI_DRV_PIXEL_FMT_NV21,     HI_FALSE },
    { HI_DRV_PIXEL_FMT_NV61_2X1, HI_DRV_PIXEL_FMT_NV21,     HI_FALSE },
};
static vpss_mac_rchn_cfg g_rchn_cfg[VPSS_MAC_RCHN_MAX];
static vpss_mac_wchn_cfg g_wchn_cfg[VPSS_MAC_WCHN_MAX];

static hi_void set_vc1_reg(vpss_reg_type *vpss_reg, vpss_mac_rchn layer, hi_drv_vc1_range_info *vc1_info)
{
    vpss_vc1_set_vc1_mapc(vpss_reg, 0, vc1_info->range_mapuv);
    vpss_vc1_set_vc1_mapy(vpss_reg, 0, vc1_info->range_mapy);
    vpss_vc1_set_vc1_mapcflg(vpss_reg, 0, vc1_info->range_mapuv_flag);
    vpss_vc1_set_vc1_mapyflg(vpss_reg, 0, vc1_info->range_mapy_flag);
    vpss_vc1_set_vc1_rangedfrm(vpss_reg, 0, vc1_info->ranged_frm);
    vpss_vc1_set_vc1_profile(vpss_reg, 0, vc1_info->vc1_profile);

    return;
}

hi_void set_vc1_cfg(vpss_reg_type *vpss_reg, vpss_hal_info *hal_info, vpss_mac_rchn layer)
{
    hi_drv_vc1_range_info *vc1_info;
    hi_bool vc1_en = HI_FALSE;

    if ((hal_info->in_info.pixel_format != HI_DRV_PIXEL_FMT_NV21) &&
        (hal_info->in_info.pixel_format != HI_DRV_PIXEL_FMT_NV12)) {
        vpss_sys_set_vc1_en(vpss_reg, 0, HI_FALSE);
        return;
    }

    vc1_info = &(hal_info->in_info.vc1_info);
    vc1_en = hal_info->in_info.vc1_en;
    vpss_sys_set_vc1_en(vpss_reg, 0, vc1_en);
    set_vc1_reg(vpss_reg, layer, vc1_info);

    return;
}

hi_void set_all_rchn_init_cfg(vpss_reg_type *vpss_reg, vpss_mac_rchn_cfg *rchn_cfg)
{
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_CF, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_CF, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_P1, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_P2, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_P3, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_P3I, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_CC_PR0, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_CC_PR4, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_CC_PR8, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_REF, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_RSADY, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_RSADC, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_RHISM, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_P1MV, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_P2MV, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_P3MV, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_CC_RCCNT, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_CC_RYCNT, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_RMAD, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_SNR_RMAD, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_CFMV, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_P1RGMV, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_P2RGMV, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_RCNT, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_RPRJH, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_RPRJV, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_P1RGMV, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_P2RGMV, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DMCNT, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_ME_CF, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_ME_REF, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_ME_P1MV, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_ME_PRMV, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_ME_P1GMV, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_ME_PRGMV, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_ME_P1RGMV, rchn_cfg);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_ME_CFRGMV, rchn_cfg);

    return;
}

hi_void set_all_wchn_init_cfg(vpss_reg_type *vpss_reg, vpss_mac_wchn_cfg *wchn_cfg)
{
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_OUT0, wchn_cfg);
#ifdef VPSS_96CV300_CS_SUPPORT
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_OUT1, wchn_cfg);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_OUT2, wchn_cfg);
#endif
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_OUT3, wchn_cfg);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_RFR, wchn_cfg);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_RFRH, wchn_cfg);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_RFRHV, wchn_cfg);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_CC_RFR, wchn_cfg);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_CC_RFR1, wchn_cfg);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_DI_RFR, wchn_cfg);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_WMAD, wchn_cfg);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_WCNT, wchn_cfg);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_CC_WCCNT, wchn_cfg);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_CC_WYCNT, wchn_cfg);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_DI_WSADY, wchn_cfg);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_DI_WSADC, wchn_cfg);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_DI_WHISM, wchn_cfg);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_RG_WPRJH, wchn_cfg);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_RG_WPRJV, wchn_cfg);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_RG_CFRGMV, wchn_cfg);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_DMCNT, wchn_cfg);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_ME_CFMV, wchn_cfg);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_ME_CFGMV, wchn_cfg);

    return;
}

hi_void init_all_chn_cfg(vpss_reg_type *vpss_reg)
{
    vpss_mac_rchn_cfg rchn_cfg = { 0 };
    vpss_mac_wchn_cfg wchn_cfg = { 0 };

    vpss_mac_init_rchn_cfg(&rchn_cfg);
    vpss_mac_init_wchn_cfg(&wchn_cfg);

    set_all_rchn_init_cfg(vpss_reg, &rchn_cfg);
    set_all_wchn_init_cfg(vpss_reg, &wchn_cfg);

    return;
}

hi_void init_cf_out0_chn_cfg(vpss_reg_type *vpss_reg)
{
    vpss_mac_rchn_cfg rchn_cfg = { 0 };
    vpss_mac_wchn_cfg wchn_cfg = { 0 };

    vpss_mac_init_rchn_cfg(&rchn_cfg);
    vpss_mac_init_wchn_cfg(&wchn_cfg);

    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_CF, &rchn_cfg);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_OUT0, &wchn_cfg);
}

static hi_void convert_chn_datafmt(hi_drv_pixel_format pixel_format, xdp_proc_fmt *data_fmt)
{
    switch (pixel_format) {
        case HI_DRV_PIXEL_FMT_NV12:
        case HI_DRV_PIXEL_FMT_NV21:
            *data_fmt = XDP_PROC_FMT_SP_420;
            break;
        case HI_DRV_PIXEL_FMT_NV16:
        case HI_DRV_PIXEL_FMT_NV61:
        case HI_DRV_PIXEL_FMT_NV16_2X1:
        case HI_DRV_PIXEL_FMT_NV61_2X1:
            *data_fmt = XDP_PROC_FMT_SP_422;
            break;
        default:
            *data_fmt = XDP_PROC_FMT_SP_400;
            vpss_error("vpss can't support fmt %d\n", pixel_format);
            break;
    }

    return;
}

static hi_void convert_chn_cmp_type(hi_drv_compress_mode cmp_mode, xdp_cmp_type *cmp_type)
{
    if ((cmp_mode == HI_DRV_COMPRESS_MODE_SEG_LOSSLESS) || (cmp_mode == HI_DRV_COMPRESS_MODE_SEG_LOSSLESS)) {
        *cmp_type = XDP_CMP_TYPE_SEG;
    } else if ((cmp_mode == HI_DRV_COMPRESS_MODE_LINE_LOSS) || (cmp_mode == HI_DRV_COMPRESS_MODE_LINE_LOSSLESS)) {
        *cmp_type = XDP_CMP_TYPE_LINE;
    } else if ((cmp_mode == HI_DRV_COMPRESS_MODE_FRM_LOSS) || (cmp_mode == HI_DRV_COMPRESS_MODE_FRM_LOSSLESS)) {
        *cmp_type = XDP_CMP_TYPE_FRM;
    } else {
        *cmp_type = XDP_CMP_TYPE_OFF;
    }

    return;
}

static hi_void convert_chn_data_type(hi_drv_data_fmt data_fmt, xdp_data_type *data_type)
{
    if (data_fmt == HI_DRV_DATA_FMT_LINER) {
        *data_type = XDP_DATA_TYPE_SP_LINEAR;
    } else if (data_fmt == HI_DRV_DATA_FMT_TILE) {
        *data_type = XDP_DATA_TYPE_SP_TILE;
    } else if (data_fmt == HI_DRV_DATA_FMT_PKG) {
        *data_type = XDP_DATA_TYPE_PACKAGE;
    } else {
        vpss_error("Wrong data_type %d \n", data_fmt);
    }

    return;
}

static hi_void convert_chn_bitwidth(hi_drv_pixel_bitwidth bit_width, xdp_data_wth *data_width)
{
    if (bit_width == HI_DRV_PIXEL_BITWIDTH_8BIT) {
        *data_width = XDP_DATA_WTH_8;
    } else if (bit_width == HI_DRV_PIXEL_BITWIDTH_10BIT) {
        *data_width = XDP_DATA_WTH_10;
    } else {
        *data_width = XDP_DATA_WTH_12;
        vpss_error("vpss can't support bitwidth %d\n", bit_width);
    }

    return;
}

hi_void convert_to_rchn_cf_cfg(vpss_hal_frame *hal_frame, vpss_mac_rchn_cfg *rchn_cfg)
{
    vpss_mac_init_rchn_cfg(rchn_cfg);

    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_y = hal_frame->hal_addr.phy_addr_y;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_u = hal_frame->hal_addr.phy_addr_c;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_v = hal_frame->hal_addr.phy_addr_cr;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].str_y = hal_frame->hal_addr.stride_y;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].str_c = hal_frame->hal_addr.stride_c;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].y_head_addr = hal_frame->hal_addr.phy_addr_y_head;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].c_head_addr = hal_frame->hal_addr.phy_addr_c_head;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].y_head_str = hal_frame->hal_addr.head_stride;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].c_head_str = hal_frame->hal_addr.head_stride;
    rchn_cfg->addr[VPSS_RCHN_ADDR_2B].addr_y = hal_frame->hal_addr_lb.phy_addr_y;
    rchn_cfg->addr[VPSS_RCHN_ADDR_2B].addr_u = hal_frame->hal_addr_lb.phy_addr_c;
    rchn_cfg->addr[VPSS_RCHN_ADDR_2B].addr_v = hal_frame->hal_addr_lb.phy_addr_cr;
    rchn_cfg->addr[VPSS_RCHN_ADDR_2B].str_y = hal_frame->hal_addr_lb.stride_y;
    rchn_cfg->addr[VPSS_RCHN_ADDR_2B].str_c = hal_frame->hal_addr_lb.stride_c;
    rchn_cfg->en = HI_TRUE;

    convert_chn_datafmt(hal_frame->pixel_format, &(rchn_cfg->data_fmt));
    convert_chn_cmp_type(hal_frame->cmp_info.cmp_mode, &(rchn_cfg->dcmp_cfg.cmp_type));
    convert_chn_data_type(hal_frame->cmp_info.data_fmt, &(rchn_cfg->data_type));

    rchn_cfg->rd_mode = XDP_RMODE_PROGRESSIVE;

    convert_chn_bitwidth(hal_frame->bit_width, &(rchn_cfg->data_width));

    rchn_cfg->in_rect.x = hal_frame->rect_x;
    rchn_cfg->in_rect.y = hal_frame->rect_y;
    rchn_cfg->in_rect.wth = hal_frame->rect_w;
    rchn_cfg->in_rect.hgt = hal_frame->rect_h;
}

hi_void convert_to_rwbc_chn_cfg(vpss_hal_frame *hal_frame, vpss_mac_rchn_cfg *rchn_cfg)
{
    vpss_mac_init_rchn_cfg(rchn_cfg);

    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_y = hal_frame->hal_addr.phy_addr_y;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_u = hal_frame->hal_addr.phy_addr_c;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_v = hal_frame->hal_addr.phy_addr_cr;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].str_y = hal_frame->hal_addr.stride_y;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].str_c = hal_frame->hal_addr.stride_c;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].y_head_addr = hal_frame->hal_addr.phy_addr_y_head;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].c_head_addr = hal_frame->hal_addr.phy_addr_c_head;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].y_head_str = hal_frame->hal_addr.head_stride;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].c_head_str = hal_frame->hal_addr.head_stride;
    rchn_cfg->addr[VPSS_RCHN_ADDR_2B].addr_y = hal_frame->hal_addr_lb.phy_addr_y;
    rchn_cfg->addr[VPSS_RCHN_ADDR_2B].addr_u = hal_frame->hal_addr_lb.phy_addr_c;
    rchn_cfg->addr[VPSS_RCHN_ADDR_2B].addr_v = hal_frame->hal_addr_lb.phy_addr_cr;
    rchn_cfg->addr[VPSS_RCHN_ADDR_2B].str_y = hal_frame->hal_addr_lb.stride_y;
    rchn_cfg->addr[VPSS_RCHN_ADDR_2B].str_c = hal_frame->hal_addr_lb.stride_c;
    rchn_cfg->en = HI_TRUE;

    convert_chn_datafmt(hal_frame->pixel_format, &(rchn_cfg->data_fmt));
    convert_chn_cmp_type(hal_frame->cmp_info.cmp_mode, &(rchn_cfg->dcmp_cfg.cmp_type));
    convert_chn_data_type(hal_frame->cmp_info.data_fmt, &(rchn_cfg->data_type));

    rchn_cfg->rd_mode = XDP_RMODE_PROGRESSIVE;

    convert_chn_bitwidth(hal_frame->bit_width, &(rchn_cfg->data_width));

    rchn_cfg->in_rect.x = hal_frame->rect_x;
    rchn_cfg->in_rect.y = hal_frame->rect_y;
    rchn_cfg->in_rect.wth = hal_frame->rect_w;
    rchn_cfg->in_rect.hgt = hal_frame->rect_h;
}

hi_void convert_to_rstt_chn_cfg(vpss_hal_stt_channel_cfg *hal_stt_cfg, vpss_mac_rchn_cfg *rchn_cfg)
{
    vpss_mac_init_rchn_cfg(rchn_cfg);
    rchn_cfg->en = HI_TRUE;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_y = hal_stt_cfg->phy_addr;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_u = hal_stt_cfg->phy_addr;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_v = hal_stt_cfg->phy_addr;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].str_y = hal_stt_cfg->stride;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].str_c = hal_stt_cfg->stride;
    rchn_cfg->en = HI_TRUE;
    rchn_cfg->in_rect.wth = hal_stt_cfg->width;
    rchn_cfg->in_rect.hgt = hal_stt_cfg->height;
}

hi_void convert_to_wwbc_chn_cfg(vpss_hal_frame *hal_frame, vpss_mac_wchn_cfg *wchn_cfg)
{
    vpss_mac_init_wchn_cfg(wchn_cfg);
    wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].addr_y = hal_frame->hal_addr.phy_addr_y;
    wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].addr_u = hal_frame->hal_addr.phy_addr_c;
    wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].addr_v = hal_frame->hal_addr.phy_addr_cr;
    wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].str_y = hal_frame->hal_addr.stride_y;
    wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].str_c = hal_frame->hal_addr.stride_c;
    wchn_cfg->en = HI_TRUE;
    wchn_cfg->vir_addr_y = hal_frame->vir_addr_y;
    wchn_cfg->vir_addr_c = hal_frame->vir_addr_c;

    convert_chn_datafmt(hal_frame->pixel_format, &(wchn_cfg->data_fmt));
    convert_chn_cmp_type(hal_frame->cmp_info.cmp_mode, &(wchn_cfg->cmp_cfg.cmp_type));
    convert_chn_data_type(hal_frame->cmp_info.data_fmt, &(wchn_cfg->data_type));

    if (wchn_cfg->cmp_cfg.cmp_type != XDP_CMP_TYPE_OFF) {
        wchn_cfg->cmp_cfg.is_raw_en = HI_FALSE;
        wchn_cfg->cmp_cfg.is_lossy_y = vpss_comm_is_loss_compress(hal_frame->cmp_info.cmp_mode);
        wchn_cfg->cmp_cfg.is_lossy_c = vpss_comm_is_loss_compress(hal_frame->cmp_info.cmp_mode);
        wchn_cfg->cmp_cfg.cmp_ratio_y = hal_frame->cmp_info.ycmp_rate;
        wchn_cfg->cmp_cfg.cmp_ratio_c = hal_frame->cmp_info.ccmp_rate;
        wchn_cfg->cmp_cfg.cmp_cfg_mode = ICE_REG_CFG_MODE_TYP;
    }

    wchn_cfg->rd_mode = XDP_RMODE_PROGRESSIVE;

    convert_chn_bitwidth(hal_frame->bit_width, &(wchn_cfg->data_width));

    wchn_cfg->out_rect.x = 0;
    wchn_cfg->out_rect.y = 0;
    wchn_cfg->out_rect.wth = hal_frame->rect_w;
    wchn_cfg->out_rect.hgt = hal_frame->rect_h;
    wchn_cfg->tunl_en = HI_FALSE;
}

hi_void convert_to_wstt_chn_cfg(vpss_hal_stt_channel_cfg *hal_stt_cfg, vpss_mac_wchn_cfg *wchn_cfg)
{
    vpss_mac_init_wchn_cfg(wchn_cfg);
    wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].addr_y = hal_stt_cfg->phy_addr;
    wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].addr_u = hal_stt_cfg->phy_addr;
    wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].addr_v = hal_stt_cfg->phy_addr;
    wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].str_y = hal_stt_cfg->stride;
    wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].str_c = hal_stt_cfg->stride;
    wchn_cfg->en = HI_TRUE;
    wchn_cfg->out_rect.x = 0;
    wchn_cfg->out_rect.y = 0;
    wchn_cfg->out_rect.wth = hal_stt_cfg->width;
    wchn_cfg->out_rect.hgt = hal_stt_cfg->height;
    wchn_cfg->size = hal_stt_cfg->size;
    wchn_cfg->vir_addr_y = hal_stt_cfg->vir_addr;
}

hi_void init_regtable(vpss_reg_type *vpss_reg_in, vpss_reg_type *pq_reg)
{
    if (pq_reg == HI_NULL) {
        memset((hi_void *)vpss_reg_in, 0, sizeof(vpss_reg_type));
    } else {
        memcpy((hi_void *)vpss_reg_in, pq_reg, sizeof(vpss_reg_type));
    }
}

hi_u32 get_safe_mode(vpss_hal_frame *in_info)
{
    if (in_info->secure == HI_TRUE) {
        return HAL_VPSS_SAFE_MODE;
    }

    return HAL_VPSS_NOSAFE_MODE;
}

static hi_void check_smmu_state(hi_u32 int_state)
{
    if ((int_state & VPSS_HAL_SMMU_INT_TYPE_TBL_MISS) == VPSS_HAL_SMMU_INT_TYPE_TBL_MISS) {
        vpss_error("TLB missing in smmu mod !!! \n");
    }

    if ((int_state & VPSS_HAL_SMMU_INT_TYPE_PTW_TRANS) == VPSS_HAL_SMMU_INT_TYPE_PTW_TRANS) {
        vpss_error("PTW trans error in smmu mod !!! \n");
    }

    if ((int_state & VPSS_HAL_SMMU_INT_TYPE_TBL_R_INVALID) == VPSS_HAL_SMMU_INT_TYPE_TBL_R_INVALID) {
        vpss_error("rd_stat error in smmu mod !!! \n");
    }

    if ((int_state & VPSS_HAL_SMMU_INT_TYPE_TBL_W_INVALID) == VPSS_HAL_SMMU_INT_TYPE_TBL_W_INVALID) {
        vpss_error("wd_stat error in smmu mod !!! \n");
    }

    return;
}

static hi_void dbg_smmu_allchn_state(hi_void)
{
    vpss_mac_rchn rchn = VPSS_MAC_RCHN_CF;
    vpss_mac_wchn wchn = VPSS_MAC_WCHN_OUT0;
    vpss_mac_rchn_cfg *rchn_cfg = &g_rchn_cfg[VPSS_MAC_RCHN_CF];
    vpss_mac_wchn_cfg *wchn_cfg = &g_wchn_cfg[VPSS_MAC_RCHN_CF];

    for (rchn = VPSS_MAC_RCHN_CF; rchn < VPSS_MAC_RCHN_MAX; rchn++) {
        if (rchn_cfg[rchn].en == HI_TRUE) {
            osal_printk("Rchn=%d, yaddr = 0x%llx, caddr = 0x%llx, w=%d,h=%d,ys=%d,cs=%d\n",
                        rchn, rchn_cfg[rchn].addr[0].addr_y, rchn_cfg[rchn].addr[0].addr_u,
                        rchn_cfg[rchn].in_rect.wth, rchn_cfg[rchn].in_rect.hgt,
                        rchn_cfg[rchn].addr[0].str_y, rchn_cfg[rchn].addr[0].str_c);
        }
    }

    for (wchn = VPSS_MAC_WCHN_OUT0; wchn < VPSS_MAC_WCHN_MAX; wchn++) {
        if (wchn_cfg[wchn].en == HI_TRUE) {
            osal_printk("Wchn=%d, yaddr = 0x%llx, caddr = 0x%llx, w=%d,h=%d,ys=%d,cs=%d\n",
                        wchn, wchn_cfg[wchn].addr[0].addr_y, wchn_cfg[wchn].addr[0].addr_u,
                        wchn_cfg[wchn].out_rect.wth, wchn_cfg[wchn].out_rect.hgt,
                        wchn_cfg[wchn].addr[0].str_y, wchn_cfg[wchn].addr[0].str_c);
        }
    }

    return;
}

hi_s32 process_smmu_state(vpss_ip ip, hi_u32 int_state)
{
    hi_u32 safe_mode;
    vpss_reg_type *vpss_reg = HI_NULL;
    vpss_hal_ctx *hal_ctx = HI_NULL;
    hi_u32 write_error_addr = 0;
    hi_u32 read_error_addr = 0;

    vpss_hal_check_ip_vaild(ip);

    hal_ctx = &g_hal_ctx[ip];
    vpss_hal_check_init(hal_ctx->init);

    vpss_reg = (vpss_reg_type *)(hal_ctx->base_reg_vir);

    vpss_mac_get_prot(vpss_reg, &safe_mode);

    if (safe_mode == HAL_VPSS_NOSAFE_MODE) {
        check_smmu_state(int_state);

        if ((int_state & VPSS_HAL_SMMU_INT_TYPE_ALLTYPE) == 0) {
            return HI_SUCCESS;
        }
    }

    if (safe_mode == HAL_VPSS_NOSAFE_MODE) {
        vpss_mmu_get_err_ns_wr_addr(vpss_reg, 0, &write_error_addr);
        vpss_mmu_get_err_ns_rd_addr(vpss_reg, 0, &read_error_addr);
    } else {
        vpss_mmu_get_err_swr_addr(vpss_reg, 0, &write_error_addr);
        vpss_mmu_get_err_srd_addr(vpss_reg, 0, &read_error_addr);
    }

    if (safe_mode == HAL_VPSS_SAFE_MODE) { /* cannot read safe smmu stat */
        return HI_SUCCESS;
    }

    if ((write_error_addr == 0) && (read_error_addr == 0)) {
        return HI_SUCCESS;
    }

    osal_printk("vpss dump logic chan config addr\n");
    osal_printk("write_error_addr:0x%x, read_error_addr:0x%x\n", write_error_addr, read_error_addr);

    dbg_smmu_allchn_state();

    return HI_SUCCESS;
}

vpss_reg_port alloc_port_id(vpss_hal_info *hal_info, vpss_hal_port_info *hal_port,
    hi_bool port_used[VPSS_PORT_MAX_NUM], hi_u32 port_num)
{
    hi_bool support_3d_detect = vpss_policy_support_logic_3d_detect_port();
    hi_bool support_vir_port = vpss_policy_check_support_virtual_port();

    if (port_num > VPSS_PORT_MAX_NUM) {
        return VPSS_REG_PORT_MAX;
    }

    if ((port_used[VPSS_REG_PORT0] == HI_FALSE) && (hal_port->port_type == HI_DRV_VPSS_PORT_TYPE_NORMAL) &&
        (hal_port->config == HI_FALSE) && (hal_info->port_used[VPSS_REG_PORT0] == HI_FALSE)) {
        port_used[VPSS_REG_PORT0] = HI_TRUE;
        return VPSS_REG_PORT0;
    }

    if (support_3d_detect == HI_TRUE) {
        if ((port_used[VPSS_REG_PORT1] == HI_FALSE) && (hal_port->port_type == HI_DRV_VPSS_PORT_TYPE_3D_DETECT) &&
            (hal_port->config == HI_FALSE) && (hal_info->port_used[VPSS_REG_PORT1] == HI_FALSE)) {
            port_used[VPSS_REG_PORT1] = HI_TRUE;
            return VPSS_REG_PORT1;
        }
    }

    /* port0 & access logic port need to cfg first, then vir port */
    if (support_vir_port == HI_TRUE) {
        if ((port_used[VPSS_REG_PORT_VIR] == HI_FALSE) && (hal_port->port_type == HI_DRV_VPSS_PORT_TYPE_NORMAL) &&
            (hal_port->config == HI_FALSE) && (hal_info->port_used[VPSS_REG_PORT_VIR] == HI_FALSE)) {
            port_used[VPSS_REG_PORT_VIR] = HI_TRUE;
            return VPSS_REG_PORT_VIR;
        }
    }

    return VPSS_REG_PORT_MAX;
}

hi_s32 get_pq_out0_zme_coef(vpss_reg_type *vpss_reg, vpss_hal_port_info *out_hal_port)
{
    hi_s32 ret;
    hi_drv_pq_vpss_layer layer_id = HI_DRV_PQ_VPSS_LAYER_PORT0;
    hi_drv_pq_vpsszme_in zme_in = { 0 };
    hi_drv_pq_vpsszme_out zme_out = { 0 };

    zme_in.comm_info.zme_w_in = out_hal_port->in_crop_rect.rect_w;
    zme_in.comm_info.zme_h_in = out_hal_port->in_crop_rect.rect_h;
    zme_in.comm_info.zme_w_out = out_hal_port->video_rect.rect_w;
    zme_in.comm_info.zme_h_out = out_hal_port->video_rect.rect_h;

    ret = vpss_comm_pq_get_zme_coef(layer_id, &zme_in, &zme_out);
    if (ret != HI_SUCCESS) {
        vpss_error("get zme coef failed %d \n", ret);
        return HI_FAILURE;
    }

    vpss_hal_set_para_addr_vhd_chn01(vpss_reg, (hi_u32)zme_out.zme_coef_addr.zme_coef_hl_addr);
    vpss_hal_set_para_haddr_vhd_chn01(vpss_reg, (hi_u32)(zme_out.zme_coef_addr.zme_coef_hl_addr >> VPSS_REG_SHIFT));
    vpss_hal_set_para_addr_vhd_chn02(vpss_reg, (hi_u32)zme_out.zme_coef_addr.zme_coef_vl_addr);
    vpss_hal_set_para_haddr_vhd_chn02(vpss_reg, (hi_u32)(zme_out.zme_coef_addr.zme_coef_vl_addr >> VPSS_REG_SHIFT));

    vpss_hal_set_para_up_vhd_chn(vpss_reg, HAL_PARA_CHN_HZME);
    vpss_hal_set_para_up_vhd_chn(vpss_reg, HAL_PARA_CHN_VZME);

    return ret;
}

hi_s32 cfg_pzme(vpss_reg_type *vpss_reg,
                vpss_hal_port_info *out_hal_port, vpss_hal_info *in_hal_info)
{
    hi_u32 hstep, wstep;
    hi_u32 in_h, in_w, out_h, out_w;
    in_h = in_hal_info->in_info.rect_h;
    in_w = in_hal_info->in_info.rect_w;
    out_h = (hi_u32)out_hal_port->video_rect.rect_h;
    out_w = (hi_u32)out_hal_port->video_rect.rect_w;

    if ((in_h / out_h) > VPSS_HAL_MAX_PZME_MULTIPLY
        || (in_w / out_w) > VPSS_HAL_MAX_PZME_MULTIPLY) {
        vpss_error("out image is too small!!!!!!!\n");
        return HI_FAILURE;
    }

    wstep = ((in_w << 16) + out_w / 2) / out_w; /* 16:for alg calculation 2 :for half */
    hstep = ((in_h << 16) + out_h / 2) / out_h; /* 16:for alg calculation 2 :for half */
    vpss_pzme_set_pzme_en(vpss_reg, 0, HI_TRUE);
    vpss_pzme_set_pzme_hstep(vpss_reg, 0, hstep);
    vpss_pzme_set_pzme_wstep(vpss_reg, 0, wstep);
    vpss_pzme_set_pzme_out_img_width(vpss_reg, 0, out_w);
    vpss_pzme_set_pzme_out_img_height(vpss_reg, 0, out_h);
    return HI_SUCCESS;
}

hi_s32 cfg_vhd0_pzme(vpss_reg_type *vpss_reg, vpss_hal_port_info *out_hal_port)
{
    hi_bool cccl_convert = HI_FALSE;
    hi_u32 hstep, wstep;
    hi_u32 in_h, in_w, out_h, out_w;
    vpss_mac_rchn_cfg *rchn_cfg;
    vpss_mac_wchn_cfg *wchn_cfg;
    xdp_proc_fmt vhd0_in_data_fmt;
    rchn_cfg = &g_rchn_cfg[VPSS_MAC_RCHN_CF];
    wchn_cfg = &g_wchn_cfg[VPSS_MAC_WCHN_OUT0];
    in_h = out_hal_port->in_crop_rect.rect_h;
    in_w = out_hal_port->in_crop_rect.rect_w;
    out_h = (hi_u32)out_hal_port->video_rect.rect_h;
    out_w = (hi_u32)out_hal_port->video_rect.rect_w;
    vhd0_in_data_fmt = rchn_cfg[VPSS_MAC_RCHN_CF].data_fmt;
    vpss_mac_get_cf_cconvert(vpss_reg, &cccl_convert);

    if ((XDP_PROC_FMT_SP_420 == rchn_cfg[VPSS_MAC_RCHN_CF].data_fmt)
        && (cccl_convert == HI_TRUE)) {
        vhd0_in_data_fmt = XDP_PROC_FMT_SP_422;
    }

    if ((in_h == out_h) && (in_w == out_w)
        && (vhd0_in_data_fmt == wchn_cfg[VPSS_MAC_WCHN_OUT0].data_fmt)) {
        vpss_pzme_set_out0_pzme_en(vpss_reg, 0, HI_FALSE);
        return HI_SUCCESS;
    }

    if ((in_h / out_h) > VPSS_HAL_MAX_PZME_MULTIPLY
        || (in_w / out_w) > VPSS_HAL_MAX_PZME_MULTIPLY) {
        vpss_error("out image is too small!!!!!!!\n");
        return HI_FAILURE;
    }

    wstep = ((in_w << 16) + out_w / 2) / out_w; /* 16:for alg calculation 2 :for half */
    hstep = ((in_h << 16) + out_h / 2) / out_h; /* 16:for alg calculation 2 :for half */
    vpss_pzme_set_out0_pzme_en(vpss_reg, 0, HI_TRUE);

    if (vhd0_in_data_fmt != wchn_cfg[VPSS_MAC_WCHN_OUT0].data_fmt) {
        vpss_pzme_set_out0_pzme_mode(vpss_reg, 0, HI_TRUE);
    }

    vpss_pzme_set_out0_pzme_hstep(vpss_reg, 0, hstep);
    vpss_pzme_set_out0_pzme_wstep(vpss_reg, 0, wstep);
    vpss_pzme_set_out0_pzme_out_img_width(vpss_reg, 0, out_w);
    vpss_pzme_set_out0_pzme_out_img_height(vpss_reg, 0, out_h);
    return HI_SUCCESS;
}

static hi_s32 cfg_vhd0_out0_hzme(vpss_reg_type *vpss_reg, vpss_hal_port_info *out_hal_port, vpss_mac_rchn_cfg *rchn_cfg)
{
    vpss_hzme_cfg hzme_cfg = { 0 };
    vpss_hzme_pq hzme_pq_cfg = { 0 };

    hzme_cfg.in_width = (hi_u64)out_hal_port->in_crop_rect.rect_w;
    hzme_cfg.out_width = (hi_u64)out_hal_port->video_rect.rect_w;

    if (((hzme_cfg.in_width / hzme_cfg.out_width) >= VPSS_MAX_ZME_MULTIPLY) ||
        ((hzme_cfg.out_width / hzme_cfg.in_width) >= VPSS_MAX_ZME_MULTIPLY)) {
        vpss_error("zme hration bigger than 16; in_w=%llu,out_w=%llu !!!!!!!\n",
                   hzme_cfg.in_width, hzme_cfg.out_width);
        return HI_FAILURE;
    }

    hzme_cfg.out_fmt = rchn_cfg[VPSS_MAC_RCHN_CF].data_fmt;
    hzme_cfg.ck_gt_en = 1;
    if (hzme_cfg.in_width != hzme_cfg.out_width) {
        hzme_cfg.lh_fir_en = 1;
        hzme_cfg.ch_fir_en = 1;
    } else {
        hzme_cfg.lh_fir_en = 0;
        hzme_cfg.ch_fir_en = 0;
    }
    hzme_cfg.lh_fir_mode = 1;
    hzme_cfg.ch_fir_mode = 1;
    hzme_pq_cfg.lh_stc2nd_en = 0;
    hzme_pq_cfg.ch_stc2nd_en = 0;
    hzme_pq_cfg.lh_med_en = 0;
    hzme_pq_cfg.ch_med_en = 0;
    hzme_pq_cfg.nonlinear_scl_en = 0;
    hzme_pq_cfg.lhfir_offset = 0;
    hzme_pq_cfg.chfir_offset = 0;
    vpss_func_set_hzme_mode(vpss_reg, XDP_ZME_ID_VPSS_CH_HZME, VPSS_HZME_TYP, &hzme_cfg, &hzme_pq_cfg);

    return HI_SUCCESS;
}

static hi_s32 cfg_vhd0_out0_vzme(vpss_reg_type *vpss_reg, vpss_hal_port_info *out_hal_port,
                                 vpss_mac_rchn_cfg *rchn_cfg, vpss_mac_wchn_cfg *wchn_cfg)
{
    vpss_vzme_cfg vzme_cfg = { 0 };
    vpss_vzme_pq vzme_pq_cfg = { 0 };

    vzme_cfg.in_height = (hi_u64)out_hal_port->in_crop_rect.rect_h;
    vzme_cfg.out_height = (hi_u64)out_hal_port->video_rect.rect_h;

    if ((vzme_cfg.in_height / vzme_cfg.out_height) >= VPSS_MAX_ZME_MULTIPLY
        || (vzme_cfg.out_height / vzme_cfg.in_height) >= VPSS_MAX_ZME_MULTIPLY) {
        vpss_error("zme vration bigger than 16; in_h=%llu,out_h=%llu !!!!!!!\n",
                   vzme_cfg.in_height, vzme_cfg.out_height);
        return HI_FAILURE;
    }

    vzme_cfg.in_fmt = rchn_cfg[VPSS_MAC_RCHN_CF].data_fmt;
    vzme_cfg.out_fmt = (out_hal_port->need_hdr == HI_TRUE) ? XDP_PROC_FMT_SP_422 :
        wchn_cfg[VPSS_MAC_WCHN_OUT0].data_fmt;
    vzme_cfg.ck_gt_en = 1;
    if ((vzme_cfg.in_height != vzme_cfg.out_height) || (vzme_cfg.in_fmt != vzme_cfg.out_fmt)) {
        vzme_cfg.lv_fir_en = 1;
        vzme_cfg.cv_fir_en = 1;
    } else {
        vzme_cfg.lv_fir_en = 0;
        vzme_cfg.cv_fir_en = 0;
    }
    vzme_cfg.lv_fir_mode = 1;
    vzme_cfg.cv_fir_mode = 1;
    vzme_pq_cfg.lv_stc2nd_en = 0;
    vzme_pq_cfg.cv_stc2nd_en = 0;
    vzme_pq_cfg.lv_med_en = 0;
    vzme_pq_cfg.cv_med_en = 0;
    vzme_pq_cfg.lvfir_offset = 0;
    vzme_pq_cfg.cvfir_offset = 0;
    vpss_func_set_vzme_mode(vpss_reg, XDP_ZME_ID_VPSS_CH_VZME, VPSS_VZME_TYP, &vzme_cfg, &vzme_pq_cfg);

    return HI_SUCCESS;
}

hi_s32 cfg_vhd0_out0_zme(vpss_reg_type *vpss_reg, vpss_hal_port_info *out_hal_port)
{
    hi_s32 ret;
    vpss_mac_rchn_cfg *rchn_cfg = &g_rchn_cfg[VPSS_MAC_RCHN_CF];
    vpss_mac_wchn_cfg *wchn_cfg = &g_wchn_cfg[VPSS_MAC_WCHN_OUT0];

    ret = cfg_vhd0_out0_hzme(vpss_reg, out_hal_port, rchn_cfg);
    if (ret != HI_SUCCESS) {
        vpss_error("cfg_vhd0_out0_hzme failed \n");
        return HI_FAILURE;
    }

    ret = cfg_vhd0_out0_vzme(vpss_reg, out_hal_port, rchn_cfg, wchn_cfg);
    if (ret != HI_SUCCESS) {
        vpss_error("cfg_vhd0_out0_vzme failed \n");
        return HI_FAILURE;
    }

#ifdef VPSS_LBX_DBG
    vpss_error("zme in:%d %d, out:%d %d\n",
               out_hal_port->in_crop_rect.rect_w, out_hal_port->in_crop_rect.rect_h,
               out_hal_port->video_rect.rect_w, out_hal_port->video_rect.rect_h);
#endif
    return HI_SUCCESS;
}

hi_void cfg_vhd0_crop(vpss_reg_type *vpss_reg, vpss_hal_port_info *out_hal_port, vpss_hal_info *in_hal_info)
{
    /* to do: 420 field add height 4 pixel align limit;  input and output add limit */
    hi_u32 vhd0_crop_en;
    hi_u32 min_out_wth = 64; /* 64:min reso */
    hi_u32 min_out_hgt = 64; /* 64:min reso */
    vpss_assert_not_ret(in_hal_info->in_info.rect_w >= min_out_wth);
    vpss_assert_not_ret(in_hal_info->in_info.rect_h >= min_out_hgt);
    vpss_assert_not_ret(in_hal_info->in_info.rect_w % 2 == 0); /* 2 align */
    vpss_assert_not_ret(in_hal_info->in_info.rect_h % 2 == 0); /* 2 align */
    vpss_assert_not_ret(in_hal_info->in_info.rect_x % 2 == 0); /* 2 align */
    vpss_assert_not_ret(in_hal_info->in_info.rect_y % 2 == 0); /* 2 align */

    if (in_hal_info->in_info.rect_w == out_hal_port->in_crop_rect.rect_w
        && in_hal_info->in_info.rect_h == out_hal_port->in_crop_rect.rect_h) {
        vhd0_crop_en = HI_FALSE;
    } else {
        vhd0_crop_en = HI_TRUE;
    }

    vpss_sys_set_vhd0_crop_en(vpss_reg, 0, vhd0_crop_en);
    vpss_sys_set_vhd0_crop_x(vpss_reg, 0, out_hal_port->in_crop_rect.rect_x) ;
    vpss_sys_set_vhd0_crop_y(vpss_reg, 0, out_hal_port->in_crop_rect.rect_y) ;
    vpss_sys_set_vhd0_crop_width(vpss_reg, 0, out_hal_port->in_crop_rect.rect_w - 1);
    vpss_sys_set_vhd0_crop_height(vpss_reg, 0, out_hal_port->in_crop_rect.rect_h - 1);

#ifdef VPSS_LBX_DBG
    vpss_error("crop_en:%d; %d %d %d %d\n", vhd0_crop_en,
               out_hal_port->in_crop_rect.rect_x, out_hal_port->in_crop_rect.rect_y,
               out_hal_port->in_crop_rect.rect_w, out_hal_port->in_crop_rect.rect_h);
#endif
}

hi_void cfg_vhd0_lbx(vpss_reg_type *vpss_reg, vpss_hal_port_info *out_hal_port)
{
    /* to do: 420 field add height 4 pixel align limit;  input and output add limit */
    hi_u32 vhd0_lbox_en;
    hi_u32 vhd0_vbk_y = 64; /* 64 : color */
    hi_u32 vhd0_vbk_cb = 512; /* 512 : color */
    hi_u32 vhd0_vbk_cr = 512; /* 512 : color */

    if (out_hal_port->video_rect.rect_w == out_hal_port->out_info.rect_w
        && out_hal_port->video_rect.rect_h == out_hal_port->out_info.rect_h) {
        vhd0_lbox_en = HI_FALSE;
    } else {
        vhd0_lbox_en = HI_TRUE;
    }

    vpss_sys_set_vhd0_vbk_y(vpss_reg, 0, vhd0_vbk_y);
    vpss_sys_set_vhd0_vbk_cb(vpss_reg, 0, vhd0_vbk_cb);
    vpss_sys_set_vhd0_vbk_cr(vpss_reg, 0, vhd0_vbk_cr);
    vpss_sys_set_vhd0_lba_xfpos(vpss_reg, 0, out_hal_port->video_rect.rect_x);
    vpss_sys_set_vhd0_lba_yfpos(vpss_reg, 0, out_hal_port->video_rect.rect_y);
    vpss_sys_set_vhd0_lba_width(vpss_reg, 0, out_hal_port->out_info.rect_w - 1);
    vpss_sys_set_vhd0_lba_height(vpss_reg, 0, out_hal_port->out_info.rect_h - 1);
    vpss_sys_set_vhd0_lba_en(vpss_reg, 0, vhd0_lbox_en);

#ifdef VPSS_LBX_DBG
    vpss_error("lbx_en:%d; video:%d %d %d %d; out:%d %d %d %d\n",
               vhd0_lbox_en,
               out_hal_port->video_rect.rect_x, out_hal_port->video_rect.rect_y,
               out_hal_port->video_rect.rect_w, out_hal_port->video_rect.rect_h,
               out_hal_port->out_info.rect_x, out_hal_port->out_info.rect_y,
               out_hal_port->out_info.rect_w, out_hal_port->out_info.rect_h);
#endif
}

static hi_void cfg_out_crop(vpss_reg_port port, vpss_reg_type *vpss_reg,
    vpss_hal_port_info *out_hal_port, vpss_hal_info *in_hal_info)
{
    if ((port == VPSS_REG_PORT0) || (port == VPSS_REG_PORT_VIR)) {
        if (vpss_policy_support_output_crop() == HI_TRUE) {
            cfg_vhd0_crop(vpss_reg, out_hal_port, in_hal_info);
        }
    } else {
        vpss_error("no crop cfg \n");
        return;
    }

    return;
}

static hi_void cfg_out_lbx(vpss_reg_port port, vpss_reg_type *vpss_reg, vpss_hal_port_info *out_hal_port)
{
    if ((port == VPSS_REG_PORT0) || (port == VPSS_REG_PORT_VIR)) {
        if (vpss_policy_support_output_crop() == HI_TRUE) {
            cfg_vhd0_lbx(vpss_reg, out_hal_port);
        }
    }  else {
        vpss_error("no crop cfg \n");
        return;
    }

    return;
}

static hi_void cfg_out_zme(vpss_reg_port port, vpss_reg_type *vpss_reg,
    vpss_hal_port_info *out_hal_port, vpss_hal_info *in_hal_info)
{
    if ((port == VPSS_REG_PORT0) || (port == VPSS_REG_PORT_VIR)) {
        if (vpss_policy_support_pzme() == HI_TRUE) {
            cfg_vhd0_pzme(vpss_reg, out_hal_port);
        }

        if (vpss_policy_support_out0_zme() == HI_TRUE) {
            cfg_vhd0_out0_zme(vpss_reg, out_hal_port);
            get_pq_out0_zme_coef(vpss_reg, out_hal_port);
        }
    } else if (port == VPSS_REG_PORT1) {
        if (vpss_policy_support_pzme() == HI_TRUE) {
            cfg_pzme(vpss_reg, out_hal_port, in_hal_info);
        }
    } else {
        vpss_error("no zme cfg \n");
        return;
    }

    return;
}

static hi_void cfg_vhd0_ifir(vpss_reg_type *vpss_reg)
{
    hi_s32 ifir_coef[8] = { /* 8:max num */
        -7, 12, -17, 25,
        -38, 58, -105, 328
    };

    vpss_ifir_set_mode(vpss_reg, 0, HAL_IFIR_MODE_FILT);
    vpss_ifir_set_coef(vpss_reg, 0, ifir_coef, 8); /* 8:max num */

    return;
}

static hi_void cfg_vhd0_dfir(vpss_reg_type *vpss_reg, vpss_hal_port_info *out_hal_port)
{
    hi_u32 dfir420en;

    if ((out_hal_port->out_info.pixel_format == HI_DRV_PIXEL_FMT_NV21) ||
        (out_hal_port->out_info.pixel_format == HI_DRV_PIXEL_FMT_NV12)) {
        dfir420en = HI_TRUE;
    } else {
        dfir420en = HI_FALSE;
    }

    vpss_dfir_set_hfir_coef0(vpss_reg, 0, -16); /* -16:para */
    vpss_dfir_set_hfir_coef1(vpss_reg, 0, 0);
    vpss_dfir_set_hfir_coef2(vpss_reg, 0, 144); /* 144:para */
    vpss_dfir_set_hfir_coef3(vpss_reg, 0, 256); /* 256:para */
    vpss_dfir_set_hfir_coef4(vpss_reg, 0, 144); /* 144:para */
    vpss_dfir_set_hfir_coef5(vpss_reg, 0, 0);
    vpss_dfir_set_hfir_coef6(vpss_reg, 0, -16); /* -16:para */

    vpss_dfir_set422_en(vpss_reg, 0, 1);
    vpss_dfir_set420_en(vpss_reg, 0, dfir420en);
    vpss_dfir_set420_mode(vpss_reg, 0, 0);

    return;
}

static hi_void cfg_out_hdr(vpss_reg_port port, vpss_reg_type *vpss_reg,
    vpss_hal_port_info *out_hal_port, vpss_hal_info *in_hal_info)
{
    hi_s32 ret;
    hi_bool support = vpss_policy_support_hdr();
    hi_drv_pq_vpss_layer layer_id = HI_DRV_PQ_VPSS_LAYER_PORT0;
    hi_drv_pq_xdr_frame_info frame_info = { 0 };

    if ((support != HI_TRUE) || (out_hal_port->need_hdr != HI_TRUE)) {
        return;
    }

    vpss_sys_sethdr_en(vpss_reg, 0, out_hal_port->need_hdr);

    /* 422 -> 444 */
    cfg_vhd0_ifir(vpss_reg);

    frame_info.window_num = 1;
    frame_info.handle = in_hal_info->inst_id;
    frame_info.src_hdr_type = in_hal_info->in_info.src_hdr_type;
    frame_info.color_space_in = in_hal_info->in_info.src_color_desp;
    frame_info.disp_hdr_type = out_hal_port->out_info.out_hdr_type;
    frame_info.color_space_out = out_hal_port->out_info.out_color_desp;

    ret = vpss_comm_pq_get_hdr_cfg(layer_id, &frame_info, vpss_reg);
    if (ret != HI_SUCCESS) {
        vpss_error("get hdr cfg failed %d \n", ret);
        return;
    }

    /* 444 -> 420/420 */
    cfg_vhd0_dfir(vpss_reg, out_hal_port);

    vpss_hal_set_para_up_vhd_chn(vpss_reg, HAL_PARA_CHN_HDRCM);
    vpss_hal_set_para_up_vhd_chn(vpss_reg, HAL_PARA_CHN_HDRDEGAMMA);
    vpss_hal_set_para_up_vhd_chn(vpss_reg, HAL_PARA_CHN_HDRGAMMA);
    vpss_hal_set_para_up_vhd_chn(vpss_reg, HAL_PARA_CHN_HDRTMAP);
    vpss_hal_set_para_up_vhd_chn(vpss_reg, HAL_PARA_CHN_HDRCACM);

    return;
}

hi_bool get_uv_invert(hi_drv_pixel_format in_format, hi_drv_pixel_format out_format)
{
    hi_u32 i;

    for (i = 0; i < sizeof(g_uv_invert) / sizeof(vpss_hal_uv); i++) {
        if ((g_uv_invert[i].in_format == in_format)
            && (g_uv_invert[i].out_format == out_format)) {
            return g_uv_invert[i].uv_invert;
        }
    }

    vpss_error("CheckUseUvInvert error,infmt = %d, outfmt = %d\n", in_format, out_format);
    return HI_FALSE;
}

static hi_void vpss_hal_change_port_config_flag(vpss_hal_info *in_hal_info, vpss_reg_port port, hi_u32 *norm_port_used)
{
    if (port == VPSS_REG_PORT0) {
        (*norm_port_used)++;
        in_hal_info->port_used[VPSS_REG_PORT0] = HI_TRUE;
    }

    if (port == VPSS_REG_PORT1) {
        in_hal_info->port_used[VPSS_REG_PORT1] = HI_TRUE;
    }

    if (port == VPSS_REG_PORT_VIR) {
        in_hal_info->port_used[VPSS_REG_PORT_VIR] = HI_TRUE;
    }
}

static hi_void set_vhd0_cfg(vpss_reg_port port, vpss_reg_type *vpss_reg,
    vpss_hal_port_info *out_hal_port, vpss_hal_info *in_hal_info, vpss_mac_wchn_cfg *wchn_cfg)
{
    cfg_out_crop(port, vpss_reg, out_hal_port, in_hal_info);
    cfg_out_zme(port, vpss_reg, out_hal_port, in_hal_info);
    cfg_out_lbx(port, vpss_reg, out_hal_port);
    cfg_out_hdr(port, vpss_reg, out_hal_port, in_hal_info);
    wchn_cfg->uv_inv_en = get_uv_invert(in_hal_info->in_info.pixel_format, out_hal_port->out_info.pixel_format);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_OUT0, wchn_cfg);

    return;
}

hi_void set_port_cfg(vpss_hal_info *in_hal_info, vpss_reg_type *vpss_reg)
{
    hi_u32 count = 0;
    vpss_mac_wchn_cfg wchn_cfg = { 0 };
    hi_u32 port_available = DEF_VPSS_HAL_PORT_NUM;
    hi_bool port_used[VPSS_PORT_MAX_NUM] = { HI_FALSE, HI_FALSE, HI_FALSE };
    hi_u32 norm_port_used = 0;

    for (count = 0; (count < VPSS_PORT_MAX_NUM) && (port_available > 0); count++) {
        vpss_hal_port_info *out_hal_port = HI_NULL;
        out_hal_port = &in_hal_info->port_info[count];
        if ((out_hal_port->enable == HI_TRUE) && (out_hal_port->config == HI_FALSE)) {
            vpss_reg_port port = VPSS_REG_PORT_MAX;

            port = alloc_port_id(in_hal_info, out_hal_port, port_used, VPSS_PORT_MAX_NUM);
            if (port == VPSS_REG_PORT_MAX) {
                vpss_error("alloc_port_id failed ! port:%d \n", port);
                break;
            } else {
                port_available--;
            }

            if ((norm_port_used != 0) && (port == VPSS_REG_PORT_VIR)) {
                continue;
            }

            convert_to_wwbc_chn_cfg(&out_hal_port->out_info, &wchn_cfg);
            wchn_cfg.flip_en = out_hal_port->need_flip;
            wchn_cfg.mirror_en = out_hal_port->need_mirror;
            wchn_cfg.uv_inv_en = out_hal_port->uv_change;
            wchn_cfg.en = out_hal_port->enable;
            wchn_cfg.out_rect.x = 0;
            wchn_cfg.out_rect.y = 0;
            wchn_cfg.out_rect.wth = out_hal_port->out_info.rect_w;
            wchn_cfg.out_rect.hgt = out_hal_port->out_info.rect_h;

            /* PreZme only used in vsd ch */
            if ((port == VPSS_REG_PORT0) || (port == VPSS_REG_PORT_VIR)) {
                g_wchn_cfg[VPSS_MAC_WCHN_OUT0] = wchn_cfg;
                set_vhd0_cfg(port, vpss_reg, out_hal_port, in_hal_info, &wchn_cfg);
            } else if (port == VPSS_REG_PORT1) {
                g_wchn_cfg[VPSS_MAC_WCHN_OUT3] = wchn_cfg;
                cfg_out_zme(port, vpss_reg, out_hal_port, in_hal_info);
                vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_OUT3, &wchn_cfg);
            } else {
                vpss_error("enPort [%d] error\n", port);
            }

            vpss_hal_change_port_config_flag(in_hal_info, port, &norm_port_used);

            out_hal_port->config = HI_TRUE;
        }
    }
}

hi_void get_all_chn_cfg(vpss_hal_info *hal_info, vpss_mac_rchn_cfg *out_rchn_cfg, vpss_mac_wchn_cfg *wchn_cfg)
{
    vpss_hal_frame *in_info = HI_NULL;
    vpss_mac_rchn_cfg rchn_cfg = { 0 };
    vpss_mac_wchn_cfg wchn_cfg1 = { 0 };
    vpss_mac_rchn_cfg r_sttchn_cfg = { 0 };
    vpss_mac_wchn_cfg w_sttchn_cfg = { 0 };
    vpss_hal_stt_cfg *stt_cfg = HI_NULL;
    vpss_hal_wbc_cfg *wbc_cfg = HI_NULL;
    stt_cfg = &hal_info->stt_cfg;
    wbc_cfg = &hal_info->wbc_cfg;
    in_info = &hal_info->in_info;
    // config input chn
    convert_to_rwbc_chn_cfg(&hal_info->in_info, &rchn_cfg);
    out_rchn_cfg[VPSS_MAC_RCHN_CF] = rchn_cfg;

    // config nr cf & p1 chn
    if (in_info->progressive == HI_TRUE) {
        convert_to_rwbc_chn_cfg(&wbc_cfg->wbc_nr_cfg.rp1_frame, &rchn_cfg);
        out_rchn_cfg[VPSS_MAC_RCHN_NR_REF] = rchn_cfg;
    } else {
        convert_to_rwbc_chn_cfg(&wbc_cfg->wbc_nr_cfg.rp2_frame, &rchn_cfg);
        out_rchn_cfg[VPSS_MAC_RCHN_NR_REF] = rchn_cfg;
    }

    out_rchn_cfg[VPSS_MAC_RCHN_NR_REF].dcmp_cfg.cmp_ratio_y = VPSS_SUPPORT_CMP_NR_RATE;
    out_rchn_cfg[VPSS_MAC_RCHN_NR_REF].dcmp_cfg.cmp_ratio_c = VPSS_SUPPORT_CMP_NR_RATE;

    convert_to_wwbc_chn_cfg(&wbc_cfg->wbc_nr_cfg.w_frame, &wchn_cfg1);
    wchn_cfg[VPSS_MAC_WCHN_NR_RFR] = wchn_cfg1;
    wchn_cfg[VPSS_MAC_WCHN_NR_RFR].cmp_cfg.cmp_ratio_y = out_rchn_cfg[VPSS_MAC_RCHN_NR_REF].dcmp_cfg.cmp_ratio_y;
    wchn_cfg[VPSS_MAC_WCHN_NR_RFR].cmp_cfg.cmp_ratio_c = out_rchn_cfg[VPSS_MAC_RCHN_NR_REF].dcmp_cfg.cmp_ratio_c;

    convert_to_rstt_chn_cfg(&stt_cfg->rgme_cfg.r_rgmv_p1_chn, &r_sttchn_cfg);
    out_rchn_cfg[VPSS_MAC_RCHN_NR_P1RGMV] = r_sttchn_cfg;

    if (in_info->progressive == HI_TRUE) {
        convert_to_rstt_chn_cfg(&stt_cfg->rgme_cfg.r_rgmv_p1_chn, &r_sttchn_cfg);
        out_rchn_cfg[VPSS_MAC_RCHN_NR_P2RGMV] = r_sttchn_cfg;
    } else {
        convert_to_rstt_chn_cfg(&stt_cfg->rgme_cfg.r_rgmv_p2_chn, &r_sttchn_cfg);
        out_rchn_cfg[VPSS_MAC_RCHN_NR_P2RGMV] = r_sttchn_cfg;
    }

    if (in_info->rect_w <= VPSS_WIDTH_FHD) {
        convert_to_rstt_chn_cfg(&stt_cfg->nr_cfg.r_cnt_chn, &r_sttchn_cfg);
        out_rchn_cfg[VPSS_MAC_RCHN_NR_RCNT] = r_sttchn_cfg;
        convert_to_wstt_chn_cfg(&stt_cfg->nr_cfg.w_cnt_chn, &w_sttchn_cfg);
        wchn_cfg[VPSS_MAC_WCHN_NR_WCNT] = w_sttchn_cfg;
    }

    convert_to_rstt_chn_cfg(&stt_cfg->nr_cfg.r_tnr_mad_chn, &r_sttchn_cfg);
    out_rchn_cfg[VPSS_MAC_RCHN_NR_RMAD] = r_sttchn_cfg;
    convert_to_rstt_chn_cfg(&stt_cfg->nr_cfg.r_snr_mad_chn, &r_sttchn_cfg);
    out_rchn_cfg[VPSS_MAC_RCHN_SNR_RMAD] = r_sttchn_cfg;
    convert_to_wstt_chn_cfg(&stt_cfg->nr_cfg.w_mad_chn, &w_sttchn_cfg);
    wchn_cfg[VPSS_MAC_WCHN_NR_WMAD] = w_sttchn_cfg;
    convert_to_rstt_chn_cfg(&stt_cfg->me_cfg.w_me_for_nr_stt_chn, &r_sttchn_cfg);
    out_rchn_cfg[VPSS_MAC_RCHN_NR_CFMV] = r_sttchn_cfg;

    if (in_info->rect_w <= VPSS_WIDTH_FHD) {
        convert_to_wwbc_chn_cfg(&wbc_cfg->wbc_hds_cfg.w_frame, &wchn_cfg1);
        wchn_cfg[VPSS_MAC_WCHN_NR_RFRH] = wchn_cfg1;
        convert_to_wwbc_chn_cfg(&wbc_cfg->wbc_hvds_cfg.w_frame, &wchn_cfg1);
        wchn_cfg[VPSS_MAC_WCHN_NR_RFRHV] = wchn_cfg1;
    }

    convert_to_rstt_chn_cfg(&stt_cfg->rgme_cfg.r_rgmv_p1_chn, &r_sttchn_cfg);
    // config rgme chn
    out_rchn_cfg[VPSS_MAC_RCHN_RG_P1RGMV] = r_sttchn_cfg;

    if (in_info->progressive == HI_TRUE) {
        convert_to_rstt_chn_cfg(&stt_cfg->rgme_cfg.r_rgmv_p1_chn, &r_sttchn_cfg);
        out_rchn_cfg[VPSS_MAC_RCHN_RG_P2RGMV] = r_sttchn_cfg;
    } else {
        convert_to_rstt_chn_cfg(&stt_cfg->rgme_cfg.r_rgmv_p2_chn, &r_sttchn_cfg);
        out_rchn_cfg[VPSS_MAC_RCHN_RG_P2RGMV] = r_sttchn_cfg;
    }

    /* config dm430 dmcnt */
    convert_to_rstt_chn_cfg(&stt_cfg->dmcnt_cfg.r_cnt_chn, &r_sttchn_cfg);
    out_rchn_cfg[VPSS_MAC_RCHN_DMCNT] = r_sttchn_cfg;
    convert_to_wstt_chn_cfg(&stt_cfg->dmcnt_cfg.w_cnt_chn, &w_sttchn_cfg);
    wchn_cfg[VPSS_MAC_WCHN_DMCNT] = w_sttchn_cfg;

    convert_to_wstt_chn_cfg(&stt_cfg->rgme_cfg.w_rgmv_chn, &w_sttchn_cfg);
    wchn_cfg[VPSS_MAC_WCHN_RG_CFRGMV] = w_sttchn_cfg;
    convert_to_rstt_chn_cfg(&stt_cfg->rgme_cfg.r_prjh_chn, &r_sttchn_cfg);
    out_rchn_cfg[VPSS_MAC_RCHN_RG_RPRJH] = r_sttchn_cfg;
    convert_to_wstt_chn_cfg(&stt_cfg->rgme_cfg.w_prjh_chn, &w_sttchn_cfg);
    wchn_cfg[VPSS_MAC_WCHN_RG_WPRJH] = w_sttchn_cfg;
    convert_to_rstt_chn_cfg(&stt_cfg->rgme_cfg.r_prjv_chn, &r_sttchn_cfg);
    out_rchn_cfg[VPSS_MAC_RCHN_RG_RPRJV] = r_sttchn_cfg;
    convert_to_wstt_chn_cfg(&stt_cfg->rgme_cfg.w_prjv_chn, &w_sttchn_cfg);
    wchn_cfg[VPSS_MAC_WCHN_RG_WPRJV] = w_sttchn_cfg;
    /* config die chn */
    convert_to_rwbc_chn_cfg(&wbc_cfg->wbc_nr_cfg.w_frame, &rchn_cfg);
    out_rchn_cfg[VPSS_MAC_RCHN_DI_CF] = rchn_cfg;

    if (in_info->progressive != HI_TRUE) {
        convert_to_rwbc_chn_cfg(&wbc_cfg->wbc_nr_cfg.rp1_frame, &rchn_cfg);
        out_rchn_cfg[VPSS_MAC_RCHN_DI_P1] = rchn_cfg;
        convert_to_rwbc_chn_cfg(&wbc_cfg->wbc_nr_cfg.rp2_frame, &rchn_cfg);
        out_rchn_cfg[VPSS_MAC_RCHN_DI_P2] = rchn_cfg;
        convert_to_rwbc_chn_cfg(&wbc_cfg->wbc_nr_cfg.rp3_frame, &rchn_cfg);
        out_rchn_cfg[VPSS_MAC_RCHN_DI_P3] = rchn_cfg;
        convert_to_rwbc_chn_cfg(&wbc_cfg->wbc_dei_cfg.r_frame, &rchn_cfg);
        out_rchn_cfg[VPSS_MAC_RCHN_DI_P3I] = rchn_cfg;
        convert_to_wwbc_chn_cfg(&wbc_cfg->wbc_dei_cfg.w_frame, &wchn_cfg1);
        wchn_cfg[VPSS_MAC_WCHN_DI_RFR] = wchn_cfg1;
        convert_to_rstt_chn_cfg(&stt_cfg->dei_cfg.r_me_mv_p1_chn, &r_sttchn_cfg);
        out_rchn_cfg[VPSS_MAC_RCHN_DI_P1MV] = r_sttchn_cfg;
        convert_to_rstt_chn_cfg(&stt_cfg->dei_cfg.r_me_mv_p2_chn, &r_sttchn_cfg);
        out_rchn_cfg[VPSS_MAC_RCHN_DI_P2MV] = r_sttchn_cfg;
        convert_to_rstt_chn_cfg(&stt_cfg->dei_cfg.r_me_mv_p3_chn, &r_sttchn_cfg);
        out_rchn_cfg[VPSS_MAC_RCHN_DI_P3MV] = r_sttchn_cfg;
        convert_to_rstt_chn_cfg(&stt_cfg->dei_cfg.r_sad_y_chn, &r_sttchn_cfg);
        out_rchn_cfg[VPSS_MAC_RCHN_DI_RSADY] = r_sttchn_cfg;
        convert_to_wstt_chn_cfg(&stt_cfg->dei_cfg.w_sad_y_chn, &w_sttchn_cfg);
        wchn_cfg[VPSS_MAC_WCHN_DI_WSADY] = w_sttchn_cfg;
        convert_to_rstt_chn_cfg(&stt_cfg->dei_cfg.r_sad_c_chn, &r_sttchn_cfg);
        out_rchn_cfg[VPSS_MAC_RCHN_DI_RSADC] = r_sttchn_cfg;
        convert_to_wstt_chn_cfg(&stt_cfg->dei_cfg.w_sad_c_chn, &w_sttchn_cfg);
        wchn_cfg[VPSS_MAC_WCHN_DI_WSADC] = w_sttchn_cfg;
        convert_to_rstt_chn_cfg(&stt_cfg->dei_cfg.r_mad_chn, &r_sttchn_cfg);
        out_rchn_cfg[VPSS_MAC_RCHN_DI_RHISM] = r_sttchn_cfg;
        convert_to_wstt_chn_cfg(&stt_cfg->dei_cfg.w_mad_chn, &w_sttchn_cfg);
        wchn_cfg[VPSS_MAC_WCHN_DI_WHISM] = w_sttchn_cfg;
    }

    /* config cccl chn */
    if (wbc_cfg->wbc_cccl_cfg.w_frame.hal_addr.phy_addr_y != 0) {
        convert_to_rwbc_chn_cfg(&wbc_cfg->wbc_cccl_cfg.rp2_frame, &rchn_cfg);
        out_rchn_cfg[VPSS_MAC_RCHN_CC_PR0] = rchn_cfg;
        convert_to_rwbc_chn_cfg(&wbc_cfg->wbc_cccl_cfg.rp4_frame, &rchn_cfg);
        out_rchn_cfg[VPSS_MAC_RCHN_CC_PR4] = rchn_cfg;
        convert_to_rwbc_chn_cfg(&wbc_cfg->wbc_cccl_cfg.rp8_frame, &rchn_cfg);
        out_rchn_cfg[VPSS_MAC_RCHN_CC_PR8] = rchn_cfg;
        convert_to_wwbc_chn_cfg(&wbc_cfg->wbc_cccl_cfg.w_frame, &wchn_cfg1);
        wchn_cfg[VPSS_MAC_WCHN_CC_RFR] = wchn_cfg1;
        convert_to_rstt_chn_cfg(&stt_cfg->cccl_cfg.r_mad_y_chn, &r_sttchn_cfg);
        out_rchn_cfg[VPSS_MAC_RCHN_CC_RYCNT] = r_sttchn_cfg;
        convert_to_wstt_chn_cfg(&stt_cfg->cccl_cfg.w_mad_y_chn, &w_sttchn_cfg);
        wchn_cfg[VPSS_MAC_WCHN_CC_WYCNT] = w_sttchn_cfg;
        convert_to_rstt_chn_cfg(&stt_cfg->cccl_cfg.r_mad_c_chn, &r_sttchn_cfg);
        out_rchn_cfg[VPSS_MAC_RCHN_CC_RCCNT] = r_sttchn_cfg;
        convert_to_wstt_chn_cfg(&stt_cfg->cccl_cfg.w_mad_c_chn, &w_sttchn_cfg);
        wchn_cfg[VPSS_MAC_WCHN_CC_WCCNT] = w_sttchn_cfg;
    }

    if (in_info->rect_w <= VPSS_WIDTH_FHD) {
        convert_to_wstt_chn_cfg(&stt_cfg->me_cfg.w_mv_chn, &w_sttchn_cfg);
        wchn_cfg[VPSS_MAC_WCHN_ME_CFMV] = w_sttchn_cfg;
        convert_to_wstt_chn_cfg(&stt_cfg->me_cfg.wg_mv_chn, &w_sttchn_cfg);
        wchn_cfg[VPSS_MAC_WCHN_ME_CFGMV] = w_sttchn_cfg;
    }
}

hi_void get_cf_chn_cfg(vpss_hal_info *hal_info, vpss_mac_rchn_cfg *out_rchn_cfg, vpss_mac_wchn_cfg *wchn_cfg)
{
    vpss_hal_frame *in_info = HI_NULL;
    vpss_mac_rchn_cfg rchn_cfg = { 0 };
    in_info = &hal_info->in_info;
    /* config input chn */
    convert_to_rchn_cf_cfg(&hal_info->in_info, &rchn_cfg);
    out_rchn_cfg[VPSS_MAC_RCHN_CF] = rchn_cfg;
}

xdp_data_rmode convert_hal_rmode(vpss_hal_rmode rd_mode)
{
    xdp_data_rmode xd_pmode = XDP_RMODE_MAX;

    switch (rd_mode) {
        case VPSS_HAL_RMODE_PROGRESSIVE:
            xd_pmode = XDP_RMODE_PROGRESSIVE;
            break;
        case VPSS_HAL_RMODE_INTER_TOP:
            xd_pmode = XDP_RMODE_TOP;
            break;
        case VPSS_HAL_RMODE_INTER_BOTTOM:
            xd_pmode = XDP_RMODE_BOTTOM;
            break;
        case VPSS_HAL_RMODE_PROG_TOP:
            xd_pmode = XDP_RMODE_PRO_TOP;
            break;
        case VPSS_HAL_RMODE_PROG_BOTTOM:
            xd_pmode = XDP_RMODE_PRO_BOTTOM;
            break;
        default:
            xd_pmode = XDP_RMODE_MAX;
            break;
    }
    return xd_pmode;
}

static hi_void set_4field_cccl(vpss_hal_info *hal_info, vpss_reg_type *vpss_reg,
    vpss_mac_rchn_cfg *rchn_cfg, vpss_mac_wchn_cfg *wchn_cfg)
{
    hi_u32 cccl_support_w;
    hi_u32 cccl_support_h;
    hi_bool cccl_support = HI_FALSE;

    /* config cccl chn */
    cccl_support = vpss_policy_support_cccl();
    if (cccl_support == HI_TRUE) {
        vpss_policy_support_cccl_height_and_width(&cccl_support_w, &cccl_support_h);
        if ((rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth <= cccl_support_w)
            && (rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.hgt <= cccl_support_h)) {
            if ((XDP_PROC_FMT_SP_420 == rchn_cfg[VPSS_MAC_RCHN_CF].data_fmt)
                && VPSS_SUPPORT_CF_UPSAMP) {
                vpss_mac_set_cf_cconvert(vpss_reg, HI_TRUE);
            } else {
                vpss_mac_set_cf_cconvert(vpss_reg, HI_FALSE);
            }

#if (!defined HI_VPSS_DRV_USE_GOLDEN) && (!defined HI_VPSS_DRV_USE_GOLDEN_COEF)
            if (hal_info->complete_cnt > HAL_SUPPORT_CCCLCOUNT) {
#endif
                vpss_sys_set_cccl_en(vpss_reg, 0, HI_TRUE);
                vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_CC_PR0, &rchn_cfg[VPSS_MAC_RCHN_CC_PR0]);
                vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_CC_PR4, &rchn_cfg[VPSS_MAC_RCHN_CC_PR4]);
                vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_CC_PR8, &rchn_cfg[VPSS_MAC_RCHN_CC_PR8]);
                vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_CC_RFR, &wchn_cfg[VPSS_MAC_WCHN_CC_RFR]);
                vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_CC_RYCNT, &rchn_cfg[VPSS_MAC_RCHN_CC_RYCNT]);
                vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_CC_RCCNT, &rchn_cfg[VPSS_MAC_RCHN_CC_RCCNT]);
                vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_CC_WYCNT, &wchn_cfg[VPSS_MAC_WCHN_CC_WYCNT]);
                vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_CC_WCCNT, &wchn_cfg[VPSS_MAC_WCHN_CC_WCCNT]);
#if (!defined HI_VPSS_DRV_USE_GOLDEN) && (!defined HI_VPSS_DRV_USE_GOLDEN_COEF)
            }
#endif
        }
    }

    return;
}

xdp_me_version convert_me_version(vpss_hal_3drs_type me_version)
{
    xdp_me_version xdp_version;

    switch (me_version) {
        case VPSS_HAL_3DRS_TYPE_V3:
            xdp_version = XDP_ME_VER3;
            break;
        case VPSS_HAL_3DRS_TYPE_V4:
            xdp_version = XDP_ME_VER4;
            break;
        case VPSS_HAL_3DRS_TYPE_V5:
            xdp_version = XDP_ME_VER5;
            break;
        default:
            xdp_version = XDP_ME_VER_MAX;
            break;
    }

    return xdp_version;
}

hi_s32 set_4field_node(vpss_hal_info *hal_info, vpss_reg_type *vpss_reg, hi_u64 phy_addr)
{
    /* 隔行配置的时候
           1  像素回写的通路，逻辑要求配置帧高，需要纠正读通道配置
        在buffer管理模块分配buffer的时候，实际上分配的是场高=帧高/2
           2 像素读写通道，尤其是DIE的读写通道，需要配置读取模式，是逐行读取顶场
              还是逐行读取低场
           3 逻辑是按照逐行读取，还是隔行读取输入帧，是看CF通道的读取模式来判断，
              是逐行读取，还是隔行读取顶场，隔行读取低场
    */
    hi_bool meds_en;
    hi_u32 safe_mode;
    hi_drv_field_mode cf_field_mode;
    vpss_mac_rchn_cfg *rchn_cfg;
    vpss_mac_wchn_cfg *wchn_cfg;
    vpss_mc_info mc_info;
    vpss_hal_3drs_type me_version;
    xdp_3_drs_version en3_drs_version;
    vpss_hal_stt_cfg *stt_cfg;
    vpss_hal_wbc_cfg *wbc_cfg;

    stt_cfg = &hal_info->stt_cfg;
    wbc_cfg = &hal_info->wbc_cfg;
    rchn_cfg = &g_rchn_cfg[VPSS_MAC_RCHN_CF];
    wchn_cfg = &g_wchn_cfg[VPSS_MAC_WCHN_OUT0];
    me_version = hal_info->en3_drs;
    en3_drs_version = HAL_VPSS_SEL_MC;
    cf_field_mode = hal_info->in_info.field_mode;
    if (hal_info->use_pq_module == HI_TRUE) {
        init_regtable(vpss_reg, &hal_info->pq_reg_data);
    } else {
        init_regtable(vpss_reg, HI_NULL);
    }

    safe_mode = get_safe_mode(&hal_info->in_info);
    (hi_void)init_all_chn_cfg(vpss_reg);
    get_all_chn_cfg(hal_info, rchn_cfg, wchn_cfg);
    vpss_mac_set_reg_load(vpss_reg);
    vpss_mac_set_prot(vpss_reg, safe_mode);

    if (hal_info->in_frame_type == VPSS_HAL_FRAME_TYPE_FIRST) {
        rchn_cfg[VPSS_MAC_RCHN_NR_REF].mute_cfg.mute_en = HI_TRUE;
        rchn_cfg[VPSS_MAC_RCHN_NR_REF].mute_cfg.mute_y = 0x0;
        rchn_cfg[VPSS_MAC_RCHN_NR_REF].mute_cfg.mute_c = 0x0;
    }

    // config input chn
    if (cf_field_mode == HI_DRV_FIELD_TOP) {
        rchn_cfg[VPSS_MAC_RCHN_CF].rd_mode = convert_hal_rmode(hal_info->in_info.rd_mode);
        rchn_cfg[VPSS_MAC_RCHN_NR_REF].rd_mode = XDP_RMODE_PRO_TOP;
        rchn_cfg[VPSS_MAC_RCHN_DI_CF].rd_mode = XDP_RMODE_PRO_TOP;
        rchn_cfg[VPSS_MAC_RCHN_DI_P1].rd_mode = XDP_RMODE_PRO_BOTTOM;
        rchn_cfg[VPSS_MAC_RCHN_DI_P2].rd_mode = XDP_RMODE_PRO_TOP;
        rchn_cfg[VPSS_MAC_RCHN_DI_P3].rd_mode = XDP_RMODE_PRO_BOTTOM;
    } else {
        rchn_cfg[VPSS_MAC_RCHN_CF].rd_mode = convert_hal_rmode(hal_info->in_info.rd_mode);
        rchn_cfg[VPSS_MAC_RCHN_NR_REF].rd_mode = XDP_RMODE_PRO_BOTTOM;
        rchn_cfg[VPSS_MAC_RCHN_DI_CF].rd_mode = XDP_RMODE_PRO_BOTTOM;
        rchn_cfg[VPSS_MAC_RCHN_DI_P1].rd_mode = XDP_RMODE_PRO_TOP;
        rchn_cfg[VPSS_MAC_RCHN_DI_P2].rd_mode = XDP_RMODE_PRO_BOTTOM;
        rchn_cfg[VPSS_MAC_RCHN_DI_P3].rd_mode = XDP_RMODE_PRO_TOP;
    }

    // pstRchnCfg[VPSS_MAC_RCHN_NR_REF].stInRect.u32Wth = pstRchnCfg[VPSS_MAC_RCHN_CF].stInRect.u32Wth;
    // pstRchnCfg[VPSS_MAC_RCHN_NR_REF].stInRect.u32Hgt = pstRchnCfg[VPSS_MAC_RCHN_CF].stInRect.u32Hgt;
    rchn_cfg[VPSS_MAC_RCHN_DI_CF].in_rect.wth = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth;
    rchn_cfg[VPSS_MAC_RCHN_DI_CF].in_rect.hgt = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.hgt;
    rchn_cfg[VPSS_MAC_RCHN_DI_P1].in_rect.wth = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth;
    rchn_cfg[VPSS_MAC_RCHN_DI_P1].in_rect.hgt = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.hgt;
    rchn_cfg[VPSS_MAC_RCHN_DI_P2].in_rect.wth = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth;
    rchn_cfg[VPSS_MAC_RCHN_DI_P2].in_rect.hgt = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.hgt;
    rchn_cfg[VPSS_MAC_RCHN_DI_P3].in_rect.wth = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth;
    rchn_cfg[VPSS_MAC_RCHN_DI_P3].in_rect.hgt = rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.hgt;
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_CF, &rchn_cfg[VPSS_MAC_RCHN_CF]);

    if (rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth > HAL_CF_WIDTH) {
        meds_en = HI_TRUE;
    } else {
        meds_en = HI_FALSE;
    }

    /* -----------------config 3drs---------------------- */
    vpss_sys_set_vpss3drs_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_ma_mac_sel(vpss_reg, 0, en3_drs_version);
    vpss_sys_set_me_version(vpss_reg, 0, me_version);
    vpss_sys_set_tnr_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_snr_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_mcnr_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_rgme_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_meds_en(vpss_reg, 0, meds_en);
    vpss_sys_set_hfr_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_lbd_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_scd_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_vc1_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_rotate_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_cccl_en(vpss_reg, 0, HI_FALSE);

    set_vc1_cfg(vpss_reg, hal_info, VPSS_MAC_RCHN_CF);
    /* config nr cf & p1 chn */
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_REF, &rchn_cfg[VPSS_MAC_RCHN_NR_REF]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_RFR, &wchn_cfg[VPSS_MAC_WCHN_NR_RFR]);
    /* config stt nr rgme */
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_P1RGMV, &rchn_cfg[VPSS_MAC_RCHN_NR_P1RGMV]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_P2RGMV, &rchn_cfg[VPSS_MAC_RCHN_NR_P2RGMV]);
    /* config stt nr cnt */
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_RCNT, &rchn_cfg[VPSS_MAC_RCHN_NR_RCNT]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_WCNT, &wchn_cfg[VPSS_MAC_WCHN_NR_WCNT]);
    /* config stt nr mad */
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_RMAD, &rchn_cfg[VPSS_MAC_RCHN_NR_RMAD]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_SNR_RMAD, &rchn_cfg[VPSS_MAC_RCHN_SNR_RMAD]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_WMAD, &wchn_cfg[VPSS_MAC_WCHN_NR_WMAD]);
    /* config stt nr read me blkmv */
    rchn_cfg[VPSS_MAC_RCHN_NR_CFMV].tunl_en = HI_TRUE;
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_CFMV, &rchn_cfg[VPSS_MAC_RCHN_NR_CFMV]);
    /* config stt nr hdow cf & p1 chn */
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_RFRH, &wchn_cfg[VPSS_MAC_WCHN_NR_RFRH]);
    /* config stt nr vdow cf & p1 chn */
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_RFRHV, &wchn_cfg[VPSS_MAC_WCHN_NR_RFRHV]);
    /* config rgme chn */
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_P1RGMV, &rchn_cfg[VPSS_MAC_RCHN_RG_P1RGMV]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_P2RGMV, &rchn_cfg[VPSS_MAC_RCHN_RG_P2RGMV]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_RG_CFRGMV, &wchn_cfg[VPSS_MAC_WCHN_RG_CFRGMV]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_RPRJH, &rchn_cfg[VPSS_MAC_RCHN_RG_RPRJH]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_RG_WPRJH, &wchn_cfg[VPSS_MAC_WCHN_RG_WPRJH]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_RPRJV, &rchn_cfg[VPSS_MAC_RCHN_RG_RPRJV]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_RG_WPRJV, &wchn_cfg[VPSS_MAC_WCHN_RG_WPRJV]);
    /* config die chn */
    vpss_sys_set_dei_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_mcdi_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_ifmd_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_igbm_en(vpss_reg, 0, HI_TRUE);
    rchn_cfg[VPSS_MAC_RCHN_DI_CF].tunl_en = HI_TRUE;
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_CF, &rchn_cfg[VPSS_MAC_RCHN_DI_CF]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_P1, &rchn_cfg[VPSS_MAC_RCHN_DI_P1]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_P2, &rchn_cfg[VPSS_MAC_RCHN_DI_P2]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_P3, &rchn_cfg[VPSS_MAC_RCHN_DI_P3]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_P3I, &rchn_cfg[VPSS_MAC_RCHN_DI_P3I]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_DI_RFR, &wchn_cfg[VPSS_MAC_WCHN_DI_RFR]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_P1MV, &rchn_cfg[VPSS_MAC_RCHN_DI_P1MV]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_P2MV, &rchn_cfg[VPSS_MAC_RCHN_DI_P2MV]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_P3MV, &rchn_cfg[VPSS_MAC_RCHN_DI_P3MV]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_RHISM, &rchn_cfg[VPSS_MAC_RCHN_DI_RHISM]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_DI_WHISM, &wchn_cfg[VPSS_MAC_WCHN_DI_WHISM]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_RSADY, &rchn_cfg[VPSS_MAC_RCHN_DI_RSADY]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_DI_WSADY, &wchn_cfg[VPSS_MAC_WCHN_DI_WSADY]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_RSADC, &rchn_cfg[VPSS_MAC_RCHN_DI_RSADC]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_DI_WSADC, &wchn_cfg[VPSS_MAC_WCHN_DI_WSADC]);

    if (hal_info->vpss_alg_ctrl.dm430_en == HI_TRUE) {
        vpss_sys_setdbm_430_en(vpss_reg, 0, HI_TRUE);
        vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DMCNT, &rchn_cfg[VPSS_MAC_RCHN_DMCNT]);
        vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_DMCNT, &wchn_cfg[VPSS_MAC_WCHN_DMCNT]);
    } else {
        vpss_sys_setdbm_430_en(vpss_reg, 0, HI_FALSE);
    }

    set_4field_cccl(hal_info, vpss_reg, rchn_cfg, wchn_cfg);

    /* config me chn */
    mc_info.en3_drs_version = en3_drs_version;
    mc_info.me_version = convert_me_version(me_version);
    mc_info.me_scan_num = hal_info->me_scan_num;
    mc_info.pro = (rchn_cfg[VPSS_MAC_RCHN_CF].rd_mode == XDP_RMODE_PROGRESSIVE) ? 1 : 0;
    mc_info.meds_en = meds_en;
    mc_info.wbc_cfg = wbc_cfg;
    mc_info.stt_cfg = stt_cfg;
    mc_info.vc1_info = &hal_info->in_info.vc1_info;
    mc_set(vpss_reg, phy_addr, &mc_info, rchn_cfg, wchn_cfg);
    /* global  stt cfg */
    vpss_sys_set_stt_raddr(vpss_reg, 0, 0x0, stt_cfg->global_cfg.r_stt_chn.phy_addr + 0x300);
    vpss_sys_set_stt_waddr(vpss_reg, 0, 0x0, stt_cfg->global_cfg.w_stt_chn.phy_addr);
    /* config output chn */
    set_port_cfg(hal_info, vpss_reg);
#if (defined HI_VPSS_DRV_USE_GOLDEN) || (defined HI_VPSS_DRV_USE_GOLDEN_COEF)
    vpss_hal_refresh_golden_reg_table(vpss_reg, rchn_cfg, wchn_cfg);
#endif
    /* must cfg to default para */
    vpss_rchn_set_cf_vid_default_cfg(vpss_reg);

    return HI_SUCCESS;
}

hi_void vpss_hal_golden_write_chn_file(hi_void)
{
#ifdef HI_VPSS_DRV_USE_GOLDEN
    hi_s32 ret;
    vpss_mac_wchn_cfg *wchn_cfg;
    wchn_cfg = &g_wchn_cfg[VPSS_MAC_WCHN_OUT0];
    ret = vpss_hal_write_chn_file(wchn_cfg);
    if (ret != HI_SUCCESS) {
        vpss_error("vpss_hal_write_chn_file error\n");
    }
#endif
    return;
}

static hi_void set_all_pq_disable(vpss_reg_type *vpss_reg)
{
    vpss_mac_set_cf_cconvert(vpss_reg, HI_FALSE);
    vpss_sys_set_vpss3drs_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_tnr_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_snr_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_mcnr_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_rgme_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_meds_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_tnr_rec_8bit_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_hfr_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_lbd_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_hcti_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_scd_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_vc1_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_rotate_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_dei_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_mcdi_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_ifmd_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_igbm_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_cccl_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_setdbm_430_en(vpss_reg, 0, HI_FALSE);

    return;
}

hi_s32 set_rotate_node(vpss_hal_info *hal_info, vpss_reg_type *vpss_reg, hi_u64 phy_addr,
                       hi_bool rotate_y, hi_u32 port_id)
{
    hi_u32 safe_mode;
    vpss_mac_rchn_cfg *rchn_cfg = &g_rchn_cfg[VPSS_MAC_RCHN_CF];
    vpss_hal_port_info *hal_port = HI_NULL;
    vpss_mac_wchn_cfg wchn_cfg = { 0 };

    if (hal_info->use_pq_module == HI_TRUE) {
        init_regtable(vpss_reg, &hal_info->pq_reg_data);
    } else {
        init_regtable(vpss_reg, HI_NULL);
    }

    safe_mode = get_safe_mode(&hal_info->in_info);
    init_cf_out0_chn_cfg(vpss_reg);
    get_cf_chn_cfg(hal_info, rchn_cfg, &wchn_cfg);
    vpss_mac_set_reg_load(vpss_reg);
    vpss_mac_set_prot(vpss_reg, safe_mode);
    vpss_mac_set_rchn_cf_cfg(vpss_reg, VPSS_MAC_RCHN_CF, &rchn_cfg[VPSS_MAC_RCHN_CF]);
    /* 4ppc on */
    vpss_sys_set_four_pixel_en(vpss_reg, 0, HI_TRUE);
    set_all_pq_disable(vpss_reg);

    vpss_mac_set_cf_rtunl_en(vpss_reg, HI_FALSE);
    vpss_sys_set_snr_mad_disable(vpss_reg, 0, HI_FALSE);

    /* 输出配置 */
    hal_port = &hal_info->port_info[port_id];
    convert_to_wwbc_chn_cfg(&hal_port->out_info, &wchn_cfg);

    if (hal_info->port_info[port_id].rotation == HI_DRV_VPSS_ROTATION_180) {
        wchn_cfg.flip_en = HI_TRUE;
        wchn_cfg.mirror_en = HI_TRUE;
        wchn_cfg.uv_inv_en = get_uv_invert(hal_info->in_info.pixel_format, hal_port->out_info.pixel_format);
    }

    g_wchn_cfg[VPSS_MAC_WCHN_OUT0] = wchn_cfg;
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_OUT0, &wchn_cfg);
    /* 旋转配置 */
    vpss_reg_set_rotate_cfg(vpss_reg, hal_port->rotation, rotate_y);
    /* must cfg to default para */
    vpss_rchn_set_cf_vid_default_cfg(vpss_reg);
    return HI_SUCCESS;
}

hi_s32 set_frame_no_me_node(vpss_hal_info *hal_info, vpss_reg_type *vpss_reg, hi_u64 phy_addr)
{
    hi_bool meds_en = HI_FALSE;
    hi_u32 safe_mode;
    vpss_mac_rchn_cfg *rchn_cfg;
    vpss_mac_wchn_cfg *wchn_cfg;
    vpss_mc_info mc_info;
    vpss_hal_3drs_type me_version;
    xdp_3_drs_version en3_drs_version;
    vpss_hal_stt_cfg *stt_cfg;
    vpss_hal_wbc_cfg *wbc_cfg;
    stt_cfg = &hal_info->stt_cfg;
    wbc_cfg = &hal_info->wbc_cfg;
    rchn_cfg = &g_rchn_cfg[VPSS_MAC_RCHN_CF];
    wchn_cfg = &g_wchn_cfg[VPSS_MAC_WCHN_OUT0];
    me_version = hal_info->en3_drs;
    en3_drs_version = HAL_VPSS_SEL_MA;

    if (hal_info->use_pq_module == HI_TRUE) {
        init_regtable(vpss_reg, &hal_info->pq_reg_data);
    } else {
        init_regtable(vpss_reg, HI_NULL);
    }

    safe_mode = get_safe_mode(&hal_info->in_info);
    (hi_void)init_all_chn_cfg(vpss_reg);
    get_all_chn_cfg(hal_info, rchn_cfg, wchn_cfg);
#if (defined HI_VPSS_DRV_USE_GOLDEN) || (defined HI_VPSS_DRV_USE_GOLDEN_COEF)
    vpss_hal_refresh_golden_reg_table(vpss_reg, rchn_cfg, wchn_cfg);
#endif
    vpss_mac_set_reg_load(vpss_reg);
    vpss_mac_set_prot(vpss_reg, safe_mode);
    /* config input chn */
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_CF, &rchn_cfg[VPSS_MAC_RCHN_CF]);

    if (rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth > HAL_CF_WIDTH) {
        meds_en = HI_TRUE;
    } else {
        meds_en = HI_FALSE;
    }

    vpss_sys_set_four_pixel_en(vpss_reg, 0, HI_FALSE);
    /* -----------------config 3drs---------------------- */
    vpss_sys_set_vpss3drs_en(vpss_reg, 0, HI_FALSE); /* todo:close tmp */
    vpss_sys_set_ma_mac_sel(vpss_reg, 0, en3_drs_version);
    vpss_sys_set_me_version(vpss_reg, 0, me_version);
    vpss_sys_set_tnr_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_snr_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_mcnr_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_rgme_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_meds_en(vpss_reg, 0, meds_en);
    vpss_sys_set_hfr_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_lbd_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_scd_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_vc1_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_rotate_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_dei_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_mcdi_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_ifmd_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_igbm_en(vpss_reg, 0, HI_FALSE);

    set_vc1_cfg(vpss_reg, hal_info, VPSS_MAC_RCHN_CF);

    if (hal_info->in_info.tunnel_addr != 0) {
        vpss_mac_set_cf_rtunl_en(vpss_reg, HI_TRUE);
        vpss_mac_set_cf_rtunl_addr_h(vpss_reg, 0x0);
        vpss_mac_set_cf_rtunl_addr_l(vpss_reg, hal_info->in_info.tunnel_addr);
    } else {
        vpss_mac_set_cf_rtunl_en(vpss_reg, HI_FALSE);
    }

    /* config nr cf & p1 chn */
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_REF, &rchn_cfg[VPSS_MAC_RCHN_NR_REF]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_RFR, &wchn_cfg[VPSS_MAC_WCHN_NR_RFR]);
    /* config stt nr rgme */
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_P1RGMV, &rchn_cfg[VPSS_MAC_RCHN_NR_P1RGMV]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_P2RGMV, &rchn_cfg[VPSS_MAC_RCHN_NR_P2RGMV]);
    /* config stt nr cnt */
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_RCNT, &rchn_cfg[VPSS_MAC_RCHN_NR_RCNT]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_WCNT, &wchn_cfg[VPSS_MAC_WCHN_NR_WCNT]);
    /* config stt nr mad */
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_RMAD, &rchn_cfg[VPSS_MAC_RCHN_NR_RMAD]);
    rchn_cfg[VPSS_MAC_RCHN_SNR_RMAD].tunl_en = HI_TRUE;
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_SNR_RMAD, &rchn_cfg[VPSS_MAC_RCHN_SNR_RMAD]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_WMAD, &wchn_cfg[VPSS_MAC_WCHN_NR_WMAD]);
    /* config stt nr read me blkmv */
    rchn_cfg[VPSS_MAC_RCHN_NR_CFMV].tunl_en = HI_TRUE;
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_CFMV, &rchn_cfg[VPSS_MAC_RCHN_NR_CFMV]);
    /* config stt nr hdow cf & p1 chn */
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_RFRH, &wchn_cfg[VPSS_MAC_WCHN_NR_RFRH]);
    /* config stt nr vdow cf & p1 chn */
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_RFRHV, &wchn_cfg[VPSS_MAC_WCHN_NR_RFRHV]);
    /* config rgme chn */
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_P1RGMV, &rchn_cfg[VPSS_MAC_RCHN_RG_P1RGMV]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_P2RGMV, &rchn_cfg[VPSS_MAC_RCHN_RG_P2RGMV]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_RG_CFRGMV, &wchn_cfg[VPSS_MAC_WCHN_RG_CFRGMV]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_RPRJH, &rchn_cfg[VPSS_MAC_RCHN_RG_RPRJH]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_RG_WPRJH, &wchn_cfg[VPSS_MAC_WCHN_RG_WPRJH]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_RPRJV, &rchn_cfg[VPSS_MAC_RCHN_RG_RPRJV]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_RG_WPRJV, &wchn_cfg[VPSS_MAC_WCHN_RG_WPRJV]);
    /* config die chn */
    vpss_sys_set_dei_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_mcdi_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_ifmd_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_igbm_en(vpss_reg, 0, HI_FALSE);
    rchn_cfg[VPSS_MAC_RCHN_DI_CF].tunl_en = HI_TRUE;
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_CF, &rchn_cfg[VPSS_MAC_RCHN_DI_CF]);

    if (hal_info->vpss_alg_ctrl.dm430_en == HI_TRUE) {
        vpss_sys_setdbm_430_en(vpss_reg, 0, HI_TRUE);
        vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DMCNT, &rchn_cfg[VPSS_MAC_RCHN_DMCNT]);
        vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_DMCNT, &wchn_cfg[VPSS_MAC_WCHN_DMCNT]);
    } else {
        vpss_sys_setdbm_430_en(vpss_reg, 0, HI_FALSE);
    }
    /* config cccl chn */
    /* config me chn */
    mc_info.en3_drs_version = en3_drs_version;
    mc_info.me_version = convert_me_version(me_version);
    mc_info.me_scan_num = hal_info->me_scan_num;
    mc_info.pro = (rchn_cfg[VPSS_MAC_RCHN_CF].rd_mode == XDP_RMODE_PROGRESSIVE) ? 1 : 0;
    mc_info.meds_en = meds_en;
    mc_info.wbc_cfg = wbc_cfg;
    mc_info.stt_cfg = stt_cfg;
    mc_info.vc1_info = &hal_info->in_info.vc1_info;
    mc_set(vpss_reg, phy_addr, &mc_info, rchn_cfg, wchn_cfg);
    /* global  stt cfg */
    vpss_sys_set_stt_raddr(vpss_reg, 0, 0x0, stt_cfg->global_cfg.r_stt_chn.phy_addr + 0x300);
    vpss_sys_set_stt_waddr(vpss_reg, 0, 0x0, stt_cfg->global_cfg.w_stt_chn.phy_addr);
    /* config output chn */
    set_port_cfg(hal_info, vpss_reg);
    /* must cfg to default para */
    vpss_rchn_set_cf_vid_default_cfg(vpss_reg);
    return HI_SUCCESS;
}

hi_s32 set_frame_node(vpss_hal_info *hal_info, vpss_reg_type *vpss_reg, hi_u64 phy_addr)
{
    hi_bool meds_en = HI_FALSE;
    hi_u32 cccl_support_w;
    hi_u32 cccl_support_h;
    hi_bool cccl_support = HI_FALSE;
    hi_u32 safe_mode;
    vpss_mac_rchn_cfg *rchn_cfg;
    vpss_mac_wchn_cfg *wchn_cfg;
    vpss_mc_info mc_info;
    vpss_hal_3drs_type me_version;
    xdp_3_drs_version en3_drs_version;
    vpss_hal_stt_cfg *stt_cfg;
    vpss_hal_wbc_cfg *wbc_cfg;

    stt_cfg = &hal_info->stt_cfg;
    wbc_cfg = &hal_info->wbc_cfg;
    rchn_cfg = &g_rchn_cfg[VPSS_MAC_RCHN_CF];
    wchn_cfg = &g_wchn_cfg[VPSS_MAC_WCHN_OUT0];
    me_version = hal_info->en3_drs;
    en3_drs_version = HAL_VPSS_SEL_MC;

    if (hal_info->use_pq_module == HI_TRUE) {
        init_regtable(vpss_reg, &hal_info->pq_reg_data);
    } else {
        init_regtable(vpss_reg, HI_NULL);
    }

    safe_mode = get_safe_mode(&hal_info->in_info);
    cccl_support = vpss_policy_support_cccl();
    (hi_void)init_all_chn_cfg(vpss_reg);
    get_all_chn_cfg(hal_info, rchn_cfg, wchn_cfg);
#if (defined HI_VPSS_DRV_USE_GOLDEN) || (defined HI_VPSS_DRV_USE_GOLDEN_COEF)
    vpss_hal_refresh_golden_reg_table(vpss_reg, rchn_cfg, wchn_cfg);
#endif
    vpss_mac_set_reg_load(vpss_reg);
    vpss_mac_set_prot(vpss_reg, safe_mode);
    /* config input chn */
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_CF, &rchn_cfg[VPSS_MAC_RCHN_CF]);

    if (rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth > HAL_CF_WIDTH) {
        meds_en = HI_TRUE;
    } else {
        meds_en = HI_FALSE;
    }

    vpss_sys_set_four_pixel_en(vpss_reg, 0, HI_FALSE);
    /* -----------------config 3drs---------------------- */
    vpss_sys_set_vpss3drs_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_ma_mac_sel(vpss_reg, 0, en3_drs_version);
    vpss_sys_set_me_version(vpss_reg, 0, me_version);
    vpss_sys_set_tnr_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_snr_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_mcnr_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_rgme_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_meds_en(vpss_reg, 0, meds_en);
    vpss_sys_set_hfr_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_lbd_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_scd_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_vc1_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_rotate_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_dei_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_mcdi_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_ifmd_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_igbm_en(vpss_reg, 0, HI_FALSE);

    set_vc1_cfg(vpss_reg, hal_info, VPSS_MAC_RCHN_CF);

    if (hal_info->in_info.tunnel_addr != 0) {
        vpss_mac_set_cf_rtunl_en(vpss_reg, HI_TRUE);
        vpss_mac_set_cf_rtunl_addr_h(vpss_reg, 0x0);
        vpss_mac_set_cf_rtunl_addr_l(vpss_reg, hal_info->in_info.tunnel_addr);
    } else {
        vpss_mac_set_cf_rtunl_en(vpss_reg, HI_FALSE);
    }

    /* config nr cf & p1 chn */
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_REF, &rchn_cfg[VPSS_MAC_RCHN_NR_REF]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_RFR, &wchn_cfg[VPSS_MAC_WCHN_NR_RFR]);
    /* config stt nr rgme */
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_P1RGMV, &rchn_cfg[VPSS_MAC_RCHN_NR_P1RGMV]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_P2RGMV, &rchn_cfg[VPSS_MAC_RCHN_NR_P2RGMV]);
    /* config stt nr cnt */
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_RCNT, &rchn_cfg[VPSS_MAC_RCHN_NR_RCNT]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_WCNT, &wchn_cfg[VPSS_MAC_WCHN_NR_WCNT]);
    /* config stt nr mad */
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_RMAD, &rchn_cfg[VPSS_MAC_RCHN_NR_RMAD]);
    rchn_cfg[VPSS_MAC_RCHN_SNR_RMAD].tunl_en = HI_TRUE;
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_SNR_RMAD, &rchn_cfg[VPSS_MAC_RCHN_SNR_RMAD]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_WMAD, &wchn_cfg[VPSS_MAC_WCHN_NR_WMAD]);
    /* config stt nr read me blkmv */
    rchn_cfg[VPSS_MAC_RCHN_NR_CFMV].tunl_en = HI_TRUE;
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_CFMV, &rchn_cfg[VPSS_MAC_RCHN_NR_CFMV]);
    /* config stt nr hdow cf & p1 chn */
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_RFRH, &wchn_cfg[VPSS_MAC_WCHN_NR_RFRH]);
    /* config stt nr vdow cf & p1 chn */
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_RFRHV, &wchn_cfg[VPSS_MAC_WCHN_NR_RFRHV]);
    /* config rgme chn */
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_P1RGMV, &rchn_cfg[VPSS_MAC_RCHN_RG_P1RGMV]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_P2RGMV, &rchn_cfg[VPSS_MAC_RCHN_RG_P2RGMV]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_RG_CFRGMV, &wchn_cfg[VPSS_MAC_WCHN_RG_CFRGMV]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_RPRJH, &rchn_cfg[VPSS_MAC_RCHN_RG_RPRJH]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_RG_WPRJH, &wchn_cfg[VPSS_MAC_WCHN_RG_WPRJH]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_RPRJV, &rchn_cfg[VPSS_MAC_RCHN_RG_RPRJV]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_RG_WPRJV, &wchn_cfg[VPSS_MAC_WCHN_RG_WPRJV]);
    /* config die chn */
    vpss_sys_set_dei_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_mcdi_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_ifmd_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_igbm_en(vpss_reg, 0, HI_FALSE);
    rchn_cfg[VPSS_MAC_RCHN_DI_CF].tunl_en = HI_TRUE;
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_CF, &rchn_cfg[VPSS_MAC_RCHN_DI_CF]);

    if (hal_info->vpss_alg_ctrl.dm430_en == HI_TRUE) {
        vpss_sys_setdbm_430_en(vpss_reg, 0, HI_TRUE);
        vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DMCNT, &rchn_cfg[VPSS_MAC_RCHN_DMCNT]);
        vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_DMCNT, &wchn_cfg[VPSS_MAC_WCHN_DMCNT]);
    } else {
        vpss_sys_setdbm_430_en(vpss_reg, 0, HI_FALSE);
    }

    /* config cccl chn */
    if (cccl_support == HI_TRUE) {
        vpss_policy_support_cccl_height_and_width(&cccl_support_w, &cccl_support_h);
        if ((rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth <= cccl_support_w)
            && (rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.hgt <= cccl_support_h)) {
            if ((XDP_PROC_FMT_SP_420 == rchn_cfg[VPSS_MAC_RCHN_CF].data_fmt)
                && VPSS_SUPPORT_CF_UPSAMP) {
                vpss_mac_set_cf_cconvert(vpss_reg, HI_TRUE);
            } else {
                vpss_mac_set_cf_cconvert(vpss_reg, HI_FALSE);
            }

            vpss_sys_set_cccl_en(vpss_reg, 0, HI_FALSE);
        }
    }

    /* config me chn */
    mc_info.en3_drs_version = en3_drs_version;
    mc_info.me_version = convert_me_version(me_version);
    mc_info.me_scan_num = hal_info->me_scan_num;
    mc_info.pro = (rchn_cfg[VPSS_MAC_RCHN_CF].rd_mode == XDP_RMODE_PROGRESSIVE) ? 1 : 0;
    mc_info.meds_en = meds_en;
    mc_info.wbc_cfg = wbc_cfg;
    mc_info.stt_cfg = stt_cfg;
    mc_info.vc1_info = &hal_info->in_info.vc1_info;
    mc_set(vpss_reg, phy_addr, &mc_info, rchn_cfg, wchn_cfg);
    /* global  stt cfg */
    vpss_sys_set_stt_raddr(vpss_reg, 0, 0x0, stt_cfg->global_cfg.r_stt_chn.phy_addr + 0x300);
    vpss_sys_set_stt_waddr(vpss_reg, 0, 0x0, stt_cfg->global_cfg.w_stt_chn.phy_addr);
    /* config output chn */
    set_port_cfg(hal_info, vpss_reg);
    /* must cfg to default para */
    vpss_rchn_set_cf_vid_default_cfg(vpss_reg);
    return HI_SUCCESS;
}

hi_s32 set_uhd_node(vpss_hal_info *hal_info, vpss_reg_type *vpss_reg, hi_u64 phy_addr)
{
    hi_bool meds_en = HI_FALSE;
    hi_u32 safe_mode;
    vpss_mac_rchn_cfg *rchn_cfg;
    vpss_mac_wchn_cfg *wchn_cfg;
    vpss_hal_3drs_type me_version;
    xdp_3_drs_version en3_drs_version;
    vpss_hal_stt_cfg *stt_cfg;
    vpss_hal_wbc_cfg *wbc_cfg;
    vpss_mc_info mc_info;

    stt_cfg = &hal_info->stt_cfg;
    wbc_cfg = &hal_info->wbc_cfg;
    rchn_cfg = &g_rchn_cfg[VPSS_MAC_RCHN_CF];
    wchn_cfg = &g_wchn_cfg[VPSS_MAC_WCHN_OUT0];
    me_version = hal_info->en3_drs;
    en3_drs_version = HAL_VPSS_SEL_MA;

    if (hal_info->use_pq_module == HI_TRUE) {
        init_regtable(vpss_reg, &hal_info->pq_reg_data);
    } else {
        init_regtable(vpss_reg, HI_NULL);
    }

    safe_mode = get_safe_mode(&hal_info->in_info);
    (hi_void)init_all_chn_cfg(vpss_reg);
    get_all_chn_cfg(hal_info, rchn_cfg, wchn_cfg);
#if (defined HI_VPSS_DRV_USE_GOLDEN) || (defined HI_VPSS_DRV_USE_GOLDEN_COEF)
    vpss_hal_refresh_golden_reg_table(vpss_reg, rchn_cfg, wchn_cfg);
#endif
    vpss_mac_set_reg_load(vpss_reg);
    vpss_mac_set_prot(vpss_reg, safe_mode);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_CF, &rchn_cfg[VPSS_MAC_RCHN_CF]);

    if (rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth > HAL_CF_WIDTH) {
        meds_en = HI_TRUE;
    } else {
        meds_en = HI_FALSE;
    }

    vpss_sys_set_four_pixel_en(vpss_reg, 0, HI_FALSE);
    vpss_mac_set_cf_cconvert(vpss_reg, HI_FALSE);
    vpss_sys_set_vpss3drs_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_ma_mac_sel(vpss_reg, 0, en3_drs_version);
    vpss_sys_set_me_version(vpss_reg, 0, me_version);
#ifndef HI_VPSS_DRV_USE_GOLDEN
    if (hal_info->in_frame_type == VPSS_HAL_FRAME_TYPE_FIRST) {
        vpss_sys_set_tnr_en(vpss_reg, 0, HI_FALSE);
        rchn_cfg[VPSS_MAC_RCHN_NR_REF].en = HI_FALSE;
    } else {
        vpss_sys_set_tnr_en(vpss_reg, 0, HI_TRUE);
    }
#else
    vpss_sys_set_tnr_en(vpss_reg, 0, HI_TRUE);
#endif
    vpss_sys_set_snr_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_mcnr_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_rgme_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_meds_en(vpss_reg, 0, meds_en);
    vpss_sys_set_tnr_rec_8bit_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_hfr_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_lbd_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_hcti_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_scd_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_vc1_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_rotate_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_dei_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_mcdi_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_ifmd_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_igbm_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_cccl_en(vpss_reg, 0, HI_FALSE);

    if (hal_info->in_info.tunnel_addr != 0) {
        vpss_mac_set_cf_rtunl_en(vpss_reg, HI_TRUE);
        vpss_mac_set_cf_rtunl_addr_h(vpss_reg, 0x0);
        vpss_mac_set_cf_rtunl_addr_l(vpss_reg, hal_info->in_info.tunnel_addr);
    } else {
        vpss_mac_set_cf_rtunl_en(vpss_reg, HI_FALSE);
    }

    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_REF, &rchn_cfg[VPSS_MAC_RCHN_NR_REF]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_RFR, &wchn_cfg[VPSS_MAC_WCHN_NR_RFR]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_P1RGMV, &rchn_cfg[VPSS_MAC_RCHN_NR_P1RGMV]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_P2RGMV, &rchn_cfg[VPSS_MAC_RCHN_NR_P2RGMV]);
    vpss_sys_set_snr_mad_disable(vpss_reg, 0, HI_FALSE);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_RMAD, &rchn_cfg[VPSS_MAC_RCHN_NR_RMAD]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_WMAD, &wchn_cfg[VPSS_MAC_WCHN_NR_WMAD]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_P1RGMV, &rchn_cfg[VPSS_MAC_RCHN_RG_P1RGMV]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_P2RGMV, &rchn_cfg[VPSS_MAC_RCHN_RG_P2RGMV]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_RG_CFRGMV, &wchn_cfg[VPSS_MAC_WCHN_RG_CFRGMV]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_RPRJH, &rchn_cfg[VPSS_MAC_RCHN_RG_RPRJH]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_RG_WPRJH, &wchn_cfg[VPSS_MAC_WCHN_RG_WPRJH]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_RPRJV, &rchn_cfg[VPSS_MAC_RCHN_RG_RPRJV]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_RG_WPRJV, &wchn_cfg[VPSS_MAC_WCHN_RG_WPRJV]);

    if (hal_info->vpss_alg_ctrl.dm430_en == HI_TRUE) {
        vpss_sys_setdbm_430_en(vpss_reg, 0, HI_TRUE);
        vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DMCNT, &rchn_cfg[VPSS_MAC_RCHN_DMCNT]);
        vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_DMCNT, &wchn_cfg[VPSS_MAC_WCHN_DMCNT]);
    } else {
        vpss_sys_setdbm_430_en(vpss_reg, 0, HI_FALSE);
    }

    vpss_sys_set_stt_raddr(vpss_reg, 0, 0x0, stt_cfg->global_cfg.r_stt_chn.phy_addr + 0x300);
    vpss_sys_set_stt_waddr(vpss_reg, 0, 0x0, stt_cfg->global_cfg.w_stt_chn.phy_addr);

    set_port_cfg(hal_info, vpss_reg);

    mc_info.en3_drs_version = en3_drs_version;
    mc_info.me_version = convert_me_version(me_version);
    mc_info.me_scan_num = hal_info->me_scan_num;
    mc_info.pro = (rchn_cfg[VPSS_MAC_RCHN_CF].rd_mode == XDP_RMODE_PROGRESSIVE) ? 1 : 0;
    mc_info.meds_en = meds_en;
    mc_info.wbc_cfg = wbc_cfg;
    mc_info.stt_cfg = stt_cfg;
    mc_info.vc1_info = &hal_info->in_info.vc1_info;
    mc_set(vpss_reg, phy_addr, &mc_info, rchn_cfg, wchn_cfg);
    mc_set_scan_addr(vpss_reg, phy_addr);
    /* must cfg to default para */
    vpss_rchn_set_cf_vid_default_cfg(vpss_reg);

    return HI_SUCCESS;
}

hi_s32 set_8k_node(vpss_hal_info *hal_info, vpss_reg_type *vpss_reg, hi_u64 phy_addr)
{
    hi_u32 safe_mode;
    vpss_mac_rchn_cfg *rchn_cfg = &g_rchn_cfg[VPSS_MAC_RCHN_CF];
    vpss_mac_wchn_cfg *wchn_cfg = &g_wchn_cfg[VPSS_MAC_WCHN_OUT0];

    if (hal_info->use_pq_module == HI_TRUE) {
        init_regtable(vpss_reg, &hal_info->pq_reg_data);
    } else {
        init_regtable(vpss_reg, HI_NULL);
    }

    safe_mode = get_safe_mode(&hal_info->in_info);
    init_cf_out0_chn_cfg(vpss_reg);
    get_cf_chn_cfg(hal_info, rchn_cfg, wchn_cfg);
    vpss_mac_set_reg_load(vpss_reg);
    vpss_mac_set_prot(vpss_reg, safe_mode);
    vpss_mac_set_rchn_cf_cfg(vpss_reg, VPSS_MAC_RCHN_CF, &rchn_cfg[VPSS_MAC_RCHN_CF]);
    /* 4ppc on */
    vpss_sys_set_four_pixel_en(vpss_reg, 0, HI_TRUE);

    set_all_pq_disable(vpss_reg);

    vpss_mac_set_cf_rtunl_en(vpss_reg, HI_FALSE);
    vpss_sys_set_snr_mad_disable(vpss_reg, 0, HI_FALSE);
    set_port_cfg(hal_info, vpss_reg);
    /* must cfg to default para */
    vpss_rchn_set_cf_vid_default_cfg(vpss_reg);
    return HI_SUCCESS;
}

hi_s32 set_frame_only_manr_node(vpss_hal_info *hal_info, vpss_reg_type *vpss_reg, hi_u64 phy_addr)
{
    hi_bool meds_en = HI_FALSE;
    hi_u32 safe_mode;
    vpss_mac_rchn_cfg *rchn_cfg;
    vpss_mac_wchn_cfg *wchn_cfg;
    vpss_mc_info mc_info;
    vpss_hal_3drs_type me_version;
    xdp_3_drs_version en3_drs_version;
    vpss_hal_stt_cfg *stt_cfg;
    vpss_hal_wbc_cfg *wbc_cfg;
    stt_cfg = &hal_info->stt_cfg;
    wbc_cfg = &hal_info->wbc_cfg;
    rchn_cfg = &g_rchn_cfg[VPSS_MAC_RCHN_CF];
    wchn_cfg = &g_wchn_cfg[VPSS_MAC_WCHN_OUT0];
    me_version = hal_info->en3_drs;
    en3_drs_version = HAL_VPSS_SEL_MA;

    if (hal_info->use_pq_module == HI_TRUE) {
        init_regtable(vpss_reg, &hal_info->pq_reg_data);
    } else {
        init_regtable(vpss_reg, HI_NULL);
    }

    safe_mode = get_safe_mode(&hal_info->in_info);
    init_all_chn_cfg(vpss_reg);
    get_all_chn_cfg(hal_info, rchn_cfg, wchn_cfg);
#if (defined HI_VPSS_DRV_USE_GOLDEN) || (defined HI_VPSS_DRV_USE_GOLDEN_COEF)
    vpss_hal_refresh_golden_reg_table(vpss_reg, rchn_cfg, wchn_cfg);
#endif
    vpss_mac_set_reg_load(vpss_reg);
    vpss_mac_set_prot(vpss_reg, safe_mode);
    /* config input chn */
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_CF, &rchn_cfg[VPSS_MAC_RCHN_CF]);

    if (rchn_cfg[VPSS_MAC_RCHN_CF].in_rect.wth > HAL_CF_WIDTH) {
        meds_en = HI_TRUE;
    } else {
        meds_en = HI_FALSE;
    }

    /* -----------------config 3drs---------------------- */
    vpss_sys_set_vpss3drs_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_ma_mac_sel(vpss_reg, 0, en3_drs_version);
    vpss_sys_set_me_version(vpss_reg, 0, me_version);
    vpss_sys_set_tnr_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_snr_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_mcnr_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_rgme_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_meds_en(vpss_reg, 0, meds_en);
    vpss_sys_set_hfr_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_lbd_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_scd_en(vpss_reg, 0, HI_TRUE);
    vpss_sys_set_vc1_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_rotate_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_dei_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_mcdi_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_ifmd_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_igbm_en(vpss_reg, 0, HI_FALSE);
    set_vc1_cfg(vpss_reg, hal_info, VPSS_MAC_RCHN_CF);
    /* config nr cf & p1 chn */
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_REF, &rchn_cfg[VPSS_MAC_RCHN_NR_REF]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_RFR, &wchn_cfg[VPSS_MAC_WCHN_NR_RFR]);
    /* config stt nr rgme */
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_P1RGMV, &rchn_cfg[VPSS_MAC_RCHN_NR_P1RGMV]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_P2RGMV, &rchn_cfg[VPSS_MAC_RCHN_NR_P2RGMV]);
    /* config stt nr cnt */
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_RCNT, &rchn_cfg[VPSS_MAC_RCHN_NR_RCNT]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_WCNT, &wchn_cfg[VPSS_MAC_WCHN_NR_WCNT]);
    /* config stt nr mad */
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_RMAD, &rchn_cfg[VPSS_MAC_RCHN_NR_RMAD]);
    rchn_cfg[VPSS_MAC_RCHN_SNR_RMAD].tunl_en = HI_FALSE;
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_SNR_RMAD, &rchn_cfg[VPSS_MAC_RCHN_SNR_RMAD]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_WMAD, &wchn_cfg[VPSS_MAC_WCHN_NR_WMAD]);
    /* config stt nr read me blkmv */
    rchn_cfg[VPSS_MAC_RCHN_NR_CFMV].tunl_en = HI_FALSE;
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_NR_CFMV, &rchn_cfg[VPSS_MAC_RCHN_NR_CFMV]);
    /* config stt nr hdow cf & p1 chn */
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_RFRH, &wchn_cfg[VPSS_MAC_WCHN_NR_RFRH]);
    /* config stt nr vdow cf & p1 chn */
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_NR_RFRHV, &wchn_cfg[VPSS_MAC_WCHN_NR_RFRHV]);
    /* config rgme chn */
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_P1RGMV, &rchn_cfg[VPSS_MAC_RCHN_RG_P1RGMV]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_P2RGMV, &rchn_cfg[VPSS_MAC_RCHN_RG_P2RGMV]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_RG_CFRGMV, &wchn_cfg[VPSS_MAC_WCHN_RG_CFRGMV]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_RPRJH, &rchn_cfg[VPSS_MAC_RCHN_RG_RPRJH]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_RG_WPRJH, &wchn_cfg[VPSS_MAC_WCHN_RG_WPRJH]);
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_RG_RPRJV, &rchn_cfg[VPSS_MAC_RCHN_RG_RPRJV]);
    vpss_mac_set_wchn_cfg(vpss_reg, VPSS_MAC_WCHN_RG_WPRJV, &wchn_cfg[VPSS_MAC_WCHN_RG_WPRJV]);
    /* config die chn */
    vpss_sys_set_dei_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_mcdi_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_ifmd_en(vpss_reg, 0, HI_FALSE);
    vpss_sys_set_igbm_en(vpss_reg, 0, HI_FALSE);
    rchn_cfg[VPSS_MAC_RCHN_DI_CF].tunl_en = HI_FALSE;
    vpss_mac_set_rchn_cfg(vpss_reg, VPSS_MAC_RCHN_DI_CF, &rchn_cfg[VPSS_MAC_RCHN_DI_CF]);
    /* config cccl chn */
    /* config me chn */
    mc_info.en3_drs_version = en3_drs_version;
    mc_info.me_version = convert_me_version(me_version);
    mc_info.me_scan_num = hal_info->me_scan_num;
    mc_info.pro = (rchn_cfg[VPSS_MAC_RCHN_CF].rd_mode == XDP_RMODE_PROGRESSIVE) ? 1 : 0;
    mc_info.meds_en = meds_en;
    mc_info.wbc_cfg = wbc_cfg;
    mc_info.stt_cfg = stt_cfg;
    mc_info.vc1_info = &hal_info->in_info.vc1_info;
    mc_set(vpss_reg, phy_addr, &mc_info, rchn_cfg, wchn_cfg);
    /* global  stt cfg */
    vpss_sys_set_stt_raddr(vpss_reg, 0, 0x0, stt_cfg->global_cfg.r_stt_chn.phy_addr + 0x300);
    vpss_sys_set_stt_waddr(vpss_reg, 0, 0x0, stt_cfg->global_cfg.w_stt_chn.phy_addr);
    /* config output chn */
    set_port_cfg(hal_info, vpss_reg);
    /* must cfg to default para */
    vpss_rchn_set_cf_vid_default_cfg(vpss_reg);
    return HI_SUCCESS;
}

hi_s32 set_prog_frame_node(vpss_hal_info *hal_info, vpss_reg_type *vpss_reg, hi_u64 phy_addr)
{
    return set_frame_node(hal_info, vpss_reg, phy_addr);
}

hi_s32 set_prog_no_me_frame_node(vpss_hal_info *hal_info, vpss_reg_type *vpss_reg, hi_u64 phy_addr)
{
    return set_frame_no_me_node(hal_info, vpss_reg, phy_addr);
}

hi_s32 set_prog_uhd_frame_node(vpss_hal_info *hal_info, vpss_reg_type *vpss_reg, hi_u64 phy_addr)
{
    return set_uhd_node(hal_info, vpss_reg, phy_addr);
}

hi_s32 set_prog_8k_node(vpss_hal_info *hal_info, vpss_reg_type *vpss_reg, hi_u64 phy_addr)
{
    return set_8k_node(hal_info, vpss_reg, phy_addr);
}

hi_s32 set_prog_manr_only_dbg_frame_node(vpss_hal_info *hal_info, vpss_reg_type *vpss_reg, hi_u64 phy_addr)
{
    return set_frame_only_manr_node(hal_info, vpss_reg, phy_addr);
}

hi_s32 vpss_hal_set_3dframe_node(vpss_hal_info *hal_info, vpss_reg_type *vpss_reg, hi_u64 phy_addr)
{
    return set_prog_frame_node(hal_info, vpss_reg, phy_addr);
}

hi_s32 add_reg_buffer_num(vpss_ip ip, hi_u32 buf_num)
{
    hi_u32 i = 0;
    hi_s32 ret;
    vpss_hal_buff *hal_node_buf = HI_NULL;
    vpss_hal_ctx *hal_ctx = HI_NULL;
    drv_vpss_mem_attr mem_attr;
    hal_ctx = &g_hal_ctx[ip];

    if (buf_num > HAL_TASK_NODE_MAX) {
        vpss_error("can't support Alloc VPSS_RegBuf num %d\n", buf_num);
    }

    for (i = 0; i < buf_num; i++) {
        hal_node_buf = vpss_vmalloc(sizeof(vpss_hal_buff));
        if (hal_node_buf == HI_NULL) {
            vpss_error("VPSS VMALLOC FAILED size %d\n", sizeof(vpss_hal_buff));
            return HI_FAILURE;
        }

        memset(hal_node_buf, 0, sizeof(vpss_hal_buff));
        mem_attr.name = "vpss_regbuf";
        mem_attr.size = VPSS_REG_SIZE;
        mem_attr.mode = OSAL_MMZ_TYPE;
        mem_attr.is_map_viraddr = HI_TRUE; /* must map */
        mem_attr.is_cache = HI_FALSE;
        ret = vpss_comm_mem_alloc(&mem_attr, &hal_node_buf->reg_buf);
        if (ret != HI_SUCCESS) {
            vpss_vfree(hal_node_buf);
            vpss_error("Alloc VPSS_RegBuf Failed\n");
            return HI_FAILURE;
        }

        osal_list_add_tail(&hal_node_buf->node, &hal_ctx->buffer_list);
        hal_node_buf->used = HI_FALSE;
        hal_node_buf->phy_addr = hal_node_buf->reg_buf.phy_addr;
        hal_node_buf->vir_addr = (hi_u64)hal_node_buf->reg_buf.vir_addr;
    }

    return HI_SUCCESS;
}

hi_void free_all_reg_buffer(vpss_ip ip)
{
    list *pos = HI_NULL;
    list *n = HI_NULL;
    vpss_hal_buff *hal_node_buf = HI_NULL;
    vpss_hal_ctx *hal_ctx = HI_NULL;

    hal_ctx = &g_hal_ctx[ip];
    osal_list_for_each_safe(pos, n, &hal_ctx->buffer_list) {
        hal_node_buf = (vpss_hal_buff *)osal_list_entry(pos, vpss_hal_buff, node);
        osal_list_del(pos);

        if (hal_node_buf->reg_buf.phy_addr != 0) {
            vpss_comm_mem_free(&hal_node_buf->reg_buf);
        }

        memset(hal_node_buf, 0, sizeof(vpss_hal_buff));
        vpss_vfree(hal_node_buf);
    }

    osal_list_for_each_safe(pos, n, &hal_ctx->buffer_loading_list) {
        hal_node_buf = (vpss_hal_buff *)osal_list_entry(pos, vpss_hal_buff, node);
        osal_list_del(pos);

        if (hal_node_buf->reg_buf.phy_addr != 0) {
            vpss_comm_mem_free(&hal_node_buf->reg_buf);
        }

        memset(hal_node_buf, 0, sizeof(vpss_hal_buff));
        vpss_vfree(hal_node_buf);
    }
}

vpss_hal_buff *get_reg_node_buffer(vpss_ip ip)
{
    hi_s32 ret;
    list *pos = HI_NULL;
    list *n = HI_NULL;
    vpss_hal_buff *hal_node_buf = HI_NULL;
    vpss_hal_ctx *hal_ctx = HI_NULL;
    hal_ctx = &g_hal_ctx[ip];
    osal_list_for_each_safe(pos, n, &hal_ctx->buffer_list) {
        hal_node_buf = (vpss_hal_buff *)osal_list_entry(pos, vpss_hal_buff, node);
        if (hal_node_buf->used != HI_TRUE) {
            return hal_node_buf;
        }
    }

    ret = add_reg_buffer_num(ip, HAL_VPSS_MALLOC_NODEBUFF_ONCE_NUM);
    if (ret != HI_SUCCESS) {
        vpss_error("VPSS add reg node buffer failed\n");
        return HI_NULL;
    }

    osal_list_for_each_safe(pos, n, &hal_ctx->buffer_list) {
        hal_node_buf = (vpss_hal_buff *)osal_list_entry(pos, vpss_hal_buff, node);
        if (hal_node_buf->used != HI_TRUE) {
            return hal_node_buf;
        }
    }
    return HI_NULL;
}

hi_void move_loading_buffer(vpss_ip ip)
{
    list *pos = HI_NULL;
    list *n = HI_NULL;
    vpss_hal_buff *hal_node_buf = HI_NULL;
    vpss_hal_ctx *hal_ctx = HI_NULL;
    hal_ctx = &g_hal_ctx[ip];
    osal_list_for_each_safe(pos, n, &hal_ctx->buffer_loading_list) {
        hal_node_buf = (vpss_hal_buff *)osal_list_entry(pos, vpss_hal_buff, node);
        hal_node_buf->used = HI_FALSE;
        osal_list_del_init(&hal_node_buf->node);
        osal_list_add_tail(&hal_node_buf->node, &hal_ctx->buffer_list);
    }
}

hi_s32 vpss_hal_dump_reg(vpss_ip ip)
{
    list *pos = HI_NULL;
    list *n = HI_NULL;
    hi_u32 i, j;
    vpss_hal_buff *hal_node_buf = HI_NULL;
    vpss_hal_ctx *hal_ctx = HI_NULL;
    hi_u32 *reg;
    vpss_hal_check_ip_vaild(ip);
    hal_ctx = &g_hal_ctx[ip];
    osal_list_for_each_safe(pos, n, &hal_ctx->buffer_loading_list) {
        hal_node_buf = (vpss_hal_buff *)osal_list_entry(pos, vpss_hal_buff, node);
        reg = (hi_u32 *)(hal_node_buf->vir_addr);

        for (i = 0; i < 64; i += 4) { /* 64:reg bit; 4:32bit */
            if (i % (64) == 0) { /* 64:reg bit; */
                osal_printk("BASE %x\n", i * 4); /* 4:32bit */
            }

            osal_printk("%x0:%.8x %.8x %.8x %.8x\n", ((i % 64) / 4), /* 64:reg bit; 4:32bit */
                        *(reg + i), *(reg + i + 1), *(reg + i + 2), *(reg + i + 3)); /* 2: 2 reg ; 3: 3 reg */
        }

        j = 5120; /* 5120:reg num */

        for (i = j; i < (j + 8); i += 4) { /* 8:reg bit; 4:32bit */
            if (i % (64) == 0) { /* 64:reg bit; */
                osal_printk("BASE %x\n", i * 4); /* 4:32bit */
            }

            osal_printk("%x0:%.8x %.8x %.8x %.8x\n", ((i % 64) / 4), /* 64:reg bit; 4:32bit */
                        *(reg + i), *(reg + i + 1), *(reg + i + 2), *(reg + i + 3)); /* 2: 2 reg ; 3: 3 reg */
        }

        j = 5632; /* 5632:reg num */

        for (i = j; i < (j + (64 * 13)); i += 4) { /* 64:reg bit;  13:32 reg; 4:32bit */
            if (i % (64) == 0) { /* 64:reg bit; */
                osal_printk("BASE %x\n", i * 4); /* 4:32bit */
            }

            osal_printk("%x0:%.8x %.8x %.8x %.8x\n", ((i % 64) / 4), /* 64:reg bit; 4:32bit */
                        *(reg + i), *(reg + i + 1), *(reg + i + 2), *(reg + i + 3)); /* 2: 2 reg ; 3: 3 reg */
        }

        j = 6464; /* 6464:reg num */

        for (i = j; i < (j + 4); i += 4) { /* 4:32bit */
            if (i % (64) == 0) { /* 64:reg bit; */
                osal_printk("BASE %x\n", i * 4); /* 4:32bit */
            }

            osal_printk("%x0:%.8x %.8x %.8x %.8x\n", ((i % 64) / 4), /* 64:reg bit; 4:32bit */
                        *(reg + i), *(reg + i + 1), *(reg + i + 2), *(reg + i + 3)); /* 2: 2 reg ; 3: 3 reg */
        }

        osal_printk("\n\n");
    }
    return HI_SUCCESS;
}

hi_void vpss_hal_init_hal_info(vpss_hal_info *hal_info)
{
    hal_info->use_pq_module = HI_FALSE;
    hal_info->in_frame_type = VPSS_HAL_FRAME_TYPE_FIRST;
    hal_info->node_type = VPSS_HAL_NODE_2D_FRAME;
    hal_info->tunl_en = HI_FALSE;
    hal_info->in_rtunl_en = HI_FALSE;
    hal_info->in_wtunl_en = HI_FALSE;
    hal_info->out_rtunl_en = HI_FALSE;
    hal_info->out_wtunl_en = HI_FALSE;
    hal_info->inst_id = 0;

    memset(&hal_info->smmu_cfg, 0, sizeof(vpss_smmu_cfg));
    memset(&hal_info->in_info, 0, sizeof(vpss_hal_frame));
    memset(&hal_info->stt_cfg, 0, sizeof(vpss_hal_stt_cfg));
    memset(&hal_info->wbc_cfg, 0, sizeof(vpss_hal_wbc_cfg));
    memset(&hal_info->port_info[0], 0, sizeof(vpss_hal_port_info) * VPSS_PORT_MAX_NUM);
    memset(&hal_info->cf_pattern, 0, sizeof(vpss_dbg_testptn));
    memset(&hal_info->out_pattern, 0, sizeof(vpss_dbg_testptn));

    memset(&hal_info->port_used[0], 0, sizeof(hi_bool) * VPSS_REG_PORT_MAX);
    memset(&hal_info->vpss_alg_ctrl, 0, sizeof(vpss_perfor_alg_ctrl));
}

hi_s32 vpss_hal_init(vpss_ip ip)
{
    hi_s32 ret;
    vpss_hal_ctx *hal_ctx = HI_NULL;

    vpss_hal_check_ip_vaild(ip);
    hal_ctx = &g_hal_ctx[ip];

    if (hal_ctx->init == HI_TRUE) {
        vpss_error("VPSS IP%d, Already Init\n", ip);
        return HI_SUCCESS;
    }

    OSAL_INIT_LIST_HEAD(&hal_ctx->buffer_list);
    OSAL_INIT_LIST_HEAD(&hal_ctx->buffer_loading_list);
    ret = add_reg_buffer_num(ip, HAL_VPSS_MALLOC_NODEBUFF_INIT_NUM);
    if (ret != HI_SUCCESS) {
        vpss_error("Alloc VPSS_RegBuf Failed\n");
        free_all_reg_buffer(ip);
        return HI_FAILURE;
    }

    /* 映射寄存器地址 */
    hal_ctx->base_reg_vir = (hi_u64)osal_ioremap_nocache(hal_ctx->base_reg_phy, VPSS_REG_SIZE);
    if (hal_ctx->base_reg_vir == 0) {
        vpss_error("ioremap VPSS_REG(%#x) Failed\n", hal_ctx->base_reg_phy);
        free_all_reg_buffer(ip);
        return HI_FAILURE;
    }

    hal_ctx->init = HI_TRUE;
    ret = vpss_hal_sys_init(ip, HI_TRUE);
    if (ret != HI_SUCCESS) {
        vpss_error("set clock failed\n");
        osal_iounmap((hi_void *)hal_ctx->base_reg_vir);
        hal_ctx->base_reg_vir = 0;
        free_all_reg_buffer(ip);
        hal_ctx->init = HI_FALSE;
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 vpss_hal_deinit(vpss_ip ip)
{
    hi_s32 ret;
    vpss_hal_ctx *hal_ctx;
    vpss_hal_check_ip_vaild(ip);
    hal_ctx = &g_hal_ctx[ip];

    if (hal_ctx->init != HI_TRUE) {
        vpss_info("VPSS IP%d, Already DeInit\n", ip);
        return HI_SUCCESS;
    }

    free_all_reg_buffer(ip);
    ret = vpss_hal_sys_init(ip, HI_FALSE);
    if (ret != HI_SUCCESS) {
        vpss_error(" set clock failed\n");
    }

    if (hal_ctx->base_reg_vir != 0) {
        osal_iounmap((hi_void *)hal_ctx->base_reg_vir);
        hal_ctx->base_reg_vir = 0;
    } else {
        vpss_error("pstHalCtx->u32BaseRegVir is zero\n");
    }

    hal_ctx->init = HI_FALSE;
    return HI_SUCCESS;
}

hi_s32 vpss_hal_sys_init(vpss_ip ip, hi_bool init_en)
{
    vpss_reg_type *vpss_reg = HI_NULL;
    vpss_hal_ctx *hal_ctx = HI_NULL;

    vpss_hal_check_ip_vaild(ip);
    hal_ctx = &g_hal_ctx[ip];
    vpss_hal_check_init(hal_ctx->init);
    vpss_reg = (vpss_reg_type *)hal_ctx->base_reg_vir;

    if (hal_ctx->init_en == init_en) {
        return HI_SUCCESS;
    }

    if (init_en == HI_TRUE) {
        vpss_crg_set_clock_en(ip, HI_TRUE);
        vpss_hal_smmu_init(ip);
        vpss_sys_set_int_mask(vpss_reg, 0x0, VPSS_INT_MASK);
        vpss_sys_set_ck_gt_en(vpss_reg, 0x0, HI_TRUE);
        vpss_sys_set_node_rst_en(vpss_reg, 0x0, HI_FALSE);
        vpss_sys_set_scan_rst_en(vpss_reg, 0x0, HI_TRUE);
        vpss_sys_set_rupd_en(vpss_reg, 0x0, HI_TRUE);
        vpss_master_set_mstr0_routstanding(vpss_reg, VPSS_ROUTSTANDING);
        vpss_master_set_mstr0_woutstanding(vpss_reg, VPSS_WOUTSTANDING);

        vpss_sys_set_allip_ck_gt_en(vpss_reg, 0, HI_TRUE);
    } else {
        vpss_hal_smmu_deinit(ip);
        vpss_crg_set_clock_en(ip, HI_FALSE);
    }

#ifdef HI_TEE_SUPPORT
    {
        hi_s32 ret;
        ret = hi_drv_ssm_iommu_config(LOGIC_MOD_ID_VPSS);
        if (ret != HI_SUCCESS) {
            vpss_error("init tee config failed! %d \n", ret);
            hal_ctx->init_en = HI_FALSE;
            return HI_FAILURE;
        }
    }
#endif

    hal_ctx->init_en = init_en;

    return HI_SUCCESS;
}

hi_s32 vpss_hal_start_logic(vpss_ip ip)
{
    list *pos = HI_NULL;
    list *n = HI_NULL;
    vpss_hal_buff *hal_node_buf = HI_NULL;
    vpss_hal_buff *hal_last_node_buf = HI_NULL;
    hi_u64 phy_addr = 0;
    vpss_reg_type *vpss_last_reg = HI_NULL;
    vpss_reg_type *vpss_base_reg = HI_NULL;
    vpss_hal_ctx *hal_ctx = HI_NULL;

    vpss_hal_check_ip_vaild(ip);
    hal_ctx = &g_hal_ctx[ip];
    vpss_hal_check_init(hal_ctx->init);
    vpss_base_reg = (vpss_reg_type *)hal_ctx->base_reg_vir;

    osal_list_for_each_safe(pos, n, &hal_ctx->buffer_list) {
        hal_node_buf = (vpss_hal_buff *)osal_list_entry(pos, vpss_hal_buff, node);
        if (hal_node_buf->used == HI_TRUE) {
            phy_addr = hal_node_buf->phy_addr + 0x5000;

            if (hal_last_node_buf == HI_NULL) {
                vpss_sys_set_pnext(vpss_base_reg, 0, (phy_addr >> 20) & 0xfffff, (phy_addr & 0xfffff)); /* 20 reg bit */
            } else {
                vpss_last_reg = (vpss_reg_type *)hal_last_node_buf->vir_addr;
                vpss_sys_set_pnext(vpss_last_reg, 0, (phy_addr >> 20) & 0xfffff, (phy_addr & 0xfffff)); /* 20 reg bit */
            }

            hal_last_node_buf = hal_node_buf;
            osal_list_del_init(&hal_node_buf->node);
            osal_list_add_tail(&hal_node_buf->node, &hal_ctx->buffer_loading_list);
        }
    }

    if (hal_last_node_buf == HI_NULL) {
        vpss_error("No Node Needs Start\n");
        return HI_FAILURE;
    }

    vpss_last_reg = (vpss_reg_type *)hal_last_node_buf->vir_addr;
    vpss_sys_set_pnext(vpss_last_reg, 0, 0x0, 0x0);
    vpss_sys_set_vpss_start(vpss_base_reg, 0, 1);
    return HI_SUCCESS;
}
hi_s32 vpss_hal_complete_logic(vpss_ip ip)
{
    vpss_hal_check_ip_vaild(ip);
    move_loading_buffer(ip);
    return HI_SUCCESS;
}

hi_s32 vpss_hal_smmu_init(vpss_ip ip)
{
    hi_bool smmu_bypass = HI_FALSE;
    vpss_hal_ctx *hal_ctx;
    hi_ulong page_addr;
    hi_ulong err_read_addr;
    hi_ulong err_write_addr;
    vpss_reg_type *vpss_reg;
    smmu_bypass = HI_FALSE;
    vpss_hal_check_ip_vaild(ip);
    hal_ctx = &g_hal_ctx[ip];
    vpss_hal_check_init(hal_ctx->init);
    vpss_reg = (vpss_reg_type *)hal_ctx->base_reg_vir;
    vpss_mmu_set_glb_bypass(vpss_reg, 0, smmu_bypass);
    vpss_mmu_set_auto_clk_gt_en(vpss_reg, 0, HI_TRUE); /* todo : false tmp;yangpiang true */
    vpss_mmu_set_int_en(vpss_reg, 0, HI_TRUE); /* todo : double check when yangpian */
    vpss_mmu_set_ptw_pf(vpss_reg, 0, 3); /* todo : ori value 3 mmu value */
    osal_mem_get_nssmmu_pgtinfo(&page_addr, &err_read_addr, &err_write_addr);  /* no secure page addr */
    vpss_mmu_set_cb_ttbr (vpss_reg, 0, (hi_u32)(page_addr & 0xffffffff));
    vpss_mmu_set_cb_ttbr_h (vpss_reg, 0, (hi_u32)((page_addr >> 32) & 0xffffffff)); /* 32 32bit offset */
    vpss_mmu_set_err_ns_rd_addr (vpss_reg, 0, (hi_u32)(err_read_addr & 0xffffffff));
    vpss_mmu_set_err_ns_rd_addr_h (vpss_reg, 0, (hi_u32)((err_read_addr >> 32) & 0xffffffff)); /* 32 32bit offset */
    vpss_mmu_set_err_ns_wr_addr (vpss_reg, 0, (hi_u32)(err_write_addr & 0xffffffff));
    vpss_mmu_set_err_ns_wr_addr_h (vpss_reg, 0, (hi_u32)((err_write_addr >> 32) & 0xffffffff)); /* 32 32bit offset */
    vpss_mmu_set_intns_tlbinvalid_rd_msk(vpss_reg, 0, HI_FALSE);
    vpss_mmu_set_intns_tlbinvalid_wr_msk(vpss_reg, 0, HI_FALSE);
    vpss_mmu_set_intns_ptw_trans_msk(vpss_reg, 0, HI_FALSE);
    vpss_mmu_set_intns_tlbmiss_msk(vpss_reg, 0, HI_FALSE);
    return HI_SUCCESS;
}

hi_s32 vpss_hal_smmu_deinit(vpss_ip ip)
{
    vpss_reg_type *vpss_reg = HI_NULL;
    vpss_hal_ctx *hal_ctx = HI_NULL;

    vpss_hal_check_ip_vaild(ip);

    hal_ctx = &g_hal_ctx[ip];
    vpss_hal_check_init(hal_ctx->init);

    vpss_reg = (vpss_reg_type *)hal_ctx->base_reg_vir;
    vpss_mmu_set_int_en(vpss_reg, 0, HI_FALSE);

    return HI_SUCCESS;
}

hi_s32 vpss_hal_check_int_state(vpss_ip ip)
{
    hi_u32 int_state = 0;
    hi_s32 ret;
    vpss_hal_check_ip_vaild(ip);
    ret = vpss_hal_get_int_state(ip, &int_state);
    if (ret != HI_SUCCESS) {
        vpss_error("get int state failed\n");
    }
    vpss_dbg("VPSS int state = %x \n", int_state);

    if (int_state == 0) {
        vpss_error("VPSS int state = %x \n", int_state);
    }

    if (int_state & 0x8) {
        vpss_error("vpss scan_timeout!!!!  state = %x \n", int_state);
    }

    if (int_state & 0x10) {
        vpss_error("vpss node_timeout!!!!  state = %x \n", int_state);
    }

    if (int_state & 0x20) {
        vpss_error("vpss bus0 write error!!!!  state = %x \n", int_state);
    }

    if (int_state & 0x40) {
        vpss_error("vpss bus0 read error!!!!  state = %x \n", int_state);
    }

    if (int_state & 0x80) {
        vpss_error("vpss bus1 write error!!!!  state = %x \n", int_state);
    }

    if (int_state & 0x100) {
        vpss_error("vpss bus1 read error!!!!  state = %x \n", int_state);
    }

    if (int_state & 0x400) {
        vpss_error("vpss dcmp error!!!!  state = %x \n", int_state);
    }

    if (int_state & 0x4) {
        ret = HI_SUCCESS;
    }

    ret = vpss_hal_clear_int_state(VPSS_IP_0, int_state);
    if (ret != HI_SUCCESS) {
        vpss_error("clear int state failed\n");
    }
    return ret;
}

hi_s32 vpss_hal_get_int_state(vpss_ip ip, hi_u32 *int_state)
{
    vpss_reg_type *vpss_reg;
    vpss_hal_ctx *hal_ctx;
    vpss_hal_check_ip_vaild(ip);
    vpss_hal_check_null_ptr(int_state);
    hal_ctx = &g_hal_ctx[ip];
    vpss_hal_check_init(hal_ctx->init);
    vpss_reg = (vpss_reg_type *)hal_ctx->base_reg_vir;
    *int_state = vpss_sys_get_int_state(vpss_reg, 0);
    return HI_SUCCESS;
}

hi_s32 vpss_hal_clear_int_state(vpss_ip ip, hi_u32 int_state)
{
    vpss_reg_type *vpss_reg;
    vpss_hal_ctx *hal_ctx;
    vpss_hal_check_ip_vaild(ip);
    hal_ctx = &g_hal_ctx[ip];
    vpss_hal_check_init(hal_ctx->init);
    vpss_reg = (vpss_reg_type *)hal_ctx->base_reg_vir;
    vpss_sys_set_int_clr(vpss_reg, 0, int_state);
    return HI_SUCCESS;
}

hi_s32 vpss_hal_get_smmu_int_state(vpss_ip ip, hi_u32 *int_state)
{
    hi_s32 ret;
    hi_u32 safe_mode;
    vpss_reg_type *vpss_reg;
    vpss_hal_ctx *hal_ctx;

    vpss_hal_check_ip_vaild(ip);

    hal_ctx = &g_hal_ctx[ip];

    vpss_hal_check_init(hal_ctx->init);

    vpss_reg = (vpss_reg_type *)hal_ctx->base_reg_vir;
    vpss_mac_get_prot(vpss_reg, &safe_mode);

    if (safe_mode == HAL_VPSS_NOSAFE_MODE) {
        *int_state = vpss_mmu_get_intns_stat(vpss_reg, 0);
    } else {
        *int_state = vpss_mmu_get_ints_stat(vpss_reg, 0);
    }

    ret = process_smmu_state(ip, *int_state);
    if (ret != HI_SUCCESS) {
        vpss_error("process smmu state failed\n");
    }

    return HI_SUCCESS;
}

hi_s32 vpss_hal_clear_smmu_int_state(vpss_ip ip, hi_u32 int_state)
{
    hi_u32 safe_mode;
    vpss_reg_type *vpss_reg;
    vpss_hal_ctx *hal_ctx;

    vpss_hal_check_ip_vaild(ip);

    hal_ctx = &g_hal_ctx[ip];

    vpss_hal_check_init(hal_ctx->init);

    vpss_reg = (vpss_reg_type *)hal_ctx->base_reg_vir;
    vpss_mac_get_prot(vpss_reg, &safe_mode);

    if (safe_mode == HAL_VPSS_NOSAFE_MODE) {
        vpss_mmu_set_intns_clr(vpss_reg, 0, int_state);
    } else {
        vpss_mmu_set_ints_clr(vpss_reg, 0, int_state);
    }

    return HI_SUCCESS;
}

static hi_s32 vpss_hal_set_node_info_byswitch(vpss_hal_info *hal_info, vpss_hal_buff *hal_node_buf)
{
    hi_s32 ret;
    hi_u64 phy_addr = hal_node_buf->phy_addr;
    hi_u64 vir_addr = hal_node_buf->vir_addr;

    switch (hal_info->node_type) {
        case VPSS_HAL_NODE_2D_FRAME:
            ret = set_prog_frame_node(hal_info, (vpss_reg_type *)vir_addr, phy_addr);
            break;
        case VPSS_HAL_NODE_2D_4FIELD:
            ret = set_4field_node(hal_info, (vpss_reg_type *)vir_addr, phy_addr);
            break;
        case VPSS_HAL_NODE_3D_FRAME_R:
            ret = vpss_hal_set_3dframe_node(hal_info, (vpss_reg_type *)vir_addr, phy_addr);
            break;
        case VPSS_HAL_NODE_UHD:
            ret = set_prog_uhd_frame_node(hal_info, (vpss_reg_type *)vir_addr, phy_addr);
            break;
        case VPSS_HAL_NODE_ROTATION_Y:
            ret = set_rotate_node(hal_info, (vpss_reg_type *)vir_addr, phy_addr, HI_TRUE, hal_info->port_id);
            break;
        case VPSS_HAL_NODE_ROTATION_C:
            ret = set_rotate_node(hal_info, (vpss_reg_type *)vir_addr, phy_addr, HI_FALSE, hal_info->port_id);
            break;
        case VPSS_HAL_NODE_8K:
            ret = set_prog_8k_node(hal_info, (vpss_reg_type *)vir_addr, phy_addr);
            break;
        case VPSS_HAL_NODE_ONLY_MANR_DBG:
            ret = set_prog_manr_only_dbg_frame_node(hal_info, (vpss_reg_type *)vir_addr, phy_addr);
            break;
        default:
            vpss_error("No this Node Type:%d!\n", hal_info->node_type);
            return HI_FAILURE;
    }

    if (ret != HI_SUCCESS) {
        vpss_error("set failed node_type:%d!\n", hal_info->node_type);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vpss_hal_set_node_info(vpss_ip ip, vpss_hal_info *hal_info)
{
    hi_s32 ret;
    vpss_hal_ctx *hal_ctx = HI_NULL;
    vpss_hal_buff *hal_node_buf = HI_NULL;

    vpss_hal_check_ip_vaild(ip);
    vpss_hal_check_null_ptr(hal_info);

    hal_ctx = &g_hal_ctx[ip];

    vpss_hal_check_init(hal_ctx->init);
    /* we will select a un used buffer to config ,than set it used */
    hal_node_buf = get_reg_node_buffer(ip);
    if (hal_node_buf == HI_NULL) {
        vpss_error("vpss has not buffer to config logic, nodetype=%d\n", hal_info->node_type);
        return HI_FAILURE;
    }

    ret = vpss_hal_set_node_info_byswitch(hal_info, hal_node_buf);
    if (ret != HI_SUCCESS) {
        vpss_error("set node failed \n", hal_info->node_type);
        return HI_FAILURE;
    }

    /* all ip ck_gt_en */
    vpss_sys_set_allip_ck_gt_en((vpss_reg_type *)(hal_node_buf->vir_addr), 0, HI_TRUE);

    hal_node_buf->used = HI_TRUE;

    if (hal_info->use_pq_module == HI_TRUE) {
        /* vpss_hal_logic_cfg_check((vpss_reg_type *)app_vir); */
    }

    return ret;
}

hi_void vpss_hal_get_lbd_info(hi_u8 *vir_addr, hi_drv_crop_rect *lbx_info, hi_drv_crop_rect *lbx_shift_info)
{
    vpss_stt_get_lbd_space((vpss_stt_reg_type *)vir_addr, &lbx_info->top_offset, &lbx_info->bottom_offset,
                           &lbx_info->left_offset, &lbx_info->right_offset);
    vpss_stt_get_lbd_shift((vpss_stt_reg_type *)vir_addr, &lbx_shift_info->top_offset, &lbx_shift_info->bottom_offset,
                           &lbx_shift_info->left_offset, &lbx_shift_info->right_offset);
    return;
}

hi_s32 vpss_hal_get_cmp_size(vpss_ip ip, hi_u32 *cmp_size)
{
    vpss_hal_ctx *hal_ctx = HI_NULL;
    vpss_reg_type *vpss_base_reg;
    vpss_hal_check_ip_vaild(ip);
    hal_ctx = &g_hal_ctx[ip];
    vpss_hal_check_init(hal_ctx->init);
    vpss_base_reg = (vpss_reg_type *)hal_ctx->base_reg_vir;
#ifdef DPT /* todo */
    vpss_mac_get_out0_frame_size(vpss_base_reg, cmp_size);
#endif
    return HI_SUCCESS;
}

hi_void vpss_hal_get_rgmv_addr(vpss_ip en_ip, hi_u64 *rgmv_addr)
{
    vpss_reg_type *temp_reg = HI_NULL;
    vpss_hal_ctx *hal_ctx = HI_NULL;
    hal_ctx = &g_hal_ctx[en_ip];

    if (hal_ctx == HI_NULL) {
        vpss_error("invalide ip %d\n", en_ip);
        return;
    }

    temp_reg = (vpss_reg_type *)hal_ctx->base_reg_vir;
    vpss_sys_get_rgmv_addr(temp_reg, 0, rgmv_addr);
    return;
}

#ifdef DPT
static vpss_hal_split_node_buffer *get_split_node_buffer(vpss_ip ip, hi_u32 size)
{
    hi_s32 ret;
    list *pos = HI_NULL;
    list *n = HI_NULL;
    vpss_hal_split_node_buffer *find_min_split_buffer = HI_NULL;
    vpss_hal_split_node_buffer *split_buffer = HI_NULL;
    vpss_hal_ctx *hal_ctx = HI_NULL;
    hal_ctx = &g_hal_ctx[ip];

    if (hal_ctx == HI_NULL) {
        vpss_error("invalide ip %d\n", ip);
        return HI_NULL;
    }

    /* find a min memory from the empty list */
    osal_list_for_each_safe(pos, n, &hal_ctx->split_node_buf_empty_list) {
        split_buffer = (vpss_hal_split_node_buffer *)osal_list_entry(pos, vpss_hal_split_node_buffer, node);
        if (split_buffer->vpss_mem.size >= size) {
            if ((find_min_split_buffer == HI_NULL) ||
                (split_buffer->vpss_mem.size < find_min_split_buffer->vpss_mem.size)) {
                find_min_split_buffer = split_buffer;
            }
        }
    }

    if (find_min_split_buffer != HI_NULL) {
        osal_list_del(&find_min_split_buffer->node);
        osal_list_add_tail(&find_min_split_buffer->node, &hal_ctx->split_node_buf_full_list);
        return find_min_split_buffer;
    }

    split_buffer = vpss_vmalloc(sizeof(vpss_hal_split_node_buffer));
    if (split_buffer == HI_NULL) {
        vpss_error("vpss alloc buffer failed. size %d\n", sizeof(vpss_hal_split_node_buffer));
        return HI_NULL;
    }

    memset(split_buffer, 0, sizeof(vpss_hal_split_node_buffer));
    split_buffer->attr.name = "vpss_split_node_buffer";
    split_buffer->attr.mode = OSAL_NSSMMU_TYPE;
    split_buffer->attr.is_map_viraddr = HI_FALSE;
    split_buffer->attr.is_cache = HI_FALSE;
    split_buffer->attr.size = size;
    ret = vpss_comm_mem_alloc(&split_buffer->attr, &split_buffer->vpss_mem);
    if (ret != HI_SUCCESS) {
        vpss_error("vpss alloc split node buffer failed, no secure,size %d\n", size);
        vpss_vfree(split_buffer);
        return HI_NULL;
    }

    osal_list_add_tail(&split_buffer->node, &hal_ctx->split_node_buf_full_list);
    return split_buffer;
}

static hi_void reset_split_node_buffer(vpss_ip ip)
{
    list *pos = HI_NULL;
    list *n = HI_NULL;
    vpss_hal_split_node_buffer *split_buffer = HI_NULL;
    vpss_hal_ctx *hal_ctx = HI_NULL;
    hal_ctx = &g_hal_ctx[ip];

    if (hal_ctx == HI_NULL) {
        vpss_error("ip %d is invalid\n", ip);
        return;
    }

    osal_list_for_each_safe(pos, n, &hal_ctx->split_node_buf_full_list) {
        split_buffer = (vpss_hal_split_node_buffer *)osal_list_entry(pos, vpss_hal_split_node_buffer, node);
        osal_list_del(pos);
        osal_list_add_tail(&split_buffer->node, &hal_ctx->split_node_buf_empty_list);
    }
    return;
}

hi_void free_all_split_node_buffer(vpss_ip ip)
{
    list *pos = HI_NULL;
    list *n = HI_NULL;
    vpss_hal_split_node_buffer *split_buf = HI_NULL;
    vpss_hal_ctx *hal_ctx = HI_NULL;
    hal_ctx = &g_hal_ctx[ip];

    if (hal_ctx == HI_NULL) {
        vpss_error("invalide ip %d\n", ip);
        return;
    }

    osal_list_for_each_safe(pos, n, &hal_ctx->split_node_buf_empty_list) {
        split_buf = (vpss_hal_split_node_buffer *)osal_list_entry(pos, vpss_hal_split_node_buffer, node);
        osal_list_del(pos);

        if (split_buf->vpss_mem.phy_addr != 0) {
            vpss_comm_mem_free(&split_buf->vpss_mem);
        }

        vpss_vfree(split_buf);
    }
    osal_list_for_each_safe(pos, n, &hal_ctx->split_node_buf_full_list) {
        split_buf = (vpss_hal_split_node_buffer *)osal_list_entry(pos, vpss_hal_split_node_buffer, node);
        osal_list_del(pos);

        if (split_buf->vpss_mem.phy_addr != 0) {
            vpss_comm_mem_free(&split_buf->vpss_mem);
        }

        vpss_vfree(split_buf);
    }
    return;
}
#endif

hi_s32 vpss_hal_clear_task_reset(vpss_ip ip)
{
    vpss_reg_type *temp_reg = HI_NULL;
    vpss_hal_ctx *hal_ctx = HI_NULL;
    hi_s32 ret;

    vpss_hal_check_ip_vaild(ip);
    hal_ctx = &g_hal_ctx[ip];
    if (hal_ctx == HI_NULL) {
        vpss_error("invalide ip %d\n", ip);
        return HI_FAILURE;
    }

    vpss_hal_check_init(hal_ctx->init);
    temp_reg = (vpss_reg_type *)hal_ctx->base_reg_vir;
    vpss_soft_reset(ip, temp_reg, HI_TRUE);
    /* after reset, need to do init like power on first time */
    ret = vpss_hal_sys_init(ip, HI_FALSE);
    if (ret != HI_SUCCESS) {
        vpss_error(" set clock failed\n");
    }

    ret = vpss_hal_sys_init(ip, HI_TRUE);
    if (ret != HI_SUCCESS) {
        vpss_error(" set clock failed\n");
    }

    return HI_SUCCESS;
}

hi_s32 vpss_hal_process_interrupt(vpss_ip ip)
{
    hi_u32 int_state = 0;
    hi_s32 ret;

    vpss_hal_check_ip_vaild(ip);

    ret = vpss_hal_get_smmu_int_state(ip, &int_state);
    if (ret != HI_SUCCESS) {
        vpss_error("get smmu int state failed\n");
    }

    ret = vpss_hal_clear_smmu_int_state(ip, int_state);
    if (ret != HI_SUCCESS) {
        vpss_error("clear smmu int state failed\n");
    }

    ret = vpss_hal_get_int_state(ip, &int_state);
    if (ret != HI_SUCCESS) {
        vpss_error("get int state failed\n");
    }

    ret = vpss_hal_clear_int_state(ip, int_state);
    if (ret != HI_SUCCESS) {
        vpss_error("clear int state failed\n");
    }

    if (int_state & HAL_NODE_END_INT_MASK) { /* bit 11 */
        vpss_error("node_end_m_int!!!!  state = %x \n", int_state);
    }

    if (int_state & HAL_TUNNEL_INT_MASK) { /* bit 9 */
        vpss_error("input tunnel error!!!!  state = %x \n", int_state);
    }
#ifdef VPSS_96CV300_CS_SUPPORT
    if (int_state & HAL_BUS0_READ_INT_MASK) { /* bit 6 */
        vpss_error("vpss bus0 read error!!!!  state = %x \n", int_state);
    }
#endif
    if (int_state & HAL_BUS0_WRITE_INT_MASK) { /* bit 5 */
        vpss_error("vpss bus0 write error!!!!  state = %x \n", int_state);
    }

    if (int_state & HAL_NODE_TIMEOUT_INT_MASK) { /* bit 4 */
        vpss_error("vpss node_timeout!!!!  state = %x \n", int_state);
    }

    if (int_state & HAL_SUCCESS_INT_MASK) { /* bit 2 */
        return HI_SUCCESS;
    }

    return HI_FAILURE;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


