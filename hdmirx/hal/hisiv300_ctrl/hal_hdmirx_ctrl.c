/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Implementation of controller functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-12-3
 */
#include "hal_hdmirx_ctrl.h"
#include <linux/delay.h>
#include "hal_hdmirx_pwd_reg.h"
#include "hal_hdmirx_aon_reg.h"
#include "hal_hdmirx_port_reg.h"
#include "hal_hdmirx_cec_reg.h"
#include "hal_hdmirx_depack_reg.h"
#include "hal_hdmirx_audio_reg.h"
#include "hal_hdmirx_video_reg.h"
#include "hal_hdmirx_decoder_reg.h"
#include "hal_hdmirx_ddc_reg.h"
#include "hal_hdmirx_frl_reg.h"
#include "hal_hdmirx_hdcp2x_cpu_reg.h"
#include "hal_hdmirx_hdcp_reg.h"
#include "hal_hdmirx_dphy_reg.h"
#include "hal_hdmirx_damix_reg.h"

static hi_void hdmirx_hal_pwd_init(hi_drv_hdmirx_port port)
{
    /* cken */
    hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_CKEN, CFG_CKEN_DPK, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_CKEN, CFG_CKEN_DPK_EMP, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_CKEN, CFG_CKEN_TMDS_DEC, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_CKEN, CFG_CKEN_FRL, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_CKEN, CFG_CKEN_AUD_AAC, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_CKEN, CFG_CKEN_AUD_DACR_RFCLK, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_CKEN, CFG_CKEN_HDCP1X, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_CKEN, CFG_CKEN_HDCP2X, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_SEL, CFG_SCDC_FRL_EN, HI_TRUE);
}

static hi_void hdmirx_hal_aon_init(hi_drv_hdmirx_port port)
{
    /* set port sel is port0 */
    hdmirx_hal_aon_reg_write_fld_align(port, REG_RX_PORT_SEL, RX_PORT_SEL, 0x0); /* 0x0 : sel port0 */
    /* 0xa : set hpd low thresholds is 10ms */
    hdmirx_hal_aon_reg_write_fld_align(port, REG_RX_HPD_CTRL, HPD_LOW_RESH, 0xa);
    /* set pwr det input is pwr5v, input pol is no_invert, pwr5v det sel is pwr5v/hpd */
    hdmirx_hal_aon_reg_write_fld_align(port, REG_PWR_DET_SRC, PWR_DET_SRC, HI_FALSE);
    hdmirx_hal_aon_reg_write_fld_align(port, REG_PWR_DET_PCTL, PWR_DET_PCTRL, HI_FALSE);
    hdmirx_hal_aon_reg_write_fld_align(port, REG_PWR5V_DET_SEL, PWR5V_DET_SEL, HI_FALSE);
    /* pwr det fiter */
    hdmirx_hal_aon_reg_write_fld_align(port, REG_PWR_FILT_RESH, PWR_FILT_POS_RES, 0x32); /* 0x32 : default value */
    hdmirx_hal_aon_reg_write_fld_align(port, REG_PWR_FILT_RESH, PWR_FILT_NEG_RES, 0x32); /* 0x32 : default value */
    hdmirx_hal_aon_reg_write_fld_align(port, REG_PWR_FILT_CTRL, PWR_FILT_EN, HI_TRUE);
    /* set global ddc slv disable, ddc scdc slv/ddc hdcp slv/ddc edid slv disable */
    hdmirx_hal_aon_reg_write_fld_align(port, REG_DDC_SLV_CTL, DDC_SLV_EN, HI_FALSE);
    hdmirx_hal_aon_reg_write_fld_align(port, REG_DDC_DEV_CTL, DDC_SCDC_EN, HI_FALSE);
    hdmirx_hal_aon_reg_write_fld_align(port, REG_DDC_DEV_CTL, DDC_HDCP_EN, HI_FALSE);
    hdmirx_hal_aon_reg_write_fld_align(port, REG_DDC_DEV_CTL, DDC_EDID_EN, HI_FALSE);
    /* set scl filter/sda filter is 84ns */
    hdmirx_hal_aon_reg_write_fld_align(port, REG_DDC_FILT_CTL, DDC_FILT_SEL, 0x2); /* 0x2 : 84ns */
    /* set sda_in/sda_out delay is 250ns */
    hdmirx_hal_aon_reg_write_fld_align(port, REG_DDC_DLY_CTL, SDA_IN_DLY_CNT, 0x6);  /* 0x6 : 6*42ns */
    hdmirx_hal_aon_reg_write_fld_align(port, REG_DDC_DLY_CTL, SDA_OUT_DLY_CNT, 0x6); /* 0x6 : 6*42ns */
    /* set sda timeout thresholds */
    hdmirx_hal_aon_reg_write_fld_align(port, REG_DDC_TOUT_RES, DDC_TOUT_RESHOLD, 0x64); /* 0x64 : timeout 100ms */
    /* set scl hold thresholds */
    hdmirx_hal_aon_reg_write_fld_align(port, REG_DDC_STRETCH_CTL, SCL_HOLD_RESH, 0x14); /* 0x14 : threshold 20us */
    /* set edid limit */
    hdmirx_hal_aon_reg_write_fld_align(port, REG_DDC_SEG_LIMIT, EDID_SEG_LIMIT, 0x1); /* 0x1 : segment limit */
    hdmirx_hal_aon_reg_write_fld_align(port, REG_EDID_LIMIT_CTL, EDID_LIMIT_EN, HI_TRUE);
    hdmirx_hal_aon_reg_write_fld_align(port, REG_EDID_LIMIT_CTL, EDID_LIMIT_MODE, HI_TRUE);
    hdmirx_hal_aon_reg_write_fld_align(port, REG_EDID_LIMIT_CTL, EDID_TYPE_LIMIT, 0x0); /* 0x0 : limit type 0-511 */
    hdmirx_hal_aon_reg_write_fld_align(port, REG_EDID_LIMIT_CTL, EDID_LIMIT_DATA, 0xFF); /* 0xFF : limit data */
    hdmirx_hal_aon_reg_write_fld_align(port, REG_EDID_LIMIT_CTL, EDID_LIMIT_ADDR, 0xFF); /* 0xFF : limit 0-255 */
    /* set aon auto reset */
    hdmirx_hal_aon_reg_write_fld_align(port, REG_DDC_SLV_RST, DDC_TOUT_RST_EN, HI_TRUE);
    hdmirx_hal_aon_reg_write_fld_align(port, REG_DDC_SLV_RST, DDC_PWR_RST_EN, HI_TRUE);
    hdmirx_hal_aon_reg_write_fld_align(port, REG_DDC_SLV_RST, DDC_HPD_RST_EN, HI_TRUE);
}

static hi_void hdmirx_hal_set_sw_pkt_id(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_ID_0, CFG_GCP_ID, 0x3);    /* 0x3 : gcp packet header */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_ID_0, CFG_ACP_ID, 0x4);    /* 0x4 : acp packet header */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_ID_0, CFG_ISRC1_ID, 0x5);  /* 0x5 : isrc1 packet header */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_ID_0, CFG_ISRC2_ID, 0x6);  /* 0x6 : isrc2 packet header */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_ID_1, CFG_GMP_ID, 0xA);    /* 0xa : gmp packet header */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_ID_1, CFG_VSI_ID, 0x81);   /* 0x81 : vsi packet header */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_ID_1, CFG_AVI_ID, 0x82);   /* 0x82 : avi packet header */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_ID_2, CFG_SPD_ID, 0x83);   /* 0x83 : spd packet header */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_ID_2, CFG_AIF_ID, 0x84);   /* 0x84 : aif packet header */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_ID_2, CFG_MPEG_ID, 0x85);  /* 0x85 : mpeg packet header */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_ID_2, CFG_HDR_ID, 0x87);   /* 0x87 : hdr packet header */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_ID_3, CFG_UNREC_ID, 0xFF); /* 0xFF : unrec packet header */
    hdmirx_hal_reg_write_fld_align(port, REG_AUD_PKT_0, CFG_ACR_ID, 0x1);   /* 0x1 : acr packet header */
    hdmirx_hal_reg_write_fld_align(port, REG_AUD_PKT_0, CFG_ASP_ID, 0x2);   /* 0x2 : asp packet header */
    hdmirx_hal_reg_write_fld_align(port, REG_AUD_PKT_0, CFG_DSD_ID, 0x7);   /* 0x7 : dsd packet header */
    hdmirx_hal_reg_write_fld_align(port, REG_AUD_PKT_0, CFG_DST_ID, 0x8);   /* 0x8 : dst packet header */
    hdmirx_hal_reg_write_fld_align(port, REG_AUD_PKT_1, CFG_HBR_ID, 0x9);   /* 0x9 : hbr packet header */
    hdmirx_hal_reg_write_fld_align(port, REG_AUD_PKT_1, CFG_3D_ASP_ID, 0xB);      /* 0xB : 3d asp packet header */
    hdmirx_hal_reg_write_fld_align(port, REG_AUD_PKT_1, CFG_3D_DSD_ID, 0xC);      /* 0xC : 3d dsd packet header */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_ID_1, CFG_AUD_META_ID, 0xD);     /* 0xD : aud meta packet header */
    hdmirx_hal_reg_write_fld_align(port, REG_AUD_PKT_1, CFG_MS_ASP_ID, 0xE);      /* 0xE : ms asp packet header */
    hdmirx_hal_reg_write_fld_align(port, REG_AUD_PKT_2, CFG_1BIT_MS_ASP_ID, 0xF); /* 0xF : 1bit ms asp packet header */

    hdmirx_hal_reg_write_fld_align(port, REG_VSI_IEEE_ID, CFG_VSI_IEEE_ID, 0xC03); /* 0xC03 : id */
    hdmirx_hal_reg_write_fld_align(port, REG_HF_VSI_3D_IEEE_ID, CFG_HFVSI_3D_IEEE_ID, 0xC45DD8); /* 0xC45DD8c : id */
    hdmirx_hal_reg_write_fld_align(port, REG_HF_VSI_HDR_IEEE_ID, CFG_HFVSI_HDR_IEEE_ID, 0xD046); /* 0xD046 : id */
}

static hi_void hdmirx_hal_set_hw_emp_pkt_org_id_and_data(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_write_fld_align(port, REG_FVAVRR_EMP_CFG, CFG_FVAVRR_EMP_DATA_SET, 0x1); /* 0x1 fv emp data set */
    hdmirx_hal_reg_write_fld_align(port, REG_FVAVRR_EMP_CFG, CFG_FVAVRR_EMP_ORG_ID, 0x1); /* 0x1 fv emp org id set */

    hdmirx_hal_reg_write_fld_align(port, REG_VS0_EMP_CFG, CFG_VS0_EMP_DATA_SET, 0x0); /* 0x0 vs0 emp data set */
    hdmirx_hal_reg_write_fld_align(port, REG_VS0_EMP_CFG, CFG_VS0_EMP_ORG_ID, 0x0); /* 0x0 vs0 emp org id set */

    hdmirx_hal_reg_write_fld_align(port, REG_VS1_EMP_CFG, CFG_VS1_EMP_DATA_SET, 0x1); /* 0x1 vs1 emp data set */
    hdmirx_hal_reg_write_fld_align(port, REG_VS1_EMP_CFG, CFG_VS1_EMP_ORG_ID, 0x0); /* 0x0 vs1 emp org id set */

    hdmirx_hal_reg_write_fld_align(port, REG_DSC_EMP_CFG, CFG_DSC_EMP_DATA_SET, 0x2); /* 0x2 : dsc emp data set */
    hdmirx_hal_reg_write_fld_align(port, REG_DSC_EMP_CFG, CFG_DSC_EMP_ORG_ID, 0x1); /* 0x1 : dsc emp data org id set */

    hdmirx_hal_reg_write_fld_align(port, REG_HDR_EMP_CFG_0, CFG_HDR_DATA_SET_LOW, 0x1); /* 0x1 : data set low */
    hdmirx_hal_reg_write_fld_align(port, REG_HDR_EMP_CFG_1, CFG_HDR_DATA_SET_HIGH, 0x4); /* 0x4 : data set high */
    hdmirx_hal_reg_write_fld_align(port, REG_HDR_EMP_CFG_0, CFG_HDR_EMP_ORG_ID, 0x2); /* 0x1 : hdr emp org id */

    hdmirx_hal_reg_write_fld_align(port, REG_GEN_EMP_CFG, CFG_GEN_EMP_DATA_SET, 0xFF); /* 0xFF : gen emp data set */
    hdmirx_hal_reg_write_fld_align(port, REG_GEN_EMP_CFG, CFG_GEN_EMP_ORG_ID, 0xF); /* 0xF : gen emp org id */

    hdmirx_hal_reg_write_fld_align(port, REG_VI_TEST_PKT, CFG_VI_TEST_DATA_SET, 0xFFFF); /* 0xFFFF : vi data set */
    hdmirx_hal_reg_write_fld_align(port, REG_VI_TEST_PKT, CFG_VI_TEST_ORG_ID, 0xFF); /* 0xFF : vi org id */

    hdmirx_hal_reg_write_fld_align(port, REG_DSC_TEST_PKT, CFG_DSC_TEST_DATA_SET, 0xFFFF); /* 0xFFFF dsc data set */
    hdmirx_hal_reg_write_fld_align(port, REG_DSC_TEST_PKT, CFG_DSC_TEST_ORG_ID, 0xFF); /* 0xFF dsc org id */

    hdmirx_hal_reg_write_fld_align(port, REG_VS0_OUI_CFG, CFG_VS0_EMP_IEEE_OUI, 0x0); /* 0x0 : vs0 emp ieee oui */
    hdmirx_hal_reg_write_fld_align(port, REG_VS1_OUI_CFG, CFG_VS1_EMP_IEEE_OUI, 0x0); /* 0x0 : vs1 emp ieee oui */
}

static hi_void hdmirx_hal_set_sw_pkt_lost_cnt(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_0, CFG_GCP_LOST_CNT, 0x3); /* 0x3 gcp lost cnt */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_0, CFG_ACP_LOST_CNT, 0x3); /* 0x3 acp lost cnt */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_1, CFG_ISRC1_LOST_CNT, 0x3); /* 0x3 isrc1 lost cnt */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_1, CFG_ISRC2_LOST_CNT, 0x3); /* 0x3 isrc2 lost cnt */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_1, CFG_GMP_LOST_CNT, 0x1); /* 0x1 gmp lost cnt */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_1, CFG_AUD_META_LOST_CNT, 0x3); /* 0x3 aud meta lost cnt */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_2, CFG_VSI_LOST_CNT, 0x3); /* 0x3 vsi lost cnt */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_4, CFG_HFVSI_3D_LOST_CNT, 0x3); /* 0x3 hfvsi 3d lost cnt */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_4, CFG_HFVSI_HDR_LOST_CNT, 0x3); /* 0x3 hfvsi hdr lost cnt */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_2, CFG_AVI_LOST_CNT, 0x3); /* 0x3 avi lost cnt */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_2, CFG_SPD_LOST_CNT, 0x3C); /* 0x3c spd lost cnt */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_2, CFG_AIF_LOST_CNT, 0x2); /* 0x2 aif lost cnt */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_3, CFG_MPEG_LOST_CNT, 0x1); /* 0x1 mpeg lost cnt */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_3, CFG_HDR_LOST_CNT, 0x3); /* 0x3 hdr lost cnt */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_3, CFG_UNREC_LOST_CNT, 0x3); /* 0x3 unrec lost cnt */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_3, CFG_TRASH_LOST_CNT, 0x3); /* 0x3 trash lost cnt */
}

