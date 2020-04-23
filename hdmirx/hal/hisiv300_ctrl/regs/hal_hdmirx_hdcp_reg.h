/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Register definition of hdcp module
 * Author: Hisilicon multimedia interface software group
 * Create: 2019/12/4
 */
#ifndef __HAL_HDMIRX_HDCP_REG_H__
#define __HAL_HDMIRX_HDCP_REG_H__

#define REG_CFG_BKSV_0 0x0000
#define CFG_BKSV_0 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_BKSV_1 0x0004
#define CFG_BKSV_1 0xFFFF /* mask bit 15:0 */

#define REG_CFG_RI 0x0008
#define CFG_RI 0xFFFF /* mask bit 15:0 */

#define REG_CFG_PJ 0x000C
#define CFG_PJ 0xFF /* mask bit 7:0 */

#define REG_CFG_AKSV_0 0x0010
#define CFG_AKSV_0 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_AKSV_1 0x0014
#define CFG_AKSV_1 0xFFFF /* mask bit 15:0 */

#define REG_CFG_AN_0 0x0018
#define CFG_AN_0 0xFFFFFFFF /* mask bit 31:0 */

#define REG_CFG_AN_1 0x001C
#define CFG_AN_1 0xFFFFFFFF /* mask bit 31:0 */

#define REG_CFG_AINFO 0x0020
#define CFG_AINFO 0x1 /* mask bit 0 */

#define REG_CFG_BCAPS_SET 0x0024
#define CFG_FAST_REAUTH  0x1  /* mask bit 0 */
#define CFG_1P1_FEATURE  0x2  /* mask bit 1 */
#define CFG_FAST         0x10 /* mask bit 4 */
#define CFG_HDMI_CAPABLE 0x80 /* mask bit 7 */

#define REG_CFG_AINFO_OVERWRITE 0x0028
#define CFG_AINFO_OVERWRITE_EN 0x1 /* mask bit 0 */
#define CFG_AINFO_VALUE        0x2 /* mask bit 1 */

#define REG_CFG_AC_EN 0x002C
#define CFG_AC_SEL             0x1  /* mask bit 0 */
#define HDCP_CFG_HDMI_MODE_SEL 0x2  /* mask bit 1 */
#define CFG_HDMI_MODE_VAL      0x4  /* mask bit 2 */
#define CFG_AC_EN              0x10 /* mask bit 4 */

#define REG_CFG_PJ_EN 0x0030
#define CFG_PJ_SEL 0x1  /* mask bit 0 */
#define CFG_PJ_EN  0x10 /* mask bit 4 */

#define REG_CFG_REAUTH_SEL 0x0034
#define CFG_REAUTH_SEL 0x1 /* mask bit 0 */

#define REG_CFG_BKSV_LD 0x0038
#define CFG_BKSV_LD 0x1 /* mask bit 0 */

#define REG_CFG_BIST_EN 0x003C
#define CFG_BIST_EN 0x1 /* mask bit 0 */

#define REG_CFG_STOP_ENC 0x0040
#define CFG_STOP_ENC 0x1 /* mask bit 0 */

#define REG_CFG_MUTE_VLD 0x0044
#define CFG_MUTE_VLD 0x1 /* mask bit 0 */

#define REG_CFG_CTL3_SEL 0x0048
#define CFG_DBG_CTL3_SEL 0x1 /* mask bit 0 */

#define REG_CFG_AUTH_STATE 0x004C
#define CFG_AUTH_DONE      0x1     /* mask bit 0 */
#define CFG_DECRYPTING_ON  0x2     /* mask bit 1 */
#define CFG_CURR_STATE     0xF0    /* mask bit 7:4 */
#define CFG_AKSV_ERR       0x100   /* mask bit 8 */
#define CFG_OTP_CTRL_STATE 0x7000  /* mask bit 14:12 */
#define CFG_OTP_WR_STATE   0x70000 /* mask bit 18:16 */

#define REG_CFG_RI_ERR_CNT 0x0050
#define CFG_RI_ERR_CNT 0xFFFF /* mask bit 15:0 */

#define REG_CFG_DE_ERR_CNT 0x0054
#define HDCP_CFG_DE_ERR_CNT 0xFFFF /* mask bit 15:0 */

#define REG_CFG_ENCRY_FRM_CNT 0x0058
#define CFG_ENCRY_FRM_CNT 0xFFFF /* mask bit 15:0 */

#define REG_CFG_NOCRY_FRM_CNT 0x005C
#define CFG_NOCRY_FRM_CNT 0xFFFF /* mask bit 15:0 */

#define REG_CFG_MUTE_ENCRY_FRM_CNT 0x0060
#define CFG_MUTE_ENCRY_FRM_CNT 0xFFFF /* mask bit 15:0 */

#define REG_CFG_REAUTH_ERR_CNT 0x0064
#define CFG_REAUTH_ERR_CNT 0xFFFF /* mask bit 15:0 */

#define REG_CFG_AKSV_ERR_EN 0x0068
#define CFG_AKSV_ERR_EN 0x1 /* mask bit 0 */