static hi_void hdmirx_hal_set_hw_emp_pkt_lost_cnt(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_LOST_CNT, CFG_VS0_EMP_LOST, 0x1); /* 0x1: vs0 emp lost */
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_LOST_CNT, CFG_VS1_EMP_LOST, 0x1); /* 0x1: vs1 emp lost */
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_LOST_CNT, CFG_DSC_EMP_LOST, 0x1); /* 0x1: dsc emp lost */
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_LOST_CNT, CFG_HDR_EMP_LOST, 0x1); /* 0x1: hdr emp lost */
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_LOST_CNT, CFG_FVAVRR_EMP_LOST, 0x1); /* 0x1: fv emp lost */
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_LOST_CNT, CFG_GEN_EMP_LOST, 0x3); /* 0x1: gen emp lost */
}

static hi_void hdmirx_hal_set_hw_emp_pkt_timeout_cnt(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_TIMEOUT_CNT, CFG_FVAVRR_EMP_TIMEOUT, 0xA); /* 0xa fv emp timeout */
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_TIMEOUT_CNT, CFG_VS0_EMP_TIMEOUT, 0xA); /* 0xa vs0 emp timeout */
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_TIMEOUT_CNT, CFG_VS1_EMP_TIMEOUT, 0xA); /* 0xa vs1 emp timeout */
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_TIMEOUT_CNT, CFG_DSC_EMP_TIMEOUT, 0xA); /* 0xa dsc emp timeout */
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_TIMEOUT_CNT, CFG_HDR_EMP_TIMEOUT, 0xA); /* 0xa hdr emp timeout */
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_TIMEOUT_CNT, CFG_GEN_EMP_TIMEOUT, 0xA); /* 0xa gen emp timeout */
}

static hi_void hdmirx_hal_set_data_clear_in_no_pkt(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_0, CFG_ACP_CLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_0, CFG_GCP_CLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_1, CFG_ISRC1_CLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_1, CFG_ISRC2_CLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_1, CFG_GMP_CLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_1, CFG_AUD_META_CLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_2, CFG_VSI_CLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_2, CFG_AVI_CLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_2, CFG_SPD_CLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_2, CFG_AIF_CLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_3, CFG_MPEG_CLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_3, CFG_HDR_CLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_3, CFG_UNREC_CLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_3, CFG_TRASH_CLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_4, CFG_HFVSI_3D_CLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_4, CFG_HFVSI_HDR_CLR_EN, HI_TRUE);
}

static hi_void hdmirx_hal_set_data_clear_in_link_lost(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_0, CFG_ACP_AUTOCLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_0, CFG_GCP_AUTOCLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_1, CFG_ISRC1_AUTOCLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_1, CFG_ISRC2_AUTOCLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_1, CFG_GMP_AUTOCLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_1, CFG_AUD_META_AUTOCLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_2, CFG_VSI_AUTOCLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_2, CFG_AVI_AUTOCLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_2, CFG_SPD_AUTOCLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_2, CFG_AIF_AUTOCLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_3, CFG_MPEG_AUTOCLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_3, CFG_HDR_AUTOCLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_3, CFG_UNREC_AUTOCLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_3, CFG_TRASH_AUTOCLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_4, CFG_HFVSI_3D_AUTOCLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CLR_4, CFG_HFVSI_HDR_AUTOCLR_EN, HI_TRUE);
}

static hi_void hdmirx_hal_set_sw_pkt_recive_en(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_DET, CFG_ACP_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_DET, CFG_GCP_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_DET, CFG_ISRC1_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_DET, CFG_ISRC2_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_DET, CFG_GMP_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_DET, CFG_AUD_META_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_DET, CFG_VSI_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_DET, CFG_AVI_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_DET, CFG_SPD_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_DET, CFG_AIF_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_DET, CFG_MPEG_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_DET, CFG_HDR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_DET, CFG_UNREC_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_DET, CFG_TRASH_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_DET, CFG_HFVSI_3D_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_DET, CFG_HFVSI_HDR_EN, HI_TRUE);
}

static hi_void hdmirx_hal_set_hw_emp_pkt_recive_en(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_CFG_EN, CFG_FVAVRR_EMP_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_CFG_EN, CFG_VS0_EMP_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_CFG_EN, CFG_VS1_EMP_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_CFG_EN, CFG_DSC_EMP_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_CFG_EN, CFG_HDR_EMP_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_CFG_EN, CFG_GEN_EMP_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_CFG_EN, CFG_DSC_UNREC_EMP_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_CFG_EN, CFG_VI_UNREC_EMP_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_TEST_CFG, CFG_VI_TEST_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_TEST_CFG, CFG_DSC_TEST_EN, HI_TRUE);
}

static hi_void hdmirx_hal_sw_pkt_init(hi_drv_hdmirx_port port)
{
    hdmirx_hal_set_sw_pkt_id(port);
    hdmirx_hal_set_sw_pkt_lost_cnt(port);
    hdmirx_hal_set_data_clear_in_no_pkt(port);
    hdmirx_hal_set_data_clear_in_link_lost(port);
    hdmirx_hal_set_sw_pkt_recive_en(port);
}

static hi_void hdmirx_hal_hw_pkt_init(hi_drv_hdmirx_port port)
{
    hdmirx_hal_set_hw_emp_pkt_org_id_and_data(port);
    hdmirx_hal_set_hw_emp_pkt_timeout_cnt(port);
    hdmirx_hal_set_hw_emp_pkt_lost_cnt(port);
    hdmirx_hal_set_hw_emp_pkt_recive_en(port);
}

static hi_void hdmirx_hal_capture_pkt_init(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_ID_CFG_0, CFG_PKT_ID0, 0x81); /* 0x81 : capture packet id */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_ID_CFG_0, CFG_PKT_ID1, 0x82); /* 0x82 : capture packet id */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_ID_CFG_0, CFG_PKT_ID2, 0x83); /* 0x83 : capture packet id */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_ID_CFG_0, CFG_PKT_ID3, 0x84); /* 0x84 : capture packet id */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_ID_CFG_1, CFG_PKT_ID4, 0x85); /* 0x85 : capture packet id */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_ID_CFG_1, CFG_PKT_ID5, 0x87); /* 0x87 : capture packet id */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_ID_CFG_1, CFG_PKT_ID6, 0x1); /* 0x1 : capture packet id */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_ID_CFG_1, CFG_PKT_ID7, 0x2); /* 0x2 : capture packet id */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_ID_CFG_2, CFG_PKT_ID8, 0x3); /* 0x3 : capture packet id */
    hdmirx_hal_reg_write_fld_align(port, REG_PKT_ID_CFG_2, CFG_PKT_ID9, 0x4); /* 0x4 : capture packet id */

    hdmirx_hal_reg_write_fld_align(port, REG_PKT_CAPTURE, CFG_PKT_EN, 0x3FF); /* 0x3FF : capture en */
}

static hi_void hdmirx_hal_depack_init(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_write_fld_align(port, REG_RX_BCH_THRES, CFG_BCH_THRESH, 0x1); /* 0x1 : bch threshold */
    hdmirx_hal_reg_write_fld_align(port, REG_RX_PKT_THRESH, CFG_HDMI_PKT_THRESH, 0x3FFF); /* 0x3FFF : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_RX_T4_PKT_THRES, CFG_T4_PKT_THRESH, 0x1); /* 0x1 : T4 error threshold */
    hdmirx_hal_reg_write_fld_align(port, REG_RX_BCH_PKT_THRES, REG_RX_BCH_PKT_THRES, 0x1); /* 0x1 bch threshold */
    hdmirx_hal_reg_write_fld_align(port, REG_RX_HDCP_THRES, CFG_HDCP_PKT_THRESH, 0x1); /* 0x1 hdcp threshold */
    hdmirx_hal_reg_write_fld_align(port, REG_VI_TEST_PKT, CFG_VI_TEST_INDEX, 0xFF); /* 0xFF : cfg vi emp index */
    hdmirx_hal_reg_write_fld_align(port, REG_DSC_TEST_PKT, CFG_DSC_TEST_INDEX, 0XFF); /* 0xFF : cfg dsc emp index */
    hdmirx_hal_reg_write_fld_align(port, REG_RX_ECC_CTRL, CFG_CAPTURE_COUNTS, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_ERR_CLEAR, CFG_EMP_ERR_CLEAR, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_ERR_CLEAR, CFG_EMP_ERR_CLEAR, HI_FALSE);

    hdmirx_hal_sw_pkt_init(port);
    hdmirx_hal_hw_pkt_init(port);
    hdmirx_hal_capture_pkt_init(port);

    hdmirx_hal_reg_write_fld_align(port, REG_VBLANK_VIC, CFG_VBLANK_VIC, 0x100); /* 0x100 : ovr cfg vlank vic */
    hdmirx_hal_reg_write_fld_align(port, REG_EMP_CFG_EN, CFG_VI_PATH_BYP, HI_FALSE);

    hdmirx_hal_reg_write_fld_align(port, REG_DEPACK_INTR1_MASK, DEPACK_INTR1_STAT7_MASK, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_DEPACK_INTR2_MASK, DEPACK_INTR1_STAT7_MASK, HI_TRUE);
}

static hi_void hdmirx_hal_audio_aac_init(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_write(port, AUDIO_CFG_INT_MASK, 0x7FFFF); /* 0x7FFFF : cts intr mask */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_AAC, CFG_AUDIO_AAC_MUTE, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_MCLK_SEL, REG_MCLK4HBRA, HI_FALSE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_MCLK_SEL, REG_MCLK4DAC, HI_FALSE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_AAC, CFG_VSYNC_CNT, 0x3); /* 0x3 : vsync cnt */
}

static hi_void hdmirx_hal_audio_acr_init(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CTS_THRE, REG_CTS_THRESH, 0xA); /* 0xA : cts change threshholds */

    /* 0x3C : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_FIFO_MAX_THRESH, CFG_FIFO_MAX_COUNT_ADJ_THRE, 0x3C);
    /* 0x5 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_FIFO_MIN_THRESH, CFG_FIFO_MIN_COUNT_ADJ_THRE, 0x5);
    /* 0x4C4B40 : fifo check time */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_FIFO_TIME_ADJ, CFG_FIFO_CHECK_TIME, 0x4C4B40);
    /* 0x14 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_FIFO_ADJ_DELTA0, CFG_FIFO_MAX_1ST_CTS_DELTA, 0x14);
    /* 0x14 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_FIFO_ADJ_DELTA0, CFG_FIFO_MAX_INC_CTS_DELTA, 0x14);
    /* 0xA : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_FIFO_ADJ_DELTA1, CFG_FIFO_MAX_UNCHANGE_CTS_DELTA, 0xA);
    /* 0x14 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_FIFO_ADJ_DELTA1, CFG_FIFO_MIN_1ST_CTS_DELTA, 0x14);
    /* 0x14 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_FIFO_ADJ_DELTA2, CFG_FIFO_MIN_DECR_CTS_DELTA, 0x14);
    /* 0xA : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_FIFO_ADJ_DELTA2, CFG_FIFO_MIN_UNCHANGE_CTS_DELTA, 0xA);
    /* 0x3E8 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_FIFO_ADJ_MAX_LIMIT, CFG_FIFO_MAX_CTS_DELTA, 0x3E8);
    /* 0x3E8 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_FIFO_ADJ_MIN_LIMIT, CFG_FIFO_MIN_CTS_DELTA, 0x3E8);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_FIFO_ADJ, CFG_FIFO_COUNT_ADJ_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_FIFO_ADJ, CFG_OBS_RP_SEL, HI_FALSE);

    hdmirx_hal_reg_write_fld_align(port, REG_CFG_ACR_CTRL, REG_ACR_INIT, HI_TRUE);
}

static hi_void hdmirx_hal_audio_output_init(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_I2S_CTRL1, CFG_STOP_SCLK_EN, HI_FALSE);

    hdmirx_hal_reg_write_fld_align(port, REG_CFG_AUD_EN, REG_SD0_EN, HI_FALSE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_AUD_EN, REG_SD1_EN, HI_FALSE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_AUD_EN, REG_SD2_EN, HI_FALSE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_AUD_EN, REG_SD3_EN, HI_FALSE);

    hdmirx_hal_reg_write_fld_align(port, REG_CFG_AAC, CFG_AUDIO_AAC_MUTE, HI_FALSE);

    hdmirx_hal_reg_write_fld_align(port, REG_CFG_UNDERUN_RST_EN, CFG_UNDERUN_RST_AUTO_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_OVERUN_RST_EN, CFG_OVERUN_RST_AUTO_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_LAYOUT_OW, CFG_LAYOUT_OW_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_LAYOUT_OW, CFG_LAYOUT, HI_FALSE);
}

static hi_void hdmirx_hal_audio_init(hi_drv_hdmirx_port port)
{
    hdmirx_hal_audio_aac_init(port);
    hdmirx_hal_audio_acr_init(port);
    hdmirx_hal_audio_output_init(port);
}

static hi_void hdmirx_hal_tmds_decoder_init(hi_drv_hdmirx_port port)
{
    /* AV Split */
    hdmirx_hal_reg_write_fld_align(port, REG_AV_SPLIT_BYP, CFG_VIDGB_BYP, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_AV_SPLIT_BYP, CFG_VIDPREAMBLE_BYP, HI_FALSE);
    hdmirx_hal_reg_write_fld_align(port, REG_AV_SPLIT_BYP, CFG_DIGB_BYP, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_AV_SPLIT_BYP, CFG_DIPREAMBLE_BYP, HI_FALSE);

    /* Hdmi Mode */
    hdmirx_hal_reg_write_fld_align(port, REG_HDMI_MODE_CTL, CFG_HDMI_MODE_SW_VALUE, HI_FALSE);
    hdmirx_hal_reg_write_fld_align(port, REG_HDMI_MODE_CTL, CFG_HDMI_MODE_OVERWRITE, HI_FALSE);
    hdmirx_hal_reg_write_fld_align(port, REG_HDMI_MODE_CTL, CFG_TERC_DET_SEL, HI_FALSE);
    hdmirx_hal_reg_write_fld_align(port, REG_HDMI_MODE_CTL, CFG_HDMI_TERC4_CNT, 0x14); /* 0x14 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_HDMI_MODE_CTL, CFG_HDMI_LOST_CNT, 0x8); /* 0x8 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_HDMI_MODE_CTL, CFG_HDMI_MODE_CLR_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_HDMI_MODE_CTL, CFG_DIGB_SEL, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_HDMI_MODE_CTL, DECODER_CFG_HDMI_MODE_SEL, HI_TRUE);

    /* Terc4 err */
    hdmirx_hal_reg_write_fld_align(port, REG_TERC_THRESHOLD_CFG, CFG_T4_CORR_THRESHOLD, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_TERC_THRESHOLD_CFG, CFG_T4_UNCORR_THRESHOLD, HI_TRUE);
}

static hi_void hdmirx_hal_set_video_det_en(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CHANGED_DET_EN, CFG_HSYNC_POLARITY_CHANGED_DET_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CHANGED_DET_EN, CFG_VSYNC_POLARITY_CHANGED_DET_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CHANGED_DET_EN, CFG_INTERLACE_CHANGED_DET_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CHANGED_DET_EN, CFG_HACTIVE_CHANGED_DET_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CHANGED_DET_EN, CFG_HSYNC_CHANGED_DET_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CHANGED_DET_EN, CFG_HTOTAL_CHANGED_DET_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CHANGED_DET_EN, CFG_HFRONT_CHANGED_DET_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CHANGED_DET_EN, CFG_HBACK_CHANGED_DET_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CHANGED_DET_EN, CFG_VACTIVE_CHANGED_DET_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CHANGED_DET_EN, CFG_VSYNC_EVEN_CHANGED_DET_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CHANGED_DET_EN, CFG_VSYNC_ODD_CHANGED_DET_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CHANGED_DET_EN, CFG_VTOTAL_EVEN_CHANGED_DET_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CHANGED_DET_EN, CFG_VTOTAL_ODD_CHANGED_DET_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CHANGED_DET_EN, CFG_VFRONT_EVEN_CHANGED_DET_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CHANGED_DET_EN, CFG_VFRONT_ODD_CHANGED_DET_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CHANGED_DET_EN, CFG_VBACK_EVEN_CHANGED_DET_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CHANGED_DET_EN, CFG_VBACK_ODD_CHANGED_DET_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CHANGED_DET_EN, CFG_FRAME_RATE_CHANGED_DET_EN, HI_TRUE);
}

static hi_void hdmirx_hal_set_video_change_thresh(hi_drv_hdmirx_port port)
{
    /* 0x4 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CHANGED_THRESHOLD, CFG_HACTIVE_CHANGED_THRESHOLD, 0x4);
    /* 0x4 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CHANGED_THRESHOLD, CFG_VACTIVE_CHANGED_THRESHOLD, 0x4);
    /* 0x4 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CHANGED_THRESHOLD, CFG_INTERLACE_CHANGED_THRESHOLD, 0x4);
    /* 0x4 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CHANGED_THRESHOLD, CFG_V_CHANGED_THRESHOLD, 0x4);
    /* 0x5 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_H_THRESHOLD, CFG_HBACK_DELTA_THRESHOLD, 0x5);
    /* 0x5 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_H_THRESHOLD, CFG_HFRONT_DELTA_THRESHOLD, 0x5);
    /* 0x5 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_H_THRESHOLD, CFG_HSYNC_DELTA_THRESHOLD, 0x5);
    /* 0x5 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_H_THRESHOLD, CFG_HACTIVE_DELTA_THRESHOLD, 0x5);
    /* 0x5 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_HTOTAL_THRESHOLD, CFG_HTOTAL_DELTA_THRESHOLD, 0x5);
    /* 0x2 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_V_THRESHOLD, CFG_VSYNC_ODD_DELTA_THRESHOLD, 0x2);
    /* 0x2 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_V_THRESHOLD, CFG_VSYNC_EVEN_DELTA_THRESHOLD, 0x2);
    /* 0x2 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_V_THRESHOLD, CFG_VACTIVE_DELTA_THRESHOLD, 0x2);
    /* 0x2 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_VFRONT_EVEN_THRESHOLD, CFG_VFRONT_EVEN_DELTA_THRESHOLD, 0x2);
    /* 0x2 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_VFRONT_ODD_THRESHOLD, CFG_VFRONT_ODD_DELTA_THRESHOLD, 0x2);
    /* 0x2 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_VBACK_THRESHOLD, CFG_VBACK_ODD_DELTA_THRESHOLD, 0x2);
    /* 0x2 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_VBACK_THRESHOLD, CFG_VBACK_EVEN_DELTA_THRESHOLD, 0x2);
    /* 0x2 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_VTOTAL_EVEN_THRESHOLD, CFG_VTOTAL_EVEN_DELTA_THRESHOLD, 0x2);
    /* 0x2 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_VTOTAL_ODD_THRESHOLD, CFG_VTOTAL_ODD_DELTA_THRESHOLD, 0x2);
    /* 0x10 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_FRAME_RATE_THRESHOLD, CFG_FRAME_RATE_DELTA_THRESHOLD, 0x10);
}

static hi_void hdmirx_hal_video_init(hi_drv_hdmirx_port port)
{
    /* don't bypass deepcolor */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CORE_ISO_EN, CFG_CORE_ISO_EN, HI_FALSE);
    /* don't force soft phase */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_DEFAULT_PHASE_CTRL, CFG_DEFAULT_PHASE_EN, HI_FALSE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_DEFAULT_PHASE_CTRL, CFG_PHASE_INC_VALUE, HI_FALSE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_DEFAULT_PHASE_CTRL, CFG_SOFT_PHASE_EN, HI_FALSE);
    /* sync configuration init */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_SYNC_PROCESS, CFG_SYNC_POLARITY_FORCE, HI_FALSE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_SYNC_PROCESS, CFG_SYNC_NORMALIZE_EN, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_SYNC_PROCESS, CFG_VSYNC_POLARITY_VALUE, HI_FALSE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_SYNC_PROCESS, CFG_HSYNC_POLARITY_VALUE, HI_FALSE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_SYNC_PROCESS, CFG_VSYNC_INVERT_EN, HI_FALSE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_SYNC_PROCESS, CFG_HSYNC_INVERT_EN, HI_FALSE);
    /* interlace detect configuration init : hsync mode */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_INTERLACE_CTRL, CFG_INTERLACE_OPT, HI_FALSE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_INTERLACE_CTRL, CFG_VSYNC_DLY, 0x2); /* 0x2 : default value */
    /* interlace force configuration init : logic detect mode */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_INTERLACE_CTRL, CFG_FIELD_MODE, HI_FALSE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_INTERLACE_CTRL, CFG_EVEN_VALUE, HI_FALSE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_INTERLACE_CTRL, CFG_ODD_VALUE, HI_TRUE);
    /* test pattern : false */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_TIMING_SEL, CFG_EXTMODE, HI_FALSE);

    hdmirx_hal_set_video_change_thresh(port);

    hdmirx_hal_reg_write_fld_align(port, REG_PREAMBLE_CNT_CFG, CFG_VIDPREAMBLE_CNT, 0x6); /* 0x6 : default value */
    hdmirx_hal_reg_write_fld_align(port, REG_PREAMBLE_CNT_CFG, CFG_DIPREAMBLE_CNT, 0x6); /* 0x6 : default value */

    hdmirx_hal_set_video_det_en(port);

    hdmirx_hal_reg_write(port, VIDEO_IRQ_MASK, 0x7FFFFF); /* 0x7FFFFF : video intr mask */

    hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_SRST, CFG_VID_FIFO_SRST_REQ, HI_TRUE);
    udelay(1);
    hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_SRST, CFG_VID_FIFO_SRST_REQ, HI_FALSE);

    /* video mute mode : frame mute */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_MUTE_CTRL, CFG_MUTE_MODE, HI_TRUE);

    /* enable crc */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CRC_EN, CFG_CRC_EN, HI_TRUE);
}

static hi_void hdmirx_hal_sys_init(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_SEL, CKSEL_AUD_MCLK, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_AUD_EVENT_MASK, CFG_VID_RSLT_MASK_FOR_AUDIO_MUTE, HI_FALSE);
    hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_SEL, CFG_DPHY_CHANNEL_FIFO_RST_LANE_EN, HI_TRUE);
}

static hi_void hdmirx_hal_hdcp1x_init(hi_drv_hdmirx_port port)
{
    /* hdcp1x bcaps init */
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_BCAPS_SET, CFG_HDMI_CAPABLE, HI_TRUE);
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_BCAPS_SET, CFG_FAST, HI_FALSE);
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_BCAPS_SET, CFG_1P1_FEATURE, HI_FALSE);
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_BCAPS_SET, CFG_FAST_REAUTH, HI_FALSE);
    /* hdcp1x ainfo force enable : false */
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_AINFO_OVERWRITE, CFG_AINFO_OVERWRITE_EN, HI_FALSE);
    /* hdcp1x advance cipher force enable : false */
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_AC_EN, CFG_AC_EN, HI_FALSE);
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_AC_EN, CFG_HDMI_MODE_VAL, HI_FALSE);
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_AC_EN, HDCP_CFG_HDMI_MODE_SEL, HI_FALSE);
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_AC_EN, CFG_AC_SEL, HI_FALSE);
    /* hdcp1x pj force enable : false */
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_PJ_EN, CFG_PJ_SEL, HI_FALSE);
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_PJ_EN, CFG_PJ_EN, HI_FALSE);
    /* hdcp1x re-authentication mode selection : immediate */
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_REAUTH_SEL, CFG_REAUTH_SEL, HI_FALSE);
    /* hdcp1x Stop decryption : disable */
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_STOP_ENC, CFG_STOP_ENC, HI_FALSE);
    /* hdcp1x mute function affects ctl3 : enable */
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_MUTE_VLD, CFG_MUTE_VLD, HI_TRUE);
    /* hdcp1x ctl3 source selection : hdcp_ctl3 */
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_CTL3_SEL, CFG_DBG_CTL3_SEL, HI_FALSE);
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_STOP_ENC, CFG_STOP_ENC, HI_FALSE);
    /* hdcp1x dvi cfg */
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_DVI_HDCP, CFG_DVI_HDCP_FORCE_EN, HI_FALSE);
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_DVI_HDCP, CFG_DVI_HDCP_MODE, HI_TRUE);
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_DVI_HDCP, CFG_OESS_SEL, HI_TRUE);
    /* hdcp1x dvi cfg */
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_FILTER_CTL, CFG_VS_FILTER_SEL, 0x2); /* 0x2 : default value */
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_FILTER_CTL, CFG_VS_FILTER_BYPASS, HI_FALSE);
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_FILTER_CTL, CFG_HS_FILTER_SEL, 0x2); /* 0x2 : default value */
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_FILTER_CTL, CFG_HS_FILTER_BYPASS, HI_FALSE);
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_FILTER_CTL, CFG_CTL_FILTER_SEL, 0x3); /* 0x3 : default value */
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_FILTER_CTL, CFG_CTL_FILTER_BYPASS, HI_FALSE);
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_FILTER_CTL, CFG_DIDE_FILTER_SEL, HI_TRUE);
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_FILTER_CTL, CFG_DIDE_FILTER_BYPASS, HI_FALSE);
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_FILTER_CTL, CFG_VIDDE_FILTER_SEL, HI_TRUE);
    hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_FILTER_CTL, CFG_VIDDE_FILTER_BYPASS, HI_FALSE);
}

static hi_void hdmirx_hal_hdcp2x_init(hi_drv_hdmirx_port port)
{
    /* hdcp2x re-authentication init */
    hdmirx_hal_hdcp2x_reg_write_fld_align(port, REG_HDCP2X_DDC_DBG_CTRL, CFG_REAUTH_REQ_CLR_MODE, HI_TRUE);
    hdmirx_hal_hdcp2x_reg_write_fld_align(port, REG_HDCP2X_DDC_DBG_CTRL, CFG_SND_FIFO_CLR_MODE, HI_TRUE);
    hdmirx_hal_hdcp2x_reg_write_fld_align(port, REG_HDCP2X_DDC_DBG_CTRL, CFG_REAUTH_REQ_DIS, HI_FALSE);
    hdmirx_hal_hdcp2x_reg_write_fld_align(port, REG_HDCP2X_DDC_DBG_CTRL, CFG_MSG_PROT_EN, HI_FALSE);
    /* hdcp2x cipher init */
    /* 0x5 Stop decryption enable cfg: bit 0 AuthDone status;bit1 HDCP1.4 mode;bit2 Non-encrypted frame mode */
    hdmirx_hal_hdcp2x_reg_write_fld_align(port, REG_HDCP2X_CIPHER_CTRL, CFG_DIS_CIPH_MODE, 0x5);
    hdmirx_hal_hdcp2x_reg_write_fld_align(port, REG_HDCP2X_CIPHER_CTRL, CFG_HDCP_MODE_SYN_EN, HI_TRUE);
    hdmirx_hal_hdcp2x_reg_write_fld_align(port, REG_HDCP2X_CIPHER_CTRL, CFG_AUTH_DONE_SYN_EN, HI_TRUE);
    /* hdcp2x ecc check mode init */
    hdmirx_hal_hdcp2x_reg_write_fld_align(port, REG_ECC_CHECK_MODE, ECC_CHECK_MODE, 0x4); /* 0x4 ecc check mode */
    hdmirx_hal_hdcp2x_reg_write_fld_align(port, REG_ECC_CHECK_MODE, ECC_CHECK_FRAME_MODE, HI_FALSE);
}

static hi_void hdmirx_hal_set_video_h_timing_changed_ref(hi_drv_hdmirx_port port,
    hdmirx_ctrl_video_timing_change_type type, hdmirx_ctrl_video_timing_change_ref ref, hi_bool en)
{
    if (en == HI_TRUE) {
        switch (type) {
            case HDMIRX_TIMING_HACTIVE_CHANGE:
                hdmirx_hal_reg_write_fld_align(port, REG_CFG_HACTIVE_CHANGED_REF, CFG_HACTIVE_CHANGED_REF,
                    ref.hactive_ref);
                break;
            case HDMIRX_TIMING_HSYNC_CHANGE:
                hdmirx_hal_reg_write_fld_align(port, REG_CFG_HSYNC_CHANGED_REF, CFG_HSYNC_CHANGED_REF, ref.hsync_ref);
                break;
            case HDMIRX_TIMING_HFRONT_CHANGE:
                hdmirx_hal_reg_write_fld_align(port, REG_CFG_HFRONT_CHANGED_REF, CFG_HFRONT_CHANGED_REF,
                    ref.hfront_ref);
                break;
            case HDMIRX_TIMING_HBACK_CHANGE:
                hdmirx_hal_reg_write_fld_align(port, REG_CFG_HBACK_CHANGED_REF, CFG_HBACK_CHANGED_REF,
                    ref.hback_ref);
                break;
            case HDMIRX_TIMING_HTOTAL_CHANGE:
                hdmirx_hal_reg_write_fld_align(port, REG_CFG_HTOTAL_CHANGED_REF, CFG_HTOTAL_CHANGED_REF,
                    ref.htotal_ref);
                break;
            default:
                break;
        }
    }
}