#define REG_CFG_REAUTH_CNT 0x006C
#define CFG_REAUTH_CNT 0xFFFF /* mask bit 15:0 */

#define REG_CFG_BKSV_STATE 0x0070
#define CFG_BKSV_LD_DONE 0x1 /* mask bit 0 */

#define REG_CFG_BIST_STATE 0x0074
#define HDCP_CFG_BIST_DONE 0x1 /* mask bit 0 */
#define CFG_BIST_PASS      0x2 /* mask bit 1 */

#define REG_CFG_ECC 0x0078
#define CFG_CNT2CHK_ECC 0x1FF     /* mask bit 8:0 */
#define CFG_ECC_THR     0xFF0000  /* mask bit 23:16 */
#define CFG_ECC_CHK_EN  0x1000000 /* mask bit 24 */

#define HDCP_ORIGINAL_IRQ_STATUS 0x007C
#define CFG_ORIGINAL_STATUS_AUTH_DONE 0x1 /* mask bit 0 */
#define CFG_ORIGINAL_STATUS_AKSV_ERR  0x2 /* mask bit 1 */
#define CFG_ORIGINAL_STATUS_ECC_ERR   0x4 /* mask bit 2 */
#define CFG_ORIGINAL_STATUS_HDCP_QUIT 0x8 /* mask bit 3 */

#define HDCP_MASKED_IRQ_STATUS 0x0080
#define CFG_MASKED_STATUS_HDCP_DONE 0x1 /* mask bit 0 */
#define CFG_MASKED_STATUS_AKSV_ERR  0x2 /* mask bit 1 */
#define CFG_MASKED_STATUS_ECC_ERR   0x4 /* mask bit 2 */
#define CFG_MASKED_STATUS_HDCP_QUIT 0x8 /* mask bit 3 */

#define HDCP_IRQ_MASK 0x0084
#define CFG_MASK_HDCP_DONE 0x1 /* mask bit 0 */
#define CFG_MASK_AKSV_ERR  0x2 /* mask bit 1 */
#define CFG_MASK_ECC_ERR   0x4 /* mask bit 2 */
#define CFG_MASK_HDCP_QUIT 0x8 /* mask bit 3 */

#define REG_CFG_FILTER_CTL 0x0100
#define CFG_VS_FILTER_SEL       0x3     /* mask bit 1:0 */
#define CFG_VS_FILTER_BYPASS    0x8     /* mask bit 3 */
#define CFG_HS_FILTER_SEL       0x30    /* mask bit 5:4 */
#define CFG_HS_FILTER_BYPASS    0x80    /* mask bit 7 */
#define CFG_CTL_FILTER_SEL      0x300   /* mask bit 9:8 */
#define CFG_CTL_FILTER_BYPASS   0x800   /* mask bit 11 */
#define CFG_DIDE_FILTER_SEL     0x3000  /* mask bit 13:12 */
#define CFG_DIDE_FILTER_BYPASS  0x8000  /* mask bit 15 */
#define CFG_VIDDE_FILTER_SEL    0x30000 /* mask bit 17:16 */
#define CFG_VIDDE_FILTER_BYPASS 0x80000 /* mask bit 19 */

#define REG_CFG_DVI_HDCP 0x0104
#define CFG_DVI_HDCP_FORCE_EN 0x1  /* mask bit 0 */
#define CFG_DVI_HDCP_MODE     0x6  /* mask bit 2:1 */
#define CFG_OESS_SEL          0x10 /* mask bit 4 */

#define REG_CFG_HDMI_HDCP 0x0108
#define CFG_CTL_MODE 0x3   /* mask bit 1:0 */
#define CFG_CTL_THR  0xF00 /* mask bit 11:8 */

#define REG_CFG_VS_PO 0x010C
#define CFG_VS_POL_FORCE_EN 0x1  /* mask bit 0 */
#define CFG_VS_POL_VALUE    0x10 /* mask bit 4 */

#define REG_CFG_VS_DET 0x0110
#define CFG_VS_POL 0x1 /* mask bit 0 */

#define REG_CFG_HDCP_WOO 0x0114
#define CFG_HDCP_WOO_SP       0x3FF      /* mask bit 9:0 */
#define CFG_HDCP_WOO_EP       0x3FF0000  /* mask bit 25:16 */
#define CFG_HDCP_WOO_FORCE_EN 0x10000000 /* mask bit 28 */

#define REG_CFG_HDCP_GLITCH_DET 0x0118
#define CFG_HDCP_GLITCH_THR    0xFF  /* mask bit 7:0 */
#define CFG_HDCP_GLITCH_DET_EN 0x100 /* mask bit 8 */

#define REG_CFG_CTL3_ERR_CNT 0x011C
#define CFG_CTL3_ERR_CNT 0xFFFF /* mask bit 15:0 */

#define REG_CFG_DVI_CTL3_THR 0x0120
#define CFG_DVI_CTL3_THR 0x7 /* mask bit 2:0 */

#endif /* __HAL_HDMIRX_HDCP_REG_H__ */