static hi_void hdmirx_hal_set_video_v_timing_changed_ref(hi_drv_hdmirx_port port,
    hdmirx_ctrl_video_timing_change_type type, hdmirx_ctrl_video_timing_change_ref ref, hi_bool en)
{
    if (en == HI_TRUE) {
        switch (type) {
            case HDMIRX_TIMING_VACTIVE_CHANGE:
                hdmirx_hal_reg_write_fld_align(port, REG_CFG_VACTIVE_CHANGED_REF, CFG_VACTIVE_CHANGED_REF,
                    ref.hactive_ref);
                break;
            case HDMIRX_TIMING_VSYNC_EVEN_CHANGE:
                hdmirx_hal_reg_write_fld_align(port, REG_CFG_VSYNC_EVEN_CHANGED_REF, CFG_VSYNC_EVEN_CHANGED_REF,
                    ref.vsync_even_ref);
                break;
            case HDMIRX_TIMING_VSYNC_ODD_CHANGE:
                hdmirx_hal_reg_write_fld_align(port, REG_CFG_VSYNC_ODD_CHANGED_REF, CFG_VSYNC_ODD_CHANGED_REF,
                    ref.vsync_odd_ref);
                break;
            case HDMIRX_TIMING_VFRONT_EVEN_CHANGE:
                hdmirx_hal_reg_write_fld_align(port, REG_CFG_VFRONT_EVEN_CHANGED_REF, CFG_VFRONT_EVEN_CHANGED_REF,
                    ref.vfront_even_ref);
                break;
            case HDMIRX_TIMING_VFRONT_ODD_CHANGE:
                hdmirx_hal_reg_write_fld_align(port, REG_CFG_VFRONT_ODD_CHANGED_REF, CFG_VFRONT_ODD_CHANGED_REF,
                    ref.vfront_odd_ref);
                break;
            case HDMIRX_TIMING_VBACK_EVEN_CHANGE:
                hdmirx_hal_reg_write_fld_align(port, REG_CFG_VBACK_EVEN_CHANGED_REF, CFG_VBACK_EVEN_CHANGED_REF,
                    ref.vback_even_ref);
                break;
            case HDMIRX_TIMING_VBACK_ODD_CHANGE:
                hdmirx_hal_reg_write_fld_align(port, REG_CFG_VBACK_ODD_CHANGED_REF, CFG_VBACK_ODD_CHANGED_REF,
                    ref.vback_odd_ref);
                break;
            case HDMIRX_TIMING_VTOTAL_EVEN_CHANGE:
                hdmirx_hal_reg_write_fld_align(port, REG_CFG_VTOTAL_EVEN_CHANGED_REF, CFG_VTOTAL_EVEN_CHANGED_REF,
                    ref.vtotal_even_ref);
                break;
            case HDMIRX_TIMING_VTOTAL_ODD_CHANGE:
                hdmirx_hal_reg_write_fld_align(port, REG_CFG_VTOTAL_ODD_CHANGED_REF, CFG_VTOTAL_ODD_CHANGED_REF,
                    ref.vtotal_odd_ref);
                break;
            default:
                break;
        }
    }
}

static hi_void hdmirx_hal_set_video_timing_changed_det_en(hi_drv_hdmirx_port port,
    hdmirx_ctrl_video_timing_change_type type, hi_bool en)
{
    hi_u32 reg_offset;
    hi_u32 first_offset = CFG_HSYNC_POLARITY_CHANGED_DET_EN;

    if (type < HDMIRX_TIMING_CHANGE_MAX) {
        reg_offset = first_offset << type;
        hdmirx_hal_reg_write_fld_align(port, REG_CFG_CHANGED_DET_EN, reg_offset, en);
    }
}

static hi_void hdmirx_hal_set_video_timing_changed_det_ref(hi_drv_hdmirx_port port,
    hdmirx_ctrl_video_timing_change_type type, hdmirx_ctrl_video_timing_change_ref ref, hi_bool en)
{
    switch (type) {
        case HDMIRX_TIMING_HACTIVE_CHANGE:
        case HDMIRX_TIMING_HSYNC_CHANGE:
        case HDMIRX_TIMING_HFRONT_CHANGE:
        case HDMIRX_TIMING_HBACK_CHANGE:
        case HDMIRX_TIMING_HTOTAL_CHANGE:
            hdmirx_hal_set_video_h_timing_changed_ref(port, type, ref, en);
            break;
        case HDMIRX_TIMING_VACTIVE_CHANGE:
        case HDMIRX_TIMING_VSYNC_EVEN_CHANGE:
        case HDMIRX_TIMING_VSYNC_ODD_CHANGE:
        case HDMIRX_TIMING_VFRONT_EVEN_CHANGE:
        case HDMIRX_TIMING_VFRONT_ODD_CHANGE:
        case HDMIRX_TIMING_VBACK_EVEN_CHANGE:
        case HDMIRX_TIMING_VBACK_ODD_CHANGE:
        case HDMIRX_TIMING_VTOTAL_EVEN_CHANGE:
        case HDMIRX_TIMING_VTOTAL_ODD_CHANGE:
            hdmirx_hal_set_video_v_timing_changed_ref(port, type, ref, en);
            break;
        case HDMIRX_TIMING_FRAME_RATE_CHANGE:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_FRAME_RATE_CHANGED_REF, CFG_FRAME_RATE_CHANGED_REF,
                ref.frame_rate_ref);
            break;
        case HDMIRX_TIMING_INTERLACE_CHANGE:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_INTERLACE_SYNC_REF, CFG_INTERLACE_CHANGED_REF,
                ref.interlace_ref);
            break;
        case HDMIRX_TIMING_VSYNC_POL_CHANGE:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_INTERLACE_SYNC_REF, CFG_VSYNC_POLARITY_CHANGED_REF,
                ref.vsync_pol_ref);
            break;
        case HDMIRX_TIMING_HSYNC_POL_CHANGE:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_INTERLACE_SYNC_REF, CFG_HSYNC_POLARITY_CHANGED_REF,
                ref.hsync_pol_ref);
            break;
        default:
            break;
    }
}

static hi_u32 hdmirx_hal_get_depack_intr_status(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 max_len)
{
    hi_u32 reg_addr, cnt;
    hi_u32 len = 5; /* 5 registers */
    hi_u32 first_addr = REG_DEPACK_INTR1;

    if (len > max_len) {
        len = max_len;
    }

    for (cnt = 0; cnt < len; cnt++) {
        reg_addr = first_addr + (cnt * 8); /* 8 : Offset 2 registers, 8 bytes */
        *(intr + cnt) = hdmirx_hal_reg_read(port, reg_addr);
    }

    return len;
}

static hi_u32 hdmirx_hal_get_video_intr_status(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 max_len)
{
    hi_u32 cnt;
    hi_u32 len = 1;

    if (len > max_len) {
        len = max_len;
    }

    for (cnt = 0; cnt < len; cnt++) {
        *(intr + cnt) = hdmirx_hal_reg_read(port, VIDEO_ORIGINAL_IRQ_STATUS);
    }

    return len;
}

static hi_u32 hdmirx_hal_get_audio_intr_status(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 max_len)
{
    hi_u32 cnt;
    hi_u32 len = 1;

    if (len > max_len) {
        len = max_len;
    }

    for (cnt = 0; cnt < len; cnt++) {
        *(intr + cnt) = hdmirx_hal_reg_read(port, REG_CFG_AUD_INT);
    }

    return len;
}

static hi_u32 hdmirx_hal_get_frl_intr_status(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 max_len)
{
    hi_u32 reg_addr, cnt;
    hi_u32 len = 5; /* 5 registers */
    hi_u32 first_addr = REG_FRL_INTSTA0;

    if (len > max_len) {
        len = max_len;
    }

    for (cnt = 0; cnt < len; cnt++) {
        reg_addr = first_addr + (cnt * 4); /* 4 : Offset 1 registers, 4 bytes */
        *(intr + cnt) = hdmirx_hal_reg_read(port, reg_addr);
    }

    return len;
}

static hi_u32 hdmirx_hal_get_hdcp2x_intr_status(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 max_len)
{
    hi_u32 cnt;
    hi_u32 len = 1;

    if (len > max_len) {
        len = max_len;
    }

    for (cnt = 0; cnt < len; cnt++) {
        *(intr + cnt) = hdmirx_hal_hdcp2x_reg_read(port, REG_HDCP2X_CTRL_INT);
    }

    return len;
}

static hi_u32 hdmirx_hal_get_hdcp1x_intr_status(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 max_len)
{
    hi_u32 cnt;
    hi_u32 len = 1;

    if (len > max_len) {
        len = max_len;
    }

    for (cnt = 0; cnt < len; cnt++) {
        *(intr + cnt) = hdmirx_hal_hdcp1x_reg_read(port, HDCP_ORIGINAL_IRQ_STATUS);
    }

    return len;
}

static hi_u32 hdmirx_hal_get_arc_intr_status(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 max_len)
{
    hi_u32 cnt;
    hi_u32 len = 1;

    if (len > max_len) {
        len = max_len;
    }

    for (cnt = 0; cnt < len; cnt++) {
        *(intr + cnt) = hdmirx_hal_reg_read_fld_align(port, REG_RX_PWD_INTR, HDMIRX_PWD_INT_ARC);
    }

    return len;
}

static hi_u32 hdmirx_hal_get_depack_emp_intr_status(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 max_len)
{
    hi_u32 reg_addr, cnt;
    hi_u32 len = 2; /* 2 registers */
    hi_u32 first_addr = REG_EMP_INTR1;

    if (len > max_len) {
        len = max_len;
    }

    for (cnt = 0; cnt < len; cnt++) {
        reg_addr = first_addr + (cnt * 8); /* 8 : Offset 2 registers, 8 bytes */
        *(intr + cnt) = hdmirx_hal_reg_read(port, reg_addr);
    }

    return len;
}

static hi_u32 hdmirx_hal_get_tmds_decoder_intr_status(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 max_len)
{
    hi_u32 cnt;
    hi_u32 len = 1;

    if (len > max_len) {
        len = max_len;
    }

    for (cnt = 0; cnt < len; cnt++) {
        *(intr + cnt) = hdmirx_hal_reg_read(port, REG_CFG_DEC_ORIGINAL_IRQ_STATUS);
    }

    return len;
}

static hi_u32 hdmirx_hal_get_dphy_intr_status(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 max_len)
{
    hi_u32 cnt;
    hi_u32 len = 1;

    if (len > max_len) {
        len = max_len;
    }

    for (cnt = 0; cnt < len; cnt++) {
        *(intr + cnt) = hdmirx_hal_dphy_reg_read(port, REG_CFG_DPHY_INT);
    }

    return len;
}

static hi_u32 hdmirx_hal_get_damix_intr_status(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 max_len)
{
    hi_u32 cnt;
    hi_u32 len = 1;

    if (len > max_len) {
        len = max_len;
    }

    for (cnt = 0; cnt < len; cnt++) {
        *(intr + cnt) = hdmirx_hal_damix_reg_read(port, REG_DAMIX_INTR_STATE);
    }

    return len;
}

static hi_void hdmirx_hal_clear_depack_intr(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 len)
{
    hi_u32 reg_addr, cnt;
    hi_u32 max_len = 5; /* 5 registers */
    hi_u32 first_addr = REG_DEPACK_INTR1;

    if (len <= max_len) {
        max_len = len;
    }

    for (cnt = 0; cnt < max_len; cnt++) {
        reg_addr = first_addr + (cnt * 8); /* 8 : Offset 2 registers, 8 bytes */
        hdmirx_hal_reg_write(port, reg_addr, intr[cnt]);
    }
}

static hi_void hdmirx_hal_clear_video_intr(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 len)
{
    hi_u32 cnt;
    hi_u32 max_len = 1;

    if (len <= max_len) {
        max_len = len;
    }

    for (cnt = 0; cnt < max_len; cnt++) {
        hdmirx_hal_reg_write(port, VIDEO_ORIGINAL_IRQ_STATUS, intr[cnt]);
    }
}

static hi_void hdmirx_hal_clear_audio_intr(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 len)
{
    hi_u32 cnt;
    hi_u32 max_len = 1;

    if (len <= max_len) {
        max_len = len;
    }

    for (cnt = 0; cnt < max_len; cnt++) {
        hdmirx_hal_reg_write(port, REG_CFG_AUD_INT, intr[cnt]);
    }
}

static hi_void hdmirx_hal_clear_frl_intr(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 len)
{
    hi_u32 reg_addr, cnt;
    hi_u32 max_len = 5; /* 5 registers */
    hi_u32 first_addr = REG_FRL_INTSTA0;

    if (len <= max_len) {
        max_len = len;
    }

    for (cnt = 0; cnt < max_len; cnt++) {
        reg_addr = first_addr + (cnt * 4); /* 4 : Offset 1 registers, 4 bytes */
        hdmirx_hal_reg_write(port, reg_addr, intr[cnt]);
    }
}

static hi_void hdmirx_hal_clear_hdcp2x_intr(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 len)
{
    hi_u32 cnt;
    hi_u32 max_len = 1;

    if (len <= max_len) {
        max_len = len;
    }

    for (cnt = 0; cnt < max_len; cnt++) {
        hdmirx_hal_hdcp2x_reg_write(port, REG_HDCP2X_CTRL_INT, intr[cnt]);
    }
}

static hi_void hdmirx_hal_clear_hdcp1x_intr(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 len)
{
    hi_u32 cnt;
    hi_u32 max_len = 1;

    if (len <= max_len) {
        max_len = len;
    }

    for (cnt = 0; cnt < max_len; cnt++) {
        hdmirx_hal_hdcp1x_reg_write(port, HDCP_ORIGINAL_IRQ_STATUS, intr[cnt]);
    }
}

static hi_void hdmirx_hal_clear_arc_intr(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 len)
{
    hi_u32 cnt;
    hi_u32 max_len = 1;

    if (len <= max_len) {
        max_len = len;
    }

    for (cnt = 0; cnt < max_len; cnt++) {
        hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_INTR, HDMIRX_PWD_INT_ARC, intr[cnt]);
    }
}

static hi_void hdmirx_hal_clear_depack_emp_intr(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 len)
{
    hi_u32 reg_addr, cnt;
    hi_u32 max_len = 2; /* 2 registers */
    hi_u32 first_addr = REG_EMP_INTR1;

    if (len <= max_len) {
        max_len = len;
    }

    for (cnt = 0; cnt < max_len; cnt++) {
        reg_addr = first_addr + (cnt * 8); /* 8 : Offset 2 registers, 8 bytes */
        hdmirx_hal_reg_write(port, reg_addr, intr[cnt]);
    }
}

static hi_void hdmirx_hal_clear_tmds_decoder_intr(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 len)
{
    hi_u32 cnt;
    hi_u32 max_len = 1;

    if (len <= max_len) {
        max_len = len;
    }

    for (cnt = 0; cnt < max_len; cnt++) {
        hdmirx_hal_reg_write(port, REG_CFG_DEC_ORIGINAL_IRQ_STATUS, intr[cnt]);
    }
}

static hi_void hdmirx_hal_clear_dphy_intr(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 len)
{
    hi_u32 cnt;
    hi_u32 max_len = 1;

    if (len <= max_len) {
        max_len = len;
    }

    for (cnt = 0; cnt < max_len; cnt++) {
        hdmirx_hal_dphy_reg_write(port, REG_CFG_DPHY_INT, intr[cnt]);
    }
}

static hi_void hdmirx_hal_clear_damix_intr(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 len)
{
    hi_u32 cnt;
    hi_u32 max_len = 1;

    if (len <= max_len) {
        max_len = len;
    }

    for (cnt = 0; cnt < max_len; cnt++) {
        hdmirx_hal_damix_reg_write(port, REG_CFG_DEC_ORIGINAL_IRQ_STATUS, intr[cnt]);
    }
}

static hi_void hdmirx_hal_clear_pwd_det_intr(hi_drv_hdmirx_port port,
    hdmirx_ctrl_pwd_intr_type type, hi_u32 *intr, hi_u32 len)
{
    hi_u32 reg_offset;
    hi_u32 value;
    hi_u32 max_len = 1;
    hi_bool flag = HI_TRUE;

    if (len <= max_len) {
        max_len = len;
    }

    if (len != 0) {
        switch (type) {
            case HDMIRX_PWD_INTR_DET_CLK0_STAT_CHG:
                reg_offset = HDMIRX_PWD_INT_DET0;
                break;
            case HDMIRX_PWD_INTR_DET_CLK1_STAT_CHG:
                reg_offset = HDMIRX_PWD_INT_DET1;
                break;
            case HDMIRX_PWD_INTR_DET_CLK2_STAT_CHG:
                reg_offset = HDMIRX_PWD_INT_DET2;
                break;
            case HDMIRX_PWD_INTR_DET_CLK3_STAT_CHG:
                reg_offset = HDMIRX_PWD_INT_DET3;
                break;
            case HDMIRX_PWD_INTR_DET_PCLK_RATIO_STAT_CHG:
                reg_offset = HDMIRX_PWD_INT_DET4;
                break;
            case HDMIRX_PWD_INTR_DET_FRL_RATE_CHG:
                reg_offset = HDMIRX_PWD_INT_DET6;
                break;
            default:
                flag = HI_FALSE;
                break;
        }
        if (flag == HI_TRUE) {
            value = *intr;
            hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_INTR, reg_offset, value);
        }
    }
}

static hi_u32 hdmirx_hal_get_cec_intr_status(hi_drv_hdmirx_port port)
{
    hi_u32 value;

    value = hdmirx_hal_aon_reg_read(port, REG_CEC_IRQ_SRC_PND);

    return value;
}

static hi_u32 hdmirx_hal_get_pwr_det_intr_status(hi_drv_hdmirx_port port)
{
    hi_u32 value;

    value = hdmirx_hal_aon_reg_read_fld_align(port, REG_PWR_DET_INT, P0_PWR5V_INT);

    return value;
}

static hi_u32 hdmirx_hal_get_single_port_det_intr_status(hi_drv_hdmirx_port port)
{
    hi_u32 value;

    value = hdmirx_hal_aon_reg_read(port, REG_RX_DET_INT);

    return value;
}

static hi_void hdmirx_hal_clear_cec_intr(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 len)
{
    hi_u32 cnt;
    hi_u32 max_len = 1;

    if (len <= max_len) {
        max_len = len;
    }

    for (cnt = 0; cnt < max_len; cnt++) {
        hdmirx_hal_aon_reg_write(port, REG_CEC_IRQ_SRC_PND, intr[cnt]);
    }
}

static hi_void hdmirx_hal_clear_pwr_det_intr(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 len)
{
    hi_u32 cnt;
    hi_u32 max_len = 1;

    if (len <= max_len) {
        max_len = len;
    }

    for (cnt = 0; cnt < max_len; cnt++) {
        hdmirx_hal_aon_reg_write_fld_align(port, REG_PWR_DET_INT, P0_PWR5V_INT, intr[cnt]);
    }
}

static hi_void hdmirx_hal_clear_single_port_det_intr(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 len)
{
    hi_u32 cnt;
    hi_u32 max_len = 1;

    if (len <= max_len) {
        max_len = len;
    }

    for (cnt = 0; cnt < max_len; cnt++) {
        hdmirx_hal_aon_reg_write(port, REG_RX_DET_INT, intr[cnt]);
    }
}

static hi_void hdmirx_hal_clear_single_intr_in_depack(hi_drv_hdmirx_port port, hdmirx_ctrl_pwd_intr_idx type)
{
    switch (type) {
        case HDMIRX_INT_CTS:
            hdmirx_hal_reg_write_fld_align(port, REG_DEPACK_INTR5, DEPACK_INTR5_STAT0, HI_TRUE);
            break;
        case HDMIRX_INT_UPDATE_AUD_PACKET:
            hdmirx_hal_reg_write_fld_align(port, REG_DEPACK_INTR2, DEPACK_INTR2_STAT1, HI_TRUE);
            break;
        case HDMIRX_INT_UPDATE_AUD:
            hdmirx_hal_reg_write_fld_align(port, REG_DEPACK_INTR2, DEPACK_INTR2_STAT9, HI_TRUE);
            break;
        case HDMIRX_INT_UPDATE_SPD:
            hdmirx_hal_reg_write_fld_align(port, REG_DEPACK_INTR2, DEPACK_INTR2_STAT8, HI_TRUE);
            break;
        case HDMIRX_INT_UPDATE_MPEG:
            hdmirx_hal_reg_write_fld_align(port, REG_DEPACK_INTR2, DEPACK_INTR2_STAT10, HI_TRUE);
            break;
        case HDMIRX_INT_NEW_ACP:
            hdmirx_hal_reg_write_fld_align(port, REG_DEPACK_INTR1, DEPACK_INTR1_STAT1, HI_TRUE);
            break;
        case HDMIRX_INT_NEW_CP:
            hdmirx_hal_reg_write_fld_align(port, REG_DEPACK_INTR1, DEPACK_INTR1_STAT0, HI_TRUE);
            break;
        case HDMIRX_INT_NEW_AVI:
            hdmirx_hal_reg_write_fld_align(port, REG_DEPACK_INTR1, DEPACK_INTR1_STAT7, HI_TRUE);
            break;
        case HDMIRX_INT_CP_MUTE:
            hdmirx_hal_reg_write_fld_align(port, REG_DEPACK_INTR4, DEPACK_INTR4_STAT3, HI_TRUE);
            break;
        case HDMIRX_INT_NO_AVI:
            hdmirx_hal_reg_write_fld_align(port, REG_DEPACK_INTR3, DEPACK_INTR3_STAT7, HI_TRUE);
            break;
        default:
            break;
    }
}

static hi_void hdmirx_hal_clear_single_intr_in_video(hi_drv_hdmirx_port port, hdmirx_ctrl_pwd_intr_idx type)
{
    switch (type) {
        case HDMIRX_INT_VRES_CHG:
            hdmirx_hal_reg_write_fld_align(port, VIDEO_ORIGINAL_IRQ_STATUS, CFG_ORIGINAL_STATUS_VACTIVE_CHANGED,
                HI_TRUE);
            break;
        default:
            break;
    }
}

static hi_void hdmirx_hal_clear_single_intr_in_audio(hi_drv_hdmirx_port port, hdmirx_ctrl_pwd_intr_idx type)
{
    switch (type) {
        case HDMIRX_INT_AUD_MUTE:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_AUD_INT, INTR_AAC_AUDIO_MUTE, HI_TRUE);
            break;
        case HDMIRX_INT_HW_CTS_CHG:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_AUD_INT, INTR_ACR_CTS_CHANGE, HI_TRUE);
            break;
        case HDMIRX_INT_CTS_DROP_ERR:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_AUD_INT, INTR_CTS_DROPPED_ERR, HI_TRUE);
            break;
        case HDMIRX_INT_CTS_REUSE_ERR:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_AUD_INT, INTR_CTS_REUSED_ERR, HI_TRUE);
            break;
        case HDMIRX_INT_AUDIO_FIFO:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_AUD_INT, INTR_FIFO_OVERUN_AF, HI_TRUE);
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_AUD_INT, INTR_FIFO_OVERUN_BF, HI_TRUE);
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_AUD_INT, INTR_FIFO_UNDERUN_AF, HI_TRUE);
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_AUD_INT, INTR_FIFO_UNDERUN_BF, HI_TRUE);
            break;
        case HDMIRX_INT_FIFO_OF:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_AUD_INT, INTR_FIFO_OVERUN_AF, HI_TRUE);
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_AUD_INT, INTR_FIFO_OVERUN_BF, HI_TRUE);
            break;
        case HDMIRX_INT_FIFO_UF:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_AUD_INT, INTR_FIFO_UNDERUN_AF, HI_TRUE);
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_AUD_INT, INTR_FIFO_UNDERUN_BF, HI_TRUE);
            break;
        default:
            break;
    }
}

static hi_void hdmirx_hal_clear_single_intr_in_dphy(hi_drv_hdmirx_port port, hdmirx_ctrl_pwd_intr_idx type)
{
    switch (type) {
        case HDMIRX_INT_SCDT:
            hdmirx_hal_dphy_reg_write_fld_align(port, REG_CFG_DPHY_INT, INTR_SCDT_CHANGE, HI_TRUE);
            break;
        default:
            break;
    }
}

static hi_void hdmirx_hal_set_pwd_auto_reset_en(hi_drv_hdmirx_port port,
    hdmirx_ctrl_auto_reset_type auto_rst, hi_bool en)
{
    switch (auto_rst) {
        case HDMIRX_AUTORST_DPHY:
            hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_SEL, CFG_DMIX_PLL_UNLOCK_RST_DPHY_EN, en);
            break;
        case HDMIRX_AUTORST_DPHY_LANE:
            hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_SEL, CFG_DPHY_CHANNEL_FIFO_RST_LANE_EN, en);
            break;
        case HDMIRX_AUTORST_AUDIO_FIFO:
            if (en == HI_TRUE) {
                hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_HW_RST_OVERRIDES,
                    CFG_AFIFO_AUTO_RST_OVERRIDE, HI_FALSE);
            } else {
                hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_HW_RST_OVERRIDES,
                    CFG_AFIFO_AUTO_RST_OVERRIDE, HI_TRUE);
            }
            break;
        case HDMIRX_AUTORST_ALL:
            hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_SEL, CFG_DMIX_PLL_UNLOCK_RST_DPHY_EN, en);
            hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_SEL, CFG_DPHY_CHANNEL_FIFO_RST_LANE_EN, en);
            if (en == HI_TRUE) {
                hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_HW_RST_OVERRIDES,
                    CFG_AFIFO_AUTO_RST_OVERRIDE, HI_FALSE);
            } else {
                hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_HW_RST_OVERRIDES,
                    CFG_AFIFO_AUTO_RST_OVERRIDE, HI_TRUE);
            }
            break;
        default:
            break;
    }
}

static hi_void hdmirx_hal_set_aon_auto_reset_en(hi_drv_hdmirx_port port,
    hdmirx_ctrl_auto_reset_type auto_rst, hi_bool en)
{
    switch (auto_rst) {
        case HDMIRX_AUTORST_DDC_TOUT:
            hdmirx_hal_aon_reg_write_fld_align(port, REG_DDC_SLV_RST, DDC_TOUT_RST_EN, en);
            break;
        case HDMIRX_AUTORST_DDC_PWR:
            hdmirx_hal_aon_reg_write_fld_align(port, REG_DDC_SLV_RST, DDC_PWR_RST_EN, en);
            break;
        case HDMIRX_AUTORST_DDC_HPD:
            hdmirx_hal_aon_reg_write_fld_align(port, REG_DDC_SLV_RST, DDC_HPD_RST_EN, en);
            break;
        case HDMIRX_AUTORST_ALL:
            hdmirx_hal_aon_reg_write_fld_align(port, REG_DDC_SLV_RST, DDC_TOUT_RST_EN, en);
            hdmirx_hal_aon_reg_write_fld_align(port, REG_DDC_SLV_RST, DDC_PWR_RST_EN, en);
            hdmirx_hal_aon_reg_write_fld_align(port, REG_DDC_SLV_RST, DDC_HPD_RST_EN, en);
            break;
        default:
            break;
    }
}

static hi_void hdmirx_hal_set_sys_event_for_video_mask_en(hi_drv_hdmirx_port port,
    hdmirx_ctrl_sys_event_type type, hi_bool en)
{
    if (type == HDMIRX_SYS_EVENT_ALL) {
        if (en == HI_TRUE) {
            hdmirx_hal_reg_write_fld_align(port, REG_SYS_EVENT_CFG_0, CFG_SYS_EVENT_MASK_FOR_VIDEO, 0x7F);
        } else {
            hdmirx_hal_reg_write_fld_align(port, REG_SYS_EVENT_CFG_0, CFG_SYS_EVENT_MASK_FOR_VIDEO, 0x0);
        }
    }
}

static hi_void hdmirx_hal_set_sys_event_for_audio_mask_en(hi_drv_hdmirx_port port,
    hdmirx_ctrl_sys_event_type type, hi_bool en)
{
    if (type == HDMIRX_SYS_EVENT_ALL) {
        if (en == HI_TRUE) {
            hdmirx_hal_reg_write_fld_align(port, REG_SYS_EVENT_CFG_0, CFG_SYS_EVENT_MASK_FOR_AUDIO, 0x7F);
        } else {
            hdmirx_hal_reg_write_fld_align(port, REG_SYS_EVENT_CFG_0, CFG_SYS_EVENT_MASK_FOR_AUDIO, 0x0);
        }
    }
}

static hi_void hdmirx_hal_set_sys_event_for_audio_unmute_mask_en(hi_drv_hdmirx_port port,
    hdmirx_ctrl_sys_event_type type, hi_bool en)
{
    if (type == HDMIRX_SYS_EVENT_ALL) {
        if (en == HI_TRUE) {
            hdmirx_hal_reg_write_fld_align(port, REG_SYS_EVENT_CFG_0, CFG_SYS_EVENT_MASK_FOR_AUDIO_UNMUTE, 0x7F);
        } else {
            hdmirx_hal_reg_write_fld_align(port, REG_SYS_EVENT_CFG_0, CFG_SYS_EVENT_MASK_FOR_AUDIO_UNMUTE, 0x0);
        }
    }
}

static hi_void hdmirx_hal_set_sys_event_for_decoder_mask_en(hi_drv_hdmirx_port port,
    hdmirx_ctrl_sys_event_type type, hi_bool en)
{
    if (type == HDMIRX_SYS_EVENT_ALL) {
        if (en == HI_TRUE) {
            hdmirx_hal_reg_write_fld_align(port, REG_SYS_EVENT_CFG_1, CFG_SYS_EVENT_MASK_FOR_DECODE, 0x7F);
        } else {
            hdmirx_hal_reg_write_fld_align(port, REG_SYS_EVENT_CFG_1, CFG_SYS_EVENT_MASK_FOR_DECODE, 0x0);
        }
    }
}

static hi_void hdmirx_hal_set_sys_event_for_depack_mask_en(hi_drv_hdmirx_port port,
    hdmirx_ctrl_sys_event_type type, hi_bool en)
{
    if (type == HDMIRX_SYS_EVENT_ALL) {
        if (en == HI_TRUE) {
            hdmirx_hal_reg_write_fld_align(port, REG_SYS_EVENT_CFG_1, CFG_SYS_EVENT_MASK_FOR_DEPACK, 0x7F);
        } else {
            hdmirx_hal_reg_write_fld_align(port, REG_SYS_EVENT_CFG_1, CFG_SYS_EVENT_MASK_FOR_DECODE, 0x0);
        }
    }
}

static hi_void hdmirx_hal_set_video_event_mask_en(hi_drv_hdmirx_port port,
    hdmirx_ctrl_video_event_type type, hi_bool en)
{
    hi_u32 reg_offset;

    hdmirx_hal_set_video_timing_changed_det_ref(port, type.timing_change_type, type.timing_change_ref, en);
    hdmirx_hal_set_video_timing_changed_det_en(port, type.timing_change_type, en);
    reg_offset = BIT0 << (type.timing_change_type);
    if (en == HI_TRUE) {
        hdmirx_hal_reg_write_fld_align(port, REG_VID_EVENT_MASK, reg_offset, HI_TRUE);
    } else {
        hdmirx_hal_reg_write_fld_align(port, REG_VID_EVENT_MASK, reg_offset, HI_FALSE);
    }
}

static hi_void hdmirx_hal_set_audio_event_mask_en(hi_drv_hdmirx_port port,
    hdmirx_ctrl_audio_event_type type, hi_bool en)
{
    if (type == HDMIRX_AUDIO_EVENT_ALL) {
        if (en == HI_TRUE) {
            hdmirx_hal_reg_write_fld_align(port, REG_AUD_EVENT_MASK_1, CFG_AUD_EVENT_MASK_UNMUTE, 0x7F);
        } else {
            hdmirx_hal_reg_write_fld_align(port, REG_AUD_EVENT_MASK_1, CFG_AUD_EVENT_MASK_UNMUTE, 0x0);
        }
    }
}

static hi_void hdmirx_hal_set_tmds_event_for_video_mask_en(hi_drv_hdmirx_port port,
    hdmirx_ctrl_tmds_event_type type, hi_bool en)
{
    if (type == HDMIRX_TMDS_EVENT_ALL) {
        if (en == HI_TRUE) {
            hdmirx_hal_reg_write_fld_align(port, REG_TMDS_EVENT_MASK, CFG_TMDS_EVENT_MASK_FOR_VIDEO, 0x7F);
        } else {
            hdmirx_hal_reg_write_fld_align(port, REG_TMDS_EVENT_MASK, CFG_TMDS_EVENT_MASK_FOR_VIDEO, 0x0);
        }
    }
}

static hi_void hdmirx_hal_set_tmds_event_for_audio_mask_en(hi_drv_hdmirx_port port,
    hdmirx_ctrl_tmds_event_type type, hi_bool en)
{
    if (type == HDMIRX_TMDS_EVENT_ALL) {
        if (en == HI_TRUE) {
            hdmirx_hal_reg_write_fld_align(port, REG_TMDS_EVENT_MASK, CFG_TMDS_EVENT_MASK_FOR_AUDIO, 0x7F);
        } else {
            hdmirx_hal_reg_write_fld_align(port, REG_TMDS_EVENT_MASK, CFG_TMDS_EVENT_MASK_FOR_AUDIO, 0x0);
        }
    }
}

static hi_void hdmirx_hal_set_tmds_event_for_audio_unmute_mask_en(hi_drv_hdmirx_port port,
    hdmirx_ctrl_tmds_event_type type, hi_bool en)
{
    if (type == HDMIRX_TMDS_EVENT_ALL) {
        if (en == HI_TRUE) {
            hdmirx_hal_reg_write_fld_align(port, REG_TMDS_EVENT_MASK, CFG_TMDS_EVENT_MASK_FOR_AUDIO_UNMUTE, 0x7F);
        } else {
            hdmirx_hal_reg_write_fld_align(port, REG_TMDS_EVENT_MASK, CFG_TMDS_EVENT_MASK_FOR_AUDIO_UNMUTE, 0x0);
        }
    }
}

static hi_void hdmirx_hal_set_depack_event_for_video_mask_en(hi_drv_hdmirx_port port,
    hdmirx_ctrl_depack_event_type type, hi_bool en)
{
    if (type == HDMIRX_DEPACK_EVENT_ALL) {
        if (en == HI_TRUE) {
            hdmirx_hal_reg_write_fld_align(port, REG_DPK_EVENT_MASK_FOR_VIDEO, CFG_DPK_EVENT_MASK_FOR_VIDEO, 0x7FFFF);
        } else {
            hdmirx_hal_reg_write_fld_align(port, REG_DPK_EVENT_MASK_FOR_VIDEO, CFG_DPK_EVENT_MASK_FOR_VIDEO, 0x0);
        }
    }
}

static hi_void hdmirx_hal_set_depack_event_for_audio_mask_en(hi_drv_hdmirx_port port,
    hdmirx_ctrl_depack_event_type type, hi_bool en)
{
    if (type == HDMIRX_DEPACK_EVENT_ALL) {
        if (en == HI_TRUE) {
            hdmirx_hal_reg_write_fld_align(port, REG_DPK_EVENT_MASK_FOR_AUDIO, CFG_DPK_EVENT_MASK_FOR_AUDIO, 0x7FFFF);
        } else {
            hdmirx_hal_reg_write_fld_align(port, REG_DPK_EVENT_MASK_FOR_AUDIO, CFG_DPK_EVENT_MASK_FOR_AUDIO, 0x0);
        }
    }
}

static hi_void hdmirx_hal_set_depack_event_for_audio_unmute_mask_en(hi_drv_hdmirx_port port,
    hdmirx_ctrl_depack_event_type type, hi_bool en)
{
    if (type == HDMIRX_DEPACK_EVENT_ALL) {
        if (en == HI_TRUE) {
            hdmirx_hal_reg_write_fld_align(port, REG_DPK_EVENT_MASK_FOR_AUD_UNMUTE,
                CFG_DPK_EVENT_MASK_FOR_AUDIO_UNMUTE, 0x7FFFF);
        } else {
            hdmirx_hal_reg_write_fld_align(port, REG_DPK_EVENT_MASK_FOR_AUD_UNMUTE,
                CFG_DPK_EVENT_MASK_FOR_AUDIO_UNMUTE, 0x0);
        }
    }
}

hi_void hal_ctrl_init(hi_drv_hdmirx_port port)
{
    hdmirx_hal_pwd_init(port);
    hdmirx_hal_aon_init(port);
    hdmirx_hal_depack_init(port);
    hdmirx_hal_audio_init(port);
    hdmirx_hal_tmds_decoder_init(port);
    hdmirx_hal_video_init(port);
    hdmirx_hal_sys_init(port);
    hdmirx_hal_hdcp1x_init(port);
    hdmirx_hal_hdcp2x_init(port);
}

hi_void hal_ctrl_set_pwd_iso_en(hi_drv_hdmirx_port port, hi_bool en)
{
    hdmirx_hal_aon_reg_write_fld_align(port, REG_RX_ISO_EN, PWD_ISO_EN, en);
}

hi_void hal_ctrl_set_hpd_det_mode(hi_drv_hdmirx_port port, hi_u32 mode)
{
    hdmirx_hal_aon_reg_write_fld_align(port, REG_RX_HPD_CTRL, HPD_DET_MODE, mode);
}

hi_void hal_ctrl_set_hpd_low_thresh(hi_drv_hdmirx_port port, hi_u32 thresh)
{
    hdmirx_hal_aon_reg_write_fld_align(port, REG_RX_HPD_CTRL, HPD_LOW_RESH, thresh);
}

hi_bool hal_ctrl_get_5v_status(hi_drv_hdmirx_port port)
{
    hi_u32 value;

    value = hdmirx_hal_aon_reg_read_fld_align(port, REG_RX_PWR_ST, PWR5V_DET_ST);
    value = value & BIT0;
    return (hi_bool)value;
}

hi_void hal_ctrl_set_hpd_pol(hi_drv_hdmirx_port port, hi_bool invert)
{
    hi_u32 hpd_det_mode;

    if (invert == HI_TRUE) {
        hpd_det_mode = 0x3; /* 3 : IO level inversion mode */
    } else {
        hpd_det_mode = 0x2; /* 2 : IO level mode */
    }
    hdmirx_hal_aon_reg_write_fld_align(port, REG_RX_HPD_CTRL, HPD_POL_CTL, invert);
    hdmirx_hal_aon_reg_write_fld_align(port, REG_RX_HPD_CTRL, HPD_DET_MODE, hpd_det_mode);
}

hi_void hal_ctrl_set_hpd_level(hi_drv_hdmirx_port port, hi_bool high)
{
    hdmirx_hal_aon_reg_write_fld_align(port, REG_RX_HPD_CFG, RX_HPD_DATA, high);
    hdmirx_hal_aon_reg_write_fld_align(port, REG_RX_HPD_CFG, RX_HPD_OEN, HI_FALSE);
}

hi_void hal_ctrl_set_video_intr_en(hi_drv_hdmirx_port port,
    hdmirx_ctrl_video_event_type type, hi_bool en)
{
    hdmirx_hal_set_video_timing_changed_det_ref(port, type.timing_change_type, type.timing_change_ref, en);
    hdmirx_hal_set_video_timing_changed_det_en(port, type.timing_change_type, en);
}

hi_u32 hal_ctrl_get_pwd_interrupt(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 max_len)
{
    hi_u32 cnt;
    hi_u32 len = 1;

    if (len > max_len) {
        len = max_len;
    }

    for (cnt = 0; cnt < len; cnt++) {
        *(intr + cnt) = hdmirx_hal_reg_read(port, REG_RX_PWD_INTR);
    }

    return len;
}

hi_u32 hal_ctrl_get_pwd_intr_status(hi_drv_hdmirx_port port, hdmirx_ctrl_pwd_intr_type type,
    hi_u32 *intr, hi_u32 max_len)
{
    hi_u32 len = 0;

    switch (type) {
        case HDMIRX_PWD_INTR_DEPACK:
            len = hdmirx_hal_get_depack_intr_status(port, intr, max_len);
            break;
        case HDMIRX_PWD_INTR_VIDEO:
            len = hdmirx_hal_get_video_intr_status(port, intr, max_len);
            break;
        case HDMIRX_PWD_INTR_AUDIO:
            len = hdmirx_hal_get_audio_intr_status(port, intr, max_len);
            break;
        case HDMIRX_PWD_INTR_FRL:
            len = hdmirx_hal_get_frl_intr_status(port, intr, max_len);
            break;
        case HDMIRX_PWD_INTR_HDCP2X:
            len = hdmirx_hal_get_hdcp2x_intr_status(port, intr, max_len);
            break;
        case HDMIRX_PWD_INTR_HDCP1X:
            len = hdmirx_hal_get_hdcp1x_intr_status(port, intr, max_len);
            break;
        case HDMIRX_PWD_INTR_ARC:
            len = hdmirx_hal_get_arc_intr_status(port, intr, max_len);
            break;
        case HDMIRX_PWD_INTR_DEPACK_EMP:
            len = hdmirx_hal_get_depack_emp_intr_status(port, intr, max_len);
            break;
        case HDMIRX_PWD_INTR_TMDS_DECODER:
            len = hdmirx_hal_get_tmds_decoder_intr_status(port, intr, max_len);
            break;
        case HDMIRX_PWD_INTR_DPHY:
            len = hdmirx_hal_get_dphy_intr_status(port, intr, max_len);
            break;
        case HDMIRX_PWD_INTR_DAMIX:
            len = hdmirx_hal_get_damix_intr_status(port, intr, max_len);
            break;
        default:
            break;
    }

    return len;
}

hi_void hal_ctrl_clear_pwd_interrupt(hi_drv_hdmirx_port port, hdmirx_ctrl_pwd_intr_type type, hi_u32 *intr, hi_u32 len)
{
    switch (type) {
        case HDMIRX_PWD_INTR_DEPACK:
            hdmirx_hal_clear_depack_intr(port, intr, len);
            break;
        case HDMIRX_PWD_INTR_VIDEO:
            hdmirx_hal_clear_video_intr(port, intr, len);
            break;
        case HDMIRX_PWD_INTR_AUDIO:
            hdmirx_hal_clear_audio_intr(port, intr, len);
            break;
        case HDMIRX_PWD_INTR_FRL:
            hdmirx_hal_clear_frl_intr(port, intr, len);
            break;
        case HDMIRX_PWD_INTR_HDCP2X:
            hdmirx_hal_clear_hdcp2x_intr(port, intr, len);
            break;
        case HDMIRX_PWD_INTR_HDCP1X:
            hdmirx_hal_clear_hdcp1x_intr(port, intr, len);
            break;
        case HDMIRX_PWD_INTR_ARC:
            hdmirx_hal_clear_arc_intr(port, intr, len);
            break;
        case HDMIRX_PWD_INTR_DEPACK_EMP:
            hdmirx_hal_clear_depack_emp_intr(port, intr, len);
            break;
        case HDMIRX_PWD_INTR_TMDS_DECODER:
            hdmirx_hal_clear_tmds_decoder_intr(port, intr, len);
            break;
        case HDMIRX_PWD_INTR_DPHY:
            hdmirx_hal_clear_dphy_intr(port, intr, len);
            break;
        case HDMIRX_PWD_INTR_DAMIX:
            hdmirx_hal_clear_damix_intr(port, intr, len);
            break;
        case HDMIRX_PWD_INTR_DET_CLK0_STAT_CHG:
        case HDMIRX_PWD_INTR_DET_CLK1_STAT_CHG:
        case HDMIRX_PWD_INTR_DET_CLK2_STAT_CHG:
        case HDMIRX_PWD_INTR_DET_CLK3_STAT_CHG:
        case HDMIRX_PWD_INTR_DET_PCLK_RATIO_STAT_CHG:
        case HDMIRX_PWD_INTR_DET_FRL_RATE_CHG:
            hdmirx_hal_clear_pwd_det_intr(port, type, intr, len);
            break;
        default:
            break;
    }
}

hi_u32 hal_ctrl_get_aon_interrupt(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 max_len)
{
    hi_u32 cnt;
    hi_u32 len = 1;

    if (len > max_len) {
        len = max_len;
    }

    for (cnt = 0; cnt < len; cnt++) {
        *(intr + cnt) = hdmirx_hal_aon_reg_read(port, REG_RX_AON_INTR_ST);
    }

    return len;
}

hi_u32 hal_ctrl_get_aon_intr_status(hi_drv_hdmirx_port port,
    hdmirx_ctrl_aon_intr_type type, hi_u32 *intr, hi_u32 max_len)
{
    hi_u32 len = 1;

    if (len > max_len) {
        len = max_len;
    }

    if (len != 0) {
        switch (type) {
            case HDMIRX_AON_INT_CEC:
                *intr = hdmirx_hal_get_cec_intr_status(port);
                break;
            case HDMIRX_AON_INT_PWR:
                *intr = hdmirx_hal_get_pwr_det_intr_status(port);
                break;
            case HDMIRX_AON_INT_P0_DET:
            case HDMIRX_AON_INT_P1_DET:
            case HDMIRX_AON_INT_P2_DET:
            case HDMIRX_AON_INT_P3_DET:
                *intr = hdmirx_hal_get_single_port_det_intr_status(port);
                break;
            default:
                break;
            }
    }

    return len;
}

hi_void hal_ctrl_clear_aon_interrupt(hi_drv_hdmirx_port port, hdmirx_ctrl_aon_intr_type type, hi_u32 *intr, hi_u32 len)
{
    switch (type) {
        case HDMIRX_AON_INT_CEC:
            hdmirx_hal_clear_cec_intr(port, intr, len);
            break;
        case HDMIRX_AON_INT_PWR:
            hdmirx_hal_clear_pwr_det_intr(port, intr, len);
            break;
        case HDMIRX_AON_INT_P0_DET:
        case HDMIRX_AON_INT_P1_DET:
        case HDMIRX_AON_INT_P2_DET:
        case HDMIRX_AON_INT_P3_DET:
            hdmirx_hal_clear_single_port_det_intr(port, intr, len);
            break;
        default:
            break;
    }
}

hi_void hal_ctrl_clear_interrupt(hi_drv_hdmirx_port port, hdmirx_ctrl_pwd_intr_idx type)
{
    switch (type) {
        case HDMIRX_INT_CTS:
        case HDMIRX_INT_UPDATE_AUD_PACKET:
        case HDMIRX_INT_UPDATE_AUD:
        case HDMIRX_INT_UPDATE_SPD:
        case HDMIRX_INT_UPDATE_MPEG:
        case HDMIRX_INT_NEW_ACP:
        case HDMIRX_INT_NEW_CP:
        case HDMIRX_INT_NEW_AVI:
        case HDMIRX_INT_CP_MUTE:
        case HDMIRX_INT_NO_AVI:
            hdmirx_hal_clear_single_intr_in_depack(port, type);
            break;
        case HDMIRX_INT_VRES_CHG:
            hdmirx_hal_clear_single_intr_in_video(port, type);
            break;
        case HDMIRX_INT_AUD_MUTE:
        case HDMIRX_INT_HW_CTS_CHG:
        case HDMIRX_INT_CTS_DROP_ERR:
        case HDMIRX_INT_CTS_REUSE_ERR:
        case HDMIRX_INT_AUDIO_FIFO:
        case HDMIRX_INT_FIFO_OF:
        case HDMIRX_INT_FIFO_UF:
            hdmirx_hal_clear_single_intr_in_audio(port, type);
            break;
        case HDMIRX_INT_SCDT:
            hdmirx_hal_clear_single_intr_in_dphy(port, type);
            break;
        default:
            break;
    }
}

hi_void hal_ctrl_clear_t4_error(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_write_fld_align(port, REG_RX_ECC_CTRL, CFG_CAPTURE_COUNTS, HI_TRUE);
}

hi_bool hal_ctrl_get_ckdt(hi_drv_hdmirx_port port)
{
    hi_u32 exist;
    hi_u32 flag = 0;

    exist = hdmirx_hal_reg_read_fld_align(port, REG_RX_PWD_CLK_DET_TMDS_STAT, DET_CLK0_FEAT_EXIST);
    if (exist == HI_TRUE) {
        flag = hdmirx_hal_reg_read_fld_align(port, REG_RX_PWD_CLK_DET_TMDS_STAT, DET_CLK0_STAT);
    }

    return (hi_bool)flag;
}

hi_bool hal_ctrl_get_scdt(hi_drv_hdmirx_port port)
{
    hi_u32 flag;

    flag = hdmirx_hal_reg_read_fld_align(port, REG_RX_PWD_STATUS, PWD_STAT_DPHY_SCDT);

    return (hi_bool)flag;
}

hi_void hal_ctrl_set_aon_reset(hi_drv_hdmirx_port port, hdmirx_ctrl_aon_reset_type type)
{
    hi_u32 reg_offset;
    hi_u32 first_offset = RX_AON_ARST_REQ;

    if (type <= HDMIRX_AON_DET_ARST) {
        reg_offset = first_offset << type;
        hdmirx_hal_aon_reg_write_fld_align(port, REG_RX_AON_RST, reg_offset, HI_TRUE);
        udelay(1); /* delay 1 us */
        hdmirx_hal_aon_reg_write_fld_align(port, REG_RX_AON_RST, reg_offset, HI_FALSE);
    }
}

hi_void hal_ctrl_set_aon_reset_en(hi_drv_hdmirx_port port, hdmirx_ctrl_aon_reset_type type, hi_bool en)
{
    hi_u32 reg_offset;
    hi_u32 first_offset = RX_AON_ARST_REQ;

    if (type <= HDMIRX_AON_DET_ARST) {
        reg_offset = first_offset << type;
        hdmirx_hal_aon_reg_write_fld_align(port, REG_RX_AON_RST, reg_offset, en);
    }
}

hi_void hal_ctrl_set_pwd_reset(hi_drv_hdmirx_port port, hdmirx_ctrl_pwd_reset_type type)
{
    hi_u32 reg_offset;
    hi_u32 first_offset = HDMIRX_PWD_SRST_REQ;

    if (type <= HDMIRX_PWD_HDCP2X_MCU_SRST) {
        if (type == HDMIRX_PWD_SRST) {
            reg_offset = first_offset;
        } else if (type <= HDMIRX_PWD_DAMIX_SRST) {
            reg_offset = first_offset << (type + 3); /* 3 : 3 offset reserved bits */
        } else if (type <= HDMIRX_PWD_SCDC_SRST) {
            reg_offset = first_offset << (type + 4); /* 4 : 4 offset reserved bits */
        } else if (type <= HDMIRX_PWD_DEPACK_SRST) {
            reg_offset = first_offset << (type + 7); /* 7 : 7 offset reserved bits */
        } else if (type <= HDMIRX_PWD_HDCP2X_MCU_SRST) {
            reg_offset = first_offset << (type + 10); /* 10 : 10 offset reserved bits */
        }
        hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_SRST, reg_offset, HI_TRUE);
        udelay(1); /* delay 1 us */
        hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_SRST, reg_offset, HI_FALSE);
    }
}

hi_void hal_ctrl_set_pwd_reset_en(hi_drv_hdmirx_port port, hdmirx_ctrl_pwd_reset_type type, hi_bool en)
{
    hi_u32 reg_offset;
    hi_u32 first_offset = HDMIRX_PWD_SRST_REQ;

    if (type <= HDMIRX_PWD_HDCP2X_MCU_SRST) {
        if (type == HDMIRX_PWD_SRST) {
            reg_offset = first_offset;
        } else if (type <= HDMIRX_PWD_DAMIX_SRST) {
            reg_offset = first_offset << (type + 3); /* 3 : 3 offset reserved bits */
        } else if (type <= HDMIRX_PWD_SCDC_SRST) {
            reg_offset = first_offset << (type + 4); /* 4 : 4 offset reserved bits */
        } else if (type <= HDMIRX_PWD_DEPACK_SRST) {
            reg_offset = first_offset << (type + 7); /* 7 : 7 offset reserved bits */
        } else if (type <= HDMIRX_PWD_HDCP2X_MCU_SRST) {
            reg_offset = first_offset << (type + 10); /* 10 : 10 offset reserved bits */
        }
        hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_SRST, reg_offset, en);
    }
}

hi_void hal_ctrl_set_auto_reset_en(hi_drv_hdmirx_port port, hdmirx_ctrl_auto_reset_type auto_rst, hi_bool en)
{
    switch (auto_rst) {
        case HDMIRX_AUTORST_DPHY:
        case HDMIRX_AUTORST_DPHY_LANE:
        case HDMIRX_AUTORST_AUDIO_FIFO:
            hdmirx_hal_set_pwd_auto_reset_en(port, auto_rst, en);
            break;
        case HDMIRX_AUTORST_DDC_TOUT:
        case HDMIRX_AUTORST_DDC_PWR:
        case HDMIRX_AUTORST_DDC_HPD:
            hdmirx_hal_set_aon_auto_reset_en(port, auto_rst, en);
            break;
        case HDMIRX_AUTORST_ALL:
            hdmirx_hal_set_pwd_auto_reset_en(port, auto_rst, en);
            hdmirx_hal_set_aon_auto_reset_en(port, auto_rst, en);
            break;
        default:
            break;
    }
}

hi_void hal_ctrl_set_ddc_slave_en(hi_drv_hdmirx_port port, hi_bool en)
{
    hdmirx_hal_aon_reg_write_fld_align(port, REG_DDC_SLV_CTL, DDC_SLV_EN, en);
}

hi_void hal_ctrl_set_ddc_scdc_en(hi_drv_hdmirx_port port, hi_bool en)
{
    hdmirx_hal_aon_reg_write_fld_align(port, REG_DDC_DEV_CTL, DDC_SCDC_EN, en);
}

hi_void hal_ctrl_set_ddc_hdcp_en(hi_drv_hdmirx_port port, hi_bool en)
{
    hdmirx_hal_aon_reg_write_fld_align(port, REG_DDC_DEV_CTL, DDC_HDCP_EN, en);
}

hi_void hal_ctrl_set_ddc_edid_en(hi_drv_hdmirx_port port, hi_bool en)
{
    hdmirx_hal_aon_reg_write_fld_align(port, REG_DDC_DEV_CTL, DDC_EDID_EN, en);
}

hi_bool hal_ctrl_get_tmds_ratio(hi_drv_hdmirx_port port)
{
    hi_u32 flag;

    flag = hdmirx_hal_reg_read_fld_align(port, REG_TMDS_CONFIGURATION, TMDS_BIT_CLOCK_RATIO);

    return (hi_bool)flag;
}

hi_bool hal_ctrl_get_scrbl_en(hi_drv_hdmirx_port port)
{
    hi_u32 en, status;
    hi_bool flag = HI_FALSE;

    en = hdmirx_hal_reg_read_fld_align(port, REG_TMDS_CONFIGURATION, SCRAMBLING_ENABLE);
    status = hdmirx_hal_reg_read_fld_align(port, REG_TMDS_SCRAMBLER_STATUS, SCRAMBLER_STATUS);
    if (en == HI_TRUE && status == HI_TRUE) {
        flag = HI_TRUE;
    }

    return flag;
}

hi_void hal_ctrl_overwrite_scramble(hi_drv_hdmirx_port port, hi_bool scdc_en)
{
    hdmirx_hal_reg_write_fld_align(port, REG_TMDS_CONFIGURATION, SCRAMBLING_ENABLE, scdc_en);
}

hi_void hal_ctrl_overwrite_hdmi2_mode(hi_drv_hdmirx_port port, hi_bool hdmi_mode)
{
    hdmirx_hal_reg_write_fld_align(port, REG_TMDS_CONFIGURATION, TMDS_BIT_CLOCK_RATIO, hdmi_mode);
}

hi_void hal_ctrl_load_edid(hi_drv_hdmirx_port port, hi_drv_hdmirx_edid *edid)
{
    hi_u32 len;
    hi_u32 offset;
    hi_u32 first_byte;
    hi_u32 second_byte;
    hi_u32 third_byte;
    hi_u32 forth_byte;
    hi_u32 reg_addr;
    hi_u32 reg_value;
    const hi_u32 first_reg_addr = 0x2800; /* 0x2800 : 1st packet offset addrest */

    len = edid->edid_length / 4; /* 4 : 4 bytes register */
    for (offset = 0; offset < len; offset++) {
        first_byte = edid->edid_data[offset * 4]; /* 4 : register first_byte */
        second_byte = edid->edid_data[offset * 4 + 1]; /* 4/1 : register second_byte */
        third_byte = edid->edid_data[offset * 4 + 2]; /* 4/2 : register third_byte */
        forth_byte = edid->edid_data[offset * 4 + 3]; /* 4/3 : register forth_byte */
        reg_addr = first_reg_addr + (offset * 4); /* 4 : register offset 4 */
        /* 8/16/24 : shift left 8/16/24 bit */
        reg_value = first_byte | (second_byte << 8) | (third_byte << 16) | (forth_byte << 24);
        hdmirx_hal_aon_reg_write(port, reg_addr, reg_value);
    }
}

hi_u32 hal_ctrl_get_packet_content(hi_drv_hdmirx_port port, hdmirx_ctrl_packet_type type, hi_u8 *data, hi_u32 max_len)
{
    hi_u32 div_val = 0;
    hi_u32 mod_val = 0;
    hi_u32 first_byte = 0;
    hi_u32 second_byte = 0;
    hi_u32 third_byte = 0;
    hi_u32 forth_byte = 0;
    hi_u32 offset = 0;
    hi_u32 reg_addr = 0;
    hi_u32 len = 31; /* 31 : Maximum length of a packet */
    hi_u32 value[8] = {0}; /* 8 register */
    hi_u32 first_reg_addr = REG_GCPRX_WORD0;

    if (len > max_len) {
        len = max_len;
    }

    if (len != 0 && type < HDMIRX_PACKET_MAX) {
        div_val = len / 4; /* 4 bytes 1 register */
        mod_val = len % 4; /* 4 bytes 1 register */
        reg_addr = first_reg_addr + (type * 32); /* 32 offset registers */
        hdmirx_hal_reg_read_block(port, reg_addr, value, 8); /* 8 register */
        for (offset = 0; offset <= div_val; offset++) {
            if (offset < div_val || ((mod_val >= 1) && (offset == div_val))) {
                first_byte = value[offset] & 0xFF;
                data[offset * 4 + 0] = first_byte; /* 4/0 Register offset first byte */
            }
            if (offset < div_val || ((mod_val >= 2) && (offset == div_val))) { /* 2 : register second byte */
                second_byte = (value[offset] & 0xFF00) >> 8; /* Shift right 8 bit */
                data[offset * 4 + 1] = second_byte; /* 4/1 Register offset second byte */
            }
            if (offset < div_val || ((mod_val == 3) && (offset == div_val))) { /* 3 : register third byte */
                third_byte = (value[offset] & 0xFF0000) >> 16; /* Shift right 16 bit */
                data[offset * 4 + 2] = third_byte; /* 4/2 Register offset third byte */
            }
            if (offset < div_val) {
                forth_byte = (value[offset] & 0xFF000000) >> 24; /* Shift right 24 bit */
                data[offset * 4 + 3] = forth_byte; /* 4/3 Register offset fourth byte */
            }
        }
    }

    return len;
}

hi_bool hal_ctrl_check_avi_color_chg(hi_drv_hdmirx_port port, hdmirx_color_space color_space)
{
    hi_u32 data[2] = {0}; /* 2 register */
    hi_u8  color_space_indicator;
    hi_bool flag = HI_FALSE;

    hdmirx_hal_reg_read_block(port, REG_AVIRX_WORD0, data, 2); /* read 2 registers */
    color_space_indicator = (data[1] & BIT7_0) >> 5; /* get the first byte from data[1], then Shift right 5 bit */
    if (color_space_indicator != color_space) {
        flag = HI_TRUE;
    }

    return flag;
}

hi_u32 hal_ctrl_get_packet_type(hi_drv_hdmirx_port port, hi_u32 addr)
{
    hi_u32 value;
    hi_u32 type = 0;

    value = hdmirx_hal_reg_read(port, addr);
    value &= BIT7_0;

    switch (value) {
        case HDMIRX_AVI:
        case HDMIRX_SPD:
        case HDMIRX_AUDIO:
        case HDMIRX_MPEG:
        case HDMIRX_HDR10:
        case HDMIRX_ISRC1:
        case HDMIRX_ISRC2:
        case HDMIRX_ACP:
        case HDMIRX_GC:
        case HDMIRX_GBD:
        case HDMIRX_VSIF:
            type = value;
            break;
        default:
            break;
    }

    return type;
}

hi_void hal_ctrl_set_mute_en(hi_drv_hdmirx_port port, hdmirx_ctrl_mute_type type, hi_bool en)
{
    switch (type) {
        case HDMIRX_MUTE_AUD:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_AUD_HOLD, REG_AUDIO_MUTE, en);
            break;
        case HDMIRX_MUTE_VDO:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_MUTE_CTRL, CFG_MUTE_EN, en);
            break;
        case HDMIRX_MUTE_ALL:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_AUD_HOLD, REG_AUDIO_MUTE, en);
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_MUTE_CTRL, CFG_MUTE_EN, en);
            break;
        default:
            break;
    }
}

hi_bool hal_ctrl_get_mute_en(hi_drv_hdmirx_port port, hdmirx_ctrl_mute_type type)
{
    hi_u32 flag = 0;

    if (type == HDMIRX_MUTE_AUD) {
        flag = hdmirx_hal_reg_read_fld_align(port, REG_CFG_AUD_HOLD, REG_AUDIO_MUTE);
    } else if (type == HDMIRX_MUTE_VDO) {
        flag = hdmirx_hal_reg_read_fld_align(port, REG_CFG_MUTE_CTRL, CFG_MUTE_EN);
    }

    return (hi_bool)flag;
}

hi_void hal_ctrl_set_pwd_hw_mute(hi_drv_hdmirx_port port, hdmirx_ctrl_hw_mute_type type,
    hdmirx_ctrl_event_type event_type, hi_bool en)
{
    switch (type) {
        case HDMIRX_HW_MUTE_SYS_EVENT_VIDEO:
            hdmirx_hal_set_sys_event_for_video_mask_en(port, event_type.sys_event, en);
            break;
        case HDMIRX_HW_MUTE_SYS_EVENT_AUDID:
            hdmirx_hal_set_sys_event_for_audio_mask_en(port, event_type.sys_event, en);
            break;
        case HDMIRX_HW_MUTE_SYS_EVENT_AUDID_UNMUTE:
            hdmirx_hal_set_sys_event_for_audio_unmute_mask_en(port, event_type.sys_event, en);
            break;
        case HDMIRX_HW_MUTE_SYS_EVENT_DECODE:
            hdmirx_hal_set_sys_event_for_decoder_mask_en(port, event_type.sys_event, en);
            break;
        case HDMIRX_HW_MUTE_SYS_EVENT_DEPACK:
            hdmirx_hal_set_sys_event_for_depack_mask_en(port, event_type.sys_event, en);
            break;
        case HDMIRX_HW_MUTE_VID_EVENT_VIDEO:
            hdmirx_hal_set_video_event_mask_en(port, event_type.video_event, en);
            break;
        case HDMIRX_HW_MUTE_AUD_EVENT_AUDID:
            hdmirx_hal_set_audio_event_mask_en(port, event_type.audio_event, en);
            break;
        case HDMIRX_HW_MUTE_TMDS_EVENT_VIDEO:
            hdmirx_hal_set_tmds_event_for_video_mask_en(port, event_type.tmds_event, en);
            break;
        case HDMIRX_HW_MUTE_TMDS_EVENT_AUDID:
            hdmirx_hal_set_tmds_event_for_audio_mask_en(port, event_type.tmds_event, en);
            break;
        case HDMIRX_HW_MUTE_TMDS_EVENT_AUDID_UNMUTE:
            hdmirx_hal_set_tmds_event_for_audio_unmute_mask_en(port, event_type.tmds_event, en);
            break;
        case HDMIRX_HW_MUTE_DPK_EVENT_VIDEO:
            hdmirx_hal_set_depack_event_for_video_mask_en(port, event_type.depack_event, en);
            break;
        case HDMIRX_HW_MUTE_DPK_EVENT_AUDIO:
            hdmirx_hal_set_depack_event_for_audio_mask_en(port, event_type.depack_event, en);
            break;
        case HDMIRX_HW_MUTE_DPK_EVENT_AUDIO_UNMUTE:
            hdmirx_hal_set_depack_event_for_audio_unmute_mask_en(port, event_type.depack_event, en);
            break;
        default:
            break;
    }
}

hi_void hal_ctrl_set_sys_mute(hi_drv_hdmirx_port port, hi_bool en)
{
    if (en == HI_TRUE) {
        hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_CTRL_CMD, CFG_SYS_SET_MUTE, HI_TRUE);
    } else {
        hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_CTRL_CMD, CFG_SYS_CLR_MUTE, HI_TRUE);
    }
}

hi_bool hal_ctrl_is_pwd_clk_stable(hi_drv_hdmirx_port port, hdmirx_ctrl_clock_type type)
{
    hi_u32 exist = 0;
    hi_u32 flag = 0;

    switch (type) {
        case HDMIRX_CLOCK_TMDS:
        case HDMIRX_CLOCK_FRL:
            exist = hdmirx_hal_reg_read_fld_align(port, REG_RX_PWD_CLK_DET_TMDS_STAT, DET_CLK0_FEAT_EXIST);
            flag = hdmirx_hal_reg_read_fld_align(port, REG_RX_PWD_CLK_DET_TMDS_STAT, DET_CLK0_STAT);
            break;
        case HDMIRX_CLOCK_PIXEL:
            exist = hdmirx_hal_reg_read_fld_align(port, REG_RX_PWD_CLK_DET3_STAT, DET_CLK3_FEAT_EXIST);
            flag = hdmirx_hal_reg_read_fld_align(port, REG_RX_PWD_CLK_DET3_STAT, DET_CLK3_STAT);
            break;
        case HDMIRX_CLOCK_AUDIO:
            exist = hdmirx_hal_reg_read_fld_align(port, REG_RX_PWD_CLK_DET_AUD_DACR_STAT, DET_CLK2_FEAT_EXIST);
            flag = hdmirx_hal_reg_read_fld_align(port, REG_RX_PWD_CLK_DET_AUD_DACR_STAT, DET_CLK2_STAT);
            break;
        case HDMIRX_CLOCK_TMDS_D4:
        case HDMIRX_CLOCK_FRL_D4:
            exist = hdmirx_hal_reg_read_fld_align(port, REG_RX_PWD_CLK_DET_TMDS_D4_STAT, DET_CLK1_FEAT_EXIST);
            flag = hdmirx_hal_reg_read_fld_align(port, REG_RX_PWD_CLK_DET_TMDS_D4_STAT, DET_CLK1_STAT);
            break;
        default:
            break;
    }

    if (exist == HI_FALSE) {
        flag = HI_FALSE;
    }

    return (hi_bool)flag;
}

hi_u32 hal_ctrl_get_pwd_clock(hi_drv_hdmirx_port port, hdmirx_ctrl_clock_type type)
{
    hi_u32 clk_cal_base;
    hi_u32 exist = 0;
    hi_u32 freq = 0;

    switch (type) {
        case HDMIRX_CLOCK_TMDS:
        case HDMIRX_CLOCK_FRL:
            exist = hdmirx_hal_reg_read_fld_align(port, REG_RX_PWD_CLK_DET_TMDS_STAT, DET_CLK0_FEAT_EXIST);
            freq = hdmirx_hal_reg_read_fld_align(port, REG_RX_PWD_CLK_DET_TMDS_STAT, DET_CLK0_FREQ);
            clk_cal_base = 600000; /* 120000 : base value for 600000 (MHz) */
            break;
        case HDMIRX_CLOCK_PIXEL:
            exist = hdmirx_hal_reg_read_fld_align(port, REG_RX_PWD_CLK_DET3_STAT, DET_CLK3_FEAT_EXIST);
            freq = hdmirx_hal_reg_read_fld_align(port, REG_RX_PWD_CLK_DET3_STAT, DET_CLK3_FREQ);
            clk_cal_base = 7200000; /* 7200000 : base value for 24*75000*4 (MHz) */
            break;
        case HDMIRX_CLOCK_AUDIO:
            exist = hdmirx_hal_reg_read_fld_align(port, REG_RX_PWD_CLK_DET_AUD_DACR_STAT, DET_CLK2_FEAT_EXIST);
            freq = hdmirx_hal_reg_read_fld_align(port, REG_RX_PWD_CLK_DET_AUD_DACR_STAT, DET_CLK2_FREQ);
            clk_cal_base = 120000; /* 120000 : base value for 24*5000 (MHz) */
            break;
        case HDMIRX_CLOCK_TMDS_D4:
        case HDMIRX_CLOCK_FRL_D4:
            exist = hdmirx_hal_reg_read_fld_align(port, REG_RX_PWD_CLK_DET_TMDS_D4_STAT, DET_CLK1_FEAT_EXIST);
            freq = hdmirx_hal_reg_read_fld_align(port, REG_RX_PWD_CLK_DET_TMDS_D4_STAT, DET_CLK1_FREQ);
            clk_cal_base = 1440000; /* 1440000 : base value for 360000*4 (MHz) */
            break;
        default:
            break;
    }

    if (exist == HI_TRUE) {
        if (freq != 0) {
            freq = (hi_u32)((hi_u64)clk_cal_base * 1000 / freq); /* 1000 : freq(kHz) */
        }
    }

    return freq;
}

hi_bool hal_ctrl_get_av_mute(hi_drv_hdmirx_port port)
{
    hi_u32 flag;

    flag = hdmirx_hal_reg_read_fld_align(port, REG_RX_AUDP_STAT, HDMI_MUTE);

    return (hi_bool)flag;
}

hi_bool hal_ctrl_get_dsc_en(hi_drv_hdmirx_port port)
{
    hi_u32 flag;

    flag = hdmirx_hal_reg_read_fld_align(port, REG_DSC_STATUS, DSC_EN);

    return (hi_bool)flag;
}

hi_void hal_ctrl_set_pp_phase(hi_drv_hdmirx_port port, hi_u32 value)
{
    hdmirx_hal_reg_write_fld_align(port, REG_GCP_CONFIG, CFG_PP_SW, value);
    hdmirx_hal_reg_write_fld_align(port, REG_GCP_CONFIG, CFG_PHASE_OVR, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_GCP_CONFIG, CFG_PHASE_UPDATE, HI_TRUE);
}

hi_void hal_ctrl_set_emp_en(hi_drv_hdmirx_port port, hdmirx_ctrl_emp_type type)
{
    hi_u32 reg_offset;
    hi_u32 first_offfset = CFG_VI_UNREC_EMP_EN;

    if (type == HDMIRX_ALL_EMP_VI) {
        hdmirx_hal_reg_write_fld_align(port, REG_EMP_CFG_EN, CFG_GEN_EMP_EN, HI_TRUE);
        hdmirx_hal_reg_write_fld_align(port, REG_EMP_CFG_EN, CFG_VS0_EMP_EN, HI_TRUE);
        hdmirx_hal_reg_write_fld_align(port, REG_EMP_CFG_EN, CFG_VS1_EMP_EN, HI_TRUE);
        hdmirx_hal_reg_write_fld_align(port, REG_EMP_CFG_EN, CFG_FVAVRR_EMP_EN, HI_TRUE);
        hdmirx_hal_reg_write_fld_align(port, REG_EMP_CFG_EN, CFG_DSC_EMP_EN, HI_TRUE);
        hdmirx_hal_reg_write_fld_align(port, REG_EMP_CFG_EN, CFG_HDR_EMP_EN, HI_TRUE);
    } else if (type == HDMIRX_MLDS_EMP_BYP) {
        hdmirx_hal_reg_write_fld_align(port, REG_EMP_CFG_EN, CFG_MLDS_EMP_BYP, HI_TRUE);
    } else if (type != HDMIRX_EMP_MAX) {
        reg_offset = first_offfset << type;
        hdmirx_hal_reg_write_fld_align(port, REG_EMP_CFG_EN, reg_offset, HI_TRUE);
    }
}

hi_void hal_ctrl_set_emp_test_en(hi_drv_hdmirx_port port, hdmirx_ctrl_emp_type type)
{
    /* to be added in the future */
}

hi_void hal_ctrl_get_dsc_emp(hi_drv_hdmirx_port port, hi_u8 *emp_data, hi_u32 len)
{
    /* to be added in the future */
}

hi_void hal_ctrl_get_vrr_fva_emp(hi_drv_hdmirx_port port, hi_u8 *emp_data, hi_u32 len)
{
    hi_u32 div_val = 0;
    hi_u32 mod_val = 0;
    hi_u32 first_byte = 0;
    hi_u32 second_byte = 0;
    hi_u32 third_byte = 0;
    hi_u32 forth_byte = 0;
    hi_u32 offset = 0;
    const hi_u32 max_len = 31; /* 31 : Maximum length of a packet */
    hi_u32 value[8] = {0}; /* 8 register */

    if (len > max_len) {
        len = max_len;
    }

    if (len != 0) {
        div_val = len / 4; /* 4 bytes 1 register */
        mod_val = len % 4; /* 4 bytes 1 register */
        hdmirx_hal_reg_read_block(port, REG_FVAVRRRX_WORD0, value, 8); /* 8 register */
        for (offset = 0; offset <= div_val; offset++) {
            if (offset < div_val || ((mod_val >= 1) && (offset == div_val))) {
                first_byte = value[offset] & 0xFF;
                emp_data[offset * 4 + 0] = first_byte; /* 4/0 Register offset first byte */
            }
            if (offset < div_val || ((mod_val >= 2) && (offset == div_val))) { /* 2 : register second byte */
                second_byte = (value[offset] & 0xFF00) >> 8; /* Shift right 8 bit */
                emp_data[offset * 4 + 1] = second_byte; /* 4/1 Register offset second byte */
            }
            if (offset < div_val || ((mod_val == 3) && (offset == div_val))) { /* 3 : register third byte */
                third_byte = (value[offset] & 0xFF0000) >> 16; /* Shift right 16 bit */
                emp_data[offset * 4 + 2] = third_byte; /* 4/2 Register offset third byte */
            }
            if (offset < div_val) {
                forth_byte = (value[offset] & 0xFF000000) >> 24; /* Shift right 24 bit */
                emp_data[offset * 4 + 3] = forth_byte; /* 4/3 Register offset fourth byte */
            }
        }
    }
}

hi_void hal_ctrl_set_fapa_end(hi_drv_hdmirx_port port, hi_u32 vic)
{
    /* to be added in the future */
}

